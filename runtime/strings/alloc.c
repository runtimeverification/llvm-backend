#include "jemalloc/jemalloc.h"
#include <string.h>

#ifdef ALLOC_DBG
#include <stdio.h>
#define DBG(...) fprintf(stderr, __VA_ARGS__)
#else
#define DBG(...)
#endif

static char* block = 0;
static size_t remaining = 0;
static size_t last_size = 0;

static void freshBlock() {
    block = malloc(1048568);
    remaining = 1048568;
    DBG("New block at %p (remaining %zd)\n", block, remaining);
}

void* koreAlloc(size_t requested) {
  if (remaining < requested) {
    DBG("Block at %p too small, %zd remaining but %zd needed\n", block, remaining, requested);
    freshBlock();
  }
  void* result = block;
  block += requested;
  remaining -= requested;
  last_size = requested;
  DBG("Allocation at %p (size %zd), next alloc at %p (if it fits)\n", result, requested, block);
  return result;
}

void* koreResizeLastAlloc(void* oldptr, size_t newrequest) {
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
