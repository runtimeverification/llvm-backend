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

void initDebugAxiom(std::map<std::string, ptr<KORECompositePattern>> const& att);
void initDebugParam(llvm::Function *func, unsigned argNo, std::string name, ValueType type);
void initDebugGlobal(std::string name, llvm::DIType *type, llvm::GlobalVariable *var);

llvm::DIType *getDebugType(ValueType type);
llvm::DIType *getIntDebugType(void);
llvm::DIType *getBoolDebugType(void);
llvm::DIType *getShortDebugType(void);
llvm::DIType *getPointerDebugType(llvm::DIType *);
llvm::DIType *getArrayDebugType(llvm::DIType *ty, size_t len, size_t align);
llvm::DIType *getCharPtrDebugType(void);
llvm::DIType *getForwardDecl(std::string name);

llvm::DISubroutineType *getDebugFunctionType(llvm::Metadata *, std::vector<llvm::Metadata *>);

void setDebugLoc(llvm::Instruction *instr);
void resetDebugLoc(void);

}
#endif
