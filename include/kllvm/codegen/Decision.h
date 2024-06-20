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
using var_set_type = std::unordered_map<
    var_type, std::unordered_set<iter_next_node *>, hash_var>;

class decision_node {
private:
  llvm::BasicBlock *cached_code_ = nullptr;
  /* completed tracks whether codegen for this decision_node has concluded */
  bool completed_ = false;

public:
  virtual ~decision_node() = default;

  virtual void codegen(decision *d) = 0;
  virtual void preprocess(std::unordered_set<leaf_node *> &) = 0;
  bool begin_node(decision *d, std::string const &name);

  void set_completed() { completed_ = true; }
  [[nodiscard]] bool is_completed() const { return completed_; }
  [[nodiscard]] uint64_t get_choice_depth() const { return choice_depth_; }

private:
  bool preprocessed_ = false, containsfail_node_ = false;
  uint64_t choice_depth_ = 0;
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
    containsfail_node_ = true;
  }
};

class decision_case {
private:
  /* constructor to switch on. if null, this is a wildcard match.
     if equal to \\dv, we are matching on a bool or mint literal. */
  kore_symbol *constructor_;
  /* the names to bind the children of this pattern to. */
  std::vector<var_type> bindings_;
  /* the literal int to match on. must have a bit width equal to the
     size of the sort being matched. */
  llvm::APInt literal_;
  /* the node in the tree to jump to if this constructor is matched */
  decision_node *child_;

public:
  decision_case(
      kore_symbol *constructor, std::vector<var_type> bindings,
      decision_node *child)
      : constructor_(constructor)
      , bindings_(std::move(std::move(bindings)))
      , child_(child) { }
  decision_case(kore_symbol *dv, llvm::APInt literal, decision_node *child)
      : constructor_(dv)
      , literal_(std::move(std::move(literal)))
      , child_(child) { }

  [[nodiscard]] kore_symbol *get_constructor() const { return constructor_; }
  [[nodiscard]] std::vector<var_type> const &get_bindings() const {
    return bindings_;
  }
  void
  add_binding(std::string const &name, value_type type, llvm::Module *mod) {
    bindings_.emplace_back(name, get_param_type(type, mod));
  }
  llvm::APInt get_literal() const { return literal_; }
  [[nodiscard]] decision_node *get_child() const { return child_; }
};

class switch_node : public decision_node {
private:
  /* the list of switch cases */
  std::vector<decision_case> cases_;
  /* the name of the variable being matched on. */
  std::string name_;
  llvm::Type *type_;

  bool is_check_null_;

  switch_node(std::string name, llvm::Type *type, bool is_check_null)
      : name_(std::move(name))
      , type_(type)
      , is_check_null_(is_check_null) { }

public:
  ~switch_node() override = default;
  void add_case(decision_case const &c) { cases_.push_back(c); }

  static switch_node *
  create(std::string const &name, llvm::Type *type, bool is_check_null) {
    return new switch_node(name, type, is_check_null);
  }

  [[nodiscard]] std::string get_name() const { return name_; }
  [[nodiscard]] llvm::Type *get_type() const { return type_; }
  [[nodiscard]] std::vector<decision_case> const &get_cases() const {
    return cases_;
  }

  void codegen(decision *d) override;
  void preprocess(std::unordered_set<leaf_node *> &leaves) override {
    if (preprocessed_) {
      return;
    }
    bool has_default = false;
    for (auto const &c : cases_) {
      c.get_child()->preprocess(leaves);
      containsfail_node_
          = containsfail_node_ || c.get_child()->containsfail_node_;
      has_default = has_default || c.get_constructor() == nullptr;
      choice_depth_ = std::max(choice_depth_, c.get_child()->choice_depth_);
    }
    if (!has_default) {
      containsfail_node_ = true;
    }
    preprocessed_ = true;
  }
};

class make_pattern_node : public decision_node {
private:
  std::string name_;
  llvm::Type *type_;
  kore_pattern *pattern_;
  std::vector<var_type> uses_;
  decision_node *child_;

  make_pattern_node(
      std::string name, llvm::Type *type, kore_pattern *pattern,
      std::vector<var_type> &uses, decision_node *child)
      : name_(std::move(name))
      , type_(type)
      , pattern_(pattern)
      , uses_(uses)
      , child_(child) { }

public:
  ~make_pattern_node() override = default;
  static make_pattern_node *create(
      std::string const &name, llvm::Type *type, kore_pattern *pattern,
      std::vector<var_type> &uses, decision_node *child) {
    return new make_pattern_node(name, type, pattern, uses, child);
  }

  void codegen(decision *d) override;
  void preprocess(std::unordered_set<leaf_node *> &leaves) override {
    if (preprocessed_) {
      return;
    }
    child_->preprocess(leaves);
    containsfail_node_ = containsfail_node_ || child_->containsfail_node_;
    choice_depth_ = child_->choice_depth_;
    preprocessed_ = true;
  }
};

class function_node : public decision_node {
private:
  /* the list of arguments to the function. */
  std::vector<std::pair<var_type, value_type>> bindings_;
  /* the name of the variable to bind to the result of the function. */
  std::string name_;
  /* the name of the function to call */
  std::string function_;
  /* the successor node in the tree */
  decision_node *child_;
  value_type cat_;
  llvm::Type *type_;

  function_node(
      std::string name, std::string function, decision_node *child,
      value_type cat, llvm::Type *type)
      : name_(std::move(name))
      , function_(std::move(function))
      , child_(child)
      , cat_(cat)
      , type_(type) { }

public:
  ~function_node() override = default;
  static function_node *create(
      std::string const &name, std::string const &function,
      decision_node *child, value_type cat, llvm::Type *type) {
    return new function_node(name, function, child, cat, type);
  }

  [[nodiscard]] std::vector<std::pair<var_type, value_type>> const &
  get_bindings() const {
    return bindings_;
  }
  void
  add_binding(std::string const &name, value_type type, llvm::Module *mod) {
    bindings_.push_back({{name, get_param_type(type, mod)}, type});
  }

  void codegen(decision *d) override;
  void preprocess(std::unordered_set<leaf_node *> &leaves) override {
    if (preprocessed_) {
      return;
    }
    child_->preprocess(leaves);
    containsfail_node_ = containsfail_node_ || child_->containsfail_node_;
    choice_depth_ = child_->choice_depth_;
    preprocessed_ = true;
  }
};

class leaf_node : public decision_node {
private:
  /* the names in the decision tree of the variables used in the rhs of
     this rule, in alphabetical order of their names in the rule. */
  std::vector<var_type> bindings_;
  /* the name of the function that constructs the rhs of this rule from
     the substitution */
  std::string name_;

  decision_node *child_ = nullptr;

  leaf_node(std::string name)
      : name_(std::move(name)) { }

public:
  ~leaf_node() override = default;
  static leaf_node *create(std::string const &name) {
    return new leaf_node(name);
  }

  [[nodiscard]] std::vector<var_type> const &get_bindings() const {
    return bindings_;
  }
  void
  add_binding(std::string const &name, value_type type, llvm::Module *mod) {
    bindings_.emplace_back(name, get_param_type(type, mod));
  }
  void set_child(decision_node *child) { this->child_ = child; }

  void codegen(decision *d) override;
  void preprocess(std::unordered_set<leaf_node *> &leaves) override {
    if (child_ != nullptr) {
      if (preprocessed_) {
        return;
      }
      leaves.insert(this);
      child_->preprocess(leaves);
      containsfail_node_ = containsfail_node_ || child_->containsfail_node_;
      choice_depth_ = child_->choice_depth_;
      preprocessed_ = true;
    } else {
      leaves.insert(this);
    }
  }
};

class make_iterator_node : public decision_node {
private:
  std::string collection_;
  llvm::Type *collection_type_;
  std::string name_;
  llvm::Type *type_;
  std::string hook_name_;
  decision_node *child_;

  make_iterator_node(
      std::string collection, llvm::Type *collection_type, std::string name,
      llvm::Type *type, std::string hook_name, decision_node *child)
      : collection_(std::move(collection))
      , collection_type_(collection_type)
      , name_(std::move(name))
      , type_(type)
      , hook_name_(std::move(hook_name))
      , child_(child) { }

public:
  ~make_iterator_node() override = default;
  static make_iterator_node *create(
      std::string const &collection, llvm::Type *collection_type,
      std::string const &name, llvm::Type *type, std::string const &hook_name,
      decision_node *child) {
    return new make_iterator_node(
        collection, collection_type, name, type, hook_name, child);
  }

  void codegen(decision *d) override;
  void preprocess(std::unordered_set<leaf_node *> &leaves) override {
    if (preprocessed_) {
      return;
    }
    child_->preprocess(leaves);
    containsfail_node_ = containsfail_node_ || child_->containsfail_node_;
    choice_depth_ = child_->choice_depth_;
    preprocessed_ = true;
  }
};

class iter_next_node : public decision_node {
private:
  std::string iterator_;
  llvm::Type *iterator_type_;
  std::string binding_;
  llvm::Type *binding_type_;
  std::string hook_name_;
  decision_node *child_;

  iter_next_node(
      std::string iterator, llvm::Type *iterator_type, std::string binding,
      llvm::Type *binding_type, std::string hook_name, decision_node *child)
      : iterator_(std::move(iterator))
      , iterator_type_(iterator_type)
      , binding_(std::move(binding))
      , binding_type_(binding_type)
      , hook_name_(std::move(hook_name))
      , child_(child) { }

public:
  ~iter_next_node() override = default;
  static iter_next_node *create(
      std::string const &iterator, llvm::Type *iterator_type,
      std::string const &binding, llvm::Type *binding_type,
      std::string const &hook_name, decision_node *child) {
    return new iter_next_node(
        iterator, iterator_type, binding, binding_type, hook_name, child);
  }

  void codegen(decision *d) override;
  void preprocess(std::unordered_set<leaf_node *> &leaves) override {
    if (preprocessed_) {
      return;
    }
    child_->preprocess(leaves);
    containsfail_node_ = containsfail_node_ || child_->containsfail_node_;
    choice_depth_ = child_->choice_depth_ + 1;
    preprocessed_ = true;
  }
};

class decision {
private:
  kore_definition *definition_;
  llvm::BasicBlock *current_block_;
  llvm::BasicBlock *failure_block_;
  llvm::IndirectBrInst *fail_jump_;
  llvm::AllocaInst *choice_buffer_;
  llvm::AllocaInst *choice_depth_;
  llvm::BasicBlock *choice_block_{nullptr};
  llvm::Module *module_;
  llvm::LLVMContext &ctx_;
  value_type cat_;
  llvm::PHINode *fail_subject_, *fail_pattern_, *fail_sort_;
  llvm::AllocaInst *has_search_results_;
  bool profile_matching_;

  std::map<var_type, llvm::AllocaInst *> symbols_{};

  llvm::Value *get_tag(llvm::Value *);

  llvm::AllocaInst *decl(var_type const &name);

  llvm::Constant *string_literal(std::string const &str);
  llvm::Value *ptr_term(llvm::Value *val);

public:
  decision(
      kore_definition *definition, llvm::BasicBlock *entry_block,
      llvm::BasicBlock *failure_block, llvm::IndirectBrInst *fail_jump,
      llvm::AllocaInst *choice_buffer, llvm::AllocaInst *choice_depth,
      llvm::Module *module, value_type cat, llvm::PHINode *fail_subject,
      llvm::PHINode *fail_pattern, llvm::PHINode *fail_sort,
      llvm::AllocaInst *has_search_results, bool profile_matching = false)
      : definition_(definition)
      , current_block_(entry_block)
      , failure_block_(failure_block)
      , fail_jump_(fail_jump)
      , choice_buffer_(choice_buffer)
      , choice_depth_(choice_depth)
      , module_(module)
      , ctx_(module->getContext())
      , cat_(cat)
      , fail_subject_(fail_subject)
      , fail_pattern_(fail_pattern)
      , fail_sort_(fail_sort)
      , has_search_results_(has_search_results)
      , profile_matching_(profile_matching) { }

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
void make_eval_function(
    kore_symbol *function, kore_definition *definition, llvm::Module *module,
    decision_node *dt);
void make_anywhere_function(
    kore_symbol *function, kore_definition *definition, llvm::Module *module,
    decision_node *dt);

void make_step_function(
    kore_definition *definition, llvm::Module *module, decision_node *dt,
    bool search, bool profile_matching);
void make_step_function(
    kore_axiom_declaration *axiom, kore_definition *definition,
    llvm::Module *module, partial_step res, bool profile_matching);
void make_match_reason_function(
    kore_definition *definition, llvm::Module *module,
    kore_axiom_declaration *axiom, decision_node *dt);

} // namespace kllvm
#endif // DECISION_H
