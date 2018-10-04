#include <string.h>
#include <stdio.h>
#include <stdlib.h>
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

static char* block = 0;
static size_t remaining = 0;
static size_t last_size = 0;

static void freshBlock() {
    block = malloc(BLOCK_SIZE);
    remaining = BLOCK_SIZE;
    DBG("New block at %p (remaining %zd)\n", block, remaining);
}

void* koreAlloc(size_t requested) {
  requested = (requested + 7) & ~7;
  if (remaining < requested) {
    DBG("Block at %p too small, %zd remaining but %zd needed\n", block, remaining, requested);
    if (requested > BLOCK_SIZE) {
      return malloc(requested);
    } else {
      freshBlock();
    }
  }
  void* result = block;
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
