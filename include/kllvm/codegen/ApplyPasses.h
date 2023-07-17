#ifndef APPLY_PASSES_H
#define APPLY_PASSES_H

#include <llvm/IR/Module.h>

namespace kllvm {

void apply_kllvm_opt_passes(llvm::Module &);

}

#endif
