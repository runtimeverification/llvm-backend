#ifndef APPLY_PASSES_H
#define APPLY_PASSES_H

#include <llvm/IR/Module.h>
#include <llvm/Support/raw_ostream.h>

namespace kllvm {

void do_bitcode_linking(llvm::Module &);

void apply_kllvm_opt_passes(llvm::Module &, bool hidden_visibility);

void generate_object_file(llvm::Module &, llvm::raw_ostream &);

} // namespace kllvm

#endif
