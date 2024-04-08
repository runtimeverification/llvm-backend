#include "runtime/collect.h"
#include "runtime/alloc.h"
#include "runtime/arena.h"
#include "runtime/header.h"
#include <cassert>
#include <cstdbool>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>

extern "C" {

char **young_alloc_ptr(void);
char **old_alloc_ptr(void);
char *youngspace_ptr(void);
char *oldspace_ptr(void);

static bool is_gc = false;
bool collect_old = false;
#ifndef GC_DBG
static uint8_t num_collection_only_young = 0;
#else
static char *last_alloc_ptr;
#endif

size_t numBytesLiveAtCollection[1 << AGE_WIDTH];
void set_gc_threshold(size_t);
size_t get_gc_threshold(void);
bool youngspace_almost_full(size_t);

bool during_gc() {
  return is_gc;
}

size_t get_size(uint64_t hdr, uint16_t layout) {
  if (!layout) {
    size_t size = (len_hdr(hdr) + sizeof(blockheader) + 7) & ~7;
    return hdr == NOT_YOUNG_OBJECT_BIT ? 8 : size < 16 ? 16 : size;
  }
  return size_hdr(hdr);
}

void migrate(block **block_ptr) {
  block *curr_block = *block_ptr;
  if (is_leaf_block(curr_block) || !is_heap_block(curr_block)) {
    return;
  }
  uint64_t const hdr = curr_block->h.hdr;
  INITIALIZE_MIGRATE();
  uint16_t layout = layout_hdr(hdr);
  size_t len_in_bytes = get_size(hdr, layout);
  auto **forwarding_address = (block **)(curr_block + 1);
  if (!hasForwardingAddress) {
    block *new_block = nullptr;
    if (shouldPromote || (isInOldGen && collect_old)) {
      new_block = (block *)kore_alloc_old(len_in_bytes);
    } else {
      new_block = (block *)kore_alloc(len_in_bytes);
    }
#ifdef GC_DBG
    numBytesLiveAtCollection[oldAge] += len_in_bytes;
#endif
    memcpy(new_block, curr_block, len_in_bytes);
    MIGRATE_HEADER(new_block);
    *forwarding_address = new_block;
    curr_block->h.hdr |= FWD_PTR_BIT;
    *block_ptr = new_block;
  } else {
    *block_ptr = *forwarding_address;
  }
}

// call this function instead of migrate on objects directly referenced by
// shared objects (like collection nodes) that are not tracked by gc
void migrate_once(block **block_ptr) {
  block *curr_block = *block_ptr;
  if (is_leaf_block(curr_block) || !is_heap_block(curr_block)) {
    return;
  }
  if (youngspace_collection_id()
          == get_arena_semispace_id_of_object((void *)curr_block)
      || oldspace_collection_id()
             == get_arena_semispace_id_of_object((void *)curr_block)) {
    migrate(block_ptr);
  }
}

static void migrate_string_buffer(stringbuffer **buffer_ptr) {
  stringbuffer *buffer = *buffer_ptr;
  uint64_t const hdr = buffer->h.hdr;
  uint64_t const cap = len(buffer->contents);
  INITIALIZE_MIGRATE();
  if (!hasForwardingAddress) {
    stringbuffer *new_buffer = nullptr;
    string *new_contents = nullptr;
    if (shouldPromote || (isInOldGen && collect_old)) {
      new_buffer = (stringbuffer *)kore_alloc_old(sizeof(stringbuffer));
      new_contents = (string *)kore_alloc_token_old(sizeof(string) + cap);
    } else {
      new_buffer = (stringbuffer *)kore_alloc(sizeof(stringbuffer));
      new_contents = (string *)kore_alloc_token(sizeof(string) + cap);
    }
#ifdef GC_DBG
    numBytesLiveAtCollection[oldAge]
        += cap + sizeof(stringbuffer) + sizeof(string);
#endif
    memcpy(new_contents, buffer->contents, sizeof(string) + buffer->strlen);
    memcpy(new_buffer, buffer, sizeof(stringbuffer));
    MIGRATE_HEADER(new_buffer);
    new_buffer->contents = new_contents;
    *(stringbuffer **)(buffer->contents) = new_buffer;
    buffer->h.hdr |= FWD_PTR_BIT;
  }
  *buffer_ptr = *(stringbuffer **)(buffer->contents);
}

static void migrate_mpz(mpz_ptr *ptr) {
  mpz_hdr *intgr = STRUCT_BASE(mpz_hdr, i, *ptr);
  uint64_t const hdr = intgr->h.hdr;
  INITIALIZE_MIGRATE();
  if (!hasForwardingAddress) {
    mpz_hdr *new_intgr = nullptr;
    string *new_limbs = nullptr;
    bool has_limbs = intgr->i->_mp_alloc > 0;
#ifdef GC_DBG
    numBytesLiveAtCollection[oldAge] += sizeof(mpz_hdr);
#endif
    if (has_limbs) {
      string *limbs = STRUCT_BASE(string, data, intgr->i->_mp_d);
      size_t len_limbs = len(limbs);

#ifdef GC_DBG
      numBytesLiveAtCollection[oldAge] += len_limbs + sizeof(string);
#endif

      assert(intgr->i->_mp_alloc * sizeof(mp_limb_t) == len_limbs);

      if (shouldPromote || (isInOldGen && collect_old)) {
        new_intgr = STRUCT_BASE(mpz_hdr, i, kore_alloc_integer_old(0));
        new_limbs = (string *)kore_alloc_token_old(sizeof(string) + len_limbs);
      } else {
        new_intgr = STRUCT_BASE(mpz_hdr, i, kore_alloc_integer(0));
        new_limbs = (string *)kore_alloc_token(sizeof(string) + len_limbs);
      }
      memcpy(new_limbs, limbs, sizeof(string) + len_limbs);
    } else {
      if (shouldPromote || (isInOldGen && collect_old)) {
        new_intgr = STRUCT_BASE(mpz_hdr, i, kore_alloc_integer_old(0));
      } else {
        new_intgr = STRUCT_BASE(mpz_hdr, i, kore_alloc_integer(0));
      }
    }
    memcpy(new_intgr, intgr, sizeof(mpz_hdr));
    MIGRATE_HEADER(new_intgr);
    if (has_limbs) {
      new_intgr->i->_mp_d = (mp_limb_t *)new_limbs->data;
    }
    *(mpz_ptr *)(&intgr->i->_mp_d) = new_intgr->i;
    intgr->h.hdr |= FWD_PTR_BIT;
  }
  *ptr = *(mpz_ptr *)(&intgr->i->_mp_d);
}

static void migrate_floating(floating **floating_ptr) {
  floating_hdr *flt = STRUCT_BASE(floating_hdr, f, *floating_ptr);
  uint64_t const hdr = flt->h.hdr;
  INITIALIZE_MIGRATE();
  if (!hasForwardingAddress) {
    floating_hdr *new_flt = nullptr;
    string *new_limbs = nullptr;
    string *limbs = STRUCT_BASE(string, data, flt->f.f->_mpfr_d - 1);
    size_t len_limbs = len(limbs);

#ifdef GC_DBG
    numBytesLiveAtCollection[oldAge]
        += sizeof(floating_hdr) + sizeof(string) + len_limbs;
#endif

    assert(
        ((flt->f.f->_mpfr_prec + mp_bits_per_limb - 1) / mp_bits_per_limb)
            * sizeof(mp_limb_t)
        <= len_limbs);

    if (shouldPromote || (isInOldGen && collect_old)) {
      new_flt = STRUCT_BASE(floating_hdr, f, kore_alloc_floating_old(0));
      new_limbs = (string *)kore_alloc_token_old(sizeof(string) + len_limbs);
    } else {
      new_flt = STRUCT_BASE(floating_hdr, f, kore_alloc_floating(0));
      new_limbs = (string *)kore_alloc_token(sizeof(string) + len_limbs);
    }
    memcpy(new_limbs, limbs, sizeof(string) + len_limbs);
    memcpy(new_flt, flt, sizeof(floating_hdr));
    MIGRATE_HEADER(new_flt);
    new_flt->f.f->_mpfr_d = (mp_limb_t *)new_limbs->data + 1;
    *(floating **)(flt->f.f->_mpfr_d) = &new_flt->f;
    flt->h.hdr |= FWD_PTR_BIT;
  }
  *floating_ptr = *(floating **)(flt->f.f->_mpfr_d);
}

static void migrate_child(
    void *curr_block, layoutitem *args, unsigned i, bool ptr, bool is_block) {
  layoutitem *arg_data = args + i;
  void *arg = ((char *)curr_block) + arg_data->offset;
  if (is_block) {
    migrate((block **)arg);
    return;
  }
  switch (arg_data->cat) {
  case MAP_LAYOUT: migrate_map(ptr ? *(map **)arg : arg); break;
  case RANGEMAP_LAYOUT: migrate_rangemap(ptr ? *(rangemap **)arg : arg); break;
  case LIST_LAYOUT: migrate_list(ptr ? *(list **)arg : arg); break;
  case SET_LAYOUT: migrate_set(ptr ? *(set **)arg : arg); break;
  case STRINGBUFFER_LAYOUT: migrate_string_buffer((stringbuffer **)arg); break;
  case SYMBOL_LAYOUT:
  case VARIABLE_LAYOUT: migrate((block **)arg); break;
  case INT_LAYOUT: migrate_mpz((mpz_ptr *)arg); break;
  case FLOAT_LAYOUT: migrate_floating((floating **)arg); break;
  case BOOL_LAYOUT:
  default: // mint
    break;
  }
}

static char *evacuate(char *scan_ptr, char **alloc_ptr) {
  auto *curr_block = (block *)scan_ptr;
  uint64_t const hdr = curr_block->h.hdr;
  uint16_t layout_int = layout_hdr(hdr);
  if (layout_int) {
    layout *layout_data = get_layout_data(layout_int);
    for (unsigned i = 0; i < layout_data->nargs; i++) {
      migrate_child(curr_block, layout_data->args, i, false, false);
    }
  }
  return move_ptr(scan_ptr, get_size(hdr, layout_int), *alloc_ptr);
}

// Contains the decision logic for collecting the old generation.
// For now, we collect the old generation every 50 young generation collections.
static bool should_collect_old_gen() {
#ifdef GC_DBG
  return true;
#else
  if (++num_collection_only_young == 50) {
    num_collection_only_young = 0;
    return true;
  }

  return false;
#endif
}

void migrate_roots();

void init_static_objects(void) {
  map m = map();
  list l = list();
  set s = set();
  set_kore_memory_functions_for_gmp();
}

void kore_collect(
    void **roots, uint8_t nroots, layoutitem *type_info, bool *are_block) {
  is_gc = true;
  collect_old = should_collect_old_gen();
  MEM_LOG("Starting garbage collection\n");
#ifdef GC_DBG
  if (!last_alloc_ptr) {
    last_alloc_ptr = youngspace_ptr();
  }
  char *current_alloc_ptr = *young_alloc_ptr();
#endif
  kore_alloc_swap(collect_old);
#ifdef GC_DBG
  for (int i = 0; i < 2048; i++) {
    numBytesLiveAtCollection[i] = 0;
  }
#endif
  char *previous_oldspace_alloc_ptr = *old_alloc_ptr();
  for (int i = 0; i < nroots; i++) {
    migrate_child(roots, type_info, i, true, are_block[i]);
  }
  migrate_roots();
  char *scan_ptr = youngspace_ptr();
  if (scan_ptr != *young_alloc_ptr()) {
    MEM_LOG("Evacuating young generation\n");
    while (scan_ptr) {
      scan_ptr = evacuate(scan_ptr, young_alloc_ptr());
    }
  }
  if (collect_old || !previous_oldspace_alloc_ptr) {
    scan_ptr = oldspace_ptr();
  } else {
    if (MEM_BLOCK_START(previous_oldspace_alloc_ptr + 1)
        == previous_oldspace_alloc_ptr) {
      // this means that the previous oldspace allocation pointer points to an
      // address that is megabyte-aligned. This can only happen if we have just
      // filled up a block but have not yet allocated the next block in the
      // sequence at the start of the collection cycle. This means that the
      // allocation pointer is invalid and does not actually point to the next
      // address that would have been allocated at, according to the logic of
      // kore_arena_alloc, which will have allocated a fresh memory block and put
      // the allocation at the start of it. Thus, we use move_ptr with a size
      // of zero to adjust and get the true address of the allocation.
      scan_ptr = move_ptr(previous_oldspace_alloc_ptr, 0, *old_alloc_ptr());
    } else {
      scan_ptr = previous_oldspace_alloc_ptr;
    }
  }
  if (scan_ptr != *old_alloc_ptr()) {
    MEM_LOG("Evacuating old generation\n");
    while (scan_ptr) {
      scan_ptr = evacuate(scan_ptr, old_alloc_ptr());
    }
  }
#ifdef GC_DBG
  ssize_t numBytesAllocedSinceLastCollection
      = ptr_diff(current_alloc_ptr, last_alloc_ptr);
  assert(numBytesAllocedSinceLastCollection >= 0);
  fwrite(&numBytesAllocedSinceLastCollection, sizeof(ssize_t), 1, stderr);
  last_alloc_ptr = *young_alloc_ptr();
  fwrite(
      numBytesLiveAtCollection, sizeof(numBytesLiveAtCollection[0]),
      sizeof(numBytesLiveAtCollection) / sizeof(numBytesLiveAtCollection[0]),
      stderr);
#endif
  MEM_LOG("Finishing garbage collection\n");
  is_gc = false;
  set_gc_threshold(youngspace_size());
}

void free_all_kore_mem() {
  kore_collect(nullptr, 0, nullptr, nullptr);
}

bool is_collection() {
  size_t threshold = get_gc_threshold();
  return youngspace_almost_full(threshold);
}

bool store_map_for_gc(void **roots, map *ptr) {
  if (get_arena_semispace_id_of_object(ptr) != ALWAYSGCSPACE_ID) {
    *roots = ptr;
    return false;
  }
  void *mem = kore_alloc(sizeof(blockheader) + sizeof(map));
  auto *hdr = (blockheader *)mem;
  std::string name = get_raw_symbol_name(kllvm::sort_category::Map) + "{}";
  *hdr = get_block_header_for_symbol(get_tag_for_symbol_name(name.c_str()));
  auto *child = (map *)(hdr + 1);
  *child = std::move(*ptr);
  *roots = mem;
  return true;
}

bool store_set_for_gc(void **roots, set *ptr) {
  if (get_arena_semispace_id_of_object(ptr) != ALWAYSGCSPACE_ID) {
    *roots = ptr;
    return false;
  }
  void *mem = kore_alloc(sizeof(blockheader) + sizeof(set));
  auto *hdr = (blockheader *)mem;
  std::string name = get_raw_symbol_name(kllvm::sort_category::Set) + "{}";
  *hdr = get_block_header_for_symbol(get_tag_for_symbol_name(name.c_str()));
  auto *child = (set *)(hdr + 1);
  *child = std::move(*ptr);
  *roots = mem;
  return true;
}

bool store_list_for_gc(void **roots, list *ptr) {
  if (get_arena_semispace_id_of_object(ptr) != ALWAYSGCSPACE_ID) {
    *roots = ptr;
    return false;
  }
  void *mem = kore_alloc(sizeof(blockheader) + sizeof(list));
  auto *hdr = (blockheader *)mem;
  std::string name = get_raw_symbol_name(kllvm::sort_category::List) + "{}";
  *hdr = get_block_header_for_symbol(get_tag_for_symbol_name(name.c_str()));
  auto *child = (list *)(hdr + 1);
  *child = std::move(*ptr);
  *roots = mem;
  return true;
}

bool store_rangemap_for_gc(void **roots, rangemap *ptr) {
  if (get_arena_semispace_id_of_object(ptr) != ALWAYSGCSPACE_ID) {
    *roots = ptr;
    return false;
  }
  void *mem = kore_alloc(sizeof(blockheader) + sizeof(rangemap));
  auto *hdr = (blockheader *)mem;
  std::string name = get_raw_symbol_name(kllvm::sort_category::RangeMap) + "{}";
  *hdr = get_block_header_for_symbol(get_tag_for_symbol_name(name.c_str()));
  auto *child = (rangemap *)(hdr + 1);
  *child = std::move(*ptr);
  *roots = mem;
  return true;
}

map *load_map_for_gc(void **roots, bool is_block) {
  void *mem = *roots;
  if (is_block) {
    return (map *)(((char *)mem) + sizeof(blockheader));
  }
  return (map *)mem;
}

set *load_set_for_gc(void **roots, bool is_block) {
  void *mem = *roots;
  if (is_block) {
    return (set *)(((char *)mem) + sizeof(blockheader));
  }
  return (set *)mem;
}

list *load_list_for_gc(void **roots, bool is_block) {
  void *mem = *roots;
  if (is_block) {
    return (list *)(((char *)mem) + sizeof(blockheader));
  }
  return (list *)mem;
}

rangemap *load_rangemap_for_gc(void **roots, bool is_block) {
  void *mem = *roots;
  if (is_block) {
    return (rangemap *)(((char *)mem) + sizeof(blockheader));
  }
  return (rangemap *)mem;
}
}
