#include "kllvm/codegen/Decision.h"
#include "kllvm/codegen/CreateTerm.h"
#include "kllvm/codegen/Debug.h"
#include "kllvm/codegen/Util.h"

#include "llvm/IR/CFG.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Support/raw_ostream.h"

#include <iostream>
#include <limits>

namespace kllvm {

static std::string BLOCK_STRUCT = "block";
static std::string LAYOUTITEM_STRUCT = "layoutitem";

FailNode FailNode::instance;

static unsigned max_name_length
    = 1024
      - std::to_string(std::numeric_limits<unsigned long long>::max()).length();

void Decision::operator()(DecisionNode *entry) {
  if (entry == FailNode::get()) {
    if (FailPattern) {
      llvm::Value *val = load(std::make_pair(
          "_1", getValueType({SortCategory::Symbol, 0}, Module)));
      FailSubject->addIncoming(
          new llvm::BitCastInst(
              val, llvm::Type::getInt8PtrTy(Ctx, 1), "", CurrentBlock),
          CurrentBlock);
      FailPattern->addIncoming(
          stringLiteral("\\bottom{SortGeneratedTopCell{}}()"), CurrentBlock);
      FailSort->addIncoming(
          stringLiteral("SortGeneratedTopCell{}"), CurrentBlock);
    }
    llvm::BranchInst::Create(this->FailureBlock, this->CurrentBlock);
  } else {
    entry->codegen(this);
  }
}

llvm::Value *Decision::ptrTerm(llvm::Value *val) {
  if (val->getType()->isIntegerTy()) {
    val = allocateTermNoReloc(val->getType(), CurrentBlock);
    val = addrspaceCast0to1(val, CurrentBlock);
  }
  return new llvm::BitCastInst(
      val, llvm::Type::getInt8PtrTy(Ctx, 1), "", CurrentBlock);
}

bool DecisionNode::beginNode(Decision *d, std::string name) {
  if (isCompleted()) {
    llvm::BranchInst::Create(cachedCode, d->CurrentBlock);
    return true;
  }
  auto Block = llvm::BasicBlock::Create(
      d->Ctx, name.substr(0, max_name_length), d->CurrentBlock->getParent());
  cachedCode = Block;
  llvm::BranchInst::Create(Block, d->CurrentBlock);
  d->CurrentBlock = Block;
  return false;
}

static std::pair<std::string, std::string>
getFailPattern(DecisionCase const &_case, bool isInt) {
  if (isInt) {
    size_t bitwidth = _case.getLiteral().getBitWidth();
    if (bitwidth == 1) {
      return std::make_pair(
          "SortBool{}", "\\dv{SortBool{}}(\""
                            + (_case.getLiteral() == 0 ? std::string("false")
                                                       : std::string("true"))
                            + "\")");
    } else {
      std::string sort = "SortMInt{Sort" + std::to_string(bitwidth) + "{}}";
      return std::make_pair(
          sort, "\\dv{" + sort + "}(\"" + _case.getLiteral().toString(10, false)
                    + "p" + std::to_string(bitwidth) + "\")");
    }
  } else {
    std::ostringstream symbol;
    _case.getConstructor()->print(symbol);
    std::ostringstream returnSort;
    _case.getConstructor()->getSort()->print(returnSort);
    std::string result = symbol.str() + "(";
    std::string conn = "";
    for (int i = 0; i < _case.getConstructor()->getArguments().size(); i++) {
      result += conn;
      result += "Var'Unds'";
      std::ostringstream argSort;
      _case.getConstructor()->getArguments()[i]->print(argSort);
      result += ":" + argSort.str();
      conn = ",";
    }
    result += ")";
    return std::make_pair(returnSort.str(), result);
  }
}

static std::pair<std::string, std::string> getFailPattern(
    std::vector<std::pair<llvm::BasicBlock *, const DecisionCase *>> const
        &caseData,
    bool isInt, llvm::BasicBlock *FailBlock) {
  std::string reason;
  std::string sort;
  for (auto &entry : caseData) {
    auto &_case = *entry.second;
    if (entry.first != FailBlock) {
      auto caseReason = getFailPattern(_case, isInt);
      if (reason.empty()) {
        reason = caseReason.second;
        sort = caseReason.first;
      } else {
        reason = "\\or{" + sort + "}(" + reason + "," + caseReason.second + ")";
      }
    }
  }
  return std::make_pair(sort, reason);
}

void SwitchNode::codegen(Decision *d) {
  if (beginNode(d, "switch" + name)) {
    return;
  }
  llvm::Value *val = d->load(std::make_pair(name, type));
  llvm::Value *ptrVal;
  if (d->FailPattern) {
    ptrVal = d->ptrTerm(val);
  }
  llvm::BasicBlock *_default = d->FailureBlock;
  const DecisionCase *defaultCase = nullptr;
  std::vector<std::pair<llvm::BasicBlock *, const DecisionCase *>> caseData;
  int idx = 0;
  bool isInt = false;
  for (auto &_case : cases) {
    auto child = _case.getChild();
    llvm::BasicBlock *CaseBlock;
    if (child == FailNode::get()) {
      CaseBlock = d->FailureBlock;
    } else {
      CaseBlock = llvm::BasicBlock::Create(
          d->Ctx,
          name.substr(0, max_name_length) + "_case_" + std::to_string(idx++),
          d->CurrentBlock->getParent());
    }
    if (auto sym = _case.getConstructor()) {
      isInt = isInt || sym->getName() == "\\dv";
      caseData.push_back(std::make_pair(CaseBlock, &_case));
    } else {
      _default = CaseBlock;
      defaultCase = &_case;
    }
  }
  if (isCheckNull) {
    auto cast = ptrToInt(val, d->CurrentBlock);
    auto cmp = new llvm::ICmpInst(
        *d->CurrentBlock, llvm::CmpInst::ICMP_NE, cast,
        llvm::ConstantExpr::getPtrToInt(
            llvm::ConstantPointerNull::get(
                llvm::dyn_cast<llvm::PointerType>(val->getType())),
            llvm::Type::getInt64Ty(d->Ctx)));
    val = cmp;
    isInt = true;
  }
  llvm::Value *failSort, *failPattern;
  if (d->FailPattern) {
    auto failReason = getFailPattern(caseData, isInt, d->FailureBlock);
    failSort = d->stringLiteral(failReason.first);
    failPattern = d->stringLiteral(failReason.second);
  }
  if (isInt) {
    auto _switch = llvm::SwitchInst::Create(
        val, _default, cases.size(), d->CurrentBlock);
    for (auto &_case : caseData) {
      _switch->addCase(
          llvm::ConstantInt::get(d->Ctx, _case.second->getLiteral()),
          _case.first);
    }
  } else {
    if (caseData.size() == 0) {
      llvm::BranchInst::Create(_default, d->CurrentBlock);
    } else {
      llvm::Value *tagVal = d->getTag(val);
      auto _switch = llvm::SwitchInst::Create(
          tagVal, _default, caseData.size(), d->CurrentBlock);
      for (auto &_case : caseData) {
        _switch->addCase(
            llvm::ConstantInt::get(
                llvm::Type::getInt32Ty(d->Ctx),
                _case.second->getConstructor()->getTag()),
            _case.first);
      }
    }
  }
  auto currChoiceBlock = d->ChoiceBlock;
  d->ChoiceBlock = nullptr;
  auto switchBlock = d->CurrentBlock;
  for (auto &entry : caseData) {
    auto &_case = *entry.second;
    if (entry.first == d->FailureBlock) {
      if (d->FailPattern) {
        d->FailSubject->addIncoming(ptrVal, switchBlock);
        d->FailPattern->addIncoming(failPattern, switchBlock);
        d->FailSort->addIncoming(failSort, switchBlock);
      }
      continue;
    }
    d->CurrentBlock = entry.first;
    if (!isInt) {
      int offset = 0;
      llvm::StructType *BlockType
          = getBlockType(d->Module, d->Definition, _case.getConstructor());
      llvm::BitCastInst *Cast = new llvm::BitCastInst(
          val, llvm::PointerType::get(BlockType, 1), "", d->CurrentBlock);
      KORESymbolDeclaration *symbolDecl
          = d->Definition->getSymbolDeclarations().at(
              _case.getConstructor()->getName());
      llvm::Instruction *Renamed;
      for (auto binding : _case.getBindings()) {
        llvm::Value *ChildPtr = llvm::GetElementPtrInst::CreateInBounds(
            BlockType, Cast,
            {llvm::ConstantInt::get(llvm::Type::getInt64Ty(d->Ctx), 0),
             llvm::ConstantInt::get(
                 llvm::Type::getInt32Ty(d->Ctx), offset + 2)},
            "", d->CurrentBlock);
        llvm::Value *Child;
        switch (dynamic_cast<KORECompositeSort *>(
                    _case.getConstructor()->getArguments()[offset].get())
                    ->getCategory(d->Definition)
                    .cat) {
        case SortCategory::Map:
        case SortCategory::List:
        case SortCategory::Set: Child = ChildPtr; break;
        default:
          Child = new llvm::LoadInst(
              ChildPtr->getType()->getPointerElementType(), ChildPtr,
              binding.first.substr(0, max_name_length), d->CurrentBlock);
          break;
        }
        auto BlockPtr
            = llvm::PointerType::get(getTypeByName(d->Module, BLOCK_STRUCT), 1);
        if (symbolDecl->getAttributes().count("binder")) {
          if (offset == 0) {
            Renamed = llvm::CallInst::Create(
                getOrInsertFunction(
                    d->Module, "alphaRename", BlockPtr, BlockPtr),
                Child, "renamedVar", d->CurrentBlock);
            setDebugLoc(Renamed);
            d->store(binding, Renamed);
          } else if (offset == _case.getBindings().size() - 1) {
            llvm::Instruction *Replaced = llvm::CallInst::Create(
                getOrInsertFunction(
                    d->Module, "replaceBinderIndex", BlockPtr, BlockPtr,
                    BlockPtr),
                {Child, Renamed}, "withUnboundIndex", d->CurrentBlock);
            setDebugLoc(Replaced);
            d->store(binding, Replaced);
          } else {
            d->store(binding, Child);
          }
        } else {
          d->store(binding, Child);
        }
        offset++;
      }
    } else {
      if (currChoiceBlock && _case.getLiteral() == 1) {
        auto PrevDepth = new llvm::LoadInst(
            d->ChoiceDepth->getType()->getPointerElementType(), d->ChoiceDepth,
            "", d->CurrentBlock);
        auto CurrDepth = llvm::BinaryOperator::Create(
            llvm::Instruction::Add, PrevDepth,
            llvm::ConstantInt::get(llvm::Type::getInt64Ty(d->Ctx), 1), "",
            d->CurrentBlock);
        new llvm::StoreInst(CurrDepth, d->ChoiceDepth, d->CurrentBlock);

        auto ty = d->ChoiceBuffer->getType()->getElementType();
        auto zero = llvm::ConstantInt::get(llvm::Type::getInt64Ty(d->Ctx), 0);
        auto currentElt = llvm::GetElementPtrInst::CreateInBounds(
            ty, d->ChoiceBuffer, {zero, CurrDepth}, "", d->CurrentBlock);
        new llvm::StoreInst(
            llvm::BlockAddress::get(
                d->CurrentBlock->getParent(), currChoiceBlock),
            currentElt, d->CurrentBlock);
        d->FailJump->addDestination(currChoiceBlock);
      }
    }
    _case.getChild()->codegen(d);
  }
  if (defaultCase) {
    if (_default != d->FailureBlock) {
      // process default also
      d->CurrentBlock = _default;
      defaultCase->getChild()->codegen(d);
    } else if (d->FailPattern) {
      d->FailSubject->addIncoming(ptrVal, switchBlock);
      d->FailPattern->addIncoming(failPattern, switchBlock);
      d->FailSort->addIncoming(failSort, switchBlock);
    }
  }

  setCompleted();
}

void MakePatternNode::codegen(Decision *d) {
  if (beginNode(d, "pattern" + name)) {
    return;
  }
  llvm::StringMap<llvm::Value *> finalSubst;
  for (auto use : uses) {
    finalSubst[use.first] = d->load(use);
  }
  CreateTerm creator(
      finalSubst, d->Definition, d->CurrentBlock, d->Module, false);
  llvm::Value *val = creator(pattern).first;
  d->CurrentBlock = creator.getCurrentBlock();
  d->store(std::make_pair(name, type), val);
  child->codegen(d);
  setCompleted();
}

void FunctionNode::codegen(Decision *d) {
  if (beginNode(d, "function" + name)) {
    return;
  }
  std::vector<llvm::Value *> args;
  llvm::StringMap<llvm::Value *> finalSubst;
  for (auto arg : bindings) {
    llvm::Value *val;
    if (arg.first.find_first_not_of("-0123456789") == std::string::npos) {
      val = llvm::ConstantInt::get(
          llvm::Type::getInt64Ty(d->Ctx), std::stoi(arg.first));
    } else {
      val = d->load(arg);
    }
    args.push_back(val);
    finalSubst[arg.first] = val;
  }
  CreateTerm creator(
      finalSubst, d->Definition, d->CurrentBlock, d->Module, false);
  auto Call = creator.createFunctionCall(
      function, cat, args, function.substr(0, 5) == "hook_", false);
  Call->setName(name.substr(0, max_name_length));
  d->store(std::make_pair(name, type), Call);
  if (d->FailPattern) {
    std::string debugName = function;
    if (function.substr(0, 5) == "hook_") {
      debugName = function.substr(5, function.find_first_of('_', 5) - 5) + "."
                  + function.substr(function.find_first_of('_', 5) + 1);
    } else if (function.substr(0, 15) == "side_condition_") {
      size_t ordinal = std::stoll(function.substr(15));
      KOREAxiomDeclaration *axiom = d->Definition->getAxiomByOrdinal(ordinal);
      if (axiom->getAttributes().count("label")) {
        debugName = axiom->getStringAttribute("label") + ".sc";
      }
    }
    std::vector<llvm::Value *> functionArgs;
    functionArgs.push_back(d->stringLiteral(debugName));
    functionArgs.push_back(d->stringLiteral(function));
    functionArgs.push_back(
        addrspaceCast1to0(d->ptrTerm(Call), d->CurrentBlock));
    for (auto arg : args) {
      functionArgs.push_back(
          addrspaceCast1to0(d->ptrTerm(arg), d->CurrentBlock));
    }
    functionArgs.push_back(
        llvm::ConstantPointerNull::get(llvm::Type::getInt8PtrTy(d->Ctx)));

    auto call = llvm::CallInst::Create(
        getOrInsertFunction(
            d->Module, "addMatchFunction",
            llvm::FunctionType::get(
                llvm::Type::getVoidTy(d->Ctx),
                {llvm::Type::getInt8PtrTy(d->Ctx),
                 llvm::Type::getInt8PtrTy(d->Ctx),
                 llvm::Type::getInt8PtrTy(d->Ctx)},
                true)),
        functionArgs, "", d->CurrentBlock);
    setDebugLoc(call);
  }
  child->codegen(d);
  setCompleted();
}

void MakeIteratorNode::codegen(Decision *d) {
  if (beginNode(d, "new_iterator" + name)) {
    return;
  }
  std::vector<llvm::Value *> args;
  std::vector<llvm::Type *> types;
  llvm::Value *arg = d->load(std::make_pair(collection, collectionType));
  args.push_back(arg);
  types.push_back(arg->getType());
  llvm::Type *sretType = getTypeByName(d->Module, "iter");
  llvm::Value *AllocSret = allocateTermNoReloc(sretType, d->CurrentBlock);
  AllocSret->setName(name.substr(0, max_name_length));
  args.insert(args.begin(), AllocSret);
  types.insert(types.begin(), AllocSret->getType());

  llvm::FunctionType *funcType = llvm::FunctionType::get(
      llvm::Type::getVoidTy(d->Module->getContext()), types, false);
  llvm::Function *func = getOrInsertFunction(d->Module, hookName, funcType);
  auto call = llvm::CallInst::Create(func, args, "", d->CurrentBlock);
  setDebugLoc(call);
#if __clang_major__ >= 12
  llvm::Attribute sretAttr
      = llvm::Attribute::get(d->Ctx, llvm::Attribute::StructRet, sretType);
#else
  llvm::Attribute sretAttr
      = llvm::Attribute::get(d->Ctx, llvm::Attribute::StructRet);
#endif
  func->arg_begin()->addAttr(sretAttr);
  call->addParamAttr(0, sretAttr);
  d->store(std::make_pair(name, type), AllocSret);
  child->codegen(d);
  setCompleted();
}

void IterNextNode::codegen(Decision *d) {
  if (beginNode(d, "choice" + binding)) {
    return;
  }
  d->ChoiceBlock = d->CurrentBlock;
  llvm::Value *arg = d->load(std::make_pair(iterator, iteratorType));

  llvm::FunctionType *funcType = llvm::FunctionType::get(
      getValueType({SortCategory::Symbol, 0}, d->Module), {arg->getType()},
      false);
  llvm::Function *func = getOrInsertFunction(d->Module, hookName, funcType);
  auto Call = llvm::CallInst::Create(
      func, {arg}, binding.substr(0, max_name_length), d->CurrentBlock);
  setDebugLoc(Call);
  d->store(std::make_pair(binding, bindingType), Call);
  child->codegen(d);
  d->ChoiceBlock = nullptr;
  setCompleted();
}

void LeafNode::codegen(Decision *d) {
  if (beginNode(d, name)) {
    return;
  }
  if (d->FailPattern) {
    auto call = llvm::CallInst::Create(
        getOrInsertFunction(
            d->Module, "addMatchSuccess", llvm::Type::getVoidTy(d->Ctx)),
        {}, "", d->CurrentBlock);
    setDebugLoc(call);
    llvm::ReturnInst::Create(d->Ctx, d->CurrentBlock);
    setCompleted();
    return;
  }
  std::vector<llvm::Value *> args;
  std::vector<llvm::Type *> types;
  for (auto arg : bindings) {
    auto val = d->load(arg);
    args.push_back(val);
    types.push_back(val->getType());
  }
  auto type = getParamType(d->Cat, d->Module);
  auto Call = llvm::CallInst::Create(
      getOrInsertFunction(
          d->Module, name, llvm::FunctionType::get(type, types, false)),
      args, "", d->CurrentBlock);
  setDebugLoc(Call);
  Call->setCallingConv(llvm::CallingConv::Tail);
  if (child == nullptr) {
    llvm::ReturnInst::Create(d->Ctx, Call, d->CurrentBlock);
  } else {
    auto Call2 = llvm::CallInst::Create(
        getOrInsertFunction(
            d->Module, "addSearchResult",
            llvm::FunctionType::get(
                llvm::Type::getVoidTy(d->Ctx),
                {type,
                 llvm::PointerType::getUnqual(llvm::PointerType::get(type, 1)),
                 llvm::Type::getInt64PtrTy(d->Ctx),
                 llvm::Type::getInt64PtrTy(d->Ctx)},
                false)),
        {Call, d->ResultBuffer, d->ResultCount, d->ResultCapacity}, "",
        d->CurrentBlock);
    setDebugLoc(Call2);
    if (child != FailNode::get()) {
      child->codegen(d);
    } else {
      llvm::BranchInst::Create(d->FailureBlock, d->CurrentBlock);
    }
  }
  setCompleted();
}

llvm::Value *Decision::getTag(llvm::Value *val) {
  auto res = llvm::CallInst::Create(
      getOrInsertFunction(
          Module, "getTag", llvm::Type::getInt32Ty(Ctx),
          getValueType({SortCategory::Symbol, 0}, Module)),
      val, "tag", CurrentBlock);
  setDebugLoc(res);
  return res;
}

llvm::AllocaInst *Decision::decl(var_type name) {
  auto sym = new llvm::AllocaInst(
      name.second, 0, "",
      this->CurrentBlock->getParent()->getEntryBlock().getFirstNonPHI());
  this->symbols[name] = sym;
  return sym;
}

llvm::Value *Decision::load(var_type name) {
  auto sym = this->symbols[name];
  if (!sym) {
    sym = this->decl(name);
  }
  return new llvm::LoadInst(
      sym->getType()->getPointerElementType(), sym,
      name.first.substr(0, max_name_length), this->CurrentBlock);
}

void Decision::store(var_type name, llvm::Value *val) {
  auto sym = this->symbols[name];
  if (!sym) {
    sym = this->decl(name);
  }
  new llvm::StoreInst(val, sym, this->CurrentBlock);
}

llvm::Constant *Decision::stringLiteral(std::string str) {
  auto Str = llvm::ConstantDataArray::getString(Ctx, str, true);
  auto global = Module->getOrInsertGlobal("str_lit_" + str, Str->getType());
  llvm::GlobalVariable *globalVar
      = llvm::dyn_cast<llvm::GlobalVariable>(global);
  if (!globalVar->hasInitializer()) {
    globalVar->setInitializer(Str);
  }
  llvm::Constant *zero = llvm::ConstantInt::get(llvm::Type::getInt64Ty(Ctx), 0);
  auto indices = std::vector<llvm::Constant *>{zero, zero};
  auto Ptr = llvm::ConstantExpr::getInBoundsGetElementPtr(
      Str->getType(), globalVar, indices);
  return Ptr;
}

static void initChoiceBuffer(
    DecisionNode *dt, llvm::Module *module, llvm::BasicBlock *block,
    llvm::BasicBlock *stuck, llvm::BasicBlock *fail,
    llvm::AllocaInst **choiceBufferOut, llvm::AllocaInst **choiceDepthOut,
    llvm::IndirectBrInst **jumpOut) {
  std::unordered_set<LeafNode *> leaves;
  dt->preprocess(leaves);
  auto ty = llvm::ArrayType::get(
      llvm::Type::getInt8PtrTy(module->getContext()), dt->getChoiceDepth() + 1);
  llvm::AllocaInst *choiceBuffer
      = new llvm::AllocaInst(ty, 0, "choiceBuffer", block);
  llvm::AllocaInst *choiceDepth = new llvm::AllocaInst(
      llvm::Type::getInt64Ty(module->getContext()), 0, "choiceDepth", block);
  auto zero
      = llvm::ConstantInt::get(llvm::Type::getInt64Ty(module->getContext()), 0);
  new llvm::StoreInst(zero, choiceDepth, block);
  auto firstElt = llvm::GetElementPtrInst::CreateInBounds(
      ty, choiceBuffer, {zero, zero}, "", block);
  new llvm::StoreInst(
      llvm::BlockAddress::get(block->getParent(), stuck), firstElt, block);

  llvm::LoadInst *currDepth = new llvm::LoadInst(
      choiceDepth->getType()->getPointerElementType(), choiceDepth, "", fail);
  auto currentElt = llvm::GetElementPtrInst::CreateInBounds(
      ty, choiceBuffer, {zero, currDepth}, "", fail);
  llvm::LoadInst *failAddress = new llvm::LoadInst(
      currentElt->getType()->getPointerElementType(), currentElt, "", fail);
  auto newDepth = llvm::BinaryOperator::Create(
      llvm::Instruction::Sub, currDepth,
      llvm::ConstantInt::get(llvm::Type::getInt64Ty(module->getContext()), 1),
      "", fail);
  new llvm::StoreInst(newDepth, choiceDepth, fail);
  llvm::IndirectBrInst *jump
      = llvm::IndirectBrInst::Create(failAddress, 1, fail);
  jump->addDestination(stuck);
  *choiceBufferOut = choiceBuffer;
  *choiceDepthOut = choiceDepth;
  *jumpOut = jump;
}

void makeEvalOrAnywhereFunction(
    KORESymbol *function, KOREDefinition *definition, llvm::Module *module,
    DecisionNode *dt,
    void (*addStuck)(
        llvm::BasicBlock *, llvm::Module *, KORESymbol *, Decision &,
        KOREDefinition *)) {
  auto returnSort = dynamic_cast<KORECompositeSort *>(function->getSort().get())
                        ->getCategory(definition);
  auto returnType = getParamType(returnSort, module);
  std::ostringstream Out;
  function->getSort()->print(Out);
  auto debugReturnType = getDebugType(returnSort, Out.str());
  std::vector<llvm::Type *> args;
  std::vector<llvm::Metadata *> debugArgs;
  std::vector<ValueType> cats;
  for (auto &sort : function->getArguments()) {
    auto cat = dynamic_cast<KORECompositeSort *>(sort.get())
                   ->getCategory(definition);
    std::ostringstream Out;
    sort->print(Out);
    debugArgs.push_back(getDebugType(cat, Out.str()));
    switch (cat.cat) {
    case SortCategory::Map:
    case SortCategory::List:
    case SortCategory::Set:
      args.push_back(llvm::PointerType::get(getValueType(cat, module), 1));
      cats.push_back(cat);
      break;
    default:
      args.push_back(getValueType(cat, module));
      cats.push_back(cat);
      break;
    }
  }
  llvm::FunctionType *funcType
      = llvm::FunctionType::get(returnType, args, false);
  std::ostringstream Out2;
  function->print(Out2, 0, false);
  std::string name = "eval_" + Out2.str();
  llvm::Function *matchFunc = getOrInsertFunction(module, name, funcType);
  matchFunc->setGC("statepoint-example");
  KORESymbolDeclaration *symbolDecl
      = definition->getSymbolDeclarations().at(function->getName());
  initDebugAxiom(symbolDecl->getAttributes());
  initDebugFunction(
      function->getName(), name,
      getDebugFunctionType(debugReturnType, debugArgs), definition, matchFunc);
  matchFunc->setCallingConv(llvm::CallingConv::Tail);
  llvm::BasicBlock *block
      = llvm::BasicBlock::Create(module->getContext(), "entry", matchFunc);
  llvm::BasicBlock *stuck
      = llvm::BasicBlock::Create(module->getContext(), "stuck", matchFunc);
  llvm::BasicBlock *fail
      = llvm::BasicBlock::Create(module->getContext(), "fail", matchFunc);

  llvm::AllocaInst *choiceBuffer, *choiceDepth;
  llvm::IndirectBrInst *jump;
  initChoiceBuffer(
      dt, module, block, stuck, fail, &choiceBuffer, &choiceDepth, &jump);

  int i = 0;
  Decision codegen(
      definition, block, fail, jump, choiceBuffer, choiceDepth, module,
      returnSort, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr);
  for (auto val = matchFunc->arg_begin(); val != matchFunc->arg_end();
       ++val, ++i) {
    val->setName("_" + std::to_string(i + 1));
    codegen.store(std::make_pair(val->getName().str(), val->getType()), val);
    std::ostringstream Out;
    function->getArguments()[i]->print(Out);
    initDebugParam(matchFunc, i, val->getName().str(), cats[i], Out.str());
  }
  addStuck(stuck, module, function, codegen, definition);

  codegen(dt);
}

void abortWhenStuck(
    llvm::BasicBlock *CurrentBlock, llvm::Module *Module, KORESymbol *symbol,
    Decision &codegen, KOREDefinition *d) {
  auto &Ctx = Module->getContext();
  std::ostringstream Out;
  symbol->print(Out);
  symbol = d->getAllSymbols().at(Out.str());
  auto BlockType = getBlockType(Module, d, symbol);
  llvm::Value *Ptr;
  auto BlockPtr
      = llvm::PointerType::get(getTypeByName(Module, BLOCK_STRUCT), 1);
  if (symbol->getArguments().empty()) {
    auto C = getOrInsertFunction(
        Module, "inttoptr_i64.p1s_blocks",
        getValueType({SortCategory::Symbol, 0}, Module),
        llvm::Type::getInt64Ty(Ctx));
    auto F = llvm::cast<llvm::Function>(C);
    F->addFnAttr(llvm::Attribute::AlwaysInline);
    F->addFnAttr("gc-leaf-function");
    Ptr = llvm::CallInst::Create(
        F,
        {llvm::ConstantInt::get(
            llvm::Type::getInt64Ty(Ctx),
            (((uint64_t)symbol->getTag()) << 32) | 1)},
        "", CurrentBlock);
  } else {
    llvm::Value *BlockHeader = getBlockHeader(Module, d, symbol, BlockType);
    llvm::Value *Block
        = allocateTerm({SortCategory::Symbol, 0}, BlockType, CurrentBlock);
    llvm::Value *BlockHeaderPtr = llvm::GetElementPtrInst::CreateInBounds(
        BlockType, Block,
        {llvm::ConstantInt::get(llvm::Type::getInt64Ty(Ctx), 0),
         llvm::ConstantInt::get(llvm::Type::getInt32Ty(Ctx), 0)},
        symbol->getName(), CurrentBlock);
    new llvm::StoreInst(BlockHeader, BlockHeaderPtr, CurrentBlock);
    for (int idx = 0; idx < symbol->getArguments().size(); idx++) {
      auto cat
          = dynamic_cast<KORECompositeSort *>(symbol->getArguments()[idx].get())
                ->getCategory(d);
      auto type = getParamType(cat, Module);
      llvm::Value *ChildValue
          = codegen.load(std::make_pair("_" + std::to_string(idx + 1), type));
      llvm::Value *ChildPtr = llvm::GetElementPtrInst::CreateInBounds(
          BlockType, Block,
          {llvm::ConstantInt::get(llvm::Type::getInt64Ty(Ctx), 0),
           llvm::ConstantInt::get(llvm::Type::getInt32Ty(Ctx), idx + 2)},
          "", CurrentBlock);
      if (ChildValue->getType() == ChildPtr->getType()) {
        ChildValue = new llvm::LoadInst(
            ChildValue->getType()->getPointerElementType(), ChildValue, "",
            CurrentBlock);
      }
      new llvm::StoreInst(ChildValue, ChildPtr, CurrentBlock);
    }
    Ptr = new llvm::BitCastInst(Block, BlockPtr, "", CurrentBlock);
  }
  auto FinishRewriting = getOrInsertFunction(
      Module, "finish_rewriting", llvm::Type::getVoidTy(Ctx), BlockPtr,
      llvm::Type::getInt1Ty(Ctx));
  FinishRewriting->setDoesNotReturn();
  llvm::CallInst::Create(
      FinishRewriting, {Ptr, llvm::ConstantInt::getTrue(Ctx)}, "",
      CurrentBlock);
  new llvm::UnreachableInst(Ctx, CurrentBlock);
}

void makeEvalFunction(
    KORESymbol *function, KOREDefinition *definition, llvm::Module *module,
    DecisionNode *dt) {
  makeEvalOrAnywhereFunction(function, definition, module, dt, abortWhenStuck);
}

void addOwise(
    llvm::BasicBlock *stuck, llvm::Module *module, KORESymbol *symbol,
    Decision &codegen, KOREDefinition *d) {
  llvm::StringMap<llvm::Value *> finalSubst;
  ptr<KORECompositePattern> pat = KORECompositePattern::Create(symbol);
  for (int i = 0; i < symbol->getArguments().size(); i++) {
    auto cat
        = dynamic_cast<KORECompositeSort *>(symbol->getArguments()[i].get())
              ->getCategory(d);
    auto type = getParamType(cat, module);

    std::string name = "_" + std::to_string(i + 1);
    finalSubst[name] = codegen.load(std::make_pair(name, type));

    auto var = KOREVariablePattern::Create(name, symbol->getArguments()[i]);
    pat->addArgument(std::move(var));
  }
  CreateTerm creator = CreateTerm(finalSubst, d, stuck, module, true);
  llvm::Value *retval = creator(pat.get()).first;
  auto returnSort = dynamic_cast<KORECompositeSort *>(symbol->getSort().get())
                        ->getCategory(d);
  auto returnType = getValueType(returnSort, module);
  switch (returnSort.cat) {
  case SortCategory::Map:
  case SortCategory::List:
  case SortCategory::Set:
    if (retval->getType() == returnType) {
      auto tempAlloc = allocateTerm(
          returnSort, retval->getType(), creator.getCurrentBlock(),
          "koreAllocAlwaysGC");
      new llvm::StoreInst(retval, tempAlloc, creator.getCurrentBlock());
      retval = tempAlloc;
    }
    break;
  default: break;
  }
  llvm::ReturnInst::Create(
      module->getContext(), retval, creator.getCurrentBlock());
}

void makeAnywhereFunction(
    KORESymbol *function, KOREDefinition *definition, llvm::Module *module,
    DecisionNode *dt) {
  makeEvalOrAnywhereFunction(function, definition, module, dt, addOwise);
}

llvm::BasicBlock *stepFunctionHeader(
    unsigned ordinal, llvm::Module *module, KOREDefinition *definition,
    llvm::BasicBlock *block, llvm::BasicBlock *stuck,
    std::vector<llvm::Value *> args, std::vector<ValueType> types) {
  auto finished = getOrInsertFunction(
      module, "finished_rewriting",
      llvm::FunctionType::get(
          llvm::Type::getInt1Ty(module->getContext()), {}, false));
  auto isFinished = llvm::CallInst::Create(finished, {}, "", block);
  auto merge = llvm::BasicBlock::Create(
      module->getContext(), "step", block->getParent());
  llvm::BranchInst::Create(stuck, merge, isFinished, block);

  auto koreCollect = getOrInsertFunction(
      module, "tryKoreCollect",
      llvm::FunctionType::get(
          llvm::Type::getVoidTy(module->getContext()),
          {llvm::Type::getInt1Ty(module->getContext())}, false));
  auto call = llvm::CallInst::Create(
      koreCollect, {llvm::ConstantInt::getTrue(module->getContext())}, "",
      merge);
  setDebugLoc(call);
  return merge;
}

void makeStepFunction(
    KOREDefinition *definition, llvm::Module *module, DecisionNode *dt,
    bool search) {
  auto blockType = getValueType({SortCategory::Symbol, 0}, module);
  auto bufType = llvm::PointerType::get(blockType, 1);
  auto debugType
      = getDebugType({SortCategory::Symbol, 0}, "SortGeneratedTopCell{}");
  llvm::FunctionType *funcType;
  std::string name;
  if (search) {
    name = "stepAll";
    funcType = llvm::FunctionType::get(
        bufType, {blockType, llvm::Type::getInt64PtrTy(module->getContext())},
        false);
  } else {
    name = "step";
    funcType = llvm::FunctionType::get(blockType, {blockType}, false);
  }
  llvm::Function *matchFunc = getOrInsertFunction(module, name, funcType);
  matchFunc->setGC("statepoint-example");
  resetDebugLoc();
  if (search) {
    initDebugFunction(
        name, name,
        getDebugFunctionType(
            getPointerDebugType(debugType, "block **"),
            {debugType, getPointerDebugType(getLongDebugType(), "uint64_t *")}),
        definition, matchFunc);
  } else {
    initDebugFunction(
        name, name, getDebugFunctionType(debugType, {debugType}), definition,
        matchFunc);
  }
  matchFunc->setCallingConv(llvm::CallingConv::Tail);
  auto val = matchFunc->arg_begin();
  llvm::BasicBlock *block
      = llvm::BasicBlock::Create(module->getContext(), "entry", matchFunc);
  llvm::BasicBlock *stuck
      = llvm::BasicBlock::Create(module->getContext(), "stuck", matchFunc);
  llvm::BasicBlock *pre_stuck
      = llvm::BasicBlock::Create(module->getContext(), "pre_stuck", matchFunc);
  llvm::BasicBlock *fail
      = llvm::BasicBlock::Create(module->getContext(), "fail", matchFunc);

  llvm::AllocaInst *choiceBuffer, *choiceDepth;
  llvm::IndirectBrInst *jump;

  llvm::Value *resultBuffer = nullptr, *resultCount = nullptr,
              *resultCapacity = nullptr;
  if (search) {
    resultBuffer = new llvm::AllocaInst(bufType, 0, "resultBuffer", block);
    resultCount = matchFunc->arg_begin() + 1;
    resultCapacity = new llvm::AllocaInst(
        llvm::Type::getInt64Ty(module->getContext()), 0, "resultCapacity",
        block);
    llvm::Value *initialBuffer = allocateTerm(
        {SortCategory::Symbol, 0}, blockType, block, "koreAllocAlwaysGC");
    new llvm::StoreInst(initialBuffer, resultBuffer, block);
    new llvm::StoreInst(
        llvm::ConstantInt::get(llvm::Type::getInt64Ty(module->getContext()), 0),
        resultCount, block);
    new llvm::StoreInst(
        llvm::ConstantInt::get(llvm::Type::getInt64Ty(module->getContext()), 1),
        resultCapacity, block);
  }

  initChoiceBuffer(
      dt, module, block, pre_stuck, fail, &choiceBuffer, &choiceDepth, &jump);

  initDebugParam(
      matchFunc, 0, "subject", {SortCategory::Symbol, 0},
      "SortGeneratedTopCell{}");
  llvm::BranchInst::Create(stuck, pre_stuck);
  auto result = stepFunctionHeader(
      0, module, definition, block, stuck, {val}, {{SortCategory::Symbol, 0}});
  val->setName("_1");
  Decision codegen(
      definition, result, fail, jump, choiceBuffer, choiceDepth, module,
      {SortCategory::Symbol, 0}, nullptr, nullptr, nullptr, resultBuffer,
      resultCount, resultCapacity);
  codegen.store(std::make_pair(val->getName().str(), val->getType()), val);
  if (search) {
    auto result = new llvm::LoadInst(bufType, resultBuffer, "", stuck);
    llvm::ReturnInst::Create(module->getContext(), result, stuck);
  } else {
    auto phi = llvm::PHINode::Create(val->getType(), 2, "phi_1", stuck);
    phi->addIncoming(val, block);
    phi->addIncoming(val, pre_stuck);
    llvm::ReturnInst::Create(module->getContext(), phi, stuck);
  }

  codegen(dt);
}

void makeMatchReasonFunction(
    KOREDefinition *definition, llvm::Module *module,
    KOREAxiomDeclaration *axiom, DecisionNode *dt) {
  auto blockType = getValueType({SortCategory::Symbol, 0}, module);
  llvm::FunctionType *funcType = llvm::FunctionType::get(
      llvm::Type::getVoidTy(module->getContext()), {blockType}, false);
  std::string name = "match_" + std::to_string(axiom->getOrdinal());
  llvm::Function *matchFunc = getOrInsertFunction(module, name, funcType);
  matchFunc->setGC("statepoint-example");
  std::string debugName = name;
  if (axiom->getAttributes().count("label")) {
    debugName = axiom->getStringAttribute("label") + ".match";
  }
  auto debugType
      = getDebugType({SortCategory::Symbol, 0}, "SortGeneratedTopCell{}");
  resetDebugLoc();
  initDebugFunction(
      debugName, debugName,
      getDebugFunctionType(getVoidDebugType(), {debugType}), definition,
      matchFunc);
  matchFunc->setCallingConv(llvm::CallingConv::Tail);
  auto val = matchFunc->arg_begin();
  llvm::BasicBlock *block
      = llvm::BasicBlock::Create(module->getContext(), "entry", matchFunc);
  llvm::BasicBlock *stuck
      = llvm::BasicBlock::Create(module->getContext(), "stuck", matchFunc);
  llvm::BasicBlock *pre_stuck
      = llvm::BasicBlock::Create(module->getContext(), "pre_stuck", matchFunc);
  llvm::BasicBlock *fail
      = llvm::BasicBlock::Create(module->getContext(), "fail", matchFunc);
  llvm::PHINode *FailSubject = llvm::PHINode::Create(
      llvm::Type::getInt8PtrTy(module->getContext(), 1), 0, "subject", fail);
  llvm::PHINode *FailPattern = llvm::PHINode::Create(
      llvm::Type::getInt8PtrTy(module->getContext()), 0, "pattern", fail);
  llvm::PHINode *FailSort = llvm::PHINode::Create(
      llvm::Type::getInt8PtrTy(module->getContext()), 0, "sort", fail);
  auto call = llvm::CallInst::Create(
      getOrInsertFunction(
          module, "addMatchFailReason",
          llvm::FunctionType::get(
              llvm::Type::getVoidTy(module->getContext()),
              {llvm::Type::getInt8PtrTy(module->getContext()),
               FailPattern->getType(), FailSort->getType()},
              false)),
      {addrspaceCast1to0(FailSubject, fail), FailPattern, FailSort}, "", fail);
  setDebugLoc(call);

  llvm::AllocaInst *choiceBuffer, *choiceDepth;
  llvm::IndirectBrInst *jump;
  initChoiceBuffer(
      dt, module, block, pre_stuck, fail, &choiceBuffer, &choiceDepth, &jump);

  initDebugParam(
      matchFunc, 0, "subject", {SortCategory::Symbol, 0},
      "SortGeneratedTopCell{}");
  llvm::BranchInst::Create(stuck, pre_stuck);
  val->setName("_1");
  Decision codegen(
      definition, block, fail, jump, choiceBuffer, choiceDepth, module,
      {SortCategory::Symbol, 0}, FailSubject, FailPattern, FailSort, nullptr,
      nullptr, nullptr);
  codegen.store(std::make_pair(val->getName().str(), val->getType()), val);
  llvm::ReturnInst::Create(module->getContext(), stuck);

  codegen(dt);
}

// TODO: actually collect the return value of this function. Right now it
// assumes that it will never be collected and constructs a unique_ptr pointing
// to the return value of the recursive call. This is not really safe if the
// object actually gets collected because it's possible for the return value of
// the recursive call to actually be owned by the axiom that it is a part of.
// However, since we then immediately call release on the pointer and never free
// it, it should be safe, it will just leak memory. But we don't really care
// that much about memory leaks in the compiler right now, so it's probably
// fine.
KOREPattern *makePartialTerm(
    KOREPattern *term, std::set<std::string> occurrences,
    std::string occurrence) {
  if (occurrences.count(occurrence)) {
    return KOREVariablePattern::Create(occurrence, term->getSort()).release();
  }
  if (auto pat = dynamic_cast<KORECompositePattern *>(term)) {
    if (pat->getConstructor()->getName() == "\\dv") {
      return term;
    }
    ptr<KORECompositePattern> result
        = KORECompositePattern::Create(pat->getConstructor());
    for (unsigned i = 0; i < pat->getArguments().size(); i++) {
      result->addArgument(ptr<KOREPattern>(makePartialTerm(
          dynamic_cast<KOREPattern *>(pat->getArguments()[i].get()),
          occurrences, "_" + std::to_string(i) + occurrence)));
    }
    return result.release();
  }
  abort();
}

void makeStepFunction(
    KOREAxiomDeclaration *axiom, KOREDefinition *definition,
    llvm::Module *module, PartialStep res) {
  auto blockType = getValueType({SortCategory::Symbol, 0}, module);
  std::vector<llvm::Type *> argTypes;
  std::vector<llvm::Metadata *> debugTypes;
  for (auto res : res.residuals) {
    auto argSort
        = dynamic_cast<KORECompositeSort *>(res.pattern->getSort().get());
    auto cat = argSort->getCategory(definition);
    std::ostringstream Out;
    argSort->print(Out);
    debugTypes.push_back(getDebugType(cat, Out.str()));
    switch (cat.cat) {
    case SortCategory::Map:
    case SortCategory::List:
    case SortCategory::Set:
      argTypes.push_back(llvm::PointerType::get(getValueType(cat, module), 1));
      break;
    default: argTypes.push_back(getValueType(cat, module)); break;
    }
  }
  auto blockDebugType
      = getDebugType({SortCategory::Symbol, 0}, "SortGeneratedTopCell{}");
  llvm::FunctionType *funcType
      = llvm::FunctionType::get(blockType, argTypes, false);
  std::string name = "step_" + std::to_string(axiom->getOrdinal());
  llvm::Function *matchFunc = getOrInsertFunction(module, name, funcType);
  matchFunc->setGC("statepoint-example");
  resetDebugLoc();
  initDebugFunction(
      name, name, getDebugFunctionType(blockDebugType, debugTypes), definition,
      matchFunc);
  matchFunc->setCallingConv(llvm::CallingConv::Tail);

  llvm::StringMap<llvm::Value *> stuckSubst;
  llvm::BasicBlock *block
      = llvm::BasicBlock::Create(module->getContext(), "entry", matchFunc);
  llvm::BasicBlock *stuck
      = llvm::BasicBlock::Create(module->getContext(), "stuck", matchFunc);
  llvm::BasicBlock *pre_stuck
      = llvm::BasicBlock::Create(module->getContext(), "pre_stuck", matchFunc);
  llvm::BasicBlock *fail
      = llvm::BasicBlock::Create(module->getContext(), "fail", matchFunc);

  llvm::AllocaInst *choiceBuffer, *choiceDepth;
  llvm::IndirectBrInst *jump;
  initChoiceBuffer(
      res.dt, module, block, pre_stuck, fail, &choiceBuffer, &choiceDepth,
      &jump);

  llvm::BranchInst::Create(stuck, pre_stuck);
  std::vector<llvm::PHINode *> phis;
  int i = 0;
  std::vector<llvm::Value *> args;
  std::vector<ValueType> types;
  for (auto val = matchFunc->arg_begin(); val != matchFunc->arg_end();
       ++val, ++i) {
    args.push_back(val);
    auto phi = llvm::PHINode::Create(
        val->getType(), 2, "phi" + res.residuals[i].occurrence, stuck);
    phi->addIncoming(val, block);
    phis.push_back(phi);
    auto sort = res.residuals[i].pattern->getSort();
    auto cat = dynamic_cast<KORECompositeSort *>(sort.get())
                   ->getCategory(definition);
    types.push_back(cat);
    std::ostringstream Out;
    sort->print(Out);
    initDebugParam(matchFunc, i, "_" + std::to_string(i + 1), cat, Out.str());
  }
  auto header = stepFunctionHeader(
      axiom->getOrdinal(), module, definition, block, stuck, args, types);
  i = 0;
  Decision codegen(
      definition, header, fail, jump, choiceBuffer, choiceDepth, module,
      {SortCategory::Symbol, 0}, nullptr, nullptr, nullptr, nullptr, nullptr,
      nullptr);
  for (auto val : args) {
    val->setName(res.residuals[i].occurrence.substr(0, max_name_length));
    codegen.store(std::make_pair(val->getName().str(), val->getType()), val);
    stuckSubst.insert({val->getName(), phis[i]});
    phis[i++]->addIncoming(val, pre_stuck);
  }
  std::set<std::string> occurrences;
  for (auto residual : res.residuals) {
    occurrences.insert(residual.occurrence);
  }
  KOREPattern *partialTerm = makePartialTerm(
      dynamic_cast<KOREPattern *>(axiom->getRightHandSide()), occurrences,
      "_1");
  CreateTerm creator(stuckSubst, definition, stuck, module, false);
  llvm::Value *retval = creator(partialTerm).first;
  llvm::ReturnInst::Create(
      module->getContext(), retval, creator.getCurrentBlock());

  codegen(res.dt);
}
} // namespace kllvm
