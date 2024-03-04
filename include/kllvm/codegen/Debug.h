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

void init_debug_info(llvm::Module *module, std::string const &filename);
void finalize_debug_info();

void init_debug_function(
    std::string const &name, std::string const &linkage_name,
    llvm::DISubroutineType *type, kore_definition *definition,
    llvm::Function *func);

void init_debug_axiom(attribute_set const &att);
void init_debug_param(
    llvm::Function *func, unsigned arg_no, std::string const &name,
    value_type type, std::string const &type_name);
void init_debug_global(
    std::string const &name, llvm::DIType *type, llvm::GlobalVariable *var);

llvm::DIType *get_debug_type(value_type type, std::string const &type_name);
llvm::DIType *get_int_debug_type();
llvm::DIType *get_long_debug_type();
llvm::DIType *get_void_debug_type();
llvm::DIType *get_bool_debug_type();
llvm::DIType *get_short_debug_type();
llvm::DIType *get_pointer_debug_type(llvm::DIType *, std::string const &type_name);
llvm::DIType *
get_array_debug_type(llvm::DIType *ty, size_t len, llvm::Align align);
llvm::DIType *get_char_ptr_debug_type();
llvm::DIType *get_char_debug_type();
llvm::DIType *get_forward_decl(std::string const &name);

llvm::DISubroutineType *
get_debug_function_type(llvm::Metadata *, std::vector<llvm::Metadata *>);

void set_debug_loc(llvm::Instruction *instr);
void reset_debug_loc();

} // namespace kllvm
#endif
