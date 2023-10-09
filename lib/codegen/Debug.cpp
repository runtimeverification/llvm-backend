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

int CODEGEN_DEBUG;

std::string SOURCE_ATT = "org'Stop'kframework'Stop'attributes'Stop'Source";
std::string LOCATION_ATT = "org'Stop'kframework'Stop'attributes'Stop'Location";

static llvm::DIBuilder *Dbg;
static llvm::DICompileUnit *DbgCU;
static llvm::DIFile *DbgFile;
static llvm::DISubprogram *DbgSP;
static unsigned DbgLine;
static unsigned DbgColumn;

#define DWARF_VERSION 4

void initDebugInfo(llvm::Module *module, std::string filename) {
  Dbg = new llvm::DIBuilder(*module);
  DbgFile = Dbg->createFile(filename, ".");

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
  DbgCU = Dbg->createCompileUnit(
      llvm::dwarf::DW_LANG_C, DbgFile, "llvm-kompile-codegen", false, "", 0, "",
      llvm::DICompileUnit::DebugEmissionKind::FullDebug, 0, false, false,
      llvm::DICompileUnit::DebugNameTableKind::None);
}

void finalizeDebugInfo(void) {
  Dbg->finalize();
}

void initDebugFunction(
    std::string name, std::string linkageName, llvm::DISubroutineType *type,
    KOREDefinition *definition, llvm::Function *func) {
  if (!Dbg)
    return;
  auto Unit = Dbg->createFile(DbgFile->getFilename(), DbgFile->getDirectory());
  llvm::DIScope *FContext = Unit;
#if LLVM_VERSION_MAJOR >= 8
  DbgSP = Dbg->createFunction(
      FContext, name, name, Unit, DbgLine, type, DbgLine,
      llvm::DINode::DIFlags::FlagZero, llvm::DISubprogram::SPFlagDefinition);
#else
  DbgSP = Dbg->createFunction(
      FContext, name, name, Unit, DbgLine, type, false, true, DbgLine);
#endif
  func->setSubprogram(DbgSP);
}

void initDebugParam(
    llvm::Function *func, unsigned argNo, std::string name, ValueType type,
    std::string typeName) {
  if (!Dbg)
    return;
  llvm::DILocalVariable *DbgVar = Dbg->createParameterVariable(
      DbgSP, name, argNo + 1, DbgFile, DbgLine, getDebugType(type, typeName),
      true);
  Dbg->insertDbgValueIntrinsic(
      func->arg_begin() + argNo, DbgVar, Dbg->createExpression(),
      llvm::DILocation::get(func->getContext(), DbgLine, DbgColumn, DbgSP),
      &func->getEntryBlock());
}

void initDebugGlobal(
    std::string name, llvm::DIType *type, llvm::GlobalVariable *var) {
  if (!Dbg)
    return;
  resetDebugLoc();
  auto DbgExp = Dbg->createGlobalVariableExpression(
      DbgCU, name, name, DbgFile, DbgLine, type, false);
  var->addDebugInfo(DbgExp);
}

void initDebugAxiom(
    std::unordered_map<std::string, sptr<KORECompositePattern>> const &att) {
  if (!Dbg)
    return;
  if (!att.count(SOURCE_ATT)) {
    resetDebugLoc();
    return;
  }
  KORECompositePattern *sourceAtt = att.at(SOURCE_ATT).get();
  assert(sourceAtt->getArguments().size() == 1);
  auto strPattern
      = dynamic_cast<KOREStringPattern *>(sourceAtt->getArguments()[0].get());
  std::string source = strPattern->getContents();
  if (!att.count(LOCATION_ATT)) {
    resetDebugLoc();
    return;
  }
  KORECompositePattern *locationAtt = att.at(LOCATION_ATT).get();
  assert(locationAtt->getArguments().size() == 1);
  auto strPattern2
      = dynamic_cast<KOREStringPattern *>(locationAtt->getArguments()[0].get());
  std::string location = strPattern2->getContents();
  source = source.substr(7, source.length() - 8);
  size_t first_comma = location.find_first_of(',');
  DbgLine = std::stoi(location.substr(9, first_comma - 9));
  DbgColumn = std::stoi(location.substr(
      first_comma + 1,
      location.find_first_of(',', first_comma + 1) - first_comma - 1));
  DbgFile = Dbg->createFile(source, DbgFile->getDirectory());
}

void resetDebugLoc(void) {
  if (!Dbg)
    return;
  DbgLine = 0;
  DbgColumn = 0;
  DbgFile = DbgCU->getFile();
}

llvm::DIType *getForwardDecl(std::string name) {
  if (!Dbg)
    return nullptr;
  auto Unit = Dbg->createFile(DbgFile->getFilename(), DbgFile->getDirectory());
  return Dbg->createForwardDecl(
      llvm::dwarf::DW_TAG_structure_type, name, DbgCU, Unit, 0);
}

static std::string MAP_STRUCT = "map";
static std::string RANGEMAP_STRUCT = "rangemap";
static std::string LIST_STRUCT = "list";
static std::string SET_STRUCT = "set";
static std::string INT_STRUCT = "__mpz_struct";
static std::string FLOAT_STRUCT = "floating";
static std::string BUFFER_STRUCT = "stringbuffer";
static std::string BLOCK_STRUCT = "block";

llvm::DIType *getDebugType(ValueType type, std::string typeName) {
  if (!Dbg)
    return nullptr;
  static std::map<std::string, llvm::DIType *> types;
  llvm::DIType *map, *rangemap, *list, *set, *integer, *floating, *buffer,
      *boolean, *mint, *symbol;
  if (types[typeName]) {
    return types[typeName];
  }
  switch (type.cat) {
  case SortCategory::Map:
    map = getPointerDebugType(getForwardDecl(MAP_STRUCT), typeName);
    types[typeName] = map;
    return map;
  case SortCategory::RangeMap:
    rangemap = getPointerDebugType(getForwardDecl(RANGEMAP_STRUCT), typeName);
    types[typeName] = rangemap;
    return rangemap;
  case SortCategory::List:
    list = getPointerDebugType(getForwardDecl(LIST_STRUCT), typeName);
    types[typeName] = list;
    return list;
  case SortCategory::Set:
    set = getPointerDebugType(getForwardDecl(SET_STRUCT), typeName);
    types[typeName] = set;
    return set;
  case SortCategory::Int:
    integer = getPointerDebugType(getForwardDecl(INT_STRUCT), typeName);
    types[typeName] = integer;
    return integer;
  case SortCategory::Float:
    floating = getPointerDebugType(getForwardDecl(FLOAT_STRUCT), typeName);
    types[typeName] = floating;
    return floating;
  case SortCategory::StringBuffer:
    buffer = getPointerDebugType(getForwardDecl(BUFFER_STRUCT), typeName);
    types[typeName] = buffer;
    return buffer;
  case SortCategory::Bool:
    boolean = Dbg->createBasicType(typeName, 8, llvm::dwarf::DW_ATE_boolean);
    types[typeName] = boolean;
    return boolean;
  case SortCategory::MInt:
    mint = Dbg->createBasicType(
        typeName, type.bits, llvm::dwarf::DW_ATE_unsigned);
    types[typeName] = mint;
    return mint;
  case SortCategory::Symbol:
  case SortCategory::Variable:
    symbol = getPointerDebugType(getForwardDecl(BLOCK_STRUCT), typeName);
    types[typeName] = symbol;
    return symbol;
  case SortCategory::Uncomputed: abort();
  }
}

llvm::DIType *getIntDebugType(void) {
  if (!Dbg)
    return nullptr;
  return Dbg->createBasicType("uint32_t", 32, llvm::dwarf::DW_ATE_unsigned);
}

llvm::DIType *getLongDebugType(void) {
  if (!Dbg)
    return nullptr;
  return Dbg->createBasicType("uint64_t", 64, llvm::dwarf::DW_ATE_unsigned);
}

llvm::DIType *getBoolDebugType(void) {
  if (!Dbg)
    return nullptr;
  return Dbg->createBasicType("bool", 8, llvm::dwarf::DW_ATE_boolean);
}

llvm::DIType *getVoidDebugType(void) {
  return nullptr;
}

llvm::DIType *getCharPtrDebugType(void) {
  if (!Dbg)
    return nullptr;
  return Dbg->createPointerType(
      Dbg->createBasicType("char", 8, llvm::dwarf::DW_ATE_signed_char),
      sizeof(size_t) * 8);
}

llvm::DIType *getCharDebugType(void) {
  if (!Dbg)
    return nullptr;
  return Dbg->createBasicType("char", 8, llvm::dwarf::DW_ATE_signed_char);
}

llvm::DIType *getPointerDebugType(llvm::DIType *ty, std::string typeName) {
  if (!Dbg)
    return nullptr;
  auto ptrType = Dbg->createPointerType(ty, sizeof(size_t) * 8);
  return Dbg->createTypedef(ptrType, typeName, DbgFile, 0, DbgCU);
}

llvm::DIType *
getArrayDebugType(llvm::DIType *ty, size_t len, llvm::Align align) {
  if (!Dbg)
    return nullptr;
  std::vector<llvm::Metadata *> subscripts;
  auto arr = Dbg->getOrCreateArray(subscripts);
  return Dbg->createArrayType(len, align.value(), ty, arr);
}

llvm::DIType *getShortDebugType(void) {
  if (!Dbg)
    return nullptr;
  return Dbg->createBasicType("uint16_t", 16, llvm::dwarf::DW_ATE_unsigned);
}

llvm::DISubroutineType *getDebugFunctionType(
    llvm::Metadata *returnType, std::vector<llvm::Metadata *> argTypes) {
  if (!Dbg)
    return nullptr;
  argTypes.insert(argTypes.begin(), returnType);
  return Dbg->createSubroutineType(Dbg->getOrCreateTypeArray(argTypes));
}

void setDebugLoc(llvm::Instruction *instr) {
  if (!Dbg)
    return;
  instr->setDebugLoc(llvm::DebugLoc(
      llvm::DILocation::get(instr->getContext(), DbgLine, DbgColumn, DbgSP)));
}
} // namespace kllvm
