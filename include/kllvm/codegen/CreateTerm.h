#ifndef CREATE_TERM_H
#define CREATE_TERM_H

#include "kllvm/ast/AST.h"

#include "llvm/ADT/StringMap.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Value.h"

namespace kllvm {

/* Creates a new llvm::Module with the predefined declarations common to all llvm modules
   in the llvm backend. */
std::unique_ptr<llvm::Module> newModule(std::string name, llvm::LLVMContext &Context);

/* returns the llvm::Type corresponding to the type of the result of calling createTerm
   on the specified pattern. */
llvm::Type *termType(KOREPattern *pattern, llvm::StringMap<llvm::Value *> &substitution, KOREDefinition *definition, llvm::LLVMContext &Context, llvm::Module *Module);

/* adds code to the specified basic block in the specified module which constructs
   an llvm value corresponding to the specified KORE RHS pattern and substitution in the
   specified definition, and returns the value itself. */
llvm::Value *createTerm(KOREPattern *pattern, llvm::StringMap<llvm::Value *> &substitution, KOREDefinition *definition, llvm::BasicBlock *block, llvm::Module *Module);

}

#endif // CREATE_TERM_H
