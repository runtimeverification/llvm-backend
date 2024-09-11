#include <kllvm/codegen/SetVisibilityHidden.h>

using namespace kllvm;

/* New PM Registration */
llvm::PassPluginLibraryInfo get_kllvm_plugin_info() {
  return {
      LLVM_PLUGIN_API_VERSION, "SetVisibilityHidden", LLVM_VERSION_STRING,
      [](PassBuilder &pb) {
        pb.registerPipelineStartEPCallback(
            [](llvm::ModulePassManager &pm, OptimizationLevel level) {
              pm.addPass(set_visibility_hidden());
            });
        pb.registerPipelineParsingCallback(
            [](StringRef name, llvm::ModulePassManager &pm,
               ArrayRef<llvm::PassBuilder::PipelineElement>) {
              if (name == "set-visibility-hidden") {
                pm.addPass(set_visibility_hidden());
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
