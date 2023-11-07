#ifndef PROOF_EVENT_H
#define PROOF_EVENT_H

#include "kllvm/ast/AST.h"
#include "kllvm/codegen/DecisionParser.h"
#include "kllvm/codegen/Util.h"

#include "llvm/IR/Instructions.h"

namespace kllvm {

void writeUInt64(
    llvm::Value *outputFile, llvm::Module *Module, uint64_t value,
    llvm::BasicBlock *Block);

class ProofEvent {
private:
  KOREDefinition *Definition;
  llvm::BasicBlock *CurrentBlock;
  llvm::Module *Module;
  llvm::LLVMContext &Ctx;
  std::pair<llvm::BasicBlock *, llvm::BasicBlock *>
  proofBranch(std::string label);

public:
  llvm::BasicBlock *hookEvent_pre(std::string name);
  llvm::BasicBlock *hookEvent_post(llvm::Value *val, KORECompositeSort *sort);
  llvm::BasicBlock *hookArg(llvm::Value *val, KORECompositeSort *sort);

public:
  ProofEvent(
      KOREDefinition *Definition, llvm::BasicBlock *EntryBlock,
      llvm::Module *Module)
      : Definition(Definition)
      , CurrentBlock(EntryBlock)
      , Module(Module)
      , Ctx(Module->getContext()) { }
};

} // namespace kllvm

#endif // PROOF_EVENT_H
