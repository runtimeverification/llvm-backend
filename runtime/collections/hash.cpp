#include "runtime/header.h"

extern "C" {
void map_hash(map *, void *);
void rangemap_hash(rangemap *, void *);
void list_hash(list *, void *);
void set_hash(set *, void *);
void int_hash(mpz_ptr, void *);
void float_hash(floating *, void *);

static thread_local uint32_t hash_length;
static thread_local uint32_t hash_depth;
static constexpr uint32_t hash_threshold = 5;
static constexpr uint32_t hash_length_threshold = 1024;

__attribute__((always_inline)) void add_hash8(void *h, uint8_t data) {
  auto *hash = (size_t *)h;
  *hash = ((*hash) ^ ((size_t)data)) * 1099511628211UL;
  hash_length++;
}

__attribute__((always_inline)) void add_hash32(void *h, uint32_t data) {
  auto *buf = (uint8_t *)&data;
  add_hash8(h, buf[0]);
  add_hash8(h, buf[1]);
  add_hash8(h, buf[2]);
  add_hash8(h, buf[3]);
}

__attribute__((always_inline)) void add_hash64(void *h, uint64_t data) {
  auto *buf = (uint8_t *)&data;
  add_hash8(h, buf[0]);
  add_hash8(h, buf[1]);
  add_hash8(h, buf[2]);
  add_hash8(h, buf[3]);
  add_hash8(h, buf[4]);
  add_hash8(h, buf[5]);
  add_hash8(h, buf[6]);
  add_hash8(h, buf[7]);
}

__attribute__((always_inline)) void
add_hash_str(void *h, char *data, size_t len) {
  if (len + hash_length > hash_length_threshold) {
    len = hash_length_threshold - hash_length;
  }
  for (size_t i = 0; i < len; i++) {
    add_hash8(h, data[i]);
  }
}

size_t hash_k(block *term) {
  size_t hash = 14695981039346656037ULL;
  hash_length = 0;
  k_hash(term, &hash);

  return hash;
}

bool hash_enter() {
  bool result
      = hash_depth < hash_threshold && hash_length < hash_length_threshold;
  hash_depth = hash_depth + 1;
  return result;
}

void hash_exit() {
  hash_depth = hash_depth - 1;
}

void k_hash(block *arg, void *h) {
  if (hash_enter()) {
    auto argintptr = (uint64_t)arg;
    if (is_leaf_block(arg)) {
      add_hash64(h, argintptr);
    } else {
      uint64_t arghdrcanon = arg->h.hdr & HDR_MASK;
      if (uint16_t arglayout = get_layout(arg)) {
        add_hash64(h, arghdrcanon);
        layout *layout_ptr = get_layout_data(arglayout);
        uint8_t length = layout_ptr->nargs;
        for (uint8_t i = 0; i < length; i++) {
          uint64_t offset = layout_ptr->args[i].offset;
          uint16_t cat = layout_ptr->args[i].cat;
          switch (cat) {
          case MAP_LAYOUT: {
            map *mapptr = (map *)(argintptr + offset);
            map_hash(mapptr, h);
            break;
          }
          case RANGEMAP_LAYOUT: {
            auto *rangemapptr = (rangemap *)(argintptr + offset);
            rangemap_hash(rangemapptr, h);
            break;
          }
          case LIST_LAYOUT: {
            list *listptr = (list *)(argintptr + offset);
            list_hash(listptr, h);
            break;
          }
          case SET_LAYOUT: {
            set *setptr = (set *)(argintptr + offset);
            set_hash(setptr, h);
            break;
          }
          case INT_LAYOUT: {
            auto *intptrptr = (mpz_ptr *)(argintptr + offset);
            int_hash(*intptrptr, h);
            break;
          }
          case FLOAT_LAYOUT: {
            auto **floatptrptr = (floating **)(argintptr + offset);
            float_hash(*floatptrptr, h);
            break;
          }
          case BOOL_LAYOUT: {
            bool *boolptr = (bool *)(argintptr + offset);
            add_hash8(h, *boolptr);
            break;
          }
          case SYMBOL_LAYOUT:
          case VARIABLE_LAYOUT: {
            auto **childptrptr = (block **)(argintptr + offset);
            k_hash(*childptrptr, h);
            break;
          }
          case MINT_LAYOUT + 32: {
            auto *intptr = (uint32_t *)(argintptr + offset);
            add_hash32(h, *intptr);
            break;
          }
          case MINT_LAYOUT + 64: {
            auto *intptr = (uint64_t *)(argintptr + offset);
            add_hash64(h, *intptr);
            break;
          }
          case MINT_LAYOUT + 160: {
            auto *intptr = (uint64_t *)(argintptr + offset);
            add_hash64(h, intptr[0]);
            add_hash64(h, intptr[1]);
            add_hash64(h, intptr[2] & UINT32_MAX);
            break;
          }
          case MINT_LAYOUT + 256: {
            auto *intptr = (uint64_t *)(argintptr + offset);
            add_hash64(h, intptr[0]);
            add_hash64(h, intptr[1]);
            add_hash64(h, intptr[2]);
            add_hash64(h, intptr[3]);
            break;
          }
          default: abort();
          }
        }
      } else {
        auto *str = (string *)arg;
        add_hash_str(h, str->data, len(arg));
      }
    }
  }
  hash_exit();
}
}
