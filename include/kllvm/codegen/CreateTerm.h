#ifndef CREATE_TERM_H
#define CREATE_TERM_H

#include "kllvm/ast/AST.h"

#include "llvm/ADT/StringMap.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Value.h"

namespace kllvm {

std::unique_ptr<llvm::Module> newModule(std::string name, llvm::LLVMContext &Context);

llvm::Type *termType(KOREPattern *pattern, llvm::StringMap<llvm::Value *> &substitution, KOREDefinition *definition, llvm::LLVMContext &Context, llvm::Module *Module);

llvm::Value *createTerm(KOREPattern *pattern, llvm::StringMap<llvm::Value *> &substitution, KOREDefinition *definition, llvm::BasicBlock *block, llvm::Module *Module);

}

#endif // CREATE_TERM_H
