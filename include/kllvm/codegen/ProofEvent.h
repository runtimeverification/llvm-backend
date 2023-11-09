#ifndef PROOF_EVENT_H
#define PROOF_EVENT_H

#include "kllvm/ast/AST.h"
#include "kllvm/codegen/DecisionParser.h"
#include "kllvm/codegen/Util.h"

#include "llvm/IR/Instructions.h"

#include <map>
#include <tuple>

namespace kllvm {

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
   * Set up a standard event prelude by creating a pair of basic blocks for the
   * proof output and continuation, then loading the output filename from its
   * global.
   *
   * Returns a triple [proof enabled, merge, output_file]; see `proofBranch` and
   * `emitGetOutputFileName`.
   */
  std::tuple<llvm::BasicBlock *, llvm::BasicBlock *, llvm::Value *>
  eventPrelude(std::string const &label, llvm::BasicBlock *insertAtEnd);

  /*
   * Emit a call that will serialize `term` to the specified `outputFile` as
   * binary KORE. This function can be called on any term, but the sort of that
   * term must be known.
   */
  llvm::CallInst *emitSerializeTerm(
      KORECompositeSort &sort, llvm::Value *outputFile, llvm::Value *term,
      llvm::BasicBlock *insertAtEnd);

  /*
   * Emit a call that will serialize `config` to the specified `outputFile` as
   * binary KORE. This function does not require a sort, but the configuration
   * passed must be a top-level configuration.
   */
  llvm::CallInst *emitSerializeConfiguration(
      llvm::Value *outputFile, llvm::Value *config,
      llvm::BasicBlock *insertAtEnd);

  /*
   * Emit a call that will serialize `value` to the specified `outputFile`.
   */
  llvm::CallInst *emitWriteUInt64(
      llvm::Value *outputFile, uint64_t value, llvm::BasicBlock *insertAtEnd);

  /*
   * Emit a call that will serialize `str` to the specified `outputFile`.
   */
  llvm::CallInst *emitWriteString(
      llvm::Value *outputFile, std::string const &str,
      llvm::BasicBlock *insertAtEnd);

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
  [[nodiscard]] llvm::BasicBlock *
  hookEvent_pre(std::string name, llvm::BasicBlock *current_block);

  [[nodiscard]] llvm::BasicBlock *hookEvent_post(
      llvm::Value *val, KORECompositeSort *sort,
      llvm::BasicBlock *current_block);

  [[nodiscard]] llvm::BasicBlock *hookArg(
      llvm::Value *val, KORECompositeSort *sort,
      llvm::BasicBlock *current_block);

  [[nodiscard]] llvm::BasicBlock *rewriteEvent(
      KOREAxiomDeclaration *axiom, llvm::Value *return_value, uint64_t arity,
      std::map<std::string, KOREVariablePattern *> vars,
      llvm::StringMap<llvm::Value *> const &subst,
      llvm::BasicBlock *current_block);

  [[nodiscard]] llvm::BasicBlock *functionEvent(
      llvm::BasicBlock *current_block, KORECompositePattern *pattern,
      std::string const &locationStack);

public:
  ProofEvent(KOREDefinition *Definition, llvm::Module *Module)
      : Definition(Definition)
      , Module(Module)
      , Ctx(Module->getContext()) { }
};

} // namespace kllvm

#endif // PROOF_EVENT_H
