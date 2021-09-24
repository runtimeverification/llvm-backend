#include "llvm/IR/Instructions.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Statepoint.h"
#include "llvm/Support/raw_ostream.h"
#include "kllvm/ast/AST.h"
#include <cstdlib>
#include <vector>

using namespace llvm;
using namespace kllvm;

#define DEBUG_TYPE "MarkTailCallsAsGCLeaf"

namespace {
  struct MarkTailCallsAsGCLeaf : public FunctionPass {
    static char ID;
    MarkTailCallsAsGCLeaf() : FunctionPass(ID) {}

    bool runOnFunction(Function &F) override {
      LLVMContext &Ctx = F.getContext();
      if (F.getCallingConv() != CallingConv::Tail) {
        return false;
      }
      bool dirty = false;
      for (BasicBlock &BB : F.getBasicBlockList()) {
        Instruction *TI = BB.getTerminator();
        if (auto R = dyn_cast<ReturnInst>(TI)) {
          if (&BB.front() == TI) {
            continue;
          }
          BasicBlock::iterator BBI(TI);
          --BBI;
          if (auto CI = dyn_cast<CallInst>(BBI)) {
            Value *RV = R->getReturnValue();
            if (RV != nullptr && RV != CI) {
              continue;
            }
            if (!CI->isTailCall() || CI->getCallingConv() != CallingConv::Tail) {
              continue;
            }
            CI->setAttributes(CI->getAttributes().addAttribute(Ctx, llvm::AttributeList::FunctionIndex, "gc-leaf-function"));
            dirty = true;
          }
        }
      }
      return dirty;
    }
  };
}

char MarkTailCallsAsGCLeaf::ID = 0;

static RegisterPass<MarkTailCallsAsGCLeaf> X("mark-tail-calls-as-gc-leaf", "Mark tailcc tail calls with gc-leaf-function attribute",
                             false /* Only looks at CFG */,
                             false /* Analysis Pass */);
