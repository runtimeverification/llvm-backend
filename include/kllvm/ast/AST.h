#ifndef AST_H
#define AST_H

#include <boost/functional/hash.hpp>

#include "llvm/ADT/StringMap.h"

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
  Uncomputed, Map, List, Set, Int, Float, StringBuffer, Bool, MInt, Symbol
};

class KOREDefinition;

class KOREObjectCompositeSort : public KOREObjectSort {
private:
  std::string name;
  std::vector<KOREObjectSort *> arguments;
  SortCategory category;

public:
  static KOREObjectCompositeSort *Create(const std::string &Name, SortCategory Cat = SortCategory::Uncomputed) {
    return new KOREObjectCompositeSort(Name, Cat);
  }

  const std::string getName() const { return name; }
  SortCategory getCategory(KOREDefinition *definition);

  virtual bool isConcrete() const override { return true; }
  virtual KOREObjectSort *substitute(const substitution &subst) override;

  void addArgument(KOREObjectSort *Argument);
  virtual void print(std::ostream &Out, unsigned indent = 0) const override;
  virtual bool operator==(const KOREObjectSort &other) const override;

private:
  KOREObjectCompositeSort(const std::string &Name, SortCategory category) : name(Name), category(category) {}
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
  /** At parse time, when parsed as part of a pattern,
      this will contain the list of actual sort parameters to the
      symbol. When parsed as part of a declaration, it contains the signature
      of the symbol. After instantiateSymbol is called on a symbol that is
      part of a pattern, it changes from being the sort parameters to being
      the signature of the symbol. instantiateSymbol is called on all object
      level symbols in axioms when KOREDefinition::preprocess is called. */
  std::vector<KOREObjectSort *> arguments;
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
  void addSort(KOREObjectSort *Sort);

  const std::string &getName() const { return name; }
  const std::vector<KOREObjectSort *> &getArguments() const {
    return arguments;
  }
  const KOREObjectSort *getSort() const { return sort; }
  KOREObjectSort *getSort() { return sort; }
  uint32_t getTag() const { assert(firstTag == lastTag); return firstTag; }
  void setTag(uint32_t val) { firstTag = lastTag = val; }
  uint16_t getLayout() const { return layout; }

  virtual void print(std::ostream &Out, unsigned indent = 0) const override;

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
  KOREObjectSort *getSort() const { return sort; }

  virtual void print(std::ostream &Out, unsigned indent = 0) const override;
  virtual void markSymbols(std::map<std::string, std::vector<KOREObjectSymbol *>> &) override {}
  virtual void markVariables(std::map<std::string, KOREObjectVariablePattern *> &map) override { map.insert({name->getName(), this}); }

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

  KOREObjectSymbol *getConstructor() const { return constructor; }
  const std::vector<KOREPattern *> &getArguments() const { return arguments; }

  void addArgument(KOREPattern *Argument);
  virtual void print(std::ostream &Out, unsigned indent = 0) const override;
  virtual void markSymbols(std::map<std::string, std::vector<KOREObjectSymbol *>> &) override;
  virtual void markVariables(std::map<std::string, KOREObjectVariablePattern *> &) override;

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
  llvm::StringMap<KOREObjectCompositePattern *> attributes;
  std::vector<KOREObjectSortVariable *> objectSortVariables;
  std::vector<KOREMetaSortVariable *> metaSortVariables;

public:
  void addAttribute(KOREPattern *Attribute);
  void addObjectSortVariable(KOREObjectSortVariable *SortVariable);
  void addMetaSortVariable(KOREMetaSortVariable *SortVariable);
  virtual void print(std::ostream &Out, unsigned indent = 0) const =0;
  const llvm::StringMap<KOREObjectCompositePattern *> &getAttributes() const { return attributes; }
  const std::vector<KOREObjectSortVariable *> &getObjectSortVariables() const { return objectSortVariables; }

protected:
  void printSortVariables(std::ostream &Out) const;
};

class KOREObjectCompositeSortDeclaration : public KOREDeclaration {
private:
  bool isHooked;
  std::string sortName;

public:
  static KOREObjectCompositeSortDeclaration *
  Create(const std::string &Name, bool isHooked = false) {
    return new KOREObjectCompositeSortDeclaration(Name, isHooked);
  }

  std::string getName() { return sortName; }

  virtual void print(std::ostream &Out, unsigned indent = 0) const override;

private:
  KOREObjectCompositeSortDeclaration(const std::string &Name, bool _isHooked)
  : sortName(Name), isHooked(_isHooked) { }
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
  bool isHooked;

public:
  static KOREObjectSymbolDeclaration *
  Create(const std::string &Name, bool isHooked = false) {
    KOREObjectSymbol *Sym = KOREObjectSymbol::Create(Name);
    return new KOREObjectSymbolDeclaration(Sym, isHooked);
  }

  virtual void print(std::ostream &Out, unsigned indent = 0) const override;

private:
  KOREObjectSymbolDeclaration(KOREObjectSymbol *Symbol, bool _isHooked)
  : KOREObjectSymbolAliasDeclaration(Symbol), isHooked(_isHooked) { }
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

public:
  static KOREAxiomDeclaration *Create() { return new KOREAxiomDeclaration(); }

  void addPattern(KOREPattern *Pattern);
  virtual void print(std::ostream &Out, unsigned indent = 0) const override;

  /* returns true if the axiom is actually required to be translated to llvm
     and false if it is an axiom pertaining to symbolic execution which is not
     required for concrete execution. Axioms that are not required are elided
     from the definition by KOREDefinition::preprocess. */
  bool isRequired();
  KOREPattern *getRightHandSide() const;

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
  llvm::StringMap<KOREObjectCompositePattern *> attributes;

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
  using KOREModuleMapType = llvm::StringMap<KOREModule *>;

  using KOREObjectSortConstructorMapType =
    llvm::StringMap<KOREObjectCompositeSort *>;

  using KOREMetaSortConstructorMapType =
    llvm::StringMap<KOREMetaCompositeSort *>;

  using KOREObjectSymbolMapType = llvm::StringMap<KOREObjectSymbol *>;

  using KOREMetaSymbolMapType = llvm::StringMap<KOREMetaSymbol *>;

  using KOREObjectSortVariableMapType =
    llvm::StringMap<KOREObjectSortVariable *>;

  using KOREMetaSortVariableMapType = llvm::StringMap<KOREMetaSortVariable *>;

  using KOREObjectVariableMapType = llvm::StringMap<KOREObjectVariable *>;

  using KOREMetaVariableMapType = llvm::StringMap<KOREMetaVariable *>;

  using KOREObjectCompositeSortDeclarationMapType = llvm::StringMap<KOREObjectCompositeSortDeclaration *>;

  using KOREObjectSymbolDeclarationMapType = llvm::StringMap<KOREObjectSymbolDeclaration *>;

private:
  // Symbol tables
  KOREObjectSortConstructorMapType objectSortConstructors;
  KOREMetaSortConstructorMapType metaSortConstructors;
  KOREObjectSymbolMapType objectSymbols;
  KOREMetaSymbolMapType metaSymbols;
  KOREObjectSortVariableMapType objectSortVariables;
  KOREMetaSortVariableMapType metaSortVariables;
  KOREObjectVariableMapType objectVariables;
  KOREMetaVariableMapType metaVariables;
  KOREModuleMapType moduleNames;
  KOREObjectCompositeSortDeclarationMapType sortDeclarations;
  KOREObjectSymbolDeclarationMapType symbolDeclarations;

  std::vector<KOREModule *> modules;
  llvm::StringMap<KOREObjectCompositePattern *> attributes;
  /* an automatically computed list of all the axioms in the definition */
  std::list<KOREAxiomDeclaration *> axioms;

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

  void addModule(KOREModule *Module);
  void addAttribute(KOREPattern *Attribute);
  void print(std::ostream &Out, unsigned indent = 0) const;

  const KOREObjectCompositeSortDeclarationMapType &getSortDeclarations() const { return sortDeclarations; }
  const KOREObjectSymbolDeclarationMapType &getSymbolDeclarations() const { return symbolDeclarations; }
  const std::list<KOREAxiomDeclaration *> &getAxioms() const { return axioms; }
  const llvm::StringMap<KOREObjectCompositePattern *> &getAttributes() const {
    return attributes;
  }
};

} // end namespace kllvm

#endif // AST_H
