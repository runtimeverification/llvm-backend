#ifndef AST_H
#define AST_H

#include "llvm/ADT/StringMap.h"

#include <string>
#include <vector>

namespace kllvm {


// KORESort
class KORESort {
public:
  virtual void print(std::ostream &Out, unsigned indent = 0) const =0;
};

struct HashSort {
  size_t operator()(const kllvm::KORESort &s) const noexcept {
    std::ostringstream Out;
    s.print(Out);
    return std::hash<std::string>{}(Out.str());
  }
};

class KOREObjectSort : public KORESort {
public:
  virtual bool operator==(const KOREObjectSort &other) const = 0;
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

class KOREObjectCompositeSort : public KOREObjectSort {
private:
  std::string name;
  std::vector<KOREObjectSort *> arguments;

public:
  static KOREObjectCompositeSort *Create(const std::string &Name) {
    return new KOREObjectCompositeSort(Name);
  }

  void addArgument(KOREObjectSort *Argument);
  virtual void print(std::ostream &Out, unsigned indent = 0) const override;
  virtual bool operator==(const KOREObjectSort &other) const override;

private:
  KOREObjectCompositeSort(const std::string &Name) : name(Name) { }
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

class KOREObjectSymbol : public KORESymbol {
private:
  std::string name;
  std::vector<KOREObjectSort *> arguments;
  KOREObjectSort *sort;

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

  virtual void print(std::ostream &Out, unsigned indent = 0) const override;

private:
  KOREObjectSymbol(const std::string &Name) : name(Name), sort(nullptr) { }
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

// KOREPattern
class KOREPattern {
public:
  virtual void print(std::ostream &Out, unsigned indent = 0) const =0;
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

  virtual void print(std::ostream &Out, unsigned indent = 0) const override;

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

  void addArgument(KOREPattern *Argument);
  virtual void print(std::ostream &Out, unsigned indent = 0) const override;

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

  virtual void print(std::ostream &Out, unsigned indent = 0) const override;

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

private:
  KOREMetaCharPattern(char Contents) : contents(Contents) { }
};

// KOREDeclaration
class KOREDeclaration {
protected:
  std::vector<KOREPattern *> attributes;
  std::vector<KOREObjectSortVariable *> objectSortVariables;
  std::vector<KOREMetaSortVariable *> metaSortVariables;

public:
  void addAttribute(KOREPattern *Attribute);
  void addObjectSortVariable(KOREObjectSortVariable *SortVariable);
  void addMetaSortVariable(KOREMetaSortVariable *SortVariable);
  virtual void print(std::ostream &Out, unsigned indent = 0) const =0;

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
  std::vector<KOREPattern *> attributes;

public:
  static KOREModule *Create(const std::string &Name) {
    return new KOREModule(Name);
  }

  void addAttribute(KOREPattern *Attribute);
  void addDeclaration(KOREDeclaration *Declaration);
  void print(std::ostream &Out, unsigned indent = 0) const;

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

  std::vector<KOREModule *> modules;
  std::vector<KOREPattern *> attributes;

public:
  static KOREDefinition *Create() { return new KOREDefinition(); }

  void addModule(KOREModule *Module);
  void addAttribute(KOREPattern *Attribute);
  void print(std::ostream &Out, unsigned indent = 0) const;
};

} // end namespace kllvm

#endif // AST_H
