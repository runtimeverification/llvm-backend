#include "kllvm/codegen/EmitConfigParser.h"
#include "kllvm/codegen/CreateTerm.h"
#include "kllvm/codegen/GenAlloc.h"
#include "kllvm/codegen/Debug.h"

#include "llvm/IR/Constants.h"
#include "llvm/IR/Instructions.h"

#include "runtime/header.h"

namespace kllvm {

static llvm::Constant *getSymbolNamePtr(KORESymbol *symbol, llvm::BasicBlock *SetBlockName, llvm::Module *module) {
  llvm::LLVMContext &Ctx = module->getContext();
  std::ostringstream Out;
  symbol->print(Out);
  if (SetBlockName) {
    SetBlockName->setName(Out.str());
  }
  auto Str = llvm::ConstantDataArray::getString(Ctx, Out.str(), true);
  auto global = module->getOrInsertGlobal("sym_name_" + Out.str(), Str->getType());
  llvm::GlobalVariable *globalVar = llvm::dyn_cast<llvm::GlobalVariable>(global);
  if (!globalVar->hasInitializer()) {
    globalVar->setInitializer(Str);
  }
  llvm::Constant *zero = llvm::ConstantInt::get(llvm::Type::getInt64Ty(Ctx), 0);
  auto indices = std::vector<llvm::Constant *>{zero, zero};
  auto Ptr = llvm::ConstantExpr::getInBoundsGetElementPtr(Str->getType(), globalVar, indices);
  return Ptr;
}

static void emitGetTagForSymbolName(KOREDefinition *definition, llvm::Module *module) {
  llvm::LLVMContext &Ctx = module->getContext();
  auto func = llvm::dyn_cast<llvm::Function>(module->getOrInsertFunction(
      "getTagForSymbolName", llvm::Type::getInt32Ty(Ctx), llvm::Type::getInt8PtrTy(Ctx)));
  auto CurrentBlock = llvm::BasicBlock::Create(Ctx, "");
  auto MergeBlock = llvm::BasicBlock::Create(Ctx, "exit");
  auto Phi = llvm::PHINode::Create(llvm::Type::getInt32Ty(Ctx), definition->getSymbols().size(), "phi", MergeBlock);
  auto &syms = definition->getAllSymbols();
  llvm::Constant *Strcmp = module->getOrInsertFunction("strcmp", 
      llvm::Type::getInt32Ty(Ctx), llvm::Type::getInt8PtrTy(Ctx),
      llvm::Type::getInt8PtrTy(Ctx));
  for (auto iter = syms.begin(); iter != syms.end(); ++iter) {
    auto &entry = *iter;
    uint32_t tag = entry.second->getTag();
    auto symbol = entry.second;
    CurrentBlock->insertInto(func);
    auto Ptr = getSymbolNamePtr(symbol, CurrentBlock, module);
    auto compare = llvm::CallInst::Create(Strcmp, {func->arg_begin(), Ptr}, "", CurrentBlock);
    auto icmp = new llvm::ICmpInst(*CurrentBlock, llvm::CmpInst::ICMP_EQ, 
       compare, llvm::ConstantInt::get(llvm::Type::getInt32Ty(Ctx), 0));
    auto FalseBlock = llvm::BasicBlock::Create(Ctx, "");
    llvm::BranchInst::Create(MergeBlock, FalseBlock, icmp, CurrentBlock);
    Phi->addIncoming(llvm::ConstantInt::get(llvm::Type::getInt32Ty(Ctx), tag), CurrentBlock);
    CurrentBlock = FalseBlock;
  }
  llvm::ReturnInst::Create(Ctx, Phi, MergeBlock);
  MergeBlock->insertInto(func);
  addAbort(CurrentBlock, module);
  CurrentBlock->setName("stuck");
  CurrentBlock->insertInto(func);
}

static std::string BLOCKHEADER_STRUCT = "blockheader";
static std::string INT_STRUCT = "mpz";
static std::string FLOAT_STRUCT = "floating";
static std::string STRING_STRUCT = "string";
static std::string BUFFER_STRUCT = "stringbuffer";
static std::string LAYOUT_STRUCT = "layout";
static std::string LAYOUTITEM_STRUCT = "layoutitem";

static void emitDataTableForSymbol(std::string name, llvm::Type *ty, llvm::DIType *dity, KOREDefinition *definition, llvm::Module *module,
    llvm::Constant * getter(KOREDefinition *, llvm::Module *,
        KORESymbol *)) {
  llvm::LLVMContext &Ctx = module->getContext();
  std::vector<llvm::Type *> argTypes;
  argTypes.push_back(llvm::Type::getInt32Ty(Ctx));
  auto func = llvm::dyn_cast<llvm::Function>(module->getOrInsertFunction(
      name, llvm::FunctionType::get(ty, argTypes, false)));
  initDebugFunction(name, name, getDebugFunctionType(dity, {getIntDebugType()}), definition, func);
  auto EntryBlock = llvm::BasicBlock::Create(Ctx, "entry", func);
  auto MergeBlock = llvm::BasicBlock::Create(Ctx, "exit");
  auto stuck = llvm::BasicBlock::Create(Ctx, "stuck");
  auto &syms = definition->getSymbols();
  auto icmp = new llvm::ICmpInst(*EntryBlock, llvm::CmpInst::ICMP_ULE, func->arg_begin(), llvm::ConstantInt::get(llvm::Type::getInt32Ty(Ctx), syms.rbegin()->first));
  llvm::BranchInst::Create(MergeBlock, stuck, icmp, EntryBlock);
  auto tableType = llvm::ArrayType::get(ty, syms.size());
  auto table = module->getOrInsertGlobal("table_" + name, tableType);
  llvm::GlobalVariable *globalVar = llvm::dyn_cast<llvm::GlobalVariable>(table);
  initDebugGlobal("table_" + name, getArrayDebugType(dity, syms.size(), llvm::DataLayout(module).getABITypeAlignment(ty)), globalVar);
  std::vector<llvm::Constant *> values;
  for (auto iter = syms.begin(); iter != syms.end(); ++iter) {
    auto entry = *iter;
    auto symbol = entry.second;
    auto val = getter(definition, module, symbol);
    values.push_back(val);
  }
  if (!globalVar->hasInitializer()) {
    globalVar->setInitializer(llvm::ConstantArray::get(tableType, values));
  }
  auto offset = new llvm::ZExtInst(func->arg_begin(), llvm::Type::getInt64Ty(Ctx), "", MergeBlock);
  llvm::Constant *zero = llvm::ConstantInt::get(llvm::Type::getInt64Ty(Ctx), 0);
  auto retval = llvm::GetElementPtrInst::Create(
    tableType, globalVar, {zero, offset}, "", MergeBlock);
  auto load = new llvm::LoadInst(retval, "", MergeBlock);
  llvm::ReturnInst::Create(Ctx, load, MergeBlock);
  MergeBlock->insertInto(func);
  addAbort(stuck, module);
  stuck->insertInto(func);
}


static void emitDataForSymbol(std::string name, llvm::Type *ty, llvm::DIType *dity, KOREDefinition *definition, llvm::Module *module, bool isEval,
    std::pair<llvm::Value *, llvm::BasicBlock *> getter(KOREDefinition *, llvm::Module *,
        KORESymbol *, llvm::Instruction *)) {
  llvm::LLVMContext &Ctx = module->getContext();
  std::vector<llvm::Type *> argTypes;
  argTypes.push_back(llvm::Type::getInt32Ty(Ctx));
  if (isEval) {
    auto ty = llvm::PointerType::getUnqual(llvm::ArrayType::get(llvm::Type::getInt8PtrTy(Ctx), 0));
    argTypes.push_back(ty);
  }
  auto func = llvm::dyn_cast<llvm::Function>(module->getOrInsertFunction(
      name, llvm::FunctionType::get(ty, argTypes, false)));
  if (!isEval) {
    initDebugFunction(name, name, getDebugFunctionType(dity, {getIntDebugType()}), definition, func);
  }
  auto EntryBlock = llvm::BasicBlock::Create(Ctx, "entry", func);
  auto MergeBlock = llvm::BasicBlock::Create(Ctx, "exit");
  auto stuck = llvm::BasicBlock::Create(Ctx, "stuck");
  auto &syms = definition->getSymbols();
  auto Switch = llvm::SwitchInst::Create(func->arg_begin(), stuck, syms.size(), EntryBlock);
  auto Phi = llvm::PHINode::Create(ty, definition->getSymbols().size(), "phi", MergeBlock);
  for (auto iter = syms.begin(); iter != syms.end(); ++iter) {
    auto entry = *iter;
    uint32_t tag = entry.first;
    auto symbol = entry.second;
    auto decl = definition->getSymbolDeclarations().at(symbol->getName());
    bool isFunc = decl->getAttributes().count("function") || decl->getAttributes().count("anywhere");
    if (isEval && !isFunc) {
      continue;
    }
    auto CaseBlock = llvm::BasicBlock::Create(Ctx, "tag" + std::to_string(tag), func);
    auto Branch = llvm::BranchInst::Create(MergeBlock, CaseBlock);
    auto pair = getter(definition, module, symbol, Branch);
    Phi->addIncoming(pair.first, pair.second);
    Switch->addCase(llvm::ConstantInt::get(llvm::Type::getInt32Ty(Ctx), tag), CaseBlock);
  }
  llvm::ReturnInst::Create(Ctx, Phi, MergeBlock);
  MergeBlock->insertInto(func);
  addAbort(stuck, module);
  stuck->insertInto(func);
}

static std::pair<llvm::Value *, llvm::BasicBlock *> getHeader(KOREDefinition *definition, llvm::Module *module,
    KORESymbol *symbol, llvm::Instruction *inst) {
  auto BlockType = getBlockType(module, definition, symbol);
  return std::make_pair(getBlockHeader(module, definition, symbol, BlockType), inst->getParent());
}

static void emitGetBlockHeaderForSymbol(KOREDefinition *def, llvm::Module *mod) {
  emitDataForSymbol("getBlockHeaderForSymbol", mod->getTypeByName(BLOCKHEADER_STRUCT), getForwardDecl(BLOCKHEADER_STRUCT),
      def, mod, false, getHeader);
}

static std::pair<llvm::Value *, llvm::BasicBlock *> getFunction(KOREDefinition *def, llvm::Module *mod,
    KORESymbol *symbol, llvm::Instruction *inst) {
  auto decl = def->getSymbolDeclarations().at(symbol->getName());
  bool res = decl->getAttributes().count("function") || decl->getAttributes().count("anywhere");
  return std::make_pair(llvm::ConstantInt::get(llvm::Type::getInt1Ty(mod->getContext()), res),
      inst->getParent());
}

static void emitIsSymbolAFunction(KOREDefinition *def, llvm::Module *mod) {
  emitDataForSymbol("isSymbolAFunction", llvm::Type::getInt1Ty(mod->getContext()), getBoolDebugType(),
      def, mod, false, getFunction);
}

static llvm::Constant * getBinder(KOREDefinition *def, llvm::Module *mod,
    KORESymbol *symbol) {
  auto decl = def->getSymbolDeclarations().at(symbol->getName());
  bool res = decl->getAttributes().count("binder");
  return llvm::ConstantInt::get(llvm::Type::getInt1Ty(mod->getContext()), res);
}

static void emitIsSymbolABinder(KOREDefinition *def, llvm::Module *mod) {
  emitDataTableForSymbol("isSymbolABinder", llvm::Type::getInt1Ty(mod->getContext()), getBoolDebugType(),
      def, mod, getBinder);
}


static std::pair<llvm::Value *, llvm::BasicBlock *> getInjection(KOREDefinition *def, llvm::Module *mod,
    KORESymbol *symbol, llvm::Instruction *inst) {
  llvm::Constant *tag = llvm::ConstantInt::get(llvm::Type::getInt32Ty(mod->getContext()), 0);
  for (auto sym : def->getSymbols()) {
    if (sym.second->getName() == "inj" && *sym.second->getArguments()[0] == *symbol->getSort()) {
      tag = llvm::ConstantInt::get(llvm::Type::getInt32Ty(mod->getContext()), sym.first);
      break;
    }
  }
  return std::make_pair(tag, inst->getParent());
}

static void emitGetInjectionForSortOfTag(KOREDefinition *def, llvm::Module *mod) {
  emitDataForSymbol("getInjectionForSortOfTag", llvm::Type::getInt32Ty(mod->getContext()), getIntDebugType(),
      def, mod, false, getInjection);
}

static llvm::Value *getArgValue(llvm::Value *ArgumentsArray, int idx,
    llvm::BasicBlock *CaseBlock, ValueType cat, llvm::Module *mod) {
  llvm::LLVMContext &Ctx = mod->getContext();
  llvm::Constant *zero = llvm::ConstantInt::get(llvm::Type::getInt64Ty(Ctx), 0);
  auto addr = llvm::GetElementPtrInst::Create(
      llvm::ArrayType::get(llvm::Type::getInt8PtrTy(Ctx), 0),
      ArgumentsArray, {zero, llvm::ConstantInt::get(llvm::Type::getInt64Ty(Ctx), idx)},
      "", CaseBlock);
  llvm::Value *arg = new llvm::LoadInst(addr, "", CaseBlock);
  switch(cat.cat) {
  case SortCategory::Bool:
  case SortCategory::MInt: {
    auto cast = new llvm::BitCastInst(arg,
        llvm::PointerType::getUnqual(getValueType(cat, mod)), "", CaseBlock);
    auto load = new llvm::LoadInst(cast, "", CaseBlock);
    arg = load;
    break;
  }
  case SortCategory::Map:
  case SortCategory::List:
  case SortCategory::Set:
    arg = new llvm::BitCastInst(arg, llvm::PointerType::getUnqual(getValueType(cat, mod)), "", CaseBlock);
    break;
  case SortCategory::Int:
  case SortCategory::Float:
  case SortCategory::StringBuffer:
  case SortCategory::Symbol:
  case SortCategory::Variable:
    arg = new llvm::BitCastInst(arg, getValueType(cat, mod), "", CaseBlock);
    break;
  case SortCategory::Uncomputed:
    abort();
  }
  return arg;
}

static std::pair<llvm::Value *, llvm::BasicBlock *> getEval(KOREDefinition *def, llvm::Module *mod,
    KORESymbol *symbol, llvm::Instruction *inst) {
  llvm::LLVMContext &Ctx = mod->getContext();
  llvm::BasicBlock *CaseBlock = inst->getParent();
  inst->removeFromParent();
  llvm::Function *func = CaseBlock->getParent();
  llvm::Value *ArgumentsArray = func->arg_begin() + 1;
  int idx = 0;
  llvm::StringMap<llvm::Value *> subst;
  auto pattern = KORECompositePattern::Create(symbol);
  for (auto &sort : symbol->getArguments()) {
    ValueType cat = dynamic_cast<KORECompositeSort *>(sort.get())->getCategory(def);
    llvm::Value *arg = getArgValue(ArgumentsArray, idx, CaseBlock, cat, mod);
    std::string name = "_" + std::to_string(idx++);
    subst.insert({name, arg});
    pattern->addArgument(KOREVariablePattern::Create(name, sort));
  }
  KORESymbolDeclaration *symbolDecl = def->getSymbolDeclarations().at(symbol->getName());
  CreateTerm creator(subst, def, CaseBlock, mod, false);
  llvm::Value *result = creator(pattern.get()).first;
  llvm::Value *retval;
  ValueType cat = dynamic_cast<KORECompositeSort *>(symbol->getSort().get())->getCategory(def);
  switch(cat.cat) {
  case SortCategory::Int:
  case SortCategory::Float:
  case SortCategory::StringBuffer:
  case SortCategory::Symbol:
  case SortCategory::Variable:
    retval = new llvm::BitCastInst(result, llvm::Type::getInt8PtrTy(Ctx), "",
        creator.getCurrentBlock());
    break;
  case SortCategory::Map:
  case SortCategory::List:
  case SortCategory::Set:
    if (symbolDecl->getAttributes().count("hook")) {
      // if this is a hook then we are using the sret abi and the reutrned value
      // is a pointer. Otherwise we need to store it like Bool/MInt
      retval = new llvm::BitCastInst(result, llvm::Type::getInt8PtrTy(Ctx), "",
          creator.getCurrentBlock());
      break;
    }
    // fall through
  case SortCategory::Bool:
  case SortCategory::MInt: {
    llvm::Instruction *Malloc = llvm::CallInst::CreateMalloc(
        creator.getCurrentBlock(), llvm::Type::getInt64Ty(Ctx), result->getType(), 
        llvm::ConstantExpr::getSizeOf(result->getType()), nullptr, nullptr);
    creator.getCurrentBlock()->getInstList().push_back(Malloc);
    new llvm::StoreInst(result, Malloc, creator.getCurrentBlock());
    retval = new llvm::BitCastInst(Malloc, llvm::Type::getInt8PtrTy(Ctx), "",
        creator.getCurrentBlock());
    break;
  }
  case SortCategory::Uncomputed:
    abort();
  }
  creator.getCurrentBlock()->getInstList().push_back(inst);
  return std::make_pair(retval, creator.getCurrentBlock());
}

static void emitEvaluateFunctionSymbol(KOREDefinition *def, llvm::Module *mod) {
  emitDataForSymbol("evaluateFunctionSymbol", llvm::Type::getInt8PtrTy(mod->getContext()), nullptr,
      def, mod, true, getEval);
}

static void emitGetTagForFreshSort(KOREDefinition *definition, llvm::Module *module) {
  llvm::LLVMContext &Ctx = module->getContext();
  auto func = llvm::dyn_cast<llvm::Function>(module->getOrInsertFunction(
      "getTagForFreshSort", llvm::Type::getInt32Ty(Ctx), llvm::Type::getInt8PtrTy(Ctx)));
  auto CurrentBlock = llvm::BasicBlock::Create(Ctx, "");
  auto MergeBlock = llvm::BasicBlock::Create(Ctx, "exit");
  auto Phi = llvm::PHINode::Create(llvm::Type::getInt32Ty(Ctx), definition->getSortDeclarations().size(), "phi", MergeBlock);
  auto &sorts = definition->getSortDeclarations();
  llvm::Constant *Strcmp = module->getOrInsertFunction("strcmp", 
      llvm::Type::getInt32Ty(Ctx), llvm::Type::getInt8PtrTy(Ctx),
      llvm::Type::getInt8PtrTy(Ctx));
  llvm::Constant *zero = llvm::ConstantInt::get(llvm::Type::getInt64Ty(Ctx), 0);
  llvm::Constant *zero32 = llvm::ConstantInt::get(llvm::Type::getInt32Ty(Ctx), 0);
  bool hasCase = false;
  for (auto iter = sorts.begin(); iter != sorts.end(); ++iter) {
    auto &entry = *iter;
    std::string name = entry.first;
    if (!definition->getFreshFunctions().count(name)) {
      continue;
    }
    hasCase = true;
    CurrentBlock->insertInto(func);
    CurrentBlock->setName("is_" + name);
    auto Str = llvm::ConstantDataArray::getString(Ctx, name, true);
    auto global = module->getOrInsertGlobal("sort_name_" + name, Str->getType());
    llvm::GlobalVariable *globalVar = llvm::dyn_cast<llvm::GlobalVariable>(global);
    if (!globalVar->hasInitializer()) {
      globalVar->setInitializer(Str);
    }
    auto indices = std::vector<llvm::Constant *>{zero, zero};
    auto Ptr = llvm::ConstantExpr::getInBoundsGetElementPtr(Str->getType(), globalVar, indices);
    auto compare = llvm::CallInst::Create(Strcmp, {func->arg_begin(), Ptr}, "", CurrentBlock);
    auto icmp = new llvm::ICmpInst(*CurrentBlock, llvm::CmpInst::ICMP_EQ, 
       compare, zero32);
    auto FalseBlock = llvm::BasicBlock::Create(Ctx, "");
    auto CaseBlock = llvm::BasicBlock::Create(Ctx, name, func);
    llvm::BranchInst::Create(CaseBlock, FalseBlock, icmp, CurrentBlock);
    auto symbol = definition->getFreshFunctions().at(name);
    std::ostringstream Out;
    symbol->print(Out);
    Phi->addIncoming(llvm::ConstantInt::get(llvm::Type::getInt32Ty(Ctx), definition->getAllSymbols().at(Out.str())->getTag()), CaseBlock);
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
  auto func = llvm::dyn_cast<llvm::Function>(module->getOrInsertFunction(
      "getToken", llvm::Type::getInt8PtrTy(Ctx), llvm::Type::getInt8PtrTy(Ctx),
      llvm::Type::getInt64Ty(Ctx), llvm::Type::getInt8PtrTy(Ctx)));
  auto CurrentBlock = llvm::BasicBlock::Create(Ctx, "");
  auto MergeBlock = llvm::BasicBlock::Create(Ctx, "exit");
  auto Phi = llvm::PHINode::Create(llvm::Type::getInt8PtrTy(Ctx), definition->getSortDeclarations().size(), "phi", MergeBlock);
  auto &sorts = definition->getSortDeclarations();
  llvm::Constant *Strcmp = module->getOrInsertFunction("strcmp", 
      llvm::Type::getInt32Ty(Ctx), llvm::Type::getInt8PtrTy(Ctx),
      llvm::Type::getInt8PtrTy(Ctx));
  llvm::Constant *StringEqual = module->getOrInsertFunction("string_equal", 
      llvm::Type::getInt1Ty(Ctx), llvm::Type::getInt8PtrTy(Ctx),
      llvm::Type::getInt8PtrTy(Ctx), llvm::Type::getInt64Ty(Ctx),
      llvm::Type::getInt64Ty(Ctx));
  llvm::Constant *zero = llvm::ConstantInt::get(llvm::Type::getInt64Ty(Ctx), 0);
  llvm::Constant *zero32 = llvm::ConstantInt::get(llvm::Type::getInt32Ty(Ctx), 0);
  for (auto iter = sorts.begin(); iter != sorts.end(); ++iter) {
    auto &entry = *iter;
    std::string name = entry.first;
    auto sort = KORECompositeSort::Create(name);
    ValueType cat = sort->getCategory(definition);
    if (cat.cat == SortCategory::Symbol || cat.cat == SortCategory::Variable) {
      continue;
    }
    CurrentBlock->insertInto(func);
    CurrentBlock->setName("is_" + name);
    auto Str = llvm::ConstantDataArray::getString(Ctx, name, true);
    auto global = module->getOrInsertGlobal("sort_name_" + name, Str->getType());
    llvm::GlobalVariable *globalVar = llvm::dyn_cast<llvm::GlobalVariable>(global);
    if (!globalVar->hasInitializer()) {
      globalVar->setInitializer(Str);
    }
    auto indices = std::vector<llvm::Constant *>{zero, zero};
    auto Ptr = llvm::ConstantExpr::getInBoundsGetElementPtr(Str->getType(), globalVar, indices);
    auto compare = llvm::CallInst::Create(Strcmp, {func->arg_begin(), Ptr}, "", CurrentBlock);
    auto icmp = new llvm::ICmpInst(*CurrentBlock, llvm::CmpInst::ICMP_EQ, 
       compare, zero32);
    auto FalseBlock = llvm::BasicBlock::Create(Ctx, "");
    auto CaseBlock = llvm::BasicBlock::Create(Ctx, name, func);
    llvm::BranchInst::Create(CaseBlock, FalseBlock, icmp, CurrentBlock);
    switch(cat.cat) {
    case SortCategory::Map:
    case SortCategory::List:
    case SortCategory::Set:
      addAbort(CaseBlock, module);
      break;
    case SortCategory::StringBuffer:
    case SortCategory::MInt:
      //TODO: tokens
      addAbort(CaseBlock, module);
      break;
    case SortCategory::Bool: {
      auto Str = llvm::ConstantDataArray::getString(Ctx, "true", false);
      auto global = module->getOrInsertGlobal("bool_true", Str->getType());
      llvm::GlobalVariable *globalVar = llvm::dyn_cast<llvm::GlobalVariable>(global);
      if (!globalVar->hasInitializer()) {
        globalVar->setInitializer(Str);
      }
      auto Ptr = llvm::ConstantExpr::getInBoundsGetElementPtr(Str->getType(), globalVar, indices);
      auto Len = llvm::ConstantInt::get(llvm::Type::getInt64Ty(Ctx), 4);
      auto compare = llvm::CallInst::Create(StringEqual,
          {func->arg_begin()+2, Ptr, func->arg_begin()+1, Len}, "", CaseBlock);
      llvm::Instruction *Malloc = llvm::CallInst::CreateMalloc(
          CaseBlock, llvm::Type::getInt64Ty(Ctx), compare->getType(), 
          llvm::ConstantExpr::getSizeOf(compare->getType()), nullptr, nullptr);
      CaseBlock->getInstList().push_back(Malloc);
      new llvm::StoreInst(compare, Malloc, CaseBlock);
      auto result = new llvm::BitCastInst(Malloc, llvm::Type::getInt8PtrTy(Ctx), "", CaseBlock);
      Phi->addIncoming(result, CaseBlock);
      llvm::BranchInst::Create(MergeBlock, CaseBlock);
      break;
    }
    case SortCategory::Float: {
      llvm::Type *Float = module->getTypeByName(FLOAT_STRUCT);
      llvm::Value *Term = allocateTerm(Float, CaseBlock, "koreAllocFloating");
      llvm::Constant *InitFloat = module->getOrInsertFunction("init_float",
          llvm::Type::getVoidTy(Ctx), llvm::PointerType::getUnqual(Float), 
          llvm::Type::getInt8PtrTy(Ctx));
      llvm::CallInst::Create(InitFloat, {Term, func->arg_begin()+2}, "", CaseBlock);
      auto cast = new llvm::BitCastInst(Term,
          llvm::Type::getInt8PtrTy(Ctx), "", CaseBlock);
      Phi->addIncoming(cast, CaseBlock);
      llvm::BranchInst::Create(MergeBlock, CaseBlock);
      break;
    }
    case SortCategory::Int: {
      llvm::Value *FirstChar = new llvm::LoadInst(func->arg_begin()+2, "", CaseBlock);
      llvm::Constant *asciiPlus = llvm::ConstantInt::get(llvm::Type::getInt8Ty(Ctx), 43);
      auto icmpFirst = new llvm::ICmpInst(*CaseBlock, llvm::CmpInst::ICMP_EQ, FirstChar, asciiPlus);
      auto IfIsPlus = llvm::BasicBlock::Create(Ctx, "if_is_plus", func);
      auto ElseNoPlus = llvm::BasicBlock::Create(Ctx, "else_no_plus", func);
      llvm::BranchInst::Create(IfIsPlus, ElseNoPlus, icmpFirst, CaseBlock);
      llvm::Constant *one = llvm::ConstantInt::get(llvm::Type::getInt32Ty(Ctx), 1);
      llvm::Value *Pruned = llvm::GetElementPtrInst::CreateInBounds(llvm::Type::getInt8Ty(Ctx), func->arg_begin()+2,
        {one}, "", IfIsPlus);
      llvm::BranchInst::Create(ElseNoPlus, IfIsPlus);
      auto phiStr = llvm::PHINode::Create(llvm::Type::getInt8PtrTy(Ctx), 2, "", ElseNoPlus);
      phiStr->addIncoming(func->arg_begin()+2, CaseBlock);
      phiStr->addIncoming(Pruned, IfIsPlus);
      CaseBlock = ElseNoPlus;
      llvm::Type *Int = module->getTypeByName(INT_STRUCT);
      llvm::Value *Term = allocateTerm(Int, CaseBlock, "koreAllocInteger");
      llvm::Constant *MpzInitSet = module->getOrInsertFunction("__gmpz_init_set_str",
          llvm::Type::getInt32Ty(Ctx), llvm::PointerType::getUnqual(Int), 
          llvm::Type::getInt8PtrTy(Ctx), llvm::Type::getInt32Ty(Ctx));
      auto Call = llvm::CallInst::Create(MpzInitSet, {Term, phiStr,
          llvm::ConstantInt::get(llvm::Type::getInt32Ty(Ctx), 10)}, "", CaseBlock);
      auto icmp = new llvm::ICmpInst(*CaseBlock, llvm::CmpInst::ICMP_EQ, 
          Call, zero32);
      auto AbortBlock = llvm::BasicBlock::Create(Ctx, "invalid_int", func);
      addAbort(AbortBlock, module);
      auto cast = new llvm::BitCastInst(Term,
          llvm::Type::getInt8PtrTy(Ctx), "", CaseBlock);
      llvm::BranchInst::Create(MergeBlock, AbortBlock, icmp, CaseBlock);
      Phi->addIncoming(cast, CaseBlock);
      break;
    }
    case SortCategory::Variable:
    case SortCategory::Symbol:
      break;
    case SortCategory::Uncomputed:
      abort();
    }
    CurrentBlock = FalseBlock;
  }
  CurrentBlock->setName("symbol");
  CurrentBlock->insertInto(func);
  auto StringType = module->getTypeByName(STRING_STRUCT);
  auto Len = llvm::BinaryOperator::Create(llvm::Instruction::Add,
      func->arg_begin()+1, llvm::ConstantExpr::getSizeOf(StringType), "", CurrentBlock);
  llvm::Value *Block = allocateTerm(StringType, Len, CurrentBlock, "koreAllocToken");
  auto HdrPtr = llvm::GetElementPtrInst::CreateInBounds(Block,
      {zero, zero32, zero32}, "", CurrentBlock);
  auto BlockSize = module->getOrInsertGlobal("BLOCK_SIZE", llvm::Type::getInt64Ty(Ctx));
  auto BlockSizeVal = new llvm::LoadInst(BlockSize, "", CurrentBlock);
  auto BlockAllocSize = llvm::BinaryOperator::Create(llvm::Instruction::Sub,
      BlockSizeVal, llvm::ConstantExpr::getSizeOf(llvm::Type::getInt8PtrTy(Ctx)), "", CurrentBlock);
  auto icmp = new llvm::ICmpInst(*CurrentBlock, llvm::CmpInst::ICMP_UGT,
      Len, BlockAllocSize);
  auto Mask = llvm::SelectInst::Create(icmp, llvm::ConstantInt::get(llvm::Type::getInt64Ty(Ctx), NOT_YOUNG_OBJECT_BIT), llvm::ConstantInt::get(llvm::Type::getInt64Ty(Ctx), 0), "", CurrentBlock);
  auto HdrOred = llvm::BinaryOperator::Create(llvm::Instruction::Or,
      func->arg_begin()+1, Mask, "", CurrentBlock);
  new llvm::StoreInst(HdrOred, HdrPtr, CurrentBlock);
  llvm::Constant *Memcpy = module->getOrInsertFunction("memcpy", 
      llvm::Type::getInt8PtrTy(Ctx), llvm::Type::getInt8PtrTy(Ctx),
      llvm::Type::getInt8PtrTy(Ctx), llvm::Type::getInt64Ty(Ctx));
  auto StrPtr = llvm::GetElementPtrInst::CreateInBounds(Block,
      {zero, llvm::ConstantInt::get(llvm::Type::getInt32Ty(Ctx), 1), zero}, "", CurrentBlock);
  llvm::CallInst::Create(Memcpy, {StrPtr, func->arg_begin()+2, func->arg_begin()+1},
      "", CurrentBlock);
  auto cast = new llvm::BitCastInst(Block,
      llvm::Type::getInt8PtrTy(Ctx), "", CurrentBlock);
  llvm::BranchInst::Create(MergeBlock, CurrentBlock);
  Phi->addIncoming(cast, CurrentBlock);
  llvm::ReturnInst::Create(Ctx, Phi, MergeBlock);
  MergeBlock->insertInto(func);
}

static llvm::PointerType *makeVisitorType(llvm::LLVMContext &Ctx, llvm::Type *file, llvm::Type *item, int numStrs, int numBools) {
  std::vector<llvm::Type *> types;
  types.push_back(file);
  types.push_back(item);
  for (int i = 0; i < numStrs; i++) {
    types.push_back(llvm::Type::getInt8PtrTy(Ctx));
  }
  for (int i = 0; i < numBools; i++) {
    types.push_back(llvm::Type::getInt1Ty(Ctx));
  }
  return llvm::PointerType::getUnqual(llvm::FunctionType::get(llvm::Type::getVoidTy(Ctx), types, false));
}

static void emitTraversal(std::string name, KOREDefinition *definition, llvm::Module *module, 
    bool isVisitor, void getter(
      KOREDefinition *, 
      llvm::Module *, 
      KORESymbol *, 
      llvm::BasicBlock *)) {
  llvm::LLVMContext &Ctx = module->getContext();
  std::vector<llvm::Type *> argTypes;
  argTypes.push_back(getValueType({SortCategory::Symbol, 0}, module));
  if (isVisitor) {
    // cf runtime/configurationparser/header.h visitChildren
    auto file = llvm::PointerType::getUnqual(llvm::StructType::create(Ctx, "writer"));
    argTypes.push_back(file);
    argTypes.push_back(makeVisitorType(Ctx, file, getValueType({SortCategory::Symbol, 0}, module), 1, 1));
    argTypes.push_back(makeVisitorType(Ctx, file, llvm::PointerType::getUnqual(getValueType({SortCategory::Map, 0}, module)), 3, 0));
    argTypes.push_back(makeVisitorType(Ctx, file, llvm::PointerType::getUnqual(getValueType({SortCategory::List, 0}, module)), 3, 0));
    argTypes.push_back(makeVisitorType(Ctx, file, llvm::PointerType::getUnqual(getValueType({SortCategory::Set, 0}, module)), 3, 0));
    argTypes.push_back(makeVisitorType(Ctx, file, getValueType({SortCategory::Int, 0}, module), 1, 0));
    argTypes.push_back(makeVisitorType(Ctx, file, getValueType({SortCategory::Float, 0}, module), 1, 0));
    argTypes.push_back(makeVisitorType(Ctx, file, getValueType({SortCategory::Bool, 0}, module), 1, 0));
    argTypes.push_back(makeVisitorType(Ctx, file, getValueType({SortCategory::StringBuffer, 0}, module), 1, 0));
    argTypes.push_back(makeVisitorType(Ctx, file, llvm::Type::getInt8PtrTy(Ctx), 1, 0));
    argTypes.push_back(llvm::PointerType::getUnqual(llvm::FunctionType::get(llvm::Type::getVoidTy(Ctx), {file}, false)));
  } else {
    argTypes.push_back(llvm::PointerType::getUnqual(llvm::ArrayType::get(llvm::Type::getInt8PtrTy(Ctx), 0)));
  }
  auto func = llvm::dyn_cast<llvm::Function>(module->getOrInsertFunction(
      name, llvm::FunctionType::get(llvm::Type::getVoidTy(Ctx), argTypes, false)));
  llvm::Constant *zero = llvm::ConstantInt::get(llvm::Type::getInt64Ty(Ctx), 0);
  llvm::Constant *zero32 = llvm::ConstantInt::get(llvm::Type::getInt32Ty(Ctx), 0);
  auto EntryBlock = llvm::BasicBlock::Create(Ctx, "entry", func);
  auto HdrPtr = llvm::GetElementPtrInst::CreateInBounds(func->arg_begin(),
      {zero, zero32, zero32}, "", EntryBlock);
  auto Hdr = new llvm::LoadInst(HdrPtr, "", EntryBlock);
  auto Tag = new llvm::TruncInst(Hdr, llvm::Type::getInt32Ty(Ctx), "", EntryBlock);
  auto stuck = llvm::BasicBlock::Create(Ctx, "stuck");
  auto &syms = definition->getSymbols();
  auto Switch = llvm::SwitchInst::Create(Tag, stuck, syms.size(), EntryBlock);
  for (auto iter = syms.begin(); iter != syms.end(); ++iter) {
    auto entry = *iter;
    uint32_t tag = entry.first;
    auto symbol = entry.second;
    if (symbol->getArguments().empty()) {
      continue;
    }
    auto CaseBlock = llvm::BasicBlock::Create(Ctx, "tag" + std::to_string(tag), func);
    Switch->addCase(llvm::ConstantInt::get(llvm::Type::getInt32Ty(Ctx), tag), CaseBlock);
    getter(definition, module, symbol, CaseBlock);
    llvm::ReturnInst::Create(Ctx, CaseBlock);
  }
  addAbort(stuck, module);
  stuck->insertInto(func); 
}

static void getStore(KOREDefinition *definition, llvm::Module *module, KORESymbol *symbol, 
    llvm::BasicBlock *CaseBlock) {
  llvm::LLVMContext &Ctx = module->getContext();
  llvm::Constant *zero = llvm::ConstantInt::get(llvm::Type::getInt64Ty(Ctx), 0);
  llvm::Function *func = CaseBlock->getParent();
  llvm::Value *ArgumentsArray = func->arg_begin() + 1;
  int idx = 0;
  auto BlockType = getBlockType(module, definition, symbol);
  auto cast = new llvm::BitCastInst(func->arg_begin(),
      llvm::PointerType::getUnqual(BlockType), "", CaseBlock);
  for (auto &sort : symbol->getArguments()) {
    ValueType cat = dynamic_cast<KORECompositeSort *>(sort.get())->getCategory(definition);
    llvm::Value *arg = getArgValue(ArgumentsArray, idx, CaseBlock, cat, module);
    llvm::Value *ChildPtr = llvm::GetElementPtrInst::CreateInBounds(BlockType, cast,
        {zero, llvm::ConstantInt::get(llvm::Type::getInt32Ty(Ctx), idx++ + 2)}, "", CaseBlock);
    if (arg->getType() == ChildPtr->getType()) {
      arg = new llvm::LoadInst(arg, "", CaseBlock);
    }
    new llvm::StoreInst(arg, ChildPtr, CaseBlock);
  }
}

static void emitStoreSymbolChildren(KOREDefinition *definition, llvm::Module *module) {
  emitTraversal("storeSymbolChildren", definition, module, false, getStore);
}

static llvm::Constant *getSymbolName(KOREDefinition *definition, llvm::Module *module, KORESymbol *symbol) {
  return getSymbolNamePtr(symbol, nullptr, module);
}

static void emitGetSymbolNameForTag(KOREDefinition *def, llvm::Module *mod) {
  emitDataTableForSymbol("getSymbolNameForTag", llvm::Type::getInt8PtrTy(mod->getContext()), getCharPtrDebugType(), def, mod, getSymbolName);
}

static void visitCollection(KOREDefinition *definition, llvm::Module *module, KORECompositeSort *compositeSort, llvm::Function *func, llvm::Value *ChildPtr, llvm::BasicBlock *CaseBlock, unsigned offset) {
  llvm::LLVMContext &Ctx = module->getContext();
  llvm::Constant *zero = llvm::ConstantInt::get(llvm::Type::getInt64Ty(Ctx), 0);
  auto indices = std::vector<llvm::Constant *>{zero, zero};
  auto sortDecl = definition->getSortDeclarations().at(compositeSort->getName());
  llvm::Constant *concatPtr;
  if (sortDecl->getAttributes().count("concat")) {
    auto concat = (KORECompositePattern *)sortDecl->getAttributes().at("concat")->getArguments()[0].get();
    concatPtr = getSymbolNamePtr(concat->getConstructor(), nullptr, module);
  } else {
    concatPtr = llvm::ConstantPointerNull::get(llvm::Type::getInt8PtrTy(Ctx));
  }
  auto unit = (KORECompositePattern *)sortDecl->getAttributes().at("unit")->getArguments()[0].get();
  auto unitPtr = getSymbolNamePtr(unit->getConstructor(), nullptr, module);
  auto element = (KORECompositePattern *)sortDecl->getAttributes().at("element")->getArguments()[0].get();
  auto elementPtr = getSymbolNamePtr(element->getConstructor(), nullptr, module);
  llvm::CallInst::Create(func->arg_begin()+offset, {func->arg_begin()+1, ChildPtr, unitPtr, elementPtr, concatPtr}, "", CaseBlock);
}

static void getVisitor(KOREDefinition *definition, llvm::Module *module, KORESymbol *symbol, llvm::BasicBlock *CaseBlock) {
  llvm::LLVMContext &Ctx = module->getContext();
  llvm::Constant *zero = llvm::ConstantInt::get(llvm::Type::getInt64Ty(Ctx), 0);
  auto indices = std::vector<llvm::Constant *>{zero, zero};
  llvm::Function *func = CaseBlock->getParent();
  int idx = 0;
  auto BlockType = getBlockType(module, definition, symbol);
  auto cast = new llvm::BitCastInst(func->arg_begin(),
      llvm::PointerType::getUnqual(BlockType), "", CaseBlock);
  unsigned i = 0;
  for (auto sort : symbol->getArguments()) {
    auto compositeSort = dynamic_cast<KORECompositeSort *>(sort.get());
    ValueType cat = compositeSort->getCategory(definition);
    llvm::Value *ChildPtr = llvm::GetElementPtrInst::CreateInBounds(BlockType, cast,
        {zero, llvm::ConstantInt::get(llvm::Type::getInt32Ty(Ctx), idx++ + 2)}, "", CaseBlock);
    llvm::Value *Child = new llvm::LoadInst(ChildPtr, "", CaseBlock);
    std::ostringstream Out;
    sort->print(Out);
    auto Str = llvm::ConstantDataArray::getString(Ctx, Out.str(), true);
    auto global = module->getOrInsertGlobal("sort_name_" + Out.str(), Str->getType());
    llvm::GlobalVariable *globalVar = llvm::dyn_cast<llvm::GlobalVariable>(global);
    if (!globalVar->hasInitializer()) {
      globalVar->setInitializer(Str);
    }
    llvm::Constant *CharPtr = llvm::ConstantExpr::getInBoundsGetElementPtr(Str->getType(), global, indices);
    switch(cat.cat) {
    case SortCategory::Variable:
    case SortCategory::Symbol:
      llvm::CallInst::Create(func->arg_begin()+2, {func->arg_begin()+1, Child, CharPtr, llvm::ConstantInt::get(llvm::Type::getInt1Ty(Ctx), cat.cat == SortCategory::Variable)}, "", CaseBlock);
      break;
    case SortCategory::Int:
      llvm::CallInst::Create(func->arg_begin()+6, {func->arg_begin()+1, Child, CharPtr}, "", CaseBlock);
      break;
    case SortCategory::Float:
      llvm::CallInst::Create(func->arg_begin()+7, {func->arg_begin()+1, Child, CharPtr}, "", CaseBlock);
      break;
    case SortCategory::Bool:
      llvm::CallInst::Create(func->arg_begin()+8, {func->arg_begin()+1, Child, CharPtr}, "", CaseBlock);
      break;
    case SortCategory::StringBuffer:
      llvm::CallInst::Create(func->arg_begin()+9, {func->arg_begin()+1, Child, CharPtr}, "", CaseBlock);
      break;
    case SortCategory::MInt:
      ChildPtr = new llvm::BitCastInst(ChildPtr, llvm::Type::getInt8PtrTy(Ctx), "", CaseBlock);
      llvm::CallInst::Create(func->arg_begin()+10, {func->arg_begin()+1, ChildPtr, CharPtr}, "", CaseBlock);
      break;
    case SortCategory::Map:
      visitCollection(definition, module, compositeSort, func, ChildPtr, CaseBlock, 3);
      break;
    case SortCategory::Set:
      visitCollection(definition, module, compositeSort, func, ChildPtr, CaseBlock, 5);
      break;
    case SortCategory::List: {
      visitCollection(definition, module, compositeSort, func, ChildPtr, CaseBlock, 4);
      break;
    } case SortCategory::Uncomputed:
      abort();
    }
    if (i != symbol->getArguments().size() - 1) {
      llvm::CallInst::Create(func->arg_begin()+11, {func->arg_begin()+1}, "", CaseBlock);
    }
    i++;
  }
}

static llvm::Constant *getLayoutData(uint16_t layout, KORESymbol *symbol, llvm::Module *module, KOREDefinition *def) {
  uint8_t len = symbol->getArguments().size();
  std::vector<llvm::Constant *> elements;
  llvm::LLVMContext &Ctx = module->getContext();
  auto BlockType = getBlockType(module, def, symbol);
  int i = 2;
  for (auto sort : symbol->getArguments()) {
    ValueType cat = dynamic_cast<KORECompositeSort *>(sort.get())->getCategory(def);
    auto offset = llvm::ConstantExpr::getOffsetOf(BlockType, i++);
    elements.push_back(llvm::ConstantStruct::get(module->getTypeByName(LAYOUTITEM_STRUCT), offset, llvm::ConstantInt::get(llvm::Type::getInt16Ty(Ctx), (int)cat.cat + cat.bits)));
  }
  auto Arr = llvm::ConstantArray::get(llvm::ArrayType::get(module->getTypeByName(LAYOUTITEM_STRUCT), len), elements);
  auto global = module->getOrInsertGlobal("layout_item_" + std::to_string(layout), Arr->getType());
  llvm::GlobalVariable *globalVar = llvm::dyn_cast<llvm::GlobalVariable>(global);
  if (!globalVar->hasInitializer()) {
    globalVar->setInitializer(Arr);
  }
  llvm::Constant *zero = llvm::ConstantInt::get(llvm::Type::getInt64Ty(Ctx), 0);
  auto indices = std::vector<llvm::Constant *>{zero, zero};
  auto Ptr = llvm::ConstantExpr::getInBoundsGetElementPtr(Arr->getType(), globalVar, indices);
  std::string name = "layout_" + std::to_string(layout);
  auto global2 = module->getOrInsertGlobal(name, module->getTypeByName(LAYOUT_STRUCT));
  llvm::GlobalVariable *globalVar2 = llvm::dyn_cast<llvm::GlobalVariable>(global2);
  initDebugGlobal(name, getForwardDecl(LAYOUT_STRUCT), globalVar2);
  if (!globalVar2->hasInitializer()) {
    globalVar2->setInitializer(llvm::ConstantStruct::get(module->getTypeByName(LAYOUT_STRUCT), llvm::ConstantInt::get(llvm::Type::getInt8Ty(Ctx), len), Ptr));
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
  auto func = llvm::dyn_cast<llvm::Function>(module->getOrInsertFunction(
      "getLayoutData", llvm::FunctionType::get(llvm::PointerType::getUnqual(module->getTypeByName(LAYOUT_STRUCT)), argTypes, false)));
  initDebugFunction("getLayoutData", "getLayoutData", getDebugFunctionType(getPointerDebugType(getForwardDecl(LAYOUT_STRUCT)), {getShortDebugType()}), definition, func);
  auto EntryBlock = llvm::BasicBlock::Create(Ctx, "entry", func);
  auto MergeBlock = llvm::BasicBlock::Create(Ctx, "exit");
  auto stuck = llvm::BasicBlock::Create(Ctx, "stuck");
  auto Switch = llvm::SwitchInst::Create(func->arg_begin(), stuck, layouts.size(), EntryBlock);
  auto Phi = llvm::PHINode::Create(llvm::PointerType::getUnqual(module->getTypeByName(LAYOUT_STRUCT)), layouts.size(), "phi", MergeBlock);
  for (auto iter = layouts.begin(); iter != layouts.end(); ++iter) {
    auto entry = *iter;
    uint16_t layout = entry.first;
    auto symbol = entry.second;
    auto CaseBlock = llvm::BasicBlock::Create(Ctx, "layout" + std::to_string(layout), func);
    llvm::BranchInst::Create(MergeBlock, CaseBlock);
    auto data = getLayoutData(layout, symbol, module, definition);
    Phi->addIncoming(data, CaseBlock);
    Switch->addCase(llvm::ConstantInt::get(llvm::Type::getInt16Ty(Ctx), layout), CaseBlock);
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
  auto global = mod->getOrInsertGlobal("first_inj_tag", llvm::Type::getInt32Ty(Ctx));
  llvm::GlobalVariable *globalVar = llvm::dyn_cast<llvm::GlobalVariable>(global);
  globalVar->setConstant(true);
  if (!globalVar->hasInitializer()) {
    globalVar->setInitializer(llvm::ConstantInt::get(llvm::Type::getInt32Ty(Ctx), def->getInjSymbol()->getFirstTag()));
  }
  global = mod->getOrInsertGlobal("last_inj_tag", llvm::Type::getInt32Ty(Ctx));
  globalVar = llvm::dyn_cast<llvm::GlobalVariable>(global);
  globalVar->setConstant(true);
  if (!globalVar->hasInitializer()) {
    globalVar->setInitializer(llvm::ConstantInt::get(llvm::Type::getInt32Ty(Ctx), def->getInjSymbol()->getLastTag()));
  }
}

void emitConfigParserFunctions(KOREDefinition *definition, llvm::Module *module) {
  emitGetTagForSymbolName(definition, module); 
  emitGetBlockHeaderForSymbol(definition, module); 
  emitIsSymbolAFunction(definition, module); 
  emitIsSymbolABinder(definition, module); 
  emitStoreSymbolChildren(definition, module); 
  emitEvaluateFunctionSymbol(definition, module); 
  emitGetToken(definition, module); 
  emitGetTagForFreshSort(definition, module);
  emitGetInjectionForSortOfTag(definition, module);

  emitGetSymbolNameForTag(definition, module);
  emitVisitChildren(definition, module);

  emitLayouts(definition, module);

  emitInjTags(definition, module);
}

}
