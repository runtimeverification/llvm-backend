#include "kllvm/codegen/EmitConfigParser.h"
#include "kllvm/codegen/CreateTerm.h"

#include "llvm/IR/Constants.h"
#include "llvm/IR/Instructions.h"

namespace kllvm {

static llvm::Constant *getSymbolNamePtr(KOREObjectSymbol *symbol, llvm::BasicBlock *SetBlockName, llvm::Module *module) {
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
  auto &syms = definition->getSymbols();
  llvm::Constant *Strcmp = module->getOrInsertFunction("strcmp", 
      llvm::Type::getInt32Ty(Ctx), llvm::Type::getInt8PtrTy(Ctx),
      llvm::Type::getInt8PtrTy(Ctx));
  for (auto iter = syms.begin(); iter != syms.end(); ++iter) {
    auto entry = *iter;
    uint32_t tag = entry.first;
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
static std::string STRING_STRUCT = "string";

static void emitDataForSymbol(std::string name, llvm::Type *ty, KOREDefinition *definition, llvm::Module *module, bool isEval,
    std::pair<llvm::Value *, llvm::BasicBlock *> getter(KOREDefinition *, llvm::Module *,
        KOREObjectSymbol *, llvm::Instruction *)) {
  llvm::LLVMContext &Ctx = module->getContext();
  std::vector<llvm::Type *> argTypes;
  argTypes.push_back(llvm::Type::getInt32Ty(Ctx));
  if (isEval) {
    auto ty = llvm::PointerType::getUnqual(llvm::ArrayType::get(llvm::Type::getInt8PtrTy(Ctx), 0));
    argTypes.push_back(ty);
  }
  auto func = llvm::dyn_cast<llvm::Function>(module->getOrInsertFunction(
      name, llvm::FunctionType::get(ty, argTypes, false)));
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
    auto decl = definition->getSymbolDeclarations().lookup(symbol->getName());
    bool isFunc = decl->getAttributes().count("function");
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
    KOREObjectSymbol *symbol, llvm::Instruction *inst) {
  auto BlockType = getBlockType(module, definition, symbol);
  return std::make_pair(getBlockHeader(module, definition, symbol, BlockType), inst->getParent());
}

static void emitGetBlockHeaderForSymbol(KOREDefinition *def, llvm::Module *mod) {
  emitDataForSymbol("getBlockHeaderForSymbol", mod->getTypeByName(BLOCKHEADER_STRUCT),
      def, mod, false, getHeader);
}

static std::pair<llvm::Value *, llvm::BasicBlock *> getFunction(KOREDefinition *def, llvm::Module *mod,
    KOREObjectSymbol *symbol, llvm::Instruction *inst) {
  auto decl = def->getSymbolDeclarations().lookup(symbol->getName());
  bool res = decl->getAttributes().count("function");
  return std::make_pair(llvm::ConstantInt::get(llvm::Type::getInt1Ty(mod->getContext()), res),
      inst->getParent());
}

static void emitIsSymbolAFunction(KOREDefinition *def, llvm::Module *mod) {
  emitDataForSymbol("isSymbolAFunction", llvm::Type::getInt1Ty(mod->getContext()),
      def, mod, false, getFunction);
}

static llvm::Value *getArgValue(llvm::Value *ArgumentsArray, int idx,
    llvm::BasicBlock *CaseBlock, SortCategory cat, llvm::Module *mod) {
  llvm::LLVMContext &Ctx = mod->getContext();
  llvm::Constant *zero = llvm::ConstantInt::get(llvm::Type::getInt64Ty(Ctx), 0);
  auto addr = llvm::GetElementPtrInst::Create(
      llvm::ArrayType::get(llvm::Type::getInt8PtrTy(Ctx), 0),
      ArgumentsArray, {zero, llvm::ConstantInt::get(llvm::Type::getInt64Ty(Ctx), idx)},
      "", CaseBlock);
  llvm::Value *arg = new llvm::LoadInst(addr, "", CaseBlock);
  switch(cat) {
  case SortCategory::Map:
  case SortCategory::List:
  case SortCategory::Set:
  case SortCategory::Bool:
  case SortCategory::MInt: {
    auto cast = new llvm::BitCastInst(arg,
        llvm::PointerType::getUnqual(getValueType(cat, mod)), "", CaseBlock);
    auto load = new llvm::LoadInst(cast, "", CaseBlock);
    CaseBlock->getInstList().push_back(llvm::CallInst::CreateFree(arg, CaseBlock));
    arg = load;
    break;
  }
  case SortCategory::Int:
  case SortCategory::Float:
  case SortCategory::StringBuffer:
  case SortCategory::Symbol:
    arg = new llvm::BitCastInst(arg, getValueType(cat, mod), "", CaseBlock);
    break;
  case SortCategory::Uncomputed:
    abort();
  }
  return arg;
}

static std::pair<llvm::Value *, llvm::BasicBlock *> getEval(KOREDefinition *def, llvm::Module *mod,
    KOREObjectSymbol *symbol, llvm::Instruction *inst) {
  llvm::LLVMContext &Ctx = mod->getContext();
  llvm::BasicBlock *CaseBlock = inst->getParent();
  inst->removeFromParent();
  llvm::Function *func = CaseBlock->getParent();
  llvm::Value *ArgumentsArray = func->arg_begin() + 1;
  int idx = 0;
  llvm::StringMap<llvm::Value *> subst;
  auto pattern = KOREObjectCompositePattern::Create(symbol);
  for (auto sort : symbol->getArguments()) {
    SortCategory cat = dynamic_cast<KOREObjectCompositeSort *>(sort)->getCategory(def);
    llvm::Value *arg = getArgValue(ArgumentsArray, idx, CaseBlock, cat, mod);
    std::string name = "_" + std::to_string(idx++);
    subst.insert({name, arg});
    pattern->addArgument(KOREObjectVariablePattern::Create(name, sort));
  }
  CreateTerm creator(subst, def, CaseBlock, mod);
  llvm::Value *result = creator(pattern);
  for (auto arg : pattern->getArguments()) {
    delete arg;
  }
  delete pattern;
  llvm::Value *retval;
  SortCategory cat = dynamic_cast<KOREObjectCompositeSort *>(symbol->getSort())->getCategory(def);
  switch(cat) {
  case SortCategory::Map:
  case SortCategory::List:
  case SortCategory::Set:
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
  case SortCategory::Int:
  case SortCategory::Float:
  case SortCategory::StringBuffer:
  case SortCategory::Symbol:
    retval = new llvm::BitCastInst(result, llvm::Type::getInt8PtrTy(Ctx), "",
        creator.getCurrentBlock());
    break;
  case SortCategory::Uncomputed:
    abort();
  }
  creator.getCurrentBlock()->getInstList().push_back(inst);
  return std::make_pair(retval, creator.getCurrentBlock());
}

static void emitEvaluateFunctionSymbol(KOREDefinition *def, llvm::Module *mod) {
  emitDataForSymbol("evaluateFunctionSymbol", llvm::Type::getInt8PtrTy(mod->getContext()),
      def, mod, true, getEval);
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
    std::string name = entry.first();
    auto sort = KOREObjectCompositeSort::Create(name);
    SortCategory cat = sort->getCategory(definition);
    if (cat == SortCategory::Symbol) {
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
    switch(cat) {
    case SortCategory::Map:
    case SortCategory::List:
    case SortCategory::Set:
      addAbort(CaseBlock, module);
      break;
    case SortCategory::Float:
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
    case SortCategory::Int: {
      llvm::Type *Int = module->getTypeByName(INT_STRUCT);
      llvm::Value *Block = allocateBlock(Int, CaseBlock);
      llvm::Constant *MpzInitSet = module->getOrInsertFunction("__gmpz_init_set_str",
          llvm::Type::getInt32Ty(Ctx), llvm::PointerType::getUnqual(Int), 
          llvm::Type::getInt8PtrTy(Ctx), llvm::Type::getInt32Ty(Ctx));
      auto Call = llvm::CallInst::Create(MpzInitSet, {Block, func->arg_begin()+2,
          llvm::ConstantInt::get(llvm::Type::getInt32Ty(Ctx), 10)}, "", CaseBlock);
      auto icmp = new llvm::ICmpInst(*CaseBlock, llvm::CmpInst::ICMP_EQ, 
          Call, zero32);
      auto AbortBlock = llvm::BasicBlock::Create(Ctx, "invalid_int", func);
      addAbort(AbortBlock, module);
      auto cast = new llvm::BitCastInst(Block,
          llvm::Type::getInt8PtrTy(Ctx), "", CaseBlock);
      llvm::BranchInst::Create(MergeBlock, AbortBlock, icmp, CaseBlock);
      Phi->addIncoming(cast, CaseBlock);
      break;
    }
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
  llvm::Value *Block = allocateBlock(StringType, Len, CurrentBlock);
  auto HdrPtr = llvm::GetElementPtrInst::CreateInBounds(Block,
      {zero, zero32, zero32}, "", CurrentBlock);
  new llvm::StoreInst(func->arg_begin()+1, HdrPtr, CurrentBlock);
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

static void emitStoreSymbolChildren(KOREDefinition *definition, llvm::Module *module) {
  llvm::LLVMContext &Ctx = module->getContext();
  auto func = llvm::dyn_cast<llvm::Function>(module->getOrInsertFunction(
      "storeSymbolChildren", llvm::Type::getVoidTy(Ctx),
      getValueType(SortCategory::Symbol, module),
      llvm::PointerType::getUnqual(llvm::ArrayType::get(llvm::Type::getInt8PtrTy(Ctx), 0))));
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
    llvm::Value *ArgumentsArray = func->arg_begin() + 1;
    int idx = 0;
    auto BlockType = getBlockType(module, definition, symbol);
    for (auto sort : symbol->getArguments()) {
      SortCategory cat = dynamic_cast<KOREObjectCompositeSort *>(sort)->getCategory(definition);
      llvm::Value *arg = getArgValue(ArgumentsArray, idx, CaseBlock, cat, module);
      auto cast = new llvm::BitCastInst(func->arg_begin(),
          llvm::PointerType::getUnqual(BlockType), "", CaseBlock);
      llvm::Value *ChildPtr = llvm::GetElementPtrInst::CreateInBounds(BlockType, cast,
          {zero, llvm::ConstantInt::get(llvm::Type::getInt32Ty(Ctx), idx++ + 2)}, "", CaseBlock);
      new llvm::StoreInst(arg, ChildPtr, CaseBlock);
    }
    llvm::ReturnInst::Create(Ctx, CaseBlock);
  }
  addAbort(stuck, module);
  stuck->insertInto(func); 
}

void emitConfigParserFunctions(KOREDefinition *definition, llvm::Module *module) {
  emitGetTagForSymbolName(definition, module); 
  emitGetBlockHeaderForSymbol(definition, module); 
  emitIsSymbolAFunction(definition, module); 
  emitStoreSymbolChildren(definition, module); 
  emitEvaluateFunctionSymbol(definition, module); 
  emitGetToken(definition, module); 
}

}
