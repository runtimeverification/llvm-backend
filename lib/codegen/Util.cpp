#include "kllvm/codegen/Util.h"

#include <llvm/ADT/ArrayRef.h>
#include <llvm/Config/llvm-config.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Value.h>
#include <llvm/Support/Casting.h>
#include <llvm/Support/raw_ostream.h>

#include <cstdlib>

namespace kllvm {

llvm::Function *koreHeapAlloc(std::string name, llvm::Module *module) {
  llvm::Type *size_type = llvm::Type::getInt64Ty(module->getContext());
  auto allocType = llvm::FunctionType::get(
      llvm::Type::getInt8PtrTy(module->getContext()),
      llvm::ArrayRef<llvm::Type *>(size_type), false);
  return getOrInsertFunction(module, name, allocType);
}

llvm::Function *castToFunctionOrAbort(llvm::Value *value) {
  llvm::Function *func = llvm::dyn_cast<llvm::Function>(value);
  if (!func) {
    value->print(llvm::errs());
    abort();
  }
  return func;
}

llvm::StructType *getTypeByName(llvm::Module *module, std::string name) {
  llvm::StructType *t;
#if LLVM_VERSION_MAJOR >= 12
  t = llvm::StructType::getTypeByName(module->getContext(), name);
#else
  t = module->getTypeByName(name);
#endif
  return t;
}

void insertAtBack(llvm::BasicBlock *block, llvm::Instruction *inst) {
#if LLVM_VERSION_MAJOR >= 16
  inst->insertInto(block, block->end());
#else
  block->getInstList().push_back(inst);
#endif
}

} // namespace kllvm
