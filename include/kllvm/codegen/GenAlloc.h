#ifndef KLLVM_ALLOC_H
#define KLLVM_ALLOC_H

#include "llvm/IR/Module.h"

namespace kllvm {

// Returns a reference to the function declaration for a memory allocation function with the given
// name, adding a declaration to the current module if one does not yet exist
llvm::Function* koreHeapAlloc(std::string name, llvm::Module *module);

// If Value is an instance of llvm::Function, cast and return. Otherwise, print errors and abort.
llvm::Function* castToFunctionOrAbort(llvm::Value* value);


llvm::Value* valueFromFunctionCallee(llvm::FunctionCallee callee);

}

#endif // KLLVM_ALLOC_H 
