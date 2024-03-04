
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "runtime/alloc.h"
#include "runtime/arena.h"
#include "runtime/header.h"

size_t const BLOCK_SIZE = 1024 * 1024;

__attribute__((always_inline)) memory_block_header *
mem_block_header(void *ptr) {
  // NOLINTNEXTLINE(*-reinterpret-cast)
  return reinterpret_cast<memory_block_header *>(
      ((uintptr_t)(ptr)-1) & ~(BLOCK_SIZE - 1));
}

__attribute__((always_inline)) void arena_reset(struct arena *arena) {
  char id = arena->allocation_semispace_id;
  if (id < 0) {
    id = ~arena->allocation_semispace_id;
  }
  arena->first_block = nullptr;
  arena->block = nullptr;
  arena->block_start = nullptr;
  arena->block_end = nullptr;
  arena->first_collection_block = nullptr;
  arena->num_blocks = 0;
  arena->num_collection_blocks = 0;
  arena->allocation_semispace_id = id;
}

__attribute__((always_inline)) char
get_arena_allocation_semispace_id(const struct arena *arena) {
  return arena->allocation_semispace_id;
}

__attribute__((always_inline)) char
get_arena_collection_semispace_id(const struct arena *arena) {
  return ~arena->allocation_semispace_id;
}

__attribute__((always_inline)) char get_arena_semispace_id_of_object(void *ptr) {
  return mem_block_header(ptr)->semispace;
}

static void *first_superblock_ptr = nullptr;
static void *superblock_ptr = nullptr;
static char **next_superblock_ptr = nullptr;
static unsigned blocks_left = 0;

static void *megabyte_malloc() {
  if (blocks_left == 0) {
    blocks_left = 15;
    if (int result
        = posix_memalign(&superblock_ptr, BLOCK_SIZE, BLOCK_SIZE * 15)) {
      errno = result;
      perror("posix_memalign");
    }
    if (!first_superblock_ptr) {
      first_superblock_ptr = superblock_ptr;
    }
    if (next_superblock_ptr) {
      *next_superblock_ptr = (char *)superblock_ptr;
    }
    auto *hdr = (memory_block_header *)superblock_ptr;
    next_superblock_ptr = &hdr->next_superblock;
    hdr->next_superblock = nullptr;
  }
  blocks_left--;
  void *result = superblock_ptr;
  superblock_ptr = (char *)superblock_ptr + BLOCK_SIZE;
  return result;
}

static void fresh_block(struct arena *arena) {
  char *next_block = nullptr;
  if (arena->block_start == nullptr) {
    next_block = (char *)megabyte_malloc();
    arena->first_block = next_block;
    auto *next_header = (memory_block_header *)next_block;
    next_header->next_block = nullptr;
    next_header->semispace = arena->allocation_semispace_id;
    arena->num_blocks++;
  } else {
    next_block = *(char **)arena->block_start;
    if (arena->block != arena->block_end) {
      if (arena->block_end - arena->block == 8) {
        *(uint64_t *)arena->block
            = NOT_YOUNG_OBJECT_BIT; // 8 bit sentinel value
      } else {
        *(uint64_t *)arena->block = arena->block_end - arena->block
                                    - 8; // 16-bit or more sentinel value
      }
    }
    if (!next_block) {
      MEM_LOG(
          "Allocating new block for the first time in arena %d\n",
          Arena->allocation_semispace_id);
      next_block = (char *)megabyte_malloc();
      *(char **)arena->block_start = next_block;
      auto *next_header = (memory_block_header *)next_block;
      next_header->next_block = nullptr;
      next_header->semispace = arena->allocation_semispace_id;
      arena->num_blocks++;
    }
  }
  arena->block = next_block + sizeof(memory_block_header);
  arena->block_start = next_block;
  arena->block_end = next_block + BLOCK_SIZE;
  MEM_LOG(
      "New block at %p (remaining %zd)\n", Arena->block,
      BLOCK_SIZE - sizeof(memory_block_header));
}

static __attribute__((noinline)) void *
do_alloc_slow(size_t requested, struct arena *arena) {
  MEM_LOG(
      "Block at %p too small, %zd remaining but %zd needed\n", Arena->block,
      Arena->block_end - Arena->block, requested);
  if (requested > BLOCK_SIZE - sizeof(memory_block_header)) {
    return malloc(requested);
  }
  fresh_block(arena);
  void *result = arena->block;
  arena->block += requested;
  MEM_LOG(
      "Allocation at %p (size %zd), next alloc at %p (if it fits)\n", result,
      requested, Arena->block);
  return result;
}

__attribute__((always_inline)) void *
kore_arena_alloc(struct arena *arena, size_t requested) {
  if (arena->block + requested > arena->block_end) {
    return do_alloc_slow(requested, arena);
  }
  void *result = arena->block;
  arena->block += requested;
  MEM_LOG(
      "Allocation at %p (size %zd), next alloc at %p (if it fits)\n", result,
      requested, Arena->block);
  return result;
}

__attribute__((always_inline)) void *
arena_resize_last_alloc(struct arena *arena, ssize_t increase) {
  if (arena->block + increase <= arena->block_end) {
    arena->block += increase;
    return arena->block;
  }
  return nullptr;
}

__attribute__((always_inline)) void arena_swap_and_clear(struct arena *arena) {
  char *tmp = arena->first_block;
  arena->first_block = arena->first_collection_block;
  arena->first_collection_block = tmp;
  size_t tmp2 = arena->num_blocks;
  arena->num_blocks = arena->num_collection_blocks;
  arena->num_collection_blocks = tmp2;
  arena->allocation_semispace_id = ~arena->allocation_semispace_id;
  arena_clear(arena);
}

__attribute__((always_inline)) void arena_clear(struct arena *arena) {
  arena->block = arena->first_block
                     ? arena->first_block + sizeof(memory_block_header)
                     : nullptr;
  arena->block_start = arena->first_block;
  arena->block_end
      = arena->first_block ? arena->first_block + BLOCK_SIZE : nullptr;
}

__attribute__((always_inline)) char *arena_start_ptr(const struct arena *arena) {
  return arena->first_block ? arena->first_block + sizeof(memory_block_header)
                            : nullptr;
}

__attribute__((always_inline)) char **arena_end_ptr(struct arena *arena) {
  return &arena->block;
}

char *move_ptr(char *ptr, size_t size, char const *arena_end_ptr) {
  char *next_ptr = ptr + size;
  if (next_ptr == arena_end_ptr) {
    return nullptr;
  }
  if (next_ptr != MEM_BLOCK_START(ptr) + BLOCK_SIZE) {
    return next_ptr;
  }
  char *next_block = *(char **)MEM_BLOCK_START(ptr);
  if (!next_block) {
    return nullptr;
  }
  return next_block + sizeof(memory_block_header);
}

ssize_t ptr_diff(char *ptr1, char *ptr2) {
  if (MEM_BLOCK_START(ptr1) == MEM_BLOCK_START(ptr2)) {
    return ptr1 - ptr2;
  }
  memory_block_header *hdr = mem_block_header(ptr2);
  ssize_t result = 0;
  while (hdr != mem_block_header(ptr1) && hdr->next_block) {
    if (ptr2) {
      result += ((char *)hdr + BLOCK_SIZE) - ptr2;
      ptr2 = nullptr;
    } else {
      result += (BLOCK_SIZE - sizeof(memory_block_header));
    }
    hdr = (memory_block_header *)hdr->next_block;
  }
  if (hdr == mem_block_header(ptr1)) {
    result += ptr1 - (char *)(hdr + 1);
    return result;
  } // reached the end of the arena and didn't find the block
  // it's possible that the result should be negative, in which
  // case the block will have been prior to the block we started
  // at. To handle this, we recurse with reversed arguments and
  // negate the result. This means that the code might not
  // terminate if the two pointers do not belong to the same
  // arena.
  return -ptr_diff(ptr2, ptr1);
}

size_t arena_size(const struct arena *arena) {
  return (arena->num_blocks > arena->num_collection_blocks
              ? arena->num_blocks
              : arena->num_collection_blocks)
         * (BLOCK_SIZE - sizeof(memory_block_header));
}

void free_all_memory() {
  auto *superblock = (memory_block_header *)first_superblock_ptr;
  while (superblock) {
    auto *next_superblock = (memory_block_header *)superblock->next_superblock;
    free(superblock);
    superblock = next_superblock;
  }
  first_superblock_ptr = nullptr;
  superblock_ptr = nullptr;
  next_superblock_ptr = nullptr;
  blocks_left = 0;
}
