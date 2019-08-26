#ifndef DEBUG_H
#define DEBUG_H

#include "llvm/IR/Module.h"
#include "llvm/IR/DebugInfoMetadata.h"
#include "kllvm/ast/AST.h"

namespace kllvm {

extern int CODEGEN_DEBUG;

void initDebugInfo(llvm::Module *module, std::string filename);
void finalizeDebugInfo(void);

void initDebugFunction(std::string name, std::string linkageName, llvm::DISubroutineType *type, KOREDefinition *definition, llvm::Function *func);

void initDebugAxiom(KOREAxiomDeclaration *axiom);
void initDebugParam(llvm::Function *func, unsigned argNo, std::string name, ValueType type);

llvm::DIType *getDebugType(ValueType type);

llvm::DISubroutineType *getDebugFunctionType(llvm::Metadata *, std::vector<llvm::Metadata *>);

void setDebugLoc(llvm::Instruction *instr);
void resetDebugLoc(void);

}
#endif
