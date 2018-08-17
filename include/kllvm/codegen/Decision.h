#ifndef DECISION_H
#define DECISION_H

#include "kllvm/ast/AST.h"

#include "llvm/ADT/StringMap.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Value.h"

namespace kllvm {

class Decision;

class DecisionNode {
public:
  virtual void codegen(Decision *d, llvm::StringMap<llvm::Value *> substitution) = 0;
};

class DecisionCase {
private:
  KOREObjectSymbol *constructor;
  std::vector<std::string> bindings;
  llvm::APInt literal;
  DecisionNode *child;

public:
  DecisionCase(KOREObjectSymbol *constructor, DecisionNode *child) :
    constructor(constructor), child(child) {}
  DecisionCase(KOREObjectSymbol *dv, llvm::APInt literal, DecisionNode *child) :
    constructor(dv), literal(literal), child(child) {}

  KOREObjectSymbol *getConstructor() const { return constructor; }
  const std::vector<std::string> &getBindings() const { return bindings; }
  void addBinding(std::string name) { bindings.push_back(name); }
  llvm::APInt getLiteral() const { return literal; }
  DecisionNode *getChild() const { return child; }
};
  
class SwitchNode : public DecisionNode {
private:
  std::vector<DecisionCase> cases;
  std::string name;

  SwitchNode(const std::string &name) : name(name) {}

public:
  void addCase(DecisionCase _case) { cases.push_back(_case); }

  static SwitchNode *Create(const std::string &name) {
    return new SwitchNode(name);
  }

  std::string getName() const { return name; }
  const std::vector<DecisionCase> &getCases() const { return cases; }
  
  virtual void codegen(Decision *d, llvm::StringMap<llvm::Value *> substitution);
};

class FunctionNode : public DecisionNode {
private:
  std::vector<std::string> bindings;
  std::string name;
  std::string function;
  DecisionNode *child;
  SortCategory cat;
  
  FunctionNode(
    const std::string &name,
    const std::string &function,
    DecisionNode *child,
    SortCategory cat) :
      name(name),
      function(function),
      child(child),
      cat(cat) {}

public:
  static FunctionNode *Create(
      const std::string &name,
      const std::string &function,
      DecisionNode *child,
      SortCategory cat) {
    return new FunctionNode(name, function, child, cat);
  }

  const std::vector<std::string> &getBindings() const { return bindings; }
  void addBinding(std::string name) { bindings.push_back(name); }
  
  virtual void codegen(Decision *d, llvm::StringMap<llvm::Value *> substitution);
};

class LeafNode : public DecisionNode {
private:
  std::vector<std::string> bindings;
  std::string name;

  LeafNode(const std::string &name) : name(name) {}

public:
  static LeafNode *Create(const std::string &name) {
    return new LeafNode(name);
  }

  const std::vector<std::string> &getBindings() const { return bindings; }
  void addBinding(std::string name) { bindings.push_back(name); }
  
  virtual void codegen(Decision *d, llvm::StringMap<llvm::Value *> substitution);
};

class FailNode : public DecisionNode {
private:
  FailNode() {}

  static FailNode instance;
public:
  static FailNode *get() { return &instance; }

  virtual void codegen(Decision *d, llvm::StringMap<llvm::Value *> substitution) {}
};

class Decision {
private:
  KOREDefinition *Definition;
  llvm::BasicBlock *CurrentBlock;
  llvm::BasicBlock *StuckBlock;
  llvm::Module *Module;
  llvm::LLVMContext &Ctx;
  SortCategory Cat;

public:
  Decision(
    KOREDefinition *Definition,
    llvm::BasicBlock *EntryBlock,
    llvm::BasicBlock *StuckBlock,
    llvm::Module *Module,
    SortCategory Cat) :
      Definition(Definition),
      CurrentBlock(EntryBlock),
      StuckBlock(StuckBlock),
      Module(Module),
      Ctx(Module->getContext()),
      Cat(Cat) {}

  /* adds code to the specified basic block to take a single step based on
     the specified decision tree and return the result of taking that step. */
  void operator()(DecisionNode *entry, llvm::StringMap<llvm::Value *> substitution);

  llvm::Value *getTag(llvm::Value *);

  friend class SwitchNode;
  friend class FunctionNode;
  friend class LeafNode;
};

void makeEvalFunction(KOREObjectSymbol *function, KOREDefinition *definition, llvm::Module *module, DecisionNode *dt);

}
#endif // DECISION_H
