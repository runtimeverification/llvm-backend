#ifndef RUNTIME_HEADER_H
#define RUNTIME_HEADER_H

#include <cstddef>
#include <cstdint>
#include <limits>
#include <vector>

#include <gmp.h>
#include <mpfr.h>

#include <fmt/printf.h>

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

struct match_log {
  enum { SUCCESS = 0, FUNCTION, FAIL } kind;

  char const *function{};
  char const *debugName{};
  void *result{};
  std::vector<void *> args{};

  char const *pattern{};
  void *subject{};
  char const *sort{};
};

// the actual length is equal to the block header with the gc bits masked out.

#define STRUCT_BASE(struct_type, member_name, member_addr)                     \
  ((struct_type *)((char *)(member_addr)-offsetof(struct_type, member_name)))

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

class k_elem {
public:
  k_elem()
      : elem(nullptr) { }

  k_elem(block *elem)
      : elem(elem) { }

  bool operator==(k_elem const &other) const {
    return hook_KEQUAL_eq(this->elem, other.elem);
  }

  bool operator!=(k_elem const &other) const { return !(*this == other); }

  bool operator<(k_elem const &other) const {
    return hook_KEQUAL_lt(this->elem, other.elem);
  }

  bool operator>(k_elem const &other) const { return other < *this; }

  bool operator<=(k_elem const &other) const { return !(other < *this); }

  bool operator>=(k_elem const &other) const { return !(*this < other); }

  operator block *() const { return elem; }

  block *elem;
};

struct kore_alloc_heap {

  template <typename... Tags>
  static void *allocate(size_t size, Tags...) {
    if (during_gc()) {
      return ::operator new(size);
    }
    auto *result = (string *)koreAllocToken(size + sizeof(blockheader));
    init_with_len(result, size);
    return result->data;
  }

  static void deallocate(size_t size, void *data) {
    if (during_gc()) {
      ::operator delete(data);
    }
  }
};

struct hash_block {
  size_t operator()(k_elem const &block) const noexcept { return hash_k(block); }
};

struct k_eq {
  bool operator()(block *const &lhs, block *const &rhs) const {
    return hook_KEQUAL_eq(lhs, rhs);
  }
};

using list = immer::flex_vector<
    k_elem, immer::memory_policy<
               immer::heap_policy<kore_alloc_heap>, immer::no_refcount_policy,
               immer::no_lock_policy>>;
using map
    = immer::map<k_elem, k_elem, hash_block, std::equal_to<>, list::memory_policy>;
using set = immer::set<k_elem, hash_block, std::equal_to<>, list::memory_policy>;
using rangemap = rng_map::RangeMap<k_elem, k_elem>;

using mapiter = struct mapiter {
  map::iterator curr{};
  map *map_item{};
};

using setiter = struct setiter {
  set::iterator curr{};
  set *set_item{};
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
using SortList = list *;
using SortMap = map *;
using SortSet = set *;
using SortRange = block *;
using SortRangeMap = rangemap *;

void *constructCompositePattern(uint32_t tag, std::vector<void *> &arguments);

extern "C" {

block *parseConfiguration(char const *filename);
block *deserializeConfiguration(char *, size_t);

void printConfiguration(FILE *file, block *subject);
void printStatistics(FILE *file, uint64_t steps);
string *printConfigurationToString(block *subject);
void printSortedConfigurationToFile(
    FILE *file, block *subject, char const *sort);
void printConfigurationInternal(
    writer *file, block *subject, char const *sort, bool, void *);

// Returns a shared_ptr to a kore_pattern. The shared_ptr managess the lifetime
// of the pattern and the pattern will be deallocated when the last reference
// to the pattern is destroyed. There may exist references beyond the ones that
// are provided to the user via this method, so destroying all values returned
// by this method may not fully deallocate the pattern. However, it will still
// be deallocated when the last reference is eventually destroyed.
// If you need to have access to a function that returns a type with C linkage,
// you can use the C bindings, which wrap the return value of this method in
// a POD struct.
std::shared_ptr<kllvm::kore_pattern> termToKorePattern(block *);
std::shared_ptr<kllvm::kore_pattern>
sortedTermToKorePattern(block *, char const *);

// This function injects its argument into KItem before printing, using the sort
// argument as the source sort. Doing so allows the term to be pretty-printed
// using the existing recursion scheme code (and without manually inspecting the
// sort to see what printer we need to call if the term isn't an entire
// configuration).
string *debug_print_term(block *subject, char const *sort);

mpz_ptr move_int(mpz_t);

void serializeConfigurations(
    FILE *file, std::unordered_set<block *, hash_block, k_eq> results);
void serializeConfiguration(
    block *subject, char const *sort, char **data_out, size_t *size_out,
    bool emit_size, bool use_intern);
void serializeConfigurationToFile(
    FILE *file, block *subject, bool emit_size, bool use_intern);
void writeUInt64ToFile(FILE *file, uint64_t i);
void serializeTermToFile(
    FILE *file, block *subject, char const *sort, bool use_intern);
void serializeRawTermToFile(
    FILE *file, void *subject, char const *sort, bool use_intern);
void printVariableToFile(FILE *file, char const *varname);

// The following functions have to be generated at kompile time
// and linked with the interpreter.
uint32_t getTagForSymbolName(char const *symbolname);
struct blockheader getBlockHeaderForSymbol(uint32_t tag);
bool isSymbolAFunction(uint32_t tag);
bool isSymbolABinder(uint32_t tag);
uint32_t getSymbolArity(uint32_t tag);
void storeSymbolChildren(block *symbol, void *children[]);
void *evaluateFunctionSymbol(uint32_t tag, void *arguments[]);
void *getToken(char const *sortname, uint64_t len, char const *tokencontents);
layout *getLayoutData(uint16_t);
uint32_t getInjectionForSortOfTag(uint32_t tag);

bool hook_STRING_eq(SortString, SortString);

char const *getSymbolNameForTag(uint32_t tag);
char const *getReturnSortForTag(uint32_t tag);
char const **getArgumentSortsForTag(uint32_t tag);
char const *topSort(void);

bool symbolIsInstantiation(uint32_t tag);

using visitor = struct {
  void (*visitConfig)(writer *, block *, char const *, bool, void *);
  void (*visitMap)(
      writer *, map *, char const *, char const *, char const *, void *);
  void (*visitList)(
      writer *, list *, char const *, char const *, char const *, void *);
  void (*visitSet)(
      writer *, set *, char const *, char const *, char const *, void *);
  void (*visitInt)(writer *, mpz_t, char const *, void *);
  void (*visitFloat)(writer *, floating *, char const *, void *);
  void (*visitBool)(writer *, bool, char const *, void *);
  void (*visitStringBuffer)(writer *, stringbuffer *, char const *, void *);
  void (*visitMInt)(writer *, size_t *, size_t, char const *, void *);
  void (*visitSeparator)(writer *, void *);
  void (*visitRangeMap)(
      writer *, rangemap *, char const *, char const *, char const *, void *);
};

void printMap(
    writer *, map *, char const *, char const *, char const *, void *);
void printRangeMap(
    writer *, rangemap *, char const *, char const *, char const *, void *);
void printSet(
    writer *, set *, char const *, char const *, char const *, void *);
void printList(
    writer *, list *, char const *, char const *, char const *, void *);
void visitChildren(block *subject, writer *file, visitor *printer, void *state);

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

extern uint32_t const FIRST_INJ_TAG, LAST_INJ_TAG;
bool is_injection(block *);
block *strip_injection(block *);
block *constructKItemInj(void *subject, char const *sort, bool raw_value);
block *constructRawTerm(void *subject, char const *sort, bool raw_value);
}

std::string floatToString(floating const *);
void init_float2(floating *, std::string);

std::string intToStringInBase(mpz_t, uint64_t);
std::string intToString(mpz_t);
void printValueOfType(
    std::ostream &os, std::string const &definitionPath, void *value,
    std::string const &type);

template <typename... Args>
void sfprintf(writer *file, char const *fmt, Args &&...args) {
  if (file->file) {
    fmt::fprintf(file->file, fmt, args...);
  } else {
    auto str = fmt::sprintf(fmt, args...);
    hook_BUFFER_concat_raw(file->buffer, str.data(), str.size());
  }
}

#endif // RUNTIME_HEADER_H
