#include <kllvm/codegen/SetVisibilityHidden.h>

using namespace llvm;

namespace kllvm {
__attribute__((visibility("default"))) bool
run_set_visibility_hidden(Module &m) {
  bool dirty = false;
  for (auto &global : m.globals()) {
    if (!global.isDeclaration()) {
      global.setVisibility(GlobalValue::HiddenVisibility);
      dirty = true;
    }
  }
  for (auto &func : m.functions()) {
    if (!func.isDeclaration()) {
      func.setVisibility(GlobalValue::HiddenVisibility);
      dirty = true;
    }
  }
  return dirty;
}

} // namespace kllvm

using namespace kllvm;

__attribute__((visibility("default"))) char legacy_set_visibility_hidden::ID
    = 0;

static RegisterPass<legacy_set_visibility_hidden>
    x("set-visibility-hidden", "Set visibility of all global values to hidden",
      false /* Only looks at CFG */, false /* Analysis Pass */);
