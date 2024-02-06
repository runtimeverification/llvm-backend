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

llvm::Function *koreHeapAlloc(std::string const &name, llvm::Module *module) {
  llvm::Type *size_type = llvm::Type::getInt64Ty(module->getContext());
  auto *allocType = llvm::FunctionType::get(
      llvm::Type::getInt8PtrTy(module->getContext()),
      llvm::ArrayRef<llvm::Type *>(size_type), false);
  return getOrInsertFunction(module, name, allocType);
}

} // namespace kllvm
