#ifndef AST_H
#define AST_H

#include <boost/functional/hash.hpp>

#include <string>
#include <vector>
#include <list>
#include <map>
#include <unordered_map>

namespace kllvm {


class KORESortVariable;

struct HashSort;

// KORESort
class KORESort {
public:
  using substitution = std::unordered_map<KORESortVariable, KORESort *, HashSort>;

  virtual bool isConcrete() const = 0;
  virtual KORESort *substitute(const substitution &) = 0;

  virtual bool operator==(const KORESort &other) const = 0;
  bool operator!=(const KORESort &other) const { return !(*this == other); }

  virtual void print(std::ostream &Out, unsigned indent = 0) const =0;
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
  static KORESortVariable *Create(const std::string &Name) {
    return new KORESortVariable(Name);
  }

  virtual bool isConcrete() const override { return false; }
  virtual KORESort *substitute(const substitution &subst) override { return subst.at(*this); }

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
  std::vector<KORESort *> arguments;
  ValueType category;

public:
  static KORECompositeSort *Create(const std::string &Name, ValueType Cat = {SortCategory::Uncomputed, 0}) {
    return new KORECompositeSort(Name, Cat);
  }

  const std::string getName() const { return name; }
  ValueType getCategory(KOREDefinition *definition);
  std::string getHook(KOREDefinition *definition);
  static ValueType getCategory(std::string hook);

  virtual bool isConcrete() const override { return true; }
  virtual KORESort *substitute(const substitution &subst) override;

  void addArgument(KORESort *Argument);
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
  std::vector<KORESort *> arguments;
  /* contains the original arguments to the symbol when parsed as parh of a pattern. */
  std::vector<KORESort *> formalArguments;
  /** At parse time, when parsed as part of a pattern, this will be null.
      When parsed as part of a declaration, it contains the return sort of the symbol.
      See above re: the behavior of KORESymbol with respect to instantiateSymbol. */
  KORESort *sort;
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
  static KORESymbol *Create(const std::string &Name) {
    return new KORESymbol(Name);
  }

  void addArgument(KORESort *Argument);
  void addFormalArgument(KORESort *Argument);
  void addSort(KORESort *Sort);
  void initPatternArguments(void) { arguments.swap(formalArguments); }

  const std::string &getName() const { return name; }
  const std::vector<KORESort *> &getArguments() const {
    return arguments;
  }
  const std::vector<KORESort *> &getFormalArguments() const {
    return formalArguments;
  }
  const KORESort *getSort() const { return sort; }
  KORESort *getSort() { return sort; }
  uint32_t getTag() const { assert(firstTag == lastTag); return firstTag; }
  uint32_t getFirstTag() const { return firstTag; }
  uint32_t getLastTag() const { return lastTag; }
  void setTag(uint32_t val) { firstTag = lastTag = val; }
  uint16_t getLayout() const { return layout; }

  virtual void print(std::ostream &Out, unsigned indent = 0) const;
  virtual void print(std::ostream &Out, unsigned indent, bool formal) const;

  bool operator==(KORESymbol other) const;
  bool operator!=(KORESymbol other) const { return !(*this == other); }

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
    for (auto arg : s.arguments) {
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
  static KOREVariable *Create(const std::string &Name) {
    return new KOREVariable(Name);
  }

  std::string getName() const;

  virtual void print(std::ostream &Out, unsigned indent = 0) const;

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

  virtual KORESort *getSort(void) const = 0;
};

class KOREVariablePattern : public KOREPattern {
private:
  KOREVariable *name;
  KORESort *sort;

public:
  static KOREVariablePattern *
  Create(const std::string &Name, KORESort *sort) {
    KOREVariable *Var = KOREVariable::Create(Name);
    return new KOREVariablePattern(Var, sort);
  }

  std::string getName() const;
  virtual KORESort *getSort() const override { return sort; }

  virtual void print(std::ostream &Out, unsigned indent = 0) const override;
  virtual void markSymbols(std::map<std::string, std::vector<KORESymbol *>> &) override {}
  virtual void markVariables(std::map<std::string, KOREVariablePattern *> &map) override { map.insert({name->getName(), this}); }

private:
  KOREVariablePattern(KOREVariable *Name, KORESort *Sort)
  : name(Name), sort(Sort) { }
};

class KORECompositePattern : public KOREPattern {
private:
  KORESymbol *constructor;
  std::vector<KOREPattern *> arguments;

public:
  static KORECompositePattern *Create(const std::string &Name) {
    KORESymbol *Sym = KORESymbol::Create(Name);
    return new KORECompositePattern(Sym);
  }
  static KORECompositePattern *Create(KORESymbol *Sym) {
    return new KORECompositePattern(Sym);
  }

  KORESort *getSort() const override { return constructor->getSort(); }

  KORESymbol *getConstructor() const { return constructor; }
  const std::vector<KOREPattern *> &getArguments() const { return arguments; }

  void addArgument(KOREPattern *Argument);
  virtual void print(std::ostream &Out, unsigned indent = 0) const override;
  virtual void markSymbols(std::map<std::string, std::vector<KORESymbol *>> &) override;
  virtual void markVariables(std::map<std::string, KOREVariablePattern *> &) override;

private:
  KORECompositePattern(KORESymbol *Constructor)
  : constructor(Constructor) { }
};

class KOREStringPattern : public KOREPattern {
private:
  std::string contents;

public:
  static KOREStringPattern *Create(const std::string &Contents) {
    return new KOREStringPattern(Contents);
  }

  std::string getContents() { return contents; }

  virtual void print(std::ostream &Out, unsigned indent = 0) const override;
  virtual void markSymbols(std::map<std::string, std::vector<KORESymbol *>> &) override {}
  virtual void markVariables(std::map<std::string, KOREVariablePattern *> &) override {}
  virtual KORESort *getSort(void) const override { abort(); }

private:
  KOREStringPattern(const std::string &Contents) : contents(Contents) { }
};

// KOREDeclaration
class KOREDeclaration {
protected:
  std::map<std::string, KORECompositePattern *> attributes;
  std::vector<KORESortVariable *> objectSortVariables;

public:
  void addAttribute(KOREPattern *Attribute);
  void addObjectSortVariable(KORESortVariable *SortVariable);
  virtual void print(std::ostream &Out, unsigned indent = 0) const =0;
  const std::map<std::string, KORECompositePattern *> &getAttributes() const { return attributes; }
  const std::vector<KORESortVariable *> &getObjectSortVariables() const { return objectSortVariables; }

protected:
  void printSortVariables(std::ostream &Out) const;
};

class KORECompositeSortDeclaration : public KOREDeclaration {
private:
  bool _isHooked;
  std::string sortName;

public:
  static KORECompositeSortDeclaration *
  Create(const std::string &Name, bool isHooked = false) {
    return new KORECompositeSortDeclaration(Name, isHooked);
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
  KORESymbol *symbol;

  KORESymbolAliasDeclaration(KORESymbol *Symbol)
  : symbol(Symbol) { }

public:
  KORESymbol *getSymbol() const { return symbol; }
};

class KORESymbolDeclaration : public KORESymbolAliasDeclaration {
private:
  bool _isHooked;

public:
  static KORESymbolDeclaration *
  Create(const std::string &Name, bool isHooked = false) {
    KORESymbol *Sym = KORESymbol::Create(Name);
    return new KORESymbolDeclaration(Sym, isHooked);
  }

  bool isHooked() const { return _isHooked; }

  bool isAnywhere();

  virtual void print(std::ostream &Out, unsigned indent = 0) const override;

private:
  KORESymbolDeclaration(KORESymbol *Symbol, bool _isHooked)
  : KORESymbolAliasDeclaration(Symbol), _isHooked(_isHooked) { }
};

class KOREAliasDeclaration : public KORESymbolAliasDeclaration {
private:
  KORECompositePattern *boundVariables;
  KOREPattern *pattern;

public:
  static KOREAliasDeclaration *Create(const std::string &Name) {
    KORESymbol *Sym = KORESymbol::Create(Name);
    return new KOREAliasDeclaration(Sym);
  }

  void addVariables(KORECompositePattern *variables);
  void addPattern(KOREPattern *Pattern);
  virtual void print(std::ostream &Out, unsigned indent = 0) const override;

private:
  KOREAliasDeclaration(KORESymbol *Symbol)
  : KORESymbolAliasDeclaration(Symbol) { }
};

class KOREAxiomDeclaration : public KOREDeclaration {
private:
  KOREPattern *pattern;
  unsigned ordinal;
  bool _isClaim;

  KOREAxiomDeclaration(bool isClaim): _isClaim(isClaim) {}

public:
  static KOREAxiomDeclaration *Create(bool isClaim = false) { return new KOREAxiomDeclaration(isClaim); }

  void addPattern(KOREPattern *Pattern);
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
  static KOREModuleImportDeclaration *Create(const std::string &Name) {
    return new KOREModuleImportDeclaration(Name);
  }

  virtual void print(std::ostream &Out, unsigned indent = 0) const override;

private:
  KOREModuleImportDeclaration(const std::string &Name) : moduleName(Name) { }
};

// KOREModule
class KOREModule {
private:
  std::string name;
  std::vector<KOREDeclaration *> declarations;
  std::map<std::string, KORECompositePattern *> attributes;

public:
  static KOREModule *Create(const std::string &Name) {
    return new KOREModule(Name);
  }

  void addAttribute(KOREPattern *Attribute);
  void addDeclaration(KOREDeclaration *Declaration);
  void print(std::ostream &Out, unsigned indent = 0) const;

  const std::vector<KOREDeclaration *> &getDeclarations() const { return declarations; } 

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
  using KORECompositeSortMapType = std::map<std::string, KORECompositeSort *>;

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

  std::vector<KOREModule *> modules;
  std::map<std::string, KORECompositePattern *> attributes;
  /* an automatically computed list of all the axioms in the definition */
  std::list<KOREAxiomDeclaration *> axioms;

  KORESymbol *injSymbol;

public:
  static KOREDefinition *Create() { return new KOREDefinition(); }

  /* Preprocesses the definition and prepares it for translation to llvm.
     This performs the following tasks:
     * removes axioms for which isRequired() returns false
     * sets the arguments field for each KORESymbol to the actual instantiated
       sort arguments of the symbol (rather than just their polymorphic parameters
     * sets the tag and layout fields on all the KORESymbols declared by the user
       in the definition. */
  void preprocess();

  void addModule(KOREModule *Module);
  void addAttribute(KOREPattern *Attribute);
  void print(std::ostream &Out, unsigned indent = 0) const;

  const KORECompositeSortDeclarationMapType &getSortDeclarations() const { return sortDeclarations; }
  const KORESymbolDeclarationMapType &getSymbolDeclarations() const { return symbolDeclarations; }
  const KORESymbolMapType &getSymbols() const { return objectSymbols; }
  const KORESymbolStringMapType &getAllSymbols() const { return allObjectSymbols; }
  const KORECompositeSortMapType getHookedSorts() const { return hookedSorts; }
  const std::list<KOREAxiomDeclaration *> &getAxioms() const { return axioms; }
  const std::map<std::string, KORECompositePattern *> &getAttributes() const {
    return attributes;
  }
  const KORESymbolStringMapType &getFreshFunctions() const { return freshFunctions; }
  KORESymbol *getInjSymbol() { return injSymbol; }
};

} // end namespace kllvm

#endif // AST_H
