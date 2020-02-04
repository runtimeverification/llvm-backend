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

static unsigned max_name_length = 1024 - std::to_string(std::numeric_limits<unsigned long long>::max()).length();

void Decision::operator()(DecisionNode *entry, std::map<std::pair<std::string, llvm::Type *>, llvm::Value *> substitution) {
  if (entry == FailNode::get()) {
    llvm::BranchInst::Create(this->FailureBlock, this->CurrentBlock);
  } else {
    if (entry->containsFailNode) {
      for (auto var : FailNode::get()->vars) {
        llvm::PHINode *failPhi = llvm::PHINode::Create(var.second, FailNode::get()->predecessors.size(), "phi" + var.first.substr(0, max_name_length), FailureBlock->getFirstNonPHI());
        failPhis[var] = failPhi;
      }
    }
    entry->codegen(this, substitution);
  }
  for (auto &entry : failPhis) {
    llvm::PHINode *Phi = entry.second;
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
  for (auto block = this->CurrentBlock->getParent()->begin(); block != this->CurrentBlock->getParent()->end(); ++block) {
    if (block->getUniquePredecessor()) {
      for (auto phi = block->phis().begin(); phi != block->phis().end(); phi = block->phis().begin()) {
        phi->replaceAllUsesWith(phi->getIncomingValue(0));
        phi->removeFromParent();
      }
    }
  }
}

void SwitchNode::eraseDefsAndAddUses(var_set_type &vars) {
  for (auto _case : cases) {
    var_set_type caseVars;
    if (_case.getChild() == FailNode::get()) {
      for (DecisionNode *trueSucc : _case.getChild()->successors) {
        if (choiceAncestors.count(dynamic_cast<IterNextNode *>(trueSucc))) {
          caseVars.insert(trueSucc->vars.begin(), trueSucc->vars.end());
        }
      }
    } else {
      caseVars = _case.getChild()->vars;
    }
    for (auto var : _case.getBindings()) {
      caseVars.erase(var);
    }
    vars.insert(caseVars.begin(), caseVars.end());
  }
  if(cases.size() != 1 || cases[0].getConstructor()) vars.insert(std::make_pair(name, type)); 
}

void DecisionNode::computeLiveness(std::unordered_set<LeafNode *> &leaves) {
  std::deque<DecisionNode *> workList;
  std::unordered_set<DecisionNode *> workListSet;
  workList.insert(workList.end(), leaves.begin(), leaves.end());
  workListSet.insert(leaves.begin(), leaves.end());
  if (containsFailNode) {
    workList.push_back(FailNode::get());
    workListSet.insert(FailNode::get());
  }
  while (!workList.empty()) {
    DecisionNode *node = workList.front();
    workList.pop_front();
    workListSet.erase(node);
    var_set_type newVars;
    if (!dynamic_cast<SwitchNode *>(node)) {
      for (DecisionNode *succ : node->successors) {
        if (succ == FailNode::get()) {
          for (DecisionNode *trueSucc : succ->successors) {
            if (node->choiceAncestors.count(dynamic_cast<IterNextNode *>(trueSucc))) {
              newVars.insert(trueSucc->vars.begin(), trueSucc->vars.end());
            }
          }
	} else {
          newVars.insert(succ->vars.begin(), succ->vars.end());
        }
      }
    }
    node->eraseDefsAndAddUses(newVars);
    if (newVars != node->vars || !livenessVisited) {
      node->vars = newVars;
      for (DecisionNode *pred : node->predecessors) {
        if (!workListSet.count(pred)) {
          workListSet.insert(pred);
          workList.push_back(pred);
        }
      }
    }
    node->livenessVisited = true;
  }
}

void DecisionNode::sharedNode(Decision *d, std::map<std::pair<std::string, llvm::Type *>, llvm::Value *> &oldSubst, std::map<std::pair<std::string, llvm::Type *>, llvm::Value *> &substitution, llvm::BasicBlock *Block) {
  for (auto var : vars) {
    auto Phi = phis[var];
    Phi->addIncoming(oldSubst[var], Block);
    substitution[var] = Phi;
  }
  for (auto &entry : phis) {
    auto var = entry.first;
    auto Phi = entry.second;
    if (!vars.count(var)) {
      Phi->addIncoming(llvm::UndefValue::get(Phi->getType()), Block);
    }
  }
}
 

bool DecisionNode::beginNode(Decision *d, std::string name, std::map<std::pair<std::string, llvm::Type *>, llvm::Value *> &substitution) {
  if (isCompleted()) {
    llvm::BranchInst::Create(cachedCode, d->CurrentBlock);
    sharedNode(d, substitution, substitution, d->CurrentBlock);
    return true;
  }
  auto Block = llvm::BasicBlock::Create(d->Ctx,
      name.substr(0, max_name_length),
      d->CurrentBlock->getParent());
  cachedCode = Block;
  llvm::BranchInst::Create(Block, d->CurrentBlock);
  for (auto var : vars) {
    auto val = substitution[var];
    if (!val) {
      val = llvm::UndefValue::get(var.second);
    }
    auto Phi = llvm::PHINode::Create(var.second, 1, "phi" + var.first.substr(0, max_name_length), Block);
    Phi->addIncoming(val, d->CurrentBlock);
    phis[var] = Phi;
    substitution[var] = Phi;
  }
  d->CurrentBlock = Block;
  return false;
}

void Decision::addFailPhiIncoming(std::map<std::pair<std::string, llvm::Type *>, llvm::Value *> oldSubst, llvm::BasicBlock *switchBlock, var_set_type &vars) {
  for (auto var : FailNode::get()->vars) {
    if (vars.count(var)) {
      auto val = oldSubst[var];
      if (!val) {
        val = llvm::UndefValue::get(var.second);
      }
      failPhis[var]->addIncoming(val, switchBlock);
    }
  }
}

void SwitchNode::codegen(Decision *d, std::map<std::pair<std::string, llvm::Type *>, llvm::Value *> substitution) {
  if (beginNode(d, "switch" + name, substitution)) {
    return;
  }
  llvm::Value *val = substitution[std::make_pair(name, type)];
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
  auto currChoiceBlock = d->ChoiceBlock;
  d->ChoiceBlock = nullptr;
  for (auto &entry : caseData) {
    auto newSubst = substitution;
    auto &_case = *entry.second;
    if (entry.first == d->FailureBlock) {
      d->addFailPhiIncoming(substitution, switchBlock, this->vars);
      continue;
    }
    d->CurrentBlock = entry.first;
    if (!isInt) {
      int offset = 0;
      llvm::StructType *BlockType = getBlockType(d->Module, d->Definition, _case.getConstructor());
      llvm::BitCastInst *Cast = new llvm::BitCastInst(val, llvm::PointerType::getUnqual(BlockType), "", d->CurrentBlock);
      KORESymbolDeclaration *symbolDecl = d->Definition->getSymbolDeclarations().at(_case.getConstructor()->getName());
      llvm::Instruction *Renamed;
      for (auto binding : _case.getBindings()) {
        llvm::Value *ChildPtr = llvm::GetElementPtrInst::CreateInBounds(BlockType, Cast, {llvm::ConstantInt::get(llvm::Type::getInt64Ty(d->Ctx), 0), llvm::ConstantInt::get(llvm::Type::getInt32Ty(d->Ctx), offset+2)}, "", d->CurrentBlock);
        llvm::Value *Child;
        switch (dynamic_cast<KORECompositeSort *>(_case.getConstructor()->getArguments()[offset].get())->getCategory(d->Definition).cat) {
        case SortCategory::Map:
        case SortCategory::List:
        case SortCategory::Set:
          Child = ChildPtr;
          break;
        default:
          Child = new llvm::LoadInst(ChildPtr, binding.first.substr(0, max_name_length), d->CurrentBlock);
          break;
        }
        auto BlockPtr = llvm::PointerType::getUnqual(d->Module->getTypeByName(BLOCK_STRUCT));
        if (symbolDecl->getAttributes().count("binder")) {
          if (offset == 0) {
            Renamed = llvm::CallInst::Create(getOrInsertFunction(d->Module, "alphaRename", BlockPtr, BlockPtr), Child, "renamedVar", d->CurrentBlock);
            setDebugLoc(Renamed);
            newSubst[binding] = Renamed;
          } else if (offset == _case.getBindings().size() - 1) {
            llvm::Instruction *Replaced = llvm::CallInst::Create(getOrInsertFunction(d->Module, "replaceBinderIndex", BlockPtr, BlockPtr, BlockPtr), {Child, Renamed}, "withUnboundIndex", d->CurrentBlock);
            setDebugLoc(Replaced);
            newSubst[binding] = Replaced;
          } else {
            newSubst[binding] = Child;
          }
        } else {
          newSubst[binding] = Child;
        }
        offset++;
      }
    } else {
      if (currChoiceBlock && _case.getLiteral() == 1) {
        auto PrevDepth = new llvm::LoadInst(d->ChoiceDepth, "", d->CurrentBlock);
        auto CurrDepth = llvm::BinaryOperator::Create(llvm::Instruction::Add, PrevDepth, llvm::ConstantInt::get(llvm::Type::getInt64Ty(d->Ctx), 1), "", d->CurrentBlock);
        new llvm::StoreInst(CurrDepth, d->ChoiceDepth, d->CurrentBlock);

        auto ty = d->ChoiceBuffer->getType()->getElementType();
        auto zero = llvm::ConstantInt::get(llvm::Type::getInt64Ty(d->Ctx), 0);
        auto currentElt = llvm::GetElementPtrInst::CreateInBounds(ty, d->ChoiceBuffer, {zero, CurrDepth}, "", d->CurrentBlock);
        new llvm::StoreInst(llvm::BlockAddress::get(d->CurrentBlock->getParent(), currChoiceBlock), currentElt, d->CurrentBlock);
        d->FailJump->addDestination(currChoiceBlock);
      }
    }
    _case.getChild()->codegen(d, newSubst);
  }
  if (defaultCase) {
    if (_default != d->FailureBlock) {
      // process default also
      d->CurrentBlock = _default;
      defaultCase->getChild()->codegen(d, substitution);
    } else {
      d->addFailPhiIncoming(substitution, switchBlock, this->vars);
    }
  } else {
    d->addFailPhiIncoming(substitution, switchBlock, this->vars);
  }

  setCompleted();
}

void MakePatternNode::codegen(Decision *d, std::map<std::pair<std::string, llvm::Type *>, llvm::Value *> substitution) {
  if (beginNode(d, "pattern" + name, substitution)) {
    return;
  }
  llvm::StringMap<llvm::Value *> finalSubst;
  for (auto use : uses) {
    finalSubst[use.first] = substitution[use];
  }
  CreateTerm creator(finalSubst, d->Definition, d->CurrentBlock, d->Module, false);
  llvm::Value *val = creator(pattern).first;
  d->CurrentBlock = creator.getCurrentBlock();
  substitution[std::make_pair(name, type)] = val;
  child->codegen(d, substitution);
  setCompleted();
}

void FunctionNode::codegen(Decision *d, std::map<std::pair<std::string, llvm::Type *>, llvm::Value *> substitution) {
  if (beginNode(d, "function" + name, substitution)) {
    return;
  }
  std::vector<llvm::Value *> args;
  llvm::StringMap<llvm::Value *> finalSubst;
  for (auto arg : bindings) {
    llvm::Value *val;
    if (arg.first.find_first_not_of("-0123456789") == std::string::npos) {
      val = llvm::ConstantInt::get(llvm::Type::getInt64Ty(d->Ctx), std::stoi(arg.first));
    } else {
      val = substitution[arg];
    }
    args.push_back(val);
    finalSubst[arg.first] = val;
  }
  CreateTerm creator(finalSubst, d->Definition, d->CurrentBlock, d->Module, false);
  auto Call = creator.createFunctionCall(function, cat, args, function.substr(0, 5) == "hook_", false);
  Call->setName(name.substr(0, max_name_length));
  substitution[std::make_pair(name, type)] = Call;
  child->codegen(d, substitution);
  setCompleted();
}

void MakeIteratorNode::codegen(Decision *d, std::map<std::pair<std::string, llvm::Type *>, llvm::Value *> substitution) {
  if (beginNode(d, "new_iterator" + name, substitution)) {
    return;
  }
  std::vector<llvm::Value *> args;
  std::vector<llvm::Type *> types;
  llvm::Value *arg = substitution[std::make_pair(collection, collectionType)];
  args.push_back(arg);
  types.push_back(arg->getType());
  llvm::Value *AllocSret = allocateTerm(d->Module->getTypeByName("iter"), d->CurrentBlock, "koreAllocAlwaysGC");
  AllocSret->setName(name.substr(0, max_name_length));
  args.insert(args.begin(), AllocSret);
  types.insert(types.begin(), AllocSret->getType());

  llvm::FunctionType *funcType = llvm::FunctionType::get(llvm::Type::getVoidTy(d->Module->getContext()), types, false);
  llvm::Function *func = getOrInsertFunction(d->Module, hookName, funcType);
  auto call = llvm::CallInst::Create(func, args, "", d->CurrentBlock);
  setDebugLoc(call);
  func->arg_begin()->addAttr(llvm::Attribute::StructRet);
  call->addParamAttr(0, llvm::Attribute::StructRet);
  substitution[std::make_pair(name, type)] = AllocSret;
  child->codegen(d, substitution);
  setCompleted();
}

void IterNextNode::codegen(Decision *d, std::map<std::pair<std::string, llvm::Type *>, llvm::Value *> substitution) {
  if (beginNode(d, "choice" + binding, substitution)) {
    return;
  }
  d->ChoiceBlock = d->CurrentBlock;
  llvm::Value *arg = substitution[std::make_pair(iterator, iteratorType)];

  if (containsFailNode) {
    for (auto var : vars) {
      auto Phi = phis[var];
      if (!Phi) {
        for (auto v : vars) {
          std::cerr << v.first << std::endl;
        }
        abort();
      }
      auto failPhi = d->failPhis[var];
      Phi->addIncoming(failPhi, d->FailureBlock);
    }
  }

  llvm::FunctionType *funcType = llvm::FunctionType::get(getValueType({SortCategory::Symbol, 0}, d->Module), {arg->getType()}, false);
  llvm::Function *func = getOrInsertFunction(d->Module, hookName, funcType);
  auto Call = llvm::CallInst::Create(func, {arg}, binding.substr(0, max_name_length), d->CurrentBlock);
  setDebugLoc(Call);
  substitution[std::make_pair(binding, bindingType)] = Call;
  child->codegen(d, substitution);
  d->ChoiceBlock = nullptr;
  setCompleted();
}

void LeafNode::codegen(Decision *d, std::map<std::pair<std::string, llvm::Type *>, llvm::Value *> substitution) {
  if (beginNode(d, name, substitution)) {
    return;
  }
  std::vector<llvm::Value *> args;
  std::vector<llvm::Type *> types;
  for (auto arg : bindings) {
    auto val = substitution[arg];
    args.push_back(val);
    types.push_back(val->getType());
  }
  auto type = getParamType(d->Cat, d->Module);
  auto Call = llvm::CallInst::Create(getOrInsertFunction(d->Module, name, llvm::FunctionType::get(type, types, false)), args, "", d->CurrentBlock);
  setDebugLoc(Call);
  Call->setCallingConv(llvm::CallingConv::Fast);
  llvm::ReturnInst::Create(d->Ctx, Call, d->CurrentBlock);
  setCompleted();
}

llvm::Value *Decision::getTag(llvm::Value *val) {
  auto res = llvm::CallInst::Create(getOrInsertFunction(Module, "getTag", llvm::Type::getInt32Ty(Ctx), getValueType({SortCategory::Symbol, 0}, Module)), val, "tag", CurrentBlock);
  setDebugLoc(res);
  return res;
}

static void initChoiceBuffer(DecisionNode *dt, llvm::Module *module, llvm::BasicBlock *block, llvm::BasicBlock *stuck, llvm::BasicBlock *fail, llvm::AllocaInst **choiceBufferOut, llvm::AllocaInst **choiceDepthOut, llvm::IndirectBrInst **jumpOut) {
  FailNode::get()->predecessors.clear();
  FailNode::get()->successors.clear();
  std::unordered_set<LeafNode *> leaves;
  dt->preprocess(leaves);
  dt->computeLiveness(leaves);
  auto ty = llvm::ArrayType::get(llvm::Type::getInt8PtrTy(module->getContext()), dt->getChoiceDepth() + 1);
  llvm::AllocaInst *choiceBuffer = new llvm::AllocaInst(ty, 0, "choiceBuffer", block);
  llvm::AllocaInst *choiceDepth = new llvm::AllocaInst(llvm::Type::getInt64Ty(module->getContext()), 0, "choiceDepth", block);
  auto zero = llvm::ConstantInt::get(llvm::Type::getInt64Ty(module->getContext()), 0);
  new llvm::StoreInst(zero, choiceDepth, block);
  auto firstElt = llvm::GetElementPtrInst::CreateInBounds(ty, choiceBuffer, {zero, zero}, "", block);
  new llvm::StoreInst(llvm::BlockAddress::get(block->getParent(), stuck), firstElt, block);

  llvm::LoadInst *currDepth = new llvm::LoadInst(choiceDepth, "", fail);
  auto currentElt = llvm::GetElementPtrInst::CreateInBounds(ty, choiceBuffer, {zero, currDepth}, "", fail);
  llvm::LoadInst *failAddress = new llvm::LoadInst(currentElt, "", fail);
  auto newDepth = llvm::BinaryOperator::Create(llvm::Instruction::Sub, currDepth, llvm::ConstantInt::get(llvm::Type::getInt64Ty(module->getContext()), 1), "", fail);
  new llvm::StoreInst(newDepth, choiceDepth, fail);
  llvm::IndirectBrInst *jump = llvm::IndirectBrInst::Create(failAddress, 1, fail);
  jump->addDestination(stuck);
  *choiceBufferOut = choiceBuffer;
  *choiceDepthOut = choiceDepth;
  *jumpOut = jump;
}


void makeEvalOrAnywhereFunction(KORESymbol *function, KOREDefinition *definition, llvm::Module *module, DecisionNode *dt, void (*addStuck)(llvm::BasicBlock*, llvm::Module*, KORESymbol *, std::map<std::pair<std::string, llvm::Type *>, llvm::Value *>&, KOREDefinition *)) {
  auto returnSort = dynamic_cast<KORECompositeSort *>(function->getSort().get())->getCategory(definition);
  auto returnType = getParamType(returnSort, module);
  std::ostringstream Out;
  function->getSort()->print(Out);
  auto debugReturnType = getDebugType(returnSort, Out.str());
  std::vector<llvm::Type *> args;
  std::vector<llvm::Metadata *> debugArgs;
  std::vector<ValueType> cats;
  for (auto &sort : function->getArguments()) {
    auto cat = dynamic_cast<KORECompositeSort *>(sort.get())->getCategory(definition);
    std::ostringstream Out;
    sort->print(Out);
    debugArgs.push_back(getDebugType(cat, Out.str()));
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
  std::ostringstream Out2;
  function->print(Out2, 0, false);
  std::string name = "eval_" + Out2.str();
  llvm::Function *matchFunc = getOrInsertFunction(module, name, funcType);
  KORESymbolDeclaration *symbolDecl = definition->getSymbolDeclarations().at(function->getName());
  initDebugAxiom(symbolDecl->getAttributes());
  initDebugFunction(function->getName(), name, getDebugFunctionType(debugReturnType, debugArgs), definition, matchFunc);
  matchFunc->setCallingConv(llvm::CallingConv::Fast);
  std::map<std::pair<std::string, llvm::Type *>, llvm::Value *> subst;
  llvm::BasicBlock *block = llvm::BasicBlock::Create(module->getContext(), "entry", matchFunc);
  llvm::BasicBlock *stuck = llvm::BasicBlock::Create(module->getContext(), "stuck", matchFunc);
  llvm::BasicBlock *fail = llvm::BasicBlock::Create(module->getContext(), "fail", matchFunc);

  llvm::AllocaInst *choiceBuffer, *choiceDepth;
  llvm::IndirectBrInst *jump;
  initChoiceBuffer(dt, module, block, stuck, fail, &choiceBuffer, &choiceDepth, &jump);

  int i = 0;
  for (auto val = matchFunc->arg_begin(); val != matchFunc->arg_end(); ++val, ++i) {
    val->setName("_" + std::to_string(i+1));
    subst[std::make_pair(val->getName(), val->getType())] = val;
    std::ostringstream Out;
    function->getArguments()[i]->print(Out);
    initDebugParam(matchFunc, i, val->getName(), cats[i], Out.str());
  }
  addStuck(stuck, module, function, subst, definition);

  Decision codegen(definition, block, fail, jump, choiceBuffer, choiceDepth, module, returnSort);
  codegen(dt, subst);
}

void abortWhenStuck(llvm::BasicBlock *CurrentBlock, llvm::Module *Module, KORESymbol *symbol, std::map<std::pair<std::string, llvm::Type *>, llvm::Value *> &subst, KOREDefinition *d) {
  auto &Ctx = Module->getContext();
  std::ostringstream Out;
  symbol->print(Out);
  symbol = d->getAllSymbols().at(Out.str());
  auto BlockType = getBlockType(Module, d, symbol);
  llvm::Value *Ptr;
  auto BlockPtr = llvm::PointerType::getUnqual(Module->getTypeByName(BLOCK_STRUCT));
  if (symbol->getArguments().empty()) {
    Ptr = llvm::ConstantExpr::getIntToPtr(llvm::ConstantInt::get(llvm::Type::getInt64Ty(Ctx), ((uint64_t)symbol->getTag() << 32 | 1)), getValueType({SortCategory::Symbol, 0}, Module));
  } else {
    llvm::Value *BlockHeader = getBlockHeader(Module, d, symbol, BlockType);
    llvm::Value *Block = allocateTerm(BlockType, CurrentBlock);
    llvm::Value *BlockHeaderPtr = llvm::GetElementPtrInst::CreateInBounds(BlockType, Block, {llvm::ConstantInt::get(llvm::Type::getInt64Ty(Ctx), 0), llvm::ConstantInt::get(llvm::Type::getInt32Ty(Ctx), 0)}, symbol->getName(), CurrentBlock);
    new llvm::StoreInst(BlockHeader, BlockHeaderPtr, CurrentBlock);
    for (int idx = 0; idx < symbol->getArguments().size(); idx++) {
      auto cat = dynamic_cast<KORECompositeSort *>(symbol->getArguments()[idx].get())->getCategory(d);
      auto type = getParamType(cat, Module);
      llvm::Value *ChildValue = subst[std::make_pair("_" + std::to_string(idx+1), type)];
      llvm::Value *ChildPtr = llvm::GetElementPtrInst::CreateInBounds(BlockType, Block, {llvm::ConstantInt::get(llvm::Type::getInt64Ty(Ctx), 0), llvm::ConstantInt::get(llvm::Type::getInt32Ty(Ctx), idx + 2)}, "", CurrentBlock);
      if (ChildValue->getType() == ChildPtr->getType()) {
        ChildValue = new llvm::LoadInst(ChildValue, "", CurrentBlock);
      }
      new llvm::StoreInst(ChildValue, ChildPtr, CurrentBlock);
    }
    Ptr = new llvm::BitCastInst(Block, BlockPtr, "", CurrentBlock);
  }
  llvm::CallInst::Create(getOrInsertFunction(Module, "finish_rewriting", llvm::Type::getVoidTy(Ctx), BlockPtr, llvm::Type::getInt1Ty(Ctx)), {Ptr, llvm::ConstantInt::getTrue(Ctx)}, "", CurrentBlock);
  new llvm::UnreachableInst(Ctx, CurrentBlock);
}

void makeEvalFunction(KORESymbol *function, KOREDefinition *definition, llvm::Module *module, DecisionNode *dt) {
  makeEvalOrAnywhereFunction(function, definition, module, dt, abortWhenStuck);
}

void addOwise(llvm::BasicBlock *stuck, llvm::Module *module, KORESymbol *symbol, std::map<std::pair<std::string, llvm::Type *>, llvm::Value *> &subst, KOREDefinition *d) {
  llvm::StringMap<llvm::Value *> finalSubst;
  ptr<KORECompositePattern> pat = KORECompositePattern::Create(symbol);
  for (int i = 0; i < symbol->getArguments().size(); i++) {
    auto cat = dynamic_cast<KORECompositeSort *>(symbol->getArguments()[i].get())->getCategory(d);
    auto type = getParamType(cat, module);

    std::string name = "_" + std::to_string(i+1);
    finalSubst[name] = subst[std::make_pair(name, type)];
 
    auto var = KOREVariablePattern::Create(name, symbol->getArguments()[i]);
    pat->addArgument(std::move(var));
  }
  CreateTerm creator = CreateTerm(finalSubst, d, stuck, module, true);
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
  auto finished = getOrInsertFunction(module, "finished_rewriting", llvm::FunctionType::get(llvm::Type::getInt1Ty(module->getContext()), {}, false));
  auto isFinished = llvm::CallInst::Create(finished, {}, "", block);
  auto checkCollect = llvm::BasicBlock::Create(module->getContext(), "checkCollect", block->getParent());
  llvm::BranchInst::Create(stuck, checkCollect, isFinished, block);

  auto collection = getOrInsertFunction(module, "is_collection", llvm::FunctionType::get(llvm::Type::getInt1Ty(module->getContext()), {}, false));
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
  auto ptrTy = llvm::PointerType::getUnqual(llvm::ArrayType::get(module->getTypeByName(LAYOUTITEM_STRUCT), 0));
  auto koreCollect = getOrInsertFunction(module, "koreCollect", llvm::FunctionType::get(llvm::Type::getVoidTy(module->getContext()), {arr->getType(), llvm::Type::getInt8Ty(module->getContext()), ptrTy}, false));
  auto call = llvm::CallInst::Create(koreCollect, {arr, llvm::ConstantInt::get(llvm::Type::getInt8Ty(module->getContext()), nroots), llvm::ConstantExpr::getBitCast(layout, ptrTy)}, "", collect);
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
  llvm::Function *matchFunc = getOrInsertFunction(module, name, funcType);
  auto debugType = getDebugType({SortCategory::Symbol, 0}, "SortGeneratedTopCell{}");
  resetDebugLoc();
  initDebugFunction(name, name, getDebugFunctionType(debugType, {debugType}), definition, matchFunc);
  matchFunc->setCallingConv(llvm::CallingConv::Fast);
  std::map<std::pair<std::string, llvm::Type *>, llvm::Value *> subst;
  auto val = matchFunc->arg_begin();
  llvm::BasicBlock *block = llvm::BasicBlock::Create(module->getContext(), "entry", matchFunc);
  llvm::BasicBlock *stuck = llvm::BasicBlock::Create(module->getContext(), "stuck", matchFunc);
  llvm::BasicBlock *pre_stuck = llvm::BasicBlock::Create(module->getContext(), "pre_stuck", matchFunc);
  llvm::BasicBlock *fail = llvm::BasicBlock::Create(module->getContext(), "fail", matchFunc);

  llvm::AllocaInst *choiceBuffer, *choiceDepth;
  llvm::IndirectBrInst *jump;
  initChoiceBuffer(dt, module, block, pre_stuck, fail, &choiceBuffer, &choiceDepth, &jump);

  initDebugParam(matchFunc, 0, "subject", {SortCategory::Symbol, 0}, "SortGeneratedTopCell{}");
  llvm::BranchInst::Create(stuck, pre_stuck);
  auto result = stepFunctionHeader(0, module, definition, block, stuck, {val}, {{SortCategory::Symbol, 0}});
  auto collectedVal = result.first[0];
  collectedVal->setName("_1");
  subst[std::make_pair(collectedVal->getName(), collectedVal->getType())] = collectedVal;
  auto phi = llvm::PHINode::Create(collectedVal->getType(), 2, "phi_1", stuck);
  phi->addIncoming(val, block);
  phi->addIncoming(collectedVal, pre_stuck);
  llvm::ReturnInst::Create(module->getContext(), phi, stuck);

  Decision codegen(definition, result.second, fail, jump, choiceBuffer, choiceDepth, module, {SortCategory::Symbol, 0});
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
    std::ostringstream Out;
    argSort->print(Out);
    debugTypes.push_back(getDebugType(cat, Out.str()));
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
  auto blockDebugType = getDebugType({SortCategory::Symbol, 0}, "SortGeneratedTopCell{}");
  llvm::FunctionType *funcType = llvm::FunctionType::get(blockType, argTypes, false);
  std::string name = "step_" + std::to_string(axiom->getOrdinal());
  llvm::Function *matchFunc = getOrInsertFunction(module, name, funcType);
  resetDebugLoc();
  initDebugFunction(name, name, getDebugFunctionType(blockDebugType, debugTypes), definition, matchFunc);
  matchFunc->setCallingConv(llvm::CallingConv::Fast);

  std::map<std::pair<std::string, llvm::Type *>, llvm::Value *> subst;
  llvm::StringMap<llvm::Value *> stuckSubst;
  llvm::BasicBlock *block = llvm::BasicBlock::Create(module->getContext(), "entry", matchFunc);
  llvm::BasicBlock *stuck = llvm::BasicBlock::Create(module->getContext(), "stuck", matchFunc);
  llvm::BasicBlock *pre_stuck = llvm::BasicBlock::Create(module->getContext(), "pre_stuck", matchFunc);
  llvm::BasicBlock *fail = llvm::BasicBlock::Create(module->getContext(), "fail", matchFunc);

  llvm::AllocaInst *choiceBuffer, *choiceDepth;
  llvm::IndirectBrInst *jump;
  initChoiceBuffer(res.dt, module, block, pre_stuck, fail, &choiceBuffer, &choiceDepth, &jump);

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
    std::ostringstream Out;
    sort->print(Out);
    initDebugParam(matchFunc, i, "_" + std::to_string(i+1), cat, Out.str());
  }
  auto header = stepFunctionHeader(axiom->getOrdinal(), module, definition, block, stuck, args, types);
  i = 0;
  for (auto val : header.first) {
    val->setName(res.residuals[i].occurrence.substr(0, max_name_length));
    subst[std::make_pair(val->getName(), val->getType())] = val;
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

  Decision codegen(definition, header.second, fail, jump, choiceBuffer, choiceDepth, module, {SortCategory::Symbol, 0});
  codegen(res.dt, subst);
}
}
