#ifndef KLLVM_ALLOC_H
#define KLLVM_ALLOC_H

#include "llvm/IR/Module.h"

namespace kllvm {

llvm::Function* koreHeapAlloc(llvm::Module *module);

}
#endif // KLLVM_ALLOC_H 
