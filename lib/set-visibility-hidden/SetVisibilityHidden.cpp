#include <kllvm/codegen/SetVisibilityHidden.h>

using namespace llvm;

namespace kllvm {
__attribute__((visibility("default"))) bool runSetVisibilityHidden(Module &M) {
  bool dirty = false;
  for (auto &global : M.globals()) {
    if (!global.isDeclaration()) {
      global.setVisibility(GlobalValue::HiddenVisibility);
      dirty = true;
    }
  }
  for (auto &func : M.functions()) {
    if (!func.isDeclaration()) {
      func.setVisibility(GlobalValue::HiddenVisibility);
      dirty = true;
    }
  }
  return dirty;
}

} // namespace kllvm

using namespace kllvm;

__attribute__((visibility("default"))) char LegacySetVisibilityHidden::ID = 0;

static RegisterPass<LegacySetVisibilityHidden>
    X("set-visibility-hidden", "Set visibility of all global values to hidden",
      false /* Only looks at CFG */, false /* Analysis Pass */);

/* New PM Registration */
llvm::PassPluginLibraryInfo getSetVisibilityHiddenPluginInfo() {
  return {
      LLVM_PLUGIN_API_VERSION, "SetVisibilityHidden", LLVM_VERSION_STRING,
      [](PassBuilder &PB) {
        PB.registerPipelineStartEPCallback(
            [](llvm::ModulePassManager &PM, OptimizationLevel Level) {
              PM.addPass(SetVisibilityHidden());
            });
        PB.registerPipelineParsingCallback(
            [](StringRef Name, llvm::ModulePassManager &PM,
               ArrayRef<llvm::PassBuilder::PipelineElement>) {
              if (Name == "set-visibility-hidden") {
                PM.addPass(SetVisibilityHidden());
                return true;
              }
              return false;
            });
      }};
}

#ifndef LLVM_BYE_LINK_INTO_TOOLS
extern "C" __attribute__((visibility("default")))
LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
  return getSetVisibilityHiddenPluginInfo();
}
#endif
