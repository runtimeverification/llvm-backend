#ifndef EMIT_CONFIG_PARSER_H
#define EMIT_CONFIG_PARSER_H

#include "kllvm/codegen/EmitConfigParser.h"

#include "kllvm/ast/AST.h"
#include "kllvm/codegen/CreateTerm.h"
#include "kllvm/codegen/Debug.h"
#include "kllvm/codegen/Util.h"

#include <llvm/ADT/StringMap.h>
#include <llvm/ADT/StringRef.h>
#include <llvm/ADT/Twine.h>
#include <llvm/IR/Argument.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Constant.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/DataLayout.h>
#include <llvm/IR/DebugInfoMetadata.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/GlobalVariable.h>
#include <llvm/IR/InstrTypes.h>
#include <llvm/IR/Instruction.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Metadata.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Value.h>
#include <llvm/Support/Casting.h>

#include <fmt/format.h>

#include <cstdint>
#include <cstdlib>
#include <iosfwd>
#include <iostream>
#include <iterator>
#include <map>
#include <memory>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <vector>

namespace kllvm {

namespace detail {

template <typename Getter>
void emitDataTableForSymbol(
    std::string name, llvm::Type *ty, llvm::DIType *dity,
    KOREDefinition *definition, llvm::Module *module, Getter &&getter) {
  llvm::LLVMContext &Ctx = module->getContext();
  std::vector<llvm::Type *> argTypes;
  argTypes.push_back(llvm::Type::getInt32Ty(Ctx));
  auto func = getOrInsertFunction(
      module, name, llvm::FunctionType::get(ty, argTypes, false));
  initDebugFunction(
      name, name, getDebugFunctionType(dity, {getIntDebugType()}), definition,
      func);
  auto EntryBlock = llvm::BasicBlock::Create(Ctx, "entry", func);
  auto MergeBlock = llvm::BasicBlock::Create(Ctx, "exit");
  auto stuck = llvm::BasicBlock::Create(Ctx, "stuck");
  auto &syms = definition->getSymbols();
  auto icmp = new llvm::ICmpInst(
      *EntryBlock, llvm::CmpInst::ICMP_ULE, func->arg_begin(),
      llvm::ConstantInt::get(
          llvm::Type::getInt32Ty(Ctx), syms.rbegin()->first));
  llvm::BranchInst::Create(MergeBlock, stuck, icmp, EntryBlock);
  auto tableType = llvm::ArrayType::get(ty, syms.size());
  auto table = module->getOrInsertGlobal("table_" + name, tableType);
  llvm::GlobalVariable *globalVar = llvm::dyn_cast<llvm::GlobalVariable>(table);
  initDebugGlobal(
      "table_" + name,
      getArrayDebugType(
          dity, syms.size(), llvm::DataLayout(module).getABITypeAlign(ty)),
      globalVar);
  std::vector<llvm::Constant *> values;
  for (auto iter = syms.begin(); iter != syms.end(); ++iter) {
    auto entry = *iter;
    auto symbol = entry.second;
    auto val = std::forward<Getter>(getter)(definition, module, symbol);
    values.push_back(val);
  }
  if (!globalVar->hasInitializer()) {
    globalVar->setInitializer(llvm::ConstantArray::get(tableType, values));
  }
  auto offset = new llvm::ZExtInst(
      func->arg_begin(), llvm::Type::getInt64Ty(Ctx), "", MergeBlock);
  llvm::Constant *zero = llvm::ConstantInt::get(llvm::Type::getInt64Ty(Ctx), 0);
  auto retval = llvm::GetElementPtrInst::Create(
      tableType, globalVar, {zero, offset}, "", MergeBlock);
  MergeBlock->insertInto(func);
  auto load = new llvm::LoadInst(ty, retval, "", MergeBlock);
  llvm::ReturnInst::Create(Ctx, load, MergeBlock);
  addAbort(stuck, module);
  stuck->insertInto(func);
}

} // namespace detail

void emitConfigParserFunctions(
    KOREDefinition *definition, llvm::Module *module);

} // namespace kllvm

#endif // EMIT_CONFIG_PARSER_H
