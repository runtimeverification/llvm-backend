#ifndef DECISION_H
#define DECISION_H

#include "kllvm/ast/AST.h"

#include "llvm/ADT/StringMap.h"
#include "llvm/IR/Instructions.h" 
#include "llvm/IR/Module.h"
#include "llvm/IR/Value.h"

#include "kllvm/codegen/DecisionParser.h"

namespace kllvm {

class Decision;
class DecisionCase;

class DecisionNode {
public:
  llvm::BasicBlock * cachedCode = nullptr;
  llvm::StringMap<std::map<llvm::Type *, llvm::PHINode *>> phis;
  std::vector<llvm::BasicBlock *> predecessors;
  /* completed tracks whether codegen for this DecisionNode has concluded */
  bool completed = false;

  virtual void codegen(Decision *d, llvm::StringMap<llvm::Value *> substitution) = 0;
  virtual void collectVars(void) = 0;
  virtual void collectFail(void) = 0;
  void sharedNode(Decision *d, llvm::StringMap<llvm::Value *> &oldSubst, llvm::StringMap<llvm::Value *> &substitution, llvm::BasicBlock *Block);
  bool beginNode(Decision *d, std::string name, llvm::StringMap<llvm::Value *> &substitution);

  void setCompleted() { completed = true; }
  bool isCompleted() const { return completed; }

private:
  bool hasVars = false, hasContainsFail = false, containsFailNode = false;
  std::set<std::string> vars;
  friend class SwitchNode;
  friend class MakePatternNode;
  friend class FunctionNode;
  friend class LeafNode;
  friend class FailNode;
  friend class MakeIteratorNode;
  friend class IterNextNode;
};

class DecisionCase {
private:
  /* constructor to switch on. if null, this is a wildcard match.
     if equal to \\dv, we are matching on a bool or mint literal. */
  KORESymbol *constructor;
  /* the names to bind the children of this pattern to. */
  std::vector<std::string> bindings;
  /* the literal int to match on. must have a bit width equal to the
     size of the sort being matched. */
  llvm::APInt literal;
  /* the node in the tree to jump to if this constructor is matched */
  DecisionNode *child;

public:
  DecisionCase(
    KORESymbol *constructor, 
    std::vector<std::string> bindings,
    DecisionNode *child) :
      constructor(constructor),
      bindings(bindings),
      child(child) {}
  DecisionCase(KORESymbol *dv, llvm::APInt literal, DecisionNode *child) :
    constructor(dv), literal(literal), child(child) {}

  KORESymbol *getConstructor() const { return constructor; }
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
  virtual void collectVars() { 
    if(hasVars) return;
    for (auto _case : cases) { 
      _case.getChild()->collectVars();
      std::set<std::string> caseVars = _case.getChild()->vars;
      for (std::string var : _case.getBindings()) {
        caseVars.erase(var);
      }
      vars.insert(caseVars.begin(), caseVars.end());
    }
    if(cases.size() != 1 || cases[0].getConstructor()) vars.insert(name); 
    hasVars = true;
  }
  virtual void collectFail() {
    if(hasContainsFail) return;
    bool hasDefault = false;
    for (auto _case : cases) {
      _case.getChild()->collectFail();
      containsFailNode = containsFailNode || _case.getChild()->containsFailNode;
      hasDefault = hasDefault || _case.getConstructor() == nullptr;
    }
    if (!hasDefault) {
      containsFailNode = true;
    }
    hasContainsFail = true;
  }
};

class MakePatternNode : public DecisionNode {
private:
  std::string name;
  KOREPattern *pattern;
  std::vector<std::string> uses;
  DecisionNode *child;

  MakePatternNode(
    const std::string &name,
    KOREPattern *pattern,
    std::vector<std::string> &uses,
    DecisionNode *child) :
      name(name),
      pattern(pattern),
      uses(uses),
      child(child) {}

public:
  static MakePatternNode *Create(
      const std::string &name,
      KOREPattern *pattern,
      std::vector<std::string> &uses,
      DecisionNode *child) {
    return new MakePatternNode(name, pattern, uses, child);
  }

  virtual void codegen(Decision *d, llvm::StringMap<llvm::Value *> substitution);
  virtual void collectVars() {
    if(hasVars) return;
    child->collectVars();
    vars = child->vars;
    vars.erase(name);
    vars.insert(uses.begin(), uses.end());
    hasVars = true;
  }
  virtual void collectFail() {
    if (hasContainsFail) return;
    child->collectFail();
    containsFailNode = containsFailNode || child->containsFailNode;
  }
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
  virtual void collectVars() { 
    if (hasVars) return;
    child->collectVars();
    vars = child->vars;
    vars.erase(name);
    for (auto var : bindings) { 
      if (var.find_first_not_of("-0123456789") != std::string::npos) {
        vars.insert(var);
      }
    }
    hasVars = true;
  }
  virtual void collectFail() {
    if (hasContainsFail) return;
    child->collectFail();
    containsFailNode = containsFailNode || child->containsFailNode;
  }
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
  virtual void collectVars() {
    if (hasVars) return;
    vars.insert(bindings.begin(), bindings.end());
    hasVars = true;
  }
  virtual void collectFail() {}
};

class MakeIteratorNode : public DecisionNode {
private:
  std::string collection;
  std::string name;
  std::string hookName;
  DecisionNode *child;

  MakeIteratorNode(const std::string &collection, const std::string &name, const std::string &hookName, DecisionNode *child) : collection(collection), name(name), hookName(hookName), child(child) {}

public:
  static MakeIteratorNode *Create(const std::string &collection, const std::string &name, const std::string &hookName, DecisionNode *child) {
    return new MakeIteratorNode(collection, name, hookName, child);
  }

  virtual void codegen(Decision *d, llvm::StringMap<llvm::Value *> substitution);
  virtual void collectVars() {
    if (hasVars) return;
    child->collectVars();
    vars = child->vars;
    vars.erase(name);
    vars.insert(collection);
    hasVars = true;
  }
  virtual void collectFail() {
    if (hasContainsFail) return;
    child->collectFail();
    containsFailNode = containsFailNode || child->containsFailNode;
  }
};

class IterNextNode : public DecisionNode {
private:
  std::string iterator;
  std::string binding;
  std::string hookName;
  DecisionNode *child;

  IterNextNode(const std::string &iterator, const std::string &binding, const std::string &hookName, DecisionNode *child) : iterator(iterator), binding(binding), hookName(hookName), child(child) {}

public:
  static IterNextNode *Create(const std::string &iterator, const std::string &binding, const std::string &hookName, DecisionNode *child) {
    return new IterNextNode(iterator, binding, hookName, child);
  }

  virtual void codegen(Decision *d, llvm::StringMap<llvm::Value *> substitution);
  virtual void collectVars() {
    if (hasVars) return;
    child->collectVars();
    vars = child->vars;
    vars.erase(binding);
    vars.insert(iterator);
    hasVars = true;
  }
  virtual void collectFail() {
    if (hasContainsFail) return;
    child->collectFail();
    containsFailNode = containsFailNode || child->containsFailNode;
  }
};

class FailNode : public DecisionNode {
private:
  FailNode() {}

  static FailNode instance;
public:
  static FailNode *get() { return &instance; }

  virtual void codegen(Decision *d, llvm::StringMap<llvm::Value *> substitution) { abort(); }
  virtual void collectVars() {}
  virtual void collectFail() { containsFailNode = true; }
};

class Decision {
private:
  KOREDefinition *Definition;
  llvm::BasicBlock *CurrentBlock;
  llvm::BasicBlock *FailureBlock;
  llvm::IndirectBrInst *FailJump;
  llvm::AllocaInst *FailAddress;
  llvm::BasicBlock *ChoiceBlock;
  DecisionNode *ChoiceNode;
  std::set<std::string> ChoiceVars;
  llvm::Module *Module;
  llvm::LLVMContext &Ctx;
  ValueType Cat;

  llvm::StringMap<std::map<llvm::Type *, llvm::PHINode *>> failPhis;

  llvm::Value *getTag(llvm::Value *);
  void addFailPhiIncoming(llvm::StringMap<llvm::Value *> oldSubst, llvm::BasicBlock *switchBlock);
public:
  Decision(
    KOREDefinition *Definition,
    llvm::BasicBlock *EntryBlock,
    llvm::BasicBlock *FailureBlock,
    llvm::IndirectBrInst *FailJump,
    llvm::AllocaInst *FailAddress,
    llvm::Module *Module,
    ValueType Cat) :
      Definition(Definition),
      CurrentBlock(EntryBlock),
      FailureBlock(FailureBlock),
      FailJump(FailJump),
      FailAddress(FailAddress),
      ChoiceBlock(nullptr),
      ChoiceNode(nullptr),
      ChoiceVars(),
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
  friend class MakeIteratorNode;
  friend class IterNextNode;
  friend class DecisionNode;
};

/* construct the function that evaluates the specified function symbol
   according to the specified decision tree and returns the result of the
   function. */
void makeEvalFunction(KORESymbol *function, KOREDefinition *definition, llvm::Module *module, DecisionNode *dt);
void makeAnywhereFunction(KORESymbol *function, KOREDefinition *definition, llvm::Module *module, DecisionNode *dt);

void makeStepFunction(KOREDefinition *definition, llvm::Module *module, DecisionNode *dt);
void makeStepFunction(KOREAxiomDeclaration *axiom, KOREDefinition *definition, llvm::Module *module, PartialStep res);

}
#endif // DECISION_H
