#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "jemalloc/jemalloc.h"

#include "runtime/alloc.h"
#include "runtime/header.h"

const size_t BLOCK_SIZE = 1024 * 1024;

bool true_is_fromspace = false;

static char* first_block = 0;
static char* first_tospace_block = 0;
static char* block_ptr = 0;
static char* block_start = 0;
static char* block_end = 0;

char *fromspace_ptr() {
  return first_block;
}

char *alloc_ptr() {
  return block_ptr;
}

char *arena_ptr() {
  return block_start;
}

void koreAllocSwap() {
  char *tmp = first_block;
  first_block = first_tospace_block;
  first_tospace_block = tmp;
  block_ptr = first_block ? first_block + sizeof(memory_block_header) : 0;
  block_start = first_block;
  block_end = first_block ? first_block + BLOCK_SIZE : first_block;
  true_is_fromspace = !true_is_fromspace;
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

static void freshBlock() {
    char *nextBlock;
    if (block_start == 0) {
      nextBlock = megabyte_malloc();
      first_block = nextBlock;
      memory_block_header hdr;
      hdr.next_block = 0;
      hdr.semispace = true_is_fromspace;
      *(memory_block_header *)nextBlock = hdr;
    } else {
      nextBlock = *(char**)block_start;
      if (block_ptr != block_end) {
        if (block_end - block_ptr == 8) {
          // we special case this value to 8 bytes.
          *(uint64_t *)block_ptr = NOT_YOUNG_OBJECT_BIT; // 8 bit sentinel value
        } else {
          // this is a valid struct string
          *(uint64_t *)block_ptr = block_end - block_ptr - 8; // 16-bit or more sentinel value
        }
      }
      if (!nextBlock) {
        MEM_LOG("Allocating new block for the first time in semispace %d\n", true_is_fromspace);
        nextBlock = megabyte_malloc();
        *(char **)block_start = nextBlock;
        memory_block_header hdr;
        hdr.next_block = 0;
        hdr.semispace = true_is_fromspace;
        memcpy(nextBlock, &hdr, sizeof(hdr));
      }
    }
    block_ptr = nextBlock + sizeof(memory_block_header);
    block_start = nextBlock;
    block_end = nextBlock + BLOCK_SIZE;
    MEM_LOG("New block at %p (remaining %zd)\n", block_ptr, BLOCK_SIZE - sizeof(memory_block_header));
}

static void* __attribute__ ((noinline)) doAllocSlow(size_t requested) {
  MEM_LOG("Block at %p too small, %zd remaining but %zd needed\n", block_ptr, block_end-block_ptr, requested);
  if (requested > BLOCK_SIZE - sizeof(memory_block_header)) {
     return malloc(requested);
  } else {
    freshBlock();
    void* result = block_ptr;
    block_ptr += requested;
    MEM_LOG("Allocation at %p (size %zd), next alloc at %p (if it fits)\n", result, requested, block_ptr);
    return result;
  }
}

static inline __attribute__ ((always_inline)) void* doAlloc(size_t requested) {
  if (block_ptr + requested > block_end) {
    return doAllocSlow(requested);
  }
  void* result = block_ptr;
  block_ptr += requested;
  MEM_LOG("Allocation at %p (size %zd), next alloc at %p (if it fits)\n", result, requested, block_ptr);
  return result;
}

void* koreAlloc(size_t requested) {
  return doAlloc(requested);
}

void* koreAllocToken(size_t requested) {
  size_t size = (requested + 7) & ~7;
  return doAlloc(size < 16 ? 16 : size);
}

void* koreResizeLastAlloc(void* oldptr, size_t newrequest, size_t last_size) {
  newrequest = (newrequest + 7) & ~7;
  last_size = (last_size + 7) & ~7;
  if (oldptr != block_ptr - last_size) {
    MEM_LOG("May only reallocate last allocation. Tried to reallocate %p to %zd\n", oldptr, newrequest);
    exit(255);
  }
  ssize_t increase = newrequest - last_size;
  if (block_ptr + increase <= block_end) {
    block_ptr += increase;
    return oldptr;
  } else {
    void* newptr = koreAlloc(newrequest);
    memcpy(newptr,oldptr,last_size);
    return newptr;
  }
}
