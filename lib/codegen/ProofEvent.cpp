#include "kllvm/codegen/ProofEvent.h"
#include "kllvm/codegen/CreateTerm.h"

#include "llvm/IR/IRBuilder.h"

#include <fmt/format.h>

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

  llvm::BranchInst::Create(
      true_block, merge_block, proof_output, insert_at_end);
  return {true_block, merge_block};
}

/*
 * Hook Events
 */

llvm::BasicBlock *
ProofEvent::hookEvent_pre(std::string name, llvm::BasicBlock *current_block) {
  auto [true_block, merge_block] = proofBranch("hookpre", current_block);
  auto outputFile = emitGetOutputFileName(true_block);

  emitWriteUInt64(outputFile, word(0xAA), true_block);
  emitWriteString(outputFile, name, true_block);

  llvm::BranchInst::Create(merge_block, true_block);
  emitNoOp(merge_block);

  return merge_block;
}

llvm::BasicBlock *ProofEvent::hookEvent_post(
    llvm::Value *val, KORECompositeSort *sort,
    llvm::BasicBlock *current_block) {
  auto [true_block, merge_block] = proofBranch("hookpost", current_block);
  auto outputFile = emitGetOutputFileName(true_block);

  emitWriteUInt64(outputFile, word(0xBB), true_block);

  emitSerializeTerm(*sort, outputFile, val, true_block);

  llvm::BranchInst::Create(merge_block, true_block);
  emitNoOp(merge_block);

  return merge_block;
}

llvm::BasicBlock *ProofEvent::hookArg(
    llvm::Value *val, KORECompositeSort *sort,
    llvm::BasicBlock *current_block) {
  auto [true_block, merge_block] = proofBranch("hookarg", current_block);

  auto outputFile = emitGetOutputFileName(true_block);

  emitSerializeTerm(*sort, outputFile, val, true_block);

  llvm::BranchInst::Create(merge_block, true_block);
  emitNoOp(merge_block);

  return merge_block;
}

llvm::BasicBlock *ProofEvent::rewriteEvent(
    KOREAxiomDeclaration *axiom, llvm::Value *return_value, uint64_t arity,
    std::map<std::string, KOREVariablePattern *> vars,
    llvm::StringMap<llvm::Value *> const &subst,
    llvm::BasicBlock *current_block) {
  auto [true_block, merge_block] = proofBranch("hookarg", current_block);
  auto outputFile = emitGetOutputFileName(true_block);

  emitWriteUInt64(outputFile, axiom->getOrdinal(), true_block);
  emitWriteUInt64(outputFile, arity, true_block);
  for (auto entry = subst.begin(); entry != subst.end(); ++entry) {
    auto key = entry->getKey();
    auto val = entry->getValue();
    auto var = vars[key.str()];

    auto sort = dynamic_cast<KORECompositeSort *>(var->getSort().get());

    emitWriteString(outputFile, key.str(), true_block);
    emitSerializeTerm(*sort, outputFile, val, true_block);
    emitWriteUInt64(outputFile, word(0xCC), true_block);
  }

  emitWriteUInt64(outputFile, word(0xFF), true_block);
  emitSerializeConfiguration(outputFile, return_value, true_block);
  emitWriteUInt64(outputFile, word(0xCC), true_block);

  llvm::BranchInst::Create(merge_block, true_block);
  return merge_block;
}

llvm::BasicBlock *ProofEvent::functionEvent(
    llvm::BasicBlock *current_block, KORECompositePattern *pattern,
    std::string const &locationStack) {
  auto [true_block, merge_block] = proofBranch("hookarg", current_block);
  auto outputFile = emitGetOutputFileName(true_block);

  std::ostringstream symbolName;
  pattern->getConstructor()->print(symbolName);

  emitWriteUInt64(outputFile, word(0xDD), true_block);
  emitWriteString(outputFile, symbolName.str(), true_block);
  emitWriteString(outputFile, locationStack, true_block);

  llvm::BranchInst::Create(merge_block, true_block);
  return merge_block;
}

} // namespace kllvm
