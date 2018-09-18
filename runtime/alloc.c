#include "jemalloc/jemalloc.h"
#include <stdio.h>

static char* block = 0;
static size_t remaining = 0;

char* koreAlloc(size_t requested) {
  if (remaining < size) {
    printf("Block at %p too small, %zd remaining but %zd needed\n", block, remaining, requested);
    block = malloc(1048568);
    remaining = 1048568;
    printf("New block at %p (remaining %zd)\n", block, remaining);
  }
  char* result = block;
  block += requested;
  remaining -= requested;
  printf("Allocation at %p (size %zd), next alloc at %p (if it fits)\n", result, size, block);
  return result;
}
