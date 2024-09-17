#ifndef REMOVE_DEAD_K_FUNCTIONS_H
#define REMOVE_DEAD_K_FUNCTIONS_H

#include "llvm/IR/Function.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Pass.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

namespace kllvm {

bool run_remove_dead_k_functions(llvm::Function &f, TargetLibraryInfo *tli);

struct legacy_remove_dead_k_functions : llvm::FunctionPass {
  // NOLINTNEXTLINE(*-identifier-naming)
  static char ID;
  legacy_remove_dead_k_functions()
      : llvm::FunctionPass(ID) { }
  bool runOnFunction(llvm::Function &f) override {
    TargetLibraryInfo *tli
        = &getAnalysis<TargetLibraryInfoWrapperPass>().getTLI(f);
    return run_remove_dead_k_functions(f, tli);
  }
};

struct remove_dead_k_functions : llvm::PassInfoMixin<remove_dead_k_functions> {
  static llvm::PreservedAnalyses
  run(llvm::Function &f, llvm::FunctionAnalysisManager &am) {
    if (!run_remove_dead_k_functions(
            f, &am.getResult<TargetLibraryAnalysis>(f))) {
      return llvm::PreservedAnalyses::all();
    }
    llvm::PreservedAnalyses pa;
    pa.preserveSet<llvm::CFGAnalyses>();
    return pa;
  }
};

} // namespace kllvm

#endif
