#include <gmp.h>

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "runtime/alloc.h"
#include "runtime/arena.h"
#include "runtime/header.h"

extern "C" {

// class arena supports ID from  0 to 127

// New data in allocated in the youngspace, which requests a
// collection when is gets too full.
thread_local constinit arena youngspace(YOUNGSPACE_ID, true);

// Data that is old enough is migrated to the oldspace. The
// migrated data is always live at this point so it never
// requests a collection.
thread_local constinit arena oldspace(OLDSPACE_ID, false);

// Temporary data is doesn't use the garbage collector.
thread_local constinit arena alwaysgcspace(ALWAYSGCSPACE_ID, false);

char *youngspace_ptr() {
  return youngspace.start_ptr();
}

char *oldspace_ptr() {
  return oldspace.start_ptr();
}

char youngspace_collection_id() {
  return youngspace.get_arena_collection_semispace_id();
}

char oldspace_collection_id() {
  return oldspace.get_arena_collection_semispace_id();
}

void kore_alloc_swap(bool swap_old) {
  youngspace.arena_swap_and_clear();
  if (swap_old) {
    oldspace.arena_swap_and_clear();
  }
}

void kore_clear() {
  alwaysgcspace.arena_clear();
}

void set_kore_memory_functions_for_gmp() {
  mp_set_memory_functions(kore_alloc_mp, kore_realloc_mp, kore_free);
}

__attribute__((always_inline)) void *kore_alloc(size_t requested) {
  return youngspace.kore_arena_alloc(requested);
}

__attribute__((always_inline)) void *kore_alloc_token(size_t requested) {
  size_t size = (requested + 7) & ~7;
  return youngspace.kore_arena_alloc(size < 16 ? 16 : size);
}

__attribute__((always_inline)) void *kore_alloc_old(size_t requested) {
  return oldspace.kore_arena_alloc(requested);
}

__attribute__((always_inline)) void *kore_alloc_token_old(size_t requested) {
  size_t size = (requested + 7) & ~7;
  return oldspace.kore_arena_alloc(size < 16 ? 16 : size);
}

__attribute__((always_inline)) void *kore_alloc_always_gc(size_t requested) {
  return alwaysgcspace.kore_arena_alloc(requested);
}

void *
kore_resize_last_alloc(void *oldptr, size_t newrequest, size_t last_size) {
  newrequest = (newrequest + 7) & ~7;
  last_size = (last_size + 7) & ~7;

  if (oldptr != youngspace.end_ptr() - last_size) {
    MEM_LOG(
        "May only reallocate last allocation. Tried to reallocate %p to %zd\n",
        oldptr, newrequest);
    exit(255);
  }

  ssize_t increase = newrequest - last_size;
  if (youngspace.arena_resize_last_alloc(increase)) {
    return oldptr;
  }

  void *newptr = kore_alloc(newrequest);
  memcpy(newptr, oldptr, last_size);
  return newptr;
}

void *kore_alloc_mp(size_t requested) {
  bool enabled = gc_enabled;
  gc_enabled = false;
  auto *new_token = (string *)kore_alloc_token(sizeof(string) + requested);
  gc_enabled = enabled;
  init_with_len(new_token, requested);
  return new_token->data;
}

void *kore_realloc_mp(void *ptr, size_t old_size, size_t new_size) {
  bool enabled = gc_enabled;
  gc_enabled = false;
  auto *new_token = (string *)kore_alloc_token(sizeof(string) + new_size);
  gc_enabled = enabled;
  size_t min = old_size > new_size ? new_size : old_size;
  memcpy(new_token->data, ptr, min);
  init_with_len(new_token, new_size);
  return new_token->data;
}

void kore_free(void *ptr, size_t size) { }

__attribute__((always_inline)) void *kore_alloc_integer(size_t requested) {
  auto *result = (mpz_hdr *)kore_alloc(sizeof(mpz_hdr));
  init_with_len(result, sizeof(mpz_hdr) - sizeof(blockheader));
  return &result->i;
}

__attribute__((always_inline)) void *kore_alloc_floating(size_t requested) {
  auto *result = (floating_hdr *)kore_alloc(sizeof(floating_hdr));
  init_with_len(result, sizeof(floating_hdr) - sizeof(blockheader));
  return &result->f;
}

__attribute__((always_inline)) void *kore_alloc_integer_old(size_t requested) {
  auto *result = (mpz_hdr *)kore_alloc_old(sizeof(mpz_hdr));
  init_with_len(result, sizeof(mpz_hdr) - sizeof(blockheader));
  return &result->i;
}

__attribute__((always_inline)) void *kore_alloc_floating_old(size_t requested) {
  auto *result = (floating_hdr *)kore_alloc_old(sizeof(floating_hdr));
  init_with_len(result, sizeof(floating_hdr) - sizeof(blockheader));
  return &result->f;
}

extern "C++" {
template <typename collection>
static inline void *kore_alloc_collection(kllvm::sort_category cat) {
  void *mem
      = kore_alloc(sizeof(blockheader) + sizeof(collection) + sizeof(uint64_t));
  auto *hdr = (blockheader *)mem;
  static thread_local std::string name = get_raw_symbol_name(cat) + "{}";
  static thread_local blockheader hdr_val
      = get_block_header_for_symbol(get_tag_for_symbol_name(name.c_str()));
  *hdr = hdr_val;
  auto *offset = (uint64_t *)(hdr + 1);
  *offset = 16;
  auto *child = hdr + 2;
  return child;
}
}

void *kore_alloc_map(size_t requested) {
  return kore_alloc_collection<map>(kllvm::sort_category::Map);
}

void *kore_alloc_set(size_t requested) {
  return kore_alloc_collection<set>(kllvm::sort_category::Set);
}

void *kore_alloc_list(size_t requested) {
  return kore_alloc_collection<list>(kllvm::sort_category::List);
}

void *kore_alloc_rangemap(size_t requested) {
  return kore_alloc_collection<rangemap>(kllvm::sort_category::RangeMap);
}
}
