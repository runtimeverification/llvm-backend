
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

__attribute__((always_inline)) void arenaReset(struct arena *arena) {
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
getArenaAllocationSemispaceID(const struct arena *arena) {
  return arena->allocation_semispace_id;
}

__attribute__((always_inline)) char
getArenaCollectionSemispaceID(const struct arena *arena) {
  return ~arena->allocation_semispace_id;
}

__attribute__((always_inline)) char getArenaSemispaceIDOfObject(void *ptr) {
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

static void freshBlock(struct arena *arena) {
  char *nextBlock = nullptr;
  if (arena->block_start == nullptr) {
    nextBlock = (char *)megabyte_malloc();
    arena->first_block = nextBlock;
    auto *nextHeader = (memory_block_header *)nextBlock;
    nextHeader->next_block = nullptr;
    nextHeader->semispace = arena->allocation_semispace_id;
    arena->num_blocks++;
  } else {
    nextBlock = *(char **)arena->block_start;
    if (arena->block != arena->block_end) {
      if (arena->block_end - arena->block == 8) {
        *(uint64_t *)arena->block
            = NOT_YOUNG_OBJECT_BIT; // 8 bit sentinel value
      } else {
        *(uint64_t *)arena->block = arena->block_end - arena->block
                                    - 8; // 16-bit or more sentinel value
      }
    }
    if (!nextBlock) {
      MEM_LOG(
          "Allocating new block for the first time in arena %d\n",
          Arena->allocation_semispace_id);
      nextBlock = (char *)megabyte_malloc();
      *(char **)arena->block_start = nextBlock;
      auto *nextHeader = (memory_block_header *)nextBlock;
      nextHeader->next_block = nullptr;
      nextHeader->semispace = arena->allocation_semispace_id;
      arena->num_blocks++;
    }
  }
  arena->block = nextBlock + sizeof(memory_block_header);
  arena->block_start = nextBlock;
  arena->block_end = nextBlock + BLOCK_SIZE;
  MEM_LOG(
      "New block at %p (remaining %zd)\n", Arena->block,
      BLOCK_SIZE - sizeof(memory_block_header));
}

static __attribute__((noinline)) void *
doAllocSlow(size_t requested, struct arena *arena) {
  MEM_LOG(
      "Block at %p too small, %zd remaining but %zd needed\n", Arena->block,
      Arena->block_end - Arena->block, requested);
  if (requested > BLOCK_SIZE - sizeof(memory_block_header)) {
    return malloc(requested);
  }
  freshBlock(arena);
  void *result = arena->block;
  arena->block += requested;
  MEM_LOG(
      "Allocation at %p (size %zd), next alloc at %p (if it fits)\n", result,
      requested, Arena->block);
  return result;
}

__attribute__((always_inline)) void *
koreArenaAlloc(struct arena *arena, size_t requested) {
  if (arena->block + requested > arena->block_end) {
    return doAllocSlow(requested, arena);
  }
  void *result = arena->block;
  arena->block += requested;
  MEM_LOG(
      "Allocation at %p (size %zd), next alloc at %p (if it fits)\n", result,
      requested, Arena->block);
  return result;
}

__attribute__((always_inline)) void *
arenaResizeLastAlloc(struct arena *arena, ssize_t increase) {
  if (arena->block + increase <= arena->block_end) {
    arena->block += increase;
    return arena->block;
  }
  return nullptr;
}

__attribute__((always_inline)) void arenaSwapAndClear(struct arena *arena) {
  char *tmp = arena->first_block;
  arena->first_block = arena->first_collection_block;
  arena->first_collection_block = tmp;
  size_t tmp2 = arena->num_blocks;
  arena->num_blocks = arena->num_collection_blocks;
  arena->num_collection_blocks = tmp2;
  arena->allocation_semispace_id = ~arena->allocation_semispace_id;
  arenaClear(arena);
}

__attribute__((always_inline)) void arenaClear(struct arena *arena) {
  arena->block = arena->first_block
                     ? arena->first_block + sizeof(memory_block_header)
                     : nullptr;
  arena->block_start = arena->first_block;
  arena->block_end
      = arena->first_block ? arena->first_block + BLOCK_SIZE : nullptr;
}

__attribute__((always_inline)) char *arenaStartPtr(const struct arena *arena) {
  return arena->first_block ? arena->first_block + sizeof(memory_block_header)
                            : nullptr;
}

__attribute__((always_inline)) char **arenaEndPtr(struct arena *arena) {
  return &arena->block;
}

char *movePtr(char *ptr, size_t size, char const *arena_end_ptr) {
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

ssize_t ptrDiff(char *ptr1, char *ptr2) {
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
  return -ptrDiff(ptr2, ptr1);
}

size_t arenaSize(const struct arena *arena) {
  return (arena->num_blocks > arena->num_collection_blocks
              ? arena->num_blocks
              : arena->num_collection_blocks)
         * (BLOCK_SIZE - sizeof(memory_block_header));
}

void freeAllMemory() {
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
