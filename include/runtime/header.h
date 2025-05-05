#ifndef RUNTIME_HEADER_H
#define RUNTIME_HEADER_H

#include <cstddef>
#include <cstdint>
#include <limits>
#include <vector>

#include <fmt/printf.h>

#include "config/macros.h"
#include "runtime/fmt_error_handling.h"
#include "runtime/types.h"

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
  enum { Success = 0, Function, Fail } kind;

  char const *function{};
  char const *debug_name{};
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
bool hook_KEQUAL_lt(block *, block *);
bool hook_KEQUAL_eq(block *, block *);
bool during_gc(void);
size_t hash_k(block *);
void k_hash(block *, void *);
bool hash_enter(void);
void hash_exit(void);
#ifdef __MACH__
//
//	thread_local disabled for Apple
//
extern bool gc_enabled;
#else
extern thread_local constinit bool gc_enabled;
#endif
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
      auto *result = (string *)::operator new(size + sizeof(blockheader));
      init_with_len(result, size);
      result->h.hdr |= NOT_YOUNG_OBJECT_BIT;
      return result->data;
    }
    bool enabled = gc_enabled;
    gc_enabled = false;
    auto *result = (string *)kore_alloc_token(size + sizeof(blockheader));
    gc_enabled = enabled;
    init_with_len(result, size);
    return result->data;
  }

  static void deallocate(size_t size, void *data) {
    if (during_gc()) {
      ::operator delete((char *)data - sizeof(blockheader));
    }
  }
};

struct hash_block {
  size_t operator()(k_elem const &block) const noexcept {
    return hash_k(block);
  }
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
using map = immer::map<
    k_elem, k_elem, hash_block, std::equal_to<>, list::memory_policy>;
using set
    = immer::set<k_elem, hash_block, std::equal_to<>, list::memory_policy>;
using rangemap = rng_map::RangeMap<k_elem, k_elem>;

using mapiter = struct mapiter {
  map::iterator curr{};
  map *map_item{};
};

using setiter = struct setiter {
  set::iterator curr{};
  set *set_item{};
};

using SortList = list *;
using SortMap = map *;
using SortSet = set *;
using SortRange = block *;
using SortRangeMap = rangemap *;

void *construct_composite_pattern(uint32_t tag, std::vector<void *> &arguments);

extern "C" {

block *parse_configuration(char const *filename);
block *deserialize_configuration(char *, size_t);

void print_configuration(FILE *file, block *subject);
void print_statistics(FILE *file, uint64_t steps);
string *print_configuration_to_string(block *subject);
void print_sorted_configuration_to_file(
    FILE *file, block *subject, char const *sort);
void print_configuration_internal(
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
std::shared_ptr<kllvm::kore_pattern> term_to_kore_pattern(block *);
std::shared_ptr<kllvm::kore_pattern>
sorted_term_to_kore_pattern(block *, char const *);

// This function injects its argument into KItem before printing, using the sort
// argument as the source sort. Doing so allows the term to be pretty-printed
// using the existing recursion scheme code (and without manually inspecting the
// sort to see what printer we need to call if the term isn't an entire
// configuration).
string *debug_print_term(block *subject, char const *sort);

mpz_ptr move_int(mpz_t);

void serialize_configurations(
    FILE *file, std::unordered_set<block *, hash_block, k_eq> results);
void serialize_configuration(
    block *subject, char const *sort, char **data_out, size_t *size_out,
    bool emit_size, bool use_intern);
void serialize_configuration_to_file(
    FILE *file, block *subject, bool emit_size, bool use_intern);
void serialize_term_to_file(
    FILE *file, void *subject, char const *sort, bool use_intern,
    bool k_item_inj = false);
void serialize_raw_term_to_file(
    FILE *file, void *subject, char const *sort, bool use_intern);
void serialize_configuration_to_proof_trace(
    FILE *file, block *subject, uint32_t sort);
void serialize_term_to_proof_trace(
    FILE *file, void *subject, uint64_t block_header, uint64_t bits);
string *serialize_term_to_string_v2(block *subject, uint32_t sort);
block *deserialize_term_v2(char *, size_t);

// The following functions are called by the generated code and runtime code to
// ouput the proof trace data.
void write_hook_event_pre_to_proof_trace(
    void *proof_writer, char const *name, char const *pattern,
    char const *location_stack);
void write_hook_event_post_to_proof_trace(
    void *proof_writer, void *hook_result, uint64_t block_header,
    uint64_t bits);
void write_argument_to_proof_trace(
    void *proof_writer, void *arg, uint64_t block_header, uint64_t bits);
void write_rewrite_event_pre_to_proof_trace(
    void *proof_writer, uint64_t ordinal, uint64_t arity);
void write_variable_to_proof_trace(
    void *proof_writer, char const *name, void *var, uint64_t block_header,
    uint64_t bits);
void write_rewrite_event_post_to_proof_trace(
    void *proof_writer, void *config, uint64_t block_header, uint64_t bits);
void write_function_event_pre_to_proof_trace(
    void *proof_writer, char const *name, char const *location_stack);
void write_function_event_post_to_proof_trace(void *proof_writer);
void write_side_condition_event_pre_to_proof_trace(
    void *proof_writer, uint64_t ordinal, uint64_t arity);
void write_side_condition_event_post_to_proof_trace(
    void *proof_writer, uint64_t ordinal, bool side_cond_result);
void write_pattern_matching_failure_to_proof_trace(
    void *proof_writer, char const *function_name);
void write_function_exit_to_proof_trace(
    void *proof_writer, uint64_t ordinal, bool is_tail);
void write_configuration_to_proof_trace(
    void *proof_writer, block *config, bool is_initial);
void start_new_chunk_in_proof_trace(void *proof_writer);

// The following functions have to be generated at kompile time
// and linked with the interpreter.
uint32_t get_tag_for_symbol_name(char const *symbolname);
struct blockheader get_block_header_for_symbol(uint32_t tag);
bool is_symbol_a_function(uint32_t tag);
bool is_symbol_a_binder(uint32_t tag);
uint32_t get_symbol_arity(uint32_t tag);
void store_symbol_children(block *symbol, void *children[]);
void *evaluate_function_symbol(uint32_t tag, void *arguments[]);
void *get_token(char const *sortname, uint64_t len, char const *tokencontents);
layout *get_layout_data(uint16_t);
uint32_t get_injection_for_sort_of_tag(uint32_t tag);

bool hook_STRING_eq(SortString, SortString);

char const *get_symbol_name_for_tag(uint32_t tag);
char const *get_return_sort_for_tag(uint32_t tag);
char const **get_argument_sorts_for_tag(uint32_t tag);
uint32_t *
get_argument_sorts_for_tag_with_proof_trace_serialization(uint32_t tag);
char const *top_sort(void);

bool symbol_is_instantiation(uint32_t tag);

using visitor = struct {
  void (*visit_config)(writer *, block *, char const *, bool, void *);
  void (*visit_map)(
      writer *, map *, char const *, char const *, char const *, void *);
  void (*visit_list)(
      writer *, list *, char const *, char const *, char const *, void *);
  void (*visit_set)(
      writer *, set *, char const *, char const *, char const *, void *);
  void (*visit_int)(writer *, mpz_t, char const *, void *);
  void (*visit_float)(writer *, floating *, char const *, void *);
  void (*visit_bool)(writer *, bool, char const *, void *);
  void (*visit_string_buffer)(writer *, stringbuffer *, char const *, void *);
  void (*visit_m_int)(writer *, size_t *, size_t, char const *, void *);
  void (*visit_separator)(writer *, void *);
  void (*visit_range_map)(
      writer *, rangemap *, char const *, char const *, char const *, void *);
};

using serialize_to_proof_trace_visitor = struct {
  void (*visit_config)(writer *, block *, uint32_t, bool);
  void (*visit_map)(writer *, map *, uint32_t, uint32_t, uint32_t);
  void (*visit_list)(writer *, list *, uint32_t, uint32_t, uint32_t);
  void (*visit_set)(writer *, set *, uint32_t, uint32_t, uint32_t);
  void (*visit_int)(writer *, mpz_t, uint32_t);
  void (*visit_float)(writer *, floating *, uint32_t);
  void (*visit_bool)(writer *, bool, uint32_t);
  void (*visit_string_buffer)(writer *, stringbuffer *, uint32_t);
  void (*visit_m_int)(writer *, size_t *, size_t, uint32_t);
  void (*visit_range_map)(writer *, rangemap *, uint32_t, uint32_t, uint32_t);
};

void print_map(
    writer *, map *, char const *, char const *, char const *, void *);
void print_range_map(
    writer *, rangemap *, char const *, char const *, char const *, void *);
void print_set(
    writer *, set *, char const *, char const *, char const *, void *);
void print_list(
    writer *, list *, char const *, char const *, char const *, void *);
void visit_children(
    block *subject, writer *file, visitor *printer, void *state);
void visit_children_for_serialize_to_proof_trace(
    block *subject, writer *file, serialize_to_proof_trace_visitor *printer);

stringbuffer *hook_BUFFER_empty(void);
stringbuffer *hook_BUFFER_concat(stringbuffer *buf, string *s);
stringbuffer *
hook_BUFFER_concat_raw(stringbuffer *buf, char const *data, uint64_t n);
string *hook_BUFFER_toString(stringbuffer *buf);

size_t hook_SET_size_long(set *);

mpz_ptr hook_MINT_import(size_t *i, uint64_t bits, bool is_signed);

block *debruijnize(block *);
block *increment_debruijn(block *);
block *alpha_rename(block *);

setiter set_iterator(set *);
block *set_iterator_next(setiter *);
mapiter map_iterator(map *);
block *map_iterator_next(mapiter *);

extern uint32_t const FIRST_INJ_TAG, LAST_INJ_TAG;
bool is_injection(block *);
block *strip_injection(block *);
block *construct_k_item_inj(void *subject, char const *sort, bool raw_value);
block *construct_raw_term(void *subject, char const *sort, bool raw_value);

__attribute__((always_inline)) inline block *dot_k() {
  return leaf_block(get_tag_for_symbol_name("dotk{}"));
}
}

std::string float_to_string(floating const *);
void init_float2(floating *, std::string);

std::string int_to_string_in_base(mpz_t, uint64_t);
std::string int_to_string(mpz_t);
void print_value_of_type(
    std::ostream &os, std::string const &definition_path, void *value,
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

template <typename... Args>
void sfwrite(void const *ptr, size_t size, size_t nmemb, writer *file) {
  if (file->file) {
    fwrite(ptr, size, nmemb, file->file);
  } else {
    std::string output;
    output.resize(size * nmemb);
    memcpy(output.data(), ptr, size * nmemb);
    hook_BUFFER_concat_raw(file->buffer, output.data(), output.size());
  }
}

#endif // RUNTIME_HEADER_H
