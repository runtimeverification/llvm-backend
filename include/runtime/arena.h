#ifndef ARENA_H
#define ARENA_H

#include <cstddef>
#include <sys/types.h>

#include "runtime/alloc.h"

extern "C" {


// An arena can be used to allocate objects that can then be deallocated all at
// once.
class arena {
public:
  arena(char id)
      : allocation_semispace_id(id) { }

  // Allocates the requested number of bytes as a contiguous region and returns a
  // pointer to the first allocated byte.
  // If called with requested size greater than the maximun single allocation
  // size, the space is allocated in a general (not garbage collected pool).
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
  size_t arena_size() const;

  // Clears the current allocation space by setting its start back to its first
  // block. It is used during garbage collection to effectively collect all of the
  // arena.
  void arena_clear() { allocation_ptr = arena_start_ptr(); }

  // Resizes the last allocation as long as the resize does not require a new
  // block allocation.
  // Returns the address of the byte following the last newlly allocated byte when
  // the resize succeeds, returns 0 otherwise.
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
  struct memory_block_header {
    char *next_block;
    char semispace;
  };

  void *slow_alloc(size_t requested);
  void *megabyte_malloc();
  
  void fresh_block();
  static memory_block_header *mem_block_header(void *ptr);

  // helper function for `kore_arena_alloc`. Do not call directly.
  void *do_alloc_slow(size_t requested);

  char *current_addr_ptr = nullptr;  // pointer to start of current address space
  char *collection_addr_ptr = nullptr;  // pointer to start of collection address space
  char *current_tripwire = nullptr;  // allocating past this triggers slow allocation
  char *collection_tripwire = nullptr;  // tripwire for collection semispace
  char *allocation_ptr  = nullptr;  // next available location in current semispace
  
  char allocation_semispace_id;  // id of current semispace
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
  if (allocation_ptr + requested >= current_tripwire) {
    //
    //	We got close to or past the last location accessed in this address range so far,
    //	thus we should consider a garbage collection so we don't just keep accessing
    //	fresh memory without bound.
    //	This condition holds trivially if current_tripwire == nullptr since
    //	all pointers are >= nullptr, and this indicates that no address range has been
    //	reserved for this semispace so far.
    //
    return slow_alloc(requested);
  }
  void *result = allocation_ptr;
  allocation_ptr += requested;
  MEM_LOG("Allocation at %p (size %zd), next alloc at %p\n", result, requested, block);
  return result;
}
}
#endif // ARENA_H
