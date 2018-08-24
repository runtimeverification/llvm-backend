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
  /* constructor to switch on. if null, this is a wildcard match.
     if equal to \\dv, we are matching on a bool or mint literal. */
  KOREObjectSymbol *constructor;
  /* the names to bind the children of this pattern to. */
  std::vector<std::string> bindings;
  /* the literal int to match on. must have a bit width equal to the
     size of the sort being matched. */
  llvm::APInt literal;
  /* the node in the tree to juwp to if this constructor is matched */
  DecisionNode *child;

public:
  DecisionCase(
    KOREObjectSymbol *constructor, 
    std::vector<std::string> bindings,
    DecisionNode *child) :
      constructor(constructor),
      bindings(bindings),
      child(child) {}
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
  /* the list of switch cases */
  std::vector<DecisionCase> cases;
  /* the name of the variable being matched on. */
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
  /* the list of arguments to the function. */
  std::vector<std::string> bindings;
  /* the name of the variable to bind to the result of the function. */
  std::string name;
  /* the name of the function to call */
  std::string function;
  /* the successor node in the tree */
  DecisionNode *child;
  /* the return sort of the function */
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
  /* the names in the decision tree of the variables used in the rhs of
     this rule, in alphabetical order of their names in the rule. */
  std::vector<std::string> bindings;
  /* the name of the function that constructs the rhs of this rule from
     the substitution */
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

  llvm::Value *getTag(llvm::Value *);
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

  friend class SwitchNode;
  friend class FunctionNode;
  friend class LeafNode;
};

/* construct the function that evaluates the specified function symbol
   according to the specified decision tree and returns the result of the
   function. */
void makeEvalFunction(KOREObjectSymbol *function, KOREDefinition *definition, llvm::Module *module, DecisionNode *dt);

}
#endif // DECISION_H
