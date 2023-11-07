#include "kllvm/codegen/ProofEvent.h"
#include "kllvm/codegen/CreateTerm.h"

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
  auto ir = new llvm::IRBuilder(TrueBlock);
  auto outputFile = new llvm::LoadInst(
      llvm::Type::getInt8PtrTy(Module->getContext()), OutputFileName, "output",
      TrueBlock);

  auto cat = sort->getCategory(Definition);
  std::ostringstream Out;
  sort->print(Out);
  auto sortptr = ir->CreateGlobalStringPtr(Out.str(), "", 0, Module);

  writeUInt64(outputFile, Module, 0xbbbbbbbbbbbbbbbb, TrueBlock);
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
  auto ir = new llvm::IRBuilder(TrueBlock);
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

  llvm::BinaryOperator::Create(
      llvm::Instruction::Add,
      llvm::ConstantInt::get(llvm::Type::getInt8Ty(Module->getContext()), 0),
      llvm::ConstantInt::get(llvm::Type::getInt8Ty(Module->getContext()), 0),
      "nop", MergeBlock);

  return CurrentBlock = MergeBlock;
}

} // namespace kllvm
