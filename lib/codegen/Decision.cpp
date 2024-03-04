#include "kllvm/codegen/Decision.h"

#include "kllvm/codegen/CreateTerm.h"
#include "kllvm/codegen/Debug.h"
#include "kllvm/codegen/ProofEvent.h"
#include "kllvm/codegen/Util.h"

#include <llvm/ADT/APInt.h>
#include <llvm/ADT/SmallString.h>
#include <llvm/ADT/StringMap.h>
#include <llvm/ADT/StringRef.h>
#include <llvm/ADT/Twine.h>
#include <llvm/IR/Argument.h>
#include <llvm/IR/Attributes.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/CallingConv.h>
#include <llvm/IR/Constant.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/DebugInfoMetadata.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/GlobalVariable.h>
#include <llvm/IR/InstrTypes.h>
#include <llvm/IR/Instruction.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/Metadata.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Value.h>
#include <llvm/Support/Casting.h>

#include <fmt/format.h>

#include <iostream>
#include <limits>
#include <memory>
#include <set>
#include <type_traits>

namespace kllvm {

static std::string layoutitem_struct = "layoutitem";

fail_node fail_node::instance;

static unsigned max_name_length
    = 1024
      - std::to_string(std::numeric_limits<unsigned long long>::max()).length();

void decision::operator()(decision_node *entry) {
  if (entry == fail_node::get()) {
    if (fail_pattern_) {
      llvm::Value *val = load(std::make_pair(
          "_1", getvalue_type({sort_category::Symbol, 0}, module_)));
      fail_subject_->addIncoming(
          new llvm::BitCastInst(
              val, llvm::Type::getInt8PtrTy(ctx_), "", current_block_),
          current_block_);
      fail_pattern_->addIncoming(
          string_literal("\\bottom{SortGeneratedTopCell{}}()"), current_block_);
      fail_sort_->addIncoming(
          string_literal("SortGeneratedTopCell{}"), current_block_);
    }
    llvm::BranchInst::Create(this->failure_block_, this->current_block_);
  } else {
    entry->codegen(this);
  }
}

llvm::Value *decision::ptr_term(llvm::Value *val) {
  if (val->getType()->isIntegerTy()) {
    val = allocateTerm(val->getType(), current_block_, "koreAllocAlwaysGC");
  }
  return new llvm::BitCastInst(
      val, llvm::Type::getInt8PtrTy(ctx_), "", current_block_);
}

bool decision_node::begin_node(decision *d, std::string const &name) {
  if (is_completed()) {
    llvm::BranchInst::Create(cached_code_, d->current_block_);
    return true;
  }
  auto *Block = llvm::BasicBlock::Create(
      d->ctx_, name.substr(0, max_name_length), d->current_block_->getParent());
  cached_code_ = Block;
  llvm::BranchInst::Create(Block, d->current_block_);
  d->current_block_ = Block;
  return false;
}

static std::pair<std::string, std::string>
getFailPattern(decision_case const &c, bool is_int) {
  if (is_int) {
    size_t bitwidth = c.get_literal().getBitWidth();
    if (bitwidth == 1) {
      return std::make_pair(
          "SortBool{}", "\\dv{SortBool{}}(\""
                            + (c.get_literal() == 0 ? std::string("false")
                                                   : std::string("true"))
                            + "\")");
    }
    std::string sort = "SortMInt{Sort" + std::to_string(bitwidth) + "{}}";
    llvm::SmallString<25> vec;
    c.get_literal().toString(vec, 10, false);
    return std::make_pair(
        sort, "\\dv{" + sort + "}(\"" + std::string(vec.c_str()) + "p"
                  + std::to_string(bitwidth) + "\")");
  }
  auto result = fmt::format("{}(", ast_to_string(*c.get_constructor()));

  std::string conn;
  for (auto const &i : c.get_constructor()->get_arguments()) {
    result += fmt::format("{}Var'Unds':{}", conn, ast_to_string(*i));
    conn = ",";
  }
  result += ")";

  auto return_sort = ast_to_string(*c.get_constructor()->get_sort());
  return std::make_pair(return_sort, result);
}

static std::pair<std::string, std::string> getFailPattern(
    std::vector<std::pair<llvm::BasicBlock *, decision_case const *>> const
        &case_data,
    bool is_int, llvm::BasicBlock *fail_block) {
  std::string reason;
  std::string sort;
  for (auto const &entry : case_data) {
    auto const &_case = *entry.second;
    if (entry.first != fail_block) {
      auto caseReason = getFailPattern(_case, is_int);
      if (reason.empty()) {
        reason = caseReason.second;
        sort = caseReason.first;
      } else {
        reason
            = fmt::format("\\or{{{}}}({},{})", sort, reason, caseReason.second);
      }
    }
  }
  return std::make_pair(sort, reason);
}

// NOLINTNEXTLINE(*-cognitive-complexity)
void switch_node::codegen(decision *d) {
  if (begin_node(d, "switch" + name_)) {
    return;
  }
  llvm::Value *val = d->load(std::make_pair(name_, type_));
  llvm::Value *ptrVal = nullptr;
  if (d->fail_pattern_) {
    ptrVal = d->ptr_term(val);
  }
  llvm::BasicBlock *_default = d->failure_block_;
  decision_case const *defaultCase = nullptr;
  std::vector<std::pair<llvm::BasicBlock *, decision_case const *>> caseData;
  int idx = 0;
  bool isInt = false;
  for (auto &_case : cases_) {
    auto *child = _case.get_child();
    llvm::BasicBlock *CaseBlock = nullptr;
    if (child == fail_node::get()) {
      CaseBlock = d->failure_block_;
    } else {
      CaseBlock = llvm::BasicBlock::Create(
          d->ctx_,
          name_.substr(0, max_name_length) + "_case_" + std::to_string(idx++),
          d->current_block_->getParent());
    }
    if (auto *sym = _case.get_constructor()) {
      isInt = isInt || sym->get_name() == "\\dv";
      caseData.emplace_back(CaseBlock, &_case);
    } else {
      _default = CaseBlock;
      defaultCase = &_case;
    }
  }
  if (is_check_null_) {
    auto *cast = new llvm::PtrToIntInst(
        val, llvm::Type::getInt64Ty(d->ctx_), "", d->current_block_);
    auto *cmp = new llvm::ICmpInst(
        *d->current_block_, llvm::CmpInst::ICMP_NE, cast,
        llvm::ConstantExpr::getPtrToInt(
            llvm::ConstantPointerNull::get(
                llvm::dyn_cast<llvm::PointerType>(val->getType())),
            llvm::Type::getInt64Ty(d->ctx_)));
    val = cmp;
    isInt = true;
  }
  llvm::Value *failSort = nullptr;
  llvm::Value *failPattern = nullptr;
  if (d->fail_pattern_) {
    auto failReason = getFailPattern(caseData, isInt, d->failure_block_);
    failSort = d->string_literal(failReason.first);
    failPattern = d->string_literal(failReason.second);
  }
  if (isInt) {
    auto *_switch = llvm::SwitchInst::Create(
        val, _default, cases_.size(), d->current_block_);
    for (auto &_case : caseData) {
      _switch->addCase(
          llvm::ConstantInt::get(d->ctx_, _case.second->get_literal()),
          _case.first);
    }
  } else {
    if (caseData.empty()) {
      llvm::BranchInst::Create(_default, d->current_block_);
    } else {
      llvm::Value *tagVal = d->get_tag(val);
      auto *_switch = llvm::SwitchInst::Create(
          tagVal, _default, caseData.size(), d->current_block_);
      for (auto &_case : caseData) {
        _switch->addCase(
            llvm::ConstantInt::get(
                llvm::Type::getInt32Ty(d->ctx_),
                _case.second->get_constructor()->get_tag()),
            _case.first);
      }
    }
  }
  auto *currChoiceBlock = d->choice_block_;
  d->choice_block_ = nullptr;
  auto *switchBlock = d->current_block_;
  for (auto &entry : caseData) {
    auto const &_case = *entry.second;
    if (entry.first == d->failure_block_) {
      if (d->fail_pattern_) {
        d->fail_subject_->addIncoming(ptrVal, switchBlock);
        d->fail_pattern_->addIncoming(failPattern, switchBlock);
        d->fail_sort_->addIncoming(failSort, switchBlock);
      }
      continue;
    }
    d->current_block_ = entry.first;
    if (!isInt) {
      int offset = 0;
      llvm::StructType *BlockType
          = getBlockType(d->module_, d->definition_, _case.get_constructor());
      auto *Cast = new llvm::BitCastInst(
          val, llvm::PointerType::getUnqual(BlockType), "", d->current_block_);
      kore_symbol_declaration *symbolDecl
          = d->definition_->get_symbol_declarations().at(
              _case.get_constructor()->get_name());
      llvm::Instruction *Renamed = nullptr;
      for (auto const &binding : _case.get_bindings()) {
        llvm::Value *ChildPtr = llvm::GetElementPtrInst::CreateInBounds(
            BlockType, Cast,
            {llvm::ConstantInt::get(llvm::Type::getInt64Ty(d->ctx_), 0),
             llvm::ConstantInt::get(
                 llvm::Type::getInt32Ty(d->ctx_), offset + 2)},
            "", d->current_block_);

        llvm::Value *Child = nullptr;
        auto cat = dynamic_cast<kore_composite_sort *>(
                       _case.get_constructor()->get_arguments()[offset].get())
                       ->get_category(d->definition_);

        switch (cat.cat) {
        case sort_category::Map:
        case sort_category::RangeMap:
        case sort_category::List:
        case sort_category::Set: Child = ChildPtr; break;
        default:
          Child = new llvm::LoadInst(
              getvalue_type(cat, d->module_), ChildPtr,
              binding.first.substr(0, max_name_length), d->current_block_);
          break;
        }
        auto *BlockPtr
            = llvm::PointerType::getUnqual(llvm::StructType::getTypeByName(
                d->module_->getContext(), block_struct));
        if (symbolDecl->attributes().contains(attribute_set::key::Binder)) {
          if (offset == 0) {
            Renamed = llvm::CallInst::Create(
                getOrInsertFunction(
                    d->module_, "alphaRename", BlockPtr, BlockPtr),
                Child, "renamedVar", d->current_block_);
            setDebugLoc(Renamed);
            d->store(binding, Renamed);
          } else if (offset == _case.get_bindings().size() - 1) {
            llvm::Instruction *Replaced = llvm::CallInst::Create(
                getOrInsertFunction(
                    d->module_, "replaceBinderIndex", BlockPtr, BlockPtr,
                    BlockPtr),
                {Child, Renamed}, "withUnboundIndex", d->current_block_);
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
      if (currChoiceBlock && _case.get_literal() == 1) {
        auto *PrevDepth = new llvm::LoadInst(
            llvm::Type::getInt64Ty(d->ctx_), d->choice_depth_, "",
            d->current_block_);
        auto *CurrDepth = llvm::BinaryOperator::Create(
            llvm::Instruction::Add, PrevDepth,
            llvm::ConstantInt::get(llvm::Type::getInt64Ty(d->ctx_), 1), "",
            d->current_block_);
        new llvm::StoreInst(CurrDepth, d->choice_depth_, d->current_block_);

        auto *alloc = llvm::cast<llvm::AllocaInst>(d->choice_buffer_);
        auto *ty = alloc->getAllocatedType();

        auto *zero = llvm::ConstantInt::get(llvm::Type::getInt64Ty(d->ctx_), 0);
        auto *currentElt = llvm::GetElementPtrInst::CreateInBounds(
            ty, d->choice_buffer_, {zero, CurrDepth}, "", d->current_block_);
        new llvm::StoreInst(
            llvm::BlockAddress::get(
                d->current_block_->getParent(), currChoiceBlock),
            currentElt, d->current_block_);
        d->fail_jump_->addDestination(currChoiceBlock);
      } else if (
          currChoiceBlock && _case.get_literal() == 0
          && d->has_search_results_) {
        // see https://github.com/runtimeverification/llvm-backend/issues/672
        // To summarize, if we are doing a search, and we have already found
        // at least one rule that applies of a given priority, we need to not
        // apply any rules with lower priority. However, by default the
        // decision tree tells us to try rules with lower priority after a
        // map/set choice has been exited, because we may have rules that apply
        // that can only be tried after we have tried higher-priority rules on
        // every map/set element in the collection. These lower-priority rules
        // cannot apply if we are doing a search and one rule of higher
        // priority has already been chosen, however, due to the nature of
        // rule priority. Thus, since we know due to the way decision trees
        // are compiled that this subtree contains only lower-priority rules,
        // in this case we simply jump immediately to the failure node.
        auto *loaded = new llvm::LoadInst(
            llvm::Type::getInt1Ty(d->ctx_), d->has_search_results_, "",
            d->current_block_);
        auto *newCaseBlock = llvm::BasicBlock::Create(
            d->ctx_, "hasNoSearchResults", d->current_block_->getParent());
        llvm::BranchInst::Create(
            d->failure_block_, newCaseBlock, loaded, d->current_block_);
        d->current_block_ = newCaseBlock;
      }
    }
    _case.get_child()->codegen(d);
  }
  if (defaultCase) {
    if (_default != d->failure_block_) {
      // process default also
      d->current_block_ = _default;
      defaultCase->get_child()->codegen(d);
    } else if (d->fail_pattern_) {
      d->fail_subject_->addIncoming(ptrVal, switchBlock);
      d->fail_pattern_->addIncoming(failPattern, switchBlock);
      d->fail_sort_->addIncoming(failSort, switchBlock);
    }
  }

  set_completed();
}

void make_pattern_node::codegen(decision *d) {
  if (begin_node(d, "pattern" + name_)) {
    return;
  }
  llvm::StringMap<llvm::Value *> finalSubst;
  for (auto const &use : uses_) {
    finalSubst[use.first] = d->load(use);
  }
  create_term creator(
      finalSubst, d->definition_, d->current_block_, d->module_, false);
  llvm::Value *val = creator(pattern_).first;
  d->current_block_ = creator.get_current_block();
  d->store(std::make_pair(name_, type_), val);
  child_->codegen(d);
  set_completed();
}

/*
 * The code generation step for function_nodes emits a call to addMatchFunction
 * that records why a match failed to apply in the case of side-condition calls
 * failing. Doing so requires that we store the _type_ of each argument
 * somewhere, in order that opaque block* values can be safely reinterpreted in
 * the sort that they originated from (int, bool, symbol, ...). In LLVM versions
 * < 16, we could encode this information in the LLVM type safely. However,
 * after the LLVM opaque pointer migration, we can no longer do so (as the
 * legacy types %mpz* and %block* would both be %ptr, for example). We
 * therefore define a compatibility translation between sort categories and what
 * their corresponding LLVM type _would have been_ before opaque pointers.
 */
static std::string legacy_value_type_to_string(value_type sort) {
  switch (sort.cat) {
  case sort_category::Int: return "%mpz*";
  case sort_category::Bool: return "i1";
  case sort_category::Float: return "%floating*";
  case sort_category::Symbol:
  case sort_category::Variable: return "%block*";

  // Cases below are deliberately not implemented; the return values are
  // placeholders to help with debugging only.
  case sort_category::Map: return "<map>";
  case sort_category::RangeMap: return "<rangemap>";
  case sort_category::List: return "<list>";
  case sort_category::Set: return "<set>";
  case sort_category::StringBuffer: return "<stringbuffer>";
  case sort_category::MInt: return "<mint>";
  case sort_category::Uncomputed: abort();
  }
}

void function_node::codegen(decision *d) {
  if (begin_node(d, "function" + name_)) {
    return;
  }
  std::vector<llvm::Value *> args;
  llvm::StringMap<llvm::Value *> finalSubst;
  for (auto [arg, cat] : bindings_) {
    llvm::Value *val = nullptr;
    if (arg.first.find_first_not_of("-0123456789") == std::string::npos) {
      val = llvm::ConstantInt::get(
          llvm::Type::getInt64Ty(d->ctx_), std::stoi(arg.first));
    } else {
      val = d->load(arg);
    }
    args.push_back(val);
    finalSubst[arg.first] = val;
  }
  bool isSideCondition = function_.substr(0, 15) == "side_condition_";

  if (isSideCondition) {
    proof_event p(d->definition_, d->module_);
    size_t ordinal = std::stoll(function_.substr(15));
    kore_axiom_declaration *axiom = d->definition_->get_axiom_by_ordinal(ordinal);
    d->current_block_
        = p.side_condition_event_pre(axiom, args, d->current_block_);
  }

  create_term creator(
      finalSubst, d->definition_, d->current_block_, d->module_, false);
  auto *Call = creator.create_function_call(
      function_, cat_, args, function_.substr(0, 5) == "hook_", false);
  Call->setName(name_.substr(0, max_name_length));
  d->store(std::make_pair(name_, type_), Call);

  if (isSideCondition) {
    proof_event p(d->definition_, d->module_);
    size_t ordinal = std::stoll(function_.substr(15));
    kore_axiom_declaration *axiom = d->definition_->get_axiom_by_ordinal(ordinal);
    d->current_block_
        = p.side_condition_event_post(axiom, Call, d->current_block_);
  }

  if (d->fail_pattern_) {
    std::string debugName = function_;
    if (function_.substr(0, 5) == "hook_") {
      debugName = function_.substr(5, function_.find_first_of('_', 5) - 5) + "."
                  + function_.substr(function_.find_first_of('_', 5) + 1);
    } else if (isSideCondition) {
      size_t ordinal = std::stoll(function_.substr(15));
      kore_axiom_declaration *axiom
          = d->definition_->get_axiom_by_ordinal(ordinal);
      if (axiom->attributes().contains(attribute_set::key::Label)) {
        debugName
            = axiom->attributes().get_string(attribute_set::key::Label) + ".sc";
      }
    }
    std::vector<llvm::Value *> functionArgs;
    functionArgs.push_back(d->string_literal(debugName));
    functionArgs.push_back(d->string_literal(function_));
    auto *tempAllocCall = d->ptr_term(Call);
    if (Call->getType() == llvm::Type::getInt1Ty(d->ctx_)) {
      llvm::Value *zext = new llvm::ZExtInst(
          Call, llvm::Type::getInt8Ty(d->ctx_), "", d->current_block_);
      new llvm::StoreInst(zext, tempAllocCall, d->current_block_);
    }
    functionArgs.push_back(tempAllocCall);
    for (auto i = 0U; i < args.size(); ++i) {
      auto *arg = args[i];
      auto cat = bindings_[i].second;

      auto *tempAllocArg = d->ptr_term(arg);
      if (arg->getType() == llvm::Type::getInt1Ty(d->ctx_)) {
        llvm::Value *zext = new llvm::ZExtInst(
            Call, llvm::Type::getInt8Ty(d->ctx_), "", d->current_block_);
        new llvm::StoreInst(zext, tempAllocArg, d->current_block_);
      }
      functionArgs.push_back(tempAllocArg);
      auto str = legacy_value_type_to_string(cat);
      functionArgs.push_back(d->string_literal(str));
    }
    functionArgs.push_back(
        llvm::ConstantPointerNull::get(llvm::Type::getInt8PtrTy(d->ctx_)));

    auto *call = llvm::CallInst::Create(
        getOrInsertFunction(
            d->module_, "addMatchFunction",
            llvm::FunctionType::get(
                llvm::Type::getVoidTy(d->ctx_),
                {llvm::Type::getInt8PtrTy(d->ctx_),
                 llvm::Type::getInt8PtrTy(d->ctx_),
                 llvm::Type::getInt8PtrTy(d->ctx_)},
                true)),
        functionArgs, "", d->current_block_);
    setDebugLoc(call);
  }
  child_->codegen(d);
  set_completed();
}

void make_iterator_node::codegen(decision *d) {
  if (begin_node(d, "new_iterator" + name_)) {
    return;
  }
  std::vector<llvm::Value *> args;
  std::vector<llvm::Type *> types;
  llvm::Value *arg = d->load(std::make_pair(collection_, collection_type_));
  args.push_back(arg);
  types.push_back(arg->getType());
  llvm::Type *sretType
      = llvm::StructType::getTypeByName(d->module_->getContext(), "iter");
  llvm::Value *AllocSret
      = allocateTerm(sretType, d->current_block_, "koreAllocAlwaysGC");
  AllocSret->setName(name_.substr(0, max_name_length));
  args.insert(args.begin(), AllocSret);
  types.insert(types.begin(), AllocSret->getType());

  llvm::FunctionType *funcType = llvm::FunctionType::get(
      llvm::Type::getVoidTy(d->module_->getContext()), types, false);
  llvm::Function *func = getOrInsertFunction(d->module_, hook_name_, funcType);
  auto *call = llvm::CallInst::Create(func, args, "", d->current_block_);
  setDebugLoc(call);
  llvm::Attribute sretAttr
      = llvm::Attribute::get(d->ctx_, llvm::Attribute::StructRet, sretType);
  func->arg_begin()->addAttr(sretAttr);
  call->addParamAttr(0, sretAttr);
  d->store(std::make_pair(name_, type_), AllocSret);
  child_->codegen(d);
  set_completed();
}

void iter_next_node::codegen(decision *d) {
  if (begin_node(d, "choice" + binding_)) {
    return;
  }
  d->choice_block_ = d->current_block_;
  llvm::Value *arg = d->load(std::make_pair(iterator_, iterator_type_));

  llvm::FunctionType *funcType = llvm::FunctionType::get(
      getvalue_type({sort_category::Symbol, 0}, d->module_), {arg->getType()},
      false);
  llvm::Function *func = getOrInsertFunction(d->module_, hook_name_, funcType);
  auto *Call = llvm::CallInst::Create(
      func, {arg}, binding_.substr(0, max_name_length), d->current_block_);
  setDebugLoc(Call);
  d->store(std::make_pair(binding_, binding_type_), Call);
  child_->codegen(d);
  d->choice_block_ = nullptr;
  set_completed();
}

void leaf_node::codegen(decision *d) {
  if (begin_node(d, name_)) {
    return;
  }
  if (d->fail_pattern_) {
    auto *call = llvm::CallInst::Create(
        getOrInsertFunction(
            d->module_, "addMatchSuccess", llvm::Type::getVoidTy(d->ctx_)),
        {}, "", d->current_block_);
    setDebugLoc(call);
    llvm::ReturnInst::Create(d->ctx_, d->current_block_);
    set_completed();
    return;
  }

  std::vector<llvm::Value *> args;
  std::vector<llvm::Type *> types;
  for (auto const &arg : bindings_) {
    auto *val = d->load(arg);
    args.push_back(val);
    types.push_back(val->getType());
  }
  auto *type = getParamType(d->cat_, d->module_);

  auto *applyRule = getOrInsertFunction(
      d->module_, name_, llvm::FunctionType::get(type, types, false));

  // We are generating code for a function with name beginning apply_rule_\d+; to
  // retrieve the corresponding ordinal we drop the apply_rule_ prefix.
  auto ordinal = std::stoll(name_.substr(11));
  auto arity = applyRule->arg_end() - applyRule->arg_begin();
  auto *axiom = d->definition_->get_axiom_by_ordinal(ordinal);

  auto vars = std::map<std::string, kore_variable_pattern *>{};
  for (kore_pattern *lhs : axiom->get_left_hand_side()) {
    lhs->mark_variables(vars);
  }
  axiom->get_right_hand_side()->mark_variables(vars);

  auto subst = llvm::StringMap<llvm::Value *>{};
  auto i = 0;
  for (auto iter = vars.begin(); iter != vars.end(); ++iter, ++i) {
    subst[iter->first] = args[i];
  }

  d->current_block_
      = proof_event(d->definition_, d->module_)
            .rewrite_event_pre(axiom, arity, vars, subst, d->current_block_);

  auto *Call = llvm::CallInst::Create(applyRule, args, "", d->current_block_);
  setDebugLoc(Call);
  Call->setCallingConv(llvm::CallingConv::Tail);

  if (child_ == nullptr) {
    llvm::ReturnInst::Create(d->ctx_, Call, d->current_block_);
  } else {
    new llvm::StoreInst(
        llvm::ConstantInt::getTrue(d->ctx_), d->has_search_results_,
        d->current_block_);
    auto *Call2 = llvm::CallInst::Create(
        getOrInsertFunction(
            d->module_, "addSearchResult",
            llvm::FunctionType::get(
                llvm::Type::getVoidTy(d->ctx_), {type}, false)),
        {Call}, "", d->current_block_);
    setDebugLoc(Call2);
    if (child_ != fail_node::get()) {
      child_->codegen(d);
    } else {
      llvm::BranchInst::Create(d->failure_block_, d->current_block_);
    }
  }
  set_completed();
}

llvm::Value *decision::get_tag(llvm::Value *val) {
  auto *res = llvm::CallInst::Create(
      getOrInsertFunction(
          module_, "getTag", llvm::Type::getInt32Ty(ctx_),
          getvalue_type({sort_category::Symbol, 0}, module_)),
      val, "tag", current_block_);
  setDebugLoc(res);
  return res;
}

llvm::AllocaInst *decision::decl(var_type const &name) {
  auto *sym = new llvm::AllocaInst(
      name.second, 0, "",
      this->current_block_->getParent()->getEntryBlock().getFirstNonPHI());
  this->symbols_[name] = sym;
  return sym;
}

llvm::Value *decision::load(var_type const &name) {
  if (name.first.empty()) {
    llvm::Type *ty = name.second;
    if (ty->isPointerTy()) {
      auto *ptr_ty = (llvm::PointerType *)ty;
      return llvm::ConstantPointerNull::get(ptr_ty);
    }
    if (ty->isIntegerTy()) {
      auto *int_ty = (llvm::IntegerType *)ty;
      return llvm::ConstantInt::get(int_ty, 0);
    }
    assert(false && "Unbound variable on LHS is neither pointer nor integral");
  }
  auto *sym = this->symbols_[name];
  if (!sym) {
    sym = this->decl(name);
  }
  auto *alloc = llvm::cast<llvm::AllocaInst>(sym);
  auto *ty = alloc->getAllocatedType();
  return new llvm::LoadInst(
      ty, sym, name.first.substr(0, max_name_length), this->current_block_);
}

void decision::store(var_type const &name, llvm::Value *val) {
  auto *sym = this->symbols_[name];
  if (!sym) {
    sym = this->decl(name);
  }
  new llvm::StoreInst(val, sym, this->current_block_);
}

llvm::Constant *decision::string_literal(std::string const &str) {
  auto *Str = llvm::ConstantDataArray::getString(ctx_, str, true);
  auto *global = module_->getOrInsertGlobal("str_lit_" + str, Str->getType());
  auto *globalVar = llvm::cast<llvm::GlobalVariable>(global);
  if (!globalVar->hasInitializer()) {
    globalVar->setInitializer(Str);
  }
  llvm::Constant *zero
      = llvm::ConstantInt::get(llvm::Type::getInt64Ty(ctx_), 0);
  auto indices = std::vector<llvm::Constant *>{zero, zero};
  auto *Ptr = llvm::ConstantExpr::getInBoundsGetElementPtr(
      Str->getType(), globalVar, indices);
  return Ptr;
}

static void initChoiceBuffer(
    decision_node *dt, llvm::Module *module, llvm::BasicBlock *block,
    llvm::BasicBlock *stuck, llvm::BasicBlock *fail,
    llvm::AllocaInst **choice_buffer_out, llvm::AllocaInst **choice_depth_out,
    llvm::IndirectBrInst **jump_out) {
  std::unordered_set<leaf_node *> leaves;
  dt->preprocess(leaves);
  auto *ty = llvm::ArrayType::get(
      llvm::Type::getInt8PtrTy(module->getContext()), dt->get_choice_depth() + 1);
  auto *choiceBuffer = new llvm::AllocaInst(ty, 0, "choiceBuffer", block);
  auto *choiceDepth = new llvm::AllocaInst(
      llvm::Type::getInt64Ty(module->getContext()), 0, "choiceDepth", block);
  auto *zero
      = llvm::ConstantInt::get(llvm::Type::getInt64Ty(module->getContext()), 0);
  new llvm::StoreInst(zero, choiceDepth, block);
  auto *firstElt = llvm::GetElementPtrInst::CreateInBounds(
      ty, choiceBuffer, {zero, zero}, "", block);
  new llvm::StoreInst(
      llvm::BlockAddress::get(block->getParent(), stuck), firstElt, block);

  auto *currDepth = new llvm::LoadInst(
      llvm::Type::getInt64Ty(module->getContext()), choiceDepth, "", fail);
  auto *currentElt = llvm::GetElementPtrInst::CreateInBounds(
      ty, choiceBuffer, {zero, currDepth}, "", fail);
  auto *failAddress
      = new llvm::LoadInst(ty->getElementType(), currentElt, "", fail);
  auto *newDepth = llvm::BinaryOperator::Create(
      llvm::Instruction::Sub, currDepth,
      llvm::ConstantInt::get(llvm::Type::getInt64Ty(module->getContext()), 1),
      "", fail);
  new llvm::StoreInst(newDepth, choiceDepth, fail);
  llvm::IndirectBrInst *jump
      = llvm::IndirectBrInst::Create(failAddress, 1, fail);
  jump->addDestination(stuck);
  *choice_buffer_out = choiceBuffer;
  *choice_depth_out = choiceDepth;
  *jump_out = jump;
}

void makeEvalOrAnywhereFunction(
    kore_symbol *function, kore_definition *definition, llvm::Module *module,
    decision_node *dt,
    void (*add_stuck)(
        llvm::BasicBlock *, llvm::Module *, kore_symbol *, decision &,
        kore_definition *)) {
  auto returnSort
      = dynamic_cast<kore_composite_sort *>(function->get_sort().get())
            ->get_category(definition);
  auto *returnType = getParamType(returnSort, module);
  auto *debugReturnType
      = getDebugType(returnSort, ast_to_string(*function->get_sort()));
  std::vector<llvm::Type *> args;
  std::vector<llvm::Metadata *> debugArgs;
  std::vector<value_type> cats;
  for (auto const &sort : function->get_arguments()) {
    auto cat = dynamic_cast<kore_composite_sort *>(sort.get())
                   ->get_category(definition);
    debugArgs.push_back(getDebugType(cat, ast_to_string(*sort)));
    switch (cat.cat) {
    case sort_category::Map:
    case sort_category::RangeMap:
    case sort_category::List:
    case sort_category::Set:
      args.push_back(llvm::PointerType::getUnqual(getvalue_type(cat, module)));
      cats.push_back(cat);
      break;
    default:
      args.push_back(getvalue_type(cat, module));
      cats.push_back(cat);
      break;
    }
  }
  llvm::FunctionType *funcType
      = llvm::FunctionType::get(returnType, args, false);
  std::string name = fmt::format("eval_{}", ast_to_string(*function, 0, false));
  llvm::Function *matchFunc = getOrInsertFunction(module, name, funcType);
  [[maybe_unused]] kore_symbol_declaration *symbolDecl
      = definition->get_symbol_declarations().at(function->get_name());
  initDebugAxiom(symbolDecl->attributes());
  initDebugFunction(
      function->get_name(), name,
      getDebugFunctionType(debugReturnType, debugArgs), definition, matchFunc);
  matchFunc->setCallingConv(llvm::CallingConv::Tail);
  llvm::BasicBlock *block
      = llvm::BasicBlock::Create(module->getContext(), "entry", matchFunc);
  llvm::BasicBlock *stuck
      = llvm::BasicBlock::Create(module->getContext(), "stuck", matchFunc);
  llvm::BasicBlock *fail
      = llvm::BasicBlock::Create(module->getContext(), "fail", matchFunc);

  llvm::AllocaInst *choiceBuffer = nullptr;
  llvm::AllocaInst *choiceDepth = nullptr;
  llvm::IndirectBrInst *jump = nullptr;
  initChoiceBuffer(
      dt, module, block, stuck, fail, &choiceBuffer, &choiceDepth, &jump);

  int i = 0;
  decision codegen(
      definition, block, fail, jump, choiceBuffer, choiceDepth, module,
      returnSort, nullptr, nullptr, nullptr, nullptr);
  for (auto *val = matchFunc->arg_begin(); val != matchFunc->arg_end();
       ++val, ++i) {
    val->setName("_" + std::to_string(i + 1));
    codegen.store(std::make_pair(val->getName().str(), val->getType()), val);
    initDebugParam(
        matchFunc, i, val->getName().str(), cats[i],
        ast_to_string(*function->get_arguments()[i]));
  }
  add_stuck(stuck, module, function, codegen, definition);

  codegen(dt);
}

void abortWhenStuck(
    llvm::BasicBlock *current_block, llvm::Module *module, kore_symbol *symbol,
    decision &codegen, kore_definition *d) {
  auto &Ctx = module->getContext();
  symbol = d->get_all_symbols().at(ast_to_string(*symbol));
  auto *BlockType = getBlockType(module, d, symbol);
  llvm::Value *Ptr = nullptr;
  auto *BlockPtr = llvm::PointerType::getUnqual(
      llvm::StructType::getTypeByName(module->getContext(), block_struct));
  if (symbol->get_arguments().empty()) {
    Ptr = llvm::ConstantExpr::getIntToPtr(
        llvm::ConstantInt::get(
            llvm::Type::getInt64Ty(Ctx),
            ((uint64_t)symbol->get_tag() << 32 | 1)),
        getvalue_type({sort_category::Symbol, 0}, module));
  } else {
    llvm::Value *BlockHeader = getBlockHeader(module, d, symbol, BlockType);
    llvm::Value *Block = allocateTerm(BlockType, current_block);
    llvm::Value *BlockHeaderPtr = llvm::GetElementPtrInst::CreateInBounds(
        BlockType, Block,
        {llvm::ConstantInt::get(llvm::Type::getInt64Ty(Ctx), 0),
         llvm::ConstantInt::get(llvm::Type::getInt32Ty(Ctx), 0)},
        symbol->get_name(), current_block);
    new llvm::StoreInst(BlockHeader, BlockHeaderPtr, current_block);
    for (int idx = 0; idx < symbol->get_arguments().size(); idx++) {
      auto cat = dynamic_cast<kore_composite_sort *>(
                     symbol->get_arguments()[idx].get())
                     ->get_category(d);
      auto *type = getParamType(cat, module);
      llvm::Value *ChildValue
          = codegen.load(std::make_pair("_" + std::to_string(idx + 1), type));
      llvm::Value *ChildPtr = llvm::GetElementPtrInst::CreateInBounds(
          BlockType, Block,
          {llvm::ConstantInt::get(llvm::Type::getInt64Ty(Ctx), 0),
           llvm::ConstantInt::get(llvm::Type::getInt32Ty(Ctx), idx + 2)},
          "", current_block);
      if (isCollectionSort(cat)) {
        ChildValue = new llvm::LoadInst(
            getArgType(cat, module), ChildValue, "", current_block);
      }
      new llvm::StoreInst(ChildValue, ChildPtr, current_block);
    }
    Ptr = new llvm::BitCastInst(Block, BlockPtr, "", current_block);
  }
  llvm::CallInst::Create(
      getOrInsertFunction(
          module, "finish_rewriting", llvm::Type::getVoidTy(Ctx), BlockPtr,
          llvm::Type::getInt1Ty(Ctx)),
      {Ptr, llvm::ConstantInt::getTrue(Ctx)}, "", current_block);
  new llvm::UnreachableInst(Ctx, current_block);
}

void makeEvalFunction(
    kore_symbol *function, kore_definition *definition, llvm::Module *module,
    decision_node *dt) {
  makeEvalOrAnywhereFunction(function, definition, module, dt, abortWhenStuck);
}

void addOwise(
    llvm::BasicBlock *stuck, llvm::Module *module, kore_symbol *symbol,
    decision &codegen, kore_definition *d) {
  llvm::StringMap<llvm::Value *> finalSubst;
  ptr<kore_composite_pattern> pat = kore_composite_pattern::create(symbol);
  for (int i = 0; i < symbol->get_arguments().size(); i++) {
    auto cat
        = dynamic_cast<kore_composite_sort *>(symbol->get_arguments()[i].get())
              ->get_category(d);
    auto *type = getParamType(cat, module);

    std::string name = "_" + std::to_string(i + 1);
    finalSubst[name] = codegen.load(std::make_pair(name, type));

    auto var = kore_variable_pattern::create(name, symbol->get_arguments()[i]);
    pat->add_argument(std::move(var));
  }
  create_term creator = create_term(finalSubst, d, stuck, module, true);
  llvm::Value *retval = creator(pat.get()).first;

  auto returnSort = dynamic_cast<kore_composite_sort *>(symbol->get_sort().get())
                        ->get_category(d);
  if (isCollectionSort(returnSort)) {
    auto *tempAlloc = allocateTerm(
        retval->getType(), creator.get_current_block(), "koreAllocAlwaysGC");
    new llvm::StoreInst(retval, tempAlloc, creator.get_current_block());
    retval = tempAlloc;
  }

  llvm::ReturnInst::Create(
      module->getContext(), retval, creator.get_current_block());
}

void makeAnywhereFunction(
    kore_symbol *function, kore_definition *definition, llvm::Module *module,
    decision_node *dt) {
  makeEvalOrAnywhereFunction(function, definition, module, dt, addOwise);
}

std::pair<std::vector<llvm::Value *>, llvm::BasicBlock *> stepFunctionHeader(
    unsigned ordinal, llvm::Module *module, kore_definition *definition,
    llvm::BasicBlock *block, llvm::BasicBlock *stuck,
    std::vector<llvm::Value *> args, std::vector<value_type> const &types) {
  auto *finished = getOrInsertFunction(
      module, "finished_rewriting",
      llvm::FunctionType::get(
          llvm::Type::getInt1Ty(module->getContext()), {}, false));
  auto *isFinished = llvm::CallInst::Create(finished, {}, "", block);
  auto *checkCollect = llvm::BasicBlock::Create(
      module->getContext(), "checkCollect", block->getParent());
  llvm::BranchInst::Create(stuck, checkCollect, isFinished, block);

  auto *collection = getOrInsertFunction(
      module, "is_collection",
      llvm::FunctionType::get(
          llvm::Type::getInt1Ty(module->getContext()), {}, false));
  auto *isCollection = llvm::CallInst::Create(collection, {}, "", checkCollect);
  setDebugLoc(isCollection);
  auto *collect = llvm::BasicBlock::Create(
      module->getContext(), "isCollect", block->getParent());
  auto *merge = llvm::BasicBlock::Create(
      module->getContext(), "step", block->getParent());
  llvm::BranchInst::Create(collect, merge, isCollection, checkCollect);

  unsigned nroots = 0;
  unsigned i = 0;
  std::vector<llvm::Type *> ptrTypes;
  std::vector<llvm::Value *> roots;
  for (auto type : types) {
    switch (type.cat) {
    case sort_category::Map:
    case sort_category::RangeMap:
    case sort_category::List:
    case sort_category::Set:
      nroots++;
      ptrTypes.push_back(
          llvm::PointerType::getUnqual(getvalue_type(type, module)));
      roots.push_back(args[i]);
      break;
    case sort_category::Int:
    case sort_category::Float:
    case sort_category::StringBuffer:
    case sort_category::Symbol:
    case sort_category::Variable:
      nroots++;
      ptrTypes.push_back(getvalue_type(type, module));
      roots.push_back(args[i]);
      break;
    case sort_category::Bool:
    case sort_category::MInt: break;
    case sort_category::Uncomputed: abort();
    }
    i++;
  }
  auto *root_ty = llvm::ArrayType::get(
      llvm::Type::getInt8PtrTy(module->getContext()), 256);
  auto *arr = module->getOrInsertGlobal("gc_roots", root_ty);
  std::vector<std::pair<llvm::Value *, llvm::Type *>> rootPtrs;
  for (unsigned i = 0; i < nroots; i++) {
    auto *ptr = llvm::GetElementPtrInst::CreateInBounds(
        root_ty, arr,
        {llvm::ConstantInt::get(
             llvm::Type::getInt64Ty(module->getContext()), 0),
         llvm::ConstantInt::get(
             llvm::Type::getInt64Ty(module->getContext()), i)},
        "", collect);
    auto *casted = new llvm::BitCastInst(
        ptr, llvm::PointerType::getUnqual(ptrTypes[i]), "", collect);
    new llvm::StoreInst(roots[i], casted, collect);
    rootPtrs.emplace_back(casted, ptrTypes[i]);
  }
  std::vector<llvm::Constant *> elements;
  i = 0;
  for (auto cat : types) {
    switch (cat.cat) {
    case sort_category::Map:
    case sort_category::RangeMap:
    case sort_category::List:
    case sort_category::Set:
    case sort_category::StringBuffer:
    case sort_category::Symbol:
    case sort_category::Variable:
    case sort_category::Int:
    case sort_category::Float:
      elements.push_back(llvm::ConstantStruct::get(
          llvm::StructType::getTypeByName(
              module->getContext(), layoutitem_struct),
          llvm::ConstantInt::get(
              llvm::Type::getInt64Ty(module->getContext()), i++ * 8),
          llvm::ConstantInt::get(
              llvm::Type::getInt16Ty(module->getContext()),
              (int)cat.cat + cat.bits)));
      break;
    case sort_category::Bool:
    case sort_category::MInt: break;
    case sort_category::Uncomputed: abort();
    }
  }
  auto *layoutArr = llvm::ConstantArray::get(
      llvm::ArrayType::get(
          llvm::StructType::getTypeByName(
              module->getContext(), layoutitem_struct),
          elements.size()),
      elements);
  auto *layout = module->getOrInsertGlobal(
      "layout_item_rule_" + std::to_string(ordinal), layoutArr->getType());
  auto *globalVar = llvm::cast<llvm::GlobalVariable>(layout);
  if (!globalVar->hasInitializer()) {
    globalVar->setInitializer(layoutArr);
  }
  auto *ptrTy = llvm::PointerType::getUnqual(llvm::ArrayType::get(
      llvm::StructType::getTypeByName(module->getContext(), layoutitem_struct),
      0));
  auto *koreCollect = getOrInsertFunction(
      module, "koreCollect",
      llvm::FunctionType::get(
          llvm::Type::getVoidTy(module->getContext()),
          {arr->getType(), llvm::Type::getInt8Ty(module->getContext()), ptrTy},
          false));
  auto *call = llvm::CallInst::Create(
      koreCollect,
      {arr,
       llvm::ConstantInt::get(
           llvm::Type::getInt8Ty(module->getContext()), nroots),
       llvm::ConstantExpr::getBitCast(layout, ptrTy)},
      "", collect);
  setDebugLoc(call);
  i = 0;
  std::vector<llvm::Value *> phis;
  for (auto [ptr, pointee_ty] : rootPtrs) {
    auto *loaded = new llvm::LoadInst(pointee_ty, ptr, "", collect);
    auto *phi = llvm::PHINode::Create(loaded->getType(), 2, "phi", merge);
    phi->addIncoming(loaded, collect);
    phi->addIncoming(roots[i++], checkCollect);
    phis.push_back(phi);
  }
  llvm::BranchInst::Create(merge, collect);
  i = 0;
  unsigned rootIdx = 0;
  std::vector<llvm::Value *> results;
  for (auto type : types) {
    switch (type.cat) {
    case sort_category::Map:
    case sort_category::RangeMap:
    case sort_category::List:
    case sort_category::Set:
    case sort_category::StringBuffer:
    case sort_category::Symbol:
    case sort_category::Variable:
    case sort_category::Int:
    case sort_category::Float: results.push_back(phis[rootIdx++]); break;
    default: results.push_back(args[i]);
    }
    i++;
  }
  return std::make_pair(results, merge);
}

void makeStepFunction(
    kore_definition *definition, llvm::Module *module, decision_node *dt,
    bool search) {
  auto *blockType = getvalue_type({sort_category::Symbol, 0}, module);
  auto *debugType
      = getDebugType({sort_category::Symbol, 0}, "SortGeneratedTopCell{}");
  llvm::FunctionType *funcType = nullptr;
  std::string name;
  if (search) {
    name = "stepAll";
    funcType = llvm::FunctionType::get(
        llvm::Type::getVoidTy(module->getContext()), {blockType}, false);
  } else {
    name = "k_step";
    funcType = llvm::FunctionType::get(blockType, {blockType}, false);
  }
  llvm::Function *matchFunc = getOrInsertFunction(module, name, funcType);
  resetDebugLoc();
  if (search) {
    initDebugFunction(
        name, name, getDebugFunctionType(getVoidDebugType(), {debugType}),
        definition, matchFunc);
  } else {
    initDebugFunction(
        name, name, getDebugFunctionType(debugType, {debugType}), definition,
        matchFunc);
  }
  matchFunc->setCallingConv(llvm::CallingConv::Tail);
  auto *val = matchFunc->arg_begin();
  llvm::BasicBlock *block
      = llvm::BasicBlock::Create(module->getContext(), "entry", matchFunc);
  llvm::BasicBlock *stuck
      = llvm::BasicBlock::Create(module->getContext(), "stuck", matchFunc);
  llvm::BasicBlock *pre_stuck
      = llvm::BasicBlock::Create(module->getContext(), "pre_stuck", matchFunc);
  llvm::BasicBlock *fail
      = llvm::BasicBlock::Create(module->getContext(), "fail", matchFunc);

  llvm::AllocaInst *choiceBuffer = nullptr;
  llvm::AllocaInst *choiceDepth = nullptr;
  llvm::IndirectBrInst *jump = nullptr;

  initChoiceBuffer(
      dt, module, block, pre_stuck, fail, &choiceBuffer, &choiceDepth, &jump);

  llvm::AllocaInst *HasSearchResults = nullptr;
  if (search) {
    HasSearchResults = new llvm::AllocaInst(
        llvm::Type::getInt1Ty(module->getContext()), 0, "hasSearchResults",
        block);
    new llvm::StoreInst(
        llvm::ConstantInt::getFalse(module->getContext()), HasSearchResults,
        block);
  }
  initDebugParam(
      matchFunc, 0, "subject", {sort_category::Symbol, 0},
      "SortGeneratedTopCell{}");
  llvm::BranchInst::Create(stuck, pre_stuck);
  auto result = stepFunctionHeader(
      0, module, definition, block, stuck, {val}, {{sort_category::Symbol, 0}});
  auto *collectedVal = result.first[0];
  collectedVal->setName("_1");
  decision codegen(
      definition, result.second, fail, jump, choiceBuffer, choiceDepth, module,
      {sort_category::Symbol, 0}, nullptr, nullptr, nullptr, HasSearchResults);
  codegen.store(
      std::make_pair(collectedVal->getName().str(), collectedVal->getType()),
      collectedVal);
  if (search) {
    llvm::ReturnInst::Create(module->getContext(), stuck);
  } else {
    auto *phi
        = llvm::PHINode::Create(collectedVal->getType(), 2, "phi_1", stuck);
    phi->addIncoming(val, block);
    phi->addIncoming(collectedVal, pre_stuck);
    llvm::ReturnInst::Create(module->getContext(), phi, stuck);
  }

  codegen(dt);
}

void makeMatchReasonFunctionWrapper(
    kore_definition *definition, llvm::Module *module,
    kore_axiom_declaration *axiom, std::string const &name) {
  auto *blockType = getvalue_type({sort_category::Symbol, 0}, module);
  llvm::FunctionType *funcType = llvm::FunctionType::get(
      llvm::Type::getVoidTy(module->getContext()), {blockType}, false);
  std::string wrapperName = "match_" + std::to_string(axiom->get_ordinal());
  llvm::Function *matchFunc
      = getOrInsertFunction(module, wrapperName, funcType);
  std::string debugName = name;
  if (axiom->attributes().contains(attribute_set::key::Label)) {
    debugName = axiom->attributes().get_string(attribute_set::key::Label)
                + "_tailcc_" + ".match";
  }
  auto *debugType
      = getDebugType({sort_category::Symbol, 0}, "SortGeneratedTopCell{}");
  resetDebugLoc();
  initDebugFunction(
      debugName, debugName,
      getDebugFunctionType(getVoidDebugType(), {debugType}), definition,
      matchFunc);
  matchFunc->setCallingConv(llvm::CallingConv::Tail);
  llvm::BasicBlock *entry
      = llvm::BasicBlock::Create(module->getContext(), "entry", matchFunc);

  auto *ci = module->getFunction(name);
  auto *call = llvm::CallInst::Create(ci, matchFunc->getArg(0), "", entry);
  setDebugLoc(call);

  llvm::ReturnInst::Create(module->getContext(), entry);
}

void makeMatchReasonFunction(
    kore_definition *definition, llvm::Module *module,
    kore_axiom_declaration *axiom, decision_node *dt) {
  auto *blockType = getvalue_type({sort_category::Symbol, 0}, module);
  llvm::FunctionType *funcType = llvm::FunctionType::get(
      llvm::Type::getVoidTy(module->getContext()), {blockType}, false);
  std::string name = "intern_match_" + std::to_string(axiom->get_ordinal());
  llvm::Function *matchFunc = getOrInsertFunction(module, name, funcType);
  std::string debugName = name;
  if (axiom->attributes().contains(attribute_set::key::Label)) {
    debugName
        = axiom->attributes().get_string(attribute_set::key::Label) + ".match";
  }
  auto *debugType
      = getDebugType({sort_category::Symbol, 0}, "SortGeneratedTopCell{}");
  resetDebugLoc();
  initDebugFunction(
      debugName, debugName,
      getDebugFunctionType(getVoidDebugType(), {debugType}), definition,
      matchFunc);
  auto *val = matchFunc->arg_begin();
  llvm::BasicBlock *block
      = llvm::BasicBlock::Create(module->getContext(), "entry", matchFunc);
  llvm::BasicBlock *stuck
      = llvm::BasicBlock::Create(module->getContext(), "stuck", matchFunc);
  llvm::BasicBlock *pre_stuck
      = llvm::BasicBlock::Create(module->getContext(), "pre_stuck", matchFunc);
  llvm::BasicBlock *fail
      = llvm::BasicBlock::Create(module->getContext(), "fail", matchFunc);
  llvm::PHINode *FailSubject = llvm::PHINode::Create(
      llvm::Type::getInt8PtrTy(module->getContext()), 0, "subject", fail);
  llvm::PHINode *FailPattern = llvm::PHINode::Create(
      llvm::Type::getInt8PtrTy(module->getContext()), 0, "pattern", fail);
  llvm::PHINode *FailSort = llvm::PHINode::Create(
      llvm::Type::getInt8PtrTy(module->getContext()), 0, "sort", fail);
  auto *call = llvm::CallInst::Create(
      getOrInsertFunction(
          module, "addMatchFailReason",
          llvm::FunctionType::get(
              llvm::Type::getVoidTy(module->getContext()),
              {FailSubject->getType(), FailPattern->getType(),
               FailSort->getType()},
              false)),
      {FailSubject, FailPattern, FailSort}, "", fail);
  setDebugLoc(call);

  llvm::AllocaInst *choiceBuffer = nullptr;
  llvm::AllocaInst *choiceDepth = nullptr;
  llvm::IndirectBrInst *jump = nullptr;
  initChoiceBuffer(
      dt, module, block, pre_stuck, fail, &choiceBuffer, &choiceDepth, &jump);

  initDebugParam(
      matchFunc, 0, "subject", {sort_category::Symbol, 0},
      "SortGeneratedTopCell{}");
  llvm::BranchInst::Create(stuck, pre_stuck);
  val->setName("_1");
  decision codegen(
      definition, block, fail, jump, choiceBuffer, choiceDepth, module,
      {sort_category::Symbol, 0}, FailSubject, FailPattern, FailSort, nullptr);
  codegen.store(std::make_pair(val->getName().str(), val->getType()), val);
  llvm::ReturnInst::Create(module->getContext(), stuck);

  codegen(dt);
  makeMatchReasonFunctionWrapper(definition, module, axiom, name);
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
kore_pattern *makePartialTerm(
    kore_pattern *term, std::set<std::string> const &occurrences,
    std::string const &occurrence) {
  if (occurrences.contains(occurrence)) {
    return kore_variable_pattern::create(occurrence, term->get_sort()).release();
  }
  if (auto *pat = dynamic_cast<kore_composite_pattern *>(term)) {
    if (pat->get_constructor()->get_name() == "\\dv") {
      return term;
    }
    ptr<kore_composite_pattern> result
        = kore_composite_pattern::create(pat->get_constructor());
    for (unsigned i = 0; i < pat->get_arguments().size(); i++) {
      result->add_argument(ptr<kore_pattern>(makePartialTerm(
          dynamic_cast<kore_pattern *>(pat->get_arguments()[i].get()),
          occurrences, "_" + std::to_string(i) + occurrence)));
    }
    return result.release();
  }
  abort();
}

void makeStepFunction(
    kore_axiom_declaration *axiom, kore_definition *definition,
    llvm::Module *module, partial_step res) {
  auto *blockType = getvalue_type({sort_category::Symbol, 0}, module);
  std::vector<llvm::Type *> argTypes;
  std::vector<llvm::Metadata *> debugTypes;
  for (auto const &res : res.residuals) {
    auto *argSort
        = dynamic_cast<kore_composite_sort *>(res.pattern->get_sort().get());
    auto cat = argSort->get_category(definition);
    debugTypes.push_back(getDebugType(cat, ast_to_string(*argSort)));
    switch (cat.cat) {
    case sort_category::Map:
    case sort_category::RangeMap:
    case sort_category::List:
    case sort_category::Set:
      argTypes.push_back(
          llvm::PointerType::getUnqual(getvalue_type(cat, module)));
      break;
    default: argTypes.push_back(getvalue_type(cat, module)); break;
    }
  }
  auto *blockDebugType
      = getDebugType({sort_category::Symbol, 0}, "SortGeneratedTopCell{}");
  llvm::FunctionType *funcType
      = llvm::FunctionType::get(blockType, argTypes, false);
  std::string name = "step_" + std::to_string(axiom->get_ordinal());
  llvm::Function *matchFunc = getOrInsertFunction(module, name, funcType);
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

  llvm::AllocaInst *choiceBuffer = nullptr;
  llvm::AllocaInst *choiceDepth = nullptr;
  llvm::IndirectBrInst *jump = nullptr;
  initChoiceBuffer(
      res.dt, module, block, pre_stuck, fail, &choiceBuffer, &choiceDepth,
      &jump);

  llvm::BranchInst::Create(stuck, pre_stuck);
  std::vector<llvm::PHINode *> phis;
  int i = 0;
  std::vector<llvm::Value *> args;
  std::vector<value_type> types;
  for (auto *val = matchFunc->arg_begin(); val != matchFunc->arg_end();
       ++val, ++i) {
    args.push_back(val);
    auto *phi = llvm::PHINode::Create(
        val->getType(), 2, "phi" + res.residuals[i].occurrence, stuck);
    phi->addIncoming(val, block);
    phis.push_back(phi);
    auto sort = res.residuals[i].pattern->get_sort();
    auto cat = dynamic_cast<kore_composite_sort *>(sort.get())
                   ->get_category(definition);
    types.push_back(cat);
    initDebugParam(
        matchFunc, i, "_" + std::to_string(i + 1), cat, ast_to_string(*sort));
  }
  auto header = stepFunctionHeader(
      axiom->get_ordinal(), module, definition, block, stuck, args, types);
  i = 0;
  decision codegen(
      definition, header.second, fail, jump, choiceBuffer, choiceDepth, module,
      {sort_category::Symbol, 0}, nullptr, nullptr, nullptr, nullptr);
  for (auto *val : header.first) {
    val->setName(res.residuals[i].occurrence.substr(0, max_name_length));
    codegen.store(std::make_pair(val->getName().str(), val->getType()), val);
    stuckSubst.insert({val->getName(), phis[i]});
    phis[i++]->addIncoming(val, pre_stuck);
  }
  std::set<std::string> occurrences;
  for (auto const &residual : res.residuals) {
    occurrences.insert(residual.occurrence);
  }
  kore_pattern *partialTerm = makePartialTerm(
      dynamic_cast<kore_pattern *>(axiom->get_right_hand_side()), occurrences,
      "_1");
  create_term creator(stuckSubst, definition, stuck, module, false);
  llvm::Value *retval = creator(partialTerm).first;
  llvm::ReturnInst::Create(
      module->getContext(), retval, creator.get_current_block());

  codegen(res.dt);
}
} // namespace kllvm
