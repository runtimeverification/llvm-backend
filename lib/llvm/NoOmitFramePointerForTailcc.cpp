#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

#define DEBUG_TYPE "NoOmitFramePointerForTailcc"

namespace {
struct NoOmitFramePointerForTailcc : public FunctionPass {
  static char ID;
  NoOmitFramePointerForTailcc()
      : FunctionPass(ID) { }

  bool runOnFunction(Function &F) override {
    if (F.getCallingConv() != CallingConv::Tail) {
      return false;
    }

    F.addFnAttr("frame-pointer", "non-leaf");
    return true;
  }
};
} // namespace

char NoOmitFramePointerForTailcc::ID = 0;

static RegisterPass<NoOmitFramePointerForTailcc>
    X("no-omit-frame-pointer-for-tailcc",
      "Mark tailcc functions with \"frame-pointer\"=\"all\" attribute",
      false /* Only looks at CFG */, false /* Analysis Pass */);
