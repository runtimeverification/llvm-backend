#ifndef DEBUG_H
#define DEBUG_H

#include "kllvm/ast/AST.h"

#include <llvm/IR/DebugInfoMetadata.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/GlobalVariable.h>
#include <llvm/IR/Instruction.h>
#include <llvm/IR/Metadata.h>
#include <llvm/IR/Module.h>

#include <cstddef>
#include <string>
#include <unordered_map>
#include <vector>

namespace kllvm {

extern std::string SOURCE_ATT;
extern std::string LOCATION_ATT;

void initDebugInfo(llvm::Module *module, std::string const &filename);
void finalizeDebugInfo(void);

void initDebugFunction(
    std::string const &name, std::string const &linkageName,
    llvm::DISubroutineType *type, KOREDefinition *definition,
    llvm::Function *func);

void initDebugAxiom(
    std::unordered_map<std::string, sptr<KORECompositePattern>> const &att);
void initDebugParam(
    llvm::Function *func, unsigned argNo, std::string const &name,
    ValueType type, std::string const &typeName);
void initDebugGlobal(
    std::string const &name, llvm::DIType *type, llvm::GlobalVariable *var);

llvm::DIType *getDebugType(ValueType type, std::string const &typeName);
llvm::DIType *getIntDebugType(void);
llvm::DIType *getLongDebugType(void);
llvm::DIType *getVoidDebugType(void);
llvm::DIType *getBoolDebugType(void);
llvm::DIType *getShortDebugType(void);
llvm::DIType *getPointerDebugType(llvm::DIType *, std::string const &typeName);
llvm::DIType *
getArrayDebugType(llvm::DIType *ty, size_t len, llvm::Align align);
llvm::DIType *getCharPtrDebugType(void);
llvm::DIType *getCharDebugType(void);
llvm::DIType *getForwardDecl(std::string const &name);

llvm::DISubroutineType *
getDebugFunctionType(llvm::Metadata *, std::vector<llvm::Metadata *>);

void setDebugLoc(llvm::Instruction *instr);
void resetDebugLoc(void);

} // namespace kllvm
#endif
