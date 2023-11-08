#ifndef PROOF_EVENT_H
#define PROOF_EVENT_H

#include "kllvm/ast/AST.h"
#include "kllvm/codegen/DecisionParser.h"
#include "kllvm/codegen/Util.h"

#include "llvm/IR/Instructions.h"

#include <map>

namespace kllvm {

llvm::CallInst *writeUInt64(
    llvm::Value *outputFile, llvm::Module *Module, uint64_t value,
    llvm::BasicBlock *Block);

class ProofEvent {
private:
  KOREDefinition *Definition;
  llvm::Module *Module;
  llvm::LLVMContext &Ctx;

  /*
   * Load the boolean flag that controls whether proof hint output is enabled or
   * not, then create a branch at the end of this basic block depending on the
   * result.
   *
   * Returns a pair of blocks [proof enabled, merge]; the first of these is
   * intended for self-contained behaviour only relevant in proof output mode,
   * while the second is for the continuation of the interpreter's previous
   * behaviour.
   */
  std::pair<llvm::BasicBlock *, llvm::BasicBlock *>
  proofBranch(std::string const &label, llvm::BasicBlock *insertAtEnd);

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

  /*
   * Emit instructions to load the path of the interpreter's current output
   * file; used here for binary proof trace data.
   */
  llvm::LoadInst *emitGetOutputFileName(llvm::BasicBlock *insertAtEnd);

public:
  llvm::BasicBlock *
  hookEvent_pre(std::string name, llvm::BasicBlock *current_block);

  llvm::BasicBlock *hookEvent_post(
      llvm::Value *val, KORECompositeSort *sort,
      llvm::BasicBlock *current_block);

  llvm::BasicBlock *hookArg(
      llvm::Value *val, KORECompositeSort *sort,
      llvm::BasicBlock *current_block);

  llvm::BasicBlock *rewriteEvent(
      KOREAxiomDeclaration *axiom, llvm::Value *return_value, uint64_t arity,
      std::map<std::string, KOREVariablePattern *> vars,
      llvm::StringMap<llvm::Value *> const &subst,
      llvm::BasicBlock *current_block);

public:
  ProofEvent(KOREDefinition *Definition, llvm::Module *Module)
      : Definition(Definition)
      , Module(Module)
      , Ctx(Module->getContext()) { }
};

} // namespace kllvm

#endif // PROOF_EVENT_H
