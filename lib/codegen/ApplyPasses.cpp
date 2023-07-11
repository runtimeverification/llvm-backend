#include <kllvm/codegen/ApplyPasses.h>

#include <llvm/IR/LegacyPassManager.h>
#include <llvm/Pass.h>
#include <llvm/Transforms/Scalar.h>
#include <llvm/Transforms/Utils.h>

using namespace llvm;

namespace kllvm {

void apply_kllvm_opt_passes(llvm::Module &mod) {
  auto pm = legacy::PassManager();

  pm.add(createPromoteMemoryToRegisterPass());
  pm.add(createTailCallEliminationPass());

  pm.run(mod);
}

} // namespace kllvm
