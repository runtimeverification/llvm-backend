#ifndef PROOF_EVENT_H
#define PROOF_EVENT_H

#include "kllvm/ast/AST.h"
#include "kllvm/codegen/Decision.h"
#include "kllvm/codegen/DecisionParser.h"
#include "kllvm/codegen/Util.h"

#include "llvm/IR/Instructions.h"

#include <map>
#include <tuple>

namespace kllvm {

class proof_event {
private:
  kore_definition *definition_;
  llvm::Module *module_;
  llvm::LLVMContext &ctx_;

  /*
   * Load the boolean flag that controls whether proof hint output is enabled or
   * not, then create a branch at the end of this basic block depending on the
   * result.
   *
   * Returns a pair of blocks [proof enabled, merge]; the first of these is
   * intended for self-contained behaviour only relevant in proof output mode,
   * while the second is for the continuation of the interpreter's previous
   * behaviour.
   */
  std::pair<llvm::BasicBlock *, llvm::BasicBlock *>
  proof_branch(std::string const &label, llvm::BasicBlock *insert_at_end);

  /*
   * Set up a standard event prelude by creating a pair of basic blocks for the
   * proof output and continuation, then loading the output filename from its
   * global.
   *
   * Returns a triple [proof enabled, merge, proof_writer]; see `proofBranch`
   * and `emitGetOutputFileName`.
   */
  std::tuple<llvm::BasicBlock *, llvm::BasicBlock *, llvm::Value *>
  event_prelude(std::string const &label, llvm::BasicBlock *insert_at_end);

  /*
   * Emit an instruction that has no effect and will be removed by optimization
   * passes.
   *
   * We need this workaround because some callsites will try to use
   * llvm::Instruction::insertAfter on the back of the MergeBlock after a proof
   * branch is created. If the MergeBlock has no instructions, this has resulted
   * in a segfault when printing the IR. Adding an effective no-op prevents this.
   */
  llvm::BinaryOperator *emit_no_op(llvm::BasicBlock *insert_at_end);

  /*
   * Emit instructions to get a pointer to the interpreter's proof_trace_writer;
   * the data structure that outputs proof trace data.
   */
  llvm::LoadInst *emit_get_proof_trace_writer(llvm::BasicBlock *insert_at_end);

  /*
   * Get the block header value for the given `sort_name`.
   */
  uint64_t get_block_header(std::string const &sort_name);

  /*
   * Emit a call to the `hook_event_pre` API of the specified `proof_writer`.
   */
  llvm::CallInst *emit_write_hook_event_pre(
      llvm::Value *proof_writer, std::string const &name,
      std::string const &pattern, std::string const &location_stack,
      llvm::BasicBlock *insert_at_end);

  /*
   * Emit a call to the `hook_event_post` API of the specified `proof_writer`.
   */
  llvm::CallInst *emit_write_hook_event_post(
      llvm::Value *proof_writer, llvm::Value *val, kore_composite_sort &sort,
      llvm::BasicBlock *insert_at_end);

  /*
   * Emit a call to the `argument` API of the specified `proof_writer`.
   */
  llvm::CallInst *emit_write_argument(
      llvm::Value *proof_writer, llvm::Value *val, kore_composite_sort &sort,
      llvm::BasicBlock *insert_at_end);

  /*
   * Emit a call to the `rewrite_event_pre` API of the specified `proof_writer`.
   */
  llvm::CallInst *emit_write_rewrite_event_pre(
      llvm::Value *proof_writer, uint64_t ordinal, uint64_t arity,
      llvm::BasicBlock *insert_at_end);

  /*
   * Emit a call to the `variable` API of the specified `proof_writer`.
   */
  llvm::CallInst *emit_write_variable(
      llvm::Value *proof_writer, std::string const &name, llvm::Value *val,
      kore_composite_sort &sort, llvm::BasicBlock *insert_at_end);

  /*
   * Emit a call to the `rewrite_event_post` API of the specified `proof_writer`.
   */
  llvm::CallInst *emit_write_rewrite_event_post(
      llvm::Value *proof_writer, llvm::Value *val, kore_composite_sort &sort,
      llvm::BasicBlock *insert_at_end);

  /*
   * Emit a call to the `function_event_pre` API of the specified `proof_writer`.
   */
  llvm::CallInst *emit_write_function_event_pre(
      llvm::Value *proof_writer, std::string const &name,
      std::string const &location_stack, llvm::BasicBlock *insert_at_end);

  /*
   * Emit a call to the `function_event_post` API of the specified `proof_writer`.
   */
  llvm::CallInst *emit_write_function_event_post(
      llvm::Value *proof_writer, llvm::BasicBlock *insert_at_end);

  /*
   * Emit a call to the `side_condition_event_pre` API of the specified `proof_writer`.
   */
  llvm::CallInst *emit_write_side_condition_event_pre(
      llvm::Value *proof_writer, uint64_t ordinal, uint64_t arity,
      llvm::BasicBlock *insert_at_end);

  /*
   * Emit a call to the `side_condition_event_post` API of the specified `proof_writer`.
   */
  llvm::CallInst *emit_write_side_condition_event_post(
      llvm::Value *proof_writer, uint64_t ordinal, llvm::Value *val,
      llvm::BasicBlock *insert_at_end);

public:
  [[nodiscard]] llvm::BasicBlock *hook_event_pre(
      std::string const &name, kore_composite_pattern *pattern,
      llvm::BasicBlock *current_block, std::string const &location_stack);

  [[nodiscard]] llvm::BasicBlock *hook_event_post(
      llvm::Value *val, kore_composite_sort *sort,
      llvm::BasicBlock *current_block);

  [[nodiscard]] llvm::BasicBlock *argument(
      llvm::Value *val, kore_composite_sort *sort, bool is_hook_arg,
      llvm::BasicBlock *current_block);

  [[nodiscard]] llvm::BasicBlock *rewrite_event_pre(
      kore_axiom_declaration const &axiom, uint64_t arity,
      std::map<std::string, kore_variable_pattern *> vars,
      llvm::StringMap<llvm::Value *> const &subst,
      llvm::BasicBlock *current_block);

  [[nodiscard]] llvm::BasicBlock *rewrite_event_post(
      kore_axiom_declaration *axiom, llvm::Value *return_value,
      llvm::BasicBlock *current_block);

  [[nodiscard]] llvm::BasicBlock *function_event_pre(
      llvm::BasicBlock *current_block, kore_composite_pattern *pattern,
      std::string const &location_stack);

  [[nodiscard]] llvm::BasicBlock *
  function_event_post(llvm::BasicBlock *current_block);

  [[nodiscard]] llvm::BasicBlock *side_condition_event_pre(
      kore_axiom_declaration const &axiom,
      std::vector<llvm::Value *> const &args, llvm::BasicBlock *current_block);

  [[nodiscard]] llvm::BasicBlock *side_condition_event_post(
      kore_axiom_declaration const &axiom, llvm::Value *check_result,
      llvm::BasicBlock *current_block);

  [[nodiscard]] llvm::BasicBlock *pattern_matching_failure(
      kore_composite_pattern const &pattern, llvm::BasicBlock *current_block);

  proof_event(kore_definition *definition, llvm::Module *module)
      : definition_(definition)
      , module_(module)
      , ctx_(module->getContext()) { }
};

} // namespace kllvm

#endif // PROOF_EVENT_H
