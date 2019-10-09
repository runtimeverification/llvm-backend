#ifndef AST_H
#define AST_H

#include <boost/functional/hash.hpp>

#include <string>
#include <vector>
#include <list>
#include <map>
#include <unordered_map>
#include <utility>

namespace kllvm {


class KORESortVariable;

struct HashSort;

template <typename T>
using ptr = std::unique_ptr<T>;

template <typename T>
using sptr = std::shared_ptr<T>;

// KORESort
class KORESort : public std::enable_shared_from_this<KORESort> {
public:
  using substitution = std::unordered_map<KORESortVariable, sptr<KORESort>, HashSort>;

  virtual bool isConcrete() const = 0;
  virtual sptr<KORESort> substitute(const substitution &) = 0;

  virtual bool operator==(const KORESort &other) const = 0;
  bool operator!=(const KORESort &other) const { return !(*this == other); }

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

class KORESortVariable : public KORESort {
private:
  std::string name;

public:
  static sptr<KORESortVariable> Create(const std::string &Name) {
    return sptr<KORESortVariable>(new KORESortVariable(Name));
  }

  virtual bool isConcrete() const override { return false; }
  virtual sptr<KORESort> substitute(const substitution &subst) override { return subst.at(*this); }

  virtual void print(std::ostream &Out, unsigned indent = 0) const override;
  virtual bool operator==(const KORESort &other) const override;

private:
  KORESortVariable(const std::string &Name) : name(Name) { }
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

class KORECompositeSort : public KORESort {
private:
  std::string name;
  std::vector<sptr<KORESort>> arguments;
  ValueType category;

public:
  static sptr<KORECompositeSort> Create(const std::string &Name, ValueType Cat = {SortCategory::Uncomputed, 0}) {
    return sptr<KORECompositeSort>(new KORECompositeSort(Name, Cat));
  }

  const std::string getName() const { return name; }
  ValueType getCategory(KOREDefinition *definition);
  std::string getHook(KOREDefinition *definition);
  static ValueType getCategory(std::string hook);

  virtual bool isConcrete() const override { return true; }
  virtual sptr<KORESort> substitute(const substitution &subst) override;

  void addArgument(sptr<KORESort> Argument);
  virtual void print(std::ostream &Out, unsigned indent = 0) const override;
  virtual bool operator==(const KORESort &other) const override;

private:
  KORECompositeSort(const std::string &Name, ValueType category) : name(Name), category(category) {}
};

struct HashSymbol;

class KORESymbolDeclaration;

// KORESymbol
class KORESymbol {
private:
  std::string name;
  /* At parse time, when parsed as part of a pattern,
      this will be empty. When parsed as part of a declaration, it contains the signature
      of the symbol. After instantiateSymbol is called on a symbol that is
      part of a pattern, it changes from being empty to being
      the signature of the symbol. instantiateSymbol is called on all object
      level symbols in axioms when KOREDefinition::preprocess is called. */
  std::vector<sptr<KORESort>> arguments;
  /* contains the original arguments to the symbol when parsed as parh of a pattern. */
  std::vector<sptr<KORESort>> formalArguments;
  /** At parse time, when parsed as part of a pattern, this will be null.
      When parsed as part of a declaration, it contains the return sort of the symbol.
      See above re: the behavior of KORESymbol with respect to instantiateSymbol. */
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
  const std::vector<sptr<KORESort>> &getArguments() const {
    return arguments;
  }
  const std::vector<sptr<KORESort>> &getFormalArguments() const {
    return formalArguments;
  }
  const sptr<KORESort> getSort() const { return sort; }
  sptr<KORESort> getSort() { return sort; }
  uint32_t getTag() const { assert(firstTag == lastTag); return firstTag; }
  uint32_t getFirstTag() const { return firstTag; }
  uint32_t getLastTag() const { return lastTag; }
  void setTag(uint32_t val) { firstTag = lastTag = val; }
  uint16_t getLayout() const { return layout; }

  void print(std::ostream &Out, unsigned indent = 0) const;
  void print(std::ostream &Out, unsigned indent, bool formal) const;

  bool operator==(const KORESymbol &other) const;
  bool operator!=(const KORESymbol &other) const { return !(*this == other); }

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
  void instantiateSymbol(KORESymbolDeclaration *decl);

  friend HashSymbol;

  friend KOREDefinition;

private:
  KORESymbol(const std::string &Name) : name(Name), sort(nullptr) { }
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
  virtual ~KOREVariable() = default;

private:
  KOREVariable(const std::string &Name) : name(Name) { }
};

class KOREVariablePattern;

// KOREPattern
class KOREPattern {
public:
  virtual ~KOREPattern() = default;

  virtual void print(std::ostream &Out, unsigned indent = 0) const =0;
  /* adds all the object level symbols contained recursively in the current pattern
     to the specified map, mapping their symbol name to the list of all instances
     of that symbol. */
  virtual void markSymbols(std::map<std::string, std::vector<KORESymbol *>> &) = 0;
  /* adds all the object level variables contained recursively in the current pattern
     to the specified map, mapping their variable name to the variable itself. */
  virtual void markVariables(std::map<std::string, KOREVariablePattern *> &) = 0;

  virtual sptr<KORESort> getSort(void) const = 0;
};

class KOREVariablePattern : public KOREPattern {
private:
  ptr<KOREVariable> name;
  sptr<KORESort> sort;

public:
  static ptr<KOREVariablePattern>
  Create(const std::string &Name, sptr<KORESort> sort) {
    ptr<KOREVariable> Var = KOREVariable::Create(Name);
    return ptr<KOREVariablePattern>(new KOREVariablePattern(std::move(Var), sort));
  }

  std::string getName() const;
  virtual sptr<KORESort> getSort() const override { return sort; }

  virtual void print(std::ostream &Out, unsigned indent = 0) const override;
  virtual void markSymbols(std::map<std::string, std::vector<KORESymbol *>> &) override {}
  virtual void markVariables(std::map<std::string, KOREVariablePattern *> &map) override { map.insert({name->getName(), this}); }

private:
  KOREVariablePattern(ptr<KOREVariable> Name, sptr<KORESort> Sort)
  : name(std::move(Name)), sort(Sort) { }
};

class KORECompositePattern : public KOREPattern {
private:
  ptr<KORESymbol> constructor;
  std::vector<ptr<KOREPattern>> arguments;

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
    return ptr<KORECompositePattern>(new KORECompositePattern(std::move(newSym)));
  }

  sptr<KORESort> getSort() const override { return constructor->getSort(); }

  KORESymbol *getConstructor() const { return constructor.get(); }
  const std::vector<ptr<KOREPattern>> &getArguments() const { return arguments; }

  void addArgument(ptr<KOREPattern> Argument);
  virtual void print(std::ostream &Out, unsigned indent = 0) const override;
  virtual void markSymbols(std::map<std::string, std::vector<KORESymbol *>> &) override;
  virtual void markVariables(std::map<std::string, KOREVariablePattern *> &) override;

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
  virtual void markSymbols(std::map<std::string, std::vector<KORESymbol *>> &) override {}
  virtual void markVariables(std::map<std::string, KOREVariablePattern *> &) override {}
  virtual sptr<KORESort> getSort(void) const override { abort(); }

private:
  KOREStringPattern(const std::string &Contents) : contents(Contents) { }
};

// KOREDeclaration
class KOREDeclaration {
protected:
  std::map<std::string, ptr<KORECompositePattern>> attributes;
  std::vector<sptr<KORESortVariable>> objectSortVariables;

public:
  void addAttribute(ptr<KORECompositePattern> Attribute);
  void addObjectSortVariable(sptr<KORESortVariable> SortVariable);
  virtual void print(std::ostream &Out, unsigned indent = 0) const =0;
  const std::map<std::string, ptr<KORECompositePattern>> &getAttributes() const { return attributes; }
  const std::vector<sptr<KORESortVariable>> &getObjectSortVariables() const { return objectSortVariables; }
  virtual ~KOREDeclaration() = default;

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
    return ptr<KORECompositeSortDeclaration>(new KORECompositeSortDeclaration(Name, isHooked));
  }

  std::string getName() const { return sortName; }
  bool isHooked() const { return _isHooked; }

  virtual void print(std::ostream &Out, unsigned indent = 0) const override;

private:
  KORECompositeSortDeclaration(const std::string &Name, bool _isHooked)
  : _isHooked(_isHooked), sortName(Name) { }
};

class KORESymbolOrAliasDeclaration : public KOREDeclaration {

};

class KORESymbolAliasDeclaration : public KORESymbolOrAliasDeclaration {
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
    return ptr<KORESymbolDeclaration>(new KORESymbolDeclaration(std::move(Sym), isHooked));
  }

  bool isHooked() const { return _isHooked; }

  bool isAnywhere();

  virtual void print(std::ostream &Out, unsigned indent = 0) const override;

private:
  KORESymbolDeclaration(ptr<KORESymbol> Symbol, bool _isHooked)
  : KORESymbolAliasDeclaration(std::move(Symbol)), _isHooked(_isHooked) { }
};

class KOREAliasDeclaration : public KORESymbolAliasDeclaration {
private:
  ptr<KORECompositePattern> boundVariables;
  ptr<KOREPattern> pattern;

public:
  static ptr<KOREAliasDeclaration> Create(const std::string &Name) {
    ptr<KORESymbol> Sym = KORESymbol::Create(Name);
    return ptr<KOREAliasDeclaration>(new KOREAliasDeclaration(std::move(Sym)));
  }

  void addVariables(ptr<KORECompositePattern> variables);
  void addPattern(ptr<KOREPattern> Pattern);
  virtual void print(std::ostream &Out, unsigned indent = 0) const override;

private:
  KOREAliasDeclaration(ptr<KORESymbol> Symbol)
  : KORESymbolAliasDeclaration(std::move(Symbol)) { }
};

class KOREAxiomDeclaration : public KOREDeclaration {
private:
  ptr<KOREPattern> pattern;
  unsigned ordinal;
  bool _isClaim;

  KOREAxiomDeclaration(bool isClaim): _isClaim(isClaim) {}

public:
  static ptr<KOREAxiomDeclaration> Create(bool isClaim = false) { 
    return ptr<KOREAxiomDeclaration>(new KOREAxiomDeclaration(isClaim));
  }

  void addPattern(ptr<KOREPattern> Pattern);
  virtual void print(std::ostream &Out, unsigned indent = 0) const override;

  /* returns true if the axiom is actually required to be translated to llvm
     and false if it is an axiom pertaining to symbolic execution which is not
     required for concrete execution. Axioms that are not required are elided
     from the definition by KOREDefinition::preprocess. */
  bool isRequired();
  bool isTopAxiom();
  bool isClaim() { return _isClaim; }
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
  std::map<std::string, ptr<KORECompositePattern>> attributes;

public:
  static ptr<KOREModule> Create(const std::string &Name) {
    return ptr<KOREModule>(new KOREModule(Name));
  }

  void addAttribute(ptr<KORECompositePattern> Attribute);
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

  using KORESortConstructorMapType =
    std::map<std::string, KORECompositeSort *>;

  using KORESymbolMapType = std::map<uint32_t, KORESymbol *>;

  using KORESymbolStringMapType = std::map<std::string, KORESymbol *>;

  using KORESortVariableMapType =
    std::map<std::string, KORESortVariable *>;

  using KOREVariableMapType = std::map<std::string, KOREVariable *>;

  using KORECompositeSortDeclarationMapType = std::map<std::string, KORECompositeSortDeclaration *>;
  using KORECompositeSortMapType = std::map<std::string, sptr<KORECompositeSort>>;

  using KORESymbolDeclarationMapType = std::map<std::string, KORESymbolDeclaration *>;

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
  KORECompositeSortMapType hookedSorts;
  KORESymbolStringMapType freshFunctions;

  std::vector<ptr<KOREModule>> modules;
  std::map<std::string, ptr<KORECompositePattern>> attributes;
  /* an automatically computed list of all the axioms in the definition */
  std::list<KOREAxiomDeclaration *> axioms;

  KORESymbol *injSymbol;

public:
  static ptr<KOREDefinition> Create() { return ptr<KOREDefinition>(new KOREDefinition()); }

  /* Preprocesses the definition and prepares it for translation to llvm.
     This performs the following tasks:
     * removes axioms for which isRequired() returns false
     * sets the arguments field for each KORESymbol to the actual instantiated
       sort arguments of the symbol (rather than just their polymorphic parameters
     * sets the tag and layout fields on all the KORESymbols declared by the user
       in the definition. */
  void preprocess();

  void addModule(ptr<KOREModule> Module);
  void addAttribute(ptr<KORECompositePattern> Attribute);
  void print(std::ostream &Out, unsigned indent = 0) const;

  const KORECompositeSortDeclarationMapType &getSortDeclarations() const { return sortDeclarations; }
  const KORESymbolDeclarationMapType &getSymbolDeclarations() const { return symbolDeclarations; }
  const KORESymbolMapType &getSymbols() const { return objectSymbols; }
  const KORESymbolStringMapType &getAllSymbols() const { return allObjectSymbols; }
  const KORECompositeSortMapType getHookedSorts() const { return hookedSorts; }
  const std::list<KOREAxiomDeclaration *> &getAxioms() const { return axioms; }
  const std::map<std::string, ptr<KORECompositePattern>> &getAttributes() const {
    return attributes;
  }
  const KORESymbolStringMapType &getFreshFunctions() const { return freshFunctions; }
  KORESymbol *getInjSymbol() { return injSymbol; }
};

} // end namespace kllvm

#endif // AST_H
