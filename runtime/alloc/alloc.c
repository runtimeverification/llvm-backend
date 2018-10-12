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

// const size_t BLOCK_SIZE = 1048568;
// 1 MiB minus 16 bytes for libc malloc overhead
const size_t BLOCK_SIZE = 1000000000;

static char* first_block = 0;
static char* first_tospace_block = 0;
static char* block = 0;
static char* block_start = 0;
static size_t remaining = 0;
static size_t last_size = 0;

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
  remaining = first_block ? BLOCK_SIZE - sizeof(char *) : 0;
  last_size = 0;
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
    remaining = BLOCK_SIZE - sizeof(char *);
    DBG("New block at %p (remaining %zd)\n", block, remaining);
}

void* koreAlloc(size_t requested) {
  requested = (requested + 7) & ~7;
  if (remaining < requested) {
    DBG("Block at %p too small, %zd remaining but %zd needed\n", block, remaining, requested);
    if (requested > BLOCK_SIZE) {
      void *bigObject = malloc(requested);
      *((uint64_t *)bigObject) = 0x400000000000;
      return bigObject;
    } else {
      freshBlock();
    }
  }
  void* result = block;
  *((uint64_t *)result) = 0;
  block += requested;
  remaining -= requested;
  last_size = requested;
  DBG("Allocation at %p (size %zd), next alloc at %p (if it fits)\n", result, requested, block);
  return result;
}

void* koreResizeLastAlloc(void* oldptr, size_t newrequest) {
  newrequest = (newrequest + 7) & ~7;
  if (oldptr != block - last_size) {
    DBG("May only reallocate last allocation. Tried to reallocate %p to %zd\n", oldptr, newrequest);
    exit(255);
  }
  ssize_t increase = newrequest - last_size;
  if (increase < remaining) {
    remaining += increase;
    last_size = newrequest;
    return oldptr;
  } else {
    void* newptr = koreAlloc(newrequest); // sets last_size
    memcpy(newptr,oldptr,last_size);
    return newptr;
  }
}
