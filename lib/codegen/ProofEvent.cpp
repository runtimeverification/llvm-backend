#include "kllvm/codegen/ProofEvent.h"
#include "kllvm/codegen/CreateTerm.h"

#include "llvm/IR/IRBuilder.h"

#include <fmt/format.h>

namespace kllvm {

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
    llvm::BasicBlock *insertAtEnd) {
  auto B = llvm::IRBuilder(insertAtEnd);

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
    llvm::Value *outputFile, uint64_t value, llvm::BasicBlock *insertAtEnd) {
  return writeUInt64(outputFile, Module, value, insertAtEnd);
}

llvm::BinaryOperator *ProofEvent::emitNoOp(llvm::BasicBlock *insertAtEnd) {
  auto i8_ty = llvm::Type::getInt8Ty(Ctx);
  auto zero = llvm::ConstantInt::get(i8_ty, 0);

  return llvm::BinaryOperator::Create(
      llvm::Instruction::Add, zero, zero, "no-op", insertAtEnd);
}

std::pair<llvm::BasicBlock *, llvm::BasicBlock *>
ProofEvent::proofBranch(std::string label) {
  llvm::Function *f = CurrentBlock->getParent();
  auto ProofOutputFlag = Module->getOrInsertGlobal(
      "proof_output", llvm::Type::getInt1Ty(Module->getContext()));
  auto proofOutput = new llvm::LoadInst(
      llvm::Type::getInt1Ty(Module->getContext()), ProofOutputFlag,
      "proof_output", CurrentBlock);
  llvm::BasicBlock *TrueBlock
      = llvm::BasicBlock::Create(Module->getContext(), "if_" + label, f);
  llvm::BasicBlock *MergeBlock
      = llvm::BasicBlock::Create(Module->getContext(), "tail_" + label, f);
  llvm::BranchInst::Create(TrueBlock, MergeBlock, proofOutput, CurrentBlock);
  return std::pair(TrueBlock, MergeBlock);
}

llvm::BasicBlock *ProofEvent::hookEvent_pre(std::string name) {
  auto b = proofBranch("hookpre");
  auto TrueBlock = b.first;
  auto MergeBlock = b.second;
  auto OutputFileName = Module->getOrInsertGlobal(
      "output_file", llvm::Type::getInt8PtrTy(Module->getContext()));
  auto ir = new llvm::IRBuilder(TrueBlock);
  auto outputFile = new llvm::LoadInst(
      llvm::Type::getInt8PtrTy(Module->getContext()), OutputFileName, "output",
      TrueBlock);

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
  auto OutputFileName = Module->getOrInsertGlobal(
      "output_file", llvm::Type::getInt8PtrTy(Module->getContext()));
  auto outputFile = new llvm::LoadInst(
      llvm::Type::getInt8PtrTy(Module->getContext()), OutputFileName, "output",
      TrueBlock);

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

  auto OutputFileName = Module->getOrInsertGlobal(
      "output_file", llvm::Type::getInt8PtrTy(Module->getContext()));
  auto outputFile = new llvm::LoadInst(
      llvm::Type::getInt8PtrTy(Module->getContext()), OutputFileName, "output",
      TrueBlock);

  emitSerializeTerm(*sort, outputFile, val, TrueBlock);

  llvm::BranchInst::Create(MergeBlock, TrueBlock);
  emitNoOp(MergeBlock);

  return CurrentBlock = MergeBlock;
}

} // namespace kllvm
