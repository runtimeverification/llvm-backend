#include <kllvm/codegen/ApplyPasses.h>

#include <llvm/IR/PassManager.h>
#include <llvm/Passes/OptimizationLevel.h>
#include <llvm/Passes/PassBuilder.h>
#include <llvm/Transforms/Scalar/TailRecursionElimination.h>
#include <llvm/Transforms/Utils/Mem2Reg.h>

using namespace llvm;

namespace kllvm {

/*
 * Boilerplate pass manager setup code derived from the official LLVM
 * documentation:
 *  https://llvm.org/docs/NewPassManager.html#just-tell-me-how-to-run-the-default-optimization-pipeline-with-the-new-pass-manager
 */
void apply_kllvm_opt_passes(llvm::Module &mod) {
  auto LAM = LoopAnalysisManager();
  auto FAM = FunctionAnalysisManager();
  auto CGAM = CGSCCAnalysisManager();
  auto MAM = ModuleAnalysisManager();

  auto PB = PassBuilder();

  PB.registerModuleAnalyses(MAM);
  PB.registerCGSCCAnalyses(CGAM);
  PB.registerFunctionAnalyses(FAM);
  PB.registerLoopAnalyses(LAM);
  PB.crossRegisterProxies(LAM, FAM, CGAM, MAM);

  auto MPM = PB.buildO0DefaultPipeline(OptimizationLevel::O0);

  MPM.addPass(createModuleToFunctionPassAdaptor(PromotePass()));
  MPM.addPass(createModuleToFunctionPassAdaptor(TailCallElimPass()));

  MPM.run(mod, MAM);
}

} // namespace kllvm
