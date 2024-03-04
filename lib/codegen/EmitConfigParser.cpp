#include "kllvm/codegen/EmitConfigParser.h"
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
#include <llvm/IR/Type.h>
#include <llvm/IR/Value.h>
#include <llvm/Support/Casting.h>

#include <fmt/format.h>

#include <cstdint>
#include <cstdlib>
#include <iosfwd>
#include <iterator>
#include <map>
#include <memory>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <vector>

#include "config/macros.h"

namespace kllvm {

static llvm::Constant *getSymbolNamePtr(
    kore_symbol *symbol, llvm::BasicBlock *set_block_name, llvm::Module *module) {
  llvm::LLVMContext &Ctx = module->getContext();
  auto name = ast_to_string(*symbol);
  if (set_block_name) {
    set_block_name->setName(name);
  }
  auto *Str = llvm::ConstantDataArray::getString(Ctx, name, true);
  auto *global = module->getOrInsertGlobal(
      fmt::format("sym_name_{}", name), Str->getType());
  auto *globalVar = llvm::dyn_cast<llvm::GlobalVariable>(global);
  if (!globalVar->hasInitializer()) {
    globalVar->setInitializer(Str);
  }
  llvm::Constant *zero = llvm::ConstantInt::get(llvm::Type::getInt64Ty(Ctx), 0);
  auto indices = std::vector<llvm::Constant *>{zero, zero};
  auto *Ptr = llvm::ConstantExpr::getInBoundsGetElementPtr(
      Str->getType(), globalVar, indices);
  return Ptr;
}

static llvm::Function *getStrcmp(llvm::Module *module) {
  llvm::LLVMContext &Ctx = module->getContext();
  auto *type = llvm::FunctionType::get(
      llvm::Type::getInt32Ty(Ctx),
      {llvm::Type::getInt8PtrTy(Ctx), llvm::Type::getInt8PtrTy(Ctx)}, false);
  return getOrInsertFunction(module, "strcmp", type);
}

static void
emitGetTagForSymbolName(kore_definition *definition, llvm::Module *module) {
  llvm::LLVMContext &Ctx = module->getContext();
  auto *type = llvm::FunctionType::get(
      llvm::Type::getInt32Ty(Ctx), {llvm::Type::getInt8PtrTy(Ctx)}, false);
  auto *func = getOrInsertFunction(module, "getTagForSymbolNameInternal", type);
  auto *CurrentBlock = llvm::BasicBlock::Create(Ctx, "");
  auto *MergeBlock = llvm::BasicBlock::Create(Ctx, "exit");
  auto *Phi = llvm::PHINode::Create(
      llvm::Type::getInt32Ty(Ctx), definition->get_symbols().size(), "phi",
      MergeBlock);
  auto const &syms = definition->get_all_symbols();
  llvm::Function *Strcmp = getStrcmp(module);
  for (auto const &entry : syms) {
    uint32_t tag = entry.second->get_tag();
    auto *symbol = entry.second;
    CurrentBlock->insertInto(func);
    auto *Ptr = getSymbolNamePtr(symbol, CurrentBlock, module);
    auto *compare = llvm::CallInst::Create(
        Strcmp, {func->arg_begin(), Ptr}, "", CurrentBlock);
    auto *icmp = new llvm::ICmpInst(
        *CurrentBlock, llvm::CmpInst::ICMP_EQ, compare,
        llvm::ConstantInt::get(llvm::Type::getInt32Ty(Ctx), 0));
    auto *FalseBlock = llvm::BasicBlock::Create(Ctx, "");
    llvm::BranchInst::Create(MergeBlock, FalseBlock, icmp, CurrentBlock);
    Phi->addIncoming(
        llvm::ConstantInt::get(llvm::Type::getInt32Ty(Ctx), tag), CurrentBlock);
    CurrentBlock = FalseBlock;
  }
  Phi->addIncoming(
      llvm::ConstantInt::get(llvm::Type::getInt32Ty(Ctx), ERROR_TAG),
      CurrentBlock);
  llvm::BranchInst::Create(MergeBlock, CurrentBlock);
  CurrentBlock->insertInto(func);
  llvm::ReturnInst::Create(Ctx, Phi, MergeBlock);
  MergeBlock->insertInto(func);
}

static std::string string_struct = "string";
static std::string layout_struct = "layout";
static std::string layoutitem_struct = "layoutitem";

static void emitDataTableForSymbol(
    std::string const &name, llvm::Type *ty, llvm::DIType *dity,
    kore_definition *definition, llvm::Module *module,
    llvm::Constant *getter(kore_definition *, llvm::Module *, kore_symbol *)) {
  llvm::LLVMContext &Ctx = module->getContext();
  std::vector<llvm::Type *> argTypes;
  argTypes.push_back(llvm::Type::getInt32Ty(Ctx));
  auto *func = getOrInsertFunction(
      module, name, llvm::FunctionType::get(ty, argTypes, false));
  initDebugFunction(
      name, name, getDebugFunctionType(dity, {getIntDebugType()}), definition,
      func);
  auto *EntryBlock = llvm::BasicBlock::Create(Ctx, "entry", func);
  auto *MergeBlock = llvm::BasicBlock::Create(Ctx, "exit");
  auto *stuck = llvm::BasicBlock::Create(Ctx, "stuck");
  auto const &syms = definition->get_symbols();
  auto *icmp = new llvm::ICmpInst(
      *EntryBlock, llvm::CmpInst::ICMP_ULE, func->arg_begin(),
      llvm::ConstantInt::get(
          llvm::Type::getInt32Ty(Ctx), syms.rbegin()->first));
  llvm::BranchInst::Create(MergeBlock, stuck, icmp, EntryBlock);
  auto *tableType = llvm::ArrayType::get(ty, syms.size());
  auto *table = module->getOrInsertGlobal("table_" + name, tableType);
  auto *globalVar = llvm::cast<llvm::GlobalVariable>(table);
  initDebugGlobal(
      "table_" + name,
      getArrayDebugType(
          dity, syms.size(), llvm::DataLayout(module).getABITypeAlign(ty)),
      globalVar);
  std::vector<llvm::Constant *> values;
  for (auto entry : syms) {
    auto *symbol = entry.second;
    auto *val = getter(definition, module, symbol);
    values.push_back(val);
  }
  if (!globalVar->hasInitializer()) {
    globalVar->setInitializer(llvm::ConstantArray::get(tableType, values));
  }
  auto *offset = new llvm::ZExtInst(
      func->arg_begin(), llvm::Type::getInt64Ty(Ctx), "", MergeBlock);
  llvm::Constant *zero = llvm::ConstantInt::get(llvm::Type::getInt64Ty(Ctx), 0);
  auto *retval = llvm::GetElementPtrInst::Create(
      tableType, globalVar, {zero, offset}, "", MergeBlock);
  MergeBlock->insertInto(func);
  auto *load = new llvm::LoadInst(ty, retval, "", MergeBlock);
  llvm::ReturnInst::Create(Ctx, load, MergeBlock);
  addAbort(stuck, module);
  stuck->insertInto(func);
}

static void emitDataForSymbol(
    std::string const &name, llvm::Type *ty, llvm::DIType *dity,
    kore_definition *definition, llvm::Module *module, bool is_eval,
    std::pair<llvm::Value *, llvm::BasicBlock *> getter(
        kore_definition *, llvm::Module *, kore_symbol *,
        llvm::Instruction *)) {
  llvm::LLVMContext &Ctx = module->getContext();
  std::vector<llvm::Type *> argTypes;
  argTypes.push_back(llvm::Type::getInt32Ty(Ctx));
  if (is_eval) {
    auto *ty = llvm::PointerType::getUnqual(
        llvm::ArrayType::get(llvm::Type::getInt8PtrTy(Ctx), 0));
    argTypes.push_back(ty);
  }
  auto *func = getOrInsertFunction(
      module, name, llvm::FunctionType::get(ty, argTypes, false));
  if (!is_eval) {
    initDebugFunction(
        name, name, getDebugFunctionType(dity, {getIntDebugType()}), definition,
        func);
  }
  auto *EntryBlock = llvm::BasicBlock::Create(Ctx, "entry", func);
  auto *MergeBlock = llvm::BasicBlock::Create(Ctx, "exit");
  auto *stuck = llvm::BasicBlock::Create(Ctx, "stuck");
  auto const &syms = definition->get_symbols();
  auto *Switch = llvm::SwitchInst::Create(
      func->arg_begin(), stuck, syms.size(), EntryBlock);
  auto *Phi = llvm::PHINode::Create(
      ty, definition->get_symbols().size(), "phi", MergeBlock);
  for (auto entry : syms) {
    uint32_t tag = entry.first;
    auto *symbol = entry.second;
    auto *decl = definition->get_symbol_declarations().at(symbol->get_name());
    bool isFunc = decl->attributes().contains(attribute_set::key::Function)
                  || decl->attributes().contains(attribute_set::key::Anywhere);
    if (is_eval && !isFunc) {
      continue;
    }
    auto *CaseBlock
        = llvm::BasicBlock::Create(Ctx, "tag" + std::to_string(tag), func);
    auto *Branch = llvm::BranchInst::Create(MergeBlock, CaseBlock);
    auto pair = getter(definition, module, symbol, Branch);
    Phi->addIncoming(pair.first, pair.second);
    Switch->addCase(
        llvm::ConstantInt::get(llvm::Type::getInt32Ty(Ctx), tag), CaseBlock);
  }
  llvm::ReturnInst::Create(Ctx, Phi, MergeBlock);
  MergeBlock->insertInto(func);
  addAbort(stuck, module);
  stuck->insertInto(func);
}

static std::pair<llvm::Value *, llvm::BasicBlock *> getHeader(
    kore_definition *definition, llvm::Module *module, kore_symbol *symbol,
    llvm::Instruction *inst) {
  auto *BlockType = getBlockType(module, definition, symbol);
  return std::make_pair(
      getBlockHeader(module, definition, symbol, BlockType), inst->getParent());
}

static void
emitGetBlockHeaderForSymbol(kore_definition *def, llvm::Module *mod) {
  emitDataForSymbol(
      "getBlockHeaderForSymbol",
      llvm::StructType::getTypeByName(mod->getContext(), blockheader_struct),
      getForwardDecl(blockheader_struct), def, mod, false, getHeader);
}

static std::pair<llvm::Value *, llvm::BasicBlock *> getFunction(
    kore_definition *def, llvm::Module *mod, kore_symbol *symbol,
    llvm::Instruction *inst) {
  auto *decl = def->get_symbol_declarations().at(symbol->get_name());
  bool res = decl->attributes().contains(attribute_set::key::Function)
             || decl->attributes().contains(attribute_set::key::Anywhere);
  return std::make_pair(
      llvm::ConstantInt::get(llvm::Type::getInt1Ty(mod->getContext()), res),
      inst->getParent());
}

static void emitIsSymbolAFunction(kore_definition *def, llvm::Module *mod) {
  emitDataForSymbol(
      "isSymbolAFunction", llvm::Type::getInt1Ty(mod->getContext()),
      getBoolDebugType(), def, mod, false, getFunction);
}

static llvm::Constant *
getBinder(kore_definition *def, llvm::Module *mod, kore_symbol *symbol) {
  auto *decl = def->get_symbol_declarations().at(symbol->get_name());
  bool res = decl->attributes().contains(attribute_set::key::Binder);
  return llvm::ConstantInt::get(llvm::Type::getInt1Ty(mod->getContext()), res);
}

static void emitIsSymbolABinder(kore_definition *def, llvm::Module *mod) {
  emitDataTableForSymbol(
      "isSymbolABinder", llvm::Type::getInt1Ty(mod->getContext()),
      getBoolDebugType(), def, mod, getBinder);
}

static std::pair<llvm::Value *, llvm::BasicBlock *> getInjection(
    kore_definition *def, llvm::Module *mod, kore_symbol *symbol,
    llvm::Instruction *inst) {
  llvm::Constant *tag
      = llvm::ConstantInt::get(llvm::Type::getInt32Ty(mod->getContext()), 0);
  for (auto sym : def->get_symbols()) {
    if (sym.second->get_name() == "inj"
        && *sym.second->get_arguments()[0] == *symbol->get_sort()) {
      tag = llvm::ConstantInt::get(
          llvm::Type::getInt32Ty(mod->getContext()), sym.first);
      break;
    }
  }
  return std::make_pair(tag, inst->getParent());
}

static void
emitGetInjectionForSortOfTag(kore_definition *def, llvm::Module *mod) {
  emitDataForSymbol(
      "getInjectionForSortOfTag", llvm::Type::getInt32Ty(mod->getContext()),
      getIntDebugType(), def, mod, false, getInjection);
}

static llvm::Constant *
getArity(kore_definition *, llvm::Module *mod, kore_symbol *symbol) {
  return llvm::ConstantInt::get(
      llvm::Type::getInt32Ty(mod->getContext()), symbol->get_arguments().size());
}

static void emitGetSymbolArity(kore_definition *def, llvm::Module *mod) {
  emitDataTableForSymbol(
      "getSymbolArity", llvm::Type::getInt32Ty(mod->getContext()),
      getIntDebugType(), def, mod, getArity);
}

static llvm::Value *getArgValue(
    llvm::Value *arguments_array, int idx, llvm::BasicBlock *case_block,
    value_type cat, llvm::Module *mod) {
  llvm::LLVMContext &Ctx = mod->getContext();
  llvm::Constant *zero = llvm::ConstantInt::get(llvm::Type::getInt64Ty(Ctx), 0);

  auto *i8_ptr_ty = llvm::Type::getInt8PtrTy(Ctx);

  auto *addr = llvm::GetElementPtrInst::Create(
      llvm::ArrayType::get(i8_ptr_ty, 0), arguments_array,
      {zero, llvm::ConstantInt::get(llvm::Type::getInt64Ty(Ctx), idx)}, "",
      case_block);
  llvm::Value *arg = new llvm::LoadInst(i8_ptr_ty, addr, "", case_block);

  switch (cat.cat) {
  case sort_category::Bool:
  case sort_category::MInt: {
    auto *val_ty = getvalue_type(cat, mod);
    auto *cast = new llvm::BitCastInst(
        arg, llvm::PointerType::getUnqual(val_ty), "", case_block);
    auto *load = new llvm::LoadInst(val_ty, cast, "", case_block);
    arg = load;
    break;
  }
  case sort_category::Map:
  case sort_category::RangeMap:
  case sort_category::List:
  case sort_category::Set:
    arg = new llvm::BitCastInst(
        arg, llvm::PointerType::getUnqual(getvalue_type(cat, mod)), "",
        case_block);
    break;
  case sort_category::Int:
  case sort_category::Float:
  case sort_category::StringBuffer:
  case sort_category::Symbol:
  case sort_category::Variable:
    arg = new llvm::BitCastInst(arg, getvalue_type(cat, mod), "", case_block);
    break;
  case sort_category::Uncomputed: abort();
  }
  return arg;
}

static std::pair<llvm::Value *, llvm::BasicBlock *> getEval(
    kore_definition *def, llvm::Module *mod, kore_symbol *symbol,
    llvm::Instruction *inst) {
  llvm::LLVMContext &Ctx = mod->getContext();
  llvm::BasicBlock *CaseBlock = inst->getParent();
  inst->removeFromParent();
  llvm::Function *func = CaseBlock->getParent();
  llvm::Value *ArgumentsArray = func->arg_begin() + 1;
  int idx = 0;
  llvm::StringMap<llvm::Value *> subst;
  auto pattern = kore_composite_pattern::create(symbol);
  for (auto const &sort : symbol->get_arguments()) {
    value_type cat
        = dynamic_cast<kore_composite_sort *>(sort.get())->get_category(def);
    llvm::Value *arg = getArgValue(ArgumentsArray, idx, CaseBlock, cat, mod);
    std::string name = "_" + std::to_string(idx++);
    subst.insert({name, arg});
    pattern->add_argument(kore_variable_pattern::create(name, sort));
  }
  create_term creator(subst, def, CaseBlock, mod, false);
  llvm::Value *result = creator(pattern.get()).first;
  llvm::Value *retval = nullptr;
  value_type cat = dynamic_cast<kore_composite_sort *>(symbol->get_sort().get())
                       ->get_category(def);
  switch (cat.cat) {
  case sort_category::Int:
  case sort_category::Float:
  case sort_category::StringBuffer:
  case sort_category::Symbol:
  case sort_category::Variable:
  case sort_category::Map:
  case sort_category::RangeMap:
  case sort_category::List:
  case sort_category::Set:
    retval = new llvm::BitCastInst(
        result, llvm::Type::getInt8PtrTy(Ctx), "", creator.get_current_block());
    break;
  case sort_category::Bool:
  case sort_category::MInt: {
    llvm::Instruction *Malloc = llvm::CallInst::CreateMalloc(
        creator.get_current_block(), llvm::Type::getInt64Ty(Ctx),
        result->getType(), llvm::ConstantExpr::getSizeOf(result->getType()),
        nullptr, nullptr);
    Malloc->insertAfter(&creator.get_current_block()->back());
    new llvm::StoreInst(result, Malloc, creator.get_current_block());
    retval = new llvm::BitCastInst(
        Malloc, llvm::Type::getInt8PtrTy(Ctx), "", creator.get_current_block());
    break;
  }
  case sort_category::Uncomputed: abort();
  }
  inst->insertAfter(&creator.get_current_block()->back());
  return std::make_pair(retval, creator.get_current_block());
}

static void
emitEvaluateFunctionSymbol(kore_definition *def, llvm::Module *mod) {
  emitDataForSymbol(
      "evaluateFunctionSymbol", llvm::Type::getInt8PtrTy(mod->getContext()),
      nullptr, def, mod, true, getEval);
}

static void
emitGetTagForFreshSort(kore_definition *definition, llvm::Module *module) {
  llvm::LLVMContext &Ctx = module->getContext();
  auto *func = getOrInsertFunction(
      module, "getTagForFreshSort", llvm::Type::getInt32Ty(Ctx),
      llvm::Type::getInt8PtrTy(Ctx));
  auto *CurrentBlock = llvm::BasicBlock::Create(Ctx, "");
  auto *MergeBlock = llvm::BasicBlock::Create(Ctx, "exit");
  auto *Phi = llvm::PHINode::Create(
      llvm::Type::getInt32Ty(Ctx), definition->get_sort_declarations().size(),
      "phi", MergeBlock);
  auto const &sorts = definition->get_sort_declarations();
  llvm::Function *Strcmp = getStrcmp(module);
  llvm::Constant *zero = llvm::ConstantInt::get(llvm::Type::getInt64Ty(Ctx), 0);
  llvm::Constant *zero32
      = llvm::ConstantInt::get(llvm::Type::getInt32Ty(Ctx), 0);
  bool hasCase = false;
  for (auto const &entry : sorts) {
    std::string name = entry.first;
    if (!definition->get_fresh_functions().contains(name)) {
      continue;
    }
    hasCase = true;
    CurrentBlock->insertInto(func);
    CurrentBlock->setName("is_" + name);
    auto *Str = llvm::ConstantDataArray::getString(Ctx, name, true);
    auto *global
        = module->getOrInsertGlobal("sort_name_" + name, Str->getType());
    auto *globalVar = llvm::cast<llvm::GlobalVariable>(global);
    if (!globalVar->hasInitializer()) {
      globalVar->setInitializer(Str);
    }
    auto indices = std::vector<llvm::Constant *>{zero, zero};
    auto *Ptr = llvm::ConstantExpr::getInBoundsGetElementPtr(
        Str->getType(), globalVar, indices);
    auto *compare = llvm::CallInst::Create(
        Strcmp, {func->arg_begin(), Ptr}, "", CurrentBlock);
    auto *icmp = new llvm::ICmpInst(
        *CurrentBlock, llvm::CmpInst::ICMP_EQ, compare, zero32);
    auto *FalseBlock = llvm::BasicBlock::Create(Ctx, "");
    auto *CaseBlock = llvm::BasicBlock::Create(Ctx, name, func);
    llvm::BranchInst::Create(CaseBlock, FalseBlock, icmp, CurrentBlock);
    auto *symbol = definition->get_fresh_functions().at(name);
    Phi->addIncoming(
        llvm::ConstantInt::get(
            llvm::Type::getInt32Ty(Ctx),
            definition->get_all_symbols().at(ast_to_string(*symbol))->get_tag()),
        CaseBlock);
    llvm::BranchInst::Create(MergeBlock, CaseBlock);
    CurrentBlock = FalseBlock;
  }
  CurrentBlock->insertInto(func);
  addAbort(CurrentBlock, module);
  if (hasCase) {
    llvm::ReturnInst::Create(Ctx, Phi, MergeBlock);
    MergeBlock->insertInto(func);
  }
}

static void emitGetToken(kore_definition *definition, llvm::Module *module) {
  llvm::LLVMContext &Ctx = module->getContext();
  auto *getTokenType = llvm::FunctionType::get(
      llvm::Type::getInt8PtrTy(Ctx),
      {llvm::Type::getInt8PtrTy(Ctx), llvm::Type::getInt64Ty(Ctx),
       llvm::Type::getInt8PtrTy(Ctx)},
      false);
  auto *func = getOrInsertFunction(module, "getToken", getTokenType);
  auto *CurrentBlock = llvm::BasicBlock::Create(Ctx, "");
  auto *MergeBlock = llvm::BasicBlock::Create(Ctx, "exit");
  auto *Phi = llvm::PHINode::Create(
      llvm::Type::getInt8PtrTy(Ctx), definition->get_sort_declarations().size(),
      "phi", MergeBlock);
  auto const &sorts = definition->get_sort_declarations();
  llvm::Function *Strcmp = getStrcmp(module);
  llvm::Function *string_equal = getOrInsertFunction(
      module, "string_equal", llvm::Type::getInt1Ty(Ctx),
      llvm::Type::getInt8PtrTy(Ctx), llvm::Type::getInt8PtrTy(Ctx),
      llvm::Type::getInt64Ty(Ctx), llvm::Type::getInt64Ty(Ctx));
  llvm::Constant *zero = llvm::ConstantInt::get(llvm::Type::getInt64Ty(Ctx), 0);
  llvm::Constant *zero32
      = llvm::ConstantInt::get(llvm::Type::getInt32Ty(Ctx), 0);
  for (auto const &entry : sorts) {
    std::string name = entry.first;
    if (!entry.second->get_object_sort_variables().empty()) {
      // TODO: MINT in initial configuration
      continue;
    }
    auto sort = kore_composite_sort::create(name);
    value_type cat = sort->get_category(definition);
    if (cat.cat == sort_category::Symbol || cat.cat == sort_category::Variable) {
      continue;
    }
    CurrentBlock->insertInto(func);
    CurrentBlock->setName("is_" + name);
    auto *Str = llvm::ConstantDataArray::getString(Ctx, name, true);
    auto *global
        = module->getOrInsertGlobal("sort_name_" + name, Str->getType());
    auto *globalVar = llvm::dyn_cast<llvm::GlobalVariable>(global);
    if (!globalVar->hasInitializer()) {
      globalVar->setInitializer(Str);
    }
    auto indices = std::vector<llvm::Constant *>{zero, zero};
    auto *Ptr = llvm::ConstantExpr::getInBoundsGetElementPtr(
        Str->getType(), globalVar, indices);
    auto *compare = llvm::CallInst::Create(
        Strcmp, {func->arg_begin(), Ptr}, "", CurrentBlock);
    auto *icmp = new llvm::ICmpInst(
        *CurrentBlock, llvm::CmpInst::ICMP_EQ, compare, zero32);
    auto *FalseBlock = llvm::BasicBlock::Create(Ctx, "");
    auto *CaseBlock = llvm::BasicBlock::Create(Ctx, name, func);
    llvm::BranchInst::Create(CaseBlock, FalseBlock, icmp, CurrentBlock);
    switch (cat.cat) {
    case sort_category::Map:
    case sort_category::RangeMap:
    case sort_category::List:
    case sort_category::Set:
    case sort_category::StringBuffer:
    case sort_category::MInt:
      // TODO: tokens
      addAbort(CaseBlock, module);
      break;
    case sort_category::Bool: {
      auto *Str = llvm::ConstantDataArray::getString(Ctx, "true", false);
      auto *global = module->getOrInsertGlobal("bool_true", Str->getType());
      auto *globalVar = llvm::dyn_cast<llvm::GlobalVariable>(global);
      if (!globalVar->hasInitializer()) {
        globalVar->setInitializer(Str);
      }
      auto *Ptr = llvm::ConstantExpr::getInBoundsGetElementPtr(
          Str->getType(), globalVar, indices);
      auto *Len = llvm::ConstantInt::get(llvm::Type::getInt64Ty(Ctx), 4);
      auto *compare = llvm::CallInst::Create(
          string_equal,
          {func->arg_begin() + 2, Ptr, func->arg_begin() + 1, Len}, "",
          CaseBlock);
      llvm::Instruction *Malloc = llvm::CallInst::CreateMalloc(
          CaseBlock, llvm::Type::getInt64Ty(Ctx), compare->getType(),
          llvm::ConstantExpr::getSizeOf(compare->getType()), nullptr, nullptr);
      Malloc->insertAfter(&CaseBlock->back());
      new llvm::StoreInst(compare, Malloc, CaseBlock);
      auto *result = new llvm::BitCastInst(
          Malloc, llvm::Type::getInt8PtrTy(Ctx), "", CaseBlock);
      Phi->addIncoming(result, CaseBlock);
      llvm::BranchInst::Create(MergeBlock, CaseBlock);
      break;
    }
    case sort_category::Float: {
      llvm::Type *Float
          = llvm::StructType::getTypeByName(module->getContext(), float_struct);
      llvm::Value *Term = allocateTerm(Float, CaseBlock, "koreAllocFloating");
      llvm::Function *InitFloat = getOrInsertFunction(
          module, "init_float", llvm::Type::getVoidTy(Ctx),
          llvm::PointerType::getUnqual(Float), llvm::Type::getInt8PtrTy(Ctx));
      llvm::CallInst::Create(
          InitFloat, {Term, func->arg_begin() + 2}, "", CaseBlock);
      auto *cast = new llvm::BitCastInst(
          Term, llvm::Type::getInt8PtrTy(Ctx), "", CaseBlock);
      Phi->addIncoming(cast, CaseBlock);
      llvm::BranchInst::Create(MergeBlock, CaseBlock);
      break;
    }
    case sort_category::Int: {
      auto const &thirdArg = func->arg_begin() + 2;
      llvm::Value *FirstChar = new llvm::LoadInst(
          llvm::Type::getInt8Ty(Ctx), thirdArg, "", CaseBlock);
      llvm::Constant *asciiPlus
          = llvm::ConstantInt::get(llvm::Type::getInt8Ty(Ctx), 43);
      auto *icmpFirst = new llvm::ICmpInst(
          *CaseBlock, llvm::CmpInst::ICMP_EQ, FirstChar, asciiPlus);
      auto *IfIsPlus = llvm::BasicBlock::Create(Ctx, "if_is_plus", func);
      auto *ElseNoPlus = llvm::BasicBlock::Create(Ctx, "else_no_plus", func);
      llvm::BranchInst::Create(IfIsPlus, ElseNoPlus, icmpFirst, CaseBlock);
      llvm::Constant *one
          = llvm::ConstantInt::get(llvm::Type::getInt32Ty(Ctx), 1);
      llvm::Value *Pruned = llvm::GetElementPtrInst::CreateInBounds(
          llvm::Type::getInt8Ty(Ctx), func->arg_begin() + 2, {one}, "",
          IfIsPlus);
      llvm::BranchInst::Create(ElseNoPlus, IfIsPlus);
      auto *phiStr = llvm::PHINode::Create(
          llvm::Type::getInt8PtrTy(Ctx), 2, "", ElseNoPlus);
      phiStr->addIncoming(func->arg_begin() + 2, CaseBlock);
      phiStr->addIncoming(Pruned, IfIsPlus);
      CaseBlock = ElseNoPlus;
      llvm::Type *Int
          = llvm::StructType::getTypeByName(module->getContext(), int_struct);
      llvm::Value *Term = allocateTerm(Int, CaseBlock, "koreAllocInteger");
      llvm::Function *MpzInitSet = getOrInsertFunction(
          module, "__gmpz_init_set_str", llvm::Type::getInt32Ty(Ctx),
          llvm::PointerType::getUnqual(Int), llvm::Type::getInt8PtrTy(Ctx),
          llvm::Type::getInt32Ty(Ctx));
      auto *Call = llvm::CallInst::Create(
          MpzInitSet,
          {Term, phiStr,
           llvm::ConstantInt::get(llvm::Type::getInt32Ty(Ctx), 10)},
          "", CaseBlock);
      auto *icmp = new llvm::ICmpInst(
          *CaseBlock, llvm::CmpInst::ICMP_EQ, Call, zero32);
      auto *AbortBlock = llvm::BasicBlock::Create(Ctx, "invalid_int", func);
      addAbort(AbortBlock, module);
      auto *cast = new llvm::BitCastInst(
          Term, llvm::Type::getInt8PtrTy(Ctx), "", CaseBlock);
      llvm::BranchInst::Create(MergeBlock, AbortBlock, icmp, CaseBlock);
      Phi->addIncoming(cast, CaseBlock);
      break;
    }
    case sort_category::Variable:
    case sort_category::Symbol: break;
    case sort_category::Uncomputed: abort();
    }
    CurrentBlock = FalseBlock;
  }
  CurrentBlock->setName("symbol");
  CurrentBlock->insertInto(func);
  auto *StringType
      = llvm::StructType::getTypeByName(module->getContext(), string_struct);
  auto *Len = llvm::BinaryOperator::Create(
      llvm::Instruction::Add, func->arg_begin() + 1,
      llvm::ConstantExpr::getSizeOf(StringType), "", CurrentBlock);
  llvm::Value *Block
      = allocateTerm(StringType, Len, CurrentBlock, "koreAllocToken");
  auto *HdrPtr = llvm::GetElementPtrInst::CreateInBounds(
      StringType, Block, {zero, zero32, zero32}, "", CurrentBlock);
  auto *BlockSize
      = module->getOrInsertGlobal("BLOCK_SIZE", llvm::Type::getInt64Ty(Ctx));
  auto *BlockSizeVal = new llvm::LoadInst(
      llvm::Type::getInt64Ty(Ctx), BlockSize, "", CurrentBlock);
  auto *BlockAllocSize = llvm::BinaryOperator::Create(
      llvm::Instruction::Sub, BlockSizeVal,
      llvm::ConstantExpr::getSizeOf(llvm::Type::getInt8PtrTy(Ctx)), "",
      CurrentBlock);
  auto *icmp = new llvm::ICmpInst(
      *CurrentBlock, llvm::CmpInst::ICMP_UGT, Len, BlockAllocSize);
  auto *Mask = llvm::SelectInst::Create(
      icmp,
      llvm::ConstantInt::get(llvm::Type::getInt64Ty(Ctx), NOT_YOUNG_OBJECT_BIT),
      llvm::ConstantInt::get(llvm::Type::getInt64Ty(Ctx), 0), "", CurrentBlock);
  auto *HdrOred = llvm::BinaryOperator::Create(
      llvm::Instruction::Or, func->arg_begin() + 1, Mask, "", CurrentBlock);
  new llvm::StoreInst(HdrOred, HdrPtr, CurrentBlock);
  llvm::Function *Memcpy = getOrInsertFunction(
      module, "memcpy", llvm::Type::getInt8PtrTy(Ctx),
      llvm::Type::getInt8PtrTy(Ctx), llvm::Type::getInt8PtrTy(Ctx),
      llvm::Type::getInt64Ty(Ctx));
  auto *StrPtr = llvm::GetElementPtrInst::CreateInBounds(
      StringType, Block,
      {zero, llvm::ConstantInt::get(llvm::Type::getInt32Ty(Ctx), 1), zero}, "",
      CurrentBlock);
  llvm::CallInst::Create(
      Memcpy, {StrPtr, func->arg_begin() + 2, func->arg_begin() + 1}, "",
      CurrentBlock);
  auto *cast = new llvm::BitCastInst(
      Block, llvm::Type::getInt8PtrTy(Ctx), "", CurrentBlock);
  llvm::BranchInst::Create(MergeBlock, CurrentBlock);
  Phi->addIncoming(cast, CurrentBlock);
  llvm::ReturnInst::Create(Ctx, Phi, MergeBlock);
  MergeBlock->insertInto(func);
}

static llvm::PointerType *makeWriterType(llvm::LLVMContext &ctx) {
  std::string const name = "writer";
  static auto types = std::map<llvm::LLVMContext *, llvm::PointerType *>{};

  if (types.find(&ctx) == types.end()) {
    types[&ctx]
        = llvm::PointerType::getUnqual(llvm::StructType::create(ctx, name));
  }

  return types.at(&ctx);
}

static llvm::PointerType *makeVisitorType(
    llvm::LLVMContext &ctx, llvm::Type *file, llvm::Type *item, int num_strs,
    int num_bools) {
  std::vector<llvm::Type *> types;
  types.push_back(file);
  types.push_back(item);
  for (int i = 0; i < num_strs; i++) {
    types.push_back(llvm::Type::getInt8PtrTy(ctx));
  }
  for (int i = 0; i < num_bools; i++) {
    types.push_back(llvm::Type::getInt1Ty(ctx));
  }

  types.push_back(llvm::Type::getInt8PtrTy(ctx));

  return llvm::PointerType::getUnqual(
      llvm::FunctionType::get(llvm::Type::getVoidTy(ctx), types, false));
}

static llvm::StructType *
makePackedVisitorStructureType(llvm::LLVMContext &ctx, llvm::Module *module) {
  std::string const name = "visitor";
  static auto types = std::map<llvm::LLVMContext *, llvm::StructType *>{};

  auto *file = makeWriterType(ctx);

  if (types.find(&ctx) == types.end()) {
    auto elementTypes = std::vector<llvm::Type *>{
        {makeVisitorType(
             ctx, file, getvalue_type({sort_category::Symbol, 0}, module), 1, 1),
         makeVisitorType(
             ctx, file,
             llvm::PointerType::getUnqual(
                 getvalue_type({sort_category::Map, 0}, module)),
             3, 0),
         makeVisitorType(
             ctx, file,
             llvm::PointerType::getUnqual(
                 getvalue_type({sort_category::List, 0}, module)),
             3, 0),
         makeVisitorType(
             ctx, file,
             llvm::PointerType::getUnqual(
                 getvalue_type({sort_category::Set, 0}, module)),
             3, 0),
         makeVisitorType(
             ctx, file, getvalue_type({sort_category::Int, 0}, module), 1, 0),
         makeVisitorType(
             ctx, file, getvalue_type({sort_category::Float, 0}, module), 1, 0),
         makeVisitorType(
             ctx, file, getvalue_type({sort_category::Bool, 0}, module), 1, 0),
         makeVisitorType(
             ctx, file, getvalue_type({sort_category::StringBuffer, 0}, module),
             1, 0),
         llvm::PointerType::getUnqual(llvm::FunctionType::get(
             llvm::Type::getVoidTy(ctx),
             {file, llvm::Type::getInt64PtrTy(ctx), llvm::Type::getInt64Ty(ctx),
              llvm::Type::getInt8PtrTy(ctx), llvm::Type::getInt8PtrTy(ctx)},
             false)),
         llvm::PointerType::getUnqual(llvm::FunctionType::get(
             llvm::Type::getVoidTy(ctx), {file, llvm::Type::getInt8PtrTy(ctx)},
             false)),
         makeVisitorType(
             ctx, file,
             llvm::PointerType::getUnqual(
                 getvalue_type({sort_category::RangeMap, 0}, module)),
             3, 0)}};

    auto *structTy = llvm::StructType::create(ctx, elementTypes, name);
    types[&ctx] = structTy;
  }

  return types.at(&ctx);
}

static void emitTraversal(
    std::string const &name, kore_definition *definition, llvm::Module *module,
    bool is_visitor,
    void getter(
        kore_definition *, llvm::Module *, kore_symbol *, llvm::BasicBlock *,
        std::vector<llvm::Value *> const &callbacks)) {
  llvm::LLVMContext &Ctx = module->getContext();
  std::vector<llvm::Type *> argTypes;

  auto *block_ty = getBlockType(module);
  argTypes.push_back(block_ty->getPointerTo());

  if (is_visitor) {
    // cf runtime/util/header.h visitChildren
    auto *file = makeWriterType(Ctx);
    argTypes.push_back(file);
    argTypes.push_back(
        makePackedVisitorStructureType(Ctx, module)->getPointerTo());
    argTypes.push_back(llvm::Type::getInt8PtrTy(Ctx));
  } else {
    argTypes.push_back(llvm::PointerType::getUnqual(
        llvm::ArrayType::get(llvm::Type::getInt8PtrTy(Ctx), 0)));
  }

  auto *func = llvm::cast<llvm::Function>(getOrInsertFunction(
      module, name,
      llvm::FunctionType::get(llvm::Type::getVoidTy(Ctx), argTypes, false)));
  llvm::Constant *zero = llvm::ConstantInt::get(llvm::Type::getInt64Ty(Ctx), 0);
  llvm::Constant *zero32
      = llvm::ConstantInt::get(llvm::Type::getInt32Ty(Ctx), 0);
  auto *EntryBlock = llvm::BasicBlock::Create(Ctx, "entry", func);

  auto *HdrPtr = llvm::GetElementPtrInst::CreateInBounds(
      block_ty, func->arg_begin(), {zero, zero32, zero32}, "", EntryBlock);
  auto *Hdr
      = new llvm::LoadInst(llvm::Type::getInt64Ty(Ctx), HdrPtr, "", EntryBlock);
  auto *Tag
      = new llvm::TruncInst(Hdr, llvm::Type::getInt32Ty(Ctx), "", EntryBlock);

  auto callbacks = std::vector<llvm::Value *>{};
  if (is_visitor) {
    auto *visitorStruct = func->getArg(2);
    auto *visitorType = makePackedVisitorStructureType(Ctx, module);

    for (auto i = 0; i < visitorType->getNumElements(); ++i) {
      auto *ptr = llvm::GetElementPtrInst::CreateInBounds(
          visitorType, visitorStruct,
          {zero, llvm::ConstantInt::get(llvm::Type::getInt32Ty(Ctx), i)}, "",
          EntryBlock);

      callbacks.push_back(new llvm::LoadInst(
          visitorType->getTypeAtIndex(i), ptr, "", EntryBlock));
    }
  }

  auto *stuck = llvm::BasicBlock::Create(Ctx, "stuck");
  auto const &syms = definition->get_symbols();
  auto *Switch = llvm::SwitchInst::Create(Tag, stuck, syms.size(), EntryBlock);

  for (auto entry : syms) {
    uint32_t tag = entry.first;
    auto *symbol = entry.second;
    if (symbol->get_arguments().empty()) {
      continue;
    }
    auto *CaseBlock
        = llvm::BasicBlock::Create(Ctx, "tag" + std::to_string(tag), func);
    Switch->addCase(
        llvm::ConstantInt::get(llvm::Type::getInt32Ty(Ctx), tag), CaseBlock);
    getter(definition, module, symbol, CaseBlock, callbacks);
    llvm::ReturnInst::Create(Ctx, CaseBlock);
  }
  addAbort(stuck, module);
  stuck->insertInto(func);
}

static void getStore(
    kore_definition *definition, llvm::Module *module, kore_symbol *symbol,
    llvm::BasicBlock *case_block, std::vector<llvm::Value *> const &) {
  llvm::LLVMContext &Ctx = module->getContext();
  llvm::Constant *zero = llvm::ConstantInt::get(llvm::Type::getInt64Ty(Ctx), 0);
  llvm::Function *func = case_block->getParent();
  llvm::Value *ArgumentsArray = func->arg_begin() + 1;
  int idx = 0;
  auto *BlockType = getBlockType(module, definition, symbol);
  auto *cast = new llvm::BitCastInst(
      func->arg_begin(), llvm::PointerType::getUnqual(BlockType), "",
      case_block);
  for (auto const &sort : symbol->get_arguments()) {
    value_type cat = dynamic_cast<kore_composite_sort *>(sort.get())
                         ->get_category(definition);
    llvm::Value *arg = getArgValue(ArgumentsArray, idx, case_block, cat, module);
    llvm::Type *arg_ty = getArgType(cat, module);
    llvm::Value *ChildPtr = llvm::GetElementPtrInst::CreateInBounds(
        BlockType, cast,
        {zero, llvm::ConstantInt::get(llvm::Type::getInt32Ty(Ctx), idx++ + 2)},
        "", case_block);
    if (isCollectionSort(cat)) {
      arg = new llvm::LoadInst(arg_ty, arg, "", case_block);
    }
    new llvm::StoreInst(arg, ChildPtr, case_block);
  }
}

static void
emitStoreSymbolChildren(kore_definition *definition, llvm::Module *module) {
  emitTraversal("storeSymbolChildren", definition, module, false, getStore);
}

static llvm::Constant *getSymbolName(
    kore_definition *definition, llvm::Module *module, kore_symbol *symbol) {
  return getSymbolNamePtr(symbol, nullptr, module);
}

static void emitGetSymbolNameForTag(kore_definition *def, llvm::Module *mod) {
  emitDataTableForSymbol(
      "getSymbolNameForTag", llvm::Type::getInt8PtrTy(mod->getContext()),
      getCharPtrDebugType(), def, mod, getSymbolName);
}

static void visitCollection(
    kore_definition *definition, llvm::Module *module,
    kore_composite_sort *composite_sort, llvm::Function *func,
    llvm::Value *child_ptr, llvm::BasicBlock *case_block, llvm::Value *callback,
    llvm::Value *state_ptr) {
  llvm::LLVMContext &Ctx = module->getContext();
  llvm::Constant *zero = llvm::ConstantInt::get(llvm::Type::getInt64Ty(Ctx), 0);
  auto indices = std::vector<llvm::Constant *>{zero, zero};
  auto *sortDecl
      = definition->get_sort_declarations().at(composite_sort->get_name());
  llvm::Constant *concatPtr = nullptr;
  if (sortDecl->attributes().contains(attribute_set::key::Concat)) {
    auto *concat = (kore_composite_pattern *)sortDecl->attributes()
                       .get(attribute_set::key::Concat)
                       ->get_arguments()[0]
                       .get();
    concatPtr = getSymbolNamePtr(concat->get_constructor(), nullptr, module);
  } else {
    concatPtr = llvm::ConstantPointerNull::get(llvm::Type::getInt8PtrTy(Ctx));
  }
  auto *unit = (kore_composite_pattern *)sortDecl->attributes()
                   .get(attribute_set::key::Unit)
                   ->get_arguments()[0]
                   .get();
  auto *unitPtr = getSymbolNamePtr(unit->get_constructor(), nullptr, module);
  auto *element = (kore_composite_pattern *)sortDecl->attributes()
                      .get(attribute_set::key::Element)
                      ->get_arguments()[0]
                      .get();
  auto *elementPtr
      = getSymbolNamePtr(element->get_constructor(), nullptr, module);
  auto *file = makeWriterType(Ctx);
  auto *i8_ptr_ty = llvm::Type::getInt8PtrTy(Ctx);
  auto *fnType = llvm::FunctionType::get(
      llvm::Type::getVoidTy(Ctx),
      {file, child_ptr->getType(), i8_ptr_ty, i8_ptr_ty, i8_ptr_ty, i8_ptr_ty},
      false);
  llvm::CallInst::Create(
      fnType, callback,
      {func->arg_begin() + 1, child_ptr, unitPtr, elementPtr, concatPtr,
       state_ptr},
      "", case_block);
}

// NOLINTNEXTLINE(*-cognitive-complexity)
static void getVisitor(
    kore_definition *definition, llvm::Module *module, kore_symbol *symbol,
    llvm::BasicBlock *case_block, std::vector<llvm::Value *> const &callbacks) {
  llvm::LLVMContext &Ctx = module->getContext();
  llvm::Constant *zero = llvm::ConstantInt::get(llvm::Type::getInt64Ty(Ctx), 0);
  auto indices = std::vector<llvm::Constant *>{zero, zero};
  llvm::Function *func = case_block->getParent();
  int idx = 0;
  auto *BlockType = getBlockType(module, definition, symbol);
  auto *cast = new llvm::BitCastInst(
      func->arg_begin(), llvm::PointerType::getUnqual(BlockType), "",
      case_block);
  unsigned i = 0;
  auto *file = makeWriterType(Ctx);

  auto *state_ptr = func->arg_end() - 1;

  for (auto const &sort : symbol->get_arguments()) {
    auto *compositeSort = dynamic_cast<kore_composite_sort *>(sort.get());
    value_type cat = compositeSort->get_category(definition);
    llvm::Value *ChildPtr = llvm::GetElementPtrInst::CreateInBounds(
        BlockType, cast,
        {zero, llvm::ConstantInt::get(llvm::Type::getInt32Ty(Ctx), idx++ + 2)},
        "", case_block);
    llvm::Value *Child = new llvm::LoadInst(
        getvalue_type(cat, module), ChildPtr, "", case_block);
    auto sort_name = ast_to_string(*sort);
    auto *Str = llvm::ConstantDataArray::getString(Ctx, sort_name, true);
    auto *global = module->getOrInsertGlobal(
        fmt::format("sort_name_{}", sort_name), Str->getType());
    auto *globalVar = llvm::dyn_cast<llvm::GlobalVariable>(global);
    if (!globalVar->hasInitializer()) {
      globalVar->setInitializer(Str);
    }
    llvm::Constant *CharPtr = llvm::ConstantExpr::getInBoundsGetElementPtr(
        Str->getType(), global, indices);
    switch (cat.cat) {
    case sort_category::Variable:
    case sort_category::Symbol:
      llvm::CallInst::Create(
          llvm::FunctionType::get(
              llvm::Type::getVoidTy(Ctx),
              {file, Child->getType(), llvm::Type::getInt8PtrTy(Ctx),
               llvm::Type::getInt1Ty(Ctx), llvm::Type::getInt8PtrTy(Ctx)},
              false),
          callbacks.at(0),
          {func->arg_begin() + 1, Child, CharPtr,
           llvm::ConstantInt::get(
               llvm::Type::getInt1Ty(Ctx), cat.cat == sort_category::Variable),
           state_ptr},
          "", case_block);
      break;
    case sort_category::Int:
      llvm::CallInst::Create(
          llvm::FunctionType::get(
              llvm::Type::getVoidTy(Ctx),
              {file, Child->getType(), llvm::Type::getInt8PtrTy(Ctx),
               llvm::Type::getInt8PtrTy(Ctx)},
              false),
          callbacks.at(4), {func->arg_begin() + 1, Child, CharPtr, state_ptr},
          "", case_block);
      break;
    case sort_category::Float:
      llvm::CallInst::Create(
          llvm::FunctionType::get(
              llvm::Type::getVoidTy(Ctx),
              {file, Child->getType(), llvm::Type::getInt8PtrTy(Ctx),
               llvm::Type::getInt8PtrTy(Ctx)},
              false),
          callbacks.at(5), {func->arg_begin() + 1, Child, CharPtr, state_ptr},
          "", case_block);
      break;
    case sort_category::Bool:
      llvm::CallInst::Create(
          llvm::FunctionType::get(
              llvm::Type::getVoidTy(Ctx),
              {file, Child->getType(), llvm::Type::getInt8PtrTy(Ctx),
               llvm::Type::getInt8PtrTy(Ctx)},
              false),
          callbacks.at(6), {func->arg_begin() + 1, Child, CharPtr, state_ptr},
          "", case_block);
      break;
    case sort_category::StringBuffer:
      llvm::CallInst::Create(
          llvm::FunctionType::get(
              llvm::Type::getVoidTy(Ctx),
              {file, Child->getType(), llvm::Type::getInt8PtrTy(Ctx),
               llvm::Type::getInt8PtrTy(Ctx)},
              false),
          callbacks.at(7), {func->arg_begin() + 1, Child, CharPtr, state_ptr},
          "", case_block);
      break;
    case sort_category::MInt: {
      llvm::Value *mint = new llvm::LoadInst(
          getArgType(cat, module), ChildPtr, "mint", case_block);
      size_t nwords = (cat.bits + 63) / 64;
      auto *nbits
          = llvm::ConstantInt::get(llvm::Type::getInt64Ty(Ctx), cat.bits);
      auto *fnType = llvm::FunctionType::get(
          llvm::Type::getVoidTy(Ctx),
          {file, llvm::Type::getInt64PtrTy(Ctx), llvm::Type::getInt64Ty(Ctx),
           llvm::Type::getInt8PtrTy(Ctx), llvm::Type::getInt8PtrTy(Ctx)},
          false);
      if (nwords == 0) {
        llvm::CallInst::Create(
            fnType, func->arg_begin() + 10,
            {func->arg_begin() + 1,
             llvm::ConstantPointerNull::get(llvm::Type::getInt64PtrTy(Ctx)),
             nbits, CharPtr, state_ptr},
            "", case_block);
      } else {
        auto *Ptr = allocateTerm(
            llvm::Type::getInt64Ty(Ctx),
            llvm::ConstantInt::get(llvm::Type::getInt64Ty(Ctx), nwords * 8),
            case_block, "koreAllocAlwaysGC");
        if (nwords == 1) {
          llvm::Value *Word = nullptr;
          if (cat.bits == 64) {
            Word = mint;
          } else {
            Word = new llvm::ZExtInst(
                mint, llvm::Type::getInt64Ty(Ctx), "word", case_block);
          }
          new llvm::StoreInst(Word, Ptr, case_block);
        } else { // nwords >= 2
          llvm::Value *Ptr2 = Ptr;
          llvm::Value *accum = mint;
          for (size_t i = 0; i < nwords; i++) {
            auto *Word = new llvm::TruncInst(
                accum, llvm::Type::getInt64Ty(Ctx), "word", case_block);
            new llvm::StoreInst(Word, Ptr2, case_block);
            Ptr2 = llvm::GetElementPtrInst::Create(
                llvm::Type::getInt64Ty(Ctx), Ptr2,
                {llvm::ConstantInt::get(llvm::Type::getInt64Ty(Ctx), 1)}, "ptr",
                case_block);
            accum = llvm::BinaryOperator::Create(
                llvm::Instruction::LShr, accum,
                llvm::ConstantInt::get(mint->getType(), 64), "shift",
                case_block);
          }
        }
        llvm::CallInst::Create(
            fnType, callbacks.at(8),
            {func->arg_begin() + 1, Ptr, nbits, CharPtr, state_ptr}, "",
            case_block);
      }
      break;
    }
    case sort_category::Map:
      visitCollection(
          definition, module, compositeSort, func, ChildPtr, case_block,
          callbacks.at(1), state_ptr);
      break;
    case sort_category::RangeMap:
      visitCollection(
          definition, module, compositeSort, func, ChildPtr, case_block,
          callbacks.at(10), state_ptr);
      break;
    case sort_category::Set:
      visitCollection(
          definition, module, compositeSort, func, ChildPtr, case_block,
          callbacks.at(3), state_ptr);
      break;
    case sort_category::List: {
      visitCollection(
          definition, module, compositeSort, func, ChildPtr, case_block,
          callbacks.at(2), state_ptr);
      break;
    }
    case sort_category::Uncomputed: abort();
    }
    if (i != symbol->get_arguments().size() - 1) {
      llvm::CallInst::Create(
          llvm::FunctionType::get(
              llvm::Type::getVoidTy(Ctx), {file, llvm::Type::getInt8PtrTy(Ctx)},
              false),
          callbacks.at(9), {func->arg_begin() + 1, state_ptr}, "", case_block);
    }
    i++;
  }
}

static llvm::Constant *getOffsetOfMember(
    [[maybe_unused]] llvm::Module *mod, llvm::StructType *struct_ty,
    int nth_member) {
#if LLVM_VERSION_MAJOR >= 17
  auto offset
      = llvm::DataLayout(mod).getStructLayout(struct_ty)->getElementOffset(
          nth_member);
  auto *offset_ty = llvm::Type::getInt64Ty(mod->getContext());
  return llvm::ConstantInt::get(offset_ty, offset);
#else
  return llvm::ConstantExpr::getOffsetOf(struct_ty, nth_member);
#endif
}

static llvm::Constant *getLayoutData(
    uint16_t layout, kore_symbol *symbol, llvm::Module *module,
    kore_definition *def) {
  uint8_t len = symbol->get_arguments().size();
  std::vector<llvm::Constant *> elements;
  llvm::LLVMContext &Ctx = module->getContext();
  auto *BlockType = getBlockType(module, def, symbol);
  int i = 2;
  for (auto const &sort : symbol->get_arguments()) {
    value_type cat
        = dynamic_cast<kore_composite_sort *>(sort.get())->get_category(def);
    auto *offset = getOffsetOfMember(
        module, BlockType,
        i++); //llvm::ConstantExpr::getOffsetOf(BlockType, i++);
    elements.push_back(llvm::ConstantStruct::get(
        llvm::StructType::getTypeByName(
            module->getContext(), layoutitem_struct),
        offset,
        llvm::ConstantInt::get(
            llvm::Type::getInt16Ty(Ctx), (int)cat.cat + cat.bits)));
  }
  auto *Arr = llvm::ConstantArray::get(
      llvm::ArrayType::get(
          llvm::StructType::getTypeByName(
              module->getContext(), layoutitem_struct),
          len),
      elements);
  auto *global = module->getOrInsertGlobal(
      "layout_item_" + std::to_string(layout), Arr->getType());
  auto *globalVar = llvm::cast<llvm::GlobalVariable>(global);
  if (!globalVar->hasInitializer()) {
    globalVar->setInitializer(Arr);
  }
  llvm::Constant *zero = llvm::ConstantInt::get(llvm::Type::getInt64Ty(Ctx), 0);
  auto indices = std::vector<llvm::Constant *>{zero, zero};
  auto *Ptr = llvm::ConstantExpr::getInBoundsGetElementPtr(
      Arr->getType(), globalVar, indices);
  std::string name = "layout_" + std::to_string(layout);
  auto *global2 = module->getOrInsertGlobal(
      name,
      llvm::StructType::getTypeByName(module->getContext(), layout_struct));
  auto *globalVar2 = llvm::cast<llvm::GlobalVariable>(global2);
  initDebugGlobal(name, getForwardDecl(layout_struct), globalVar2);
  if (!globalVar2->hasInitializer()) {
    globalVar2->setInitializer(llvm::ConstantStruct::get(
        llvm::StructType::getTypeByName(module->getContext(), layout_struct),
        llvm::ConstantInt::get(llvm::Type::getInt8Ty(Ctx), len), Ptr));
  }
  return globalVar2;
}

static void emitLayouts(kore_definition *definition, llvm::Module *module) {
  std::map<uint16_t, kore_symbol *> layouts;
  for (auto entry : definition->get_symbols()) {
    layouts[entry.second->get_layout()] = entry.second;
  }
  llvm::LLVMContext &Ctx = module->getContext();
  std::vector<llvm::Type *> argTypes;
  argTypes.push_back(llvm::Type::getInt16Ty(Ctx));
  auto *func = llvm::cast<llvm::Function>(getOrInsertFunction(
      module, "getLayoutData",
      llvm::FunctionType::get(
          llvm::PointerType::getUnqual(llvm::StructType::getTypeByName(
              module->getContext(), layout_struct)),
          argTypes, false)));
  initDebugFunction(
      "getLayoutData", "getLayoutData",
      getDebugFunctionType(
          getPointerDebugType(getForwardDecl(layout_struct), "layout *"),
          {getShortDebugType()}),
      definition, func);
  auto *EntryBlock = llvm::BasicBlock::Create(Ctx, "entry", func);
  auto *MergeBlock = llvm::BasicBlock::Create(Ctx, "exit");
  auto *stuck = llvm::BasicBlock::Create(Ctx, "stuck");
  auto *Switch = llvm::SwitchInst::Create(
      func->arg_begin(), stuck, layouts.size(), EntryBlock);
  auto *Phi = llvm::PHINode::Create(
      llvm::PointerType::getUnqual(
          llvm::StructType::getTypeByName(module->getContext(), layout_struct)),
      layouts.size(), "phi", MergeBlock);
  for (auto entry : layouts) {
    uint16_t layout = entry.first;
    auto *symbol = entry.second;
    auto *CaseBlock = llvm::BasicBlock::Create(
        Ctx, "layout" + std::to_string(layout), func);
    llvm::BranchInst::Create(MergeBlock, CaseBlock);
    auto *data = getLayoutData(layout, symbol, module, definition);
    Phi->addIncoming(data, CaseBlock);
    Switch->addCase(
        llvm::ConstantInt::get(llvm::Type::getInt16Ty(Ctx), layout), CaseBlock);
  }
  llvm::ReturnInst::Create(Ctx, Phi, MergeBlock);
  MergeBlock->insertInto(func);
  addAbort(stuck, module);
  stuck->insertInto(func);
}

static void emitVisitChildren(kore_definition *def, llvm::Module *mod) {
  emitTraversal("visitChildren", def, mod, true, getVisitor);
}

static void emitInjTags(kore_definition *def, llvm::Module *mod) {
  llvm::LLVMContext &Ctx = mod->getContext();
  auto *global
      = mod->getOrInsertGlobal("FIRST_INJ_TAG", llvm::Type::getInt32Ty(Ctx));
  auto *globalVar = llvm::cast<llvm::GlobalVariable>(global);
  globalVar->setConstant(true);
  if (!globalVar->hasInitializer()) {
    globalVar->setInitializer(llvm::ConstantInt::get(
        llvm::Type::getInt32Ty(Ctx), def->get_inj_symbol()->get_first_tag()));
  }
  global = mod->getOrInsertGlobal("LAST_INJ_TAG", llvm::Type::getInt32Ty(Ctx));
  globalVar = llvm::cast<llvm::GlobalVariable>(global);
  globalVar->setConstant(true);
  if (!globalVar->hasInitializer()) {
    globalVar->setInitializer(llvm::ConstantInt::get(
        llvm::Type::getInt32Ty(Ctx), def->get_inj_symbol()->get_last_tag()));
  }
}

static void emitSortTable(kore_definition *def, llvm::Module *mod) {
  auto getter = [](kore_definition *definition, llvm::Module *module,
                   kore_symbol *symbol) -> llvm::Constant * {
    auto &ctx = module->getContext();

    auto *subtableType = llvm::ArrayType::get(
        llvm::Type::getInt8PtrTy(ctx), symbol->get_arguments().size());
    auto *subtable = module->getOrInsertGlobal(
        fmt::format("sorts_{}", ast_to_string(*symbol)), subtableType);
    auto *subtableVar = llvm::dyn_cast<llvm::GlobalVariable>(subtable);
    initDebugGlobal(
        "sorts_" + symbol->get_name(),
        getArrayDebugType(
            getCharPtrDebugType(), symbol->get_arguments().size(),
            llvm::DataLayout(module).getABITypeAlign(
                llvm::Type::getInt8PtrTy(ctx))),
        subtableVar);
    llvm::Constant *zero
        = llvm::ConstantInt::get(llvm::Type::getInt64Ty(ctx), 0);
    auto indices = std::vector<llvm::Constant *>{zero, zero};

    std::vector<llvm::Constant *> subvalues;
    for (const auto &i : symbol->get_arguments()) {
      auto arg_str = ast_to_string(*i);
      auto *strType = llvm::ArrayType::get(
          llvm::Type::getInt8Ty(ctx), arg_str.size() + 1);
      auto *sortName = module->getOrInsertGlobal(
          fmt::format("sort_name_{}", arg_str), strType);
      subvalues.push_back(llvm::ConstantExpr::getInBoundsGetElementPtr(
          strType, sortName, indices));
    }
    subtableVar->setInitializer(
        llvm::ConstantArray::get(subtableType, subvalues));

    return llvm::ConstantExpr::getInBoundsGetElementPtr(
        subtableType, subtableVar, indices);
  };

  auto *i8_ptr_ty = llvm::Type::getInt8PtrTy(mod->getContext());
  auto *entry_ty = llvm::PointerType::getUnqual(i8_ptr_ty);
  auto *debug_ty = getPointerDebugType(getCharPtrDebugType(), "char **");

  emitDataTableForSymbol(
      "getArgumentSortsForTag", entry_ty, debug_ty, def, mod, getter);
}

/*
 * Emit a table mapping symbol tags to the declared return sort for that symbol.
 * For example:
 *
 *   tag_of(initGeneratedTopCell) |-> sort_name_SortGeneratedTopCell{}
 *
 * Each value in the table is a pointer to a global variable containing the
 * relevant sort name as a null-terminated string.
 */
static void emitReturnSortTable(kore_definition *def, llvm::Module *mod) {
  auto getter = [](kore_definition *definition, llvm::Module *module,
                   kore_symbol *symbol) -> llvm::Constant * {
    auto &ctx = module->getContext();

    auto sort = symbol->get_sort();
    auto sort_str = ast_to_string(*sort);

    auto *char_type = llvm::Type::getInt8Ty(ctx);
    auto *str_type = llvm::ArrayType::get(char_type, sort_str.size() + 1);

    auto *sort_name
        = module->getOrInsertGlobal("sort_name_" + sort_str, str_type);

    auto *i64_type = llvm::Type::getInt64Ty(ctx);
    auto *zero = llvm::ConstantInt::get(i64_type, 0);

    return llvm::ConstantExpr::getInBoundsGetElementPtr(
        str_type, sort_name, std::vector<llvm::Constant *>{zero});
  };

  emitDataTableForSymbol(
      "getReturnSortForTag", llvm::Type::getInt8PtrTy(mod->getContext()),
      getCharPtrDebugType(), def, mod, getter);
}

/*
 * Emit a table that records whether the symbol corresponding to a particular
 * tag has instantiated sort parameters. For example:
 *
 *   tag_of(inj{SortA{}, SortKItem{}}) |-> true
 *   tag_of(someSymbol{})              |-> false
 */
static void emitSymbolIsInstantiation(kore_definition *def, llvm::Module *mod) {
  auto getter = [](kore_definition *definition, llvm::Module *module,
                   kore_symbol *symbol) -> llvm::Constant * {
    auto &ctx = module->getContext();

    auto *bool_ty = llvm::Type::getInt1Ty(ctx);

    return llvm::ConstantInt::get(
        bool_ty, !symbol->get_formal_arguments().empty());
  };

  emitDataTableForSymbol(
      "symbolIsInstantiation", llvm::Type::getInt1Ty(mod->getContext()),
      getBoolDebugType(), def, mod, getter);
}

void emitConfigParserFunctions(
    kore_definition *definition, llvm::Module *module) {
  emitGetTagForSymbolName(definition, module);
  emitGetBlockHeaderForSymbol(definition, module);
  emitIsSymbolAFunction(definition, module);
  emitIsSymbolABinder(definition, module);
  emitStoreSymbolChildren(definition, module);
  emitEvaluateFunctionSymbol(definition, module);
  emitGetToken(definition, module);
  emitGetTagForFreshSort(definition, module);
  emitGetInjectionForSortOfTag(definition, module);
  emitGetSymbolArity(definition, module);

  emitGetSymbolNameForTag(definition, module);
  emitVisitChildren(definition, module);

  emitLayouts(definition, module);

  emitInjTags(definition, module);

  emitSortTable(definition, module);
  emitReturnSortTable(definition, module);
  emitSymbolIsInstantiation(definition, module);
}

} // namespace kllvm
