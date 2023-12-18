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
    KORESymbol *symbol, llvm::BasicBlock *SetBlockName, llvm::Module *module) {
  llvm::LLVMContext &Ctx = module->getContext();
  auto name = ast_to_string(*symbol);
  if (SetBlockName) {
    SetBlockName->setName(name);
  }
  auto Str = llvm::ConstantDataArray::getString(Ctx, name, true);
  auto global = module->getOrInsertGlobal(
      fmt::format("sym_name_{}", name), Str->getType());
  auto *globalVar = llvm::dyn_cast<llvm::GlobalVariable>(global);
  if (!globalVar->hasInitializer()) {
    globalVar->setInitializer(Str);
  }
  llvm::Constant *zero = llvm::ConstantInt::get(llvm::Type::getInt64Ty(Ctx), 0);
  auto indices = std::vector<llvm::Constant *>{zero, zero};
  auto Ptr = llvm::ConstantExpr::getInBoundsGetElementPtr(
      Str->getType(), globalVar, indices);
  return Ptr;
}

static llvm::Function *getStrcmp(llvm::Module *module) {
  llvm::LLVMContext &Ctx = module->getContext();
  auto type = llvm::FunctionType::get(
      llvm::Type::getInt32Ty(Ctx),
      {llvm::Type::getInt8PtrTy(Ctx), llvm::Type::getInt8PtrTy(Ctx)}, false);
  return getOrInsertFunction(module, "strcmp", type);
}

static llvm::Function *getPuts(llvm::Module *module) {
  llvm::LLVMContext &Ctx = module->getContext();
  auto type = llvm::FunctionType::get(
      llvm::Type::getInt32Ty(Ctx), {llvm::Type::getInt8PtrTy(Ctx)}, false);
  return getOrInsertFunction(module, "puts", type);
}

static void
emitGetTagForSymbolName(KOREDefinition *definition, llvm::Module *module) {
  llvm::LLVMContext &Ctx = module->getContext();
  auto type = llvm::FunctionType::get(
      llvm::Type::getInt32Ty(Ctx), {llvm::Type::getInt8PtrTy(Ctx)}, false);
  auto func = getOrInsertFunction(module, "getTagForSymbolNameInternal", type);
  auto CurrentBlock = llvm::BasicBlock::Create(Ctx, "");
  auto MergeBlock = llvm::BasicBlock::Create(Ctx, "exit");
  auto Phi = llvm::PHINode::Create(
      llvm::Type::getInt32Ty(Ctx), definition->getSymbols().size(), "phi",
      MergeBlock);
  auto &syms = definition->getAllSymbols();
  llvm::Function *Strcmp = getStrcmp(module);
  for (const auto & entry : syms) {
    uint32_t tag = entry.second->getTag();
    auto symbol = entry.second;
    CurrentBlock->insertInto(func);
    auto Ptr = getSymbolNamePtr(symbol, CurrentBlock, module);
    auto compare = llvm::CallInst::Create(
        Strcmp, {func->arg_begin(), Ptr}, "", CurrentBlock);
    auto icmp = new llvm::ICmpInst(
        *CurrentBlock, llvm::CmpInst::ICMP_EQ, compare,
        llvm::ConstantInt::get(llvm::Type::getInt32Ty(Ctx), 0));
    auto FalseBlock = llvm::BasicBlock::Create(Ctx, "");
    llvm::BranchInst::Create(MergeBlock, FalseBlock, icmp, CurrentBlock);
    Phi->addIncoming(
        llvm::ConstantInt::get(llvm::Type::getInt32Ty(Ctx), tag), CurrentBlock);
    CurrentBlock = FalseBlock;
  }
  llvm::Function *Puts = getPuts(module);
  llvm::CallInst::Create(Puts, {func->arg_begin()}, "", CurrentBlock);
  Phi->addIncoming(
      llvm::ConstantInt::get(llvm::Type::getInt32Ty(Ctx), ERROR_TAG),
      CurrentBlock);
  llvm::BranchInst::Create(MergeBlock, CurrentBlock);
  CurrentBlock->insertInto(func);
  llvm::ReturnInst::Create(Ctx, Phi, MergeBlock);
  MergeBlock->insertInto(func);
}

static std::string STRING_STRUCT = "string";
static std::string LAYOUT_STRUCT = "layout";
static std::string LAYOUTITEM_STRUCT = "layoutitem";

static void emitDataTableForSymbol(
    std::string name, llvm::Type *ty, llvm::DIType *dity,
    KOREDefinition *definition, llvm::Module *module,
    llvm::Constant *getter(KOREDefinition *, llvm::Module *, KORESymbol *)) {
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
  auto *globalVar = llvm::cast<llvm::GlobalVariable>(table);
  initDebugGlobal(
      "table_" + name,
      getArrayDebugType(
          dity, syms.size(), llvm::DataLayout(module).getABITypeAlign(ty)),
      globalVar);
  std::vector<llvm::Constant *> values;
  for (auto entry : syms) {
    auto symbol = entry.second;
    auto val = getter(definition, module, symbol);
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

static void emitDataForSymbol(
    std::string name, llvm::Type *ty, llvm::DIType *dity,
    KOREDefinition *definition, llvm::Module *module, bool isEval,
    std::pair<llvm::Value *, llvm::BasicBlock *> getter(
        KOREDefinition *, llvm::Module *, KORESymbol *, llvm::Instruction *)) {
  llvm::LLVMContext &Ctx = module->getContext();
  std::vector<llvm::Type *> argTypes;
  argTypes.push_back(llvm::Type::getInt32Ty(Ctx));
  if (isEval) {
    auto ty = llvm::PointerType::getUnqual(
        llvm::ArrayType::get(llvm::Type::getInt8PtrTy(Ctx), 0));
    argTypes.push_back(ty);
  }
  auto func = getOrInsertFunction(
      module, name, llvm::FunctionType::get(ty, argTypes, false));
  if (!isEval) {
    initDebugFunction(
        name, name, getDebugFunctionType(dity, {getIntDebugType()}), definition,
        func);
  }
  auto EntryBlock = llvm::BasicBlock::Create(Ctx, "entry", func);
  auto MergeBlock = llvm::BasicBlock::Create(Ctx, "exit");
  auto stuck = llvm::BasicBlock::Create(Ctx, "stuck");
  auto &syms = definition->getSymbols();
  auto Switch = llvm::SwitchInst::Create(
      func->arg_begin(), stuck, syms.size(), EntryBlock);
  auto Phi = llvm::PHINode::Create(
      ty, definition->getSymbols().size(), "phi", MergeBlock);
  for (auto entry : syms) {
    uint32_t tag = entry.first;
    auto symbol = entry.second;
    auto decl = definition->getSymbolDeclarations().at(symbol->getName());
    bool isFunc = decl->getAttributes().count("function")
                  || decl->getAttributes().count("anywhere");
    if (isEval && !isFunc) {
      continue;
    }
    auto CaseBlock
        = llvm::BasicBlock::Create(Ctx, "tag" + std::to_string(tag), func);
    auto Branch = llvm::BranchInst::Create(MergeBlock, CaseBlock);
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
    KOREDefinition *definition, llvm::Module *module, KORESymbol *symbol,
    llvm::Instruction *inst) {
  auto BlockType = getBlockType(module, definition, symbol);
  return std::make_pair(
      getBlockHeader(module, definition, symbol, BlockType), inst->getParent());
}

static void
emitGetBlockHeaderForSymbol(KOREDefinition *def, llvm::Module *mod) {
  emitDataForSymbol(
      "getBlockHeaderForSymbol",
      llvm::StructType::getTypeByName(mod->getContext(), BLOCKHEADER_STRUCT),
      getForwardDecl(BLOCKHEADER_STRUCT), def, mod, false, getHeader);
}

static std::pair<llvm::Value *, llvm::BasicBlock *> getFunction(
    KOREDefinition *def, llvm::Module *mod, KORESymbol *symbol,
    llvm::Instruction *inst) {
  auto decl = def->getSymbolDeclarations().at(symbol->getName());
  bool res = decl->getAttributes().count("function")
             || decl->getAttributes().count("anywhere");
  return std::make_pair(
      llvm::ConstantInt::get(llvm::Type::getInt1Ty(mod->getContext()), res),
      inst->getParent());
}

static void emitIsSymbolAFunction(KOREDefinition *def, llvm::Module *mod) {
  emitDataForSymbol(
      "isSymbolAFunction", llvm::Type::getInt1Ty(mod->getContext()),
      getBoolDebugType(), def, mod, false, getFunction);
}

static llvm::Constant *
getBinder(KOREDefinition *def, llvm::Module *mod, KORESymbol *symbol) {
  auto decl = def->getSymbolDeclarations().at(symbol->getName());
  bool res = decl->getAttributes().count("binder");
  return llvm::ConstantInt::get(llvm::Type::getInt1Ty(mod->getContext()), res);
}

static void emitIsSymbolABinder(KOREDefinition *def, llvm::Module *mod) {
  emitDataTableForSymbol(
      "isSymbolABinder", llvm::Type::getInt1Ty(mod->getContext()),
      getBoolDebugType(), def, mod, getBinder);
}

static std::pair<llvm::Value *, llvm::BasicBlock *> getInjection(
    KOREDefinition *def, llvm::Module *mod, KORESymbol *symbol,
    llvm::Instruction *inst) {
  llvm::Constant *tag
      = llvm::ConstantInt::get(llvm::Type::getInt32Ty(mod->getContext()), 0);
  for (auto sym : def->getSymbols()) {
    if (sym.second->getName() == "inj"
        && *sym.second->getArguments()[0] == *symbol->getSort()) {
      tag = llvm::ConstantInt::get(
          llvm::Type::getInt32Ty(mod->getContext()), sym.first);
      break;
    }
  }
  return std::make_pair(tag, inst->getParent());
}

static void
emitGetInjectionForSortOfTag(KOREDefinition *def, llvm::Module *mod) {
  emitDataForSymbol(
      "getInjectionForSortOfTag", llvm::Type::getInt32Ty(mod->getContext()),
      getIntDebugType(), def, mod, false, getInjection);
}

static llvm::Constant *
getArity(KOREDefinition *, llvm::Module *mod, KORESymbol *symbol) {
  return llvm::ConstantInt::get(
      llvm::Type::getInt32Ty(mod->getContext()), symbol->getArguments().size());
}

static void emitGetSymbolArity(KOREDefinition *def, llvm::Module *mod) {
  emitDataTableForSymbol(
      "getSymbolArity", llvm::Type::getInt32Ty(mod->getContext()),
      getIntDebugType(), def, mod, getArity);
}

static llvm::Value *getArgValue(
    llvm::Value *ArgumentsArray, int idx, llvm::BasicBlock *CaseBlock,
    ValueType cat, llvm::Module *mod) {
  llvm::LLVMContext &Ctx = mod->getContext();
  llvm::Constant *zero = llvm::ConstantInt::get(llvm::Type::getInt64Ty(Ctx), 0);

  auto i8_ptr_ty = llvm::Type::getInt8PtrTy(Ctx);

  auto addr = llvm::GetElementPtrInst::Create(
      llvm::ArrayType::get(i8_ptr_ty, 0), ArgumentsArray,
      {zero, llvm::ConstantInt::get(llvm::Type::getInt64Ty(Ctx), idx)}, "",
      CaseBlock);
  llvm::Value *arg = new llvm::LoadInst(i8_ptr_ty, addr, "", CaseBlock);

  switch (cat.cat) {
  case SortCategory::Bool:
  case SortCategory::MInt: {
    auto val_ty = getValueType(cat, mod);
    auto cast = new llvm::BitCastInst(
        arg, llvm::PointerType::getUnqual(val_ty), "", CaseBlock);
    auto load = new llvm::LoadInst(val_ty, cast, "", CaseBlock);
    arg = load;
    break;
  }
  case SortCategory::Map:
  case SortCategory::RangeMap:
  case SortCategory::List:
  case SortCategory::Set:
    arg = new llvm::BitCastInst(
        arg, llvm::PointerType::getUnqual(getValueType(cat, mod)), "",
        CaseBlock);
    break;
  case SortCategory::Int:
  case SortCategory::Float:
  case SortCategory::StringBuffer:
  case SortCategory::Symbol:
  case SortCategory::Variable:
    arg = new llvm::BitCastInst(arg, getValueType(cat, mod), "", CaseBlock);
    break;
  case SortCategory::Uncomputed: abort();
  }
  return arg;
}

static std::pair<llvm::Value *, llvm::BasicBlock *> getEval(
    KOREDefinition *def, llvm::Module *mod, KORESymbol *symbol,
    llvm::Instruction *inst) {
  llvm::LLVMContext &Ctx = mod->getContext();
  llvm::BasicBlock *CaseBlock = inst->getParent();
  inst->removeFromParent();
  llvm::Function *func = CaseBlock->getParent();
  llvm::Value *ArgumentsArray = func->arg_begin() + 1;
  int idx = 0;
  llvm::StringMap<llvm::Value *> subst;
  auto pattern = KORECompositePattern::Create(symbol);
  for (auto &sort : symbol->getArguments()) {
    ValueType cat
        = dynamic_cast<KORECompositeSort *>(sort.get())->getCategory(def);
    llvm::Value *arg = getArgValue(ArgumentsArray, idx, CaseBlock, cat, mod);
    std::string name = "_" + std::to_string(idx++);
    subst.insert({name, arg});
    pattern->addArgument(KOREVariablePattern::Create(name, sort));
  }
  CreateTerm creator(subst, def, CaseBlock, mod, false);
  llvm::Value *result = creator(pattern.get()).first;
  llvm::Value *retval;
  ValueType cat = dynamic_cast<KORECompositeSort *>(symbol->getSort().get())
                      ->getCategory(def);
  switch (cat.cat) {
  case SortCategory::Int:
  case SortCategory::Float:
  case SortCategory::StringBuffer:
  case SortCategory::Symbol:
  case SortCategory::Variable:
  case SortCategory::Map:
  case SortCategory::RangeMap:
  case SortCategory::List:
  case SortCategory::Set:
    retval = new llvm::BitCastInst(
        result, llvm::Type::getInt8PtrTy(Ctx), "", creator.getCurrentBlock());
    break;
  case SortCategory::Bool:
  case SortCategory::MInt: {
    llvm::Instruction *Malloc = llvm::CallInst::CreateMalloc(
        creator.getCurrentBlock(), llvm::Type::getInt64Ty(Ctx),
        result->getType(), llvm::ConstantExpr::getSizeOf(result->getType()),
        nullptr, nullptr);
    Malloc->insertAfter(&creator.getCurrentBlock()->back());
    new llvm::StoreInst(result, Malloc, creator.getCurrentBlock());
    retval = new llvm::BitCastInst(
        Malloc, llvm::Type::getInt8PtrTy(Ctx), "", creator.getCurrentBlock());
    break;
  }
  case SortCategory::Uncomputed: abort();
  }
  inst->insertAfter(&creator.getCurrentBlock()->back());
  return std::make_pair(retval, creator.getCurrentBlock());
}

static void emitEvaluateFunctionSymbol(KOREDefinition *def, llvm::Module *mod) {
  emitDataForSymbol(
      "evaluateFunctionSymbol", llvm::Type::getInt8PtrTy(mod->getContext()),
      nullptr, def, mod, true, getEval);
}

static void
emitGetTagForFreshSort(KOREDefinition *definition, llvm::Module *module) {
  llvm::LLVMContext &Ctx = module->getContext();
  auto func = getOrInsertFunction(
      module, "getTagForFreshSort", llvm::Type::getInt32Ty(Ctx),
      llvm::Type::getInt8PtrTy(Ctx));
  auto CurrentBlock = llvm::BasicBlock::Create(Ctx, "");
  auto MergeBlock = llvm::BasicBlock::Create(Ctx, "exit");
  auto Phi = llvm::PHINode::Create(
      llvm::Type::getInt32Ty(Ctx), definition->getSortDeclarations().size(),
      "phi", MergeBlock);
  auto &sorts = definition->getSortDeclarations();
  llvm::Function *Strcmp = getStrcmp(module);
  llvm::Constant *zero = llvm::ConstantInt::get(llvm::Type::getInt64Ty(Ctx), 0);
  llvm::Constant *zero32
      = llvm::ConstantInt::get(llvm::Type::getInt32Ty(Ctx), 0);
  bool hasCase = false;
  for (const auto & entry : sorts) {
    std::string name = entry.first;
    if (!definition->getFreshFunctions().count(name)) {
      continue;
    }
    hasCase = true;
    CurrentBlock->insertInto(func);
    CurrentBlock->setName("is_" + name);
    auto Str = llvm::ConstantDataArray::getString(Ctx, name, true);
    auto global
        = module->getOrInsertGlobal("sort_name_" + name, Str->getType());
    auto *globalVar = llvm::cast<llvm::GlobalVariable>(global);
    if (!globalVar->hasInitializer()) {
      globalVar->setInitializer(Str);
    }
    auto indices = std::vector<llvm::Constant *>{zero, zero};
    auto Ptr = llvm::ConstantExpr::getInBoundsGetElementPtr(
        Str->getType(), globalVar, indices);
    auto compare = llvm::CallInst::Create(
        Strcmp, {func->arg_begin(), Ptr}, "", CurrentBlock);
    auto icmp = new llvm::ICmpInst(
        *CurrentBlock, llvm::CmpInst::ICMP_EQ, compare, zero32);
    auto FalseBlock = llvm::BasicBlock::Create(Ctx, "");
    auto CaseBlock = llvm::BasicBlock::Create(Ctx, name, func);
    llvm::BranchInst::Create(CaseBlock, FalseBlock, icmp, CurrentBlock);
    auto symbol = definition->getFreshFunctions().at(name);
    Phi->addIncoming(
        llvm::ConstantInt::get(
            llvm::Type::getInt32Ty(Ctx),
            definition->getAllSymbols().at(ast_to_string(*symbol))->getTag()),
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

static void emitGetToken(KOREDefinition *definition, llvm::Module *module) {
  llvm::LLVMContext &Ctx = module->getContext();
  auto getTokenType = llvm::FunctionType::get(
      llvm::Type::getInt8PtrTy(Ctx),
      {llvm::Type::getInt8PtrTy(Ctx), llvm::Type::getInt64Ty(Ctx),
       llvm::Type::getInt8PtrTy(Ctx)},
      false);
  auto func = getOrInsertFunction(module, "getToken", getTokenType);
  auto CurrentBlock = llvm::BasicBlock::Create(Ctx, "");
  auto MergeBlock = llvm::BasicBlock::Create(Ctx, "exit");
  auto Phi = llvm::PHINode::Create(
      llvm::Type::getInt8PtrTy(Ctx), definition->getSortDeclarations().size(),
      "phi", MergeBlock);
  auto &sorts = definition->getSortDeclarations();
  llvm::Function *Strcmp = getStrcmp(module);
  llvm::Function *StringEqual = getOrInsertFunction(
      module, "string_equal", llvm::Type::getInt1Ty(Ctx),
      llvm::Type::getInt8PtrTy(Ctx), llvm::Type::getInt8PtrTy(Ctx),
      llvm::Type::getInt64Ty(Ctx), llvm::Type::getInt64Ty(Ctx));
  llvm::Constant *zero = llvm::ConstantInt::get(llvm::Type::getInt64Ty(Ctx), 0);
  llvm::Constant *zero32
      = llvm::ConstantInt::get(llvm::Type::getInt32Ty(Ctx), 0);
  for (const auto & entry : sorts) {
    std::string name = entry.first;
    if (!entry.second->getObjectSortVariables().empty()) {
      // TODO: MINT in initial configuration
      continue;
    }
    auto sort = KORECompositeSort::Create(name);
    ValueType cat = sort->getCategory(definition);
    if (cat.cat == SortCategory::Symbol || cat.cat == SortCategory::Variable) {
      continue;
    }
    CurrentBlock->insertInto(func);
    CurrentBlock->setName("is_" + name);
    auto Str = llvm::ConstantDataArray::getString(Ctx, name, true);
    auto global
        = module->getOrInsertGlobal("sort_name_" + name, Str->getType());
    auto *globalVar = llvm::dyn_cast<llvm::GlobalVariable>(global);
    if (!globalVar->hasInitializer()) {
      globalVar->setInitializer(Str);
    }
    auto indices = std::vector<llvm::Constant *>{zero, zero};
    auto Ptr = llvm::ConstantExpr::getInBoundsGetElementPtr(
        Str->getType(), globalVar, indices);
    auto compare = llvm::CallInst::Create(
        Strcmp, {func->arg_begin(), Ptr}, "", CurrentBlock);
    auto icmp = new llvm::ICmpInst(
        *CurrentBlock, llvm::CmpInst::ICMP_EQ, compare, zero32);
    auto FalseBlock = llvm::BasicBlock::Create(Ctx, "");
    auto CaseBlock = llvm::BasicBlock::Create(Ctx, name, func);
    llvm::BranchInst::Create(CaseBlock, FalseBlock, icmp, CurrentBlock);
    switch (cat.cat) {
    case SortCategory::Map:
    case SortCategory::RangeMap:
    case SortCategory::List:
    case SortCategory::Set: addAbort(CaseBlock, module); break;
    case SortCategory::StringBuffer:
    case SortCategory::MInt:
      // TODO: tokens
      addAbort(CaseBlock, module);
      break;
    case SortCategory::Bool: {
      auto Str = llvm::ConstantDataArray::getString(Ctx, "true", false);
      auto global = module->getOrInsertGlobal("bool_true", Str->getType());
      auto *globalVar = llvm::dyn_cast<llvm::GlobalVariable>(global);
      if (!globalVar->hasInitializer()) {
        globalVar->setInitializer(Str);
      }
      auto Ptr = llvm::ConstantExpr::getInBoundsGetElementPtr(
          Str->getType(), globalVar, indices);
      auto Len = llvm::ConstantInt::get(llvm::Type::getInt64Ty(Ctx), 4);
      auto compare = llvm::CallInst::Create(
          StringEqual, {func->arg_begin() + 2, Ptr, func->arg_begin() + 1, Len},
          "", CaseBlock);
      llvm::Instruction *Malloc = llvm::CallInst::CreateMalloc(
          CaseBlock, llvm::Type::getInt64Ty(Ctx), compare->getType(),
          llvm::ConstantExpr::getSizeOf(compare->getType()), nullptr, nullptr);
      Malloc->insertAfter(&CaseBlock->back());
      new llvm::StoreInst(compare, Malloc, CaseBlock);
      auto result = new llvm::BitCastInst(
          Malloc, llvm::Type::getInt8PtrTy(Ctx), "", CaseBlock);
      Phi->addIncoming(result, CaseBlock);
      llvm::BranchInst::Create(MergeBlock, CaseBlock);
      break;
    }
    case SortCategory::Float: {
      llvm::Type *Float
          = llvm::StructType::getTypeByName(module->getContext(), FLOAT_STRUCT);
      llvm::Value *Term = allocateTerm(Float, CaseBlock, "koreAllocFloating");
      llvm::Function *InitFloat = getOrInsertFunction(
          module, "init_float", llvm::Type::getVoidTy(Ctx),
          llvm::PointerType::getUnqual(Float), llvm::Type::getInt8PtrTy(Ctx));
      llvm::CallInst::Create(
          InitFloat, {Term, func->arg_begin() + 2}, "", CaseBlock);
      auto cast = new llvm::BitCastInst(
          Term, llvm::Type::getInt8PtrTy(Ctx), "", CaseBlock);
      Phi->addIncoming(cast, CaseBlock);
      llvm::BranchInst::Create(MergeBlock, CaseBlock);
      break;
    }
    case SortCategory::Int: {
      const auto &thirdArg = func->arg_begin() + 2;
      llvm::Value *FirstChar = new llvm::LoadInst(
          llvm::Type::getInt8Ty(Ctx), thirdArg, "", CaseBlock);
      llvm::Constant *asciiPlus
          = llvm::ConstantInt::get(llvm::Type::getInt8Ty(Ctx), 43);
      auto icmpFirst = new llvm::ICmpInst(
          *CaseBlock, llvm::CmpInst::ICMP_EQ, FirstChar, asciiPlus);
      auto IfIsPlus = llvm::BasicBlock::Create(Ctx, "if_is_plus", func);
      auto ElseNoPlus = llvm::BasicBlock::Create(Ctx, "else_no_plus", func);
      llvm::BranchInst::Create(IfIsPlus, ElseNoPlus, icmpFirst, CaseBlock);
      llvm::Constant *one
          = llvm::ConstantInt::get(llvm::Type::getInt32Ty(Ctx), 1);
      llvm::Value *Pruned = llvm::GetElementPtrInst::CreateInBounds(
          llvm::Type::getInt8Ty(Ctx), func->arg_begin() + 2, {one}, "",
          IfIsPlus);
      llvm::BranchInst::Create(ElseNoPlus, IfIsPlus);
      auto phiStr = llvm::PHINode::Create(
          llvm::Type::getInt8PtrTy(Ctx), 2, "", ElseNoPlus);
      phiStr->addIncoming(func->arg_begin() + 2, CaseBlock);
      phiStr->addIncoming(Pruned, IfIsPlus);
      CaseBlock = ElseNoPlus;
      llvm::Type *Int
          = llvm::StructType::getTypeByName(module->getContext(), INT_STRUCT);
      llvm::Value *Term = allocateTerm(Int, CaseBlock, "koreAllocInteger");
      llvm::Function *MpzInitSet = getOrInsertFunction(
          module, "__gmpz_init_set_str", llvm::Type::getInt32Ty(Ctx),
          llvm::PointerType::getUnqual(Int), llvm::Type::getInt8PtrTy(Ctx),
          llvm::Type::getInt32Ty(Ctx));
      auto Call = llvm::CallInst::Create(
          MpzInitSet,
          {Term, phiStr,
           llvm::ConstantInt::get(llvm::Type::getInt32Ty(Ctx), 10)},
          "", CaseBlock);
      auto icmp = new llvm::ICmpInst(
          *CaseBlock, llvm::CmpInst::ICMP_EQ, Call, zero32);
      auto AbortBlock = llvm::BasicBlock::Create(Ctx, "invalid_int", func);
      addAbort(AbortBlock, module);
      auto cast = new llvm::BitCastInst(
          Term, llvm::Type::getInt8PtrTy(Ctx), "", CaseBlock);
      llvm::BranchInst::Create(MergeBlock, AbortBlock, icmp, CaseBlock);
      Phi->addIncoming(cast, CaseBlock);
      break;
    }
    case SortCategory::Variable:
    case SortCategory::Symbol: break;
    case SortCategory::Uncomputed: abort();
    }
    CurrentBlock = FalseBlock;
  }
  CurrentBlock->setName("symbol");
  CurrentBlock->insertInto(func);
  auto StringType
      = llvm::StructType::getTypeByName(module->getContext(), STRING_STRUCT);
  auto Len = llvm::BinaryOperator::Create(
      llvm::Instruction::Add, func->arg_begin() + 1,
      llvm::ConstantExpr::getSizeOf(StringType), "", CurrentBlock);
  llvm::Value *Block
      = allocateTerm(StringType, Len, CurrentBlock, "koreAllocToken");
  auto HdrPtr = llvm::GetElementPtrInst::CreateInBounds(
      StringType, Block, {zero, zero32, zero32}, "", CurrentBlock);
  auto BlockSize
      = module->getOrInsertGlobal("BLOCK_SIZE", llvm::Type::getInt64Ty(Ctx));
  auto BlockSizeVal = new llvm::LoadInst(
      llvm::Type::getInt64Ty(Ctx), BlockSize, "", CurrentBlock);
  auto BlockAllocSize = llvm::BinaryOperator::Create(
      llvm::Instruction::Sub, BlockSizeVal,
      llvm::ConstantExpr::getSizeOf(llvm::Type::getInt8PtrTy(Ctx)), "",
      CurrentBlock);
  auto icmp = new llvm::ICmpInst(
      *CurrentBlock, llvm::CmpInst::ICMP_UGT, Len, BlockAllocSize);
  auto Mask = llvm::SelectInst::Create(
      icmp,
      llvm::ConstantInt::get(llvm::Type::getInt64Ty(Ctx), NOT_YOUNG_OBJECT_BIT),
      llvm::ConstantInt::get(llvm::Type::getInt64Ty(Ctx), 0), "", CurrentBlock);
  auto HdrOred = llvm::BinaryOperator::Create(
      llvm::Instruction::Or, func->arg_begin() + 1, Mask, "", CurrentBlock);
  new llvm::StoreInst(HdrOred, HdrPtr, CurrentBlock);
  llvm::Function *Memcpy = getOrInsertFunction(
      module, "memcpy", llvm::Type::getInt8PtrTy(Ctx),
      llvm::Type::getInt8PtrTy(Ctx), llvm::Type::getInt8PtrTy(Ctx),
      llvm::Type::getInt64Ty(Ctx));
  auto StrPtr = llvm::GetElementPtrInst::CreateInBounds(
      StringType, Block,
      {zero, llvm::ConstantInt::get(llvm::Type::getInt32Ty(Ctx), 1), zero}, "",
      CurrentBlock);
  llvm::CallInst::Create(
      Memcpy, {StrPtr, func->arg_begin() + 2, func->arg_begin() + 1}, "",
      CurrentBlock);
  auto cast = new llvm::BitCastInst(
      Block, llvm::Type::getInt8PtrTy(Ctx), "", CurrentBlock);
  llvm::BranchInst::Create(MergeBlock, CurrentBlock);
  Phi->addIncoming(cast, CurrentBlock);
  llvm::ReturnInst::Create(Ctx, Phi, MergeBlock);
  MergeBlock->insertInto(func);
}

static llvm::PointerType *makeWriterType(llvm::LLVMContext &Ctx) {
  const std::string name = "writer";
  static auto types = std::map<llvm::LLVMContext *, llvm::PointerType *>{};

  if (types.find(&Ctx) == types.end()) {
    types[&Ctx]
        = llvm::PointerType::getUnqual(llvm::StructType::create(Ctx, name));
  }

  return types.at(&Ctx);
}

static llvm::PointerType *makeVisitorType(
    llvm::LLVMContext &Ctx, llvm::Type *file, llvm::Type *item, int numStrs,
    int numBools) {
  std::vector<llvm::Type *> types;
  types.push_back(file);
  types.push_back(item);
  for (int i = 0; i < numStrs; i++) {
    types.push_back(llvm::Type::getInt8PtrTy(Ctx));
  }
  for (int i = 0; i < numBools; i++) {
    types.push_back(llvm::Type::getInt1Ty(Ctx));
  }

  types.push_back(llvm::Type::getInt8PtrTy(Ctx));

  return llvm::PointerType::getUnqual(
      llvm::FunctionType::get(llvm::Type::getVoidTy(Ctx), types, false));
}

static llvm::StructType *
makePackedVisitorStructureType(llvm::LLVMContext &Ctx, llvm::Module *module) {
  const std::string name = "visitor";
  static auto types = std::map<llvm::LLVMContext *, llvm::StructType *>{};

  auto file = makeWriterType(Ctx);

  if (types.find(&Ctx) == types.end()) {
    auto elementTypes = std::vector<llvm::Type *>{
        {makeVisitorType(
             Ctx, file, getValueType({SortCategory::Symbol, 0}, module), 1, 1),
         makeVisitorType(
             Ctx, file,
             llvm::PointerType::getUnqual(
                 getValueType({SortCategory::Map, 0}, module)),
             3, 0),
         makeVisitorType(
             Ctx, file,
             llvm::PointerType::getUnqual(
                 getValueType({SortCategory::List, 0}, module)),
             3, 0),
         makeVisitorType(
             Ctx, file,
             llvm::PointerType::getUnqual(
                 getValueType({SortCategory::Set, 0}, module)),
             3, 0),
         makeVisitorType(
             Ctx, file, getValueType({SortCategory::Int, 0}, module), 1, 0),
         makeVisitorType(
             Ctx, file, getValueType({SortCategory::Float, 0}, module), 1, 0),
         makeVisitorType(
             Ctx, file, getValueType({SortCategory::Bool, 0}, module), 1, 0),
         makeVisitorType(
             Ctx, file, getValueType({SortCategory::StringBuffer, 0}, module),
             1, 0),
         llvm::PointerType::getUnqual(llvm::FunctionType::get(
             llvm::Type::getVoidTy(Ctx),
             {file, llvm::Type::getInt64PtrTy(Ctx), llvm::Type::getInt64Ty(Ctx),
              llvm::Type::getInt8PtrTy(Ctx), llvm::Type::getInt8PtrTy(Ctx)},
             false)),
         llvm::PointerType::getUnqual(llvm::FunctionType::get(
             llvm::Type::getVoidTy(Ctx), {file, llvm::Type::getInt8PtrTy(Ctx)},
             false)),
         makeVisitorType(
             Ctx, file,
             llvm::PointerType::getUnqual(
                 getValueType({SortCategory::RangeMap, 0}, module)),
             3, 0)}};

    auto structTy = llvm::StructType::create(Ctx, elementTypes, name);
    types[&Ctx] = structTy;
  }

  return types.at(&Ctx);
}

static void emitTraversal(
    std::string name, KOREDefinition *definition, llvm::Module *module,
    bool isVisitor,
    void getter(
        KOREDefinition *, llvm::Module *, KORESymbol *, llvm::BasicBlock *,
        std::vector<llvm::Value *> const &callbacks)) {
  llvm::LLVMContext &Ctx = module->getContext();
  std::vector<llvm::Type *> argTypes;

  auto block_ty = getBlockType(module);
  argTypes.push_back(block_ty->getPointerTo());

  if (isVisitor) {
    // cf runtime/util/header.h visitChildren
    auto file = makeWriterType(Ctx);
    argTypes.push_back(file);
    argTypes.push_back(
        makePackedVisitorStructureType(Ctx, module)->getPointerTo());
    argTypes.push_back(llvm::Type::getInt8PtrTy(Ctx));
  } else {
    argTypes.push_back(llvm::PointerType::getUnqual(
        llvm::ArrayType::get(llvm::Type::getInt8PtrTy(Ctx), 0)));
  }

  auto func = llvm::cast<llvm::Function>(getOrInsertFunction(
      module, name,
      llvm::FunctionType::get(llvm::Type::getVoidTy(Ctx), argTypes, false)));
  llvm::Constant *zero = llvm::ConstantInt::get(llvm::Type::getInt64Ty(Ctx), 0);
  llvm::Constant *zero32
      = llvm::ConstantInt::get(llvm::Type::getInt32Ty(Ctx), 0);
  auto EntryBlock = llvm::BasicBlock::Create(Ctx, "entry", func);

  auto HdrPtr = llvm::GetElementPtrInst::CreateInBounds(
      block_ty, func->arg_begin(), {zero, zero32, zero32}, "", EntryBlock);
  auto Hdr
      = new llvm::LoadInst(llvm::Type::getInt64Ty(Ctx), HdrPtr, "", EntryBlock);
  auto Tag
      = new llvm::TruncInst(Hdr, llvm::Type::getInt32Ty(Ctx), "", EntryBlock);

  auto callbacks = std::vector<llvm::Value *>{};
  if (isVisitor) {
    auto visitorStruct = func->getArg(2);
    auto visitorType = makePackedVisitorStructureType(Ctx, module);

    for (auto i = 0; i < visitorType->getNumElements(); ++i) {
      auto ptr = llvm::GetElementPtrInst::CreateInBounds(
          visitorType, visitorStruct,
          {zero, llvm::ConstantInt::get(llvm::Type::getInt32Ty(Ctx), i)}, "",
          EntryBlock);

      callbacks.push_back(new llvm::LoadInst(
          visitorType->getTypeAtIndex(i), ptr, "", EntryBlock));
    }
  }

  auto stuck = llvm::BasicBlock::Create(Ctx, "stuck");
  auto &syms = definition->getSymbols();
  auto Switch = llvm::SwitchInst::Create(Tag, stuck, syms.size(), EntryBlock);

  for (auto entry : syms) {
    uint32_t tag = entry.first;
    auto symbol = entry.second;
    if (symbol->getArguments().empty()) {
      continue;
    }
    auto CaseBlock
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
    KOREDefinition *definition, llvm::Module *module, KORESymbol *symbol,
    llvm::BasicBlock *CaseBlock, std::vector<llvm::Value *> const &) {
  llvm::LLVMContext &Ctx = module->getContext();
  llvm::Constant *zero = llvm::ConstantInt::get(llvm::Type::getInt64Ty(Ctx), 0);
  llvm::Function *func = CaseBlock->getParent();
  llvm::Value *ArgumentsArray = func->arg_begin() + 1;
  int idx = 0;
  auto BlockType = getBlockType(module, definition, symbol);
  auto cast = new llvm::BitCastInst(
      func->arg_begin(), llvm::PointerType::getUnqual(BlockType), "",
      CaseBlock);
  for (auto &sort : symbol->getArguments()) {
    ValueType cat = dynamic_cast<KORECompositeSort *>(sort.get())
                        ->getCategory(definition);
    llvm::Value *arg = getArgValue(ArgumentsArray, idx, CaseBlock, cat, module);
    llvm::Type *arg_ty = getArgType(cat, module);
    llvm::Value *ChildPtr = llvm::GetElementPtrInst::CreateInBounds(
        BlockType, cast,
        {zero, llvm::ConstantInt::get(llvm::Type::getInt32Ty(Ctx), idx++ + 2)},
        "", CaseBlock);
    if (isCollectionSort(cat)) {
      arg = new llvm::LoadInst(arg_ty, arg, "", CaseBlock);
    }
    new llvm::StoreInst(arg, ChildPtr, CaseBlock);
  }
}

static void
emitStoreSymbolChildren(KOREDefinition *definition, llvm::Module *module) {
  emitTraversal("storeSymbolChildren", definition, module, false, getStore);
}

static llvm::Constant *getSymbolName(
    KOREDefinition *definition, llvm::Module *module, KORESymbol *symbol) {
  return getSymbolNamePtr(symbol, nullptr, module);
}

static void emitGetSymbolNameForTag(KOREDefinition *def, llvm::Module *mod) {
  emitDataTableForSymbol(
      "getSymbolNameForTag", llvm::Type::getInt8PtrTy(mod->getContext()),
      getCharPtrDebugType(), def, mod, getSymbolName);
}

static void visitCollection(
    KOREDefinition *definition, llvm::Module *module,
    KORECompositeSort *compositeSort, llvm::Function *func,
    llvm::Value *ChildPtr, llvm::BasicBlock *CaseBlock, llvm::Value *callback,
    llvm::Value *state_ptr) {
  llvm::LLVMContext &Ctx = module->getContext();
  llvm::Constant *zero = llvm::ConstantInt::get(llvm::Type::getInt64Ty(Ctx), 0);
  auto indices = std::vector<llvm::Constant *>{zero, zero};
  auto sortDecl
      = definition->getSortDeclarations().at(compositeSort->getName());
  llvm::Constant *concatPtr;
  if (sortDecl->getAttributes().count("concat")) {
    auto concat = (KORECompositePattern *)sortDecl->getAttributes()
                      .at("concat")
                      ->getArguments()[0]
                      .get();
    concatPtr = getSymbolNamePtr(concat->getConstructor(), nullptr, module);
  } else {
    concatPtr = llvm::ConstantPointerNull::get(llvm::Type::getInt8PtrTy(Ctx));
  }
  auto unit = (KORECompositePattern *)sortDecl->getAttributes()
                  .at("unit")
                  ->getArguments()[0]
                  .get();
  auto unitPtr = getSymbolNamePtr(unit->getConstructor(), nullptr, module);
  auto element = (KORECompositePattern *)sortDecl->getAttributes()
                     .at("element")
                     ->getArguments()[0]
                     .get();
  auto elementPtr
      = getSymbolNamePtr(element->getConstructor(), nullptr, module);
  auto file = makeWriterType(Ctx);
  auto i8_ptr_ty = llvm::Type::getInt8PtrTy(Ctx);
  auto fnType = llvm::FunctionType::get(
      llvm::Type::getVoidTy(Ctx),
      {file, ChildPtr->getType(), i8_ptr_ty, i8_ptr_ty, i8_ptr_ty, i8_ptr_ty},
      false);
  llvm::CallInst::Create(
      fnType, callback,
      {func->arg_begin() + 1, ChildPtr, unitPtr, elementPtr, concatPtr,
       state_ptr},
      "", CaseBlock);
}

static void getVisitor(
    KOREDefinition *definition, llvm::Module *module, KORESymbol *symbol,
    llvm::BasicBlock *CaseBlock, std::vector<llvm::Value *> const &callbacks) {
  llvm::LLVMContext &Ctx = module->getContext();
  llvm::Constant *zero = llvm::ConstantInt::get(llvm::Type::getInt64Ty(Ctx), 0);
  auto indices = std::vector<llvm::Constant *>{zero, zero};
  llvm::Function *func = CaseBlock->getParent();
  int idx = 0;
  auto BlockType = getBlockType(module, definition, symbol);
  auto cast = new llvm::BitCastInst(
      func->arg_begin(), llvm::PointerType::getUnqual(BlockType), "",
      CaseBlock);
  unsigned i = 0;
  auto file = makeWriterType(Ctx);

  auto state_ptr = func->arg_end() - 1;

  for (auto sort : symbol->getArguments()) {
    auto compositeSort = dynamic_cast<KORECompositeSort *>(sort.get());
    ValueType cat = compositeSort->getCategory(definition);
    llvm::Value *ChildPtr = llvm::GetElementPtrInst::CreateInBounds(
        BlockType, cast,
        {zero, llvm::ConstantInt::get(llvm::Type::getInt32Ty(Ctx), idx++ + 2)},
        "", CaseBlock);
    llvm::Value *Child = new llvm::LoadInst(
        getValueType(cat, module), ChildPtr, "", CaseBlock);
    auto sort_name = ast_to_string(*sort);
    auto Str = llvm::ConstantDataArray::getString(Ctx, sort_name, true);
    auto global = module->getOrInsertGlobal(
        fmt::format("sort_name_{}", sort_name), Str->getType());
    auto *globalVar = llvm::dyn_cast<llvm::GlobalVariable>(global);
    if (!globalVar->hasInitializer()) {
      globalVar->setInitializer(Str);
    }
    llvm::Constant *CharPtr = llvm::ConstantExpr::getInBoundsGetElementPtr(
        Str->getType(), global, indices);
    switch (cat.cat) {
    case SortCategory::Variable:
    case SortCategory::Symbol:
      llvm::CallInst::Create(
          llvm::FunctionType::get(
              llvm::Type::getVoidTy(Ctx),
              {file, Child->getType(), llvm::Type::getInt8PtrTy(Ctx),
               llvm::Type::getInt1Ty(Ctx), llvm::Type::getInt8PtrTy(Ctx)},
              false),
          callbacks.at(0),
          {func->arg_begin() + 1, Child, CharPtr,
           llvm::ConstantInt::get(
               llvm::Type::getInt1Ty(Ctx), cat.cat == SortCategory::Variable),
           state_ptr},
          "", CaseBlock);
      break;
    case SortCategory::Int:
      llvm::CallInst::Create(
          llvm::FunctionType::get(
              llvm::Type::getVoidTy(Ctx),
              {file, Child->getType(), llvm::Type::getInt8PtrTy(Ctx),
               llvm::Type::getInt8PtrTy(Ctx)},
              false),
          callbacks.at(4), {func->arg_begin() + 1, Child, CharPtr, state_ptr},
          "", CaseBlock);
      break;
    case SortCategory::Float:
      llvm::CallInst::Create(
          llvm::FunctionType::get(
              llvm::Type::getVoidTy(Ctx),
              {file, Child->getType(), llvm::Type::getInt8PtrTy(Ctx),
               llvm::Type::getInt8PtrTy(Ctx)},
              false),
          callbacks.at(5), {func->arg_begin() + 1, Child, CharPtr, state_ptr},
          "", CaseBlock);
      break;
    case SortCategory::Bool:
      llvm::CallInst::Create(
          llvm::FunctionType::get(
              llvm::Type::getVoidTy(Ctx),
              {file, Child->getType(), llvm::Type::getInt8PtrTy(Ctx),
               llvm::Type::getInt8PtrTy(Ctx)},
              false),
          callbacks.at(6), {func->arg_begin() + 1, Child, CharPtr, state_ptr},
          "", CaseBlock);
      break;
    case SortCategory::StringBuffer:
      llvm::CallInst::Create(
          llvm::FunctionType::get(
              llvm::Type::getVoidTy(Ctx),
              {file, Child->getType(), llvm::Type::getInt8PtrTy(Ctx),
               llvm::Type::getInt8PtrTy(Ctx)},
              false),
          callbacks.at(7), {func->arg_begin() + 1, Child, CharPtr, state_ptr},
          "", CaseBlock);
      break;
    case SortCategory::MInt: {
      llvm::Value *mint = new llvm::LoadInst(
          getArgType(cat, module), ChildPtr, "mint", CaseBlock);
      size_t nwords = (cat.bits + 63) / 64;
      auto nbits
          = llvm::ConstantInt::get(llvm::Type::getInt64Ty(Ctx), cat.bits);
      auto fnType = llvm::FunctionType::get(
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
            "", CaseBlock);
      } else {
        auto Ptr = allocateTerm(
            llvm::Type::getInt64Ty(Ctx),
            llvm::ConstantInt::get(llvm::Type::getInt64Ty(Ctx), nwords * 8),
            CaseBlock, "koreAllocAlwaysGC");
        if (nwords == 1) {
          llvm::Value *Word;
          if (cat.bits == 64) {
            Word = mint;
          } else {
            Word = new llvm::ZExtInst(
                mint, llvm::Type::getInt64Ty(Ctx), "word", CaseBlock);
          }
          new llvm::StoreInst(Word, Ptr, CaseBlock);
        } else { // nwords >= 2
          llvm::Value *Ptr2 = Ptr;
          llvm::Value *accum = mint;
          for (size_t i = 0; i < nwords; i++) {
            auto Word = new llvm::TruncInst(
                accum, llvm::Type::getInt64Ty(Ctx), "word", CaseBlock);
            new llvm::StoreInst(Word, Ptr2, CaseBlock);
            Ptr2 = llvm::GetElementPtrInst::Create(
                llvm::Type::getInt64Ty(Ctx), Ptr2,
                {llvm::ConstantInt::get(llvm::Type::getInt64Ty(Ctx), 1)}, "ptr",
                CaseBlock);
            accum = llvm::BinaryOperator::Create(
                llvm::Instruction::LShr, accum,
                llvm::ConstantInt::get(mint->getType(), 64), "shift",
                CaseBlock);
          }
        }
        llvm::CallInst::Create(
            fnType, callbacks.at(8),
            {func->arg_begin() + 1, Ptr, nbits, CharPtr, state_ptr}, "",
            CaseBlock);
      }
      break;
    }
    case SortCategory::Map:
      visitCollection(
          definition, module, compositeSort, func, ChildPtr, CaseBlock,
          callbacks.at(1), state_ptr);
      break;
    case SortCategory::RangeMap:
      visitCollection(
          definition, module, compositeSort, func, ChildPtr, CaseBlock,
          callbacks.at(10), state_ptr);
      break;
    case SortCategory::Set:
      visitCollection(
          definition, module, compositeSort, func, ChildPtr, CaseBlock,
          callbacks.at(3), state_ptr);
      break;
    case SortCategory::List: {
      visitCollection(
          definition, module, compositeSort, func, ChildPtr, CaseBlock,
          callbacks.at(2), state_ptr);
      break;
    }
    case SortCategory::Uncomputed: abort();
    }
    if (i != symbol->getArguments().size() - 1) {
      llvm::CallInst::Create(
          llvm::FunctionType::get(
              llvm::Type::getVoidTy(Ctx), {file, llvm::Type::getInt8PtrTy(Ctx)},
              false),
          callbacks.at(9), {func->arg_begin() + 1, state_ptr}, "", CaseBlock);
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
  auto offset_ty = llvm::Type::getInt64Ty(mod->getContext());
  return llvm::ConstantInt::get(offset_ty, offset);
#else
  return llvm::ConstantExpr::getOffsetOf(struct_ty, nth_member);
#endif
}

static llvm::Constant *getLayoutData(
    uint16_t layout, KORESymbol *symbol, llvm::Module *module,
    KOREDefinition *def) {
  uint8_t len = symbol->getArguments().size();
  std::vector<llvm::Constant *> elements;
  llvm::LLVMContext &Ctx = module->getContext();
  auto BlockType = getBlockType(module, def, symbol);
  int i = 2;
  for (auto sort : symbol->getArguments()) {
    ValueType cat
        = dynamic_cast<KORECompositeSort *>(sort.get())->getCategory(def);
    auto offset = getOffsetOfMember(
        module, BlockType,
        i++); //llvm::ConstantExpr::getOffsetOf(BlockType, i++);
    elements.push_back(llvm::ConstantStruct::get(
        llvm::StructType::getTypeByName(
            module->getContext(), LAYOUTITEM_STRUCT),
        offset,
        llvm::ConstantInt::get(
            llvm::Type::getInt16Ty(Ctx), (int)cat.cat + cat.bits)));
  }
  auto Arr = llvm::ConstantArray::get(
      llvm::ArrayType::get(
          llvm::StructType::getTypeByName(
              module->getContext(), LAYOUTITEM_STRUCT),
          len),
      elements);
  auto global = module->getOrInsertGlobal(
      "layout_item_" + std::to_string(layout), Arr->getType());
  auto *globalVar = llvm::cast<llvm::GlobalVariable>(global);
  if (!globalVar->hasInitializer()) {
    globalVar->setInitializer(Arr);
  }
  llvm::Constant *zero = llvm::ConstantInt::get(llvm::Type::getInt64Ty(Ctx), 0);
  auto indices = std::vector<llvm::Constant *>{zero, zero};
  auto Ptr = llvm::ConstantExpr::getInBoundsGetElementPtr(
      Arr->getType(), globalVar, indices);
  std::string name = "layout_" + std::to_string(layout);
  auto global2 = module->getOrInsertGlobal(
      name,
      llvm::StructType::getTypeByName(module->getContext(), LAYOUT_STRUCT));
  auto *globalVar2 = llvm::cast<llvm::GlobalVariable>(global2);
  initDebugGlobal(name, getForwardDecl(LAYOUT_STRUCT), globalVar2);
  if (!globalVar2->hasInitializer()) {
    globalVar2->setInitializer(llvm::ConstantStruct::get(
        llvm::StructType::getTypeByName(module->getContext(), LAYOUT_STRUCT),
        llvm::ConstantInt::get(llvm::Type::getInt8Ty(Ctx), len), Ptr));
  }
  return globalVar2;
}

static void emitLayouts(KOREDefinition *definition, llvm::Module *module) {
  std::map<uint16_t, KORESymbol *> layouts;
  for (auto entry : definition->getSymbols()) {
    layouts[entry.second->getLayout()] = entry.second;
  }
  llvm::LLVMContext &Ctx = module->getContext();
  std::vector<llvm::Type *> argTypes;
  argTypes.push_back(llvm::Type::getInt16Ty(Ctx));
  auto func = llvm::cast<llvm::Function>(getOrInsertFunction(
      module, "getLayoutData",
      llvm::FunctionType::get(
          llvm::PointerType::getUnqual(llvm::StructType::getTypeByName(
              module->getContext(), LAYOUT_STRUCT)),
          argTypes, false)));
  initDebugFunction(
      "getLayoutData", "getLayoutData",
      getDebugFunctionType(
          getPointerDebugType(getForwardDecl(LAYOUT_STRUCT), "layout *"),
          {getShortDebugType()}),
      definition, func);
  auto EntryBlock = llvm::BasicBlock::Create(Ctx, "entry", func);
  auto MergeBlock = llvm::BasicBlock::Create(Ctx, "exit");
  auto stuck = llvm::BasicBlock::Create(Ctx, "stuck");
  auto Switch = llvm::SwitchInst::Create(
      func->arg_begin(), stuck, layouts.size(), EntryBlock);
  auto Phi = llvm::PHINode::Create(
      llvm::PointerType::getUnqual(
          llvm::StructType::getTypeByName(module->getContext(), LAYOUT_STRUCT)),
      layouts.size(), "phi", MergeBlock);
  for (auto entry : layouts) {
    uint16_t layout = entry.first;
    auto symbol = entry.second;
    auto CaseBlock = llvm::BasicBlock::Create(
        Ctx, "layout" + std::to_string(layout), func);
    llvm::BranchInst::Create(MergeBlock, CaseBlock);
    auto data = getLayoutData(layout, symbol, module, definition);
    Phi->addIncoming(data, CaseBlock);
    Switch->addCase(
        llvm::ConstantInt::get(llvm::Type::getInt16Ty(Ctx), layout), CaseBlock);
  }
  llvm::ReturnInst::Create(Ctx, Phi, MergeBlock);
  MergeBlock->insertInto(func);
  addAbort(stuck, module);
  stuck->insertInto(func);
}

static void emitVisitChildren(KOREDefinition *def, llvm::Module *mod) {
  emitTraversal("visitChildren", def, mod, true, getVisitor);
}

static void emitInjTags(KOREDefinition *def, llvm::Module *mod) {
  llvm::LLVMContext &Ctx = mod->getContext();
  auto global
      = mod->getOrInsertGlobal("first_inj_tag", llvm::Type::getInt32Ty(Ctx));
  auto *globalVar = llvm::cast<llvm::GlobalVariable>(global);
  globalVar->setConstant(true);
  if (!globalVar->hasInitializer()) {
    globalVar->setInitializer(llvm::ConstantInt::get(
        llvm::Type::getInt32Ty(Ctx), def->getInjSymbol()->getFirstTag()));
  }
  global = mod->getOrInsertGlobal("last_inj_tag", llvm::Type::getInt32Ty(Ctx));
  globalVar = llvm::cast<llvm::GlobalVariable>(global);
  globalVar->setConstant(true);
  if (!globalVar->hasInitializer()) {
    globalVar->setInitializer(llvm::ConstantInt::get(
        llvm::Type::getInt32Ty(Ctx), def->getInjSymbol()->getLastTag()));
  }
}

static void emitSortTable(KOREDefinition *def, llvm::Module *mod) {
  auto getter = [](KOREDefinition *definition, llvm::Module *module,
                   KORESymbol *symbol) -> llvm::Constant * {
    auto &ctx = module->getContext();

    auto subtableType = llvm::ArrayType::get(
        llvm::Type::getInt8PtrTy(ctx), symbol->getArguments().size());
    auto subtable = module->getOrInsertGlobal(
        fmt::format("sorts_{}", ast_to_string(*symbol)), subtableType);
    auto *subtableVar = llvm::dyn_cast<llvm::GlobalVariable>(subtable);
    initDebugGlobal(
        "sorts_" + symbol->getName(),
        getArrayDebugType(
            getCharPtrDebugType(), symbol->getArguments().size(),
            llvm::DataLayout(module).getABITypeAlign(
                llvm::Type::getInt8PtrTy(ctx))),
        subtableVar);
    llvm::Constant *zero
        = llvm::ConstantInt::get(llvm::Type::getInt64Ty(ctx), 0);
    auto indices = std::vector<llvm::Constant *>{zero, zero};

    std::vector<llvm::Constant *> subvalues;
    for (const auto & i : symbol->getArguments()) {
      auto arg_str = ast_to_string(*i);
      auto strType = llvm::ArrayType::get(
          llvm::Type::getInt8Ty(ctx), arg_str.size() + 1);
      auto sortName = module->getOrInsertGlobal(
          fmt::format("sort_name_{}", arg_str), strType);
      subvalues.push_back(llvm::ConstantExpr::getInBoundsGetElementPtr(
          strType, sortName, indices));
    }
    subtableVar->setInitializer(
        llvm::ConstantArray::get(subtableType, subvalues));

    return llvm::ConstantExpr::getInBoundsGetElementPtr(
        subtableType, subtableVar, indices);
  };

  auto i8_ptr_ty = llvm::Type::getInt8PtrTy(mod->getContext());
  auto entry_ty = llvm::PointerType::getUnqual(i8_ptr_ty);
  auto debug_ty = getPointerDebugType(getCharPtrDebugType(), "char **");

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
static void emitReturnSortTable(KOREDefinition *def, llvm::Module *mod) {
  auto getter = [](KOREDefinition *definition, llvm::Module *module,
                   KORESymbol *symbol) -> llvm::Constant * {
    auto &ctx = module->getContext();

    auto sort = symbol->getSort();
    auto sort_str = ast_to_string(*sort);

    auto char_type = llvm::Type::getInt8Ty(ctx);
    auto str_type = llvm::ArrayType::get(char_type, sort_str.size() + 1);

    auto sort_name
        = module->getOrInsertGlobal("sort_name_" + sort_str, str_type);

    auto i64_type = llvm::Type::getInt64Ty(ctx);
    auto zero = llvm::ConstantInt::get(i64_type, 0);

    return llvm::ConstantExpr::getInBoundsGetElementPtr(
        str_type, sort_name, std::vector<llvm::Constant *>{zero});
  };

  emitDataTableForSymbol(
      "getReturnSortForTag", llvm::Type::getInt8PtrTy(mod->getContext()),
      getCharPtrDebugType(), def, mod, getter);
}

void emitConfigParserFunctions(
    KOREDefinition *definition, llvm::Module *module) {
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
}

} // namespace kllvm
