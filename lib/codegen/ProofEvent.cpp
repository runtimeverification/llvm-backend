#include "kllvm/codegen/ProofEvent.h"
#include "kllvm/codegen/CreateTerm.h"

#include "llvm/IR/IRBuilder.h"
#include "llvm/Support/CommandLine.h"

#include <fmt/format.h>

extern llvm::cl::OptionCategory CodegenCat;

llvm::cl::opt<bool> ProofHintInstrumentation(
    "proof-hint-instrumentation",
    llvm::cl::desc("Enable instrumentation for generation of proof hints"),
    llvm::cl::cat(CodegenCat));

namespace kllvm {

/*
 * Internal implementation details
 */

namespace {

template <typename IRBuilder>
llvm::Constant *createGlobalSortStringPtr(
    IRBuilder &B, KORECompositeSort &sort, llvm::Module *mod) {
  auto os = std::ostringstream{};
  sort.print(os);
  return B.CreateGlobalStringPtr(
      os.str(), fmt::format("{}_str", sort.getName()), 0, mod);
}

constexpr uint64_t word(uint8_t byte) {
  auto ret = uint64_t{0};
  for (auto i = 0u; i < sizeof(ret); ++i) {
    ret <<= 8;
    ret |= byte;
  }
  return ret;
}

static_assert(word(0xAA) == 0xAAAAAAAAAAAAAAAA);

} // namespace

llvm::CallInst *ProofEvent::emitSerializeTerm(
    KORECompositeSort &sort, llvm::Value *outputFile, llvm::Value *term,
    llvm::BasicBlock *insert_at_end) {
  auto B = llvm::IRBuilder(insert_at_end);

  auto cat = sort.getCategory(Definition);
  auto sort_name_ptr = createGlobalSortStringPtr(B, sort, Module);

  auto void_ty = llvm::Type::getVoidTy(Ctx);
  auto i8_ptr_ty = llvm::Type::getInt8PtrTy(Ctx);

  if (cat.cat == SortCategory::Symbol || cat.cat == SortCategory::Variable) {
    auto block_ty = getValueType({SortCategory::Symbol, 0}, Module);

    auto func_ty = llvm::FunctionType::get(
        void_ty, {i8_ptr_ty, block_ty, i8_ptr_ty}, false);

    auto serialize
        = getOrInsertFunction(Module, "serializeTermToFile", func_ty);

    return B.CreateCall(serialize, {outputFile, term, sort_name_ptr});
  } else {
    if (term->getType()->isIntegerTy()) {
      term = B.CreateIntToPtr(term, i8_ptr_ty);
    } else {
      term = B.CreatePointerCast(term, i8_ptr_ty);
    }

    auto func_ty = llvm::FunctionType::get(
        void_ty, {i8_ptr_ty, i8_ptr_ty, i8_ptr_ty}, false);

    auto serialize
        = getOrInsertFunction(Module, "serializeRawTermToFile", func_ty);

    return B.CreateCall(serialize, {outputFile, term, sort_name_ptr});
  }
}

llvm::CallInst *ProofEvent::emitSerializeConfiguration(
    llvm::Value *outputFile, llvm::Value *config,
    llvm::BasicBlock *insertAtEnd) {
  auto void_ty = llvm::Type::getVoidTy(Ctx);
  auto i8_ptr_ty = llvm::Type::getInt8PtrTy(Ctx);
  auto block_ty = getValueType({SortCategory::Symbol, 0}, Module);

  auto func_ty = llvm::FunctionType::get(void_ty, {i8_ptr_ty, block_ty}, false);
  auto serialize
      = getOrInsertFunction(Module, "serializeConfigurationToFile", func_ty);

  return llvm::CallInst::Create(
      serialize, {outputFile, config}, "", insertAtEnd);
}

llvm::CallInst *ProofEvent::emitWriteUInt64(
    llvm::Value *outputFile, uint64_t value, llvm::BasicBlock *insert_at_end) {
  auto void_ty = llvm::Type::getVoidTy(Ctx);
  auto i8_ptr_ty = llvm::Type::getInt8PtrTy(Ctx);
  auto i64_ptr_ty = llvm::Type::getInt64Ty(Ctx);

  auto func_ty
      = llvm::FunctionType::get(void_ty, {i8_ptr_ty, i64_ptr_ty}, false);
  auto func = getOrInsertFunction(Module, "writeUInt64ToFile", func_ty);

  auto i64_value = llvm::ConstantInt::get(i64_ptr_ty, value);

  return llvm::CallInst::Create(
      func, {outputFile, i64_value}, "", insert_at_end);
}

llvm::CallInst *ProofEvent::emitWriteString(
    llvm::Value *outputFile, std::string const &str,
    llvm::BasicBlock *insertAtEnd) {
  auto B = llvm::IRBuilder(insertAtEnd);

  auto void_ty = llvm::Type::getVoidTy(Ctx);
  auto i8_ptr_ty = llvm::Type::getInt8PtrTy(Ctx);

  auto func_ty
      = llvm::FunctionType::get(void_ty, {i8_ptr_ty, i8_ptr_ty}, false);

  auto print = getOrInsertFunction(Module, "printVariableToFile", func_ty);

  auto varname = B.CreateGlobalStringPtr(str, "", 0, Module);
  return B.CreateCall(print, {outputFile, varname});
}

llvm::BinaryOperator *ProofEvent::emitNoOp(llvm::BasicBlock *insert_at_end) {
  auto i8_ty = llvm::Type::getInt8Ty(Ctx);
  auto zero = llvm::ConstantInt::get(i8_ty, 0);

  return llvm::BinaryOperator::Create(
      llvm::Instruction::Add, zero, zero, "no-op", insert_at_end);
}

llvm::LoadInst *
ProofEvent::emitGetOutputFileName(llvm::BasicBlock *insert_at_end) {
  auto i8_ptr_ty = llvm::Type::getInt8PtrTy(Ctx);
  auto fileNamePointer = Module->getOrInsertGlobal("output_file", i8_ptr_ty);
  return new llvm::LoadInst(
      i8_ptr_ty, fileNamePointer, "output", insert_at_end);
}

std::pair<llvm::BasicBlock *, llvm::BasicBlock *> ProofEvent::proofBranch(
    std::string const &label, llvm::BasicBlock *insert_at_end) {
  auto i1_ty = llvm::Type::getInt1Ty(Ctx);

  auto proof_output_flag = Module->getOrInsertGlobal("proof_output", i1_ty);
  auto proof_output = new llvm::LoadInst(
      i1_ty, proof_output_flag, "proof_output", insert_at_end);

  auto f = insert_at_end->getParent();
  auto true_block
      = llvm::BasicBlock::Create(Ctx, fmt::format("if_{}", label), f);
  auto *merge_block
      = llvm::BasicBlock::Create(Ctx, fmt::format("tail_{}", label), f);

  emitNoOp(merge_block);

  llvm::BranchInst::Create(
      true_block, merge_block, proof_output, insert_at_end);
  return {true_block, merge_block};
}

std::tuple<llvm::BasicBlock *, llvm::BasicBlock *, llvm::Value *>
ProofEvent::eventPrelude(
    std::string const &label, llvm::BasicBlock *insertAtEnd) {
  auto [true_block, merge_block] = proofBranch(label, insertAtEnd);
  return {true_block, merge_block, emitGetOutputFileName(true_block)};
}

/*
 * Hook Events
 */

llvm::BasicBlock *
ProofEvent::hookEvent_pre(std::string name, llvm::BasicBlock *current_block) {
  if (!ProofHintInstrumentation) {
    return current_block;
  }

  auto [true_block, merge_block, outputFile]
      = eventPrelude("hookpre", current_block);

  emitWriteUInt64(outputFile, word(0xAA), true_block);
  emitWriteString(outputFile, name, true_block);

  llvm::BranchInst::Create(merge_block, true_block);
  return merge_block;
}

llvm::BasicBlock *ProofEvent::hookEvent_post(
    llvm::Value *val, KORECompositeSort *sort,
    llvm::BasicBlock *current_block) {
  if (!ProofHintInstrumentation) {
    return current_block;
  }

  auto [true_block, merge_block, outputFile]
      = eventPrelude("hookpost", current_block);

  emitWriteUInt64(outputFile, word(0xBB), true_block);

  emitSerializeTerm(*sort, outputFile, val, true_block);

  llvm::BranchInst::Create(merge_block, true_block);
  return merge_block;
}

llvm::BasicBlock *ProofEvent::hookArg(
    llvm::Value *val, KORECompositeSort *sort,
    llvm::BasicBlock *current_block) {
  if (!ProofHintInstrumentation) {
    return current_block;
  }

  auto [true_block, merge_block, outputFile]
      = eventPrelude("hookarg", current_block);

  emitSerializeTerm(*sort, outputFile, val, true_block);

  llvm::BranchInst::Create(merge_block, true_block);
  return merge_block;
}

/*
 * Rewrite Events
 */

llvm::BasicBlock *ProofEvent::rewriteEvent_pre(
    KOREAxiomDeclaration *axiom, uint64_t arity,
    std::map<std::string, KOREVariablePattern *> vars,
    llvm::StringMap<llvm::Value *> const &subst,
    llvm::BasicBlock *current_block) {
  if (!ProofHintInstrumentation) {
    return current_block;
  }

  auto [true_block, merge_block, outputFile]
      = eventPrelude("rewrite_pre", current_block);

  emitWriteUInt64(outputFile, word(0x22), true_block);
  emitWriteUInt64(outputFile, axiom->getOrdinal(), true_block);
  emitWriteUInt64(outputFile, arity, true_block);
  for (auto entry = subst.begin(); entry != subst.end(); ++entry) {
    auto key = entry->getKey();
    auto val = entry->getValue();
    auto var = vars[key.str()];

    auto sort = std::dynamic_pointer_cast<KORECompositeSort>(var->getSort());

    emitWriteString(outputFile, key.str(), true_block);
    emitSerializeTerm(*sort, outputFile, val, true_block);
    emitWriteUInt64(outputFile, word(0xCC), true_block);
  }

  llvm::BranchInst::Create(merge_block, true_block);
  return merge_block;
}

llvm::BasicBlock *ProofEvent::rewriteEvent_post(
    KOREAxiomDeclaration *axiom, llvm::Value *return_value,
    llvm::BasicBlock *current_block) {
  auto [true_block, merge_block, output_file]
      = eventPrelude("rewrite_post", current_block);

  auto return_sort = std::dynamic_pointer_cast<KORECompositeSort>(
      axiom->getRightHandSide()->getSort());

  emitWriteUInt64(output_file, word(0xFF), true_block);
  emitSerializeTerm(*return_sort, output_file, return_value, true_block);
  emitWriteUInt64(output_file, word(0xCC), true_block);

  llvm::BranchInst::Create(merge_block, true_block);
  return merge_block;
}

/*
 * Function Events
 */

llvm::BasicBlock *ProofEvent::functionEvent_pre(
    llvm::BasicBlock *current_block, KORECompositePattern *pattern,
    std::string const &locationStack) {
  if (!ProofHintInstrumentation) {
    return current_block;
  }

  auto [true_block, merge_block, outputFile]
      = eventPrelude("function_pre", current_block);

  std::ostringstream symbolName;
  pattern->getConstructor()->print(symbolName);

  emitWriteUInt64(outputFile, word(0xDD), true_block);
  emitWriteString(outputFile, symbolName.str(), true_block);
  emitWriteString(outputFile, locationStack, true_block);

  llvm::BranchInst::Create(merge_block, true_block);
  return merge_block;
}

llvm::BasicBlock *
ProofEvent::functionEvent_post(llvm::BasicBlock *current_block) {
  auto [true_block, merge_block, outputFile]
      = eventPrelude("function_post", current_block);

  emitWriteUInt64(outputFile, word(0x11), true_block);

  llvm::BranchInst::Create(merge_block, true_block);

  return merge_block;
}

llvm::BasicBlock *ProofEvent::sideConditionEvent(
    KOREAxiomDeclaration *axiom, std::vector<llvm::Value *> const &args,
    llvm::BasicBlock *current_block) {
  if (!ProofHintInstrumentation) {
    return current_block;
  }

  auto [true_block, merge_block, outputFile]
      = eventPrelude("side_condition", current_block);

  size_t ordinal = axiom->getOrdinal();
  size_t arity = args.size();

  emitWriteUInt64(outputFile, word(0xEE), true_block);
  emitWriteUInt64(outputFile, ordinal, true_block);
  emitWriteUInt64(outputFile, arity, true_block);

  KOREPattern *pattern = axiom->getRequires();
  std::map<std::string, KOREVariablePattern *> vars;
  pattern->markVariables(vars);

  int i = 0;
  for (auto entry = vars.begin(); entry != vars.end(); ++i, ++entry) {
    auto varName = entry->first;
    auto var = entry->second;
    auto val = args[i];

    auto sort = std::dynamic_pointer_cast<KORECompositeSort>(var->getSort());

    emitWriteString(outputFile, varName, true_block);
    emitSerializeTerm(*sort, outputFile, val, true_block);
    emitWriteUInt64(outputFile, word(0xCC), true_block);
  }

  llvm::BranchInst::Create(merge_block, true_block);

  return merge_block;
}

} // namespace kllvm
