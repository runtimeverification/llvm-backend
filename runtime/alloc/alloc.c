#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "jemalloc/jemalloc.h"

#include "runtime/alloc.h"

#ifdef ALLOC_DBG
#define DBG(...) fprintf(stderr, __VA_ARGS__)
#else
#define DBG(...)
#endif

// 1 MiB minus 16 bytes for libc malloc overhead
const size_t BLOCK_SIZE = 1048568;

static char* first_block = 0;
static char* first_tospace_block = 0;
static char* block = 0;
static char* block_start = 0;
static char* block_end = 0;

char *fromspace_ptr() {
  return first_block;
}

char *alloc_ptr() {
  return block;
}

char *arena_ptr() {
  return block_start;
}

void koreAllocSwap() {
  char *tmp = first_block;
  first_block = first_tospace_block;
  first_tospace_block = tmp;
  block = first_block ? first_block + sizeof(char *) : 0;
  block_start = first_block;
  block_end = first_block ? first_block + BLOCK_SIZE : first_block;
}

static void freshBlock() {
    char *nextBlock;
    if (block_start == 0) {
      nextBlock = malloc(BLOCK_SIZE);
      first_block = nextBlock;
      memset(nextBlock, 0, sizeof(char *));
    } else {
      memcpy(&nextBlock, block_start, sizeof(char *));
      if (!nextBlock) {
        nextBlock = malloc(BLOCK_SIZE);
        memcpy(block_start, &nextBlock, sizeof(char *));
        memset(nextBlock, 0, sizeof(char *));
      }
    }
    block = nextBlock + sizeof(char *);
    block_start = nextBlock;
    block_end = nextBlock + BLOCK_SIZE;
    DBG("New block at %p (remaining %zd)\n", block, BLOCK_SIZE - sizeof(char *));
}

static void* __attribute__ ((noinline)) doAllocSlow(size_t requested) {
  DBG("Block at %p too small, %zd remaining but %zd needed\n", block, block_end-block, requested);
  if (requested > BLOCK_SIZE - sizeof(char *)) {
     return malloc(requested);
  } else {
    freshBlock();
    void* result = block;
    block += requested;
    DBG("Allocation at %p (size %zd), next alloc at %p (if it fits)\n", result, requested, block);
    return result;
  }
}

static inline void* doAlloc(size_t requested) {
  if (block + requested > block_end) {
    return doAllocSlow(requested);
  }
  void* result = block;
  block += requested;
  DBG("Allocation at %p (size %zd), next alloc at %p (if it fits)\n", result, requested, block);
  return result;
}

void* koreAlloc(size_t requested) {
  return doAlloc(requested);
}

void* koreAllocToken(size_t requested) {
  return doAlloc((requested + 7) & ~7);
}

void* koreResizeLastAlloc(void* oldptr, size_t newrequest, size_t last_size) {
  newrequest = (newrequest + 7) & ~7;
  last_size = (last_size + 7) & ~7;
  if (oldptr != block - last_size) {
    DBG("May only reallocate last allocation. Tried to reallocate %p to %zd\n", oldptr, newrequest);
    exit(255);
  }
  ssize_t increase = newrequest - last_size;
  if (block + increase <= block_end) {
    block += increase;
    return oldptr;
  } else {
    void* newptr = koreAlloc(newrequest);
    memcpy(newptr,oldptr,last_size);
    return newptr;
  }
}
