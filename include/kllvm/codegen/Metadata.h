#ifndef KLLVM_CODEGEN_H
#define KLLVM_CODEGEN_H

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>

#include <string>

namespace kllvm {

void addKompiledDirSymbol(
    llvm::Module &mod, std::string const &dir, bool debug);

void addMutableBytesFlag(llvm::Module &mod, bool enabled, bool debug);

void addSafePartialFlag(llvm::Module &mod, bool enabled, bool debug);

} // namespace kllvm

#endif
