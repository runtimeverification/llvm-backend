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

} // namespace

llvm::CallInst *writeUInt64(
    llvm::Value *outputFile, llvm::Module *Module, uint64_t value,
    llvm::BasicBlock *Block) {
  auto &Ctx = Module->getContext();

  auto void_ty = llvm::Type::getVoidTy(Ctx);
  auto i8_ptr_ty = llvm::Type::getInt8PtrTy(Ctx);
  auto i64_ptr_ty = llvm::Type::getInt64Ty(Ctx);

  auto func_ty
      = llvm::FunctionType::get(void_ty, {i8_ptr_ty, i64_ptr_ty}, false);
  auto func = getOrInsertFunction(Module, "writeUInt64ToFile", func_ty);

  auto i64_value = llvm::ConstantInt::get(i64_ptr_ty, value);

  return llvm::CallInst::Create(func, {outputFile, i64_value}, "", Block);
}

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

llvm::CallInst *ProofEvent::emitWriteUInt64(
    llvm::Value *outputFile, uint64_t value, llvm::BasicBlock *insert_at_end) {
  return writeUInt64(outputFile, Module, value, insert_at_end);
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

std::pair<llvm::BasicBlock *, llvm::BasicBlock *>
ProofEvent::proofBranch(std::string const &label) {
  return proofBranch(label, CurrentBlock);
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

llvm::BasicBlock *ProofEvent::hookEvent_pre(std::string name) {
  auto b = proofBranch("hookpre");
  auto TrueBlock = b.first;
  auto MergeBlock = b.second;
  auto outputFile = emitGetOutputFileName(TrueBlock);
  auto ir = new llvm::IRBuilder(TrueBlock);

  auto nameptr = ir->CreateGlobalStringPtr(name, "", 0, Module);

  emitWriteUInt64(outputFile, 0xaaaaaaaaaaaaaaaa, TrueBlock);
  ir->CreateCall(
      getOrInsertFunction(
          Module, "printVariableToFile",
          llvm::Type::getVoidTy(Module->getContext()),
          llvm::Type::getInt8PtrTy(Module->getContext()),
          llvm::Type::getInt8PtrTy(Module->getContext())),
      {outputFile, nameptr});

  llvm::BranchInst::Create(MergeBlock, TrueBlock);
  emitNoOp(MergeBlock);

  return CurrentBlock = MergeBlock;
}

llvm::BasicBlock *
ProofEvent::hookEvent_post(llvm::Value *val, KORECompositeSort *sort) {
  auto b = proofBranch("hookpost");
  auto TrueBlock = b.first;
  auto MergeBlock = b.second;
  auto outputFile = emitGetOutputFileName(TrueBlock);

  emitWriteUInt64(outputFile, 0xbbbbbbbbbbbbbbbb, TrueBlock);

  emitSerializeTerm(*sort, outputFile, val, TrueBlock);

  llvm::BranchInst::Create(MergeBlock, TrueBlock);
  emitNoOp(MergeBlock);

  return CurrentBlock = MergeBlock;
}

llvm::BasicBlock *
ProofEvent::hookArg(llvm::Value *val, KORECompositeSort *sort) {
  auto b = proofBranch("hookarg");
  auto TrueBlock = b.first;
  auto MergeBlock = b.second;

  auto outputFile = emitGetOutputFileName(TrueBlock);

  emitSerializeTerm(*sort, outputFile, val, TrueBlock);

  llvm::BranchInst::Create(MergeBlock, TrueBlock);
  emitNoOp(MergeBlock);

  return CurrentBlock = MergeBlock;
}

} // namespace kllvm
