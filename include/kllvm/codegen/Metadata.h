#ifndef KLLVM_CODEGEN_H
#define KLLVM_CODEGEN_H

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>

#include <string>

namespace kllvm {

void add_kompiled_dir_symbol(
    llvm::Module &mod, std::string const &dir, bool debug);

void add_mutable_bytes_flag(llvm::Module &mod, bool enabled, bool debug);

void add_safe_partial_flag(llvm::Module &mod, bool enabled, bool debug);

} // namespace kllvm

#endif
