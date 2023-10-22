#include "kllvm/codegen/CreateTerm.h"
#include "kllvm/codegen/ProofEvent.h"

#include "llvm/IR/IRBuilder.h"

namespace kllvm {

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

llvm::BasicBlock *ProofEvent::hookEvent_pre(std::string name) {
  llvm::Function *f = CurrentBlock->getParent();
  auto ProofOutputFlag = Module->getOrInsertGlobal(
      "proof_output", llvm::Type::getInt1Ty(Module->getContext()));
  auto OutputFileName = Module->getOrInsertGlobal(
      "output_file", llvm::Type::getInt8PtrTy(Module->getContext()));
  auto proofOutput = new llvm::LoadInst(
      llvm::Type::getInt1Ty(Module->getContext()), ProofOutputFlag,
      "proof_output", CurrentBlock);
  llvm::BasicBlock *TrueBlock
      = llvm::BasicBlock::Create(Module->getContext(), "if_hookpre", f);
  auto ir = new llvm::IRBuilder(TrueBlock);
  llvm::BasicBlock *MergeBlock
      = llvm::BasicBlock::Create(Module->getContext(), "tail_hookpre", f);
  llvm::BranchInst::Create(TrueBlock, MergeBlock, proofOutput, CurrentBlock);
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
  return CurrentBlock = MergeBlock;
}

llvm::BasicBlock *
ProofEvent::hookEvent_post(llvm::Value *val, KORECompositeSort *sort) {
  llvm::Function *f = CurrentBlock->getParent();
  auto ProofOutputFlag = Module->getOrInsertGlobal(
      "proof_output", llvm::Type::getInt1Ty(Module->getContext()));
  auto OutputFileName = Module->getOrInsertGlobal(
      "output_file", llvm::Type::getInt8PtrTy(Module->getContext()));
  auto proofOutput = new llvm::LoadInst(
      llvm::Type::getInt1Ty(Module->getContext()), ProofOutputFlag,
      "proof_output", CurrentBlock);
  llvm::BasicBlock *TrueBlock
      = llvm::BasicBlock::Create(Module->getContext(), "if_hookpost", f);
  auto ir = new llvm::IRBuilder(TrueBlock);
  llvm::BasicBlock *MergeBlock
      = llvm::BasicBlock::Create(Module->getContext(), "tail_hookpost", f);
  llvm::BranchInst::Create(TrueBlock, MergeBlock, proofOutput, CurrentBlock);
  auto outputFile = new llvm::LoadInst(
      llvm::Type::getInt8PtrTy(Module->getContext()), OutputFileName, "output",
      TrueBlock);

  auto cat = sort->getCategory(Definition);
  std::ostringstream Out;
  sort->print(Out);
  auto sortptr = ir->CreateGlobalStringPtr(Out.str(), "", 0, Module);

  if (cat.cat == SortCategory::Symbol || cat.cat == SortCategory::Variable) {
    ir->CreateCall(
        getOrInsertFunction(
            Module, "serializeTermToFile",
            llvm::Type::getVoidTy(Module->getContext()),
            llvm::Type::getInt8PtrTy(Module->getContext()),
            getValueType({SortCategory::Symbol, 0}, Module),
            llvm::Type::getInt8PtrTy(Module->getContext())),
        {outputFile, val, sortptr});
  } else if (val->getType()->isIntegerTy()) {
    val = ir->CreateIntToPtr(
        val, llvm::Type::getInt8PtrTy(Module->getContext()));
    ir->CreateCall(
        getOrInsertFunction(
            Module, "serializeRawTermToFile",
            llvm::Type::getVoidTy(Module->getContext()),
            llvm::Type::getInt8PtrTy(Module->getContext()),
            llvm::Type::getInt8PtrTy(Module->getContext()),
            llvm::Type::getInt8PtrTy(Module->getContext())),
        {outputFile, val, sortptr});
  } else {
    val = ir->CreatePointerCast(
        val, llvm::Type::getInt8PtrTy(Module->getContext()));
    ir->CreateCall(
        getOrInsertFunction(
            Module, "serializeRawTermToFile",
            llvm::Type::getVoidTy(Module->getContext()),
            llvm::Type::getInt8PtrTy(Module->getContext()),
            llvm::Type::getInt8PtrTy(Module->getContext()),
            llvm::Type::getInt8PtrTy(Module->getContext())),
        {outputFile, val, sortptr});
  }

  llvm::BranchInst::Create(MergeBlock, TrueBlock);
  return CurrentBlock = MergeBlock;
}

llvm::BasicBlock *
ProofEvent::hookArg(llvm::Value *val, KORECompositeSort *sort) {
  llvm::Function *f = CurrentBlock->getParent();
  auto ProofOutputFlag = Module->getOrInsertGlobal(
      "proof_output", llvm::Type::getInt1Ty(Module->getContext()));
  auto OutputFileName = Module->getOrInsertGlobal(
      "output_file", llvm::Type::getInt8PtrTy(Module->getContext()));
  auto proofOutput = new llvm::LoadInst(
      llvm::Type::getInt1Ty(Module->getContext()), ProofOutputFlag,
      "proof_output", CurrentBlock);
  llvm::BasicBlock *TrueBlock
      = llvm::BasicBlock::Create(Module->getContext(), "if_hookarg", f);
  auto ir = new llvm::IRBuilder(TrueBlock);
  llvm::BasicBlock *MergeBlock
      = llvm::BasicBlock::Create(Module->getContext(), "tail_hookarg", f);
  llvm::BranchInst::Create(TrueBlock, MergeBlock, proofOutput, CurrentBlock);
  auto outputFile = new llvm::LoadInst(
      llvm::Type::getInt8PtrTy(Module->getContext()), OutputFileName, "output",
      TrueBlock);

  auto cat = sort->getCategory(Definition);
  std::ostringstream Out;
  sort->print(Out);
  auto sortptr = ir->CreateGlobalStringPtr(Out.str(), "", 0, Module);

  if (cat.cat == SortCategory::Symbol || cat.cat == SortCategory::Variable) {
    ir->CreateCall(
        getOrInsertFunction(
            Module, "serializeTermToFile",
            llvm::Type::getVoidTy(Module->getContext()),
            llvm::Type::getInt8PtrTy(Module->getContext()),
            getValueType({SortCategory::Symbol, 0}, Module),
            llvm::Type::getInt8PtrTy(Module->getContext())),
        {outputFile, val, sortptr});
  } else if (val->getType()->isIntegerTy()) {
    val = ir->CreateIntToPtr(
        val, llvm::Type::getInt8PtrTy(Module->getContext()));
    ir->CreateCall(
        getOrInsertFunction(
            Module, "serializeRawTermToFile",
            llvm::Type::getVoidTy(Module->getContext()),
            llvm::Type::getInt8PtrTy(Module->getContext()),
            llvm::Type::getInt8PtrTy(Module->getContext()),
            llvm::Type::getInt8PtrTy(Module->getContext())),
        {outputFile, val, sortptr});
  } else {
    val = ir->CreatePointerCast(
        val, llvm::Type::getInt8PtrTy(Module->getContext()));
    ir->CreateCall(
        getOrInsertFunction(
            Module, "serializeRawTermToFile",
            llvm::Type::getVoidTy(Module->getContext()),
            llvm::Type::getInt8PtrTy(Module->getContext()),
            llvm::Type::getInt8PtrTy(Module->getContext()),
            llvm::Type::getInt8PtrTy(Module->getContext())),
        {outputFile, val, sortptr});
  }

  llvm::BranchInst::Create(MergeBlock, TrueBlock);
  return CurrentBlock = MergeBlock;
}

} // namespace kllvm
