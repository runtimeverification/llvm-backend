#ifndef PROOF_EVENT_H
#define PROOF_EVENT_H

#include "kllvm/ast/AST.h"
#include "kllvm/codegen/DecisionParser.h"
#include "kllvm/codegen/Util.h"

#include "llvm/IR/Instructions.h"

namespace kllvm {

llvm::CallInst *writeUInt64(
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

  /*
   * Emit a call that will serialize `term` to the specified `outputFile` as
   * binary KORE.
   */
  llvm::CallInst *emitSerializeTerm(
      KORECompositeSort &sort, llvm::Value *outputFile, llvm::Value *term,
      llvm::BasicBlock *insertAtEnd);

  /*
   * Emit a call that will serialize `value` to the specified `outputFile`.
   */
  llvm::CallInst *emitWriteUInt64(
      llvm::Value *outputFile, uint64_t value, llvm::BasicBlock *insertAtEnd);

  /* 
   * Emit an instruction that has no effect and will be removed by optimization
   * passes.
   *
   * We need this workaround because some callsites will try to use
   * llvm::Instruction::insertAfter on the back of the MergeBlock after a proof
   * branch is created. If the MergeBlock has no instructions, this has resulted
   * in a segfault when printing the IR. Adding an effective no-op prevents this.
   */
  llvm::BinaryOperator *emitNoOp(llvm::BasicBlock *insertAtEnd);

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
