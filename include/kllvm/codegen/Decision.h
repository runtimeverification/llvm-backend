#ifndef DECISION_H
#define DECISION_H

#include "kllvm/ast/AST.h"

#include "llvm/ADT/StringMap.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Value.h"

namespace kllvm {

class Decision;
class DecisionCase;

class DecisionNode {
public:
  llvm::BasicBlock * cachedCode = nullptr;
  llvm::StringMap<llvm::PHINode *> phis;
  /* completed tracks whether codegen for this DecisionNode has concluded */
  bool completed = false;

  virtual void codegen(Decision *d, llvm::StringMap<llvm::Value *> substitution) = 0;
  virtual void collectUses(std::set<std::string> &vars) = 0;
  virtual void collectDefs(std::set<std::string> &vars) = 0;
  std::set<std::string> collectVars(void);
  bool beginNode(Decision *d, std::string name, llvm::StringMap<llvm::Value *> &substitution);

  void setCompleted() { completed = true; }
  bool isCompleted() const { return completed; }
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
  /* the node in the tree to jump to if this constructor is matched */
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

  bool isCheckNull;

  SwitchNode(const std::string &name, bool isCheckNull) : name(name), isCheckNull(isCheckNull) {}

public:
  void addCase(DecisionCase _case) { cases.push_back(_case); }

  static SwitchNode *Create(const std::string &name, bool isCheckNull) {
    return new SwitchNode(name, isCheckNull);
  }

  std::string getName() const { return name; }
  const std::vector<DecisionCase> &getCases() const { return cases; }
  
  virtual void codegen(Decision *d, llvm::StringMap<llvm::Value *> substitution);
  virtual void collectUses(std::set<std::string> &vars) { if(cases.size() != 1 || cases[0].getConstructor()) vars.insert(name); for (auto _case : cases) { _case.getChild()->collectUses(vars); } }
  virtual void collectDefs(std::set<std::string> &vars) { for (auto _case : cases) { vars.insert(_case.getBindings().begin(), _case.getBindings().end()); _case.getChild()->collectDefs(vars); } }
};

class MakePatternNode : public DecisionNode {
private:
  std::string name;
  KOREObjectPattern *pattern;
  DecisionNode *child;

  MakePatternNode(
    const std::string &name,
    KOREObjectPattern *pattern,
    DecisionNode *child) :
      name(name),
      pattern(pattern),
      child(child) {}

public:
  static MakePatternNode *Create(
      const std::string &name,
      KOREObjectPattern *pattern,
      DecisionNode *child) {
    return new MakePatternNode(name, pattern, child);
  }

  virtual void codegen(Decision *d, llvm::StringMap<llvm::Value *> substitution);
  virtual void collectUses(std::set<std::string> &vars) { child->collectUses(vars); }
  virtual void collectDefs(std::set<std::string> &vars) { vars.insert(name); child->collectDefs(vars); }
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
  ValueType cat;
  
  FunctionNode(
    const std::string &name,
    const std::string &function,
    DecisionNode *child,
    ValueType cat) :
      name(name),
      function(function),
      child(child),
      cat(cat) {}

public:
  static FunctionNode *Create(
      const std::string &name,
      const std::string &function,
      DecisionNode *child,
      ValueType cat) {
    return new FunctionNode(name, function, child, cat);
  }

  const std::vector<std::string> &getBindings() const { return bindings; }
  void addBinding(std::string name) { bindings.push_back(name); }
  
  virtual void codegen(Decision *d, llvm::StringMap<llvm::Value *> substitution);
  virtual void collectUses(std::set<std::string> &vars) { 
    for (auto var : bindings) { 
      if (var.find_first_not_of("-0123456789") != std::string::npos) {
        vars.insert(var);
      }
    }
    child->collectUses(vars);
  }
  virtual void collectDefs(std::set<std::string> &vars) { vars.insert(name); child->collectDefs(vars); }
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
  virtual void collectUses(std::set<std::string> &vars) { vars.insert(bindings.begin(), bindings.end()); }
  virtual void collectDefs(std::set<std::string> &vars) {}
};

class FailNode : public DecisionNode {
private:
  FailNode() {}

  static FailNode instance;
public:
  static FailNode *get() { return &instance; }

  virtual void codegen(Decision *d, llvm::StringMap<llvm::Value *> substitution) { abort(); }
  virtual void collectUses(std::set<std::string> &vars) {}
  virtual void collectDefs(std::set<std::string> &vars) {}
};

class Decision {
private:
  KOREDefinition *Definition;
  llvm::BasicBlock *CurrentBlock;
  llvm::BasicBlock *StuckBlock;
  llvm::Module *Module;
  llvm::LLVMContext &Ctx;
  ValueType Cat;

  llvm::Value *getTag(llvm::Value *);
public:
  Decision(
    KOREDefinition *Definition,
    llvm::BasicBlock *EntryBlock,
    llvm::BasicBlock *StuckBlock,
    llvm::Module *Module,
    ValueType Cat) :
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
  friend class MakePatternNode;
  friend class FunctionNode;
  friend class LeafNode;
  friend class DecisionNode;
};

/* construct the function that evaluates the specified function symbol
   according to the specified decision tree and returns the result of the
   function. */
void makeEvalFunction(KOREObjectSymbol *function, KOREDefinition *definition, llvm::Module *module, DecisionNode *dt);

void makeStepFunction(KOREDefinition *definition, llvm::Module *module, DecisionNode *dt);

}
#endif // DECISION_H
