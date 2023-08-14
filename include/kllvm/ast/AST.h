#ifndef AST_H
#define AST_H

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

class KORESortVariable;

struct HashSort;

template <typename T>
using ptr = std::unique_ptr<T>;

template <typename T>
using sptr = std::shared_ptr<T>;

std::string decodeKore(std::string);

// KORESort
class KORESort : public std::enable_shared_from_this<KORESort> {
public:
  using substitution
      = std::unordered_map<KORESortVariable, sptr<KORESort>, HashSort>;

  virtual bool isConcrete() const = 0;
  virtual sptr<KORESort> substitute(const substitution &) = 0;

  virtual bool operator==(const KORESort &other) const = 0;
  bool operator!=(const KORESort &other) const { return !(*this == other); }

  virtual void print(std::ostream &Out, unsigned indent = 0) const = 0;
  virtual void prettyPrint(std::ostream &Out) const = 0;
  virtual void serialize_to(serializer &s) const = 0;

  virtual ~KORESort() = default;
};

static inline std::ostream &operator<<(std::ostream &out, const KORESort &s) {
  s.print(out);
  return out;
}

struct HashSort {
  size_t operator()(const kllvm::KORESort &s) const noexcept {
    std::ostringstream Out;
    s.print(Out);
    return std::hash<std::string>{}(Out.str());
  }
};

struct EqualSortPtr {
  bool operator()(KORESort *const &first, KORESort *const &second) const {
    return *first == *second;
  }
};

struct HashSortPtr {
  size_t operator()(kllvm::KORESort *const &s) const noexcept {
    std::ostringstream Out;
    s->print(Out);
    return std::hash<std::string>{}(Out.str());
  }
};

size_t hash_value(const kllvm::KORESort &s);

class KORESortVariable : public KORESort {
private:
  std::string name;

public:
  static sptr<KORESortVariable> Create(const std::string &Name) {
    return sptr<KORESortVariable>(new KORESortVariable(Name));
  }

  virtual bool isConcrete() const override { return false; }
  virtual sptr<KORESort> substitute(const substitution &subst) override {
    return subst.at(*this);
  }

  virtual void print(std::ostream &Out, unsigned indent = 0) const override;
  virtual void prettyPrint(std::ostream &Out) const override;
  virtual void serialize_to(serializer &s) const override;

  virtual bool operator==(const KORESort &other) const override;

  std::string const &getName() const { return name; }

private:
  KORESortVariable(const std::string &Name)
      : name(Name) { }
};

enum class SortCategory {
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
struct ValueType {
  // fundamental category of the term
  SortCategory cat;
  // if this is an MInt, the number of bits in the MInt
  uint64_t bits;

  bool operator<(const ValueType &that) const {
    return std::make_tuple(this->cat, this->bits)
           < std::make_tuple(that.cat, that.bits);
  }
};

class KOREDefinition;

class KORECompositeSort : public KORESort {
private:
  std::string name;
  std::vector<sptr<KORESort>> arguments;
  ValueType category;

public:
  static sptr<KORECompositeSort> Create(
      const std::string &Name, ValueType Cat = {SortCategory::Uncomputed, 0}) {
    return sptr<KORECompositeSort>(new KORECompositeSort(Name, Cat));
  }

  const std::string getName() const { return name; }
  ValueType getCategory(KOREDefinition *definition);
  std::string getHook(KOREDefinition *definition);
  static ValueType getCategory(std::string hook);

  virtual bool isConcrete() const override;
  virtual sptr<KORESort> substitute(const substitution &subst) override;

  void addArgument(sptr<KORESort> Argument);
  virtual void print(std::ostream &Out, unsigned indent = 0) const override;
  virtual void prettyPrint(std::ostream &out) const override;
  virtual void serialize_to(serializer &s) const override;
  virtual bool operator==(const KORESort &other) const override;

  std::vector<sptr<KORESort>> const &getArguments() const { return arguments; }

private:
  KORECompositeSort(const std::string &Name, ValueType category)
      : name(Name)
      , category(category) { }
};

struct HashSymbol;

class KORESymbolDeclaration;

// KORESymbol
class KORESymbol {
private:
  std::string name;
  /* At parse time, when parsed as part of a pattern,
      this will be empty. When parsed as part of a declaration, it contains the
     signature of the symbol. After instantiateSymbol is called on a symbol that
     is part of a pattern, it changes from being empty to being the signature of
     the symbol. instantiateSymbol is called on all object level symbols in
     axioms when KOREDefinition::preprocess is called. */
  std::vector<sptr<KORESort>> arguments;
  /* contains the original arguments to the symbol when parsed as parh of a
   * pattern. */
  std::vector<sptr<KORESort>> formalArguments;
  /** At parse time, when parsed as part of a pattern, this will be null.
      When parsed as part of a declaration, it contains the return sort of the
     symbol. See above re: the behavior of KORESymbol with respect to
     instantiateSymbol. */
  sptr<KORESort> sort;
  /* the first integer in a continuous range representing the tags of all the
     polymorphic instantiations of this symbol. If the symbol has no parameters
     or its parameters are fully specified, firstTag == lastTag. */
  uint32_t firstTag;
  /* the last integer in a continuous range representing the tags of all the
     polymorphic instantiations of this symbol. If the symbol has no parameters
     or its parameters are fully specified, firstTag == lastTag. */
  uint32_t lastTag;
  /* A unique integer representing the layout of the symbol in memory.
     See CreateTerm.cpp for more information about the layout of K terms. */
  uint16_t layout;

public:
  static ptr<KORESymbol> Create(const std::string &Name) {
    return ptr<KORESymbol>(new KORESymbol(Name));
  }

  void addArgument(sptr<KORESort> Argument);
  void addFormalArgument(sptr<KORESort> Argument);
  void addSort(sptr<KORESort> Sort);
  void initPatternArguments(void) { arguments.swap(formalArguments); }

  const std::string &getName() const { return name; }
  const std::vector<sptr<KORESort>> &getArguments() const { return arguments; }
  const std::vector<sptr<KORESort>> &getFormalArguments() const {
    return formalArguments;
  }
  const sptr<KORESort> getSort() const { return sort; }
  sptr<KORESort> getSort() { return sort; }
  uint32_t getTag() const {
    assert(firstTag == lastTag);
    return firstTag;
  }
  uint32_t getFirstTag() const { return firstTag; }
  uint32_t getLastTag() const { return lastTag; }
  void setTag(uint32_t val) { firstTag = lastTag = val; }
  uint16_t getLayout() const { return layout; }

  void print(std::ostream &Out, unsigned indent = 0) const;
  void print(std::ostream &Out, unsigned indent, bool formal) const;
  void serialize_to(serializer &s) const;

  bool operator==(const KORESymbol &other) const;
  bool operator!=(const KORESymbol &other) const { return !(*this == other); }

  std::string layoutString(KOREDefinition *) const;

  bool isConcrete() const;
  bool isPolymorphic() const;
  bool isBuiltin() const;

  /* instantiates this symbol (which should be parsed from a pattern in an
     axiom) with the sorts corresponding to its actual sort parameters after
     instantiating polymorphic parameters. This happens by replacing the
     variables in the arguments of the specified declaration with their
     substitution in the arguments to the pattern that were parsed in braces.
     The result is that the arguments and sort fields are replaced with the
     instantiated signature of the symbol. */
  void instantiateSymbol(KORESymbolDeclaration *decl);

  friend HashSymbol;

  friend KOREDefinition;

private:
  KORESymbol(const std::string &Name)
      : name(Name)
      , sort(nullptr) { }
};

struct HashSymbol {
  size_t operator()(const kllvm::KORESymbol &s) const noexcept {
    size_t hash = 0;
    boost::hash_combine(hash, s.name);
    for (auto &arg : s.arguments) {
      boost::hash_combine(hash, *arg);
    }
    return hash;
  }
};

struct EqualSymbolPtr {
  bool operator()(KORESymbol *const &first, KORESymbol *const &second) const {
    std::ostringstream Out1, Out2;
    first->print(Out1);
    second->print(Out2);
    return Out1.str() == Out2.str();
  }
};

struct HashSymbolPtr {
  size_t operator()(kllvm::KORESymbol *const &s) const noexcept {
    std::ostringstream Out;
    s->print(Out);
    return std::hash<std::string>{}(Out.str());
  }
};

// KOREVariable
class KOREVariable {
private:
  std::string name;

public:
  static ptr<KOREVariable> Create(const std::string &Name) {
    return ptr<KOREVariable>(new KOREVariable(Name));
  }

  std::string getName() const;

  virtual void print(std::ostream &Out, unsigned indent = 0) const;
  virtual void serialize_to(serializer &s) const;

  virtual ~KOREVariable() = default;

private:
  KOREVariable(const std::string &Name)
      : name(Name) { }
};

class KOREVariablePattern;

using SortSet = std::unordered_set<KORESort *, HashSortPtr, EqualSortPtr>;
using SymbolSet
    = std::unordered_set<KORESymbol *, HashSymbolPtr, EqualSymbolPtr>;
using SubsortMap
    = std::unordered_map<KORESort *, SortSet, HashSortPtr, EqualSortPtr>;
using SymbolMap = std::unordered_map<
    KORESymbol *, SymbolSet, HashSymbolPtr, EqualSymbolPtr>;
using BracketMap = std::unordered_map<
    KORESort *, std::vector<KORESymbol *>, HashSortPtr, EqualSortPtr>;

struct PrettyPrintData {
  // map from symbol name to format attribute specifying how to print that
  // symbol
  std::map<std::string, std::string> format;
  // map from symbol name to vector of colors for that symbol
  std::map<std::string, std::vector<std::string>> colors;
  // map from symbol name to terminal pattern
  std::map<std::string, std::string> terminals;
  std::map<std::string, std::set<std::string>> priorities;
  std::map<std::string, std::set<std::string>> leftAssoc;
  std::map<std::string, std::set<std::string>> rightAssoc;
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
  bool hasColor;
};

class KOREDeclaration;
class KORECompositePattern;

// KOREPattern
class KOREPattern : public std::enable_shared_from_this<KOREPattern> {
public:
  /*
   * Load a pattern from disk, examining the first 4 bytes to see if it's a
   * binary file or a textual KORE file.
   */
  static sptr<KOREPattern> load(std::string const &filename);

  virtual ~KOREPattern() = default;

  virtual void print(std::ostream &Out, unsigned indent = 0) const = 0;

  virtual void serialize_to(serializer &s) const = 0;

  /* adds all the object level symbols contained recursively in the current
     pattern to the specified map, mapping their symbol name to the list of all
     instances of that symbol. */
  virtual void markSymbols(std::map<std::string, std::vector<KORESymbol *>> &)
      = 0;
  /* adds all the object level variables contained recursively in the current
     pattern to the specified map, mapping their variable name to the variable
     itself. */
  virtual void markVariables(std::map<std::string, KOREVariablePattern *> &)
      = 0;

  virtual sptr<KORESort> getSort(void) const = 0;

  using substitution = std::unordered_map<std::string, sptr<KOREPattern>>;

  virtual sptr<KOREPattern> substitute(const substitution &) = 0;
  virtual sptr<KOREPattern> expandAliases(KOREDefinition *) = 0;

  virtual void
  prettyPrint(std::ostream &, PrettyPrintData const &data) const = 0;
  virtual sptr<KOREPattern> sortCollections(PrettyPrintData const &data) = 0;
  std::set<std::string> gatherSingletonVars(void);
  virtual std::map<std::string, int> gatherVarCounts(void) = 0;
  virtual sptr<KOREPattern> filterSubstitution(
      PrettyPrintData const &data, std::set<std::string> const &vars)
      = 0;
  virtual sptr<KOREPattern> dedupeDisjuncts(void) = 0;
  virtual bool matches(
      substitution &subst, SubsortMap const &subsorts,
      SymbolMap const &overloads, sptr<KOREPattern> subject)
      = 0;
  sptr<KOREPattern> expandMacros(
      SubsortMap const &subsorts, SymbolMap const &overloads,
      std::vector<ptr<KOREDeclaration>> const &axioms, bool reverse);

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
  virtual sptr<KOREPattern> desugarAssociative() = 0;

  friend KORECompositePattern;

private:
  virtual sptr<KOREPattern> expandMacros(
      SubsortMap const &subsorts, SymbolMap const &overloads,
      std::vector<ptr<KOREDeclaration>> const &axioms, bool reverse,
      std::set<size_t> &appliedRules, std::set<std::string> const &macroSymbols)
      = 0;
};

void flatten(
    KORECompositePattern *pat, std::string name,
    std::vector<sptr<KOREPattern>> &result);

class KOREVariablePattern : public KOREPattern {
private:
  ptr<KOREVariable> name;
  sptr<KORESort> sort;

public:
  static ptr<KOREVariablePattern>
  Create(const std::string &Name, sptr<KORESort> sort) {
    ptr<KOREVariable> Var = KOREVariable::Create(Name);
    return ptr<KOREVariablePattern>(
        new KOREVariablePattern(std::move(Var), sort));
  }

  std::string getName() const;
  virtual sptr<KORESort> getSort() const override { return sort; }

  virtual void print(std::ostream &Out, unsigned indent = 0) const override;
  virtual void serialize_to(serializer &s) const override;

  virtual void
  markSymbols(std::map<std::string, std::vector<KORESymbol *>> &) override { }
  virtual void
  markVariables(std::map<std::string, KOREVariablePattern *> &map) override {
    map.insert({name->getName(), this});
  }
  virtual sptr<KOREPattern> substitute(const substitution &subst) override {
    auto val = subst.find(name->getName());
    if (val == subst.end()) {
      return shared_from_this();
    }
    return val->second;
  }
  virtual sptr<KOREPattern> expandAliases(KOREDefinition *) override {
    return shared_from_this();
  }
  virtual sptr<KOREPattern>
  sortCollections(PrettyPrintData const &data) override {
    return shared_from_this();
  }
  virtual sptr<KOREPattern> dedupeDisjuncts(void) override {
    return shared_from_this();
  }
  virtual std::map<std::string, int> gatherVarCounts(void) override {
    return std::map<std::string, int>{{name->getName(), 1}};
  }
  virtual sptr<KOREPattern> filterSubstitution(
      PrettyPrintData const &data, std::set<std::string> const &vars) override {
    return shared_from_this();
  }

  virtual sptr<KOREPattern> desugarAssociative() override {
    return shared_from_this();
  }

  virtual bool matches(
      substitution &subst, SubsortMap const &, SymbolMap const &,
      sptr<KOREPattern> subject) override;
  virtual void
  prettyPrint(std::ostream &out, PrettyPrintData const &data) const override;

private:
  virtual sptr<KOREPattern> expandMacros(
      SubsortMap const &, SymbolMap const &,
      std::vector<ptr<KOREDeclaration>> const &macros, bool reverse,
      std::set<size_t> &appliedRules,
      std::set<std::string> const &macroSymbols) override {
    return shared_from_this();
  }

private:
  KOREVariablePattern(ptr<KOREVariable> Name, sptr<KORESort> Sort)
      : name(std::move(Name))
      , sort(Sort) { }
};

void deallocateSPtrKorePattern(sptr<KOREPattern> pattern);

class KORECompositePattern : public KOREPattern {
private:
  ptr<KORESymbol> constructor;
  std::vector<sptr<KOREPattern>> arguments;

public:
  static ptr<KORECompositePattern> Create(const std::string &Name) {
    ptr<KORESymbol> Sym = KORESymbol::Create(Name);
    return ptr<KORECompositePattern>(new KORECompositePattern(std::move(Sym)));
  }
  static ptr<KORECompositePattern> Create(ptr<KORESymbol> Sym) {
    return ptr<KORECompositePattern>(new KORECompositePattern(std::move(Sym)));
  }
  static ptr<KORECompositePattern> Create(KORESymbol *Sym) {
    ptr<KORESymbol> newSym = KORESymbol::Create(Sym->getName());
    *newSym = *Sym;
    return ptr<KORECompositePattern>(
        new KORECompositePattern(std::move(newSym)));
  }

  sptr<KORESort> getSort() const override { return constructor->getSort(); }

  KORESymbol *getConstructor() const { return constructor.get(); }
  const std::vector<sptr<KOREPattern>> &getArguments() const {
    return arguments;
  }

  void addArgument(sptr<KOREPattern> Argument);

  virtual void print(std::ostream &Out, unsigned indent = 0) const override;
  virtual void serialize_to(serializer &s) const override;

  virtual void
  prettyPrint(std::ostream &out, PrettyPrintData const &data) const override;
  virtual void
  markSymbols(std::map<std::string, std::vector<KORESymbol *>> &) override;
  virtual void
  markVariables(std::map<std::string, KOREVariablePattern *> &) override;
  virtual sptr<KOREPattern> substitute(const substitution &) override;
  virtual sptr<KOREPattern> expandAliases(KOREDefinition *) override;
  virtual sptr<KOREPattern>
  sortCollections(PrettyPrintData const &data) override;
  virtual sptr<KOREPattern> dedupeDisjuncts(void) override;
  virtual std::map<std::string, int> gatherVarCounts(void) override;
  virtual sptr<KOREPattern> desugarAssociative() override;
  virtual sptr<KOREPattern> filterSubstitution(
      PrettyPrintData const &data, std::set<std::string> const &vars) override;
  virtual bool matches(
      substitution &, SubsortMap const &, SymbolMap const &,
      sptr<KOREPattern>) override;

private:
  virtual sptr<KOREPattern> expandMacros(
      SubsortMap const &, SymbolMap const &,
      std::vector<ptr<KOREDeclaration>> const &macros, bool reverse,
      std::set<size_t> &appliedRules,
      std::set<std::string> const &macroSymbols) override;

  friend void ::kllvm::deallocateSPtrKorePattern(sptr<KOREPattern> pattern);

private:
  KORECompositePattern(ptr<KORESymbol> Constructor)
      : constructor(std::move(Constructor)) { }
};

class KOREStringPattern : public KOREPattern {
private:
  std::string contents;

public:
  static ptr<KOREStringPattern> Create(const std::string &Contents) {
    return ptr<KOREStringPattern>(new KOREStringPattern(Contents));
  }

  std::string getContents() { return contents; }

  virtual void print(std::ostream &Out, unsigned indent = 0) const override;
  virtual void serialize_to(serializer &s) const override;
  virtual void
  prettyPrint(std::ostream &out, PrettyPrintData const &data) const override {
    abort();
  }

  virtual void
  markSymbols(std::map<std::string, std::vector<KORESymbol *>> &) override { }
  virtual void
  markVariables(std::map<std::string, KOREVariablePattern *> &) override { }
  virtual sptr<KORESort> getSort(void) const override { abort(); }
  virtual sptr<KOREPattern> substitute(const substitution &) override {
    return shared_from_this();
  }
  virtual sptr<KOREPattern> expandAliases(KOREDefinition *) override {
    return shared_from_this();
  }
  virtual sptr<KOREPattern>
  sortCollections(PrettyPrintData const &data) override {
    return shared_from_this();
  }
  virtual sptr<KOREPattern> dedupeDisjuncts(void) override {
    return shared_from_this();
  }
  virtual std::map<std::string, int> gatherVarCounts(void) override {
    return std::map<std::string, int>{};
  }

  virtual sptr<KOREPattern> desugarAssociative() override {
    return shared_from_this();
  }

  virtual sptr<KOREPattern> filterSubstitution(
      PrettyPrintData const &data, std::set<std::string> const &var) override {
    return shared_from_this();
  }
  virtual bool matches(
      substitution &, SubsortMap const &, SymbolMap const &,
      sptr<KOREPattern> subject) override;

private:
  virtual sptr<KOREPattern> expandMacros(
      SubsortMap const &, SymbolMap const &,
      std::vector<ptr<KOREDeclaration>> const &macros, bool reverse,
      std::set<size_t> &appliedRules,
      std::set<std::string> const &macroSymbols) override {
    return shared_from_this();
  }

private:
  KOREStringPattern(const std::string &Contents)
      : contents(Contents) { }
};

// A Bytes domain value is represented as a KOREStringPattern by storing each
// byte 0xHH as the (UTF-8 encoded) Unicode codepoint U+00HH.
//
// Given the contents of such a KOREStringPattern, this function produces the
// actual char[] for the Bytes domain value. In effect, this is just a conversion
// from UTF-8 to latin-1.
//
// The input buffer is overwritten, and the new length is returned.
size_t bytesStringPatternToBytes(char *contents, size_t length);

// KOREDeclaration
class KOREDeclaration {
protected:
  std::unordered_map<std::string, sptr<KORECompositePattern>> attributes;
  std::vector<sptr<KORESortVariable>> objectSortVariables;

public:
  void addAttribute(sptr<KORECompositePattern> Attribute);
  void addObjectSortVariable(sptr<KORESortVariable> SortVariable);
  virtual void print(std::ostream &Out, unsigned indent = 0) const = 0;

  const std::unordered_map<std::string, sptr<KORECompositePattern>> &
  getAttributes() const {
    return attributes;
  }
  const std::vector<sptr<KORESortVariable>> &getObjectSortVariables() const {
    return objectSortVariables;
  }
  virtual ~KOREDeclaration() = default;

  std::string getStringAttribute(std::string name) const;

protected:
  void printSortVariables(std::ostream &Out) const;
};

class KORECompositeSortDeclaration : public KOREDeclaration {
private:
  bool _isHooked;
  std::string sortName;

public:
  static ptr<KORECompositeSortDeclaration>
  Create(const std::string &Name, bool isHooked = false) {
    return ptr<KORECompositeSortDeclaration>(
        new KORECompositeSortDeclaration(Name, isHooked));
  }

  std::string getName() const { return sortName; }
  bool isHooked() const { return _isHooked; }

  virtual void print(std::ostream &Out, unsigned indent = 0) const override;

private:
  KORECompositeSortDeclaration(const std::string &Name, bool _isHooked)
      : _isHooked(_isHooked)
      , sortName(Name) { }
};

class KORESymbolAliasDeclaration : public KOREDeclaration {
protected:
  ptr<KORESymbol> symbol;

  KORESymbolAliasDeclaration(ptr<KORESymbol> Symbol)
      : symbol(std::move(Symbol)) { }

public:
  KORESymbol *getSymbol() const { return symbol.get(); }
};

class KORESymbolDeclaration : public KORESymbolAliasDeclaration {
private:
  bool _isHooked;

public:
  static ptr<KORESymbolDeclaration>
  Create(const std::string &Name, bool isHooked = false) {
    ptr<KORESymbol> Sym = KORESymbol::Create(Name);
    return ptr<KORESymbolDeclaration>(
        new KORESymbolDeclaration(std::move(Sym), isHooked));
  }

  bool isHooked() const { return _isHooked; }

  bool isAnywhere() const;

  virtual void print(std::ostream &Out, unsigned indent = 0) const override;

private:
  KORESymbolDeclaration(ptr<KORESymbol> Symbol, bool _isHooked)
      : KORESymbolAliasDeclaration(std::move(Symbol))
      , _isHooked(_isHooked) { }
};

class KOREAliasDeclaration : public KORESymbolAliasDeclaration {
private:
  sptr<KORECompositePattern> boundVariables;
  sptr<KOREPattern> pattern;

public:
  static ptr<KOREAliasDeclaration> Create(const std::string &Name) {
    ptr<KORESymbol> Sym = KORESymbol::Create(Name);
    return ptr<KOREAliasDeclaration>(new KOREAliasDeclaration(std::move(Sym)));
  }

  void addVariables(sptr<KORECompositePattern> variables);
  void addPattern(sptr<KOREPattern> Pattern);
  KOREPattern::substitution getSubstitution(KORECompositePattern *subject);
  KORECompositePattern *getBoundVariables() const {
    return boundVariables.get();
  }
  sptr<KOREPattern> &getPattern() { return pattern; }
  virtual void print(std::ostream &Out, unsigned indent = 0) const override;

private:
  KOREAliasDeclaration(ptr<KORESymbol> Symbol)
      : KORESymbolAliasDeclaration(std::move(Symbol)) { }
};

class KOREAxiomDeclaration : public KOREDeclaration {
private:
  sptr<KOREPattern> pattern;
  unsigned ordinal;
  bool _isClaim;

  KOREAxiomDeclaration(bool isClaim)
      : _isClaim(isClaim) { }

public:
  static ptr<KOREAxiomDeclaration> Create(bool isClaim = false) {
    return ptr<KOREAxiomDeclaration>(new KOREAxiomDeclaration(isClaim));
  }

  void addPattern(sptr<KOREPattern> Pattern);
  virtual void print(std::ostream &Out, unsigned indent = 0) const override;

  /* returns true if the axiom is actually required to be translated to llvm
     and false if it is an axiom pertaining to symbolic execution which is not
     required for concrete execution. Axioms that are not required are elided
     from the definition by KOREDefinition::preprocess. */
  bool isRequired() const;
  bool isTopAxiom() const;
  bool isClaim() const { return _isClaim; }
  KOREPattern *getRightHandSide() const;
  std::vector<KOREPattern *> getLeftHandSide() const;
  KOREPattern *getRequires() const;
  sptr<KOREPattern> getPattern() const { return pattern; }
  unsigned getOrdinal() const { return ordinal; }

  friend KOREDefinition;
};

class KOREModuleImportDeclaration : public KOREDeclaration {
private:
  std::string moduleName;

public:
  static ptr<KOREModuleImportDeclaration> Create(const std::string &Name) {
    return ptr<KOREModuleImportDeclaration>(
        new KOREModuleImportDeclaration(Name));
  }

  const std::string &getModuleName() const { return moduleName; }

  virtual void print(std::ostream &Out, unsigned indent = 0) const override;

private:
  KOREModuleImportDeclaration(const std::string &Name)
      : moduleName(Name) { }
};

// KOREModule
class KOREModule {
private:
  std::string name;
  std::vector<sptr<KOREDeclaration>> declarations;
  std::unordered_map<std::string, sptr<KORECompositePattern>> attributes;

public:
  static ptr<KOREModule> Create(const std::string &Name) {
    return ptr<KOREModule>(new KOREModule(Name));
  }

  void addAttribute(sptr<KORECompositePattern> Attribute);
  void addDeclaration(sptr<KOREDeclaration> Declaration);
  void print(std::ostream &Out, unsigned indent = 0) const;

  const std::string &getName() const { return name; }
  const std::unordered_map<std::string, sptr<KORECompositePattern>> &
  getAttributes() const {
    return attributes;
  }
  const std::vector<sptr<KOREDeclaration>> &getDeclarations() const {
    return declarations;
  }

private:
  KOREModule(const std::string &Name)
      : name(Name) { }
};

// KOREDefinition
class KOREDefinition {
public:
  // Symbol table types
  using KOREModuleMapType = std::map<std::string, KOREModule *>;

  using KORESortConstructorMapType = std::map<std::string, KORECompositeSort *>;

  using KORESymbolMapType = std::map<uint32_t, KORESymbol *>;

  using KORESymbolStringMapType = std::map<std::string, KORESymbol *>;

  using KORESortVariableMapType = std::map<std::string, KORESortVariable *>;

  using KOREVariableMapType = std::map<std::string, KOREVariable *>;

  using KORECompositeSortDeclarationMapType
      = std::map<std::string, KORECompositeSortDeclaration *>;
  using KORECompositeSortMapType = std::map<ValueType, sptr<KORECompositeSort>>;

  using KORESymbolDeclarationMapType
      = std::map<std::string, KORESymbolDeclaration *>;
  using KOREAliasDeclarationMapType
      = std::map<std::string, KOREAliasDeclaration *>;

  using KOREAxiomMapType = std::map<size_t, KOREAxiomDeclaration *>;

private:
  // Symbol tables
  KORESortConstructorMapType objectSortConstructors;
  KORESymbolMapType objectSymbols;
  KORESymbolStringMapType allObjectSymbols;
  KORESortVariableMapType objectSortVariables;
  KOREVariableMapType objectVariables;
  KOREModuleMapType moduleNames;
  KORECompositeSortDeclarationMapType sortDeclarations;
  KORESymbolDeclarationMapType symbolDeclarations;
  KOREAliasDeclarationMapType aliasDeclarations;
  KORECompositeSortMapType hookedSorts;
  KORESymbolStringMapType freshFunctions;
  KOREAxiomMapType ordinals;

  std::vector<sptr<KOREModule>> modules;
  std::unordered_map<std::string, sptr<KORECompositePattern>> attributes;
  /* an automatically computed list of all the axioms in the definition */
  std::list<KOREAxiomDeclaration *> axioms;

  KORESymbol *injSymbol;

public:
  static ptr<KOREDefinition> Create() {
    return ptr<KOREDefinition>(new KOREDefinition());
  }

  /* Preprocesses the definition and prepares it for translation to llvm.
     This performs the following tasks:
     * removes axioms for which isRequired() returns false
     * sets the arguments field for each KORESymbol to the actual instantiated
       sort arguments of the symbol (rather than just their polymorphic
     parameters
     * sets the tag and layout fields on all the KORESymbols declared by the
     user in the definition. */
  void preprocess();

  void addModule(sptr<KOREModule> Module);
  void addAttribute(sptr<KORECompositePattern> Attribute);
  void print(std::ostream &Out, unsigned indent = 0) const;

  const std::vector<sptr<KOREModule>> &getModules() const { return modules; }
  const KORECompositeSortDeclarationMapType &getSortDeclarations() const {
    return sortDeclarations;
  }
  const KORESymbolDeclarationMapType &getSymbolDeclarations() const {
    return symbolDeclarations;
  }
  const KOREAliasDeclarationMapType &getAliasDeclarations() const {
    return aliasDeclarations;
  }
  const KORESymbolMapType &getSymbols() const { return objectSymbols; }
  const KORESymbolStringMapType &getAllSymbols() const {
    return allObjectSymbols;
  }
  const KORECompositeSortMapType getHookedSorts() const { return hookedSorts; }
  const std::list<KOREAxiomDeclaration *> &getAxioms() const { return axioms; }
  KOREAxiomDeclaration *getAxiomByOrdinal(size_t ordinal) const {
    return ordinals.at(ordinal);
  }
  const std::unordered_map<std::string, sptr<KORECompositePattern>> &
  getAttributes() const {
    return attributes;
  }
  const KORESymbolStringMapType &getFreshFunctions() const {
    return freshFunctions;
  }
  KORESymbol *getInjSymbol() { return injSymbol; }
};

void readMultimap(
    std::string, KORESymbolDeclaration *,
    std::map<std::string, std::set<std::string>> &, std::string);

template <typename Elem, typename Hash, typename Equal>
std::unordered_map<Elem *, std::unordered_set<Elem *, Hash, Equal>, Hash, Equal>
transitiveClosure(std::unordered_map<
                  Elem *, std::unordered_set<Elem *, Hash, Equal>, Hash, Equal>
                      relations) {
  bool dirty;
  do {
    dirty = false;
    for (auto &entry : relations) {
      SortSet newSucc;
      for (auto &elem : entry.second) {
        auto &relation = relations[elem];
        for (auto elem2 : relation) {
          dirty |= relations[entry.first].insert(elem2).second;
        }
      }
    }
  } while (dirty);
  return relations;
}

namespace detail {

template <typename T>
struct header_byte_t;

#define VARIANT_HEADER(C, V)                                                   \
  template <>                                                                  \
  struct header_byte_t<C> {                                                    \
    static constexpr std::byte value = std::byte(V);                           \
  }

VARIANT_HEADER(KORECompositePattern, 0x4);
VARIANT_HEADER(KOREStringPattern, 0x5);
VARIANT_HEADER(KORECompositeSort, 0x6);
VARIANT_HEADER(KORESortVariable, 0x7);
VARIANT_HEADER(KORESymbol, 0x8);
VARIANT_HEADER(KOREVariablePattern, 0x9);
VARIANT_HEADER(KOREVariable, 0xD);

#undef VARIANT_HEADER

} // namespace detail

/**
 * Helps to make sure that different AST variants don't end up with conflicting
 * header bytes.
 */
template <typename T>
constexpr inline std::byte header_byte = detail::header_byte_t<T>::value;

} // end namespace kllvm

#endif // AST_H
