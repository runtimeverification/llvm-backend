#ifndef ARENA_H
#define ARENA_H

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <sys/types.h>
#include <utility>

#include "runtime/alloc.h"

extern "C" {

size_t const HYPERBLOCK_SIZE = (size_t)BLOCK_SIZE * 1024 * 1024;

// After a garbage collect we change the tripwire to the amount of non-garbage times
// this factor, so we do a decent amount of allocations between collections even
// when there is very little garbage
size_t const EXPAND_FACTOR = 2;

// We don't consider collecting garbage until at least this amount of space has
// been allocated, to avoid collections near startup when there is little garbage.
size_t const MIN_SPACE = 1024 * 1024;

// An arena can be used to allocate objects that can then be deallocated all at
// once.
class arena {
public:
  arena(char id, bool trigger_collection)
      : allocation_semispace_id(id)
      , trigger_collection(trigger_collection) {
    initialize_semispace();
  }

  char *evacuate(char *scan_ptr);

  // Allocates the requested number of bytes as a contiguous region and returns a
  // pointer to the first allocated byte.
  void *kore_arena_alloc(size_t requested);

  // Returns the address of the first byte that belongs in the given arena.
  // Returns nullptr if nothing has been allocated ever in that arena.
  char *start_ptr() const { return current_addr_ptr; }

  // Returns a pointer to a location holding the address of last allocated
  // byte in the given arena plus 1.
  // This address is nullptr if nothing has been allocated ever in that arena.
  char *end_ptr() { return allocation_ptr; }

  // Clears the current allocation space by setting its start back to its first
  // block. It is used during garbage collection to effectively collect all of the
  // arena.
  void arena_clear() { allocation_ptr = current_addr_ptr; }

  // Resizes the last allocation.
  // Returns the address of the byte following the last newly allocated byte.
  void *arena_resize_last_alloc(ssize_t increase) {
    return (allocation_ptr += increase);
  }

  // Returns the given arena's current collection semispace ID.
  // Each arena has 2 semispace IDs one equal to the arena ID and the other equal
  // to the 1's complement of the arena ID. At any time one of these semispaces
  // is used for allocation and the other is used for collection.
  char get_arena_collection_semispace_id() const {
    return ~allocation_semispace_id;
  }

  // Exchanges the current allocation and collection semispaces and clears the new
  // current allocation semispace by setting its start back to its first block.
  // It is used before garbage collection.
  void arena_swap_and_clear();

  // Decide how much space to use in arena before setting the flag for a collection.
  // If an arena is going to request collections, updating this at the end of a
  // collection is mandatory.
  void update_tripwire() {
    size_t space = EXPAND_FACTOR * (allocation_ptr - current_addr_ptr);
    size_t min_space = old_tripwire - current_addr_ptr;
    tripwire = current_addr_ptr + ((space < min_space) ? min_space : space);
  }

  // Given two pointers to objects allocated in the same arena, return the number
  // of bytes they are apart. Undefined behavior will result if the pointers
  // don't belong to the same arena
  static ssize_t ptr_diff(char *ptr1, char *ptr2) { return ptr1 - ptr2; }

  // Given a starting pointer to an address allocated in an arena and a size in
  // bytes, this function returns a pointer to an address allocated in the
  // same arena after size bytes from the starting pointer.
  //
  // 1st argument: the starting pointer
  // 2nd argument: the size in bytes to add to the starting pointer
  // 3rd argument: the address of last allocated byte in the arena plus 1
  // Return value: starting pointer + size unless this points to unallocated space
  //               in which case nullptr is returned
  static char *move_ptr(char *ptr, size_t size, char const *end_ptr) {
    char *next_ptr = ptr + size;
    return (next_ptr == end_ptr) ? nullptr : next_ptr;
  }

  // Returns the ID of the semispace where the given address was allocated.
  // The behavior is undefined if called with an address that has not been
  // allocated within an arena.
  static char get_arena_semispace_id_of_object(void *ptr);

private:
  void initialize_semispace();
  //
  //	Current semispace where allocations are being made.
  //
  char *current_addr_ptr; // pointer to start of current address space
  char *allocation_ptr; // next available location in current semispace
  char *tripwire; // allocating past this sets flag for collection
  char *old_tripwire; // for monotonic expansion
  char allocation_semispace_id; // id of current semispace
  bool const trigger_collection; // request collections?
  //
  //	Semispace where allocations will be made during and after garbage collect.
  //
  char *collection_addr_ptr
      = nullptr; // pointer to start of collection address space
};

inline char arena::get_arena_semispace_id_of_object(void *ptr) {
  //
  //	We don't have to deal with the "1 past the end of block" case because
  //	a valid pointer will always point into our hyperblock - we will never return
  //	an allocation anywhere near the end of our hyperblock.
  //
  //	Set the low bits to 1 to get the address of the last byte in the hyperblock.
  //
  uintptr_t end_address
      = reinterpret_cast<uintptr_t>(ptr) | (HYPERBLOCK_SIZE - 1);
  return *reinterpret_cast<char *>(end_address);
}

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
    //
    //	We move the tripwire to 1 past the end of our hyperblock so that we have
    //	a well defined comparison that will always be false update_tripwire() is called.
    //
    old_tripwire = tripwire;
    tripwire = current_addr_ptr + HYPERBLOCK_SIZE;
  }
  void *result = allocation_ptr;
  allocation_ptr += requested;
  MEM_LOG(
      "Allocation at %p (size %zd), next alloc at %p\n", result, requested,
      block);
  return result;
}

inline void arena::arena_swap_and_clear() {
  std::swap(current_addr_ptr, collection_addr_ptr);
  allocation_semispace_id = ~allocation_semispace_id;
  if (current_addr_ptr == nullptr)
    initialize_semispace(); // not yet initialized
  else
    arena_clear();
}
}
#endif // ARENA_H
