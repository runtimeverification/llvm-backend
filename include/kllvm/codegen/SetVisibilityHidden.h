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

bool run_set_visibility_hidden(llvm::Module &m);

struct legacy_set_visibility_hidden : llvm::ModulePass {
  // NOLINTNEXTLINE(*-identifier-naming)
  static char ID;
  legacy_set_visibility_hidden()
      : llvm::ModulePass(ID) { }
  bool runOnModule(llvm::Module &m) override {
    return run_set_visibility_hidden(m);
  }
};

struct set_visibility_hidden : llvm::PassInfoMixin<set_visibility_hidden> {
  static llvm::PreservedAnalyses
  run(llvm::Module &m, llvm::ModuleAnalysisManager &) {
    if (!run_set_visibility_hidden(m)) {
      return llvm::PreservedAnalyses::all();
    }
    return llvm::PreservedAnalyses::none();
  }
};

} // namespace kllvm

#endif
