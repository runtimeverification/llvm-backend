#ifndef KLLVM_UTIL_H
#define KLLVM_UTIL_H

#include <kllvm/ast/AST.h>

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
    llvm::BasicBlock *block, llvm::Value *alloc_size, llvm::Function *malloc_f);
llvm::Instruction *create_malloc(
    llvm::Instruction *inst, llvm::Value *alloc_size, llvm::Function *malloc_f);

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

char const *get_collection_alloc_fn(sort_category cat);

void insert_call_to_clear(llvm::BasicBlock *bb);

} // namespace kllvm

#endif // KLLVM_UTIL_H
