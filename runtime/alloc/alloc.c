#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "jemalloc/jemalloc.h"

#include "runtime/alloc.h"
#include "runtime/header.h"

const size_t BLOCK_SIZE = 1024 * 1024;

struct semispace_info {
  char* first_block;
  char* block;
  char* block_start;
  char* block_end;
  char semispace_id;
};

static struct semispace_info fromspace;
static struct semispace_info oldspace = {0, 0, 0, 0, 2};

static char* first_tospace_block;

char *fromspace_ptr() {
  return fromspace.first_block;
}

char *oldspace_ptr() {
  return oldspace.first_block;
}

char **alloc_ptr() {
  return &fromspace.block;
}

char **old_alloc_ptr() {
  return &oldspace.block;
}

char fromspace_id() {
  // bit has been flipped by now, so we need flip it back
  return 1 - fromspace.semispace_id;
}

void koreAllocSwap() {
  char *tmp = fromspace.first_block;
  fromspace.first_block = first_tospace_block;
  first_tospace_block = tmp;
  fromspace.block = fromspace.first_block ? fromspace.first_block + sizeof(memory_block_header) : 0;
  fromspace.block_start = fromspace.first_block;
  fromspace.block_end = fromspace.first_block ? fromspace.first_block + BLOCK_SIZE : fromspace.first_block;
  fromspace.semispace_id = 1 - fromspace.semispace_id;
}

static void* superblock_ptr = 0;
static unsigned blocks_left = 0;

static void* megabyte_malloc() {
  if (blocks_left == 0) {
    blocks_left = 15;
    posix_memalign(&superblock_ptr, BLOCK_SIZE, BLOCK_SIZE * 15);
  }
  blocks_left--;
  void* result = superblock_ptr;
  superblock_ptr += BLOCK_SIZE;
  return result;
}

static void freshBlock(struct semispace_info *space) {
    char *nextBlock;
    if (space->block_start == 0) {
      nextBlock = megabyte_malloc();
      space->first_block = nextBlock;
      memory_block_header hdr;
      hdr.next_block = 0;
      hdr.semispace = space->semispace_id;
      *(memory_block_header *)nextBlock = hdr;
    } else {
      nextBlock = *(char**)space->block_start;
      if (space->block != space->block_end) {
        if (space->block_end - space->block == 8) {
          *(uint64_t *)space->block = NOT_YOUNG_OBJECT_BIT; // 8 bit sentinel value
        } else {
          *(uint64_t *)space->block = space->block_end - space->block - 8; // 16-bit or more sentinel value
        }
      }
      if (!nextBlock) {
        MEM_LOG("Allocating new block for the first time in semispace %d\n", space->semispace_id);
        nextBlock = megabyte_malloc();
        *(char **)space->block_start = nextBlock;
        memory_block_header hdr;
        hdr.next_block = 0;
        hdr.semispace = space->semispace_id;
        memcpy(nextBlock, &hdr, sizeof(hdr));
      }
    }
    space->block = nextBlock + sizeof(memory_block_header);
    space->block_start = nextBlock;
    space->block_end = nextBlock + BLOCK_SIZE;
    MEM_LOG("New block at %p (remaining %zd)\n", space->block, BLOCK_SIZE - sizeof(memory_block_header));
}

static void* __attribute__ ((noinline)) doAllocSlow(size_t requested, struct semispace_info *space) {
  MEM_LOG("Block at %p too small, %zd remaining but %zd needed\n", space->block, space->block_end-space->block, requested);
  if (requested > BLOCK_SIZE - sizeof(memory_block_header)) {
     return malloc(requested);
  } else {
    freshBlock(space);
    void* result = space->block;
    space->block += requested;
    MEM_LOG("Allocation at %p (size %zd), next alloc at %p (if it fits)\n", result, requested, space->block);
    return result;
  }
}

static inline __attribute__ ((always_inline)) void* doAlloc(size_t requested, struct semispace_info *space) {
  if (space->block + requested > space->block_end) {
    return doAllocSlow(requested, space);
  }
  void* result = space->block;
  space->block += requested;
  MEM_LOG("Allocation at %p (size %zd), next alloc at %p (if it fits)\n", result, requested, space->block);
  return result;
}

__attribute__ ((always_inline)) void* koreAlloc(size_t requested) {
  return doAlloc(requested, &fromspace);
}

__attribute__ ((always_inline)) void* koreAllocToken(size_t requested) {
  size_t size = (requested + 7) & ~7;
  return doAlloc(size < 16 ? 16 : size, &fromspace);
}

__attribute__ ((always_inline)) void* koreAllocOld(size_t requested) {
  void *out = doAlloc(requested, &oldspace);
  printf("ALLOC OLD requested: %ld @ %llx\n", requested, (unsigned long long)out);
  return out;
}

__attribute__ ((always_inline)) void* koreAllocTokenOld(size_t requested) {
  size_t size = (requested + 7) & ~7;
  return doAlloc(size < 16 ? 16 : size, &oldspace);
}

void* koreResizeLastAlloc(void* oldptr, size_t newrequest, size_t last_size) {
  newrequest = (newrequest + 7) & ~7;
  last_size = (last_size + 7) & ~7;
  if (oldptr != fromspace.block - last_size) {
    MEM_LOG("May only reallocate last allocation. Tried to reallocate %p to %zd\n", oldptr, newrequest);
    exit(255);
  }
  ssize_t increase = newrequest - last_size;
  if (fromspace.block + increase <= fromspace.block_end) {
    fromspace.block += increase;
    return oldptr;
  } else {
    void* newptr = koreAlloc(newrequest);
    memcpy(newptr,oldptr,last_size);
    return newptr;
  }
}

void* koreReallocOld(void* ptr, size_t old_size, size_t new_size) {
  void* new = koreAllocOld(new_size);
  size_t min = old_size > new_size ? new_size : old_size;
  memcpy(new, ptr, min);
  return new;
}

void koreFree(void* ptr, size_t size) {}
