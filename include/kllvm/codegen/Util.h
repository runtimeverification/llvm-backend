#ifndef KLLVM_UTIL_H
#define KLLVM_UTIL_H

#include <llvm/Config/llvm-config.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Value.h>

#include <string>

namespace kllvm {

llvm::Constant *get_offset_of_member(llvm::Module *, llvm::StructType *, int);

// Returns a reference to the function declaration for a memory allocation
// function with the given name, adding a declaration to the current module if
// one does not yet exist
llvm::Function *kore_heap_alloc(std::string const &name, llvm::Module *module);

llvm::Instruction *create_malloc(
    llvm::BasicBlock *block, llvm::Type *int_ptr_ty, llvm::Type *alloc_ty,
    llvm::Value *alloc_size, llvm::Value *array_size, llvm::Function *malloc_f,
    std::string const &name = "");

// getOrInsertFunction on module, aborting on failure
template <class... Ts>
llvm::Function *get_or_insert_function(llvm::Module *module, Ts &&...args) {
  auto callee
      = module->getOrInsertFunction(std::forward<Ts>(args)...).getCallee();
  auto func = llvm::dyn_cast<llvm::Function>(callee);

  if (!func) {
    abort();
  }

  return func;
}

} // namespace kllvm

#endif // KLLVM_UTIL_H
