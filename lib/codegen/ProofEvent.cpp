#include "kllvm/codegen/ProofEvent.h"

#include "kllvm/binary/ProofTraceParser.h"
#include "kllvm/codegen/CreateTerm.h"
#include "kllvm/codegen/Options.h"

#include "llvm/IR/IRBuilder.h"
#include "llvm/Support/CommandLine.h"

#include <fmt/format.h>

namespace kllvm {

/*
 * Internal implementation details
 */

namespace {

template <typename IRBuilder>
llvm::Constant *create_global_sort_string_ptr(
    IRBuilder &b, kore_composite_sort &sort, llvm::Module *mod) {
  return b.CreateGlobalStringPtr(
      ast_to_string(sort), fmt::format("{}_str", sort.get_name()), 0, mod);
}

template <typename IRBuilder>
llvm::Value *get_llvm_value_for_kore_term(
    llvm::Value *val, uint64_t bits, IRBuilder &b, llvm::Module *mod) {
  if (bits <= 64) {
    return val;
  }

  // If the llvm value is larger than 64 bits, we need to pass its address to
  // the runtime functions that emit proof trace events.
  auto *int_ty = llvm::IntegerType::get(mod->getContext(), bits);
  auto *ptr_val = b.CreateAlloca(int_ty);
  b.CreateStore(val, ptr_val);
  return ptr_val;
}

} // namespace

uint64_t proof_event::get_llvm_scalar_bits(kore_composite_sort &sort) {
  value_type sort_category = sort.get_category(definition_);
  switch (sort_category.cat) {
  case sort_category::Bool: return 1;
  case sort_category::MInt: return sort_category.bits;
  default: return 0;
  }
}

uint64_t proof_event::get_block_header(std::string const &sort_name) {
  std::string inj_name;
  if (sort_name == "SortKItem{}") {
    inj_name = "rawTerm{}";
  } else if (sort_name == "SortK{}") {
    inj_name = "rawKTerm{}";
  } else {
    inj_name = "inj{" + sort_name + ", SortKItem{}}";
  }
  auto *symbol = definition_->get_all_symbols().at(inj_name);
  return get_block_header_val(
      module_, symbol, get_block_type(module_, definition_, symbol));
}

llvm::CallInst *proof_event::emit_write_hook_event_pre(
    llvm::Value *proof_writer, std::string const &name,
    std::string const &pattern, std::string const &location_stack,
    llvm::BasicBlock *insert_at_end) {
  auto b = llvm::IRBuilder(insert_at_end);

  auto *void_ty = llvm::Type::getVoidTy(ctx_);
  auto *i8_ptr_ty = llvm::PointerType::getUnqual(ctx_);

  auto *func_ty = llvm::FunctionType::get(
      void_ty, {i8_ptr_ty, i8_ptr_ty, i8_ptr_ty, i8_ptr_ty}, false);

  auto *func = get_or_insert_function(
      module_, "write_hook_event_pre_to_proof_trace", func_ty);

  auto *var_name = b.CreateGlobalStringPtr(name, "", 0, module_);
  auto *var_pattern = b.CreateGlobalStringPtr(pattern, "", 0, module_);
  auto *var_location = b.CreateGlobalStringPtr(location_stack, "", 0, module_);
  return b.CreateCall(
      func, {proof_writer, var_name, var_pattern, var_location});
}

llvm::CallInst *proof_event::emit_write_hook_event_post(
    llvm::Value *proof_writer, llvm::Value *val, kore_composite_sort &sort,
    llvm::BasicBlock *insert_at_end) {
  auto b = llvm::IRBuilder(insert_at_end);

  uint64_t bits = get_llvm_scalar_bits(sort);
  std::string sort_name = ast_to_string(sort);
  uint64_t block_header = get_block_header(sort_name);

  auto *void_ty = llvm::Type::getVoidTy(ctx_);
  auto *i8_ptr_ty = llvm::PointerType::getUnqual(ctx_);
  auto *i64_ty = llvm::Type::getInt64Ty(ctx_);

  auto *func_ty = llvm::FunctionType::get(
      void_ty, {i8_ptr_ty, i8_ptr_ty, i64_ty, i64_ty}, false);

  auto *func = get_or_insert_function(
      module_, "write_hook_event_post_to_proof_trace", func_ty);

  auto *var_val = get_llvm_value_for_kore_term(val, bits, b, module_);
  auto *var_block_header = llvm::ConstantInt::get(i64_ty, block_header);
  auto *var_bits = llvm::ConstantInt::get(i64_ty, bits);
  return b.CreateCall(
      func, {proof_writer, var_val, var_block_header, var_bits});
}

llvm::CallInst *proof_event::emit_write_argument(
    llvm::Value *proof_writer, llvm::Value *val, kore_composite_sort &sort,
    llvm::BasicBlock *insert_at_end) {
  auto b = llvm::IRBuilder(insert_at_end);

  uint64_t bits = get_llvm_scalar_bits(sort);
  std::string sort_name = ast_to_string(sort);
  uint64_t block_header = get_block_header(sort_name);

  auto *void_ty = llvm::Type::getVoidTy(ctx_);
  auto *i8_ptr_ty = llvm::PointerType::getUnqual(ctx_);
  auto *i64_ty = llvm::Type::getInt64Ty(ctx_);

  auto *func_ty = llvm::FunctionType::get(
      void_ty, {i8_ptr_ty, i8_ptr_ty, i64_ty, i64_ty}, false);

  auto *func = get_or_insert_function(
      module_, "write_argument_to_proof_trace", func_ty);

  auto *var_val = get_llvm_value_for_kore_term(val, bits, b, module_);
  auto *var_block_header = llvm::ConstantInt::get(i64_ty, block_header);
  auto *var_bits = llvm::ConstantInt::get(i64_ty, bits);
  return b.CreateCall(
      func, {proof_writer, var_val, var_block_header, var_bits});
}

llvm::CallInst *proof_event::emit_write_rewrite_event_pre(
    llvm::Value *proof_writer, uint64_t ordinal, uint64_t arity,
    llvm::BasicBlock *insert_at_end) {
  auto b = llvm::IRBuilder(insert_at_end);

  auto *void_ty = llvm::Type::getVoidTy(ctx_);
  auto *i8_ptr_ty = llvm::PointerType::getUnqual(ctx_);
  auto *i64_ty = llvm::Type::getInt64Ty(ctx_);

  auto *func_ty
      = llvm::FunctionType::get(void_ty, {i8_ptr_ty, i64_ty, i64_ty}, false);

  auto *func = get_or_insert_function(
      module_, "write_rewrite_event_pre_to_proof_trace", func_ty);

  auto *var_ordinal = llvm::ConstantInt::get(i64_ty, ordinal);
  auto *var_arity = llvm::ConstantInt::get(i64_ty, arity);
  return b.CreateCall(func, {proof_writer, var_ordinal, var_arity});
}

llvm::CallInst *proof_event::emit_write_variable(
    llvm::Value *proof_writer, std::string const &name, llvm::Value *val,
    kore_composite_sort &sort, llvm::BasicBlock *insert_at_end) {
  auto b = llvm::IRBuilder(insert_at_end);

  uint64_t bits = get_llvm_scalar_bits(sort);
  std::string sort_name = ast_to_string(sort);
  uint64_t block_header = get_block_header(sort_name);

  auto *void_ty = llvm::Type::getVoidTy(ctx_);
  auto *i8_ptr_ty = llvm::PointerType::getUnqual(ctx_);
  auto *i64_ty = llvm::Type::getInt64Ty(ctx_);

  auto *func_ty = llvm::FunctionType::get(
      void_ty, {i8_ptr_ty, i8_ptr_ty, i8_ptr_ty, i64_ty, i64_ty}, false);

  auto *func = get_or_insert_function(
      module_, "write_variable_to_proof_trace", func_ty);

  auto *var_name = b.CreateGlobalStringPtr(name, "", 0, module_);
  auto *var_val = get_llvm_value_for_kore_term(val, bits, b, module_);
  auto *var_block_header = llvm::ConstantInt::get(i64_ty, block_header);
  auto *var_bits = llvm::ConstantInt::get(i64_ty, bits);
  return b.CreateCall(
      func, {proof_writer, var_name, var_val, var_block_header, var_bits});
}

llvm::CallInst *proof_event::emit_write_rewrite_event_post(
    llvm::Value *proof_writer, llvm::Value *val, kore_composite_sort &sort,
    llvm::BasicBlock *insert_at_end) {
  auto b = llvm::IRBuilder(insert_at_end);

  uint64_t bits = get_llvm_scalar_bits(sort);
  std::string sort_name = ast_to_string(sort);
  uint64_t block_header = get_block_header(sort_name);

  auto *void_ty = llvm::Type::getVoidTy(ctx_);
  auto *i8_ptr_ty = llvm::PointerType::getUnqual(ctx_);
  auto *i64_ty = llvm::Type::getInt64Ty(ctx_);

  auto *func_ty = llvm::FunctionType::get(
      void_ty, {i8_ptr_ty, i8_ptr_ty, i64_ty, i64_ty}, false);

  auto *func = get_or_insert_function(
      module_, "write_rewrite_event_post_to_proof_trace", func_ty);

  auto *var_val = get_llvm_value_for_kore_term(val, bits, b, module_);
  auto *var_block_header = llvm::ConstantInt::get(i64_ty, block_header);
  auto *var_bits = llvm::ConstantInt::get(i64_ty, bits);
  return b.CreateCall(
      func, {proof_writer, var_val, var_block_header, var_bits});
}

llvm::CallInst *proof_event::emit_write_function_event_pre(
    llvm::Value *proof_writer, std::string const &name,
    std::string const &location_stack, llvm::BasicBlock *insert_at_end) {
  auto b = llvm::IRBuilder(insert_at_end);

  auto *void_ty = llvm::Type::getVoidTy(ctx_);
  auto *i8_ptr_ty = llvm::PointerType::getUnqual(ctx_);

  auto *func_ty = llvm::FunctionType::get(
      void_ty, {i8_ptr_ty, i8_ptr_ty, i8_ptr_ty}, false);

  auto *func = get_or_insert_function(
      module_, "write_function_event_pre_to_proof_trace", func_ty);

  auto *var_name = b.CreateGlobalStringPtr(name, "", 0, module_);
  auto *var_location = b.CreateGlobalStringPtr(location_stack, "", 0, module_);
  return b.CreateCall(func, {proof_writer, var_name, var_location});
}

llvm::CallInst *proof_event::emit_write_function_event_post(
    llvm::Value *proof_writer, llvm::BasicBlock *insert_at_end) {
  auto b = llvm::IRBuilder(insert_at_end);

  auto *void_ty = llvm::Type::getVoidTy(ctx_);
  auto *i8_ptr_ty = llvm::PointerType::getUnqual(ctx_);

  auto *func_ty = llvm::FunctionType::get(void_ty, {i8_ptr_ty}, false);

  auto *func = get_or_insert_function(
      module_, "write_function_event_post_to_proof_trace", func_ty);

  return b.CreateCall(func, {proof_writer});
}

llvm::CallInst *proof_event::emit_write_side_condition_event_pre(
    llvm::Value *proof_writer, uint64_t ordinal, uint64_t arity,
    llvm::BasicBlock *insert_at_end) {
  auto b = llvm::IRBuilder(insert_at_end);

  auto *void_ty = llvm::Type::getVoidTy(ctx_);
  auto *i8_ptr_ty = llvm::PointerType::getUnqual(ctx_);
  auto *i64_ty = llvm::Type::getInt64Ty(ctx_);

  auto *func_ty
      = llvm::FunctionType::get(void_ty, {i8_ptr_ty, i64_ty, i64_ty}, false);

  auto *func = get_or_insert_function(
      module_, "write_side_condition_event_pre_to_proof_trace", func_ty);

  auto *var_ordinal = llvm::ConstantInt::get(i64_ty, ordinal);
  auto *var_arity = llvm::ConstantInt::get(i64_ty, arity);
  return b.CreateCall(func, {proof_writer, var_ordinal, var_arity});
}

llvm::CallInst *proof_event::emit_write_side_condition_event_post(
    llvm::Value *proof_writer, uint64_t ordinal, llvm::Value *val,
    llvm::BasicBlock *insert_at_end) {
  auto b = llvm::IRBuilder(insert_at_end);

  auto *void_ty = llvm::Type::getVoidTy(ctx_);
  auto *i8_ptr_ty = llvm::PointerType::getUnqual(ctx_);
  auto *i64_ty = llvm::Type::getInt64Ty(ctx_);

  auto *func_ty
      = llvm::FunctionType::get(void_ty, {i8_ptr_ty, i64_ty, i8_ptr_ty}, false);

  auto *func = get_or_insert_function(
      module_, "write_side_condition_event_post_to_proof_trace", func_ty);

  auto *var_ordinal = llvm::ConstantInt::get(i64_ty, ordinal);
  auto *var_val = b.CreateIntToPtr(val, i8_ptr_ty);
  return b.CreateCall(func, {proof_writer, var_ordinal, var_val});
}

llvm::CallInst *proof_event::emit_write_pattern_matching_failure(
    llvm::Value *proof_writer, std::string const &function_name,
    llvm::BasicBlock *insert_at_end) {
  auto b = llvm::IRBuilder(insert_at_end);

  auto *void_ty = llvm::Type::getVoidTy(ctx_);
  auto *i8_ptr_ty = llvm::PointerType::getUnqual(ctx_);

  auto *func_ty
      = llvm::FunctionType::get(void_ty, {i8_ptr_ty, i8_ptr_ty}, false);

  auto *func = get_or_insert_function(
      module_, "write_pattern_matching_failure_to_proof_trace", func_ty);

  auto *var_function_name
      = b.CreateGlobalStringPtr(function_name, "", 0, module_);
  return b.CreateCall(func, {proof_writer, var_function_name});
}

llvm::CallInst *proof_event::emit_write_tail_call_info(
    llvm::Value *proof_writer, std::string const &caller_name,
    bool is_tail, llvm::BasicBlock *insert_at_end) {
  auto b = llvm::IRBuilder(insert_at_end);

  auto *void_ty = llvm::Type::getVoidTy(ctx_);
  auto *i8_ptr_ty = llvm::PointerType::getUnqual(ctx_);
  auto *i8_ty = llvm::Type::getInt64Ty(ctx_);

  auto *func_ty
      = llvm::FunctionType::get(void_ty, {i8_ptr_ty, i8_ptr_ty, i8_ty}, false);

  auto *func = get_or_insert_function(
      module_, "write_tail_call_info_to_proof_trace", func_ty);

  auto *var_caller_name
      = b.CreateGlobalStringPtr(caller_name, "", 0, module_);
  auto *var_is_tail = llvm::ConstantInt::get(i8_ty, is_tail);
  return b.CreateCall(func, {proof_writer, var_caller_name, var_is_tail});
}

llvm::CallInst *proof_event::emit_start_new_chunk(
    llvm::Value *proof_writer, llvm::BasicBlock *insert_at_end) {
  auto b = llvm::IRBuilder(insert_at_end);

  auto *void_ty = llvm::Type::getVoidTy(ctx_);
  auto *i8_ptr_ty = llvm::PointerType::getUnqual(ctx_);

  auto *func_ty = llvm::FunctionType::get(void_ty, {i8_ptr_ty}, false);

  auto *func = get_or_insert_function(
      module_, "start_new_chunk_in_proof_trace", func_ty);

  return b.CreateCall(func, {proof_writer});
}

llvm::BinaryOperator *proof_event::emit_no_op(llvm::BasicBlock *insert_at_end) {
  auto *i8_ty = llvm::Type::getInt8Ty(ctx_);
  auto *zero = llvm::ConstantInt::get(i8_ty, 0);

  return llvm::BinaryOperator::Create(
      llvm::Instruction::Add, zero, zero, "no-op", insert_at_end);
}

llvm::LoadInst *
proof_event::emit_get_proof_trace_writer(llvm::BasicBlock *insert_at_end) {
  auto *i8_ptr_ty = llvm::PointerType::getUnqual(ctx_);
  auto *file_name_pointer
      = module_->getOrInsertGlobal("proof_writer", i8_ptr_ty);
  return new llvm::LoadInst(
      i8_ptr_ty, file_name_pointer, "output", insert_at_end);
}

llvm::LoadInst *proof_event::emit_get_steps(llvm::BasicBlock *insert_at_end) {
  auto *i64_ty = llvm::Type::getInt64Ty(ctx_);
  auto *steps_pointer = module_->getOrInsertGlobal("steps", i64_ty);
  return new llvm::LoadInst(i64_ty, steps_pointer, "steps", insert_at_end);
}

llvm::LoadInst *
proof_event::emit_get_proof_chunk_size(llvm::BasicBlock *insert_at_end) {
  auto *i64_ty = llvm::Type::getInt64Ty(ctx_);
  auto *proof_chunk_size_pointer
      = module_->getOrInsertGlobal("proof_chunk_size", i64_ty);
  return new llvm::LoadInst(
      i64_ty, proof_chunk_size_pointer, "proof_chunk_size", insert_at_end);
}

std::pair<llvm::BasicBlock *, llvm::BasicBlock *> proof_event::proof_branch(
    std::string const &label, llvm::BasicBlock *insert_at_end) {
  auto *i1_ty = llvm::Type::getInt1Ty(ctx_);

  auto *proof_output_flag = module_->getOrInsertGlobal("proof_output", i1_ty);
  auto *proof_output = new llvm::LoadInst(
      i1_ty, proof_output_flag, "proof_output", insert_at_end);

  auto *f = insert_at_end->getParent();
  auto *true_block
      = llvm::BasicBlock::Create(ctx_, fmt::format("if_{}", label), f);
  auto *merge_block
      = llvm::BasicBlock::Create(ctx_, fmt::format("tail_{}", label), f);

  emit_no_op(merge_block);

  llvm::BranchInst::Create(
      true_block, merge_block, proof_output, insert_at_end);
  return {true_block, merge_block};
}

std::pair<llvm::BasicBlock *, llvm::BasicBlock *> proof_event::proof_branch(
    std::string const &label, llvm::Instruction *insert_before) {
  auto *i1_ty = llvm::Type::getInt1Ty(ctx_);

  auto *proof_output_flag = module_->getOrInsertGlobal("proof_output", i1_ty);
  auto *proof_output = new llvm::LoadInst(
      i1_ty, proof_output_flag, "proof_output", insert_before);

  auto *f = insert_before->getParent()->getParent();
  auto *true_block
      = llvm::BasicBlock::Create(ctx_, fmt::format("if_{}", label), f);
  auto *merge_block
      = llvm::BasicBlock::Create(ctx_, fmt::format("tail_{}", label), f);

  llvm::BranchInst::Create(
      true_block, merge_block, proof_output, insert_before);

  insert_before->moveBefore(*merge_block, merge_block->begin());

  return {true_block, merge_block};
}

std::tuple<llvm::BasicBlock *, llvm::BasicBlock *, llvm::Value *>
proof_event::event_prelude(
    std::string const &label, llvm::BasicBlock *insert_at_end) {
  auto [true_block, merge_block] = proof_branch(label, insert_at_end);
  return {true_block, merge_block, emit_get_proof_trace_writer(true_block)};
}

std::tuple<llvm::BasicBlock *, llvm::BasicBlock *, llvm::Value *>
proof_event::event_prelude(
    std::string const &label, llvm::Instruction *insert_before) {
  auto [true_block, merge_block] = proof_branch(label, insert_before);
  return {true_block, merge_block, emit_get_proof_trace_writer(true_block)};
}

llvm::BasicBlock *proof_event::check_for_emit_new_chunk(
    llvm::BasicBlock *insert_at_end, llvm::BasicBlock *merge_block) {
  auto *f = insert_at_end->getParent();
  auto *check_steps_block = llvm::BasicBlock::Create(ctx_, "if_do_chunks", f);
  auto *emit_new_chunk_block
      = llvm::BasicBlock::Create(ctx_, "if_new_chunk", f);

  auto *i64_ty = llvm::Type::getInt64Ty(ctx_);
  auto *zero = llvm::ConstantInt::get(i64_ty, 0);

  auto *chunk_size = emit_get_proof_chunk_size(insert_at_end);
  auto *do_chunks_cond = llvm::CmpInst::Create(
      llvm::Instruction::ICmp, llvm::CmpInst::ICMP_NE, chunk_size, zero,
      "do_chunks", insert_at_end);
  llvm::BranchInst::Create(
      check_steps_block, merge_block, do_chunks_cond, insert_at_end);

  auto *steps = emit_get_steps(check_steps_block);
  auto *chunk_steps = llvm::BinaryOperator::Create(
      llvm::Instruction::URem, steps, chunk_size, "current_chunk_steps",
      check_steps_block);
  auto *new_chunk_cond = llvm::CmpInst::Create(
      llvm::Instruction::ICmp, llvm::CmpInst::ICMP_EQ, chunk_steps, zero,
      "start_new_chunk", check_steps_block);
  llvm::BranchInst::Create(
      emit_new_chunk_block, merge_block, new_chunk_cond, check_steps_block);

  return emit_new_chunk_block;
}

/*
 * Hook Events
 */

llvm::BasicBlock *proof_event::hook_event_pre(
    std::string const &name, kore_composite_pattern *pattern,
    llvm::BasicBlock *current_block, std::string const &location_stack) {
  if (!proof_hint_instrumentation) {
    return current_block;
  }

  auto [true_block, merge_block, proof_writer]
      = event_prelude("hookpre", current_block);

  emit_write_hook_event_pre(
      proof_writer, name, ast_to_string(*pattern->get_constructor()),
      location_stack, true_block);

  llvm::BranchInst::Create(merge_block, true_block);
  return merge_block;
}

llvm::BasicBlock *proof_event::hook_event_post(
    llvm::Value *val, kore_composite_sort *sort,
    llvm::BasicBlock *current_block) {
  if (!proof_hint_instrumentation) {
    return current_block;
  }

  auto [true_block, merge_block, proof_writer]
      = event_prelude("hookpost", current_block);

  emit_write_hook_event_post(proof_writer, val, *sort, true_block);

  llvm::BranchInst::Create(merge_block, true_block);
  return merge_block;
}

/*
 * Arguments for Hook/Function Events
 */

llvm::BasicBlock *proof_event::argument(
    llvm::Value *val, kore_composite_sort *sort, bool is_hook_arg,
    llvm::BasicBlock *current_block) {
  if (!proof_hint_instrumentation) {
    return current_block;
  }

  if (!is_hook_arg && !proof_hint_instrumentation_slow) {
    return current_block;
  }

  auto [true_block, merge_block, proof_writer]
      = event_prelude("eventarg", current_block);

  emit_write_argument(proof_writer, val, *sort, true_block);

  llvm::BranchInst::Create(merge_block, true_block);
  return merge_block;
}

llvm::BasicBlock *proof_event::short_circuit_hook_argument(
    llvm::Value *val, llvm::Value *short_circuit_cond, bool invert_cond,
    kore_composite_sort *sort, llvm::BasicBlock *current_block) {
  if (!proof_hint_instrumentation) {
    return current_block;
  }

  auto *f = current_block->getParent();
  auto *true_block = llvm::BasicBlock::Create(ctx_, "if_no_short_circ", f);
  auto *merge_block = llvm::BasicBlock::Create(ctx_, "tail_short_circ", f);

  if (invert_cond) {
    short_circuit_cond = llvm::BinaryOperator::Create(
        llvm::Instruction::Xor, short_circuit_cond,
        llvm::ConstantInt::get(llvm::Type::getInt1Ty(ctx_), 1), "",
        current_block);
  }
  llvm::BranchInst::Create(
      true_block, merge_block, short_circuit_cond, current_block);

  true_block = argument(val, sort, true, true_block);
  llvm::BranchInst::Create(merge_block, true_block);

  return merge_block;
}

llvm::BasicBlock *proof_event::short_circuit_hook_argument(
    llvm::Value *val_first, llvm::Value *val_second, llvm::Value *select_cond,
    kore_composite_sort *sort_first, kore_composite_sort *sort_second,
    llvm::BasicBlock *current_block) {
  if (!proof_hint_instrumentation) {
    return current_block;
  }

  auto *f = current_block->getParent();
  auto *first_block
      = llvm::BasicBlock::Create(ctx_, "if_short_circ_select_first", f);
  auto *second_block
      = llvm::BasicBlock::Create(ctx_, "if_short_circ_select_second", f);
  auto *merge_block
      = llvm::BasicBlock::Create(ctx_, "tail_short_circ_select", f);

  llvm::BranchInst::Create(
      first_block, second_block, select_cond, current_block);

  first_block = argument(val_first, sort_first, true, first_block);
  llvm::BranchInst::Create(merge_block, first_block);

  second_block = argument(val_second, sort_second, true, second_block);
  llvm::BranchInst::Create(merge_block, second_block);

  return merge_block;
}

/*
 * Rewrite Events
 */

llvm::BasicBlock *proof_event::rewrite_event_pre(
    kore_axiom_declaration const &axiom, uint64_t arity,
    std::map<std::string, kore_variable_pattern *> vars,
    llvm::StringMap<llvm::Value *> const &subst,
    llvm::BasicBlock *current_block) {
  if (!proof_hint_instrumentation) {
    return current_block;
  }

  auto [true_block, merge_block, proof_writer]
      = event_prelude("rewrite_pre", current_block);

  emit_write_rewrite_event_pre(
      proof_writer, axiom.get_ordinal(), arity, true_block);

  for (auto entry = subst.begin(); entry != subst.end(); ++entry) {
    auto key = entry->getKey();
    auto *val = entry->getValue();
    auto *var = vars[key.str()];

    auto sort = std::dynamic_pointer_cast<kore_composite_sort>(var->get_sort());

    emit_write_variable(proof_writer, key.str(), val, *sort, true_block);
  }

  llvm::BranchInst::Create(merge_block, true_block);
  return merge_block;
}

llvm::BasicBlock *proof_event::rewrite_event_post(
    kore_axiom_declaration *axiom, llvm::Value *return_value,
    llvm::BasicBlock *current_block) {
  if (!proof_hint_instrumentation) {
    return current_block;
  }

  auto [true_block, merge_block, proof_writer]
      = event_prelude("rewrite_post", current_block);

  if (proof_hint_instrumentation_slow) {
    auto return_sort = std::dynamic_pointer_cast<kore_composite_sort>(
        axiom->get_right_hand_side()->get_sort());

    emit_write_rewrite_event_post(
        proof_writer, return_value, *return_sort, true_block);
  }

  true_block = check_for_emit_new_chunk(true_block, merge_block);
  emit_start_new_chunk(proof_writer, true_block);

  llvm::BranchInst::Create(merge_block, true_block);
  return merge_block;
}

/*
 * Function Events
 */

llvm::BasicBlock *proof_event::function_event_pre(
    llvm::BasicBlock *current_block, kore_composite_pattern *pattern,
    std::string const &location_stack) {
  if (!proof_hint_instrumentation) {
    return current_block;
  }

  auto [true_block, merge_block, proof_writer]
      = event_prelude("function_pre", current_block);

  emit_write_function_event_pre(
      proof_writer, ast_to_string(*pattern->get_constructor()), location_stack,
      true_block);

  llvm::BranchInst::Create(merge_block, true_block);
  return merge_block;
}

llvm::BasicBlock *
proof_event::function_event_post(llvm::BasicBlock *current_block) {
  if (!proof_hint_instrumentation) {
    return current_block;
  }

  auto [true_block, merge_block, proof_writer]
      = event_prelude("function_post", current_block);

  emit_write_function_event_post(proof_writer, true_block);

  llvm::BranchInst::Create(merge_block, true_block);

  return merge_block;
}

llvm::BasicBlock *proof_event::side_condition_event_pre(
    kore_axiom_declaration const &axiom, std::vector<llvm::Value *> const &args,
    llvm::BasicBlock *current_block) {
  if (!proof_hint_instrumentation) {
    return current_block;
  }

  auto [true_block, merge_block, proof_writer]
      = event_prelude("side_condition_pre", current_block);

  size_t ordinal = axiom.get_ordinal();
  size_t arity = args.size();

  emit_write_side_condition_event_pre(proof_writer, ordinal, arity, true_block);

  kore_pattern *pattern = axiom.get_requires();
  std::map<std::string, kore_variable_pattern *> vars;
  pattern->mark_variables(vars);

  int i = 0;
  for (auto entry = vars.begin(); entry != vars.end(); ++i, ++entry) {
    auto var_name = entry->first;
    auto *var = entry->second;
    auto *val = args[i];

    auto sort = std::dynamic_pointer_cast<kore_composite_sort>(var->get_sort());

    emit_write_variable(proof_writer, var_name, val, *sort, true_block);
  }

  llvm::BranchInst::Create(merge_block, true_block);

  return merge_block;
}

llvm::BasicBlock *proof_event::side_condition_event_post(
    kore_axiom_declaration const &axiom, llvm::Value *check_result,
    llvm::BasicBlock *current_block) {
  if (!proof_hint_instrumentation) {
    return current_block;
  }

  auto [true_block, merge_block, proof_writer]
      = event_prelude("side_condition_post", current_block);

  size_t ordinal = axiom.get_ordinal();

  emit_write_side_condition_event_post(
      proof_writer, ordinal, check_result, true_block);

  llvm::BranchInst::Create(merge_block, true_block);

  return merge_block;
}

llvm::BasicBlock *proof_event::pattern_matching_failure(
    kore_composite_pattern const &pattern, llvm::BasicBlock *current_block) {

  if (!proof_hint_instrumentation) {
    return current_block;
  }

  auto [true_block, merge_block, proof_writer]
      = event_prelude("pattern_matching_failure", current_block);

  std::string function_name = ast_to_string(*pattern.get_constructor());

  emit_write_pattern_matching_failure(proof_writer, function_name, true_block);

  llvm::BranchInst::Create(merge_block, true_block);

  return merge_block;
}

llvm::BasicBlock *proof_event::tail_call_info(
    std::string const &caller_name, bool is_tail,
    llvm::Instruction *insert_before, llvm::BasicBlock *current_block) {

  if (!proof_hint_instrumentation) {
    return current_block;
  }

  std::tuple<llvm::BasicBlock *, llvm::BasicBlock *, llvm::Value *>  prelude;
  if (is_tail) {
    assert(insert_before);
    prelude = event_prelude("tail_call_info", insert_before);
  } else {
    prelude = event_prelude("tail_call_info", current_block);
  }

  auto [true_block, merge_block, proof_writer] = prelude;

  emit_write_tail_call_info(proof_writer, caller_name, is_tail, true_block);

  llvm::BranchInst::Create(merge_block, true_block);

  return merge_block;
}

} // namespace kllvm
