#ifndef AST_H
#define AST_H

#include <boost/functional/hash.hpp>

#include <string>
#include <vector>
#include <list>
#include <map>
#include <unordered_map>

namespace kllvm {


// KORESort
class KORESort {
public:
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
  static KOREObjectSortVariable *Create(const std::string &Name) {
    return new KOREObjectSortVariable(Name);
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
  static KOREMetaSortVariable *Create(const std::string &Name) {
    return new KOREMetaSortVariable(Name);
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
  std::vector<KOREObjectSort *> arguments;
  ValueType category;

public:
  static KOREObjectCompositeSort *Create(const std::string &Name, ValueType Cat = {SortCategory::Uncomputed, 0}) {
    return new KOREObjectCompositeSort(Name, Cat);
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
  static KOREMetaCompositeSort *Create(const std::string &Name) {
    return new KOREMetaCompositeSort(Name);
  }

  virtual void print(std::ostream &Out, unsigned indent = 0) const override;

private:
  KOREMetaCompositeSort(const std::string &Name) : name(Name) { }
};

// KORESymbol
class KORESymbol {
public:
  virtual void print(std::ostream &Out, unsigned indent = 0) const =0;
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
  std::vector<KOREObjectSort *> arguments;
  /* contains the original arguments to the symbol when parsed as parh of a pattern. */
  std::vector<KOREObjectSort *> formalArguments;
  /** At parse time, when parsed as part of a pattern, this will be null.
      When parsed as part of a declaration, it contains the return sort of the symbol.
      See above re: the behavior of KOREObjectSymbol with respect to instantiateSymbol. */
  KOREObjectSort *sort;
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
  static KOREObjectSymbol *Create(const std::string &Name) {
    return new KOREObjectSymbol(Name);
  }

  void addArgument(KOREObjectSort *Argument);
  void addFormalArgument(KOREObjectSort *Argument);
  void addSort(KOREObjectSort *Sort);

  const std::string &getName() const { return name; }
  const std::vector<KOREObjectSort *> &getArguments() const {
    return arguments;
  }
  const std::vector<KOREObjectSort *> &getFormalArguments() const {
    return formalArguments;
  }
  const KOREObjectSort *getSort() const { return sort; }
  KOREObjectSort *getSort() { return sort; }
  uint32_t getTag() const { assert(firstTag == lastTag); return firstTag; }
  uint32_t getFirstTag() const { return firstTag; }
  uint32_t getLastTag() const { return lastTag; }
  void setTag(uint32_t val) { firstTag = lastTag = val; }
  uint16_t getLayout() const { return layout; }

  virtual void print(std::ostream &Out, unsigned indent = 0) const override;
  virtual void print(std::ostream &Out, unsigned indent, bool formal) const;

  bool operator==(KOREObjectSymbol other) const;
  bool operator!=(KOREObjectSymbol other) const { return !(*this == other); }

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
    for (auto arg : s.arguments) {
      boost::hash_combine(hash, *arg);
    }
    return hash;
  }
};

class KOREMetaSymbol : public KORESymbol {
private:
  std::string name;
  std::vector<KOREMetaSort *> arguments;
  KOREMetaSort *sort;

public:
  static KOREMetaSymbol *Create(const std::string &Name) {
    return new KOREMetaSymbol(Name);
  }

  void addArgument(KOREMetaSort *Argument);
  void addSort(KOREMetaSort *Sort);

  const std::string &getName() const { return name; }
  const std::vector<KOREMetaSort *> &getArguments() const {
    return arguments;
  }
  const KOREMetaSort *getSort() const { return sort; }

  virtual void print(std::ostream &Out, unsigned indent = 0) const override;

private:
  KOREMetaSymbol(const std::string &Name) : name(Name), sort(nullptr) { }
};

// KOREVariable
class KOREVariable {
public:
  virtual void print(std::ostream &Out, unsigned indent = 0) const =0;
};

class KOREObjectVariable : public KOREVariable {
private:
  std::string name;

public:
  static KOREObjectVariable *Create(const std::string &Name) {
    return new KOREObjectVariable(Name);
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
  static KOREMetaVariable *Create(const std::string &Name) {
    return new KOREMetaVariable(Name);
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
  using substitution = std::unordered_map<std::string, KOREObjectPattern *, std::hash<std::string>>;
  virtual KOREObjectSort *getSort(void) const = 0;
  virtual KOREObjectPattern *substitute(const substitution &) = 0;

  virtual bool operator==(const KOREObjectPattern &other) const = 0;
  bool operator!=(const KOREObjectPattern &other) const { return !(*this == other); }
};

class KOREMetaPattern : public KOREPattern {

};

class KOREObjectVariablePattern : public KOREObjectPattern {
private:
  KOREObjectVariable *name;
  KOREObjectSort *sort;

public:
  static KOREObjectVariablePattern *
  Create(const std::string &Name, KOREObjectSort *sort) {
    KOREObjectVariable *Var = KOREObjectVariable::Create(Name);
    return new KOREObjectVariablePattern(Var, sort);
  }

  std::string getName() const;
  virtual KOREObjectSort *getSort() const override { return sort; }

  virtual void print(std::ostream &Out, unsigned indent = 0) const override;
  virtual void markSymbols(std::map<std::string, std::vector<KOREObjectSymbol *>> &) override {}
  virtual void markVariables(std::map<std::string, KOREObjectVariablePattern *> &map) override { map.insert({name->getName(), this}); }
  virtual KOREObjectPattern *substitute(const substitution &subst) override { return subst.at(this->getName()); }

  virtual bool operator==(const KOREObjectPattern &other) const override;

private:
  KOREObjectVariablePattern(KOREObjectVariable *Name, KOREObjectSort *Sort)
  : name(Name), sort(Sort) { }
};

class KOREMetaVariablePattern : public KOREMetaPattern {
private:
  KOREMetaVariable *name;
  KOREMetaSort *sort;

public:
  static KOREMetaVariablePattern *
  Create(const std::string &Name, KOREMetaSort *sort) {
    KOREMetaVariable *Var = KOREMetaVariable::Create(Name);
    return new KOREMetaVariablePattern(Var, sort);
  }

  virtual void print(std::ostream &Out, unsigned indent = 0) const override;
  virtual void markSymbols(std::map<std::string, std::vector<KOREObjectSymbol *>> &) override {}
  virtual void markVariables(std::map<std::string, KOREObjectVariablePattern *> &) override {}

private:
  KOREMetaVariablePattern(KOREMetaVariable *Name, KOREMetaSort *Sort)
  : name(Name), sort(Sort) { }
};

class KOREObjectCompositePattern : public KOREObjectPattern {
private:
  KOREObjectSymbol *constructor;
  std::vector<KOREPattern *> arguments;

public:
  static KOREObjectCompositePattern *Create(const std::string &Name) {
    KOREObjectSymbol *Sym = KOREObjectSymbol::Create(Name);
    return new KOREObjectCompositePattern(Sym);
  }
  static KOREObjectCompositePattern *Create(KOREObjectSymbol *Sym) {
    return new KOREObjectCompositePattern(Sym);
  }

  KOREObjectSort *getSort() const override { return constructor->getSort(); }

  KOREObjectSymbol *getConstructor() const { return constructor; }
  const std::vector<KOREPattern *> &getArguments() const { return arguments; }

  void addArgument(KOREPattern *Argument);
  virtual void print(std::ostream &Out, unsigned indent = 0) const override;
  virtual void markSymbols(std::map<std::string, std::vector<KOREObjectSymbol *>> &) override;
  virtual void markVariables(std::map<std::string, KOREObjectVariablePattern *> &) override;
  virtual KOREObjectPattern *substitute(const substitution &subst) override;

  virtual bool operator==(const KOREObjectPattern &other) const override;

private:
  KOREObjectCompositePattern(KOREObjectSymbol *Constructor)
  : constructor(Constructor) { }
};

class KOREMetaCompositePattern : public KOREMetaPattern {
private:
  KOREMetaSymbol *constructor;
  std::vector<KOREPattern *> arguments;

public:
  static KOREMetaCompositePattern *Create(const std::string &Name) {
    KOREMetaSymbol *Sym = KOREMetaSymbol::Create(Name);
    return new KOREMetaCompositePattern(Sym);
  }

  KOREMetaSymbol *getConstructor() const { return constructor; }

  void addArgument(KOREPattern *Argument);
  virtual void print(std::ostream &Out, unsigned indent = 0) const override;
  virtual void markSymbols(std::map<std::string, std::vector<KOREObjectSymbol *>> &) override;
  virtual void markVariables(std::map<std::string, KOREObjectVariablePattern *> &) override;

private:
  KOREMetaCompositePattern(KOREMetaSymbol *Constructor)
  : constructor(Constructor) { }
};

class KOREMetaStringPattern : public KOREMetaPattern {
private:
  std::string contents;

public:
  static KOREMetaStringPattern *Create(const std::string &Contents) {
    return new KOREMetaStringPattern(Contents);
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
  static KOREMetaCharPattern *Create(char Contents) {
    return new KOREMetaCharPattern(Contents);
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
  std::map<std::string, KOREObjectCompositePattern *> attributes;
  std::vector<KOREObjectSortVariable *> objectSortVariables;
  std::vector<KOREMetaSortVariable *> metaSortVariables;

public:
  void addAttribute(KOREPattern *Attribute);
  void addObjectSortVariable(KOREObjectSortVariable *SortVariable);
  void addMetaSortVariable(KOREMetaSortVariable *SortVariable);
  virtual void print(std::ostream &Out, unsigned indent = 0) const =0;
  const std::map<std::string, KOREObjectCompositePattern *> &getAttributes() const { return attributes; }
  const std::vector<KOREObjectSortVariable *> &getObjectSortVariables() const { return objectSortVariables; }

protected:
  void printSortVariables(std::ostream &Out) const;
};

class KOREObjectCompositeSortDeclaration : public KOREDeclaration {
private:
  bool _isHooked;
  std::string sortName;

public:
  static KOREObjectCompositeSortDeclaration *
  Create(const std::string &Name, bool isHooked = false) {
    return new KOREObjectCompositeSortDeclaration(Name, isHooked);
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
  KOREObjectSymbol *symbol;

  KOREObjectSymbolAliasDeclaration(KOREObjectSymbol *Symbol)
  : symbol(Symbol) { }

public:
  KOREObjectSymbol *getSymbol() const { return symbol; }
};

class KOREMetaSymbolAliasDeclaration : public KORESymbolDeclaration {
protected:
  KOREMetaSymbol *symbol;

  KOREMetaSymbolAliasDeclaration(KOREMetaSymbol *Symbol)
  : symbol(Symbol) { }

public:
  KOREMetaSymbol *getSymbol() const { return symbol; }
};

class KOREObjectSymbolDeclaration : public KOREObjectSymbolAliasDeclaration {
private:
  bool _isHooked;

public:
  static KOREObjectSymbolDeclaration *
  Create(const std::string &Name, bool isHooked = false) {
    KOREObjectSymbol *Sym = KOREObjectSymbol::Create(Name);
    return new KOREObjectSymbolDeclaration(Sym, isHooked);
  }

  bool isHooked() const { return _isHooked; }

  bool isAnywhere();

  virtual void print(std::ostream &Out, unsigned indent = 0) const override;

private:
  KOREObjectSymbolDeclaration(KOREObjectSymbol *Symbol, bool _isHooked)
  : KOREObjectSymbolAliasDeclaration(Symbol), _isHooked(_isHooked) { }
};

class KOREMetaSymbolDeclaration : public KOREMetaSymbolAliasDeclaration {
public:
  static KOREMetaSymbolDeclaration *Create(const std::string &Name) {
    KOREMetaSymbol *Sym = KOREMetaSymbol::Create(Name);
    return new KOREMetaSymbolDeclaration(Sym);
  }

  virtual void print(std::ostream &Out, unsigned indent = 0) const override;

private:
  KOREMetaSymbolDeclaration(KOREMetaSymbol *Symbol)
  : KOREMetaSymbolAliasDeclaration(Symbol) { }
};

class KOREObjectAliasDeclaration : public KOREObjectSymbolAliasDeclaration {
private:
  std::vector<KOREObjectVariablePattern *> boundVariables;
  KOREObjectPattern *pattern;

public:
  static KOREObjectAliasDeclaration *Create(const std::string &Name) {
    KOREObjectSymbol *Sym = KOREObjectSymbol::Create(Name);
    return new KOREObjectAliasDeclaration(Sym);
  }

  void addVariable(KOREObjectVariablePattern *Variable);
  void addPattern(KOREObjectPattern *Pattern);
  virtual void print(std::ostream &Out, unsigned indent = 0) const override;
  KOREObjectPattern *getPattern() const { return pattern; }
  const std::vector<KOREObjectVariablePattern *> &getBoundVariables() const { return boundVariables; }

private:
  KOREObjectAliasDeclaration(KOREObjectSymbol *Symbol)
  : KOREObjectSymbolAliasDeclaration(Symbol) { }
};

class KOREMetaAliasDeclaration : public KOREMetaSymbolAliasDeclaration {
private:
  std::vector<KOREMetaVariablePattern *> boundVariables;
  KOREMetaPattern *pattern;

public:
  static KOREMetaAliasDeclaration *Create(const std::string &Name) {
    KOREMetaSymbol *Sym = KOREMetaSymbol::Create(Name);
    return new KOREMetaAliasDeclaration(Sym);
  }

  void addVariable(KOREMetaVariablePattern *Variable);
  void addPattern(KOREMetaPattern *Pattern);
  virtual void print(std::ostream &Out, unsigned indent = 0) const override;

private:
  KOREMetaAliasDeclaration(KOREMetaSymbol *Symbol)
  : KOREMetaSymbolAliasDeclaration(Symbol) { }
};

class KOREAxiomDeclaration : public KOREDeclaration {
private:
  KOREPattern *pattern;
  unsigned ordinal;

public:
  static KOREAxiomDeclaration *Create() { return new KOREAxiomDeclaration(); }

  void addPattern(KOREPattern *Pattern);
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
  KOREPattern *getPattern() const { return pattern; }

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
  std::map<std::string, KOREObjectCompositePattern *> attributes;

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

  using KOREObjectAliasDeclarationMapType = std::map<std::string, KOREObjectAliasDeclaration *>;

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
  KOREObjectAliasDeclarationMapType aliasDeclarations;
  KOREObjectSymbolDeclarationMapType symbolDeclarations;
  KOREObjectCompositeSortMapType hookedSorts;
  KOREObjectSymbolStringMapType freshFunctions;

  std::vector<KOREModule *> modules;
  std::map<std::string, KOREObjectCompositePattern *> attributes;
  /* an automatically computed list of all the axioms in the definition */
  std::list<KOREAxiomDeclaration *> axioms;

  KOREObjectSymbol *injSymbol;

public:
  static KOREDefinition *Create() { return new KOREDefinition(); }

  /* Preprocesses the definition and prepares it for translation to llvm.
     This performs the following tasks:
     * removes axioms for which isRequired() returns false
     * sets the arguments field for each KOREObjectSymbol to the actual instantiated
       sort arguments of the symbol (rather than just their polymorphic parameters
     * sets the tag and layout fields on all the KOREObjectSymbols declared by the user
       in the definition. */
  void preprocess();

  void expandAliases(KOREPattern *pattern);

  void addModule(KOREModule *Module);
  void addAttribute(KOREPattern *Attribute);
  void print(std::ostream &Out, unsigned indent = 0) const;

  const KOREObjectCompositeSortDeclarationMapType &getSortDeclarations() const { return sortDeclarations; }
  const KOREObjectSymbolDeclarationMapType &getSymbolDeclarations() const { return symbolDeclarations; }
  const KOREObjectAliasDeclarationMapType &getAliasDeclarations() const { return aliasDeclarations; }
  const KOREObjectSymbolMapType &getSymbols() const { return objectSymbols; }
  const KOREObjectSymbolStringMapType &getAllSymbols() const { return allObjectSymbols; }
  const KOREObjectCompositeSortMapType getHookedSorts() const { return hookedSorts; }
  const std::list<KOREAxiomDeclaration *> &getAxioms() const { return axioms; }
  const std::map<std::string, KOREObjectCompositePattern *> &getAttributes() const {
    return attributes;
  }
  const KOREObjectSymbolStringMapType &getFreshFunctions() const { return freshFunctions; }
  KOREObjectSymbol *getInjSymbol() { return injSymbol; }
};

} // end namespace kllvm

#endif // AST_H
