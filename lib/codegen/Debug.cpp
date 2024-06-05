#include "kllvm/codegen/Debug.h"

#include <llvm/BinaryFormat/Dwarf.h>
#include <llvm/Config/llvm-config.h>
#include <llvm/IR/Argument.h>
#include <llvm/IR/DIBuilder.h>
#include <llvm/IR/DebugInfoMetadata.h>
#include <llvm/IR/DebugLoc.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/GlobalVariable.h>
#include <llvm/IR/Instruction.h>
#include <llvm/IR/Metadata.h>
#include <llvm/IR/Module.h>

#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <map>
#include <memory>

namespace kllvm {

static llvm::DIBuilder *dbg;
static llvm::DICompileUnit *dbg_cu;
static llvm::DIFile *dbg_file;
static llvm::DISubprogram *dbg_sp;
static unsigned dbg_line;
static unsigned dbg_column;

#define DWARF_VERSION 4

void init_debug_info(llvm::Module *module, std::string const &filename) {
  dbg = new llvm::DIBuilder(*module);
  dbg_file = dbg->createFile(filename, ".");

  module->addModuleFlag(
      llvm::Module::Warning, "Debug Info Version",
      (uint32_t)llvm::DEBUG_METADATA_VERSION);
  module->addModuleFlag(llvm::Module::Warning, "Dwarf Version", DWARF_VERSION);

  // There are no overloads for this method in DIBuilder, so we need to
  // re-specify lots of the default arguments to get to the DebugNameTableKind
  // one; for DWARFv5 objects, we don't emit a global name table to avoid
  // trampling the existing one.
  //
  // See these links for the original issue context and a reference for the
  // arguments to createCompileUnit:
  //   https://github.com/runtimeverification/k/issues/2637
  //   https://llvm.org/doxygen/classllvm_1_1DIBuilder.html
  dbg_cu = dbg->createCompileUnit(
      llvm::dwarf::DW_LANG_C, dbg_file, "llvm-kompile-codegen", false, "", 0,
      "", llvm::DICompileUnit::DebugEmissionKind::FullDebug, 0, false, false,
      llvm::DICompileUnit::DebugNameTableKind::None);
}

void finalize_debug_info() {
  dbg->finalize();
}

void init_debug_function(
    std::string const &name, std::string const &linkage_name,
    llvm::DISubroutineType *type, kore_definition *definition,
    llvm::Function *func) {
  if (!dbg) {
    return;
  }
  auto *unit
      = dbg->createFile(dbg_file->getFilename(), dbg_file->getDirectory());
  llvm::DIScope *f_context = unit;
  dbg_sp = dbg->createFunction(
      f_context, name, name, unit, dbg_line, type, dbg_line,
      llvm::DINode::DIFlags::FlagZero, llvm::DISubprogram::SPFlagDefinition);
  func->setSubprogram(dbg_sp);
}

void init_debug_param(
    llvm::Function *func, unsigned arg_no, std::string const &name,
    value_type type, std::string const &type_name) {
  if (!dbg) {
    return;
  }
  llvm::DILocalVariable *dbg_var = dbg->createParameterVariable(
      dbg_sp, name, arg_no + 1, dbg_file, dbg_line,
      get_debug_type(type, type_name), true);
  dbg->insertDbgValueIntrinsic(
      func->arg_begin() + arg_no, dbg_var, dbg->createExpression(),
      llvm::DILocation::get(func->getContext(), dbg_line, dbg_column, dbg_sp),
      &func->getEntryBlock());
}

void init_debug_global(
    std::string const &name, llvm::DIType *type, llvm::GlobalVariable *var) {
  if (!dbg) {
    return;
  }
  reset_debug_loc();
  auto *dbg_exp = dbg->createGlobalVariableExpression(
      dbg_cu, name, name, dbg_file, dbg_line, type, false);
  var->addDebugInfo(dbg_exp);
}

void init_debug_axiom(attribute_set const &att) {
  if (!dbg) {
    return;
  }
  if (!att.contains(attribute_set::key::Source)) {
    reset_debug_loc();
    return;
  }
  kore_composite_pattern *source_att
      = att.get(attribute_set::key::Source).get();
  assert(source_att->get_arguments().size() == 1);
  auto *str_pattern = dynamic_cast<kore_string_pattern *>(
      source_att->get_arguments()[0].get());
  std::string source = str_pattern->get_contents();
  if (!att.contains(attribute_set::key::Location)) {
    reset_debug_loc();
    return;
  }
  kore_composite_pattern *location_att
      = att.get(attribute_set::key::Location).get();
  assert(location_att->get_arguments().size() == 1);
  auto *str_pattern2 = dynamic_cast<kore_string_pattern *>(
      location_att->get_arguments()[0].get());
  std::string location = str_pattern2->get_contents();
  source = source.substr(7, source.length() - 8);
  size_t first_comma = location.find_first_of(',');
  dbg_line = std::stoi(location.substr(9, first_comma - 9));
  dbg_column = std::stoi(location.substr(
      first_comma + 1,
      location.find_first_of(',', first_comma + 1) - first_comma - 1));
  dbg_file = dbg->createFile(source, dbg_file->getDirectory());
}

void reset_debug_loc() {
  if (!dbg) {
    return;
  }
  dbg_line = 0;
  dbg_column = 0;
  dbg_file = dbg_cu->getFile();
}

llvm::DIType *get_forward_decl(std::string const &name) {
  if (!dbg) {
    return nullptr;
  }
  auto *unit
      = dbg->createFile(dbg_file->getFilename(), dbg_file->getDirectory());
  return dbg->createForwardDecl(
      llvm::dwarf::DW_TAG_structure_type, name, dbg_cu, unit, 0);
}

static std::string map_struct = "map";
static std::string iter_struct = "iter";
static std::string rangemap_struct = "rangemap";
static std::string list_struct = "list";
static std::string set_struct = "set";
static std::string int_struct = "__mpz_struct";
static std::string float_struct = "floating";
static std::string buffer_struct = "stringbuffer";
static std::string block_struct = "block";

llvm::DIType *get_debug_type(value_type type, std::string const &type_name) {
  if (!dbg) {
    return nullptr;
  }
  static std::map<std::string, llvm::DIType *> types;
  llvm::DIType *map = nullptr;
  llvm::DIType *mapiter = nullptr;
  llvm::DIType *rangemap = nullptr;
  llvm::DIType *list = nullptr;
  llvm::DIType *set = nullptr;
  llvm::DIType *setiter = nullptr;
  llvm::DIType *integer = nullptr;
  llvm::DIType *floating = nullptr;
  llvm::DIType *buffer = nullptr;
  llvm::DIType *boolean = nullptr;
  llvm::DIType *mint = nullptr;
  llvm::DIType *symbol = nullptr;
  if (types[type_name]) {
    return types[type_name];
  }
  switch (type.cat) {
  case sort_category::Map:
    map = get_pointer_debug_type(get_forward_decl(map_struct), type_name);
    types[type_name] = map;
    return map;
  case sort_category::MapIter:
    mapiter = get_pointer_debug_type(get_forward_decl(iter_struct), type_name);
    types[type_name] = mapiter;
    return mapiter;
  case sort_category::RangeMap:
    rangemap
        = get_pointer_debug_type(get_forward_decl(rangemap_struct), type_name);
    types[type_name] = rangemap;
    return rangemap;
  case sort_category::List:
    list = get_pointer_debug_type(get_forward_decl(list_struct), type_name);
    types[type_name] = list;
    return list;
  case sort_category::Set:
    set = get_pointer_debug_type(get_forward_decl(set_struct), type_name);
    types[type_name] = set;
    return set;
  case sort_category::SetIter:
    setiter = get_pointer_debug_type(get_forward_decl(iter_struct), type_name);
    types[type_name] = setiter;
    return setiter;
  case sort_category::Int:
    integer = get_pointer_debug_type(get_forward_decl(int_struct), type_name);
    types[type_name] = integer;
    return integer;
  case sort_category::Float:
    floating
        = get_pointer_debug_type(get_forward_decl(float_struct), type_name);
    types[type_name] = floating;
    return floating;
  case sort_category::StringBuffer:
    buffer = get_pointer_debug_type(get_forward_decl(buffer_struct), type_name);
    types[type_name] = buffer;
    return buffer;
  case sort_category::Bool:
    boolean = dbg->createBasicType(type_name, 8, llvm::dwarf::DW_ATE_boolean);
    types[type_name] = boolean;
    return boolean;
  case sort_category::MInt:
    mint = dbg->createBasicType(
        type_name, type.bits, llvm::dwarf::DW_ATE_unsigned);
    types[type_name] = mint;
    return mint;
  case sort_category::Symbol:
  case sort_category::Variable:
    symbol = get_pointer_debug_type(get_forward_decl(block_struct), type_name);
    types[type_name] = symbol;
    return symbol;
  case sort_category::Uncomputed: abort();
  }
}

llvm::DIType *get_int_debug_type() {
  if (!dbg) {
    return nullptr;
  }
  return dbg->createBasicType("uint32_t", 32, llvm::dwarf::DW_ATE_unsigned);
}

llvm::DIType *get_long_debug_type() {
  if (!dbg) {
    return nullptr;
  }
  return dbg->createBasicType("uint64_t", 64, llvm::dwarf::DW_ATE_unsigned);
}

llvm::DIType *get_bool_debug_type() {
  if (!dbg) {
    return nullptr;
  }
  return dbg->createBasicType("bool", 8, llvm::dwarf::DW_ATE_boolean);
}

llvm::DIType *get_void_debug_type() {
  return nullptr;
}

llvm::DIType *get_char_ptr_debug_type() {
  if (!dbg) {
    return nullptr;
  }
  return dbg->createPointerType(
      dbg->createBasicType("char", 8, llvm::dwarf::DW_ATE_signed_char),
      sizeof(size_t) * 8);
}

llvm::DIType *get_char_debug_type() {
  if (!dbg) {
    return nullptr;
  }
  return dbg->createBasicType("char", 8, llvm::dwarf::DW_ATE_signed_char);
}

llvm::DIType *
get_pointer_debug_type(llvm::DIType *ty, std::string const &type_name) {
  if (!dbg) {
    return nullptr;
  }
  auto *ptr_type = dbg->createPointerType(ty, sizeof(size_t) * 8);
  return dbg->createTypedef(ptr_type, type_name, dbg_file, 0, dbg_cu);
}

llvm::DIType *
get_array_debug_type(llvm::DIType *ty, size_t len, llvm::Align align) {
  if (!dbg) {
    return nullptr;
  }
  std::vector<llvm::Metadata *> subscripts;
  auto arr = dbg->getOrCreateArray(subscripts);
  return dbg->createArrayType(len, align.value(), ty, arr);
}

llvm::DIType *get_short_debug_type() {
  if (!dbg) {
    return nullptr;
  }
  return dbg->createBasicType("uint16_t", 16, llvm::dwarf::DW_ATE_unsigned);
}

llvm::DISubroutineType *get_debug_function_type(
    llvm::Metadata *return_type, std::vector<llvm::Metadata *> arg_types) {
  if (!dbg) {
    return nullptr;
  }
  arg_types.insert(arg_types.begin(), return_type);
  return dbg->createSubroutineType(dbg->getOrCreateTypeArray(arg_types));
}

void set_debug_loc(llvm::Instruction *instr) {
  if (!dbg) {
    return;
  }
  instr->setDebugLoc(llvm::DebugLoc(llvm::DILocation::get(
      instr->getContext(), dbg_line, dbg_column, dbg_sp)));
}
} // namespace kllvm
