#ifndef KLLVM_UTIL_H
#define KLLVM_UTIL_H

#include <llvm/Config/llvm-config.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Value.h>

#include <string>

namespace kllvm {

// Returns a reference to the function declaration for a memory allocation
// function with the given name, adding a declaration to the current module if
// one does not yet exist
llvm::Function *koreHeapAlloc(std::string name, llvm::Module *module);

// getOrInsertFunction on module, aborting on failure
template <class... Ts>
llvm::Function *getOrInsertFunction(llvm::Module *module, Ts... Args) {
  auto callee = module->getOrInsertFunction(Args...).getCallee();
  auto func = llvm::dyn_cast<llvm::Function>(callee);

  if (!func) {
    abort();
  }

  return func;
}

} // namespace kllvm

#endif // KLLVM_UTIL_H
