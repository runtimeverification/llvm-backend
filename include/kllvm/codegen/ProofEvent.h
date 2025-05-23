#ifndef PROOF_EVENT_H
#define PROOF_EVENT_H

#include "kllvm/ast/AST.h"
#include "kllvm/codegen/Decision.h"
#include "kllvm/codegen/DecisionParser.h"
#include "kllvm/codegen/Options.h"
#include "kllvm/codegen/Util.h"

#include "llvm/IR/Instructions.h"

#include <fmt/format.h>

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
   * not, then create a branch at the specified location depending on the
   * result. The location can be before a given instruction or at the end of a
   * given basic block.
   *
   * Returns a pair of blocks [proof enabled, merge]; the first of these is
   * intended for self-contained behaviour only relevant in proof output mode,
   * while the second is for the continuation of the interpreter's previous
   * behaviour.
   */
  template <typename Location>
  std::pair<llvm::BasicBlock *, llvm::BasicBlock *>
  proof_branch(std::string const &label, Location *insert_loc);

  /*
   * Return the parent function of the given location.

   * Template specializations for llvm::Instruction and llvm::BasicBlock.
   */
  template <typename Location>
  llvm::Function *get_parent_function(Location *loc);

  /*
   * Return the parent basic block of the given location.

   * Template specializations for llvm::Instruction and llvm::BasicBlock.
   */
  template <typename Location>
  llvm::BasicBlock *get_parent_block(Location *loc);

  /*
   * If the given location is an Instruction, this method moves the instruction
   * to the merge block.
   * If the given location is a BasicBlock, this method simply emits a no-op
   * instruction to the merge block.

   * Template specializations for llvm::Instruction and llvm::BasicBlock.
   */
  template <typename Location>
  void fix_insert_loc(Location *loc, llvm::BasicBlock *merge_block);

  /*
   * Set up a standard event prelude by creating a pair of basic blocks for the
   * proof output and continuation, then loading the output filename from its
   * global. The location for the prelude  can be before a given instruction or
   * at the end of a given basic block.
   *
   * Returns a triple [proof enabled, merge, proof_writer]; see `proofBranch`
   * and `emitGetOutputFileName`.
   */
  template <typename Location>
  std::tuple<llvm::BasicBlock *, llvm::BasicBlock *, llvm::Value *>
  event_prelude(std::string const &label, Location *insert_loc);

  /*
   * Set up a check of whether a new proof hint chunk should be started. The
   * condition for that is
   *   proof_chunk_size != 0 and steps % proof_chunk_size = 0
   * Returns a block intended for adding code that starts a new chunk. If the
   * condition is false, we branch to the given merge_block.
   */
  llvm::BasicBlock *check_for_emit_new_chunk(
      llvm::BasicBlock *insert_at_end, llvm::BasicBlock *merge_block);

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
   * Emit instructions to get the current value of the steps global variable,
   * which counts the number of rewrite steps taken.
   */
  llvm::LoadInst *emit_get_steps(llvm::BasicBlock *insert_at_end);

  /*
   * Emit instructions to get the current value of the proof_chunk_size global
   * variable, which dictates how many rewrite steps should be included per
   * chunk of the hint trace.
   */
  llvm::LoadInst *emit_get_proof_chunk_size(llvm::BasicBlock *insert_at_end);

  /*
   * Check if a value of the given `sort` corresponds to an llvm scalar and
   * return the size in bits of that scalar. Returns 0 if the given `sort` does
   * not correspond to an llvm scalar.
   */
  uint64_t get_llvm_scalar_bits(kore_composite_sort &sort);

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

  /*
   * Emit a call to the `pattern_matching_failure` API of the specified `proof_writer`.
   */
  llvm::CallInst *emit_write_pattern_matching_failure(
      llvm::Value *proof_writer, std::string const &function_name,
      llvm::BasicBlock *insert_at_end);

  /*
   * Emit a call to the `function_exit` API of the specified `proof_writer`.
   */
  llvm::CallInst *emit_write_function_exit(
      llvm::Value *proof_writer, uint64_t ordinal, bool is_tail,
      llvm::BasicBlock *insert_at_end);

  /*
   * Emit a call to the `start_new_chunk` API of the specified `proof_writer`.
   */
  llvm::CallInst *emit_start_new_chunk(
      llvm::Value *proof_writer, llvm::BasicBlock *insert_at_end);

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

  [[nodiscard]] llvm::BasicBlock *short_circuit_hook_argument(
      llvm::Value *val, llvm::Value *short_circuit_cond, bool invert_cond,
      kore_composite_sort *sort, llvm::BasicBlock *current_block);

  [[nodiscard]] llvm::BasicBlock *short_circuit_hook_argument(
      llvm::Value *val_first, llvm::Value *val_second, llvm::Value *select_cond,
      kore_composite_sort *sort_first, kore_composite_sort *sort_second,
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

  template <typename Location>
  [[nodiscard]] llvm::BasicBlock *
  function_exit(uint64_t ordinal, bool is_tail, Location *insert_loc);

  proof_event(kore_definition *definition, llvm::Module *module)
      : definition_(definition)
      , module_(module)
      , ctx_(module->getContext()) { }
};

} // namespace kllvm

//===----------------------------------------------------------------------===//
// Implementation for method templates
//===----------------------------------------------------------------------===//

template <typename Location>
std::pair<llvm::BasicBlock *, llvm::BasicBlock *>
kllvm::proof_event::proof_branch(
    std::string const &label, Location *insert_loc) {
  auto *i1_ty = llvm::Type::getInt1Ty(ctx_);

  auto *proof_output_flag = module_->getOrInsertGlobal("proof_output", i1_ty);
  auto *proof_output = new llvm::LoadInst(
      i1_ty, proof_output_flag, "proof_output", insert_loc);

  auto *f = get_parent_function(insert_loc);
  auto *true_block
      = llvm::BasicBlock::Create(ctx_, fmt::format("if_{}", label), f);
  auto *merge_block
      = llvm::BasicBlock::Create(ctx_, fmt::format("tail_{}", label), f);

  llvm::BranchInst::Create(true_block, merge_block, proof_output, insert_loc);

  fix_insert_loc(insert_loc, merge_block);

  return {true_block, merge_block};
}

template <typename Location>
std::tuple<llvm::BasicBlock *, llvm::BasicBlock *, llvm::Value *>
kllvm::proof_event::event_prelude(
    std::string const &label, Location *insert_loc) {
  auto [true_block, merge_block] = proof_branch(label, insert_loc);
  return {true_block, merge_block, emit_get_proof_trace_writer(true_block)};
}

template <typename Location>
llvm::BasicBlock *kllvm::proof_event::function_exit(
    uint64_t ordinal, bool is_tail, Location *insert_loc) {

  if (!proof_hint_instrumentation) {
    return get_parent_block(insert_loc);
  }

  auto [true_block, merge_block, proof_writer]
      = event_prelude("function_exit", insert_loc);

  emit_write_function_exit(proof_writer, ordinal, is_tail, true_block);

  llvm::BranchInst::Create(merge_block, true_block);

  return merge_block;
}

#endif // PROOF_EVENT_H
