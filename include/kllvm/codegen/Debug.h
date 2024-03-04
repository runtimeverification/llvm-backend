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

void initDebugInfo(llvm::Module *module, std::string const &filename);
void finalizeDebugInfo();

void initDebugFunction(
    std::string const &name, std::string const &linkage_name,
    llvm::DISubroutineType *type, kore_definition *definition,
    llvm::Function *func);

void initDebugAxiom(attribute_set const &att);
void initDebugParam(
    llvm::Function *func, unsigned arg_no, std::string const &name,
    value_type type, std::string const &type_name);
void initDebugGlobal(
    std::string const &name, llvm::DIType *type, llvm::GlobalVariable *var);

llvm::DIType *getDebugType(value_type type, std::string const &type_name);
llvm::DIType *getIntDebugType();
llvm::DIType *getLongDebugType();
llvm::DIType *getVoidDebugType();
llvm::DIType *getBoolDebugType();
llvm::DIType *getShortDebugType();
llvm::DIType *getPointerDebugType(llvm::DIType *, std::string const &type_name);
llvm::DIType *
getArrayDebugType(llvm::DIType *ty, size_t len, llvm::Align align);
llvm::DIType *getCharPtrDebugType();
llvm::DIType *getCharDebugType();
llvm::DIType *getForwardDecl(std::string const &name);

llvm::DISubroutineType *
getDebugFunctionType(llvm::Metadata *, std::vector<llvm::Metadata *>);

void setDebugLoc(llvm::Instruction *instr);
void resetDebugLoc();

} // namespace kllvm
#endif
