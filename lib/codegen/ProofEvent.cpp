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

} // namespace

llvm::CallInst *proof_event::emit_serialize_term(
    kore_composite_sort &sort, llvm::Value *output_file, llvm::Value *term,
    llvm::BasicBlock *insert_at_end) {
  auto b = llvm::IRBuilder(insert_at_end);

  std::string sort_name = ast_to_string(sort);
  bool indirect
      = sort_name == "SortBool{}" || sort_name.substr(0, 9) == "SortMInt{";
  std::string inj_name;
  if (sort_name == "SortKItem{}") {
    inj_name = "rawTerm{}";
  } else if (sort_name == "SortK{}") {
    inj_name = "rawKTerm{}";
  } else {
    inj_name = "inj{" + sort_name + ", SortKItem{}}";
  }
  auto *symbol = definition_->get_all_symbols().at(inj_name);
  uint64_t block_header = get_block_header_val(
      module_, symbol, get_block_type(module_, definition_, symbol));

  auto *void_ty = llvm::Type::getVoidTy(ctx_);
  auto *i8_ptr_ty = llvm::Type::getInt8PtrTy(ctx_);
  auto *i1_ty = llvm::Type::getInt1Ty(ctx_);
  auto *i64_ty = llvm::Type::getInt64Ty(ctx_);

  auto *func_ty = llvm::FunctionType::get(
      void_ty, {i8_ptr_ty, i8_ptr_ty, i64_ty, i1_ty}, false);

  auto *serialize
      = get_or_insert_function(module_, "serialize_term_to_file_v2", func_ty);

  return b.CreateCall(
      serialize,
      {output_file, term, llvm::ConstantInt::get(i64_ty, block_header),
       llvm::ConstantInt::get(i1_ty, indirect)});
}

llvm::CallInst *proof_event::emit_write_uint64(
    llvm::Value *output_file, uint64_t value, llvm::BasicBlock *insert_at_end) {
  auto *void_ty = llvm::Type::getVoidTy(ctx_);
  auto *i8_ptr_ty = llvm::Type::getInt8PtrTy(ctx_);
  auto *i64_ptr_ty = llvm::Type::getInt64Ty(ctx_);

  auto *func_ty
      = llvm::FunctionType::get(void_ty, {i8_ptr_ty, i64_ptr_ty}, false);
  auto *func = get_or_insert_function(module_, "write_uint64_to_file", func_ty);

  auto *i64_value = llvm::ConstantInt::get(i64_ptr_ty, value);

  return llvm::CallInst::Create(
      func, {output_file, i64_value}, "", insert_at_end);
}

llvm::CallInst *proof_event::emit_bool_term(
    llvm::Value *output_file, llvm::Value *term,
    llvm::BasicBlock *insert_at_end) {
  auto b = llvm::IRBuilder(insert_at_end);

  auto *void_ty = llvm::Type::getVoidTy(ctx_);
  auto *i8_ptr_ty = llvm::Type::getInt8PtrTy(ctx_);

  term = b.CreateIntToPtr(term, i8_ptr_ty);

  auto *func_ty
      = llvm::FunctionType::get(void_ty, {i8_ptr_ty, i8_ptr_ty}, false);

  auto *serialize
      = get_or_insert_function(module_, "write_bool_to_file", func_ty);

  return b.CreateCall(serialize, {output_file, term});
}

llvm::CallInst *proof_event::emit_write_string(
    llvm::Value *output_file, std::string const &str,
    llvm::BasicBlock *insert_at_end) {
  auto b = llvm::IRBuilder(insert_at_end);

  auto *void_ty = llvm::Type::getVoidTy(ctx_);
  auto *i8_ptr_ty = llvm::Type::getInt8PtrTy(ctx_);

  auto *func_ty
      = llvm::FunctionType::get(void_ty, {i8_ptr_ty, i8_ptr_ty}, false);

  auto *print
      = get_or_insert_function(module_, "print_variable_to_file", func_ty);

  auto *varname = b.CreateGlobalStringPtr(str, "", 0, module_);
  return b.CreateCall(print, {output_file, varname});
}

llvm::BinaryOperator *proof_event::emit_no_op(llvm::BasicBlock *insert_at_end) {
  auto *i8_ty = llvm::Type::getInt8Ty(ctx_);
  auto *zero = llvm::ConstantInt::get(i8_ty, 0);

  return llvm::BinaryOperator::Create(
      llvm::Instruction::Add, zero, zero, "no-op", insert_at_end);
}

llvm::LoadInst *
proof_event::emit_get_output_file_name(llvm::BasicBlock *insert_at_end) {
  auto *i8_ptr_ty = llvm::Type::getInt8PtrTy(ctx_);
  auto *file_name_pointer
      = module_->getOrInsertGlobal("output_file", i8_ptr_ty);
  return new llvm::LoadInst(
      i8_ptr_ty, file_name_pointer, "output", insert_at_end);
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

std::tuple<llvm::BasicBlock *, llvm::BasicBlock *, llvm::Value *>
proof_event::event_prelude(
    std::string const &label, llvm::BasicBlock *insert_at_end) {
  auto [true_block, merge_block] = proof_branch(label, insert_at_end);
  return {true_block, merge_block, emit_get_output_file_name(true_block)};
}

/*
 * Hook Events
 */

llvm::BasicBlock *proof_event::hook_event_pre(
    std::string const &name, llvm::BasicBlock *current_block,
    std::string const &location_stack) {
  if (!proof_hint_instrumentation) {
    return current_block;
  }

  auto [true_block, merge_block, outputFile]
      = event_prelude("hookpre", current_block);

  emit_write_uint64(outputFile, detail::word(0xAA), true_block);
  emit_write_string(outputFile, name, true_block);
  emit_write_string(outputFile, location_stack, true_block);

  llvm::BranchInst::Create(merge_block, true_block);
  return merge_block;
}

llvm::BasicBlock *proof_event::hook_event_post(
    llvm::Value *val, kore_composite_sort *sort,
    llvm::BasicBlock *current_block) {
  if (!proof_hint_instrumentation) {
    return current_block;
  }

  auto [true_block, merge_block, outputFile]
      = event_prelude("hookpost", current_block);

  emit_write_uint64(outputFile, detail::word(0xBB), true_block);

  emit_serialize_term(*sort, outputFile, val, true_block);

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

  auto [true_block, merge_block, outputFile]
      = event_prelude("eventarg", current_block);

  emit_serialize_term(*sort, outputFile, val, true_block);

  llvm::BranchInst::Create(merge_block, true_block);
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

  auto [true_block, merge_block, outputFile]
      = event_prelude("rewrite_pre", current_block);

  emit_write_uint64(outputFile, detail::word(0x22), true_block);
  emit_write_uint64(outputFile, axiom.get_ordinal(), true_block);
  emit_write_uint64(outputFile, arity, true_block);
  for (auto entry = subst.begin(); entry != subst.end(); ++entry) {
    auto key = entry->getKey();
    auto *val = entry->getValue();
    auto *var = vars[key.str()];

    auto sort = std::dynamic_pointer_cast<kore_composite_sort>(var->get_sort());

    emit_write_string(outputFile, key.str(), true_block);
    emit_serialize_term(*sort, outputFile, val, true_block);
    emit_write_uint64(outputFile, detail::word(0xCC), true_block);
  }

  llvm::BranchInst::Create(merge_block, true_block);
  return merge_block;
}

llvm::BasicBlock *proof_event::rewrite_event_post(
    kore_axiom_declaration *axiom, llvm::Value *return_value,
    llvm::BasicBlock *current_block) {
  if (!proof_hint_instrumentation_slow) {
    return current_block;
  }

  auto [true_block, merge_block, output_file]
      = event_prelude("rewrite_post", current_block);

  auto return_sort = std::dynamic_pointer_cast<kore_composite_sort>(
      axiom->get_right_hand_side()->get_sort());

  emit_write_uint64(output_file, detail::word(0xFF), true_block);
  emit_serialize_term(*return_sort, output_file, return_value, true_block);
  emit_write_uint64(output_file, detail::word(0xCC), true_block);

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

  auto [true_block, merge_block, outputFile]
      = event_prelude("function_pre", current_block);

  emit_write_uint64(outputFile, detail::word(0xDD), true_block);
  emit_write_string(
      outputFile, ast_to_string(*pattern->get_constructor()), true_block);
  emit_write_string(outputFile, location_stack, true_block);

  llvm::BranchInst::Create(merge_block, true_block);
  return merge_block;
}

llvm::BasicBlock *
proof_event::function_event_post(llvm::BasicBlock *current_block) {
  if (!proof_hint_instrumentation) {
    return current_block;
  }

  auto [true_block, merge_block, outputFile]
      = event_prelude("function_post", current_block);

  emit_write_uint64(outputFile, detail::word(0x11), true_block);

  llvm::BranchInst::Create(merge_block, true_block);

  return merge_block;
}

llvm::BasicBlock *proof_event::side_condition_event_pre(
    kore_axiom_declaration const &axiom, std::vector<llvm::Value *> const &args,
    llvm::BasicBlock *current_block) {
  if (!proof_hint_instrumentation) {
    return current_block;
  }

  auto [true_block, merge_block, outputFile]
      = event_prelude("side_condition_pre", current_block);

  size_t ordinal = axiom.get_ordinal();
  size_t arity = args.size();

  emit_write_uint64(outputFile, detail::word(0xEE), true_block);
  emit_write_uint64(outputFile, ordinal, true_block);
  emit_write_uint64(outputFile, arity, true_block);

  kore_pattern *pattern = axiom.get_requires();
  std::map<std::string, kore_variable_pattern *> vars;
  pattern->mark_variables(vars);

  int i = 0;
  for (auto entry = vars.begin(); entry != vars.end(); ++i, ++entry) {
    auto var_name = entry->first;
    auto *var = entry->second;
    auto *val = args[i];

    auto sort = std::dynamic_pointer_cast<kore_composite_sort>(var->get_sort());

    emit_write_string(outputFile, var_name, true_block);
    emit_serialize_term(*sort, outputFile, val, true_block);
    emit_write_uint64(outputFile, detail::word(0xCC), true_block);
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

  auto [true_block, merge_block, outputFile]
      = event_prelude("side_condition_post", current_block);

  size_t ordinal = axiom.get_ordinal();

  emit_write_uint64(outputFile, detail::word(0x33), true_block);
  emit_write_uint64(outputFile, ordinal, true_block);
  emit_bool_term(outputFile, check_result, true_block);

  llvm::BranchInst::Create(merge_block, true_block);

  return merge_block;
}

} // namespace kllvm
