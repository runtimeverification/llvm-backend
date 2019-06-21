#include "kllvm/codegen/Decision.h"
#include "kllvm/codegen/CreateTerm.h"

#include "llvm/IR/CFG.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Instructions.h" 
#include "llvm/Support/raw_ostream.h"

#include <iostream>

namespace kllvm {

static std::string BLOCK_STRUCT = "block";

FailNode FailNode::instance;

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

std::set<std::string> DecisionNode::collectVars() {
  if (hasVars) {
    return vars;
  }
  collectDefs();
  collectUses();
  vars = uses;
  for (std::string var : defs) {
    vars.erase(var);
  }
  hasVars = true;
  return vars;
}

void DecisionNode::sharedNode(Decision *d, llvm::StringMap<llvm::Value *> &oldSubst, llvm::StringMap<llvm::Value *> &substitution, llvm::BasicBlock *Block) {
  std::set<std::string> vars = collectVars();
  collectFail();
  if (containsFailNode) {
    vars.insert(d->ChoiceVars.begin(), d->ChoiceVars.end());
  }
  for (std::string var : vars) {
    auto Phi = phis[var][oldSubst[var]->getType()];
    if (!Phi) {
      Phi = llvm::PHINode::Create(oldSubst[var]->getType(), 1, "aux_phi" + var, cachedCode->getFirstNonPHI());
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
  std::set<std::string> vars = collectVars();
  collectFail();
  if (containsFailNode) {
    vars.insert(d->ChoiceVars.begin(), d->ChoiceVars.end());
  }
  auto Block = llvm::BasicBlock::Create(d->Ctx,
      name,
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
    auto Phi = llvm::PHINode::Create(substitution[var]->getType(), 1, "phi" + var, Block);
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
          name + "_case_" + std::to_string(idx++),
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
      KOREObjectSymbolDeclaration *symbolDecl = d->Definition->getSymbolDeclarations().at(_case.getConstructor()->getName());
      llvm::Value *Renamed;
      for (std::string binding : _case.getBindings()) {
        llvm::Value *ChildPtr = llvm::GetElementPtrInst::CreateInBounds(BlockType, Cast, {llvm::ConstantInt::get(llvm::Type::getInt64Ty(d->Ctx), 0), llvm::ConstantInt::get(llvm::Type::getInt32Ty(d->Ctx), offset+2)}, "", d->CurrentBlock);
        llvm::Value *Child;
        switch (dynamic_cast<KOREObjectCompositeSort *>(_case.getConstructor()->getArguments()[offset])->getCategory(d->Definition).cat) {
        case SortCategory::Map:
        case SortCategory::List:
        case SortCategory::Set:
          Child = ChildPtr;
          break;
        default:
          Child = new llvm::LoadInst(ChildPtr, binding, d->CurrentBlock);
          break;
        }
        auto BlockPtr = llvm::PointerType::getUnqual(d->Module->getTypeByName(BLOCK_STRUCT));
        if (symbolDecl->getAttributes().count("binder")) {
          if (offset == 0) {
            Renamed = llvm::CallInst::Create(d->Module->getOrInsertFunction("alphaRename", BlockPtr, BlockPtr), Child, "renamedVar", d->CurrentBlock);
            substitution[binding] = Renamed;
          } else if (offset == _case.getBindings().size() - 1) {
            llvm::Value *Replaced = llvm::CallInst::Create(d->Module->getOrInsertFunction("replaceBinderIndex", BlockPtr, BlockPtr, BlockPtr), {Child, Renamed}, "withUnboundIndex", d->CurrentBlock);
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
  Call->setName(name);
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
  AllocSret->setName(name);
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
  func->arg_begin()->addAttr(llvm::Attribute::StructRet);
  call->addParamAttr(0, llvm::Attribute::StructRet);
  substitution[name] = AllocSret;
  child->codegen(d, substitution);
  setCompleted();
}

void IterNextNode::codegen(Decision *d, llvm::StringMap<llvm::Value *> substitution) {
  d->ChoiceVars = collectVars();
  if (beginNode(d, "choice" + binding, substitution)) {
    return;
  }
  if (d->ChoiceBlock) {
    abort();
  }
  d->ChoiceBlock = d->CurrentBlock;
  d->ChoiceNode = this;
  llvm::Value *arg = substitution.lookup(iterator);

  collectFail();
  if (containsFailNode) {
    for (std::string var : d->ChoiceVars) {
      auto Phi = phis[var][substitution[var]->getType()];
      if (!Phi) {
        for (std::string v : collectVars()) {
          std::cerr << v << std::endl;
        }
        abort();
      }
      auto failPhi = d->failPhis[var][Phi->getType()];
      if (!failPhi) {
        failPhi = llvm::PHINode::Create(Phi->getType(), 0, "phi" + var, d->FailureBlock->getFirstNonPHI());
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
  auto Call = llvm::CallInst::Create(func, {arg}, binding, d->CurrentBlock);
  substitution[binding] = Call;
  child->codegen(d, substitution);
  d->ChoiceBlock = nullptr;
  d->ChoiceNode = nullptr;
  d->ChoiceVars.clear();
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
  auto Call = llvm::CallInst::Create(d->Module->getOrInsertFunction(name, llvm::FunctionType::get(getValueType(d->Cat, d->Module), types, false)), args, "", d->CurrentBlock);
  Call->setCallingConv(llvm::CallingConv::Fast);
  llvm::ReturnInst::Create(d->Ctx, Call, d->CurrentBlock);
  setCompleted();
}

llvm::Value *Decision::getTag(llvm::Value *val) {
  return llvm::CallInst::Create(Module->getOrInsertFunction("getTag", llvm::Type::getInt32Ty(Ctx), getValueType({SortCategory::Symbol, 0}, Module)), val, "tag", CurrentBlock);
}

void makeEvalOrAnywhereFunction(KOREObjectSymbol *function, KOREDefinition *definition, llvm::Module *module, DecisionNode *dt, void (*addStuck)(llvm::BasicBlock*, llvm::Module*, KOREObjectSymbol *, llvm::StringMap<llvm::Value *>&, KOREDefinition *)) {
  auto returnSort = dynamic_cast<KOREObjectCompositeSort *>(function->getSort())->getCategory(definition);
  auto returnType = getValueType(returnSort, module);
  std::vector<llvm::Type *> args;
  std::vector<ValueType> cats;
  for (auto sort : function->getArguments()) {
    auto cat = dynamic_cast<KOREObjectCompositeSort *>(sort)->getCategory(definition);
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
  matchFunc->setCallingConv(llvm::CallingConv::Fast);
  llvm::StringMap<llvm::Value *> subst;
  llvm::BasicBlock *block = llvm::BasicBlock::Create(module->getContext(), "entry", matchFunc);
  int i = 0;
  for (auto val = matchFunc->arg_begin(); val != matchFunc->arg_end(); ++val, ++i) {
    val->setName("_" + std::to_string(i+1));
    subst.insert({val->getName(), val});
  }
  llvm::AllocaInst *addr = new llvm::AllocaInst(llvm::Type::getInt8PtrTy(module->getContext()), 0, "jumpTo", block);
  llvm::BasicBlock *stuck = llvm::BasicBlock::Create(module->getContext(), "stuck", matchFunc);
  new llvm::StoreInst(llvm::BlockAddress::get(matchFunc, stuck), addr, block);
  addStuck(stuck, module, function, subst, definition);

  llvm::BasicBlock *fail = llvm::BasicBlock::Create(module->getContext(), "fail", matchFunc);
  llvm::LoadInst *load = new llvm::LoadInst(addr, "", fail);
  llvm::IndirectBrInst *jump = llvm::IndirectBrInst::Create(load, 1, fail);
  jump->addDestination(stuck);

  Decision codegen(definition, block, fail, jump, addr, module, returnSort);
  codegen(dt, subst);
}

void abortWhenStuck(llvm::BasicBlock *stuck, llvm::Module *module, KOREObjectSymbol *, llvm::StringMap<llvm::Value *> &, KOREDefinition *) {
  addAbort(stuck, module);
}

void makeEvalFunction(KOREObjectSymbol *function, KOREDefinition *definition, llvm::Module *module, DecisionNode *dt) {
  makeEvalOrAnywhereFunction(function, definition, module, dt, abortWhenStuck);
}

void addOwise(llvm::BasicBlock *stuck, llvm::Module *module, KOREObjectSymbol *symbol, llvm::StringMap<llvm::Value *> &subst, KOREDefinition *d) {
  CreateTerm creator = CreateTerm(subst, d, stuck, module, true);
  KOREObjectCompositePattern *pat = KOREObjectCompositePattern::Create(symbol);
  for (int i = 0; i < symbol->getArguments().size(); i++) {
     auto var = KOREObjectVariablePattern::Create("_" + std::to_string(i+1), symbol->getArguments()[i]);
     pat->addArgument(var);
  }
  llvm::Value *retval = creator(pat).first;
  auto returnSort = dynamic_cast<KOREObjectCompositeSort *>(symbol->getSort())->getCategory(d);
  auto returnType = getValueType(returnSort, module);
  if (retval->getType() == llvm::PointerType::getUnqual(returnType)) {
    retval = new llvm::LoadInst(retval, "", creator.getCurrentBlock());
  }
  llvm::ReturnInst::Create(module->getContext(), retval, creator.getCurrentBlock());
}

void makeAnywhereFunction(KOREObjectSymbol *function, KOREDefinition *definition, llvm::Module *module, DecisionNode *dt) {
  makeEvalOrAnywhereFunction(function, definition, module, dt, addOwise);
}

void makeStepFunction(KOREDefinition *definition, llvm::Module *module, DecisionNode *dt) {
  auto blockType = getValueType({SortCategory::Symbol, 0}, module);
  llvm::FunctionType *funcType = llvm::FunctionType::get(blockType, {blockType}, false);
  std::string name = "step";
  llvm::Constant *func = module->getOrInsertFunction(name, funcType);
  llvm::Function *matchFunc = llvm::cast<llvm::Function>(func);
  llvm::StringMap<llvm::Value *> subst;
  auto val = matchFunc->arg_begin();
  val->setName("_1");
  subst.insert({val->getName(), val});
  llvm::BasicBlock *block = llvm::BasicBlock::Create(module->getContext(), "entry", matchFunc);
  llvm::AllocaInst *addr = new llvm::AllocaInst(llvm::Type::getInt8PtrTy(module->getContext()), 0, "jumpTo", block);
  llvm::BasicBlock *stuck = llvm::BasicBlock::Create(module->getContext(), "stuck", matchFunc);
  new llvm::StoreInst(llvm::BlockAddress::get(matchFunc, stuck), addr, block);
  llvm::ReturnInst::Create(module->getContext(), llvm::ConstantPointerNull::get(llvm::dyn_cast<llvm::PointerType>(blockType)), stuck);

  llvm::BasicBlock *fail = llvm::BasicBlock::Create(module->getContext(), "fail", matchFunc);
  llvm::LoadInst *load = new llvm::LoadInst(addr, "", fail);
  llvm::IndirectBrInst *jump = llvm::IndirectBrInst::Create(load, 1, fail);
  jump->addDestination(stuck);

  Decision codegen(definition, block, fail, jump, addr, module, {SortCategory::Symbol, 0});
  codegen(dt, subst);
}


}
