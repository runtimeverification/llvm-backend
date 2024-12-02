#ifndef RUNTIME_TYPES_H
#define RUNTIME_TYPES_H

#include <cstdint>
#include <gmp.h>
#include <mpfr.h>

#include "config/macros.h"
#include "runtime/alloc.h"

extern "C" {
// llvm: blockheader = type { i64 }
using blockheader = struct blockheader {
  uint64_t hdr;
};

// A value b of type block* is either a constant or a block.
// if (((uintptr_t)b) & 3) == 3, then it is a bound variable and
// ((uintptr_t)b) >> 32 is the debruijn index. If ((uintptr_t)b) & 3 == 1)
// then it is a symbol with 0 arguments and ((uintptr_t)b) >> 32 is the tag
// of the symbol. Otherwise, if ((uintptr_t)b) & 1 == 0 then it is a pointer to
// a block.
// llvm: block = type { %blockheader, [0 x i64 *] }
using block = struct block {
  blockheader h;
  uint64_t *children[];
};

// llvm: string = type { %blockheader, [0 x i8] }
using string = struct string {
  blockheader h;
  char data[];
};

// llvm: stringbuffer = type { i64, i64, %string* }
using stringbuffer = struct stringbuffer {
  blockheader h;
  uint64_t strlen;
  string *contents;
};

using mpz_hdr = struct mpz_hdr {
  blockheader h;
  mpz_t i;
};

using floating = struct floating {
  uint64_t exp; // number of bits in exponent range
  mpfr_t f;
};

using floating_hdr = struct floating_hdr {
  blockheader h;
  floating f;
};

using layoutitem = struct layoutitem {
  uint64_t offset;
  uint16_t cat;
};

using layout = struct layout {
  uint8_t nargs;
  layoutitem *args;
};

using writer = struct {
  FILE *file;
  stringbuffer *buffer;
};

using SortFloat = floating *;
using SortInt = mpz_ptr;
using SortString = string *;
using SortBytes = string *;
using SortStringBuffer = stringbuffer *;
using SortK = block *;
using SortKItem = block *;
using SortIOInt = block *;
using SortIOFile = block *;
using SortIOString = block *;
using SortJSON = block *;
using SortEndianness = block *;
using SortSignedness = block *;
using SortFFIType = block *;
}

__attribute__((always_inline)) constexpr uint64_t len_hdr(uint64_t hdr) {
  return hdr & LENGTH_MASK;
}

template <typename T>
__attribute__((always_inline)) constexpr uint64_t len(T const *s) {
  return len_hdr(s->h.hdr);
}

template <typename T>
__attribute__((always_inline)) constexpr void init_with_len(T *s, uint64_t l) {
  s->h.hdr = l | (l > BLOCK_SIZE - sizeof(char *) ? NOT_YOUNG_OBJECT_BIT : 0);
}

__attribute__((always_inline)) constexpr uint64_t size_hdr(uint64_t hdr) {
  return ((hdr >> 32) & 0xff) * 8;
}

__attribute__((always_inline)) constexpr uint64_t layout_hdr(uint64_t hdr) {
  return hdr >> LAYOUT_OFFSET;
}

template <typename T>
__attribute__((always_inline)) constexpr uint64_t get_layout(T const *s) {
  return layout_hdr((s)->h.hdr);
}

__attribute__((always_inline)) constexpr uint64_t tag_hdr(uint64_t hdr) {
  return hdr & TAG_MASK;
}

template <typename T>
__attribute__((always_inline)) constexpr uint64_t tag(T const *s) {
  return tag_hdr(s->h.hdr);
}

__attribute__((always_inline)) constexpr bool
is_in_young_gen_hdr(uint64_t hdr) {
  return !(hdr & NOT_YOUNG_OBJECT_BIT);
}

__attribute__((always_inline)) constexpr bool is_in_old_gen_hdr(uint64_t hdr) {
  return (hdr & NOT_YOUNG_OBJECT_BIT) && (hdr & AGE_MASK);
}

template <typename T>
__attribute__((always_inline)) constexpr void reset_gc(T *s) {
  constexpr auto all_gc_mask = NOT_YOUNG_OBJECT_BIT | AGE_MASK | FWD_PTR_BIT;
  s->h.hdr = s->h.hdr & ~all_gc_mask;
}

__attribute__((always_inline)) inline block *leaf_block(uint64_t tag) {
  auto value = uintptr_t{(tag << 32) | 1};
  return reinterpret_cast<block *>(value);
}

__attribute__((always_inline)) inline block *variable_block(uint64_t tag) {
  auto value = uintptr_t{(tag << 32) | 3};
  return reinterpret_cast<block *>(value);
}

template <typename T>
__attribute__((always_inline)) inline bool is_leaf_block(T const *b) {
  return reinterpret_cast<uintptr_t>(b) & 1;
}

template <typename T>
__attribute__((always_inline)) inline bool is_variable_block(T const *b) {
  return (reinterpret_cast<uintptr_t>(b) & 3) == 3;
}

template <typename T>
__attribute__((always_inline)) constexpr bool is_heap_block(T const *s) {
  return is_in_young_gen_hdr(s->h.hdr) || is_in_old_gen_hdr(s->h.hdr);
}

#endif
