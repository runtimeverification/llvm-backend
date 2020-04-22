#ifndef DECISION_H
#define DECISION_H

#include "kllvm/ast/AST.h"

#include "llvm/ADT/StringMap.h"
#include "llvm/IR/Instructions.h" 
#include "llvm/IR/Module.h"
#include "llvm/IR/Value.h"

#include "kllvm/codegen/DecisionParser.h"

#include <unordered_set>

namespace kllvm {

class Decision;
class DecisionCase;
class LeafNode;
class IterNextNode;

struct HashVar {
  size_t operator()(const std::pair<std::string, llvm::Type *> &s) const noexcept {
    return std::hash<std::string>()(s.first) ^ std::hash<llvm::Type *>()(s.second);
  }
};


using var_type = std::pair<std::string, llvm::Type *>;
using var_set_type = std::unordered_map<var_type, std::unordered_set<IterNextNode *>, HashVar>;

class DecisionNode {
public:
  llvm::BasicBlock * cachedCode = nullptr;
  /* completed tracks whether codegen for this DecisionNode has concluded */
  bool completed = false;

  virtual void codegen(Decision *d) = 0;
  virtual void preprocess(std::unordered_set<LeafNode *> &) = 0;
  bool beginNode(Decision *d, std::string name);

  void setCompleted() { completed = true; }
  bool isCompleted() const { return completed; }
  uint64_t getChoiceDepth() const { return choiceDepth; }

private:
  bool preprocessed = false, containsFailNode = false;
  uint64_t choiceDepth = 0;
  friend class Decision;
  friend class SwitchNode;
  friend class MakePatternNode;
  friend class FunctionNode;
  friend class LeafNode;
  friend class FailNode;
  friend class MakeIteratorNode;
  friend class IterNextNode;
};

class FailNode : public DecisionNode {
private:
  FailNode() {}

  static FailNode instance;
public:
  static FailNode *get() { return &instance; }

  virtual void codegen(Decision *d) { abort(); }
  virtual void preprocess(std::unordered_set<LeafNode *> &) { containsFailNode = true; }
};

class DecisionCase {
private:
  /* constructor to switch on. if null, this is a wildcard match.
     if equal to \\dv, we are matching on a bool or mint literal. */
  KORESymbol *constructor;
  /* the names to bind the children of this pattern to. */
  std::vector<var_type> bindings;
  /* the literal int to match on. must have a bit width equal to the
     size of the sort being matched. */
  llvm::APInt literal;
  /* the node in the tree to jump to if this constructor is matched */
  DecisionNode *child;

public:
  DecisionCase(
    KORESymbol *constructor, 
    std::vector<var_type> bindings,
    DecisionNode *child) :
      constructor(constructor),
      bindings(bindings),
      child(child) {}
  DecisionCase(KORESymbol *dv, llvm::APInt literal, DecisionNode *child) :
    constructor(dv), literal(literal), child(child) {}

  KORESymbol *getConstructor() const { return constructor; }
  const std::vector<var_type> &getBindings() const { return bindings; }
  void addBinding(std::string name, llvm::Type *type) { bindings.push_back(std::make_pair(name, type)); }
  llvm::APInt getLiteral() const { return literal; }
  DecisionNode *getChild() const { return child; }
};
  
class SwitchNode : public DecisionNode {
private:
  /* the list of switch cases */
  std::vector<DecisionCase> cases;
  /* the name of the variable being matched on. */
  std::string name;
  llvm::Type *type;

  bool isCheckNull;

  SwitchNode(const std::string &name, llvm::Type *type, bool isCheckNull) : name(name), type(type), isCheckNull(isCheckNull) {}

public:
  void addCase(DecisionCase _case) { cases.push_back(_case); }

  static SwitchNode *Create(const std::string &name, llvm::Type *type, bool isCheckNull) {
    return new SwitchNode(name, type, isCheckNull);
  }

  std::string getName() const { return name; }
  llvm::Type *getType() const { return type; }
  const std::vector<DecisionCase> &getCases() const { return cases; }
  
  virtual void codegen(Decision *d);
  virtual void preprocess(std::unordered_set<LeafNode *> &leaves) {
    if(preprocessed) return;
    bool hasDefault = false;
    for (auto _case : cases) {
      _case.getChild()->preprocess(leaves);
      containsFailNode = containsFailNode || _case.getChild()->containsFailNode;
      hasDefault = hasDefault || _case.getConstructor() == nullptr;
      choiceDepth = std::max(choiceDepth, _case.getChild()->choiceDepth);
    }
    if (!hasDefault) {
      containsFailNode = true;
    }
    preprocessed = true;
  }
};

class MakePatternNode : public DecisionNode {
private:
  std::string name;
  llvm::Type *type;
  KOREPattern *pattern;
  std::vector<var_type> uses;
  DecisionNode *child;

  MakePatternNode(
    const std::string &name,
    llvm::Type *type,
    KOREPattern *pattern,
    std::vector<var_type> &uses,
    DecisionNode *child) :
      name(name),
      type(type),
      pattern(pattern),
      uses(uses),
      child(child) {}

public:
  static MakePatternNode *Create(
      const std::string &name,
      llvm::Type *type,
      KOREPattern *pattern,
      std::vector<var_type> &uses,
      DecisionNode *child) {
    return new MakePatternNode(name, type, pattern, uses, child);
  }

  virtual void codegen(Decision *d);
  virtual void preprocess(std::unordered_set<LeafNode *> &leaves) {
    if (preprocessed) return;
    child->preprocess(leaves);
    containsFailNode = containsFailNode || child->containsFailNode;
    choiceDepth = child->choiceDepth;
    preprocessed = true;
  }
};



class FunctionNode : public DecisionNode {
private:
  /* the list of arguments to the function. */
  std::vector<var_type> bindings;
  /* the name of the variable to bind to the result of the function. */
  std::string name;
  /* the name of the function to call */
  std::string function;
  /* the successor node in the tree */
  DecisionNode *child;
  ValueType cat;
  llvm::Type *type;
  
  FunctionNode(
    const std::string &name,
    const std::string &function,
    DecisionNode *child,
    ValueType cat,
    llvm::Type *type) :
      name(name),
      function(function),
      child(child),
      cat(cat),
      type(type) {}

public:
  static FunctionNode *Create(
      const std::string &name,
      const std::string &function,
      DecisionNode *child,
      ValueType cat,
      llvm::Type *type) {
    return new FunctionNode(name, function, child, cat, type);
  }

  const std::vector<var_type> &getBindings() const { return bindings; }
  void addBinding(std::string name, llvm::Type *type) { bindings.push_back(std::make_pair(name, type)); }
  
  virtual void codegen(Decision *d);
  virtual void preprocess(std::unordered_set<LeafNode *> &leaves) {
    if (preprocessed) return;
    child->preprocess(leaves);
    containsFailNode = containsFailNode || child->containsFailNode;
    choiceDepth = child->choiceDepth;
    preprocessed = true;
  }
};

class LeafNode : public DecisionNode {
private:
  /* the names in the decision tree of the variables used in the rhs of
     this rule, in alphabetical order of their names in the rule. */
  std::vector<var_type> bindings;
  /* the name of the function that constructs the rhs of this rule from
     the substitution */
  std::string name;

  LeafNode(const std::string &name) : name(name) {}

public:
  static LeafNode *Create(const std::string &name) {
    return new LeafNode(name);
  }

  const std::vector<var_type> &getBindings() const { return bindings; }
  void addBinding(std::string name, llvm::Type *type) { bindings.push_back(std::make_pair(name, type)); }
  
  virtual void codegen(Decision *d);
  virtual void preprocess(std::unordered_set<LeafNode *> &leaves) {
    leaves.insert(this);
  }
};

class MakeIteratorNode : public DecisionNode {
private:
  std::string collection;
  llvm::Type *collectionType;
  std::string name;
  llvm::Type *type;
  std::string hookName;
  DecisionNode *child;

  MakeIteratorNode(const std::string &collection, llvm::Type *collectionType, const std::string &name, llvm::Type *type, const std::string &hookName, DecisionNode *child) : collection(collection), collectionType(collectionType), name(name), type(type), hookName(hookName), child(child) {}

public:
  static MakeIteratorNode *Create(const std::string &collection, llvm::Type *collectionType, const std::string &name, llvm::Type *type, const std::string &hookName, DecisionNode *child) {
    return new MakeIteratorNode(collection, collectionType, name, type, hookName, child);
  }

  virtual void codegen(Decision *d);
  virtual void preprocess(std::unordered_set<LeafNode *> &leaves) {
    if (preprocessed) return;
    child->preprocess(leaves);
    containsFailNode = containsFailNode || child->containsFailNode;
    choiceDepth = child->choiceDepth;
    preprocessed = true;
  }
};

class IterNextNode : public DecisionNode {
private:
  std::string iterator;
  llvm::Type *iteratorType;
  std::string binding;
  llvm::Type *bindingType;
  std::string hookName;
  DecisionNode *child;

  IterNextNode(const std::string &iterator, llvm::Type *iteratorType, const std::string &binding, llvm::Type *bindingType, const std::string &hookName, DecisionNode *child) : iterator(iterator), iteratorType(iteratorType), binding(binding), bindingType(bindingType), hookName(hookName), child(child) {}

public:
  static IterNextNode *Create(const std::string &iterator, llvm::Type *iteratorType, const std::string &binding, llvm::Type *bindingType, const std::string &hookName, DecisionNode *child) {
    return new IterNextNode(iterator, iteratorType, binding, bindingType, hookName, child);
  }

  virtual void codegen(Decision *d);
  virtual void preprocess(std::unordered_set<LeafNode *> &leaves) {
    if (preprocessed) return;
    child->preprocess(leaves);
    containsFailNode = containsFailNode || child->containsFailNode;
    choiceDepth = child->choiceDepth + 1;
    preprocessed = true;
  }
};

class Decision {
private:
  KOREDefinition *Definition;
  llvm::BasicBlock *CurrentBlock;
  llvm::BasicBlock *FailureBlock;
  llvm::IndirectBrInst *FailJump;
  llvm::AllocaInst *ChoiceBuffer;
  llvm::AllocaInst *ChoiceDepth;
  llvm::BasicBlock *ChoiceBlock;
  llvm::Module *Module;
  llvm::LLVMContext &Ctx;
  ValueType Cat;

  std::map<var_type, llvm::AllocaInst *> symbols;

  llvm::Value *getTag(llvm::Value *);

  llvm::AllocaInst *decl(var_type name);

  llvm::Constant *stringLiteral(std::string name);

public:
  Decision(
    KOREDefinition *Definition,
    llvm::BasicBlock *EntryBlock,
    llvm::BasicBlock *FailureBlock,
    llvm::IndirectBrInst *FailJump,
    llvm::AllocaInst *ChoiceBuffer,
    llvm::AllocaInst *ChoiceDepth,
    llvm::Module *Module,
    ValueType Cat) :
      Definition(Definition),
      CurrentBlock(EntryBlock),
      FailureBlock(FailureBlock),
      FailJump(FailJump),
      ChoiceBuffer(ChoiceBuffer),
      ChoiceDepth(ChoiceDepth),
      ChoiceBlock(nullptr),
      Module(Module),
      Ctx(Module->getContext()),
      Cat(Cat) {}

  /* adds code to the specified basic block to take a single step based on
     the specified decision tree and return the result of taking that step. */
  void operator()(DecisionNode *entry);
  void store(var_type name, llvm::Value *val);
  llvm::Value *load(var_type name);

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
