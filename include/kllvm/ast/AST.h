#ifndef AST_H
#define AST_H

#include <boost/functional/hash.hpp>

#include <string>
#include <vector>
#include <list>
#include <map>
#include <unordered_map>

namespace kllvm {

template<typename T>
using ptr = std::unique_ptr<T>;


// KORESort
class KORESort {
public:
  virtual void print(std::ostream &Out, unsigned indent = 0) const =0;
  virtual ~KORESort() = default;
};

static inline std::ostream &operator<<(std::ostream &out, const KORESort &s) { s.print(out); return out; }

struct HashSort {
  size_t operator()(const kllvm::KORESort &s) const noexcept {
    std::ostringstream Out;
    s.print(Out);
    return std::hash<std::string>{}(Out.str());
  }
};

size_t hash_value(const kllvm::KORESort &s);

class KOREObjectSortVariable;

class KOREObjectSort : public KORESort {
public:
  using substitution = std::unordered_map<KOREObjectSortVariable, KOREObjectSort *, HashSort>;

  virtual bool isConcrete() const = 0;
  virtual KOREObjectSort *substitute(const substitution &) = 0;

  virtual bool operator==(const KOREObjectSort &other) const = 0;
  bool operator!=(const KOREObjectSort &other) const { return !(*this == other); }
};

class KOREMetaSort : public KORESort {

};

class KOREObjectSortVariable : public KOREObjectSort {
private:
  std::string name;

public:
  static ptr<KOREObjectSortVariable> Create(const std::string &Name) {
    return ptr<KOREObjectSortVariable>(new KOREObjectSortVariable(Name));
  }

  virtual bool isConcrete() const override { return false; }
  virtual KOREObjectSort *substitute(const substitution &subst) override { return subst.at(*this); }

  virtual void print(std::ostream &Out, unsigned indent = 0) const override;
  virtual bool operator==(const KOREObjectSort &other) const override;

private:
  KOREObjectSortVariable(const std::string &Name) : name(Name) { }
};

class KOREMetaSortVariable : public KOREMetaSort {
private:
  std::string name;

public:
  static ptr<KOREMetaSortVariable> Create(const std::string &Name) {
    return ptr<KOREMetaSortVariable>(new KOREMetaSortVariable(Name));
  }

  virtual void print(std::ostream &Out, unsigned indent = 0) const override;

private:
  KOREMetaSortVariable(const std::string &Name) : name(Name) { }
};

enum class SortCategory {
  Uncomputed, Map, List, Set, Int, Float, StringBuffer, Bool, Symbol, Variable, MInt
};

// represents the syntactic category of an LLVM backend term at runtime
struct ValueType {
  // fundamental category of the term
  SortCategory cat;
  // if this is an MInt, the number of bits in the MInt
  uint64_t bits;
};

class KOREDefinition;

class KOREObjectCompositeSort : public KOREObjectSort {
private:
  std::string name;
  std::vector<ptr<KOREObjectSort>> arguments;
  ValueType category;

public:
  static ptr<KOREObjectCompositeSort> Create(const std::string &Name, ValueType Cat = {SortCategory::Uncomputed, 0}) {
    return ptr<KOREObjectCompositeSort>(new KOREObjectCompositeSort(Name, Cat));
  }

  const std::string getName() const { return name; }
  ValueType getCategory(KOREDefinition *definition);
  std::string getHook(KOREDefinition *definition);
  static ValueType getCategory(std::string hook);

  virtual bool isConcrete() const override { return true; }
  virtual KOREObjectSort *substitute(const substitution &subst) override;

  void addArgument(KOREObjectSort *Argument);
  virtual void print(std::ostream &Out, unsigned indent = 0) const override;
  virtual bool operator==(const KOREObjectSort &other) const override;

private:
  KOREObjectCompositeSort(const std::string &Name, ValueType category) : name(Name), category(category) {}
};

class KOREMetaCompositeSort : public KOREMetaSort {
private:
  std::string name; // only predefined names allowed

public:
  static ptr<KOREMetaCompositeSort> Create(const std::string &Name) {
    return ptr<KOREMetaCompositeSort>(new KOREMetaCompositeSort(Name));
  }

  virtual void print(std::ostream &Out, unsigned indent = 0) const override;

private:
  KOREMetaCompositeSort(const std::string &Name) : name(Name) { }
};

// KORESymbol
class KORESymbol {
public:
  virtual void print(std::ostream &Out, unsigned indent = 0) const =0;
  virtual ~KORESymbol() = default;
};

struct HashSymbol;

class KOREObjectSymbolDeclaration;

class KOREObjectSymbol : public KORESymbol {
private:
  std::string name;
  /* At parse time, when parsed as part of a pattern,
      this will be empty. When parsed as part of a declaration, it contains the signature
      of the symbol. After instantiateSymbol is called on a symbol that is
      part of a pattern, it changes from being empty to being
      the signature of the symbol. instantiateSymbol is called on all object
      level symbols in axioms when KOREDefinition::preprocess is called. */
  std::vector<ptr<KOREObjectSort>> arguments;
  /* contains the original arguments to the symbol when parsed as parh of a pattern. */
  std::vector<ptr<KOREObjectSort>> formalArguments;
  /** At parse time, when parsed as part of a pattern, this will be null.
      When parsed as part of a declaration, it contains the return sort of the symbol.
      See above re: the behavior of KOREObjectSymbol with respect to instantiateSymbol. */
  ptr<KOREObjectSort> sort;
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
  static ptr<KOREObjectSymbol> Create(const std::string &Name) {
    return ptr<KOREObjectSymbol>(new KOREObjectSymbol(Name));
  }

  void addArgument(ptr<KOREObjectSort> Argument);
  void addFormalArgument(ptr<KOREObjectSort> Argument);
  void addSort(ptr<KOREObjectSort> Sort);

  const std::string &getName() const { return name; }
  const std::vector<ptr<KOREObjectSort>> &getArguments() const {
    return arguments;
  }
  const std::vector<ptr<KOREObjectSort>> &getFormalArguments() const {
    return formalArguments;
  }
  const KOREObjectSort *getSort() const { return sort.get(); }
  KOREObjectSort *getSort() { return sort.get(); }
  uint32_t getTag() const { assert(firstTag == lastTag); return firstTag; }
  uint32_t getFirstTag() const { return firstTag; }
  uint32_t getLastTag() const { return lastTag; }
  void setTag(uint32_t val) { firstTag = lastTag = val; }
  uint16_t getLayout() const { return layout; }

  virtual void print(std::ostream &Out, unsigned indent = 0) const override;
  virtual void print(std::ostream &Out, unsigned indent, bool formal) const;

  bool operator==(KOREObjectSymbol &other) const;
  bool operator!=(KOREObjectSymbol &other) const { return !(*this == other); }

  std::string layoutString(KOREDefinition *) const;

  bool isConcrete() const;
  bool isPolymorphic() const;
  bool isBuiltin() const;

  /* instantiates this symbol (which should be parsed from a pattern in an axiom)
     with the sorts corresponding to its actual sort parameters after instantiating
     polymorphic parameters. This happens by replacing the variables in the arguments
     of the specified declaration with their substitution in the arguments to the pattern
     that were parsed in braces. The result is that the arguments and sort fields are replaced
     with the instantiated signature of the symbol. */
  void instantiateSymbol(KOREObjectSymbolDeclaration *decl);

  friend HashSymbol;

  friend KOREDefinition;

private:
  KOREObjectSymbol(const std::string &Name) : name(Name), sort(nullptr) { }
};

struct HashSymbol {
  size_t operator()(const kllvm::KOREObjectSymbol &s) const noexcept {
    size_t hash = 0;
    boost::hash_combine(hash, s.name);
    for (auto &arg : s.arguments) {
      boost::hash_combine(hash, *arg);
    }
    return hash;
  }
};

class KOREMetaSymbol : public KORESymbol {
private:
  std::string name;
  std::vector<ptr<KOREMetaSort>> arguments;
  ptr<KOREMetaSort> sort;

public:
  static ptr<KOREMetaSymbol> Create(const std::string &Name) {
    return ptr<KOREMetaSymbol>(new KOREMetaSymbol(Name));
  }

  void addArgument(ptr<KOREMetaSort> Argument);
  void addSort(ptr<KOREMetaSort> Sort);

  const std::string &getName() const { return name; }
  const std::vector<ptr<KOREMetaSort>> &getArguments() const {
    return arguments;
  }
  const KOREMetaSort *getSort() const { return sort.get(); }

  virtual void print(std::ostream &Out, unsigned indent = 0) const override;

private:
  KOREMetaSymbol(const std::string &Name) : name(Name), sort(nullptr) { }
};

// KOREVariable
class KOREVariable {
public:
  virtual void print(std::ostream &Out, unsigned indent = 0) const =0;
  virtual ~KOREVariable() = default;
};

class KOREObjectVariable : public KOREVariable {
private:
  std::string name;

public:
  static ptr<KOREObjectVariable> Create(const std::string &Name) {
    return ptr<KOREObjectVariable>(new KOREObjectVariable(Name));
  }

  std::string getName() const;

  virtual void print(std::ostream &Out, unsigned indent = 0) const override;

private:
  KOREObjectVariable(const std::string &Name) : name(Name) { }
};

class KOREMetaVariable : public KOREVariable {
private:
  std::string name;

public:
  static ptr<KOREMetaVariable> Create(const std::string &Name) {
    return ptr<KOREMetaVariable>(new KOREMetaVariable(Name));
  }

  virtual void print(std::ostream &Out, unsigned indent = 0) const override;

private:
  KOREMetaVariable(const std::string &Name) : name(Name) { }
};

class KOREObjectVariablePattern;

// KOREPattern
class KOREPattern {
public:
  virtual ~KOREPattern() = default;

  virtual void print(std::ostream &Out, unsigned indent = 0) const =0;
  /* adds all the object level symbols contained recursively in the current pattern
     to the specified map, mapping their symbol name to the list of all instances
     of that symbol. */
  virtual void markSymbols(std::map<std::string, std::vector<KOREObjectSymbol *>> &) = 0;
  /* adds all the object level variables contained recursively in the current pattern
     to the specified map, mapping their variable name to the variable itself. */
  virtual void markVariables(std::map<std::string, KOREObjectVariablePattern *> &) = 0;
};

class KOREObjectPattern : public KOREPattern {
public:
  virtual KOREObjectSort *getSort(void) const = 0;
};

class KOREMetaPattern : public KOREPattern {

};

class KOREObjectVariablePattern : public KOREObjectPattern {
private:
  ptr<KOREObjectVariable> name;
  ptr<KOREObjectSort> sort;

public:
  static ptr<KOREObjectVariablePattern>
  Create(const std::string &Name, ptr<KOREObjectSort> sort) {
    ptr<KOREObjectVariable> Var = KOREObjectVariable::Create(Name);
    return ptr<KOREObjectVariablePattern>(new KOREObjectVariablePattern(std::move(Var), std::move(sort)));
  }

  std::string getName() const;
  virtual KOREObjectSort *getSort() const override { return sort.get(); }

  virtual void print(std::ostream &Out, unsigned indent = 0) const override;
  virtual void markSymbols(std::map<std::string, std::vector<KOREObjectSymbol *>> &) override {}
  virtual void markVariables(std::map<std::string, KOREObjectVariablePattern *> &map) override { map.insert({name->getName(), this}); }

private:
  KOREObjectVariablePattern(ptr<KOREObjectVariable> Name, ptr<KOREObjectSort> Sort)
  : name(std::move(Name)), sort(std::move(Sort)) { }
};

class KOREMetaVariablePattern : public KOREMetaPattern {
private:
  ptr<KOREMetaVariable> name;
  ptr<KOREMetaSort> sort;

public:
  static ptr<KOREMetaVariablePattern>
  Create(const std::string &Name, ptr<KOREMetaSort> sort) {
    ptr<KOREMetaVariable> Var = KOREMetaVariable::Create(Name);
    return ptr<KOREMetaVariablePattern>(new KOREMetaVariablePattern(std::move(Var), std::move(sort)));
  }

  virtual void print(std::ostream &Out, unsigned indent = 0) const override;
  virtual void markSymbols(std::map<std::string, std::vector<KOREObjectSymbol *>> &) override {}
  virtual void markVariables(std::map<std::string, KOREObjectVariablePattern *> &) override {}

private:
  KOREMetaVariablePattern(ptr<KOREMetaVariable> Name, ptr<KOREMetaSort> Sort)
  : name(std::move(Name)), sort(std::move(Sort)) { }
};

class KOREObjectCompositePattern : public KOREObjectPattern {
private:
  ptr<KOREObjectSymbol> constructor;
  std::vector<ptr<KOREPattern>> arguments;

public:
  static ptr<KOREObjectCompositePattern> Create(const std::string &Name) {
    ptr<KOREObjectSymbol> Sym = KOREObjectSymbol::Create(Name);
    return ptr<KOREObjectCompositePattern>(new KOREObjectCompositePattern(std::move(Sym)));
  }
  static ptr<KOREObjectCompositePattern> Create(ptr<KOREObjectSymbol> Sym) {
    return ptr<KOREObjectCompositePattern>(new KOREObjectCompositePattern(std::move(Sym)));
  }

  KOREObjectSort *getSort() const override { return constructor->getSort(); }

  KOREObjectSymbol *getConstructor() const { return constructor.get(); }
  const std::vector<ptr<KOREPattern>> &getArguments() const { return arguments; }

  void addArgument(ptr<KOREPattern> Argument);
  virtual void print(std::ostream &Out, unsigned indent = 0) const override;
  virtual void markSymbols(std::map<std::string, std::vector<KOREObjectSymbol *>> &) override;
  virtual void markVariables(std::map<std::string, KOREObjectVariablePattern *> &) override;

private:
  KOREObjectCompositePattern(ptr<KOREObjectSymbol> Constructor)
  : constructor(std::move(Constructor)) { }
};

class KOREMetaCompositePattern : public KOREMetaPattern {
private:
  ptr<KOREMetaSymbol> constructor;
  std::vector<ptr<KOREPattern>> arguments;

public:
  static ptr<KOREMetaCompositePattern> Create(const std::string &Name) {
    ptr<KOREMetaSymbol> Sym = KOREMetaSymbol::Create(Name);
    return ptr<KOREMetaCompositePattern>(new KOREMetaCompositePattern(std::move(Sym)));
  }

  KOREMetaSymbol *getConstructor() const { return constructor.get(); }

  void addArgument(ptr<KOREPattern> Argument);
  virtual void print(std::ostream &Out, unsigned indent = 0) const override;
  virtual void markSymbols(std::map<std::string, std::vector<KOREObjectSymbol *>> &) override;
  virtual void markVariables(std::map<std::string, KOREObjectVariablePattern *> &) override;

private:
  KOREMetaCompositePattern(ptr<KOREMetaSymbol> Constructor)
  : constructor(std::move(Constructor)) { }
};

class KOREMetaStringPattern : public KOREMetaPattern {
private:
  std::string contents;

public:
  static ptr<KOREMetaStringPattern> Create(const std::string &Contents) {
    return ptr<KOREMetaStringPattern>(new KOREMetaStringPattern(Contents));
  }

  std::string getContents() { return contents; }

  virtual void print(std::ostream &Out, unsigned indent = 0) const override;
  virtual void markSymbols(std::map<std::string, std::vector<KOREObjectSymbol *>> &) override {}
  virtual void markVariables(std::map<std::string, KOREObjectVariablePattern *> &) override {}

private:
  KOREMetaStringPattern(const std::string &Contents) : contents(Contents) { }
};

class KOREMetaCharPattern : public KOREMetaPattern {
private:
  char contents;

public:
  static ptr<KOREMetaCharPattern> Create(char Contents) {
    return ptr<KOREMetaCharPattern>(new KOREMetaCharPattern(Contents));
  }

  virtual void print(std::ostream &Out, unsigned indent = 0) const override;
  virtual void markSymbols(std::map<std::string, std::vector<KOREObjectSymbol *>> &) override {}
  virtual void markVariables(std::map<std::string, KOREObjectVariablePattern *> &) override {}

private:
  KOREMetaCharPattern(char Contents) : contents(Contents) { }
};

// KOREDeclaration
class KOREDeclaration {
protected:
  std::map<std::string, ptr<KOREObjectCompositePattern>> attributes;
  std::vector<ptr<KOREObjectSortVariable>> objectSortVariables;
  std::vector<ptr<KOREMetaSortVariable>> metaSortVariables;

public:
  void addAttribute(ptr<KOREPattern> Attribute);
  void addObjectSortVariable(ptr<KOREObjectSortVariable> SortVariable);
  void addMetaSortVariable(ptr<KOREMetaSortVariable> SortVariable);
  virtual void print(std::ostream &Out, unsigned indent = 0) const =0;
  virtual ~KOREDeclaration() = default;
  const std::map<std::string, ptr<KOREObjectCompositePattern>> &getAttributes() const { return attributes; }
  const std::vector<ptr<KOREObjectSortVariable>> &getObjectSortVariables() const { return objectSortVariables; }

protected:
  void printSortVariables(std::ostream &Out) const;
};

class KOREObjectCompositeSortDeclaration : public KOREDeclaration {
private:
  bool _isHooked;
  std::string sortName;

public:
  static ptr<KOREObjectCompositeSortDeclaration>
  Create(const std::string &Name, bool isHooked = false) {
    return ptr<KOREObjectCompositeSortDeclaration>(new KOREObjectCompositeSortDeclaration(Name, isHooked));
  }

  std::string getName() const { return sortName; }
  bool isHooked() const { return _isHooked; }

  virtual void print(std::ostream &Out, unsigned indent = 0) const override;

private:
  KOREObjectCompositeSortDeclaration(const std::string &Name, bool _isHooked)
  : _isHooked(_isHooked), sortName(Name) { }
};

class KORESymbolDeclaration : public KOREDeclaration {

};

class KOREObjectSymbolAliasDeclaration : public KORESymbolDeclaration {
protected:
  ptr<KOREObjectSymbol> symbol;

  KOREObjectSymbolAliasDeclaration(ptr<KOREObjectSymbol> Symbol)
  : symbol(std::move(Symbol)) { }

public:
  KOREObjectSymbol *getSymbol() const { return symbol.get(); }
};

class KOREMetaSymbolAliasDeclaration : public KORESymbolDeclaration {
protected:
  ptr<KOREMetaSymbol> symbol;

  KOREMetaSymbolAliasDeclaration(ptr<KOREMetaSymbol> Symbol)
  : symbol(std::move(Symbol)) { }

public:
  KOREMetaSymbol *getSymbol() const { return symbol.get(); }
};

class KOREObjectSymbolDeclaration : public KOREObjectSymbolAliasDeclaration {
private:
  bool _isHooked;

public:
  static ptr<KOREObjectSymbolDeclaration>
  Create(const std::string &Name, bool isHooked = false) {
    ptr<KOREObjectSymbol> Sym = KOREObjectSymbol::Create(Name);
    return ptr<KOREObjectSymbolDeclaration>(new KOREObjectSymbolDeclaration(std::move(Sym), isHooked));
  }

  bool isHooked() const { return _isHooked; }

  bool isAnywhere();

  virtual void print(std::ostream &Out, unsigned indent = 0) const override;

private:
  KOREObjectSymbolDeclaration(ptr<KOREObjectSymbol> Symbol, bool _isHooked)
  : KOREObjectSymbolAliasDeclaration(std::move(Symbol)), _isHooked(_isHooked) { }
};

class KOREMetaSymbolDeclaration : public KOREMetaSymbolAliasDeclaration {
public:
  static ptr<KOREMetaSymbolDeclaration> Create(const std::string &Name) {
    ptr<KOREMetaSymbol> Sym = KOREMetaSymbol::Create(Name);
    return ptr<KOREMetaSymbolDeclaration>(new KOREMetaSymbolDeclaration(std::move(Sym)));
  }

  virtual void print(std::ostream &Out, unsigned indent = 0) const override;

private:
  KOREMetaSymbolDeclaration(ptr<KOREMetaSymbol> Symbol)
  : KOREMetaSymbolAliasDeclaration(std::move(Symbol)) { }
};

class KOREObjectAliasDeclaration : public KOREObjectSymbolAliasDeclaration {
private:
  std::vector<ptr<KOREObjectVariablePattern>> boundVariables;
  ptr<KOREObjectPattern> pattern;

public:
  static ptr<KOREObjectAliasDeclaration> Create(const std::string &Name) {
    ptr<KOREObjectSymbol> Sym = KOREObjectSymbol::Create(Name);
    return ptr<KOREObjectAliasDeclaration>(new KOREObjectAliasDeclaration(std::move(Sym)));
  }

  void addVariable(ptr<KOREObjectVariablePattern> Variable);
  void addPattern(ptr<KOREObjectPattern> Pattern);
  virtual void print(std::ostream &Out, unsigned indent = 0) const override;

private:
  KOREObjectAliasDeclaration(ptr<KOREObjectSymbol> Symbol)
  : KOREObjectSymbolAliasDeclaration(std::move(Symbol)) { }
};

class KOREMetaAliasDeclaration : public KOREMetaSymbolAliasDeclaration {
private:
  std::vector<ptr<KOREMetaVariablePattern>> boundVariables;
  ptr<KOREMetaPattern> pattern;

public:
  static ptr<KOREMetaAliasDeclaration> Create(const std::string &Name) {
    ptr<KOREMetaSymbol> Sym = KOREMetaSymbol::Create(Name);
    return ptr<KOREMetaAliasDeclaration>(new KOREMetaAliasDeclaration(std::move(Sym)));
  }

  void addVariable(ptr<KOREMetaVariablePattern> Variable);
  void addPattern(ptr<KOREMetaPattern> Pattern);
  virtual void print(std::ostream &Out, unsigned indent = 0) const override;

private:
  KOREMetaAliasDeclaration(ptr<KOREMetaSymbol> Symbol)
  : KOREMetaSymbolAliasDeclaration(std::move(Symbol)) { }
};

class KOREAxiomDeclaration : public KOREDeclaration {
private:
  ptr<KOREPattern> pattern;
  unsigned ordinal;

public:
  static ptr<KOREAxiomDeclaration> Create() { return ptr<KOREAxiomDeclaration>(new KOREAxiomDeclaration()); }

  void addPattern(ptr<KOREPattern> Pattern);
  virtual void print(std::ostream &Out, unsigned indent = 0) const override;

  /* returns true if the axiom is actually required to be translated to llvm
     and false if it is an axiom pertaining to symbolic execution which is not
     required for concrete execution. Axioms that are not required are elided
     from the definition by KOREDefinition::preprocess. */
  bool isRequired();
  bool isTopAxiom();
  KOREPattern *getRightHandSide() const;
  KOREPattern *getRequires() const;
  unsigned getOrdinal() const { return ordinal; }

  friend KOREDefinition;
};

class KOREModule;
class KOREModuleImportDeclaration : public KOREDeclaration {
private:
  std::string moduleName;

public:
  static ptr<KOREModuleImportDeclaration> Create(const std::string &Name) {
    return ptr<KOREModuleImportDeclaration>(new KOREModuleImportDeclaration(Name));
  }

  virtual void print(std::ostream &Out, unsigned indent = 0) const override;

private:
  KOREModuleImportDeclaration(const std::string &Name) : moduleName(Name) { }
};

// KOREModule
class KOREModule {
private:
  std::string name;
  std::vector<ptr<KOREDeclaration>> declarations;
  std::map<std::string, ptr<KOREObjectCompositePattern>> attributes;

public:
  static ptr<KOREModule> Create(const std::string &Name) {
    return ptr<KOREModule>(new KOREModule(Name));
  }

  void addAttribute(ptr<KOREPattern> Attribute);
  void addDeclaration(ptr<KOREDeclaration> Declaration);
  void print(std::ostream &Out, unsigned indent = 0) const;

  const std::vector<ptr<KOREDeclaration>> &getDeclarations() const { return declarations; } 

private:
  KOREModule(const std::string &Name) : name(Name) { }
};

// KOREDefinition
class KOREDefinition {
public:
  // Symbol table types
  using KOREModuleMapType = std::map<std::string, KOREModule *>;

  using KOREObjectSortConstructorMapType =
    std::map<std::string, KOREObjectCompositeSort *>;

  using KOREMetaSortConstructorMapType =
    std::map<std::string, KOREMetaCompositeSort *>;

  using KOREObjectSymbolMapType = std::map<uint32_t, KOREObjectSymbol *>;

  using KOREObjectSymbolStringMapType = std::map<std::string, KOREObjectSymbol *>;

  using KOREMetaSymbolMapType = std::map<std::string, KOREMetaSymbol *>;

  using KOREObjectSortVariableMapType =
    std::map<std::string, KOREObjectSortVariable *>;

  using KOREMetaSortVariableMapType = std::map<std::string, KOREMetaSortVariable *>;

  using KOREObjectVariableMapType = std::map<std::string, KOREObjectVariable *>;

  using KOREMetaVariableMapType = std::map<std::string, KOREMetaVariable *>;

  using KOREObjectCompositeSortDeclarationMapType = std::map<std::string, KOREObjectCompositeSortDeclaration *>;
  using KOREObjectCompositeSortMapType = std::map<std::string, KOREObjectCompositeSort *>;

  using KOREObjectSymbolDeclarationMapType = std::map<std::string, KOREObjectSymbolDeclaration *>;

private:
  // Symbol tables
  KOREObjectSortConstructorMapType objectSortConstructors;
  KOREMetaSortConstructorMapType metaSortConstructors;
  KOREObjectSymbolMapType objectSymbols;
  KOREObjectSymbolStringMapType allObjectSymbols;
  KOREMetaSymbolMapType metaSymbols;
  KOREObjectSortVariableMapType objectSortVariables;
  KOREMetaSortVariableMapType metaSortVariables;
  KOREObjectVariableMapType objectVariables;
  KOREMetaVariableMapType metaVariables;
  KOREModuleMapType moduleNames;
  KOREObjectCompositeSortDeclarationMapType sortDeclarations;
  KOREObjectSymbolDeclarationMapType symbolDeclarations;
  KOREObjectCompositeSortMapType hookedSorts;
  KOREObjectSymbolStringMapType freshFunctions;

  std::vector<ptr<KOREModule>> modules;
  std::map<std::string, ptr<KOREObjectCompositePattern>> attributes;
  /* an automatically computed list of all the axioms in the definition */
  std::list<KOREAxiomDeclaration *> axioms;

  KOREObjectSymbol *injSymbol;

public:
  static ptr<KOREDefinition> Create() { return ptr<KOREDefinition>(new KOREDefinition()); }

  /* Preprocesses the definition and prepares it for translation to llvm.
     This performs the following tasks:
     * removes axioms for which isRequired() returns false
     * sets the arguments field for each KOREObjectSymbol to the actual instantiated
       sort arguments of the symbol (rather than just their polymorphic parameters
     * sets the tag and layout fields on all the KOREObjectSymbols declared by the user
       in the definition. */
  void preprocess();

  void addModule(ptr<KOREModule> Module);
  void addAttribute(ptr<KOREPattern> Attribute);
  void print(std::ostream &Out, unsigned indent = 0) const;

  const KOREObjectCompositeSortDeclarationMapType &getSortDeclarations() const { return sortDeclarations; }
  const KOREObjectSymbolDeclarationMapType &getSymbolDeclarations() const { return symbolDeclarations; }
  const KOREObjectSymbolMapType &getSymbols() const { return objectSymbols; }
  const KOREObjectSymbolStringMapType &getAllSymbols() const { return allObjectSymbols; }
  const KOREObjectCompositeSortMapType getHookedSorts() const { return hookedSorts; }
  const std::list<KOREAxiomDeclaration *> &getAxioms() const { return axioms; }
  const std::map<std::string, ptr<KOREObjectCompositePattern>> &getAttributes() const {
    return attributes;
  }
  const KOREObjectSymbolStringMapType &getFreshFunctions() const { return freshFunctions; }
  KOREObjectSymbol *getInjSymbol() { return injSymbol; }
};

} // end namespace kllvm

#endif // AST_H
