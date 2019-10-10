#include "kllvm/codegen/Decision.h"
#include "kllvm/codegen/CreateTerm.h"
#include "kllvm/codegen/Debug.h"

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

static unsigned max_name_length = 1024 - std::to_string(std::numeric_limits<unsigned long long>::max()).length();

void Decision::operator()(DecisionNode *entry, llvm::StringMap<llvm::Value *> substitution) {
  if (entry == FailNode::get()) {
    llvm::BranchInst::Create(this->FailureBlock, this->CurrentBlock);
  } else {
    entry->codegen(this, substitution);
  }
  for (auto &entry : failPhis) {
    for (auto &entry2 : entry.second) {
      llvm::PHINode *Phi = entry2.second;
      std::vector<llvm::BasicBlock *> undefBlocks;
      for (llvm::BasicBlock *block : llvm::predecessors(Phi->getParent())) {
        if (Phi->getBasicBlockIndex(block) == -1) {
          undefBlocks.push_back(block);
        }
      }
      for (llvm::BasicBlock *block : undefBlocks) {
        Phi->addIncoming(llvm::UndefValue::get(Phi->getType()), block);
      }
    }
  }
  for (auto block = this->CurrentBlock->getParent()->begin(); block != this->CurrentBlock->getParent()->end(); ++block) {
    if (block->getUniquePredecessor()) {
      for (auto phi = block->phis().begin(); phi != block->phis().end(); phi = block->phis().begin()) {
        phi->replaceAllUsesWith(phi->getIncomingValue(0));
        phi->removeFromParent();
      }
    }
  }
}

void DecisionNode::sharedNode(Decision *d, llvm::StringMap<llvm::Value *> &oldSubst, llvm::StringMap<llvm::Value *> &substitution, llvm::BasicBlock *Block) {
  collectVars();
  std::set<std::string> vars = this->vars;
  collectFail();
  if (containsFailNode) {
    vars.insert(d->ChoiceVars.begin(), d->ChoiceVars.end());
  }
  for (std::string var : vars) {
    auto Phi = phis[var][oldSubst[var]->getType()];
    if (!Phi) {
      Phi = llvm::PHINode::Create(oldSubst[var]->getType(), 1, "aux_phi" + var.substr(0, max_name_length), cachedCode->getFirstNonPHI());
      for (llvm::BasicBlock *pred : predecessors) {
        Phi->addIncoming(llvm::UndefValue::get(Phi->getType()), pred);
      }
      phis[var][oldSubst[var]->getType()] = Phi;
    }
    Phi->addIncoming(oldSubst[var], Block);
    substitution[var] = Phi;
  }
  predecessors.push_back(Block);
  for (auto &entry : phis) {
    std::string var = entry.first();
    for (auto &entry2 : entry.second) {
      auto Phi = entry2.second;
      if (!vars.count(var) || oldSubst[var]->getType() != entry2.first) {
        Phi->addIncoming(llvm::UndefValue::get(Phi->getType()), Block);
      }
    }
  }
}
 

bool DecisionNode::beginNode(Decision *d, std::string name, llvm::StringMap<llvm::Value *> &substitution) {
  if (isCompleted()) {
    llvm::BranchInst::Create(cachedCode, d->CurrentBlock);
    sharedNode(d, substitution, substitution, d->CurrentBlock);
    return true;
  }
  collectVars();
  std::set<std::string> vars = this->vars;
  collectFail();
  if (containsFailNode) {
    vars.insert(d->ChoiceVars.begin(), d->ChoiceVars.end());
  }
  auto Block = llvm::BasicBlock::Create(d->Ctx,
      name.substr(0, max_name_length),
      d->CurrentBlock->getParent());
  cachedCode = Block;
  llvm::BranchInst::Create(Block, d->CurrentBlock);
  for (std::string var : vars) {
    auto val = substitution.lookup(var);
    if (!val) {
      std::cerr << var << std::endl;
      for (auto key : substitution.keys()) {
        std::cerr << key.str() << std::endl;
      }
      abort();
    }
    auto Phi = llvm::PHINode::Create(substitution[var]->getType(), 1, "phi" + var.substr(0, max_name_length), Block);
    Phi->addIncoming(substitution[var], d->CurrentBlock);
    phis[var][substitution[var]->getType()] = Phi;
    substitution[var] = Phi;
  }
  predecessors.push_back(d->CurrentBlock);
  d->CurrentBlock = Block;
  return false;
}

void Decision::addFailPhiIncoming(llvm::StringMap<llvm::Value *> oldSubst, llvm::BasicBlock *switchBlock) {
  if (ChoiceNode != nullptr) {
    for (std::string var : ChoiceVars) {
      failPhis[var][oldSubst[var]->getType()]->addIncoming(oldSubst[var], switchBlock);
    }
  }
}

void SwitchNode::codegen(Decision *d, llvm::StringMap<llvm::Value *> substitution) {
  if (beginNode(d, "switch" + name, substitution)) {
    return;
  }
  llvm::Value *val = substitution.lookup(name);
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
      CaseBlock = llvm::BasicBlock::Create(d->Ctx, 
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
    auto cast = new llvm::PtrToIntInst(val, llvm::Type::getInt64Ty(d->Ctx), "", d->CurrentBlock);
    auto cmp = new llvm::ICmpInst(*d->CurrentBlock, llvm::CmpInst::ICMP_NE, cast, llvm::ConstantExpr::getPtrToInt(llvm::ConstantPointerNull::get(llvm::dyn_cast<llvm::PointerType>(val->getType())), llvm::Type::getInt64Ty(d->Ctx)));
    val = cmp;
    isInt = true;
  }
  if (d->ChoiceBlock) {
    new llvm::StoreInst(llvm::BlockAddress::get(d->CurrentBlock->getParent(), d->ChoiceBlock), d->FailAddress, d->CurrentBlock);
    d->FailJump->addDestination(d->ChoiceBlock);
    d->ChoiceBlock = nullptr;
  }
  if (isInt) {
    auto _switch = llvm::SwitchInst::Create(val, _default, cases.size(), d->CurrentBlock);
    for (auto &_case : caseData) {
      _switch->addCase(llvm::ConstantInt::get(d->Ctx, _case.second->getLiteral()), _case.first);
    }
  } else { 
    if (caseData.size() == 0) {
      llvm::BranchInst::Create(_default, d->CurrentBlock);
    } else {
      llvm::Value *tagVal = d->getTag(val);
      auto _switch = llvm::SwitchInst::Create(tagVal, _default, caseData.size(), d->CurrentBlock);
      for (auto &_case : caseData) {
        _switch->addCase(llvm::ConstantInt::get(llvm::Type::getInt32Ty(d->Ctx), _case.second->getConstructor()->getTag()), _case.first); 
      }
    }
  }
  auto switchBlock = d->CurrentBlock;
  auto oldSubst = substitution;
  for (auto &entry : caseData) {
    auto &_case = *entry.second;
    if (entry.first == d->FailureBlock) {
      d->addFailPhiIncoming(oldSubst, switchBlock);
      continue;
    }
    d->CurrentBlock = entry.first;
    if (!isInt) {
      int offset = 0;
      llvm::StructType *BlockType = getBlockType(d->Module, d->Definition, _case.getConstructor());
      llvm::BitCastInst *Cast = new llvm::BitCastInst(substitution.lookup(name), llvm::PointerType::getUnqual(BlockType), "", d->CurrentBlock);
      KORESymbolDeclaration *symbolDecl = d->Definition->getSymbolDeclarations().at(_case.getConstructor()->getName());
      llvm::Instruction *Renamed;
      for (std::string binding : _case.getBindings()) {
        llvm::Value *ChildPtr = llvm::GetElementPtrInst::CreateInBounds(BlockType, Cast, {llvm::ConstantInt::get(llvm::Type::getInt64Ty(d->Ctx), 0), llvm::ConstantInt::get(llvm::Type::getInt32Ty(d->Ctx), offset+2)}, "", d->CurrentBlock);
        llvm::Value *Child;
        switch (dynamic_cast<KORECompositeSort *>(_case.getConstructor()->getArguments()[offset].get())->getCategory(d->Definition).cat) {
        case SortCategory::Map:
        case SortCategory::List:
        case SortCategory::Set:
          Child = ChildPtr;
          break;
        default:
          Child = new llvm::LoadInst(ChildPtr, binding.substr(0, max_name_length), d->CurrentBlock);
          break;
        }
        auto BlockPtr = llvm::PointerType::getUnqual(d->Module->getTypeByName(BLOCK_STRUCT));
        if (symbolDecl->getAttributes().count("binder")) {
          if (offset == 0) {
            Renamed = llvm::CallInst::Create(d->Module->getOrInsertFunction("alphaRename", BlockPtr, BlockPtr), Child, "renamedVar", d->CurrentBlock);
            setDebugLoc(Renamed);
            substitution[binding] = Renamed;
          } else if (offset == _case.getBindings().size() - 1) {
            llvm::Instruction *Replaced = llvm::CallInst::Create(d->Module->getOrInsertFunction("replaceBinderIndex", BlockPtr, BlockPtr, BlockPtr), {Child, Renamed}, "withUnboundIndex", d->CurrentBlock);
            setDebugLoc(Replaced);
            substitution[binding] = Replaced;
          } else {
            substitution[binding] = Child;
          }
        } else {
          substitution[binding] = Child;
        }
        offset++;
      }
    }
    _case.getChild()->codegen(d, substitution);
  }
  if (defaultCase) {
    if (_default != d->FailureBlock) {
      // process default also
      d->CurrentBlock = _default;
      defaultCase->getChild()->codegen(d, substitution);
    } else {
      d->addFailPhiIncoming(oldSubst, switchBlock);
    }
  } else {
    d->addFailPhiIncoming(oldSubst, switchBlock);
  }

  setCompleted();
}

void MakePatternNode::codegen(Decision *d, llvm::StringMap<llvm::Value *> substitution) {
  if (beginNode(d, "pattern" + name, substitution)) {
    return;
  }
  CreateTerm creator(substitution, d->Definition, d->CurrentBlock, d->Module, false);
  llvm::Value *val = creator(pattern).first;
  d->CurrentBlock = creator.getCurrentBlock();
  substitution[name] = val;
  child->codegen(d, substitution);
  setCompleted();
}

void FunctionNode::codegen(Decision *d, llvm::StringMap<llvm::Value *> substitution) {
  if (beginNode(d, "function" + name, substitution)) {
    return;
  }
  std::vector<llvm::Value *> args;
  for (auto arg : bindings) {
    llvm::Value *val;
    if (arg.find_first_not_of("-0123456789") == std::string::npos) {
      val = llvm::ConstantInt::get(llvm::Type::getInt64Ty(d->Ctx), std::stoi(arg));
    } else {
      val = substitution.lookup(arg);
    }
    args.push_back(val);
  }
  CreateTerm creator(substitution, d->Definition, d->CurrentBlock, d->Module, false);
  auto Call = creator.createFunctionCall(function, cat, args, function.substr(0, 5) == "hook_", false);
  Call->setName(name.substr(0, max_name_length));
  substitution[name] = Call;
  child->codegen(d, substitution);
  setCompleted();
}

void MakeIteratorNode::codegen(Decision *d, llvm::StringMap<llvm::Value *> substitution) {
  if (beginNode(d, "new_iterator" + name, substitution)) {
    return;
  }
  std::vector<llvm::Value *> args;
  std::vector<llvm::Type *> types;
  llvm::Value *arg = substitution.lookup(collection);
  args.push_back(arg);
  types.push_back(arg->getType());
  llvm::Value *AllocSret = allocateTerm(d->Module->getTypeByName("iter"), d->CurrentBlock, "koreAllocAlwaysGC");
  AllocSret->setName(name.substr(0, max_name_length));
  args.insert(args.begin(), AllocSret);
  types.insert(types.begin(), AllocSret->getType());

  llvm::FunctionType *funcType = llvm::FunctionType::get(llvm::Type::getVoidTy(d->Module->getContext()), types, false);
  llvm::Constant *constant = d->Module->getOrInsertFunction(hookName, funcType);
  llvm::Function *func = llvm::dyn_cast<llvm::Function>(constant);
  if (!func) {
    constant->print(llvm::errs());
    abort();
  }
  auto call = llvm::CallInst::Create(func, args, "", d->CurrentBlock);
  setDebugLoc(call);
  func->arg_begin()->addAttr(llvm::Attribute::StructRet);
  call->addParamAttr(0, llvm::Attribute::StructRet);
  substitution[name] = AllocSret;
  child->codegen(d, substitution);
  setCompleted();
}

void IterNextNode::codegen(Decision *d, llvm::StringMap<llvm::Value *> substitution) {
  collectVars();
  auto oldVars = d->ChoiceVars;
  d->ChoiceVars = vars;
  if (beginNode(d, "choice" + binding, substitution)) {
    return;
  }
  d->ChoiceBlock = d->CurrentBlock;
  auto oldNode = d->ChoiceNode;
  d->ChoiceNode = this;
  llvm::Value *arg = substitution.lookup(iterator);

  collectFail();
  if (containsFailNode) {
    for (std::string var : d->ChoiceVars) {
      auto Phi = phis[var][substitution[var]->getType()];
      if (!Phi) {
        for (std::string v : vars) {
          std::cerr << v << std::endl;
        }
        abort();
      }
      auto failPhi = d->failPhis[var][Phi->getType()];
      if (!failPhi) {
        failPhi = llvm::PHINode::Create(Phi->getType(), 0, "phi" + var.substr(0, max_name_length), d->FailureBlock->getFirstNonPHI());
        d->failPhis[var][Phi->getType()] = failPhi;
      }
      Phi->addIncoming(failPhi, d->FailureBlock);
    }
  }

  llvm::FunctionType *funcType = llvm::FunctionType::get(getValueType({SortCategory::Symbol, 0}, d->Module), {arg->getType()}, false);
  llvm::Constant *constant = d->Module->getOrInsertFunction(hookName, funcType);
  llvm::Function *func = llvm::dyn_cast<llvm::Function>(constant);
  if (!func) {
    constant->print(llvm::errs());
    abort();
  }
  auto Call = llvm::CallInst::Create(func, {arg}, binding.substr(0, max_name_length), d->CurrentBlock);
  setDebugLoc(Call);
  substitution[binding] = Call;
  child->codegen(d, substitution);
  d->ChoiceBlock = nullptr;
  d->ChoiceNode = oldNode;
  d->ChoiceVars = oldVars;
  setCompleted();
}

void LeafNode::codegen(Decision *d, llvm::StringMap<llvm::Value *> substitution) {
  if (beginNode(d, name, substitution)) {
    return;
  }
  std::vector<llvm::Value *> args;
  std::vector<llvm::Type *> types;
  for (auto arg : bindings) {
    auto val = substitution.lookup(arg);
    args.push_back(val);
    types.push_back(val->getType());
  }
  auto type = getValueType(d->Cat, d->Module);
  switch(d->Cat.cat) {
  case SortCategory::Map:
  case SortCategory::List:
  case SortCategory::Set:
    type = llvm::PointerType::getUnqual(type);
    break;
  default:
    break;
  }
  auto Call = llvm::CallInst::Create(d->Module->getOrInsertFunction(name, llvm::FunctionType::get(type, types, false)), args, "", d->CurrentBlock);
  setDebugLoc(Call);
  Call->setCallingConv(llvm::CallingConv::Fast);
  llvm::ReturnInst::Create(d->Ctx, Call, d->CurrentBlock);
  setCompleted();
}

llvm::Value *Decision::getTag(llvm::Value *val) {
  auto res = llvm::CallInst::Create(Module->getOrInsertFunction("getTag", llvm::Type::getInt32Ty(Ctx), getValueType({SortCategory::Symbol, 0}, Module)), val, "tag", CurrentBlock);
  setDebugLoc(res);
  return res;
}

void makeEvalOrAnywhereFunction(KORESymbol *function, KOREDefinition *definition, llvm::Module *module, DecisionNode *dt, void (*addStuck)(llvm::BasicBlock*, llvm::Module*, KORESymbol *, llvm::StringMap<llvm::Value *>&, KOREDefinition *)) {
  auto returnSort = dynamic_cast<KORECompositeSort *>(function->getSort().get())->getCategory(definition);
  auto returnType = getValueType(returnSort, module);
  switch(returnSort.cat) {
  case SortCategory::Map:
  case SortCategory::List:
  case SortCategory::Set:
    returnType = llvm::PointerType::getUnqual(returnType);
    break;
  default:
    break;
  }
  auto debugReturnType = getDebugType(returnSort);
  std::vector<llvm::Type *> args;
  std::vector<llvm::Metadata *> debugArgs;
  std::vector<ValueType> cats;
  for (auto &sort : function->getArguments()) {
    auto cat = dynamic_cast<KORECompositeSort *>(sort.get())->getCategory(definition);
    debugArgs.push_back(getDebugType(cat));
    switch (cat.cat) {
    case SortCategory::Map:
    case SortCategory::List:
    case SortCategory::Set:
      args.push_back(llvm::PointerType::getUnqual(getValueType(cat, module)));
      cats.push_back(cat);
      break;
    default:
      args.push_back(getValueType(cat, module));
      cats.push_back(cat);
      break;
    }
  }
  llvm::FunctionType *funcType = llvm::FunctionType::get(returnType, args, false);
  std::ostringstream Out;
  function->print(Out, 0, false);
  std::string name = "eval_" + Out.str();
  llvm::Constant *func = module->getOrInsertFunction(name, funcType);
  llvm::Function *matchFunc = llvm::cast<llvm::Function>(func);
  KORESymbolDeclaration *symbolDecl = definition->getSymbolDeclarations().at(function->getName());
  initDebugAxiom(symbolDecl->getAttributes());
  initDebugFunction(function->getName(), name, getDebugFunctionType(debugReturnType, debugArgs), definition, matchFunc);
  matchFunc->setCallingConv(llvm::CallingConv::Fast);
  llvm::StringMap<llvm::Value *> subst;
  llvm::BasicBlock *block = llvm::BasicBlock::Create(module->getContext(), "entry", matchFunc);
  llvm::AllocaInst *addr = new llvm::AllocaInst(llvm::Type::getInt8PtrTy(module->getContext()), 0, "jumpTo", block);
  llvm::BasicBlock *stuck = llvm::BasicBlock::Create(module->getContext(), "stuck", matchFunc);
  new llvm::StoreInst(llvm::BlockAddress::get(matchFunc, stuck), addr, block);

  int i = 0;
  for (auto val = matchFunc->arg_begin(); val != matchFunc->arg_end(); ++val, ++i) {
    val->setName("_" + std::to_string(i+1));
    subst.insert({val->getName(), val});
    initDebugParam(matchFunc, i, val->getName(), cats[i]);
  }
  addStuck(stuck, module, function, subst, definition);

  llvm::BasicBlock *fail = llvm::BasicBlock::Create(module->getContext(), "fail", matchFunc);
  llvm::LoadInst *load = new llvm::LoadInst(addr, "", fail);
  llvm::IndirectBrInst *jump = llvm::IndirectBrInst::Create(load, 1, fail);
  jump->addDestination(stuck);

  Decision codegen(definition, block, fail, jump, addr, module, returnSort);
  codegen(dt, subst);
}

void abortWhenStuck(llvm::BasicBlock *stuck, llvm::Module *module, KORESymbol *, llvm::StringMap<llvm::Value *> &, KOREDefinition *) {
  addAbort(stuck, module);
}

void makeEvalFunction(KORESymbol *function, KOREDefinition *definition, llvm::Module *module, DecisionNode *dt) {
  makeEvalOrAnywhereFunction(function, definition, module, dt, abortWhenStuck);
}

void addOwise(llvm::BasicBlock *stuck, llvm::Module *module, KORESymbol *symbol, llvm::StringMap<llvm::Value *> &subst, KOREDefinition *d) {
  CreateTerm creator = CreateTerm(subst, d, stuck, module, true);
  ptr<KORECompositePattern> pat = KORECompositePattern::Create(symbol);
  for (int i = 0; i < symbol->getArguments().size(); i++) {
     auto var = KOREVariablePattern::Create("_" + std::to_string(i+1), symbol->getArguments()[i]);
     pat->addArgument(std::move(var));
  }
  llvm::Value *retval = creator(pat.get()).first;
  auto returnSort = dynamic_cast<KORECompositeSort *>(symbol->getSort().get())->getCategory(d);
  auto returnType = getValueType(returnSort, module);
  switch(returnSort.cat) {
  case SortCategory::Map:
  case SortCategory::List:
  case SortCategory::Set:
    if (retval->getType() == returnType) {
      auto tempAlloc = allocateTerm(retval->getType(), creator.getCurrentBlock(), "koreAllocAlwaysGC");
      new llvm::StoreInst(retval, tempAlloc, creator.getCurrentBlock());
      retval = tempAlloc;
    }
    break;
  default:
    break;
  }
  llvm::ReturnInst::Create(module->getContext(), retval, creator.getCurrentBlock());
}

void makeAnywhereFunction(KORESymbol *function, KOREDefinition *definition, llvm::Module *module, DecisionNode *dt) {
  makeEvalOrAnywhereFunction(function, definition, module, dt, addOwise);
}

std::pair<std::vector<llvm::Value *>, llvm::BasicBlock *> stepFunctionHeader(unsigned ordinal, llvm::Module *module, KOREDefinition *definition, llvm::BasicBlock *block, llvm::BasicBlock *stuck, std::vector<llvm::Value *> args, std::vector<ValueType> types) {
  auto finished = module->getOrInsertFunction("finished_rewriting", llvm::FunctionType::get(llvm::Type::getInt1Ty(module->getContext()), {}, false));
  auto isFinished = llvm::CallInst::Create(finished, {}, "", block);
  auto checkCollect = llvm::BasicBlock::Create(module->getContext(), "checkCollect", block->getParent());
  llvm::BranchInst::Create(stuck, checkCollect, isFinished, block);

  auto collection = module->getOrInsertFunction("is_collection", llvm::FunctionType::get(llvm::Type::getInt1Ty(module->getContext()), {}, false));
  auto isCollection = llvm::CallInst::Create(collection, {}, "", checkCollect);
  auto collect = llvm::BasicBlock::Create(module->getContext(), "isCollect", block->getParent());
  auto merge = llvm::BasicBlock::Create(module->getContext(), "step", block->getParent());
  llvm::BranchInst::Create(collect, merge, isCollection, checkCollect);

  unsigned nroots = 0;
  unsigned i = 0;
  std::vector<llvm::Type *> ptrTypes;
  std::vector<llvm::Value *> roots;
  for (auto type : types) {
    switch(type.cat) {
      case SortCategory::Map:
      case SortCategory::List:
      case SortCategory::Set:
        nroots++;
        ptrTypes.push_back(llvm::PointerType::getUnqual(getValueType(type, module)));
        roots.push_back(args[i]);
        break;
      case SortCategory::Int:
      case SortCategory::Float:
      case SortCategory::StringBuffer:
      case SortCategory::Symbol:
      case SortCategory::Variable:
        nroots++;
        ptrTypes.push_back(getValueType(type, module));
        roots.push_back(args[i]);
        break;
      case SortCategory::Bool:
      case SortCategory::MInt:
        break;
      case SortCategory::Uncomputed:
        abort();
    }
    i++;
  }
  auto arr = module->getOrInsertGlobal("gc_roots", llvm::ArrayType::get(llvm::Type::getInt8PtrTy(module->getContext()), 256));
  std::vector<llvm::Value *> rootPtrs;
  for (unsigned i = 0; i < nroots; i++) {
    auto ptr = llvm::GetElementPtrInst::CreateInBounds(llvm::dyn_cast<llvm::PointerType>(arr->getType())->getElementType(), arr, {llvm::ConstantInt::get(llvm::Type::getInt64Ty(module->getContext()), 0), llvm::ConstantInt::get(llvm::Type::getInt64Ty(module->getContext()), i)}, "", collect);
    auto casted = new llvm::BitCastInst(ptr, llvm::PointerType::getUnqual(ptrTypes[i]), "", collect);
    new llvm::StoreInst(roots[i], casted, collect);
    rootPtrs.push_back(casted);
  }
  std::vector<llvm::Constant *> elements;
  i = 0;
  for (auto cat : types) {
    switch(cat.cat) {
      case SortCategory::Map:
      case SortCategory::List:
      case SortCategory::Set:
      case SortCategory::StringBuffer:
      case SortCategory::Symbol:
      case SortCategory::Variable:
      case SortCategory::Int:
      case SortCategory::Float:
        elements.push_back(llvm::ConstantStruct::get(module->getTypeByName(LAYOUTITEM_STRUCT), llvm::ConstantInt::get(llvm::Type::getInt64Ty(module->getContext()), i++ * 8), llvm::ConstantInt::get(llvm::Type::getInt16Ty(module->getContext()), (int)cat.cat + cat.bits)));
        break;
      case SortCategory::Bool:
      case SortCategory::MInt:
        break;
      case SortCategory::Uncomputed:
        abort();
    }
  }
  auto layoutArr = llvm::ConstantArray::get(llvm::ArrayType::get(module->getTypeByName(LAYOUTITEM_STRUCT), elements.size()), elements);
  auto layout = module->getOrInsertGlobal("layout_item_rule_" + std::to_string(ordinal), layoutArr->getType());
  llvm::GlobalVariable *globalVar = llvm::dyn_cast<llvm::GlobalVariable>(layout);
  if (!globalVar->hasInitializer()) {
    globalVar->setInitializer(layoutArr);
  }
  auto koreCollect = module->getOrInsertFunction("koreCollect", llvm::FunctionType::get(llvm::Type::getVoidTy(module->getContext()), {arr->getType(), llvm::Type::getInt8Ty(module->getContext()), layout->getType()}, false));
  auto call = llvm::CallInst::Create(koreCollect, {arr, llvm::ConstantInt::get(llvm::Type::getInt8Ty(module->getContext()), nroots), layout}, "", collect);
  setDebugLoc(call);
  i = 0;
  std::vector<llvm::Value *> phis;
  for (auto ptr : rootPtrs) {
    auto loaded = new llvm::LoadInst(ptr, "", collect);
    auto phi = llvm::PHINode::Create(loaded->getType(), 2, "phi", merge);
    phi->addIncoming(loaded, collect);
    phi->addIncoming(roots[i++], checkCollect);
    phis.push_back(phi);
  }
  llvm::BranchInst::Create(merge, collect);
  i = 0;
  unsigned rootIdx = 0;
  std::vector<llvm::Value *> results;
  for (auto type : types) {
    switch(type.cat) {
      case SortCategory::Map:
      case SortCategory::List:
      case SortCategory::Set:
      case SortCategory::StringBuffer:
      case SortCategory::Symbol:
      case SortCategory::Variable:
      case SortCategory::Int:
      case SortCategory::Float:
        results.push_back(phis[rootIdx++]);
        break;
      default:
        results.push_back(args[i]);
    }
    i++;
  }
  return std::make_pair(results, merge);
}

void makeStepFunction(KOREDefinition *definition, llvm::Module *module, DecisionNode *dt) {
  auto blockType = getValueType({SortCategory::Symbol, 0}, module);
  llvm::FunctionType *funcType = llvm::FunctionType::get(blockType, {blockType}, false);
  std::string name = "step";
  llvm::Constant *func = module->getOrInsertFunction(name, funcType);
  llvm::Function *matchFunc = llvm::cast<llvm::Function>(func);
  auto debugType = getDebugType({SortCategory::Symbol, 0});
  resetDebugLoc();
  initDebugFunction(name, name, getDebugFunctionType(debugType, {debugType}), definition, matchFunc);
  matchFunc->setCallingConv(llvm::CallingConv::Fast);
  llvm::StringMap<llvm::Value *> subst;
  auto val = matchFunc->arg_begin();
  llvm::BasicBlock *block = llvm::BasicBlock::Create(module->getContext(), "entry", matchFunc);
  llvm::AllocaInst *addr = new llvm::AllocaInst(llvm::Type::getInt8PtrTy(module->getContext()), 0, "jumpTo", block);
  llvm::BasicBlock *stuck = llvm::BasicBlock::Create(module->getContext(), "stuck", matchFunc);
  llvm::BasicBlock *pre_stuck = llvm::BasicBlock::Create(module->getContext(), "pre_stuck", matchFunc);
  new llvm::StoreInst(llvm::BlockAddress::get(matchFunc, pre_stuck), addr, block);
  initDebugParam(matchFunc, 0, "subject", {SortCategory::Symbol, 0});
  llvm::BranchInst::Create(stuck, pre_stuck);
  auto result = stepFunctionHeader(0, module, definition, block, stuck, {val}, {{SortCategory::Symbol, 0}});
  auto collectedVal = result.first[0];
  collectedVal->setName("_1");
  subst.insert({collectedVal->getName(), collectedVal});
  auto phi = llvm::PHINode::Create(collectedVal->getType(), 2, "phi_1", stuck);
  phi->addIncoming(val, block);
  phi->addIncoming(collectedVal, pre_stuck);
  llvm::ReturnInst::Create(module->getContext(), phi, stuck);

  llvm::BasicBlock *fail = llvm::BasicBlock::Create(module->getContext(), "fail", matchFunc);
  llvm::LoadInst *load = new llvm::LoadInst(addr, "", fail);
  llvm::IndirectBrInst *jump = llvm::IndirectBrInst::Create(load, 1, fail);
  jump->addDestination(pre_stuck);

  Decision codegen(definition, result.second, fail, jump, addr, module, {SortCategory::Symbol, 0});
  codegen(dt, subst);
}

// TODO: actually collect the return value of this function. Right now it
// assumes that it will never be collected and constructs a unique_ptr pointing
// to the return value of the recursive call. This is not really safe if the
// object actually gets collected because it's possible for the return value of the
// recursive call to actually be owned by the axiom that it is a part of. However,
// since we then immediately call release on the pointer and never free it, it should be
// safe, it will just leak memory. But we don't really care that much about memory leaks
// in the compiler right now, so it's probably fine.
KOREPattern *makePartialTerm(KOREPattern *term, std::set<std::string> occurrences, std::string occurrence) {
  if (occurrences.count(occurrence)) {
    return KOREVariablePattern::Create(occurrence, term->getSort()).release();
  }
  if (auto pat = dynamic_cast<KORECompositePattern *>(term)) {
    if (pat->getConstructor()->getName() == "\\dv") {
      return term;
    }
    ptr<KORECompositePattern> result = KORECompositePattern::Create(pat->getConstructor());
    for (unsigned i = 0; i < pat->getArguments().size(); i++) {
      result->addArgument(ptr<KOREPattern>(makePartialTerm(dynamic_cast<KOREPattern *>(pat->getArguments()[i].get()), occurrences, "_" + std::to_string(i) + occurrence)));
    }
    return result.release();
  }
  abort();
}

void makeStepFunction(KOREAxiomDeclaration *axiom, KOREDefinition *definition, llvm::Module *module, PartialStep res) {
  auto blockType = getValueType({SortCategory::Symbol, 0}, module);
  std::vector<llvm::Type *> argTypes;
  std::vector<llvm::Metadata *> debugTypes;
  for (auto res : res.residuals) {
    auto argSort = dynamic_cast<KORECompositeSort *>(res.pattern->getSort().get());
    auto cat = argSort->getCategory(definition);
    debugTypes.push_back(getDebugType(cat));
    switch (cat.cat) {
    case SortCategory::Map:
    case SortCategory::List:
    case SortCategory::Set:
      argTypes.push_back(llvm::PointerType::getUnqual(getValueType(cat, module)));
      break;
    default:
      argTypes.push_back(getValueType(cat, module));
      break;
    }
  }
  auto blockDebugType = getDebugType({SortCategory::Symbol, 0});
  llvm::FunctionType *funcType = llvm::FunctionType::get(blockType, argTypes, false);
  std::string name = "step_" + std::to_string(axiom->getOrdinal());
  llvm::Constant *func = module->getOrInsertFunction(name, funcType);
  llvm::Function *matchFunc = llvm::dyn_cast<llvm::Function>(func);
  resetDebugLoc();
  initDebugFunction(name, name, getDebugFunctionType(blockDebugType, debugTypes), definition, matchFunc);
  if (!matchFunc) {
    func->print(llvm::errs());
    abort();
  }
  matchFunc->setCallingConv(llvm::CallingConv::Fast);
  if (!matchFunc) {
    func->print(llvm::errs());
    abort();
  }
  llvm::StringMap<llvm::Value *> subst;
  llvm::StringMap<llvm::Value *> stuckSubst;
  llvm::BasicBlock *block = llvm::BasicBlock::Create(module->getContext(), "entry", matchFunc);
  llvm::AllocaInst *addr = new llvm::AllocaInst(llvm::Type::getInt8PtrTy(module->getContext()), 0, "jumpTo", block);
  llvm::BasicBlock *stuck = llvm::BasicBlock::Create(module->getContext(), "stuck", matchFunc);
  llvm::BasicBlock *pre_stuck = llvm::BasicBlock::Create(module->getContext(), "pre_stuck", matchFunc);
  new llvm::StoreInst(llvm::BlockAddress::get(matchFunc, pre_stuck), addr, block);
  llvm::BranchInst::Create(stuck, pre_stuck);
  std::vector<llvm::PHINode *> phis;
  int i = 0;
  std::vector<llvm::Value *> args;
  std::vector<ValueType> types;
  for (auto val = matchFunc->arg_begin(); val != matchFunc->arg_end(); ++val, ++i) {
    args.push_back(val);
    auto phi = llvm::PHINode::Create(val->getType(), 2, "phi" + res.residuals[i].occurrence, stuck);
    phi->addIncoming(val, block);
    phis.push_back(phi);
    auto sort = res.residuals[i].pattern->getSort();
    auto cat = dynamic_cast<KORECompositeSort *>(sort.get())->getCategory(definition);
    types.push_back(cat);
    initDebugParam(matchFunc, i, "_" + std::to_string(i+1), cat);
  }
  auto header = stepFunctionHeader(axiom->getOrdinal(), module, definition, block, stuck, args, types);
  i = 0;
  for (auto val : header.first) {
    val->setName(res.residuals[i].occurrence.substr(0, max_name_length));
    subst.insert({val->getName(), val});
    stuckSubst.insert({val->getName(), phis[i]});
    phis[i++]->addIncoming(val, pre_stuck);
  }
  std::set<std::string> occurrences;
  for (auto residual : res.residuals) {
    occurrences.insert(residual.occurrence);
  }
  KOREPattern *partialTerm = makePartialTerm(dynamic_cast<KOREPattern *>(axiom->getRightHandSide()), occurrences, "_1");
  CreateTerm creator(stuckSubst, definition, stuck, module, false);
  llvm::Value *retval = creator(partialTerm).first;
  llvm::ReturnInst::Create(module->getContext(), retval, creator.getCurrentBlock());

  llvm::BasicBlock *fail = llvm::BasicBlock::Create(module->getContext(), "fail", matchFunc);
  llvm::LoadInst *load = new llvm::LoadInst(addr, "", fail);
  llvm::IndirectBrInst *jump = llvm::IndirectBrInst::Create(load, 1, fail);
  jump->addDestination(pre_stuck);

  Decision codegen(definition, header.second, fail, jump, addr, module, {SortCategory::Symbol, 0});
  codegen(res.dt, subst);
}
}
