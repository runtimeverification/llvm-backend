#ifndef CREATE_TERM_H
#define CREATE_TERM_H

#include "kllvm/ast/AST.h"
#include "kllvm/codegen/DecisionParser.h"

#include "llvm/ADT/StringMap.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Value.h"

namespace kllvm {

class create_term {
private:
  llvm::StringMap<llvm::Value *> &substitution_;
  kore_definition *definition_;
  llvm::BasicBlock *current_block_;
  llvm::Module *module_;
  llvm::LLVMContext &ctx_;
  bool is_anywhere_owise_;
  std::set<kore_pattern *> static_terms_;

  llvm::Value *alloc_arg(
      kore_composite_pattern *pattern, int idx,
      std::string const &location_stack);
  llvm::Value *create_hook(
      kore_composite_pattern *hook_att, kore_composite_pattern *pattern,
      std::string const &location_stack = "0");
  llvm::Value *create_function_call(
      std::string const &name, kore_composite_pattern *pattern, bool sret,
      bool tailcc, bool is_hook, std::string const &location_stack = "0");
  llvm::Value *not_injection_case(
      kore_composite_pattern *constructor, llvm::Value *val,
      std::string const &location_stack = "0");
  bool populate_static_set(kore_pattern *pattern);
  std::pair<llvm::Value *, bool> create_allocation(
      kore_pattern *pattern, std::string const &location_stack = "0");

public:
  create_term(
      llvm::StringMap<llvm::Value *> &substitution, kore_definition *definition,
      llvm::BasicBlock *entry_block, llvm::Module *module,
      bool is_anywhere_owise)
      : substitution_(substitution)
      , definition_(definition)
      , current_block_(entry_block)
      , module_(module)
      , ctx_(module->getContext())
      , is_anywhere_owise_(is_anywhere_owise) { }

  /* adds code to the specified basic block in the specified module which
     constructs an llvm value corresponding to the specified KORE RHS pattern
     and substitution in the
     specified definition, and returns the value itself, along with a boolean
     indicating whether the resulting term could be an injection. */
  std::pair<llvm::Value *, bool> operator()(kore_pattern *pattern);

  /* creates a call instruction calling a particular LLVM function, abstracting
     certain ABI and calling convention details:
      - name: the name of the function to call
      - returnCat: the value category of the return type of the function
      - args: the arguments to pass to the function
      - sret: if true, this is a function that returns a struct constant via the
              C ABI (that is, the function actually returns void and the return value
              is via a pointer). Note that this can be set to true even if the
              function does not return a struct, in which case its value is
              ignored.
      - load: if the function returns a struct via sret and load is true,
              we load the value on return.
      - tailcc: true if the call should be made via the tailcc calling convention.
    */
  llvm::Value *create_function_call(
      std::string const &name, value_type return_cat,
      std::vector<llvm::Value *> const &args, bool sret, bool tailcc,
      std::string const &location_stack = "0");

  [[nodiscard]] llvm::BasicBlock *get_current_block() const {
    return current_block_;
  }
};

std::string escape(std::string const &str);

/* Creates a new llvm::Module with the predefined declarations common to all
   llvm modules in the llvm backend. */
std::unique_ptr<llvm::Module>
new_module(std::string const &name, llvm::LLVMContext &context);

llvm::StructType *get_block_type(
    llvm::Module *module, kore_definition *definition,
    kore_symbol const *symbol);
uint64_t get_block_header_val(
    llvm::Module *module, kore_symbol const *symbol, llvm::Type *block_type);
llvm::Value *get_block_header(
    llvm::Module *module, kore_definition *definition,
    kore_symbol const *symbol, llvm::Type *block_type);

/* returns the llvm::Type corresponding to the type of the result of calling
   createTerm on the specified pattern. */
value_type term_type(
    kore_pattern *pattern, llvm::StringMap<value_type> &substitution,
    kore_definition *definition);

/** creates a function that applies the specified rule once it has matched, and
 * returns the name of the function. */
void make_apply_rule_function(
    kore_axiom_declaration *axiom, kore_definition *definition,
    llvm::Module *module, bool big_step = false);
std::string make_apply_rule_function(
    kore_axiom_declaration *axiom, kore_definition *definition,
    llvm::Module *module, std::vector<residual> const &residuals);
/** creates a function that evaluates the side condition of the specified rule,
 * and returns the name of the function. Returns empty string if function has no
 * side condition. */
std::string make_side_condition_function(
    kore_axiom_declaration *axiom, kore_definition *definition,
    llvm::Module *module);

extern std::string map_struct;
extern std::string list_struct;
extern std::string set_struct;
extern std::string int_wrapper_struct;
extern std::string int_struct;
extern std::string float_wrapper_struct;
extern std::string float_struct;
extern std::string buffer_struct;
extern std::string block_struct;
extern std::string blockheader_struct;

llvm::StructType *get_block_type(llvm::Module *module);

llvm::Type *get_arg_type(value_type cat, llvm::Module *mod);

/* returns the llvm::Type corresponding to the specified KORE sort category */
llvm::Type *getvalue_type(value_type sort, llvm::Module *module);
llvm::Type *get_param_type(value_type sort, llvm::Module *module);

bool is_collection_sort(value_type cat);
bool is_injection_symbol(kore_pattern *p, kore_symbol *sym);

void add_abort(llvm::BasicBlock *block, llvm::Module *module);

llvm::Value *allocate_term(
    llvm::Type *alloc_type, llvm::BasicBlock *block,
    char const *alloc_fn = "kore_alloc");
llvm::Value *allocate_term(
    llvm::Type *alloc_type, llvm::Value *len, llvm::BasicBlock *block,
    char const *alloc_fn = "kore_alloc");
} // namespace kllvm

#endif // CREATE_TERM_H
