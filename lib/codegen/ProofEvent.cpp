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

void writeUInt64(
    llvm::Value *outputFile, llvm::Module *Module, uint64_t value,
    llvm::BasicBlock *Block) {
  llvm::CallInst::Create(
      getOrInsertFunction(
          Module, "writeUInt64ToFile",
          llvm::Type::getVoidTy(Module->getContext()),
          llvm::Type::getInt8PtrTy(Module->getContext()),
          llvm::Type::getInt64Ty(Module->getContext())),
      {outputFile, llvm::ConstantInt::get(
                       llvm::Type::getInt64Ty(Module->getContext()), value)},
      "", Block);
}

llvm::CallInst *ProofEvent::emitSerializeTerm(
    KORECompositeSort &sort, llvm::Value *outputFile, llvm::Value *term,
    llvm::BasicBlock *insertAtEnd) {
  auto B = llvm::IRBuilder(insertAtEnd);

  auto cat = sort.getCategory(Definition);
  auto sort_name_ptr = createGlobalSortStringPtr(B, sort, Module);

  auto &ctx = Module->getContext();
  auto void_ty = llvm::Type::getVoidTy(ctx);
  auto i8_ptr_ty = llvm::Type::getInt8PtrTy(ctx);

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

  writeUInt64(outputFile, Module, 0xaaaaaaaaaaaaaaaa, TrueBlock);
  ir->CreateCall(
      getOrInsertFunction(
          Module, "printVariableToFile",
          llvm::Type::getVoidTy(Module->getContext()),
          llvm::Type::getInt8PtrTy(Module->getContext()),
          llvm::Type::getInt8PtrTy(Module->getContext())),
      {outputFile, nameptr});

  llvm::BranchInst::Create(MergeBlock, TrueBlock);

  // Some places will try to use llvm::Instruction::insertAfter on the back of
  // the MergeBlock.
  // If the MergeBlock has no instructions, this has resulted in a segfault when
  // printing the IR. Adding an effective nop prevents this.
  llvm::BinaryOperator::Create(
      llvm::Instruction::Add,
      llvm::ConstantInt::get(llvm::Type::getInt8Ty(Module->getContext()), 0),
      llvm::ConstantInt::get(llvm::Type::getInt8Ty(Module->getContext()), 0),
      "nop", MergeBlock);

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

  writeUInt64(outputFile, Module, 0xbbbbbbbbbbbbbbbb, TrueBlock);

  emitSerializeTerm(*sort, outputFile, val, TrueBlock);

  llvm::BranchInst::Create(MergeBlock, TrueBlock);

  llvm::BinaryOperator::Create(
      llvm::Instruction::Add,
      llvm::ConstantInt::get(llvm::Type::getInt8Ty(Module->getContext()), 0),
      llvm::ConstantInt::get(llvm::Type::getInt8Ty(Module->getContext()), 0),
      "nop", MergeBlock);

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

  llvm::BinaryOperator::Create(
      llvm::Instruction::Add,
      llvm::ConstantInt::get(llvm::Type::getInt8Ty(Module->getContext()), 0),
      llvm::ConstantInt::get(llvm::Type::getInt8Ty(Module->getContext()), 0),
      "nop", MergeBlock);

  return CurrentBlock = MergeBlock;
}

} // namespace kllvm
