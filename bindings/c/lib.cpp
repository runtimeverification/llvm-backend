#include <kllvm-c/kllvm-c.h>

#include <kllvm/ast/AST.h>
#include <kllvm/binary/serializer.h>
#include <kllvm/bindings/core/core.h>
#include <kllvm/parser/KOREParser.h>
#include <kllvm/printer/printer.h>

#include <fmt/format.h>

#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <optional>
#include <unistd.h>

// These headers need to be included last because they pollute a number of macro
// definitions into the global namespace.
#include <runtime/arena.h>
#include <runtime/header.h>

namespace fs = std::filesystem;

// Internal implementation details
namespace {

char *get_c_string(std::string const &);

kore_pattern *kore_string_pattern_new_internal(std::string const &);

kore_pattern *
kore_pattern_new_token_internal(kore_pattern const *, kore_sort const *);

struct pretty_print_definition {
  std::string syntax;
  std::string macros;
};

std::optional<pretty_print_definition> get_print_data();

/*
 * Any intermediate objects that are created in bindings functions and not
 * returned to the caller should be allocated using `managed`; there should be
 * no manual calls to any `kore_*_free` functions in this translation unit.
 */
auto managed(kore_pattern *ptr) {
  return std::unique_ptr<kore_pattern, decltype(kore_pattern_free) *>(
      ptr, kore_pattern_free);
}

auto managed(kore_sort *ptr) {
  return std::unique_ptr<kore_sort, decltype(kore_sort_free) *>(
      ptr, kore_sort_free);
}

auto managed(kore_symbol *ptr) {
  return std::unique_ptr<kore_symbol, decltype(kore_symbol_free) *>(
      ptr, kore_symbol_free);
}

} // namespace

/*
 * These declarations are internal to the backend and aren't exposed explicitly
 * through any header files, so we pull them in manually here.
 */

extern "C" {
void initStaticObjects(void);
void freeAllKoreMem(void);
}

extern "C" {

/*
 * These symbols may not have been compiled into the library (if
 * `--embed-kprint` was not passed), and so we need to give them a weak default
 * definition. If the embed flag was passed, the value of these symbols will be
 * given by the embedded data.
 */
int kore_definition_syntax_len __attribute__((weak)) = -1;
char kore_definition_syntax __attribute__((weak)) = -1;
int kore_definition_macros_len __attribute__((weak)) = -1;
char kore_definition_macros __attribute__((weak)) = -1;

/* Completed types */

struct kore_error {
  bool success_ = true;
  std::optional<std::string> message_ = std::nullopt;

  [[nodiscard]] char const *c_str() const {
    if (!success_ && message_.has_value()) {
      return message_->c_str();
    }

    return nullptr;
  }

  void set_error(std::string const &msg) {
    success_ = false;
    message_ = msg;
  }
};

struct kore_pattern {
  std::shared_ptr<kllvm::KOREPattern> ptr_;
};

struct kore_sort {
  std::shared_ptr<kllvm::KORESort> ptr_;
};

struct kore_symbol {
  std::shared_ptr<kllvm::KORESymbol> ptr_;
};

/* Error handling */

kore_error *kore_error_new(void) {
  return new kore_error;
}

bool kore_error_is_success(kore_error const *err) {
  return err->success_;
}

char const *kore_error_message(kore_error const *err) {
  return err->c_str();
}

void kore_error_free(kore_error *err) {
  delete err;
}

/* KOREPattern */

char *kore_pattern_dump(kore_pattern const *pat) {
  return get_c_string(ast_to_string(*pat->ptr_));
}

char *kore_pattern_pretty_print(kore_pattern const *pat) {
  char temp_dir_name[] = "tmp.pretty_print.XXXXXX";
  auto temp_path = [&temp_dir_name](auto const &file) {
    return fmt::format("{}/{}", temp_dir_name, file);
  };

  if (!mkdtemp(temp_dir_name)) {
    return nullptr;
  }

  auto maybe_print_data = get_print_data();
  if (!maybe_print_data) {
    return nullptr;
  }

  auto [syntax, macros] = *maybe_print_data;

  // Clean up ostreams at block scope exit
  {
    auto syntax_out
        = std::ofstream(temp_path("syntaxDefinition.kore"), std::ios::out);
    syntax_out << syntax;

    auto macros_out = std::ofstream(temp_path("macros.kore"), std::ios::out);
    macros_out << macros;

    auto pattern_out = std::ofstream(temp_path("pattern.kore"), std::ios::out);
    pat->ptr_->print(pattern_out);
  }

  auto ss = std::stringstream{};
  kllvm::printKORE(
      ss, temp_dir_name, temp_path("pattern.kore"), false, false, true);

  fs::remove_all(temp_dir_name);

  return get_c_string(ss.str());
}

void kore_pattern_serialize(
    kore_pattern const *pat, char **data_out, size_t *size_out) {
  auto out = kllvm::serializer();
  pat->ptr_->serialize_to(out);

  auto const &binary_data = out.data();
  auto binary_size = binary_data.size();

  *size_out = binary_size;
  *data_out = static_cast<char *>(malloc(binary_size));

  std::memcpy(*data_out, binary_data.data(), binary_size);
}

void kore_pattern_free(kore_pattern const *pat) {
  delete pat;
}

kore_pattern *kore_pattern_parse(char const *kore_text) {
  return new kore_pattern{
      kllvm::parser::KOREParser::from_string(kore_text)->pattern()};
}

kore_pattern *kore_pattern_parse_file(char const *filename) {
  return new kore_pattern{
      kllvm::parser::KOREParser(std::string(filename)).pattern()};
}

kore_pattern *kore_pattern_new_token(char const *value, kore_sort const *sort) {
  auto pat = managed(kore_string_pattern_new(value));
  return kore_pattern_new_token_internal(pat.get(), sort);
}

kore_pattern *kore_pattern_new_token_with_len(
    char const *value, size_t len, kore_sort const *sort) {
  auto pat = managed(kore_string_pattern_new_with_len(value, len));
  return kore_pattern_new_token_internal(pat.get(), sort);
}

kore_pattern *kore_pattern_new_injection(
    kore_pattern const *term, kore_sort const *from, kore_sort const *to) {
  return new kore_pattern{
      kllvm::bindings::make_injection(term->ptr_, from->ptr_, to->ptr_)};
}

kore_pattern *kore_pattern_make_interpreter_input(
    kore_pattern const *pgm, kore_sort const *sort) {
  auto config_sort = managed(kore_composite_sort_new("SortKConfigVar"));
  auto kitem_sort = managed(kore_composite_sort_new("SortKItem"));

  auto pgm_token = managed(kore_pattern_new_token("$PGM", config_sort.get()));
  auto key = managed(kore_pattern_new_injection(
      pgm_token.get(), config_sort.get(), kitem_sort.get()));

  auto map_item
      = managed(kore_composite_pattern_new("Lbl'UndsPipe'-'-GT-Unds'"));
  kore_composite_pattern_add_argument(map_item.get(), key.get());

  if (kore_sort_is_kitem(sort)) {
    kore_composite_pattern_add_argument(map_item.get(), pgm);
  } else {
    auto inj = managed(kore_pattern_new_injection(pgm, sort, kitem_sort.get()));
    kore_composite_pattern_add_argument(map_item.get(), inj.get());
  }

  auto map_unit = managed(kore_composite_pattern_new("Lbl'Stop'Map"));

  auto map_concat = managed(kore_composite_pattern_new("Lbl'Unds'Map'Unds'"));
  kore_composite_pattern_add_argument(map_concat.get(), map_unit.get());
  kore_composite_pattern_add_argument(map_concat.get(), map_item.get());

  auto *top_cell = kore_composite_pattern_new("LblinitGeneratedTopCell");
  kore_composite_pattern_add_argument(top_cell, map_concat.get());

  return top_cell;
}

kore_pattern *kore_pattern_desugar_associative(kore_pattern const *pat) {
  return new kore_pattern{pat->ptr_->desugarAssociative()};
}

block *kore_pattern_construct(kore_pattern const *pat) {
  return kllvm::bindings::construct_term(pat->ptr_);
}

char *kore_block_dump(block *term) {
  auto *hooked_str = printConfigurationToString(term)->data;
  auto len = std::strlen(hooked_str);

  auto *new_str = static_cast<char *>(malloc(len + 1));
  std::strncpy(new_str, hooked_str, len);
  new_str[len] = '\0';

  return new_str;
}

kore_pattern *kore_pattern_from_block(block *term) {
  return new kore_pattern{kllvm::bindings::term_to_pattern(term)};
}

bool kore_block_get_bool(block *term) {
  return kllvm::bindings::get_bool(term);
}

bool kore_simplify_bool(kore_error *err, kore_pattern const *pattern) {
  try {
    return kllvm::bindings::simplify_to_bool(pattern->ptr_);
  } catch (std::exception &e) {
    if (err == nullptr) {
      throw;
    }

    err->set_error(e.what());
    return false;
  }
}

void kore_simplify(
    kore_error *err, kore_pattern const *pattern, kore_sort const *sort,
    char **data_out, size_t *size_out) {
  try {
    auto *block = kllvm::bindings::simplify_to_term(pattern->ptr_, sort->ptr_);
    serializeConfiguration(block, "SortKItem{}", data_out, size_out, true);
  } catch (std::exception &e) {
    if (err == nullptr) {
      throw;
    }

    err->set_error(e.what());
  }
}

void kore_simplify_binary(
    kore_error *err, char *data_in, size_t size_in, kore_sort const *sort,
    char **data_out, size_t *size_out) {
  try {
    auto sort_str = std::unique_ptr<char, decltype(std::free) *>(
        kore_sort_dump(sort), std::free);

    auto *block = deserializeConfiguration(data_in, size_in);
    serializeConfiguration(block, sort_str.get(), data_out, size_out, true);
  } catch (std::exception &e) {
    if (err == nullptr) {
      throw;
    }

    err->set_error(e.what());
  }
}

/* KORECompositePattern */

kore_pattern *kore_composite_pattern_new(char const *name) {
  return new kore_pattern{
      kllvm::KORECompositePattern::Create(std::string(name))};
}

kore_pattern *kore_composite_pattern_from_symbol(kore_symbol *sym) {
  return new kore_pattern{kllvm::KORECompositePattern::Create(sym->ptr_.get())};
}

void kore_composite_pattern_add_argument(
    kore_pattern *pat, kore_pattern const *arg) {
  if (auto const &cast
      = std::dynamic_pointer_cast<kllvm::KORECompositePattern>(pat->ptr_)) {
    cast->addArgument(arg->ptr_);
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
  return get_c_string(ast_to_string(*sort->ptr_));
}

void kore_sort_free(kore_sort const *sort) {
  delete sort;
}

bool kore_sort_is_concrete(kore_sort const *sort) {
  return sort->ptr_->isConcrete();
}

bool kore_sort_is_kitem(kore_sort const *sort) {
  return kllvm::bindings::is_sort_kitem(sort->ptr_);
}

bool kore_sort_is_k(kore_sort const *sort) {
  return kllvm::bindings::is_sort_k(sort->ptr_);
}

/* KORECompositeSort */

kore_sort *kore_composite_sort_new(char const *name) {
  return new kore_sort{kllvm::KORECompositeSort::Create(std::string(name))};
}

void kore_composite_sort_add_argument(
    kore_sort const *sort, kore_sort const *arg) {
  if (auto const &cast
      = std::dynamic_pointer_cast<kllvm::KORECompositeSort>(sort->ptr_)) {
    cast->addArgument(arg->ptr_);
  } else {
    abort();
  }
}

/* KORESymbol */

kore_symbol *kore_symbol_new(char const *name) {
  return new kore_symbol{kllvm::KORESymbol::Create(std::string(name))};
}

void kore_symbol_free(kore_symbol const *sym) {
  delete sym;
}

char *kore_symbol_dump(kore_symbol const *sym) {
  return get_c_string(ast_to_string(*sym->ptr_));
}

void kore_symbol_add_formal_argument(kore_symbol *sym, kore_sort const *sort) {
  sym->ptr_->addFormalArgument(sort->ptr_);
}

/* Memory management */

void kllvm_init(void) {
  initStaticObjects();
}

void kllvm_free_all_memory(void) {
  freeAllKoreMem();
}
}

namespace {

char *get_c_string(std::string const &str) {
  // Include null terminator
  auto total_length = str.length() + 1;

  auto *c_str = static_cast<char *>(malloc(total_length));
  std::strncpy(c_str, str.c_str(), total_length);

  return c_str;
}

kore_pattern *kore_string_pattern_new_internal(std::string const &str) {
  return new kore_pattern{kllvm::KOREStringPattern::Create(str)};
}

kore_pattern *kore_pattern_new_token_internal(
    kore_pattern const *value, kore_sort const *sort) {
  auto sym = managed(kore_symbol_new("\\dv"));
  kore_symbol_add_formal_argument(sym.get(), sort);

  auto *pat = kore_composite_pattern_from_symbol(sym.get());
  kore_composite_pattern_add_argument(pat, value);

  return pat;
}

std::optional<pretty_print_definition> get_print_data() {
  if (kore_definition_macros_len == -1 || kore_definition_macros == -1
      || kore_definition_syntax_len == -1 || kore_definition_syntax == -1) {
    return std::nullopt;
  }

  return pretty_print_definition{
      std::string(&kore_definition_syntax, kore_definition_syntax_len),
      std::string(&kore_definition_macros, kore_definition_macros_len)};
}

} // namespace
