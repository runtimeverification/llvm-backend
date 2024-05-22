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

llvm::Function *kore_heap_alloc(std::string const &name, llvm::Module *module) {
  llvm::Type *size_type = llvm::Type::getInt64Ty(module->getContext());
  auto *alloc_type = llvm::FunctionType::get(
      llvm::PointerType::getUnqual(module->getContext()),
      llvm::ArrayRef<llvm::Type *>(size_type), false);
  return get_or_insert_function(module, name, alloc_type);
}

llvm::Constant *get_offset_of_member(
    [[maybe_unused]] llvm::Module *mod, llvm::StructType *struct_ty,
    int nth_member) {
#if LLVM_VERSION_MAJOR >= 17
  auto offset
      = llvm::DataLayout(mod).getStructLayout(struct_ty)->getElementOffset(
          nth_member);
  auto *offset_ty = llvm::Type::getInt64Ty(mod->getContext());
  return llvm::ConstantInt::get(offset_ty, offset);
#else
  return llvm::ConstantExpr::getOffsetOf(struct_ty, nth_member);
#endif
}

} // namespace kllvm
