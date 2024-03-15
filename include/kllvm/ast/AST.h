#ifndef AST_H
#define AST_H

#include <kllvm/ast/attribute_set.h>

#include <boost/container_hash/extensions.hpp>

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <list>
#include <map>
#include <memory>
#include <set>
#include <sstream>
#include <string>
#include <tuple>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

namespace kllvm {

class serializer;

class kore_sort_variable;

struct hash_sort;

template <typename T>
using ptr = std::unique_ptr<T>;

template <typename T>
using sptr = std::shared_ptr<T>;

std::string decode_kore(std::string);

/*
 * Helper function to avoid repeated call-site uses of ostringstream when we
 * just want the string representation of a node, rather than to print it to a
 * stream.
 */
template <typename T, typename... Args>
std::string ast_to_string(T &&node, Args &&...args) {
  auto os = std::ostringstream{};
  std::forward<T>(node).print(os, std::forward<Args>(args)...);
  return os.str();
}

// kore_sort
class kore_sort : public std::enable_shared_from_this<kore_sort> {
public:
  using substitution
      = std::unordered_map<kore_sort_variable, sptr<kore_sort>, hash_sort>;

  virtual bool is_concrete() const = 0;
  virtual sptr<kore_sort> substitute(substitution const &) = 0;

  virtual bool operator==(kore_sort const &other) const = 0;
  bool operator!=(kore_sort const &other) const { return !(*this == other); }

  virtual void print(std::ostream &out, unsigned indent = 0) const = 0;
  virtual void pretty_print(std::ostream &out) const = 0;
  virtual void serialize_to(serializer &s) const = 0;

  virtual ~kore_sort() = default;
};

static inline std::ostream &operator<<(std::ostream &out, kore_sort const &s) {
  s.print(out);
  return out;
}

struct hash_sort {
  size_t operator()(kllvm::kore_sort const &s) const noexcept {
    return std::hash<std::string>{}(ast_to_string(s));
  }
};

struct equal_sort_ptr {
  bool operator()(kore_sort *const &first, kore_sort *const &second) const {
    return *first == *second;
  }
};

struct hash_sort_ptr {
  size_t operator()(kllvm::kore_sort *const &s) const noexcept {
    return std::hash<std::string>{}(ast_to_string(*s));
  }
};

size_t hash_value(kllvm::kore_sort const &s);

class kore_sort_variable : public kore_sort {
private:
  std::string name_;

public:
  static sptr<kore_sort_variable> create(std::string const &name) {
    return sptr<kore_sort_variable>(new kore_sort_variable(name));
  }

  bool is_concrete() const override { return false; }
  sptr<kore_sort> substitute(substitution const &subst) override {
    return subst.at(*this);
  }

  void print(std::ostream &out, unsigned indent = 0) const override;
  void pretty_print(std::ostream &out) const override;
  void serialize_to(serializer &s) const override;

  bool operator==(kore_sort const &other) const override;

  std::string const &get_name() const { return name_; }

private:
  kore_sort_variable(std::string name)
      : name_(std::move(name)) { }
};

enum class sort_category {
  Uncomputed,
  Map,
  List,
  Set,
  Int,
  Float,
  StringBuffer,
  Bool,
  Symbol,
  Variable,
  MInt,
  RangeMap
};

// represents the syntactic category of an LLVM backend term at runtime
struct value_type {
  // fundamental category of the term
  sort_category cat;
  // if this is an MInt, the number of bits in the MInt
  uint64_t bits;

  bool operator<(value_type const &that) const {
    return std::make_tuple(this->cat, this->bits)
           < std::make_tuple(that.cat, that.bits);
  }
};

class kore_definition;

class kore_composite_sort : public kore_sort {
private:
  std::string name_;
  std::vector<sptr<kore_sort>> arguments_;
  value_type category_;

public:
  static sptr<kore_composite_sort> create(
      std::string const &name,
      value_type cat = {sort_category::Uncomputed, 0}) {
    return sptr<kore_composite_sort>(new kore_composite_sort(name, cat));
  }

  std::string get_name() const { return name_; }
  value_type get_category(kore_definition *definition);
  std::string get_hook(kore_definition *definition) const;
  static value_type get_category(std::string const &hook_name);

  bool is_concrete() const override;
  sptr<kore_sort> substitute(substitution const &subst) override;

  void add_argument(sptr<kore_sort> const &argument);
  void print(std::ostream &out, unsigned indent = 0) const override;
  void pretty_print(std::ostream &out) const override;
  void serialize_to(serializer &s) const override;
  bool operator==(kore_sort const &other) const override;

  std::vector<sptr<kore_sort>> const &get_arguments() const {
    return arguments_;
  }

private:
  kore_composite_sort(std::string name, value_type category)
      : name_(std::move(name))
      , category_(category) { }
};

struct hash_symbol;

class kore_symbol_declaration;

// kore_symbol
class kore_symbol {
private:
  std::string name_;
  /* At parse time, when parsed as part of a pattern,
      this will be empty. When parsed as part of a declaration, it contains the
     signature of the symbol. After instantiateSymbol is called on a symbol that
     is part of a pattern, it changes from being empty to being the signature of
     the symbol. instantiateSymbol is called on all object level symbols in
     axioms when kore_definition::preprocess is called. */
  std::vector<sptr<kore_sort>> arguments_;
  /* contains the original arguments to the symbol when parsed as parh of a
   * pattern. */
  std::vector<sptr<kore_sort>> formal_arguments_;
  /** At parse time, when parsed as part of a pattern, this will be null.
      When parsed as part of a declaration, it contains the return sort of the
     symbol. See above re: the behavior of kore_symbol with respect to
     instantiateSymbol. */
  sptr<kore_sort> sort_;
  /* the first integer in a continuous range representing the tags of all the
     polymorphic instantiations of this symbol. If the symbol has no parameters
     or its parameters are fully specified, firstTag == lastTag. */
  uint32_t first_tag_{};
  /* the last integer in a continuous range representing the tags of all the
     polymorphic instantiations of this symbol. If the symbol has no parameters
     or its parameters are fully specified, firstTag == lastTag. */
  uint32_t last_tag_{};
  /* A unique integer representing the layout of the symbol in memory.
     See create_term.cpp for more information about the layout of K terms. */
  uint16_t layout_{};
  bool instantiated_ = false;

public:
  static ptr<kore_symbol> create(std::string const &name) {
    return ptr<kore_symbol>(new kore_symbol(name));
  }

  void add_argument(sptr<kore_sort> const &argument);
  void add_formal_argument(sptr<kore_sort> const &argument);
  void add_sort(sptr<kore_sort> sort);
  void init_pattern_arguments() { arguments_.swap(formal_arguments_); }

  [[nodiscard]] std::string const &get_name() const { return name_; }
  [[nodiscard]] std::vector<sptr<kore_sort>> const &get_arguments() const {
    return arguments_;
  }
  [[nodiscard]] std::vector<sptr<kore_sort>> const &
  get_formal_arguments() const {
    return formal_arguments_;
  }
  [[nodiscard]] sptr<kore_sort> get_sort() const { return sort_; }
  sptr<kore_sort> get_sort() { return sort_; }
  [[nodiscard]] uint32_t get_tag() const {
    assert(first_tag_ == last_tag_);
    return first_tag_;
  }
  [[nodiscard]] uint32_t get_first_tag() const { return first_tag_; }
  [[nodiscard]] uint32_t get_last_tag() const { return last_tag_; }
  void set_tag(uint32_t val) { first_tag_ = last_tag_ = val; }
  [[nodiscard]] uint16_t get_layout() const { return layout_; }

  void print(std::ostream &out, unsigned indent = 0) const;
  void print(std::ostream &out, unsigned indent, bool formal) const;
  void serialize_to(serializer &s) const;

  bool operator==(kore_symbol const &other) const;
  bool operator!=(kore_symbol const &other) const { return !(*this == other); }

  std::string layout_string(kore_definition *) const;

  [[nodiscard]] bool is_concrete() const;
  [[nodiscard]] bool is_polymorphic() const;
  [[nodiscard]] bool is_builtin() const;

  /* instantiates this symbol (which should be parsed from a pattern in an
     axiom) with the sorts corresponding to its actual sort parameters after
     instantiating polymorphic parameters. This happens by replacing the
     variables in the arguments of the specified declaration with their
     substitution in the arguments to the pattern that were parsed in braces.
     The result is that the arguments and sort fields are replaced with the
     instantiated signature of the symbol. */
  void instantiate_symbol(kore_symbol_declaration *decl);

  friend hash_symbol;

  friend kore_definition;

private:
  kore_symbol(std::string name)
      : name_(std::move(name))
      , sort_(nullptr) { }
};

struct hash_symbol {
  size_t operator()(kllvm::kore_symbol const &s) const noexcept {
    size_t hash = 0;
    boost::hash_combine(hash, s.name_);
    for (auto const &arg : s.arguments_) {
      boost::hash_combine(hash, *arg);
    }
    return hash;
  }
};

struct equal_symbol_ptr {
  bool operator()(kore_symbol *const &first, kore_symbol *const &second) const {
    return ast_to_string(*first) == ast_to_string(*second);
  }
};

struct hash_symbol_ptr {
  size_t operator()(kllvm::kore_symbol *const &s) const noexcept {
    return std::hash<std::string>{}(ast_to_string(*s));
  }
};

// kore_variable
class kore_variable {
private:
  std::string name_;

public:
  static ptr<kore_variable> create(std::string const &name) {
    return ptr<kore_variable>(new kore_variable(name));
  }

  [[nodiscard]] std::string get_name() const;

  virtual void print(std::ostream &out, unsigned indent = 0) const;
  virtual void serialize_to(serializer &s) const;

  virtual ~kore_variable() = default;

private:
  kore_variable(std::string name)
      : name_(std::move(name)) { }
};

class kore_variable_pattern;

using SortSet = std::unordered_set<kore_sort *, hash_sort_ptr, equal_sort_ptr>;
using SymbolSet
    = std::unordered_set<kore_symbol *, hash_symbol_ptr, equal_symbol_ptr>;
using SubsortMap
    = std::unordered_map<kore_sort *, SortSet, hash_sort_ptr, equal_sort_ptr>;
using SymbolMap = std::unordered_map<
    kore_symbol *, SymbolSet, hash_symbol_ptr, equal_symbol_ptr>;
using BracketMap = std::unordered_map<
    kore_sort *, std::vector<kore_symbol *>, hash_sort_ptr, equal_sort_ptr>;

struct pretty_print_data {
  // map from symbol name to format attribute specifying how to print that
  // symbol
  std::map<std::string, std::string> format;
  // map from symbol name to vector of colors for that symbol
  std::map<std::string, std::vector<std::string>> colors;
  // map from symbol name to terminal pattern
  std::map<std::string, std::string> terminals;
  std::map<std::string, std::set<std::string>> priorities;
  std::map<std::string, std::set<std::string>> left_assoc;
  std::map<std::string, std::set<std::string>> right_assoc;
  // map from sort name to hook attribute for that sort
  std::map<std::string, std::string> hook;
  // map from sort name to bracket for that sort
  BracketMap brackets;
  // set of associative symbols
  std::set<std::string> assoc;
  // set of commutative symbols
  std::set<std::string> comm;
  SubsortMap subsorts;
  // enable coloring
  bool has_color{};
};

class kore_declaration;
class kore_composite_pattern;

// kore_pattern
class kore_pattern : public std::enable_shared_from_this<kore_pattern> {
public:
  /*
   * Load a pattern from disk, examining the first 4 bytes to see if it's a
   * binary file or a textual KORE file.
   */
  static sptr<kore_pattern> load(std::string const &filename);

  virtual ~kore_pattern() = default;

  virtual void print(std::ostream &out, unsigned indent = 0) const = 0;

  virtual void serialize_to(serializer &s) const = 0;

  /* adds all the object level symbols contained recursively in the current
     pattern to the specified map, mapping their symbol name to the list of all
     instances of that symbol. */
  virtual void mark_symbols(std::map<std::string, std::vector<kore_symbol *>> &)
      = 0;
  /* adds all the object level variables contained recursively in the current
     pattern to the specified map, mapping their variable name to the variable
     itself. */
  virtual void mark_variables(std::map<std::string, kore_variable_pattern *> &)
      = 0;

  virtual sptr<kore_sort> get_sort() const = 0;

  using substitution = std::unordered_map<std::string, sptr<kore_pattern>>;

  virtual sptr<kore_pattern> substitute(substitution const &) = 0;
  virtual sptr<kore_pattern> expand_aliases(kore_definition *) = 0;

  virtual void pretty_print(std::ostream &, pretty_print_data const &data) const
      = 0;
  virtual sptr<kore_pattern> sort_collections(pretty_print_data const &data)
      = 0;
  std::set<std::string> gather_singleton_vars();
  virtual std::map<std::string, int> gather_var_counts() = 0;
  virtual sptr<kore_pattern> filter_substitution(
      pretty_print_data const &data, std::set<std::string> const &vars)
      = 0;
  virtual sptr<kore_pattern> dedupe_disjuncts() = 0;
  virtual bool matches(
      substitution &subst, SubsortMap const &subsorts,
      SymbolMap const &overloads, sptr<kore_pattern> subject)
      = 0;
  sptr<kore_pattern> expand_macros(
      SubsortMap const &subsorts, SymbolMap const &overloads,
      std::vector<ptr<kore_declaration>> const &axioms, bool reverse);
  virtual sptr<kore_pattern> unflatten_and_or() = 0;

  /*
   * Recursively expands productions of the form:
   *
   *   \{left,right}-assoc{}(LABEL(a_0, a_1, ..., a_n-1, a_n))
   *
   * Into their canonical forms:
   *
   *    left: LABEL(LABEL(..., a_n-1), a_n)
   *   right: LABEL(a_0, LABEL(a_1, ...))
   *
   * This will be a no-op if this pattern was produced by the KORE parser, which
   * performs this desugaring "live" at parsing time. The method is intended
   * primarily to be called from language bindings that programatically
   * construct patterns.
   */
  virtual sptr<kore_pattern> desugar_associative() = 0;

  friend kore_composite_pattern;

private:
  virtual sptr<kore_pattern> expand_macros(
      SubsortMap const &subsorts, SymbolMap const &overloads,
      std::vector<ptr<kore_declaration>> const &axioms, bool reverse,
      std::set<size_t> &applied_rules,
      std::set<std::string> const &macro_symbols)
      = 0;
};

void flatten(
    kore_composite_pattern *pat, std::string const &name,
    std::vector<sptr<kore_pattern>> &result);

class kore_variable_pattern : public kore_pattern {
private:
  ptr<kore_variable> name_;
  sptr<kore_sort> sort_;

public:
  static ptr<kore_variable_pattern>
  create(std::string const &name, sptr<kore_sort> sort) {
    ptr<kore_variable> var = kore_variable::create(name);
    return ptr<kore_variable_pattern>(
        new kore_variable_pattern(std::move(var), std::move(sort)));
  }

  std::string get_name() const;
  sptr<kore_sort> get_sort() const override { return sort_; }

  void print(std::ostream &out, unsigned indent = 0) const override;
  void serialize_to(serializer &s) const override;

  void
  mark_symbols(std::map<std::string, std::vector<kore_symbol *>> &) override { }
  void
  mark_variables(std::map<std::string, kore_variable_pattern *> &map) override {
    map.insert({name_->get_name(), this});
  }
  sptr<kore_pattern> substitute(substitution const &subst) override {
    auto val = subst.find(name_->get_name());
    if (val == subst.end()) {
      return shared_from_this();
    }
    return val->second;
  }
  sptr<kore_pattern> expand_aliases(kore_definition *) override {
    return shared_from_this();
  }
  sptr<kore_pattern> sort_collections(pretty_print_data const &data) override {
    return shared_from_this();
  }
  sptr<kore_pattern> dedupe_disjuncts() override { return shared_from_this(); }
  std::map<std::string, int> gather_var_counts() override {
    return std::map<std::string, int>{{name_->get_name(), 1}};
  }
  sptr<kore_pattern> filter_substitution(
      pretty_print_data const &data,
      std::set<std::string> const &vars) override {
    return shared_from_this();
  }

  sptr<kore_pattern> desugar_associative() override {
    return shared_from_this();
  }

  sptr<kore_pattern> unflatten_and_or() override { return shared_from_this(); }

  bool matches(
      substitution &subst, SubsortMap const &, SymbolMap const &,
      sptr<kore_pattern> subject) override;
  void
  pretty_print(std::ostream &out, pretty_print_data const &data) const override;

private:
  sptr<kore_pattern> expand_macros(
      SubsortMap const &, SymbolMap const &,
      std::vector<ptr<kore_declaration>> const &macros, bool reverse,
      std::set<size_t> &applied_rules,
      std::set<std::string> const &macro_symbols) override {
    return shared_from_this();
  }

  kore_variable_pattern(ptr<kore_variable> name, sptr<kore_sort> sort)
      : name_(std::move(name))
      , sort_(std::move(std::move(sort))) { }
};

void deallocate_s_ptr_kore_pattern(sptr<kore_pattern> pattern);

class kore_composite_pattern : public kore_pattern {
private:
  ptr<kore_symbol> constructor_;
  std::vector<sptr<kore_pattern>> arguments_;

public:
  static ptr<kore_composite_pattern> create(std::string const &name) {
    ptr<kore_symbol> sym = kore_symbol::create(name);
    return ptr<kore_composite_pattern>(
        new kore_composite_pattern(std::move(sym)));
  }
  static ptr<kore_composite_pattern> create(ptr<kore_symbol> sym) {
    return ptr<kore_composite_pattern>(
        new kore_composite_pattern(std::move(sym)));
  }
  static ptr<kore_composite_pattern> create(kore_symbol *sym) {
    ptr<kore_symbol> new_sym = kore_symbol::create(sym->get_name());
    *new_sym = *sym;
    return ptr<kore_composite_pattern>(
        new kore_composite_pattern(std::move(new_sym)));
  }

  sptr<kore_sort> get_sort() const override {
    if (constructor_->get_name() == "\\dv"
        && !constructor_->get_formal_arguments().empty()) {
      if (auto arg = constructor_->get_formal_arguments()[0]) {
        return arg;
      }
    }

    return constructor_->get_sort();
  }

  kore_symbol *get_constructor() const { return constructor_.get(); }
  std::vector<sptr<kore_pattern>> const &get_arguments() const {
    return arguments_;
  }

  void add_argument(sptr<kore_pattern> const &argument);

  void print(std::ostream &out, unsigned indent = 0) const override;
  void serialize_to(serializer &s) const override;

  void
  pretty_print(std::ostream &out, pretty_print_data const &data) const override;
  void
  mark_symbols(std::map<std::string, std::vector<kore_symbol *>> &) override;
  void
  mark_variables(std::map<std::string, kore_variable_pattern *> &) override;
  sptr<kore_pattern> substitute(substitution const &) override;
  sptr<kore_pattern> expand_aliases(kore_definition *) override;
  sptr<kore_pattern> sort_collections(pretty_print_data const &data) override;
  sptr<kore_pattern> dedupe_disjuncts() override;
  std::map<std::string, int> gather_var_counts() override;
  sptr<kore_pattern> desugar_associative() override;
  sptr<kore_pattern> unflatten_and_or() override;
  sptr<kore_pattern> filter_substitution(
      pretty_print_data const &data,
      std::set<std::string> const &vars) override;
  bool matches(
      substitution &, SubsortMap const &, SymbolMap const &,
      sptr<kore_pattern>) override;

private:
  sptr<kore_pattern> expand_macros(
      SubsortMap const &, SymbolMap const &,
      std::vector<ptr<kore_declaration>> const &macros, bool reverse,
      std::set<size_t> &applied_rules,
      std::set<std::string> const &macro_symbols) override;

  friend void ::kllvm::deallocate_s_ptr_kore_pattern(
      sptr<kore_pattern> pattern);

  kore_composite_pattern(ptr<kore_symbol> constructor)
      : constructor_(std::move(constructor)) { }
};

class kore_string_pattern : public kore_pattern {
private:
  std::string contents_;

public:
  static ptr<kore_string_pattern> create(std::string const &contents) {
    return ptr<kore_string_pattern>(new kore_string_pattern(contents));
  }

  std::string get_contents() { return contents_; }

  void print(std::ostream &out, unsigned indent = 0) const override;
  void serialize_to(serializer &s) const override;
  void pretty_print(
      std::ostream &out, pretty_print_data const &data) const override {
    abort();
  }

  void
  mark_symbols(std::map<std::string, std::vector<kore_symbol *>> &) override { }
  void
  mark_variables(std::map<std::string, kore_variable_pattern *> &) override { }
  sptr<kore_sort> get_sort() const override { abort(); }
  sptr<kore_pattern> substitute(substitution const &) override {
    return shared_from_this();
  }
  sptr<kore_pattern> expand_aliases(kore_definition *) override {
    return shared_from_this();
  }
  sptr<kore_pattern> sort_collections(pretty_print_data const &data) override {
    return shared_from_this();
  }
  sptr<kore_pattern> dedupe_disjuncts() override { return shared_from_this(); }
  std::map<std::string, int> gather_var_counts() override {
    return std::map<std::string, int>{};
  }

  sptr<kore_pattern> desugar_associative() override {
    return shared_from_this();
  }

  sptr<kore_pattern> unflatten_and_or() override { return shared_from_this(); }

  sptr<kore_pattern> filter_substitution(
      pretty_print_data const &data,
      std::set<std::string> const &var) override {
    return shared_from_this();
  }
  bool matches(
      substitution &, SubsortMap const &, SymbolMap const &,
      sptr<kore_pattern> subject) override;

private:
  sptr<kore_pattern> expand_macros(
      SubsortMap const &, SymbolMap const &,
      std::vector<ptr<kore_declaration>> const &macros, bool reverse,
      std::set<size_t> &applied_rules,
      std::set<std::string> const &macro_symbols) override {
    return shared_from_this();
  }

  kore_string_pattern(std::string contents)
      : contents_(std::move(contents)) { }
};

// kore_declaration
class kore_declaration {
private:
  attribute_set attributes_;
  std::vector<sptr<kore_sort_variable>> object_sort_variables_;

public:
  attribute_set &attributes() { return attributes_; }
  [[nodiscard]] attribute_set const &attributes() const { return attributes_; }

  void add_object_sort_variable(sptr<kore_sort_variable> const &sort_variable);
  virtual void print(std::ostream &out, unsigned indent = 0) const = 0;

  [[nodiscard]] std::vector<sptr<kore_sort_variable>> const &
  get_object_sort_variables() const {
    return object_sort_variables_;
  }
  virtual ~kore_declaration() = default;

protected:
  void print_sort_variables(std::ostream &out) const;
};

class kore_composite_sort_declaration : public kore_declaration {
private:
  bool is_hooked_;
  std::string sort_name_;

public:
  static ptr<kore_composite_sort_declaration>
  create(std::string const &name, bool is_hooked = false) {
    return ptr<kore_composite_sort_declaration>(
        new kore_composite_sort_declaration(name, is_hooked));
  }

  [[nodiscard]] std::string get_name() const { return sort_name_; }
  [[nodiscard]] bool is_hooked() const { return is_hooked_; }

  void print(std::ostream &out, unsigned indent = 0) const override;

private:
  kore_composite_sort_declaration(std::string name, bool is_hooked)
      : is_hooked_(is_hooked)
      , sort_name_(std::move(name)) { }
};

class kore_symbol_alias_declaration : public kore_declaration {
private:
  ptr<kore_symbol> symbol_;

protected:
  kore_symbol_alias_declaration(ptr<kore_symbol> symbol)
      : symbol_(std::move(symbol)) { }

public:
  [[nodiscard]] kore_symbol *get_symbol() const { return symbol_.get(); }
};

class kore_symbol_declaration : public kore_symbol_alias_declaration {
private:
  bool is_hooked_;

public:
  static ptr<kore_symbol_declaration>
  create(std::string const &name, bool is_hooked = false) {
    ptr<kore_symbol> sym = kore_symbol::create(name);
    return ptr<kore_symbol_declaration>(
        new kore_symbol_declaration(std::move(sym), is_hooked));
  }

  [[nodiscard]] bool is_hooked() const { return is_hooked_; }

  [[nodiscard]] bool is_anywhere() const;

  void print(std::ostream &out, unsigned indent = 0) const override;

private:
  kore_symbol_declaration(ptr<kore_symbol> symbol, bool is_hooked)
      : kore_symbol_alias_declaration(std::move(symbol))
      , is_hooked_(is_hooked) { }
};

class kore_alias_declaration : public kore_symbol_alias_declaration {
private:
  sptr<kore_composite_pattern> bound_variables_;
  sptr<kore_pattern> pattern_;

public:
  static ptr<kore_alias_declaration> create(std::string const &name) {
    ptr<kore_symbol> sym = kore_symbol::create(name);
    return ptr<kore_alias_declaration>(
        new kore_alias_declaration(std::move(sym)));
  }

  void add_variables(sptr<kore_composite_pattern> variables);
  void add_pattern(sptr<kore_pattern> pattern);
  kore_pattern::substitution get_substitution(kore_composite_pattern *subject);
  [[nodiscard]] kore_composite_pattern *get_bound_variables() const {
    return bound_variables_.get();
  }
  sptr<kore_pattern> &get_pattern() { return pattern_; }
  void print(std::ostream &out, unsigned indent = 0) const override;

private:
  kore_alias_declaration(ptr<kore_symbol> symbol)
      : kore_symbol_alias_declaration(std::move(symbol)) { }
};

class kore_axiom_declaration : public kore_declaration {
private:
  sptr<kore_pattern> pattern_;
  unsigned ordinal_{};
  bool is_claim_;

  kore_axiom_declaration(bool is_claim)
      : is_claim_(is_claim) { }

public:
  static ptr<kore_axiom_declaration> create(bool is_claim = false) {
    return ptr<kore_axiom_declaration>(new kore_axiom_declaration(is_claim));
  }

  void add_pattern(sptr<kore_pattern> pattern);
  void print(std::ostream &out, unsigned indent = 0) const override;

  /* returns true if the axiom is actually required to be translated to llvm
     and false if it is an axiom pertaining to symbolic execution which is not
     required for concrete execution. Axioms that are not required are elided
     from the definition by kore_definition::preprocess. */
  [[nodiscard]] bool is_required() const;
  [[nodiscard]] bool is_top_axiom() const;
  [[nodiscard]] bool is_claim() const { return is_claim_; }
  [[nodiscard]] kore_pattern *get_right_hand_side() const;
  [[nodiscard]] std::vector<kore_pattern *> get_left_hand_side() const;
  [[nodiscard]] kore_pattern *get_requires() const;
  [[nodiscard]] sptr<kore_pattern> get_pattern() const { return pattern_; }
  [[nodiscard]] unsigned get_ordinal() const { return ordinal_; }

  friend kore_definition;
};

class kore_module_import_declaration : public kore_declaration {
private:
  std::string module_name_;

public:
  static ptr<kore_module_import_declaration> create(std::string const &name) {
    return ptr<kore_module_import_declaration>(
        new kore_module_import_declaration(name));
  }

  [[nodiscard]] std::string const &get_module_name() const {
    return module_name_;
  }

  void print(std::ostream &out, unsigned indent = 0) const override;

private:
  kore_module_import_declaration(std::string name)
      : module_name_(std::move(name)) { }
};

// kore_module
class kore_module {
private:
  std::string name_;
  std::vector<sptr<kore_declaration>> declarations_;
  attribute_set attributes_;

public:
  static ptr<kore_module> create(std::string const &name) {
    return ptr<kore_module>(new kore_module(name));
  }

  attribute_set &attributes() { return attributes_; }
  [[nodiscard]] attribute_set const &attributes() const { return attributes_; }

  void add_declaration(sptr<kore_declaration> declaration);
  void print(std::ostream &out, unsigned indent = 0) const;

  [[nodiscard]] std::string const &get_name() const { return name_; }
  [[nodiscard]] std::vector<sptr<kore_declaration>> const &
  get_declarations() const {
    return declarations_;
  }

private:
  kore_module(std::string name)
      : name_(std::move(name)) { }
};

// kore_definition
class kore_definition {
public:
  // Symbol table types
  using kore_moduleMapType = std::map<std::string, kore_module *>;

  using kore_sortConstructorMapType
      = std::map<std::string, kore_composite_sort *>;

  using kore_symbolMapType = std::map<uint32_t, kore_symbol *>;

  using kore_symbolStringMapType = std::map<std::string, kore_symbol *>;

  using kore_sort_variableMapType = std::map<std::string, kore_sort_variable *>;

  using kore_variableMapType = std::map<std::string, kore_variable *>;

  using kore_composite_sortDeclarationMapType
      = std::map<std::string, kore_composite_sort_declaration *>;
  using kore_composite_sortMapType
      = std::map<value_type, sptr<kore_composite_sort>>;

  using kore_symbol_declarationMapType
      = std::map<std::string, kore_symbol_declaration *>;
  using kore_alias_declarationMapType
      = std::map<std::string, kore_alias_declaration *>;

  using KOREAxiomMapType = std::map<size_t, kore_axiom_declaration *>;

private:
  // Symbol tables
  kore_sortConstructorMapType object_sort_constructors_;
  kore_symbolMapType object_symbols_;
  kore_symbolStringMapType all_object_symbols_;
  kore_sort_variableMapType object_sort_variables_;
  kore_variableMapType object_variables_;
  kore_moduleMapType module_names_;
  kore_composite_sortDeclarationMapType sort_declarations_;
  kore_symbol_declarationMapType symbol_declarations_;
  kore_alias_declarationMapType alias_declarations_;
  kore_composite_sortMapType hooked_sorts_;
  kore_symbolStringMapType fresh_functions_;
  KOREAxiomMapType ordinals_;

  std::vector<sptr<kore_module>> modules_;
  attribute_set attributes_;

  /* an automatically computed list of all the axioms in the definition */
  std::list<kore_axiom_declaration *> axioms_;

  kore_symbol *inj_symbol_{};

  /*
   * Insert symbols into this definition that have knowable labels, but cannot
   * be directly referenced in user code:
   *   - rawTerm(KItem) for serializing non-symbol backend terms
   */
  void insert_reserved_symbols();

public:
  static ptr<kore_definition> create() {
    return std::make_unique<kore_definition>();
  }

  /* Preprocesses the definition and prepares it for translation to llvm.
     This performs the following tasks:
     * removes axioms for which isRequired() returns false
     * sets the arguments field for each kore_symbol to the actual instantiated
       sort arguments of the symbol (rather than just their polymorphic
     parameters
     * sets the tag and layout fields on all the kore_symbols declared by the
     user in the definition. */
  void preprocess();

  attribute_set &attributes() { return attributes_; }
  [[nodiscard]] attribute_set const &attributes() const { return attributes_; }

  void add_module(sptr<kore_module> module);
  void print(std::ostream &out, unsigned indent = 0) const;

  /*
   * Return the set of sorts that are hooked to a particular hook name.
   * Typically, the set returned will be the singleton set. For example:
   *
   *   getSortsHookedTo("BYTES.Bytes") -> { "SortBytes" }
   *
   * If user K code hooks sorts to K's internal implementations (e.g. to
   * implement type-safe collections), those user sorts will be returned as
   * well.
   */
  [[nodiscard]] std::unordered_set<std::string>
  get_sorts_hooked_to(std::string const &hook_name) const;

  /*
   * Build this definition's subsort relation from axioms that have the
   * `subsort` attribute.
   *
   * The returned map is as follows:
   *
   *   S |-> {T . S is a subsort of T}
   */
  [[nodiscard]] SubsortMap get_subsorts() const;

  /*
   * Build this definition's overload relation from axioms that have the
   * `overload` attribute.
   *
   * The returned map is as follows:
   *
   *  P |-> {Q . P is a more specific overload of Q}
   */
  [[nodiscard]] SymbolMap get_overloads() const;

  [[nodiscard]] std::vector<sptr<kore_module>> const &get_modules() const {
    return modules_;
  }
  [[nodiscard]] kore_composite_sortDeclarationMapType const &
  get_sort_declarations() const {
    return sort_declarations_;
  }
  [[nodiscard]] kore_symbol_declarationMapType const &
  get_symbol_declarations() const {
    return symbol_declarations_;
  }
  [[nodiscard]] kore_alias_declarationMapType const &
  get_alias_declarations() const {
    return alias_declarations_;
  }
  [[nodiscard]] kore_symbolMapType const &get_symbols() const {
    return object_symbols_;
  }
  [[nodiscard]] kore_symbolStringMapType const &get_all_symbols() const {
    return all_object_symbols_;
  }
  [[nodiscard]] kore_composite_sortMapType get_hooked_sorts() const {
    return hooked_sorts_;
  }
  [[nodiscard]] std::list<kore_axiom_declaration *> const &get_axioms() const {
    return axioms_;
  }
  [[nodiscard]] kore_axiom_declaration *
  get_axiom_by_ordinal(size_t ordinal) const {
    return ordinals_.at(ordinal);
  }
  [[nodiscard]] kore_symbolStringMapType const &get_fresh_functions() const {
    return fresh_functions_;
  }
  kore_symbol *get_inj_symbol() { return inj_symbol_; }
};

void read_multimap(
    std::string const &, kore_symbol_declaration *,
    std::map<std::string, std::set<std::string>> &, attribute_set::key);

sptr<kore_pattern> strip_raw_term(sptr<kore_pattern> const &term);

std::string get_raw_symbol_name(sort_category);

namespace detail {

template <typename T>
struct header_byte_t;

#define VARIANT_HEADER(C, V)                                                   \
  template <>                                                                  \
  struct header_byte_t<C> {                                                    \
    static constexpr char value = V;                                           \
  }

VARIANT_HEADER(kore_composite_pattern, 0x4);
VARIANT_HEADER(kore_string_pattern, 0x5);
VARIANT_HEADER(kore_composite_sort, 0x6);
VARIANT_HEADER(kore_sort_variable, 0x7);
VARIANT_HEADER(kore_symbol, 0x8);
VARIANT_HEADER(kore_variable_pattern, 0x9);
VARIANT_HEADER(kore_variable, 0xD);

#undef VARIANT_HEADER

} // namespace detail

/**
 * Helps to make sure that different AST variants don't end up with conflicting
 * header bytes.
 */
template <typename T>
constexpr inline char header_byte = detail::header_byte_t<T>::value;

} // end namespace kllvm

#endif // AST_H
