#include <kllvm/codegen/MustTailDeadArgElimination.h>
#include <kllvm/codegen/RemoveDeadKFunctions.h>
#include <kllvm/codegen/SetVisibilityHidden.h>

using namespace kllvm;

/* New PM Registration */
llvm::PassPluginLibraryInfo get_kllvm_plugin_info() {
  return {
      LLVM_PLUGIN_API_VERSION, "SetVisibilityHidden", LLVM_VERSION_STRING,
      [](PassBuilder &pb) {
        pb.registerPipelineParsingCallback(
            [](StringRef name, llvm::ModulePassManager &pm,
               ArrayRef<llvm::PassBuilder::PipelineElement>) {
              if (name == "set-visibility-hidden") {
                pm.addPass(set_visibility_hidden());
                return true;
              }
              if (name == "deadargelim-musttail") {
                pm.addPass(DeadArgumentEliminationPass());
                return true;
              }
              return false;
            });
        pb.registerPipelineParsingCallback(
            [](StringRef name, llvm::FunctionPassManager &pm,
               ArrayRef<llvm::PassBuilder::PipelineElement>) {
              if (name == "remove-dead-k-functions") {
                pm.addPass(remove_dead_k_functions());
                return true;
              }
              return false;
            });
      }};
}

#ifndef LLVM_BYE_LINK_INTO_TOOLS
extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
  return get_kllvm_plugin_info();
}
#endif
