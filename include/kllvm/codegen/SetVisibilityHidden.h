#ifndef SET_VISIBILITY_HIDDEN_H
#define SET_VISIBILITY_HIDDEN_H

#include "llvm/IR/Function.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Pass.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/raw_ostream.h"

namespace kllvm {

bool runSetVisibilityHidden(llvm::Module &M);

struct LegacySetVisibilityHidden : llvm::ModulePass {
  static char ID;
  LegacySetVisibilityHidden()
      : llvm::ModulePass(ID) { }
  bool runOnModule(llvm::Module &M) override {
    return runSetVisibilityHidden(M);
  }
};

struct SetVisibilityHidden : llvm::PassInfoMixin<SetVisibilityHidden> {
  llvm::PreservedAnalyses run(llvm::Module &M, llvm::ModuleAnalysisManager &) {
    if (!runSetVisibilityHidden(M))
      return llvm::PreservedAnalyses::all();
    return llvm::PreservedAnalyses::none();
  }
};

} // namespace kllvm

#endif
