#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "runtime/alloc.h"
#include "runtime/header.h"
#include "runtime/arena.h"


REGISTER_ARENA(youngspace, 0);
REGISTER_ARENA(oldspace, 1);

char *youngspace_ptr() {
  return arenaStartPtr(&youngspace);
}

char *oldspace_ptr() {
  return arenaStartPtr(&oldspace);
}

char **young_alloc_ptr() {
  return arenaEndPtr(&youngspace);
}

char **old_alloc_ptr() {
  return arenaEndPtr(&oldspace);
}

char youngspace_collection_id() {
  return getArenaCollectionSemispaceID(&youngspace);
}

void koreAllocSwap() {
  arenaSwapAndReset(&youngspace);
}

void freeAllKoreMem() {
  freeAllMemory();
  arenaReset(&youngspace);
  arenaReset(&oldspace);
}

__attribute__ ((always_inline)) void* koreAlloc(size_t requested) {
  return arenaAlloc(&youngspace, requested);
}

__attribute__ ((always_inline)) void* koreAllocToken(size_t requested) {
  size_t size = (requested + 7) & ~7;
  return arenaAlloc(&youngspace, size < 16 ? 16 : size);
}

__attribute__ ((always_inline)) void* koreAllocOld(size_t requested) {
  return arenaAlloc(&oldspace, requested);
}

__attribute__ ((always_inline)) void* koreAllocTokenOld(size_t requested) {
  size_t size = (requested + 7) & ~7;
  return arenaAlloc(&oldspace, size < 16 ? 16 : size);
}

void* koreResizeLastAlloc(void* oldptr, size_t newrequest, size_t last_size) {
  newrequest = (newrequest + 7) & ~7;
  last_size = (last_size + 7) & ~7;
  if (oldptr != *arenaEndPtr(&youngspace) - last_size) {
    MEM_LOG("May only reallocate last allocation. Tried to reallocate %p to %zd\n", oldptr, newrequest);
    exit(255);
  }
  ssize_t increase = newrequest - last_size;
  if (arenaResizeLastAlloc(&youngspace, increase)) {
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
