#ifndef DECISION_H
#define DECISION_H

#include "kllvm/ast/AST.h"
#include "kllvm/codegen/CreateTerm.h"
#include "kllvm/codegen/DecisionParser.h"

#include <llvm/ADT/APInt.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Constant.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Value.h>

#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <map>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

namespace kllvm {

class decision;
class leaf_node;
class iter_next_node;

struct hash_var {
  size_t
  operator()(std::pair<std::string, llvm::Type *> const &s) const noexcept {
    return std::hash<std::string>()(s.first)
           ^ std::hash<llvm::Type *>()(s.second);
  }
};

using var_type = std::pair<std::string, llvm::Type *>;
using var_set_type
    = std::unordered_map<var_type, std::unordered_set<iter_next_node *>, hash_var>;

class decision_node {
private:
  llvm::BasicBlock *cachedCode = nullptr;
  /* completed tracks whether codegen for this decision_node has concluded */
  bool completed = false;

public:
  virtual ~decision_node() = default;

  virtual void codegen(decision *d) = 0;
  virtual void preprocess(std::unordered_set<leaf_node *> &) = 0;
  bool beginNode(decision *d, std::string const &name);

  void setCompleted() { completed = true; }
  [[nodiscard]] bool isCompleted() const { return completed; }
  [[nodiscard]] uint64_t getChoiceDepth() const { return choiceDepth; }

private:
  bool preprocessed = false, containsfail_node = false;
  uint64_t choiceDepth = 0;
  friend class decision;
  friend class switch_node;
  friend class make_pattern_node;
  friend class function_node;
  friend class leaf_node;
  friend class fail_node;
  friend class make_iterator_node;
  friend class iter_next_node;
};

class fail_node : public decision_node {
private:
  fail_node() = default;

  static fail_node instance;

public:
  ~fail_node() override = default;
  static fail_node *get() { return &instance; }

  void codegen(decision *d) override { abort(); }
  void preprocess(std::unordered_set<leaf_node *> &) override {
    containsfail_node = true;
  }
};

class decision_case {
private:
  /* constructor to switch on. if null, this is a wildcard match.
     if equal to \\dv, we are matching on a bool or mint literal. */
  kore_symbol *constructor;
  /* the names to bind the children of this pattern to. */
  std::vector<var_type> bindings;
  /* the literal int to match on. must have a bit width equal to the
     size of the sort being matched. */
  llvm::APInt literal;
  /* the node in the tree to jump to if this constructor is matched */
  decision_node *child;

public:
  decision_case(
      kore_symbol *constructor, std::vector<var_type> bindings,
      decision_node *child)
      : constructor(constructor)
      , bindings(std::move(std::move(bindings)))
      , child(child) { }
  decision_case(kore_symbol *dv, llvm::APInt literal, decision_node *child)
      : constructor(dv)
      , literal(std::move(std::move(literal)))
      , child(child) { }

  [[nodiscard]] kore_symbol *getConstructor() const { return constructor; }
  [[nodiscard]] std::vector<var_type> const &getBindings() const {
    return bindings;
  }
  void addBinding(std::string const &name, value_type type, llvm::Module *mod) {
    bindings.emplace_back(name, getParamType(type, mod));
  }
  llvm::APInt getLiteral() const { return literal; }
  [[nodiscard]] decision_node *getChild() const { return child; }
};

class switch_node : public decision_node {
private:
  /* the list of switch cases */
  std::vector<decision_case> cases;
  /* the name of the variable being matched on. */
  std::string name;
  llvm::Type *type;

  bool isCheckNull;

  switch_node(std::string name, llvm::Type *type, bool is_check_null)
      : name(std::move(name))
      , type(type)
      , isCheckNull(is_check_null) { }

public:
  ~switch_node() override = default;
  void addCase(decision_case const &_case) { cases.push_back(_case); }

  static switch_node *
  Create(std::string const &name, llvm::Type *type, bool is_check_null) {
    return new switch_node(name, type, is_check_null);
  }

  [[nodiscard]] std::string getName() const { return name; }
  [[nodiscard]] llvm::Type *getType() const { return type; }
  [[nodiscard]] std::vector<decision_case> const &getCases() const {
    return cases;
  }

  void codegen(decision *d) override;
  void preprocess(std::unordered_set<leaf_node *> &leaves) override {
    if (preprocessed) {
      return;
    }
    bool hasDefault = false;
    for (auto const &_case : cases) {
      _case.getChild()->preprocess(leaves);
      containsfail_node = containsfail_node || _case.getChild()->containsfail_node;
      hasDefault = hasDefault || _case.getConstructor() == nullptr;
      choiceDepth = std::max(choiceDepth, _case.getChild()->choiceDepth);
    }
    if (!hasDefault) {
      containsfail_node = true;
    }
    preprocessed = true;
  }
};

class make_pattern_node : public decision_node {
private:
  std::string name;
  llvm::Type *type;
  kore_pattern *pattern;
  std::vector<var_type> uses;
  decision_node *child;

  make_pattern_node(
      std::string name, llvm::Type *type, kore_pattern *pattern,
      std::vector<var_type> &uses, decision_node *child)
      : name(std::move(name))
      , type(type)
      , pattern(pattern)
      , uses(uses)
      , child(child) { }

public:
  ~make_pattern_node() override = default;
  static make_pattern_node *Create(
      std::string const &name, llvm::Type *type, kore_pattern *pattern,
      std::vector<var_type> &uses, decision_node *child) {
    return new make_pattern_node(name, type, pattern, uses, child);
  }

  void codegen(decision *d) override;
  void preprocess(std::unordered_set<leaf_node *> &leaves) override {
    if (preprocessed) {
      return;
    }
    child->preprocess(leaves);
    containsfail_node = containsfail_node || child->containsfail_node;
    choiceDepth = child->choiceDepth;
    preprocessed = true;
  }
};

class function_node : public decision_node {
private:
  /* the list of arguments to the function. */
  std::vector<std::pair<var_type, value_type>> bindings;
  /* the name of the variable to bind to the result of the function. */
  std::string name;
  /* the name of the function to call */
  std::string function;
  /* the successor node in the tree */
  decision_node *child;
  value_type cat;
  llvm::Type *type;

  function_node(
      std::string name, std::string function, decision_node *child,
      value_type cat, llvm::Type *type)
      : name(std::move(name))
      , function(std::move(function))
      , child(child)
      , cat(cat)
      , type(type) { }

public:
  ~function_node() override = default;
  static function_node *Create(
      std::string const &name, std::string const &function, decision_node *child,
      value_type cat, llvm::Type *type) {
    return new function_node(name, function, child, cat, type);
  }

  [[nodiscard]] std::vector<std::pair<var_type, value_type>> const &
  getBindings() const {
    return bindings;
  }
  void addBinding(std::string const &name, value_type type, llvm::Module *mod) {
    bindings.push_back({{name, getParamType(type, mod)}, type});
  }

  void codegen(decision *d) override;
  void preprocess(std::unordered_set<leaf_node *> &leaves) override {
    if (preprocessed) {
      return;
    }
    child->preprocess(leaves);
    containsfail_node = containsfail_node || child->containsfail_node;
    choiceDepth = child->choiceDepth;
    preprocessed = true;
  }
};

class leaf_node : public decision_node {
private:
  /* the names in the decision tree of the variables used in the rhs of
     this rule, in alphabetical order of their names in the rule. */
  std::vector<var_type> bindings;
  /* the name of the function that constructs the rhs of this rule from
     the substitution */
  std::string name;

  decision_node *child = nullptr;

  leaf_node(std::string name)
      : name(std::move(name)) { }

public:
  ~leaf_node() override = default;
  static leaf_node *Create(std::string const &name) {
    return new leaf_node(name);
  }

  [[nodiscard]] std::vector<var_type> const &getBindings() const {
    return bindings;
  }
  void addBinding(std::string const &name, value_type type, llvm::Module *mod) {
    bindings.emplace_back(name, getParamType(type, mod));
  }
  void setChild(decision_node *child) { this->child = child; }

  void codegen(decision *d) override;
  void preprocess(std::unordered_set<leaf_node *> &leaves) override {
    if (child != nullptr) {
      if (preprocessed) {
        return;
      }
      leaves.insert(this);
      child->preprocess(leaves);
      containsfail_node = containsfail_node || child->containsfail_node;
      choiceDepth = child->choiceDepth;
      preprocessed = true;
    } else {
      leaves.insert(this);
    }
  }
};

class make_iterator_node : public decision_node {
private:
  std::string collection;
  llvm::Type *collectionType;
  std::string name;
  llvm::Type *type;
  std::string hookName;
  decision_node *child;

  make_iterator_node(
      std::string collection, llvm::Type *collection_type, std::string name,
      llvm::Type *type, std::string hook_name, decision_node *child)
      : collection(std::move(collection))
      , collectionType(collection_type)
      , name(std::move(name))
      , type(type)
      , hookName(std::move(hook_name))
      , child(child) { }

public:
  ~make_iterator_node() override = default;
  static make_iterator_node *Create(
      std::string const &collection, llvm::Type *collection_type,
      std::string const &name, llvm::Type *type, std::string const &hook_name,
      decision_node *child) {
    return new make_iterator_node(
        collection, collection_type, name, type, hook_name, child);
  }

  void codegen(decision *d) override;
  void preprocess(std::unordered_set<leaf_node *> &leaves) override {
    if (preprocessed) {
      return;
    }
    child->preprocess(leaves);
    containsfail_node = containsfail_node || child->containsfail_node;
    choiceDepth = child->choiceDepth;
    preprocessed = true;
  }
};

class iter_next_node : public decision_node {
private:
  std::string iterator;
  llvm::Type *iteratorType;
  std::string binding;
  llvm::Type *bindingType;
  std::string hookName;
  decision_node *child;

  iter_next_node(
      std::string iterator, llvm::Type *iterator_type, std::string binding,
      llvm::Type *binding_type, std::string hook_name, decision_node *child)
      : iterator(std::move(iterator))
      , iteratorType(iterator_type)
      , binding(std::move(binding))
      , bindingType(binding_type)
      , hookName(std::move(hook_name))
      , child(child) { }

public:
  ~iter_next_node() override = default;
  static iter_next_node *Create(
      std::string const &iterator, llvm::Type *iterator_type,
      std::string const &binding, llvm::Type *binding_type,
      std::string const &hook_name, decision_node *child) {
    return new iter_next_node(
        iterator, iterator_type, binding, binding_type, hook_name, child);
  }

  void codegen(decision *d) override;
  void preprocess(std::unordered_set<leaf_node *> &leaves) override {
    if (preprocessed) {
      return;
    }
    child->preprocess(leaves);
    containsfail_node = containsfail_node || child->containsfail_node;
    choiceDepth = child->choiceDepth + 1;
    preprocessed = true;
  }
};

class decision {
private:
  kore_definition *Definition;
  llvm::BasicBlock *CurrentBlock;
  llvm::BasicBlock *FailureBlock;
  llvm::IndirectBrInst *FailJump;
  llvm::AllocaInst *ChoiceBuffer;
  llvm::AllocaInst *ChoiceDepth;
  llvm::BasicBlock *ChoiceBlock{nullptr};
  llvm::Module *Module;
  llvm::LLVMContext &Ctx;
  value_type Cat;
  llvm::PHINode *FailSubject, *FailPattern, *FailSort;
  llvm::AllocaInst *HasSearchResults;

  std::map<var_type, llvm::AllocaInst *> symbols{};

  llvm::Value *getTag(llvm::Value *);

  llvm::AllocaInst *decl(var_type const &name);

  llvm::Constant *stringLiteral(std::string const &str);
  llvm::Value *ptrTerm(llvm::Value *val);

public:
  decision(
      kore_definition *definition, llvm::BasicBlock *entry_block,
      llvm::BasicBlock *failure_block, llvm::IndirectBrInst *fail_jump,
      llvm::AllocaInst *choice_buffer, llvm::AllocaInst *choice_depth,
      llvm::Module *module, value_type cat, llvm::PHINode *fail_subject,
      llvm::PHINode *fail_pattern, llvm::PHINode *fail_sort,
      llvm::AllocaInst *has_search_results)
      : Definition(definition)
      , CurrentBlock(entry_block)
      , FailureBlock(failure_block)
      , FailJump(fail_jump)
      , ChoiceBuffer(choice_buffer)
      , ChoiceDepth(choice_depth)
      , Module(module)
      , Ctx(module->getContext())
      , Cat(cat)
      , FailSubject(fail_subject)
      , FailPattern(fail_pattern)
      , FailSort(fail_sort)
      , HasSearchResults(has_search_results) { }

  /* adds code to the specified basic block to take a single step based on
     the specified decision tree and return the result of taking that step. */
  void operator()(decision_node *entry);
  void store(var_type const &name, llvm::Value *val);
  llvm::Value *load(var_type const &name);

  friend class switch_node;
  friend class make_pattern_node;
  friend class function_node;
  friend class leaf_node;
  friend class make_iterator_node;
  friend class iter_next_node;
  friend class decision_node;
};

/* construct the function that evaluates the specified function symbol
   according to the specified decision tree and returns the result of the
   function. */
void makeEvalFunction(
    kore_symbol *function, kore_definition *definition, llvm::Module *module,
    decision_node *dt);
void makeAnywhereFunction(
    kore_symbol *function, kore_definition *definition, llvm::Module *module,
    decision_node *dt);

void makeStepFunction(
    kore_definition *definition, llvm::Module *module, decision_node *dt,
    bool search);
void makeStepFunction(
    kore_axiom_declaration *axiom, kore_definition *definition,
    llvm::Module *module, partial_step res);
void makeMatchReasonFunction(
    kore_definition *definition, llvm::Module *module,
    kore_axiom_declaration *axiom, decision_node *dt);

} // namespace kllvm
#endif // DECISION_H
