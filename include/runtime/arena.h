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
  arena(char id) : allocation_semispace_id(id) {}

private:
  char *first_block;
  char *block;
  char *block_start;
  char *block_end;
  char *first_collection_block;
  size_t num_blocks;
  size_t num_collection_blocks;
  char allocation_semispace_id;
  //
  //	First step, make all the functions that look like they should be member
  //	functions into friends so that they can access the data members.
  //
  friend void *kore_arena_alloc(arena *arena, size_t requested);

  friend void arena_reset(arena *arena);
  friend char get_arena_allocation_semispace_id(const arena *arena);
  friend char get_arena_collection_semispace_id(const arena *arena);
  friend void fresh_block(arena *arena);

  friend void fresh_block(arena *arena);
  friend void *do_alloc_slow(size_t requested, arena *arena);
  friend void *arena_resize_last_alloc(arena *arena, ssize_t increase);
  friend void arena_swap_and_clear(arena *arena);
  friend void arena_clear(arena *arena);
  friend char * arena_start_ptr(const arena *arena);
  friend char **arena_end_ptr(arena *arena);
  friend size_t arena_size(const arena *arena);
  //
  //	These things probably shouldn't be friends but are needed to compile.
  //
  friend bool youngspace_almost_full(size_t threshold);
};

using memory_block_header = struct {
  char *next_block;
  char *next_superblock;
  char semispace;
};

// Macro to define a new arena with the given ID. Supports IDs ranging from 0 to
// 127.
#define REGISTER_ARENA(name, id)                                               \
  static thread_local arena name(id)

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

// Resets the given arena.
void arena_reset(arena *);

// Returns the given arena's current allocation semispace ID.
// Each arena has 2 semispace IDs one equal to the arena ID and the other equal
// to the 1's complement of the arena ID. At any time one of these semispaces
// is used for allocation and the other is used for collection.
char get_arena_allocation_semispace_id(const arena *);

// Returns the given arena's current collection semispace ID.
// See above for details.
char get_arena_collection_semispace_id(const arena *);

// Returns the ID of the semispace where the given address was allocated.
// The behavior is undefined if called with an address that has not been
// allocated within an arena.
char get_arena_semispace_id_of_object(void *);

// helper function for `kore_arena_alloc`. Do not call directly.
void *do_alloc_slow(size_t, arena *);

// Allocates the requested number of bytes as a contiguous region and returns a
// pointer to the first allocated byte.
// If called with requested size greater than the maximun single allocation
// size, the space is allocated in a general (not garbage collected pool).
inline void *kore_arena_alloc(arena *arena, size_t requested) {
  if (arena->block + requested > arena->block_end) {
    return do_alloc_slow(requested, arena);
  }
  void *result = arena->block;
  arena->block += requested;
  MEM_LOG(
      "Allocation at %p (size %zd), next alloc at %p (if it fits)\n", result,
      requested, arena->block);
  return result;
}

// Resizes the last allocation as long as the resize does not require a new
// block allocation.
// Returns the address of the byte following the last newlly allocated byte when
// the resize succeeds, returns 0 otherwise.
void *arena_resize_last_alloc(arena *, ssize_t);

// Exchanges the current allocation and collection semispaces and clears the new
// current allocation semispace by setting its start back to its first block.
// It is used before garbage collection.
void arena_swap_and_clear(arena *);

// Clears the current allocation space by setting its start back to its first
// block. It is used during garbage collection to effectively collect all of the
// arena.
void arena_clear(arena *);

// Returns the address of the first byte that belongs in the given arena.
// Returns 0 if nothing has been allocated ever in that arena.
char *arena_start_ptr(const arena *);

// Returns a pointer to a location holding the address of last allocated
// byte in the given arena plus 1.
// This address is 0 if nothing has been allocated ever in that arena.
char **arena_end_ptr(arena *);

// Given a starting pointer to an address allocated in an arena and a size in
// bytes, this function returns a pointer to an address allocated in the
// same arena after size bytes from the starting pointer.
//
// 1st argument: the starting pointer
// 2nd argument: the size in bytes to add to the starting pointer
// 3rd argument: the address of last allocated byte in the arena plus 1
// Return value: the address allocated in the arena after size bytes from the
//               starting pointer, or 0 if this is equal to the 3rd argument.
char *move_ptr(char *, size_t, char const *);

// Given two pointers to objects allocated in the same arena, return the number
// of bytes they are separated by within the virtual block of memory represented
// by the blocks of that arena. This difference will include blocks containing
// sentinel bytes. Undefined behavior will result if the pointers belong to
// different arenas.
ssize_t ptr_diff(char *, char *);

// return the total number of allocatable bytes currently in the arena in its
// active semispace.
size_t arena_size(const arena *);

// Deallocates all the memory allocated for registered arenas.
void free_all_memory(void);
}

#endif // ARENA_H
