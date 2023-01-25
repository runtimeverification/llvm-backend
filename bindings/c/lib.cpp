#include <iostream>
#include <memory>

#include <kllvm/ast/AST.h>

#include <kllvm-c/kllvm-c.h>

#include <runtime/arena.h>

// This header needs to be included last because it pollutes a number of macro
// definitions into the global namespace.
#include <runtime/header.h>

// Internal implementation details
namespace {

template <typename OS>
char *get_c_string(OS const &);

kore_pattern *kore_string_pattern_new_internal(std::string const &);

kore_pattern *
kore_pattern_new_token_internal(kore_pattern *, kore_sort const *);

} // namespace

/*
 * These declarations are internal to the backend and aren't exposed explicitly
 * through any header files, so we pull them in manually here.
 */
void *constructInitialConfiguration(const kllvm::KOREPattern *);
extern "C" block *take_steps(int64_t, block *);

extern "C" {

/* Completed types */

struct kore_pattern {
  std::unique_ptr<kllvm::KOREPattern> ptr_;
};

struct kore_sort {
  std::shared_ptr<kllvm::KORESort> ptr_;
};

struct kore_symbol {
  std::unique_ptr<kllvm::KORESymbol> ptr_;
};

/* KOREPattern */

char *kore_pattern_dump(kore_pattern const *pat) {
  auto os = std::ostringstream{};
  pat->ptr_->print(os);
  return get_c_string(os);
}

void kore_pattern_free(kore_pattern const *pat) {
  delete pat;
}

kore_pattern *kore_pattern_new_token(char const *value, kore_sort const *sort) {
  auto pat = kore_string_pattern_new(value);
  return kore_pattern_new_token_internal(pat, sort);
}

kore_pattern *kore_pattern_new_token_with_len(
    char const *value, size_t len, kore_sort const *sort) {
  auto pat = kore_string_pattern_new_with_len(value, len);
  return kore_pattern_new_token_internal(pat, sort);
}

kore_pattern *kore_pattern_new_injection(
    kore_pattern *term, kore_sort const *from, kore_sort const *to) {
  auto inj_sym = kore_symbol_new("inj");
  kore_symbol_add_formal_argument(inj_sym, from);
  kore_symbol_add_formal_argument(inj_sym, to);

  auto inj = kore_composite_pattern_from_symbol(inj_sym);
  kore_composite_pattern_add_argument(inj, term);
  return inj;
}

kore_pattern *kore_pattern_make_interpreter_input(kore_pattern *pgm) {
  auto config_sort = kore_composite_sort_new("SortKConfigVar");
  auto kitem_sort = kore_composite_sort_new("SortKItem");

  auto key = kore_pattern_new_injection(
      kore_pattern_new_token("$PGM", config_sort), config_sort, kitem_sort);

  auto map_item = kore_composite_pattern_new("Lbl'UndsPipe'-'-GT-Unds'");
  kore_composite_pattern_add_argument(map_item, key);
  kore_composite_pattern_add_argument(map_item, pgm);

  auto map_unit = kore_composite_pattern_new("Lbl'Stop'Map");

  auto map_concat = kore_composite_pattern_new("Lbl'Unds'Map'Unds'");
  kore_composite_pattern_add_argument(map_concat, map_unit);
  kore_composite_pattern_add_argument(map_concat, map_item);

  auto top_cell = kore_composite_pattern_new("LblinitGeneratedTopCell");
  kore_composite_pattern_add_argument(top_cell, map_concat);

  return top_cell;
}

block *kore_pattern_construct(kore_pattern const *pat) {
  return static_cast<block *>(constructInitialConfiguration(pat->ptr_.get()));
}

char *kore_block_dump(block *term) {
  auto hooked_str = printConfigurationToString(term)->data;
  auto len = std::strlen(hooked_str);

  auto new_str = static_cast<char *>(malloc(len * sizeof(char)));
  std::strncpy(new_str, hooked_str, len);
  new_str[len] = '\0';

  return new_str;
}

bool kore_block_get_bool(block *term) {
  assert((((uintptr_t)term) & 1) == 0);
  return (bool)(term->children[0]);
}

bool kore_simplify_bool(kore_pattern *pattern) {
  auto bool_sort = kore_composite_sort_new("SortBool");
  auto kitem_sort = kore_composite_sort_new("SortKItem");

  auto inj = kore_pattern_new_injection(pattern, bool_sort, kitem_sort);

  return kore_block_get_bool(kore_pattern_construct(inj));
}

void kore_simplify(
    kore_pattern *pattern, kore_sort *sort, char **data_out, size_t *size_out) {
  auto kitem_sort = kore_composite_sort_new("SortKItem");
  auto kitem_sort_str = kore_sort_dump(kitem_sort);

  auto block = [&] {
    if (kore_sort_is_kitem(sort) || kore_sort_is_k(sort)) {
      return kore_pattern_construct(pattern);
    } else {
      auto inj = kore_pattern_new_injection(pattern, sort, kitem_sort);
      return kore_pattern_construct(inj);
    }
  }();

  serializeConfiguration(block, kitem_sort_str, data_out, size_out);
  free(kitem_sort_str);
}

/* KORECompositePattern */

kore_pattern *kore_composite_pattern_new(char const *name) {
  auto pat = new kore_pattern;
  pat->ptr_ = kllvm::KORECompositePattern::Create(std::string(name));
  return pat;
}

kore_pattern *kore_composite_pattern_from_symbol(kore_symbol *sym) {
  auto pat = new kore_pattern;
  pat->ptr_ = kllvm::KORECompositePattern::Create(std::move(sym->ptr_));
  return pat;
}

void kore_composite_pattern_add_argument(kore_pattern *pat, kore_pattern *arg) {
  if (auto cast_ptr
      = dynamic_cast<kllvm::KORECompositePattern *>(pat->ptr_.get())) {
    cast_ptr->addArgument(
        std::shared_ptr<kllvm::KOREPattern>(arg->ptr_.release()));
  } else {
    abort();
  }
}

/* KOREStringPattern */

kore_pattern *kore_string_pattern_new(char const *contents) {
  return kore_string_pattern_new_internal(std::string(contents));
}

kore_pattern *
kore_string_pattern_new_with_len(char const *contents, size_t len) {
  return kore_string_pattern_new_internal(std::string(contents, len));
}

/* KORESort */

char *kore_sort_dump(kore_sort const *sort) {
  auto os = std::ostringstream{};
  sort->ptr_->print(os);
  return get_c_string(os);
}

void kore_sort_free(kore_sort const *sort) {
  delete sort;
}

bool kore_sort_is_concrete(kore_sort const *sort) {
  return sort->ptr_->isConcrete();
}

bool kore_sort_is_kitem(kore_sort const *sort) {
  if (auto composite
      = dynamic_cast<kllvm::KORECompositeSort *>(sort->ptr_.get())) {
    return composite->getName() == "SortKItem";
  }

  return false;
}

bool kore_sort_is_k(kore_sort const *sort) {
  if (auto composite
      = dynamic_cast<kllvm::KORECompositeSort *>(sort->ptr_.get())) {
    return composite->getName() == "SortK";
  }

  return false;
}

/* KORECompositeSort */

kore_sort *kore_composite_sort_new(char const *name) {
  auto sort = new kore_sort;
  sort->ptr_ = kllvm::KORECompositeSort::Create(std::string(name));
  return sort;
}

void kore_composite_sort_add_argument(
    kore_sort const *sort, kore_sort const *arg) {
  if (auto cast_ptr
      = dynamic_cast<kllvm::KORECompositeSort *>(sort->ptr_.get())) {
    cast_ptr->addArgument(arg->ptr_);
  } else {
    abort();
  }
}

/* KORESymbol */

kore_symbol *kore_symbol_new(char const *name) {
  auto sym = new kore_symbol;
  sym->ptr_ = kllvm::KORESymbol::Create(std::string(name));
  return sym;
}

void kore_symbol_free(kore_symbol const *sym) {
  delete sym;
}

char *kore_symbol_dump(kore_symbol const *sym) {
  auto os = std::ostringstream{};
  sym->ptr_->print(os);
  return get_c_string(os);
}

void kore_symbol_add_formal_argument(kore_symbol *sym, kore_sort const *sort) {
  sym->ptr_->addFormalArgument(sort->ptr_);
}

/* Memory management */

void kllvm_free_all_memory(void) {
  freeAllMemory();
}
}

namespace {

template <typename OS>
char *get_c_string(OS const &os) {
  auto str = os.str();

  // Include null terminator
  auto total_length = str.length() + 1;

  auto c_str = reinterpret_cast<char *>(malloc(total_length * sizeof(char)));
  std::strncpy(c_str, str.c_str(), total_length);

  return c_str;
}

kore_pattern *kore_string_pattern_new_internal(std::string const &str) {
  auto pat = new kore_pattern;
  pat->ptr_ = kllvm::KOREStringPattern::Create(str);
  return pat;
}

kore_pattern *
kore_pattern_new_token_internal(kore_pattern *value, kore_sort const *sort) {
  auto sym = kore_symbol_new("\\dv");
  kore_symbol_add_formal_argument(sym, sort);

  auto pat = kore_composite_pattern_from_symbol(sym);
  kore_composite_pattern_add_argument(pat, value);

  return pat;
}

} // namespace
