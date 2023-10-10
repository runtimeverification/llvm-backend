#ifndef RUNTIME_HEADER_H
#define RUNTIME_HEADER_H

#include <cstddef>
#include <cstdint>
#include <limits>
#include <vector>

#include <gmp.h>
#include <mpfr.h>

#include "config/macros.h"
#include "runtime/alloc.h"
#include "runtime/fmt_error_handling.h"

#ifndef IMMER_TAGGED_NODE
#define IMMER_TAGGED_NODE 0
#endif
#include <immer/flex_vector.hpp>
#include <immer/map.hpp>
#include <immer/set.hpp>
#include <kllvm/ast/AST.h>
#include <runtime/collections/rangemap.h>
#include <unordered_set>

struct MatchLog {
  enum { SUCCESS = 0, FUNCTION, FAIL } kind;

  char *function;
  char *debugName;
  void *result;
  std::vector<void *> args;

  char *pattern;
  void *subject;
  char *sort;
};

// the actual length is equal to the block header with the gc bits masked out.

#define struct_base(struct_type, member_name, member_addr)                     \
  ((struct_type *)((char *)(member_addr)-offsetof(struct_type, member_name)))

extern "C" {
// llvm: blockheader = type { i64 }
typedef struct blockheader {
  uint64_t hdr;
} blockheader;

// A value b of type block* is either a constant or a block.
// if (((uintptr_t)b) & 3) == 3, then it is a bound variable and
// ((uintptr_t)b) >> 32 is the debruijn index. If ((uintptr_t)b) & 3 == 1)
// then it is a symbol with 0 arguments and ((uintptr_t)b) >> 32 is the tag
// of the symbol. Otherwise, if ((uintptr_t)b) & 1 == 0 then it is a pointer to
// a block.
// llvm: block = type { %blockheader, [0 x i64 *] }
typedef struct block {
  blockheader h;
  uint64_t *children[];
} block;

// llvm: string = type { %blockheader, [0 x i8] }
typedef struct string {
  blockheader h;
  char data[];
} string;

// llvm: stringbuffer = type { i64, i64, %string* }
typedef struct stringbuffer {
  blockheader h;
  uint64_t strlen;
  string *contents;
} stringbuffer;

typedef struct mpz_hdr {
  blockheader h;
  mpz_t i;
} mpz_hdr;

typedef struct floating {
  uint64_t exp; // number of bits in exponent range
  mpfr_t f;
} floating;

typedef struct floating_hdr {
  blockheader h;
  floating f;
} floating_hdr;

typedef struct layoutitem {
  uint64_t offset;
  uint16_t cat;
} layoutitem;

typedef struct layout {
  uint8_t nargs;
  layoutitem *args;
} layout;

typedef struct {
  FILE *file;
  stringbuffer *buffer;
} writer;

bool hook_KEQUAL_lt(block *, block *);
bool hook_KEQUAL_eq(block *, block *);
bool during_gc(void);
size_t hash_k(block *);
void k_hash(block *, void *);
bool hash_enter(void);
void hash_exit(void);
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

class KElem {
public:
  KElem() { this->elem = NULL; }

  KElem(block *elem) { this->elem = elem; }

  bool operator==(const KElem &other) const {
    return hook_KEQUAL_eq(this->elem, other.elem);
  }

  bool operator!=(const KElem &other) const { return !(*this == other); }

  bool operator<(const KElem &other) const {
    return hook_KEQUAL_lt(this->elem, other.elem);
  }

  bool operator>(const KElem &other) const { return other < *this; }

  bool operator<=(const KElem &other) const { return !(other < *this); }

  bool operator>=(const KElem &other) const { return !(*this < other); }

  operator block *() const { return elem; }

  block *elem;
};

struct kore_alloc_heap {

  template <typename... Tags>
  static void *allocate(size_t size, Tags...) {
    if (during_gc()) {
      return ::operator new(size);
    } else {
      string *result = (string *)koreAllocToken(size + sizeof(blockheader));
      init_with_len(result, size);
      return result->data;
    }
  }

  static void deallocate(size_t size, void *data) {
    if (during_gc()) {
      ::operator delete(data);
    }
  }
};

struct HashBlock {
  size_t operator()(const KElem &block) const noexcept { return hash_k(block); }
};

struct KEq {
  bool operator()(block *const &lhs, block *const &rhs) const {
    return hook_KEQUAL_eq(lhs, rhs);
  }
};

using list = immer::flex_vector<
    KElem, immer::memory_policy<
               immer::heap_policy<kore_alloc_heap>, immer::no_refcount_policy,
               immer::no_lock_policy>>;
using map = immer::map<
    KElem, KElem, HashBlock, std::equal_to<KElem>, list::memory_policy>;
using set
    = immer::set<KElem, HashBlock, std::equal_to<KElem>, list::memory_policy>;
using rangemap = rng_map::RangeMap<KElem, KElem>;

typedef struct mapiter {
  map::iterator curr;
  map *map_item;
} mapiter;

typedef struct setiter {
  set::iterator curr;
  set *set_item;
} setiter;

typedef floating *SortFloat;
typedef mpz_ptr SortInt;
typedef string *SortString;
typedef string *SortBytes;
typedef stringbuffer *SortStringBuffer;
typedef block *SortK;
typedef block *SortKItem;
typedef block *SortIOInt;
typedef block *SortIOFile;
typedef block *SortIOString;
typedef block *SortJSON;
typedef block *SortEndianness;
typedef block *SortSignedness;
typedef block *SortFFIType;
typedef list *SortList;
typedef map *SortMap;
typedef set *SortSet;
typedef block *SortRange;
typedef rangemap *SortRangeMap;

void *constructCompositePattern(uint32_t tag, std::vector<void *> &arguments);

extern "C" {

block *parseConfiguration(const char *filename);
block *deserializeConfiguration(char *, size_t);

void printConfiguration(const char *filename, block *subject);
void printStatistics(const char *filename, uint64_t steps);
string *printConfigurationToString(block *subject);
void printConfigurationToFile(FILE *, block *subject);
void printSortedConfigurationToFile(
    FILE *file, block *subject, char const *sort);
void printConfigurationInternal(
    writer *file, block *subject, const char *sort, bool, void *);

// Returns a shared_ptr to a KOREPattern. The shared_ptr managess the lifetime
// of the pattern and the pattern will be deallocated when the last reference
// to the pattern is destroyed. There may exist references beyond the ones that
// are provided to the user via this method, so destroying all values returned
// by this method may not fully deallocate the pattern. However, it will still
// be deallocated when the last reference is eventually destroyed.
// If you need to have access to a function that returns a type with C linkage,
// you can use the C bindings, which wrap the return value of this method in
// a POD struct.
std::shared_ptr<kllvm::KOREPattern> termToKorePattern(block *);

// This function injects its argument into KItem before printing, using the sort
// argument as the source sort. Doing so allows the term to be pretty-printed
// using the existing recursion scheme code (and without manually inspecting the
// sort to see what printer we need to call if the term isn't an entire
// configuration).
string *debug_print_term(block *subject, char const *sort);

mpz_ptr move_int(mpz_t);

void serializeConfigurations(
    const char *filename, std::unordered_set<block *, HashBlock, KEq> results);
void serializeConfiguration(
    block *subject, char const *sort, char **data_out, size_t *size_out,
    bool emit_size);
void serializeConfigurationToFile(
    const char *filename, block *subject, bool emit_size);
void writeUInt64ToFile(const char *filename, uint64_t i);
void serializeTermToFile(
    const char *filename, block *subject, const char *sort);
void serializeRawTermToFile(
    const char *filename, void *subject, const char *sort);
void printVariableToFile(const char *filename, const char *varname);

// The following functions have to be generated at kompile time
// and linked with the interpreter.
uint32_t getTagForSymbolName(const char *symbolname);
struct blockheader getBlockHeaderForSymbol(uint32_t tag);
bool isSymbolAFunction(uint32_t tag);
bool isSymbolABinder(uint32_t tag);
uint32_t getSymbolArity(uint32_t tag);
void storeSymbolChildren(block *symbol, void *children[]);
void *evaluateFunctionSymbol(uint32_t tag, void *arguments[]);
void *getToken(const char *sortname, uint64_t len, const char *tokencontents);
layout *getLayoutData(uint16_t);
uint32_t getInjectionForSortOfTag(uint32_t tag);

bool hook_STRING_eq(SortString, SortString);

const char *getSymbolNameForTag(uint32_t tag);
const char *topSort(void);

typedef struct {
  void (*visitConfig)(writer *, block *, const char *, bool, void *);
  void (*visitMap)(
      writer *, map *, const char *, const char *, const char *, void *);
  void (*visitList)(
      writer *, list *, const char *, const char *, const char *, void *);
  void (*visitSet)(
      writer *, set *, const char *, const char *, const char *, void *);
  void (*visitInt)(writer *, mpz_t, const char *, void *);
  void (*visitFloat)(writer *, floating *, const char *, void *);
  void (*visitBool)(writer *, bool, const char *, void *);
  void (*visitStringBuffer)(writer *, stringbuffer *, const char *, void *);
  void (*visitMInt)(writer *, size_t *, size_t, const char *, void *);
  void (*visitSeparator)(writer *, void *);
  void (*visitRangeMap)(
      writer *, rangemap *, const char *, const char *, const char *, void *);
} visitor;

void printMap(
    writer *, map *, const char *, const char *, const char *, void *);
void printRangeMap(
    writer *, rangemap *, const char *, const char *, const char *, void *);
void printSet(
    writer *, set *, const char *, const char *, const char *, void *);
void printList(
    writer *, list *, const char *, const char *, const char *, void *);
void visitChildren(block *subject, writer *file, visitor *printer, void *state);

void sfprintf(writer *, const char *, ...);

stringbuffer *hook_BUFFER_empty(void);
stringbuffer *hook_BUFFER_concat(stringbuffer *buf, string *s);
stringbuffer *
hook_BUFFER_concat_raw(stringbuffer *buf, char const *data, uint64_t n);
string *hook_BUFFER_toString(stringbuffer *buf);

size_t hook_SET_size_long(set *);

mpz_ptr hook_MINT_import(size_t *i, uint64_t bits, bool isSigned);

block *dot_k();

block *debruijnize(block *);
block *incrementDebruijn(block *);
block *alphaRename(block *);

setiter set_iterator(set *);
block *set_iterator_next(setiter *);
mapiter map_iterator(map *);
block *map_iterator_next(mapiter *);

extern const uint32_t first_inj_tag, last_inj_tag;
bool is_injection(block *);
block *strip_injection(block *);
block *constructKItemInj(void *subject, const char *sort, bool raw_value);
}

std::string floatToString(const floating *);
void init_float2(floating *, std::string);

std::string intToStringInBase(mpz_t, uint64_t);
std::string intToString(mpz_t);
void printValueOfType(
    std::ostream &os, std::string definitionPath, void *, std::string);
#endif // RUNTIME_HEADER_H
