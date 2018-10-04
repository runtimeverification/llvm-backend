#include "kllvm/codegen/GenAlloc.h"

namespace kllvm {

static llvm::Function* koreHeapAllocF = nullptr;

llvm::Function* koreHeapAlloc(llvm::Module *module) {
  if (!koreHeapAllocF || koreHeapAllocF->getParent() != module) {
    llvm::Type* size_type = llvm::Type::getInt64Ty(module->getContext());
    auto allocType = llvm::FunctionType::get(llvm::Type::getInt8PtrTy(module->getContext()), llvm::ArrayRef<llvm::Type*>(size_type), false);
    koreHeapAllocF = llvm::cast<llvm::Function>(module->getOrInsertFunction("koreAlloc", allocType));
  }
  return koreHeapAllocF;
}

}
