#ifndef ARENA_H
#define ARENA_H

#include <cstddef>
#include <cstdint>
#include <utility>
#include <algorithm>
#include <sys/types.h>

#include "runtime/alloc.h"

extern "C" {

size_t const HYPERBLOCK_SIZE = (size_t)BLOCK_SIZE * 1024 * 1024;

// An arena can be used to allocate objects that can then be deallocated all at
// once.
class arena {
public:
  arena(char id) : allocation_semispace_id(id) { initialize_semispace(); }

  // Allocates the requested number of bytes as a contiguous region and returns a
  // pointer to the first allocated byte.
  void *kore_arena_alloc(size_t requested);

  // Returns the address of the first byte that belongs in the given arena.
  // Returns 0 if nothing has been allocated ever in that arena.
  char *arena_start_ptr() const { return current_addr_ptr ?  current_addr_ptr + sizeof(memory_block_header) : nullptr; }

  // Returns a pointer to a location holding the address of last allocated
  // byte in the given arena plus 1.
  // This address is 0 if nothing has been allocated ever in that arena.
  char **arena_end_ptr() { return &allocation_ptr; }


  // return the total number of allocatable bytes currently in the arena in its
  // active semispace.
  size_t arena_size() const { update_num_blocks(); return BLOCK_SIZE * std::max(num_blocks, num_collection_blocks); }

  // Clears the current allocation space by setting its start back to its first
  // block. It is used during garbage collection to effectively collect all of the
  // arena.
  void arena_clear();

  // Resizes the last allocation as long as the resize does not require a new
  // block allocation.
  // Returns the address of the byte following the last newlly allocated byte.
  void *arena_resize_last_alloc(ssize_t increase) { return (allocation_ptr += increase); }

  // Returns the given arena's current collection semispace ID.
  // Each arena has 2 semispace IDs one equal to the arena ID and the other equal
  // to the 1's complement of the arena ID. At any time one of these semispaces
  // is used for allocation and the other is used for collection.
  char get_arena_collection_semispace_id() const { return ~allocation_semispace_id; }

  // Exchanges the current allocation and collection semispaces and clears the new
  // current allocation semispace by setting its start back to its first block.
  // It is used before garbage collection.
  void arena_swap_and_clear();

  // Given two pointers to objects allocated in the same arena, return the number
  // of bytes they are separated by within the virtual block of memory represented
  // by the blocks of that arena. This difference will include blocks containing
  // sentinel bytes. Undefined behavior will result if the pointers belong to
  // different arenas.
  static ssize_t ptr_diff(char *ptr1, char *ptr2) { return ptr1 - ptr2; }

  // Given a starting pointer to an address allocated in an arena and a size in
  // bytes, this function returns a pointer to an address allocated in the
  // same arena after size bytes from the starting pointer.
  //
  // 1st argument: the starting pointer
  // 2nd argument: the size in bytes to add to the starting pointer
  // 3rd argument: the address of last allocated byte in the arena plus 1
  // Return value: the address allocated in the arena after size bytes from the
  //               starting pointer, or 0 if this is equal to the 3rd argument.
  static char *move_ptr(char *ptr, size_t size, char const *arena_end_ptr);
  
  // Returns the ID of the semispace where the given address was allocated.
  // The behavior is undefined if called with an address that has not been
  // allocated within an arena.
  static char get_arena_semispace_id_of_object(void *ptr);

private:
  union memory_block_header {
    //
    //	Currently the header just holds the semispace id. But we need it to be a
    //	multiple of sizeof(char*) for alignment purposes so we add a dummy char*.
    //
    char semispace;
    char *alignment_dummy;
  };

  //
  //	We update the number of 1MB blocks actually written to, only when we need this value,
  //	or before a garbage collection rather than trying to determine when we write to a fresh block.
  //
  void update_num_blocks() const {
    //
    //	Calculate how many 1M blocks of the current arena we used.
    //
    size_t num_used_blocks = (allocation_ptr - current_addr_ptr - 1) / BLOCK_SIZE + 1;
    if (num_used_blocks > num_blocks)
      num_blocks = num_used_blocks;
  }

  void initialize_semispace();
  
  static memory_block_header *mem_block_header(void *ptr) {
    uintptr_t address = reinterpret_cast<uintptr_t>(ptr);
    return reinterpret_cast<arena::memory_block_header *>((address - 1) & ~(HYPERBLOCK_SIZE - 1));
  }

  //
  //	Current semispace where allocations are being made.
  //
  char *current_addr_ptr;  // pointer to start of current address space
  char *allocation_ptr;  // next available location in current semispace
  char *tripwire;  // allocating past this triggers slow allocation
  mutable size_t num_blocks;  // notional number of BLOCK_SIZE blocks in current semispace
  char allocation_semispace_id;  // id of current semispace
  //
  //	Semispace where allocations will be made during and after garbage collect.
  //
  char *collection_addr_ptr = nullptr;  // pointer to start of collection address space
  size_t num_collection_blocks = 0;  // notional number of BLOCK_SIZE blocks in collection semispace
};

// Macro to define a new arena with the given ID. Supports IDs ranging from 0 to
// 127.
#define REGISTER_ARENA(name, id) static thread_local arena name(id)

#define MEM_BLOCK_START(ptr)                                                   \
  ((char *)(((uintptr_t)(ptr)-1) & ~(BLOCK_SIZE - 1)))

#ifdef __MACH__
//
//	thread_local disabled for Apple
//
extern bool time_for_collection;
#else
extern thread_local bool time_for_collection;
#endif

size_t get_gc_threshold();

inline void *arena::kore_arena_alloc(size_t requested) {
  if (allocation_ptr + requested >= tripwire) {
    //
    //	We got close to or past the last location accessed in this address range so far,
    //	depending on the requested size and tripwire setting. This triggers a garbage
    //	collect when allowed.
    //
    time_for_collection = true;
    tripwire = current_addr_ptr + HYPERBLOCK_SIZE;  // won't trigger again until arena swap
  }
  void *result = allocation_ptr;
  allocation_ptr += requested;
  MEM_LOG("Allocation at %p (size %zd), next alloc at %p\n", result, requested, block);
  return result;
}

inline void arena::arena_clear() {
  //
  //	We set the allocation pointer to the first available address.
  //
  allocation_ptr = arena_start_ptr();
  //
  //	If the number of blocks we've touched is >= threshold, we want to trigger
  //	a garbage collection if we get within 1 block of the end of this area.
  //	Otherwise we only want to generate a garbage collect if we allocate off the
  //	end of this area.
  //
  tripwire = current_addr_ptr + (num_blocks - (num_blocks >= get_gc_threshold())) * BLOCK_SIZE;
}

inline void arena::arena_swap_and_clear() {
  update_num_blocks();  // so we save the correct number of touched blocks
  std::swap(current_addr_ptr, collection_addr_ptr);
  std::swap(num_blocks, num_collection_blocks);
  allocation_semispace_id = ~allocation_semispace_id;
  if (current_addr_ptr == nullptr)
    {
      //
      //	The other semispace hasn't be initialized yet.
      //
      void initialize_semispace();
    }
  else
    arena_clear();
}

}
#endif // ARENA_H
