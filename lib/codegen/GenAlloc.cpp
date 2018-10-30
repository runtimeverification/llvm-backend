#include "kllvm/codegen/GenAlloc.h"

namespace kllvm {

llvm::Function* koreHeapAlloc(std::string name, llvm::Module *module) {
  llvm::Type* size_type = llvm::Type::getInt64Ty(module->getContext());
  auto allocType = llvm::FunctionType::get(llvm::Type::getInt8PtrTy(module->getContext()), llvm::ArrayRef<llvm::Type*>(size_type), false);
  return llvm::cast<llvm::Function>(module->getOrInsertFunction(name, allocType));
}

}
