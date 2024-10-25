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
              val, llvm::PointerType::getUnqual(ctx_), "", current_block_),
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
    val = allocate_term(val->getType(), current_block_, "kore_alloc_always_gc");
  }
  return new llvm::BitCastInst(
      val, llvm::PointerType::getUnqual(ctx_), "", current_block_);
}

bool decision_node::begin_node(decision *d, std::string const &name) {
  if (is_completed()) {
    llvm::BranchInst::Create(cached_code_, d->current_block_);
    return true;
  }
  auto *block = llvm::BasicBlock::Create(
      d->ctx_, name.substr(0, max_name_length), d->current_block_->getParent());
  cached_code_ = block;
  llvm::BranchInst::Create(block, d->current_block_);
  d->current_block_ = block;
  return false;
}

static std::pair<std::string, std::string>
get_fail_pattern(decision_case const &c, bool is_int) {
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

static std::pair<std::string, std::string> get_fail_pattern(
    std::vector<std::pair<llvm::BasicBlock *, decision_case const *>> const
        &case_data,
    bool is_int, llvm::BasicBlock *fail_block) {
  std::string reason;
  std::string sort;
  for (auto const &entry : case_data) {
    auto const &c = *entry.second;
    if (entry.first != fail_block) {
      auto case_reason = get_fail_pattern(c, is_int);
      if (reason.empty()) {
        reason = case_reason.second;
        sort = case_reason.first;
      } else {
        reason = fmt::format(
            "\\or{{{}}}({},{})", sort, reason, case_reason.second);
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
  auto *ptr_ty = llvm::PointerType::getUnqual(d->ctx_);
  llvm::Value *val = d->load(std::make_pair(name_, type_));
  llvm::Value *ptr_val = nullptr;
  if (d->fail_pattern_) {
    ptr_val = d->ptr_term(val);
  }
  llvm::BasicBlock *default_block = d->failure_block_;
  decision_case const *default_case = nullptr;
  std::vector<std::pair<llvm::BasicBlock *, decision_case const *>> case_data;
  int idx = 0;
  bool is_int = false;
  for (auto &c : cases_) {
    auto *child = c.get_child();
    llvm::BasicBlock *case_block = nullptr;
    if (child == fail_node::get()) {
      case_block = d->failure_block_;
    } else {
      case_block = llvm::BasicBlock::Create(
          d->ctx_,
          name_.substr(0, max_name_length) + "_case_" + std::to_string(idx++),
          d->current_block_->getParent());
    }
    if (auto *sym = c.get_constructor()) {
      is_int = is_int || sym->get_name() == "\\dv";
      case_data.emplace_back(case_block, &c);
    } else {
      default_block = case_block;
      default_case = &c;
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
    is_int = true;
  }
  llvm::Value *fail_sort = nullptr;
  llvm::Value *fail_pattern = nullptr;
  if (d->fail_pattern_) {
    auto fail_reason = get_fail_pattern(case_data, is_int, d->failure_block_);
    fail_sort = d->string_literal(fail_reason.first);
    fail_pattern = d->string_literal(fail_reason.second);
  }
  if (is_int) {
    auto *switch_inst = llvm::SwitchInst::Create(
        val, default_block, cases_.size(), d->current_block_);
    for (auto &c : case_data) {
      switch_inst->addCase(
          llvm::ConstantInt::get(d->ctx_, c.second->get_literal()), c.first);
    }
  } else {
    if (case_data.empty()) {
      llvm::BranchInst::Create(default_block, d->current_block_);
    } else {
      llvm::Value *tag_val = d->get_tag(val);
      auto *switch_inst = llvm::SwitchInst::Create(
          tag_val, default_block, case_data.size(), d->current_block_);
      for (auto &c : case_data) {
        switch_inst->addCase(
            llvm::ConstantInt::get(
                llvm::Type::getInt32Ty(d->ctx_),
                c.second->get_constructor()->get_tag()),
            c.first);
      }
    }
  }
  auto *curr_choice_block = d->choice_block_;
  d->choice_block_ = nullptr;
  auto *switch_block = d->current_block_;
  for (auto &entry : case_data) {
    auto const &switch_case = *entry.second;
    if (entry.first == d->failure_block_) {
      if (d->fail_pattern_) {
        d->fail_subject_->addIncoming(ptr_val, switch_block);
        d->fail_pattern_->addIncoming(fail_pattern, switch_block);
        d->fail_sort_->addIncoming(fail_sort, switch_block);
      }
      continue;
    }
    d->current_block_ = entry.first;
    if (!is_int) {
      int offset = 0;
      llvm::StructType *block_type = get_block_type(
          d->module_, d->definition_, switch_case.get_constructor());
      auto *cast = new llvm::BitCastInst(val, ptr_ty, "", d->current_block_);
      kore_symbol_declaration *symbol_decl
          = d->definition_->get_symbol_declarations().at(
              switch_case.get_constructor()->get_name());
      llvm::Instruction *renamed = nullptr;
      for (auto const &binding : switch_case.get_bindings()) {
        llvm::Value *child_ptr = llvm::GetElementPtrInst::CreateInBounds(
            block_type, cast,
            {llvm::ConstantInt::get(llvm::Type::getInt64Ty(d->ctx_), 0),
             llvm::ConstantInt::get(
                 llvm::Type::getInt32Ty(d->ctx_),
                 get_block_offset(
                     d->definition_, switch_case.get_constructor(), offset))},
            "", d->current_block_);

        llvm::Value *child = nullptr;
        auto cat
            = dynamic_cast<kore_composite_sort *>(
                  switch_case.get_constructor()->get_arguments()[offset].get())
                  ->get_category(d->definition_);

        switch (cat.cat) {
        case sort_category::Map:
        case sort_category::RangeMap:
        case sort_category::List:
        case sort_category::Set: child = child_ptr; break;
        default:
          child = new llvm::LoadInst(
              getvalue_type(cat, d->module_), child_ptr,
              binding.first.substr(0, max_name_length), d->current_block_);
          break;
        }
        auto *block_ptr = ptr_ty;
        if (symbol_decl->attributes().contains(attribute_set::key::Binder)) {
          if (offset == 0) {
            renamed = llvm::CallInst::Create(
                get_or_insert_function(
                    d->module_, "alpha_rename", block_ptr, block_ptr),
                child, "renamedVar", d->current_block_);
            set_debug_loc(renamed);
            d->store(binding, renamed);
          } else if (offset == switch_case.get_bindings().size() - 1) {
            llvm::Instruction *replaced = llvm::CallInst::Create(
                get_or_insert_function(
                    d->module_, "replace_binder_index", block_ptr, block_ptr,
                    block_ptr),
                {child, renamed}, "withUnboundIndex", d->current_block_);
            set_debug_loc(replaced);
            d->store(binding, replaced);
          } else {
            d->store(binding, child);
          }
        } else {
          d->store(binding, child);
        }
        offset++;
      }
    } else {
      if (curr_choice_block && switch_case.get_literal() == 1) {
        auto *prev_depth = new llvm::LoadInst(
            llvm::Type::getInt64Ty(d->ctx_), d->choice_depth_, "",
            d->current_block_);
        auto *curr_depth = llvm::BinaryOperator::Create(
            llvm::Instruction::Add, prev_depth,
            llvm::ConstantInt::get(llvm::Type::getInt64Ty(d->ctx_), 1), "",
            d->current_block_);
        new llvm::StoreInst(curr_depth, d->choice_depth_, d->current_block_);

        auto *alloc = llvm::cast<llvm::AllocaInst>(d->choice_buffer_);
        auto *ty = alloc->getAllocatedType();

        auto *zero = llvm::ConstantInt::get(llvm::Type::getInt64Ty(d->ctx_), 0);
        auto *current_elt = llvm::GetElementPtrInst::CreateInBounds(
            ty, d->choice_buffer_, {zero, curr_depth}, "", d->current_block_);
        new llvm::StoreInst(
            llvm::BlockAddress::get(
                d->current_block_->getParent(), curr_choice_block),
            current_elt, d->current_block_);
        d->fail_jump_->addDestination(curr_choice_block);
      } else if (
          curr_choice_block && switch_case.get_literal() == 0
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
        auto *new_case_block = llvm::BasicBlock::Create(
            d->ctx_, "hasNoSearchResults", d->current_block_->getParent());
        llvm::BranchInst::Create(
            d->failure_block_, new_case_block, loaded, d->current_block_);
        d->current_block_ = new_case_block;
      }
    }
    switch_case.get_child()->codegen(d);
  }
  if (default_case) {
    if (default_block != d->failure_block_) {
      // process default also
      d->current_block_ = default_block;
      default_case->get_child()->codegen(d);
    } else if (d->fail_pattern_) {
      d->fail_subject_->addIncoming(ptr_val, switch_block);
      d->fail_pattern_->addIncoming(fail_pattern, switch_block);
      d->fail_sort_->addIncoming(fail_sort, switch_block);
    }
  }

  set_completed();
}

void make_pattern_node::codegen(decision *d) {
  if (begin_node(d, "pattern" + name_)) {
    return;
  }
  llvm::StringMap<llvm::Value *> final_subst;
  for (auto const &use : uses_) {
    final_subst[use.first] = d->load(use);
  }
  create_term creator(
      final_subst, d->definition_, d->current_block_, d->module_, false);
  llvm::Value *val = creator(pattern_).first;
  d->current_block_ = creator.get_current_block();
  d->store(std::make_pair(name_, type_), val);
  child_->codegen(d);
  set_completed();
}

/*
 * The code generation step for function_nodes emits a call to add_match_function
 * that records why a match failed to apply in the case of side-condition calls
 * failing. Doing so requires that we store the _type_ of each argument
 * somewhere, in order that opaque block* values can be safely reinterpreted in
 * the sort that they originated from (int, bool, symbol, ...). In LLVM versions
 * < 16, we could encode this information in the LLVM type safely. However,
 * after the LLVM opaque pointer migration, we can no longer do so (as the
 * legacy types %mpz* and %block* would both be ptr, for example). We
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
  case sort_category::MapIter: return "<mapiter>";
  case sort_category::RangeMap: return "<rangemap>";
  case sort_category::List: return "<list>";
  case sort_category::Set: return "<set>";
  case sort_category::SetIter: return "<setiter>";
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
  llvm::StringMap<llvm::Value *> final_subst;
  for (auto [arg, cat] : bindings_) {
    llvm::Value *val = nullptr;
    if (arg.first.find_first_not_of("-0123456789") == std::string::npos) {
      val = llvm::ConstantInt::get(
          llvm::Type::getInt64Ty(d->ctx_), std::stoi(arg.first));
    } else {
      val = d->load(arg);
    }
    args.push_back(val);
    final_subst[arg.first] = val;
  }
  bool is_side_condition = function_.substr(0, 15) == "side_condition_";

  if (is_side_condition) {
    proof_event p(d->definition_, d->module_);
    size_t ordinal = std::stoll(function_.substr(15));
    auto axiom = d->definition_->get_axiom_by_ordinal(ordinal);
    d->current_block_
        = p.side_condition_event_pre(axiom, args, d->current_block_);
  }

  create_term creator(
      final_subst, d->definition_, d->current_block_, d->module_, false);
  auto *call = creator.create_function_call(
      function_, cat_, args, function_.substr(0, 5) == "hook_",
      is_side_condition, false);
  call->setName(name_.substr(0, max_name_length));
  d->store(std::make_pair(name_, type_), call);

  if (is_side_condition) {
    proof_event p(d->definition_, d->module_);
    size_t ordinal = std::stoll(function_.substr(15));
    auto axiom = d->definition_->get_axiom_by_ordinal(ordinal);
    d->current_block_
        = p.side_condition_event_post(axiom, call, d->current_block_);
  }

  if (d->fail_pattern_) {
    std::string debug_name = function_;
    if (function_.substr(0, 5) == "hook_") {
      debug_name = function_.substr(5, function_.find_first_of('_', 5) - 5)
                   + "."
                   + function_.substr(function_.find_first_of('_', 5) + 1);
    } else if (is_side_condition) {
      size_t ordinal = std::stoll(function_.substr(15));
      auto axiom = d->definition_->get_axiom_by_ordinal(ordinal);
      if (axiom.attributes().contains(attribute_set::key::Label)) {
        debug_name
            = axiom.attributes().get_string(attribute_set::key::Label) + ".sc";
      }
    }
    std::vector<llvm::Value *> function_args;
    function_args.push_back(d->string_literal(debug_name));
    function_args.push_back(d->string_literal(function_));
    auto *temp_alloc_call = d->ptr_term(call);
    if (call->getType() == llvm::Type::getInt1Ty(d->ctx_)) {
      llvm::Value *zext = new llvm::ZExtInst(
          call, llvm::Type::getInt8Ty(d->ctx_), "", d->current_block_);
      new llvm::StoreInst(zext, temp_alloc_call, d->current_block_);
    }
    function_args.push_back(temp_alloc_call);
    for (auto i = 0U; i < args.size(); ++i) {
      auto *arg = args[i];
      auto cat = bindings_[i].second;

      auto *temp_alloc_arg = d->ptr_term(arg);
      if (arg->getType() == llvm::Type::getInt1Ty(d->ctx_)) {
        llvm::Value *zext = new llvm::ZExtInst(
            call, llvm::Type::getInt8Ty(d->ctx_), "", d->current_block_);
        new llvm::StoreInst(zext, temp_alloc_arg, d->current_block_);
      }
      function_args.push_back(temp_alloc_arg);
      auto str = legacy_value_type_to_string(cat);
      function_args.push_back(d->string_literal(str));
    }
    auto *ptr_ty = llvm::PointerType::getUnqual(d->ctx_);
    function_args.push_back(llvm::ConstantPointerNull::get(ptr_ty));

    auto *call = llvm::CallInst::Create(
        get_or_insert_function(
            d->module_, "add_match_function",
            llvm::FunctionType::get(
                llvm::Type::getVoidTy(d->ctx_), {ptr_ty, ptr_ty, ptr_ty},
                true)),
        function_args, "", d->current_block_);
    set_debug_loc(call);
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
  llvm::Type *sret_type
      = llvm::StructType::getTypeByName(d->module_->getContext(), "iter");
  llvm::Value *alloc_sret
      = allocate_term(sret_type, d->current_block_, "kore_alloc_always_gc");
  alloc_sret->setName(name_.substr(0, max_name_length));
  args.insert(args.begin(), alloc_sret);
  types.insert(types.begin(), alloc_sret->getType());

  llvm::FunctionType *func_type = llvm::FunctionType::get(
      llvm::Type::getVoidTy(d->module_->getContext()), types, false);
  llvm::Function *func
      = get_or_insert_function(d->module_, hook_name_, func_type);
  auto *call = llvm::CallInst::Create(func, args, "", d->current_block_);
  set_debug_loc(call);
  llvm::Attribute sret_attr
      = llvm::Attribute::get(d->ctx_, llvm::Attribute::StructRet, sret_type);
  func->arg_begin()->addAttr(sret_attr);
  call->addParamAttr(0, sret_attr);
  d->store(std::make_pair(name_, type_), alloc_sret);
  child_->codegen(d);
  set_completed();
}

void iter_next_node::codegen(decision *d) {
  if (begin_node(d, "choice" + binding_)) {
    return;
  }
  d->choice_block_ = d->current_block_;
  llvm::Value *arg = d->load(std::make_pair(iterator_, iterator_type_));

  llvm::FunctionType *func_type = llvm::FunctionType::get(
      getvalue_type({sort_category::Symbol, 0}, d->module_), {arg->getType()},
      false);
  llvm::Function *func
      = get_or_insert_function(d->module_, hook_name_, func_type);
  auto *call = llvm::CallInst::Create(
      func, {arg}, binding_.substr(0, max_name_length), d->current_block_);
  set_debug_loc(call);
  d->store(std::make_pair(binding_, binding_type_), call);
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
        get_or_insert_function(
            d->module_, "add_match_success", llvm::Type::getVoidTy(d->ctx_)),
        {}, "", d->current_block_);
    set_debug_loc(call);
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
  auto *type = get_param_type(d->cat_, d->module_);

  auto *apply_rule = get_or_insert_function(
      d->module_, name_, llvm::FunctionType::get(type, types, false));

  // We are generating code for a function with name beginning apply_rule_\d+; to
  // retrieve the corresponding ordinal we drop the apply_rule_ prefix.
  auto ordinal = std::stoll(name_.substr(11));
  auto arity = apply_rule->arg_end() - apply_rule->arg_begin();
  auto axiom = d->definition_->get_axiom_by_ordinal(ordinal);

  auto vars = std::map<std::string, kore_variable_pattern *>{};
  for (kore_pattern *lhs : axiom.get_left_hand_side()) {
    lhs->mark_variables(vars);
  }
  axiom.get_right_hand_side()->mark_variables(vars);

  auto subst = llvm::StringMap<llvm::Value *>{};
  auto i = 0;
  for (auto iter = vars.begin(); iter != vars.end(); ++iter, ++i) {
    subst[iter->first] = args[i];
  }

  d->current_block_
      = proof_event(d->definition_, d->module_)
            .rewrite_event_pre(axiom, arity, vars, subst, d->current_block_);

  if (d->profile_matching_) {
    llvm::CallInst::Create(
        get_or_insert_function(
            d->module_, "stop_clock", llvm::Type::getVoidTy(d->ctx_),
            llvm::Type::getInt64Ty(d->ctx_)),
        {llvm::ConstantInt::get(llvm::Type::getInt64Ty(d->ctx_), ordinal)}, "",
        d->current_block_);
  }
  auto *call = llvm::CallInst::Create(apply_rule, args, "", d->current_block_);
  set_debug_loc(call);
  call->setCallingConv(llvm::CallingConv::Tail);

  if (child_ == nullptr) {
    if (can_tail_call(call->getType())) {
      call->setTailCallKind(llvm::CallInst::TCK_MustTail);
    }
    llvm::ReturnInst::Create(d->ctx_, call, d->current_block_);
  } else {
    new llvm::StoreInst(
        llvm::ConstantInt::getTrue(d->ctx_), d->has_search_results_,
        d->current_block_);
    auto *call2 = llvm::CallInst::Create(
        get_or_insert_function(
            d->module_, "add_search_result",
            llvm::FunctionType::get(
                llvm::Type::getVoidTy(d->ctx_), {type}, false)),
        {call}, "", d->current_block_);
    set_debug_loc(call2);
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
      get_or_insert_function(
          module_, "get_tag", llvm::Type::getInt32Ty(ctx_),
          getvalue_type({sort_category::Symbol, 0}, module_)),
      val, "tag", current_block_);
  set_debug_loc(res);
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
  auto *str_cst = llvm::ConstantDataArray::getString(ctx_, str, true);
  auto *global
      = module_->getOrInsertGlobal("str_lit_" + str, str_cst->getType());
  auto *global_var = llvm::cast<llvm::GlobalVariable>(global);
  if (!global_var->hasInitializer()) {
    global_var->setInitializer(str_cst);
  }
  llvm::Constant *zero
      = llvm::ConstantInt::get(llvm::Type::getInt64Ty(ctx_), 0);
  auto indices = std::vector<llvm::Constant *>{zero, zero};
  auto *ptr = llvm::ConstantExpr::getInBoundsGetElementPtr(
      str_cst->getType(), global_var, indices);
  return ptr;
}

static void init_choice_buffer(
    decision_node *dt, llvm::Module *module, llvm::BasicBlock *block,
    llvm::BasicBlock *stuck, llvm::BasicBlock *fail,
    llvm::AllocaInst **choice_buffer_out, llvm::AllocaInst **choice_depth_out,
    llvm::IndirectBrInst **jump_out, bool profile_matching) {
  std::unordered_set<leaf_node *> leaves;
  dt->preprocess(leaves);
  auto *ty = llvm::ArrayType::get(
      llvm::PointerType::getUnqual(module->getContext()),
      dt->get_choice_depth() + 1);
  auto *choice_buffer = new llvm::AllocaInst(ty, 0, "choiceBuffer", block);
  auto *choice_depth = new llvm::AllocaInst(
      llvm::Type::getInt64Ty(module->getContext()), 0, "choiceDepth", block);
  if (profile_matching) {
    llvm::CallInst::Create(
        get_or_insert_function(
            module, "start_clock", llvm::Type::getVoidTy(module->getContext())),
        {}, "", block);
  }
  auto *zero
      = llvm::ConstantInt::get(llvm::Type::getInt64Ty(module->getContext()), 0);
  new llvm::StoreInst(zero, choice_depth, block);
  auto *first_elt = llvm::GetElementPtrInst::CreateInBounds(
      ty, choice_buffer, {zero, zero}, "", block);
  new llvm::StoreInst(
      llvm::BlockAddress::get(block->getParent(), stuck), first_elt, block);

  auto *curr_depth = new llvm::LoadInst(
      llvm::Type::getInt64Ty(module->getContext()), choice_depth, "", fail);
  auto *current_elt = llvm::GetElementPtrInst::CreateInBounds(
      ty, choice_buffer, {zero, curr_depth}, "", fail);
  auto *fail_address
      = new llvm::LoadInst(ty->getElementType(), current_elt, "", fail);
  auto *new_depth = llvm::BinaryOperator::Create(
      llvm::Instruction::Sub, curr_depth,
      llvm::ConstantInt::get(llvm::Type::getInt64Ty(module->getContext()), 1),
      "", fail);
  new llvm::StoreInst(new_depth, choice_depth, fail);
  llvm::IndirectBrInst *jump
      = llvm::IndirectBrInst::Create(fail_address, 1, fail);
  jump->addDestination(stuck);
  *choice_buffer_out = choice_buffer;
  *choice_depth_out = choice_depth;
  *jump_out = jump;
}

void make_eval_or_anywhere_function(
    kore_symbol *function, kore_definition *definition, llvm::Module *module,
    decision_node *dt,
    void (*add_stuck)(
        llvm::BasicBlock *, llvm::Module *, kore_symbol *, decision &,
        kore_definition *)) {
  auto return_sort
      = dynamic_cast<kore_composite_sort *>(function->get_sort().get())
            ->get_category(definition);
  auto *return_type = get_param_type(return_sort, module);
  auto *debug_return_type
      = get_debug_type(return_sort, ast_to_string(*function->get_sort()));
  std::vector<llvm::Type *> args;
  std::vector<llvm::Metadata *> debug_args;
  std::vector<value_type> cats;
  for (auto const &sort : function->get_arguments()) {
    auto cat = dynamic_cast<kore_composite_sort *>(sort.get())
                   ->get_category(definition);
    debug_args.push_back(get_debug_type(cat, ast_to_string(*sort)));
    switch (cat.cat) {
    case sort_category::Map:
    case sort_category::RangeMap:
    case sort_category::List:
    case sort_category::Set:
      args.push_back(llvm::PointerType::getUnqual(module->getContext()));
      cats.push_back(cat);
      break;
    default:
      args.push_back(getvalue_type(cat, module));
      cats.push_back(cat);
      break;
    }
  }
  llvm::FunctionType *func_type
      = llvm::FunctionType::get(return_type, args, false);
  std::string name = fmt::format("eval_{}", ast_to_string(*function, 0, false));
  llvm::Function *match_func = get_or_insert_function(module, name, func_type);
  // there are a few functions that are given default implementations in
  // llvm_header.inc. Previously these were implemented as weak function
  // definitions, but now they live in the same translation unit, so resolving
  // them via the linker is not possible. The purpose of these weak function
  // definitions was to provide an implementation of some required
  // functionality in the event that the K definition we are compiling does
  // not provide the functionality itself. As such, in the case where we
  // are actually generating the real body of the function, a previous, dummy
  // body might already exist. We simply delete the body of the function here
  // and start from scratch every time, in order to ensure that we always
  // have one correct version of the function body after code generation
  // finishes.
  match_func->deleteBody();
  auto const &att = definition->get_symbol_declarations()
                        .at(function->get_name())
                        ->attributes();
  if (!att.contains(attribute_set::key::Impure)
      && att.contains(attribute_set::key::Total)) {
    match_func->addFnAttr("kllvm-pure");
  }
  [[maybe_unused]] kore_symbol_declaration *symbol_decl
      = definition->get_symbol_declarations().at(function->get_name());
  init_debug_axiom(symbol_decl->attributes());
  init_debug_function(
      function->get_name(), name,
      get_debug_function_type(debug_return_type, debug_args), definition,
      match_func);
  match_func->setCallingConv(llvm::CallingConv::Tail);
  llvm::BasicBlock *block
      = llvm::BasicBlock::Create(module->getContext(), "entry", match_func);
  llvm::BasicBlock *stuck
      = llvm::BasicBlock::Create(module->getContext(), "stuck", match_func);
  llvm::BasicBlock *fail
      = llvm::BasicBlock::Create(module->getContext(), "fail", match_func);

  llvm::AllocaInst *choice_buffer = nullptr;
  llvm::AllocaInst *choice_depth = nullptr;
  llvm::IndirectBrInst *jump = nullptr;
  init_choice_buffer(
      dt, module, block, stuck, fail, &choice_buffer, &choice_depth, &jump,
      false);

  int i = 0;
  decision codegen(
      definition, block, fail, jump, choice_buffer, choice_depth, module,
      return_sort, nullptr, nullptr, nullptr, nullptr);
  for (auto *val = match_func->arg_begin(); val != match_func->arg_end();
       ++val, ++i) {
    val->setName("_" + std::to_string(i + 1));
    codegen.store(std::make_pair(val->getName().str(), val->getType()), val);
    init_debug_param(
        match_func, i, val->getName().str(), cats[i],
        ast_to_string(*function->get_arguments()[i]));
  }
  add_stuck(stuck, module, function, codegen, definition);

  codegen(dt);
}

void abort_when_stuck(
    llvm::BasicBlock *current_block, llvm::Module *module, kore_symbol *symbol,
    decision &codegen, kore_definition *d) {
  auto &ctx = module->getContext();
  symbol = d->get_all_symbols().at(ast_to_string(*symbol));
  auto *block_type = get_block_type(module, d, symbol);
  llvm::Value *ptr = nullptr;
  auto *block_ptr = llvm::PointerType::getUnqual(module->getContext());
  if (symbol->get_arguments().empty()) {
    ptr = llvm::ConstantExpr::getIntToPtr(
        llvm::ConstantInt::get(
            llvm::Type::getInt64Ty(ctx),
            ((uint64_t)symbol->get_tag() << 32 | 1)),
        getvalue_type({sort_category::Symbol, 0}, module));
  } else {
    llvm::Value *block_header = get_block_header(module, d, symbol, block_type);
    llvm::Value *block = allocate_term(block_type, current_block);
    llvm::Value *block_header_ptr = llvm::GetElementPtrInst::CreateInBounds(
        block_type, block,
        {llvm::ConstantInt::get(llvm::Type::getInt64Ty(ctx), 0),
         llvm::ConstantInt::get(llvm::Type::getInt32Ty(ctx), 0)},
        symbol->get_name(), current_block);
    new llvm::StoreInst(block_header, block_header_ptr, current_block);
    for (int idx = 0; idx < symbol->get_arguments().size(); idx++) {
      auto cat = dynamic_cast<kore_composite_sort *>(
                     symbol->get_arguments()[idx].get())
                     ->get_category(d);
      auto *type = get_param_type(cat, module);
      llvm::Value *child_value
          = codegen.load(std::make_pair("_" + std::to_string(idx + 1), type));
      llvm::Value *child_ptr = llvm::GetElementPtrInst::CreateInBounds(
          block_type, block,
          {llvm::ConstantInt::get(llvm::Type::getInt64Ty(ctx), 0),
           llvm::ConstantInt::get(
               llvm::Type::getInt32Ty(ctx), get_block_offset(d, symbol, idx))},
          "", current_block);
      if (is_collection_sort(cat)) {
        child_value = new llvm::LoadInst(
            get_arg_type(cat, module), child_value, "", current_block);
      }
      new llvm::StoreInst(child_value, child_ptr, current_block);
    }
    ptr = new llvm::BitCastInst(block, block_ptr, "", current_block);
  }
  auto *func = get_or_insert_function(
      module, "finish_rewriting", llvm::Type::getVoidTy(ctx), block_ptr,
      llvm::Type::getInt1Ty(ctx));
  func->setDoesNotReturn();
  llvm::CallInst::Create(
      func, {ptr, llvm::ConstantInt::getTrue(ctx)}, "", current_block);
  new llvm::UnreachableInst(ctx, current_block);
}

void make_eval_function(
    kore_symbol *function, kore_definition *definition, llvm::Module *module,
    decision_node *dt) {
  make_eval_or_anywhere_function(
      function, definition, module, dt, abort_when_stuck);
}

void add_owise(
    llvm::BasicBlock *stuck, llvm::Module *module, kore_symbol *symbol,
    decision &codegen, kore_definition *d) {
  llvm::StringMap<llvm::Value *> final_subst;
  ptr<kore_composite_pattern> pat = kore_composite_pattern::create(symbol);
  for (int i = 0; i < symbol->get_arguments().size(); i++) {
    auto cat
        = dynamic_cast<kore_composite_sort *>(symbol->get_arguments()[i].get())
              ->get_category(d);
    auto *type = get_param_type(cat, module);

    std::string name = "_" + std::to_string(i + 1);
    final_subst[name] = codegen.load(std::make_pair(name, type));

    auto var = kore_variable_pattern::create(name, symbol->get_arguments()[i]);
    pat->add_argument(std::move(var));
  }
  proof_event p(d, module);
  stuck = p.pattern_matching_failure(*pat, stuck);
  create_term creator = create_term(final_subst, d, stuck, module, true);
  llvm::Value *retval = creator(pat.get()).first;

  auto return_sort
      = dynamic_cast<kore_composite_sort *>(symbol->get_sort().get())
            ->get_category(d);
  if (is_collection_sort(return_sort)) {
    auto *temp_alloc = allocate_term(
        retval->getType(), creator.get_current_block(),
        get_collection_alloc_fn(return_sort.cat));
    new llvm::StoreInst(retval, temp_alloc, creator.get_current_block());
    retval = temp_alloc;
  }

  llvm::ReturnInst::Create(
      module->getContext(), retval, creator.get_current_block());
}

void make_anywhere_function(
    kore_symbol *function, kore_definition *definition, llvm::Module *module,
    decision_node *dt) {
  make_eval_or_anywhere_function(function, definition, module, dt, add_owise);
}

// writes pointers to gc_roots prior to garbage collection
static void store_ptrs_for_gc(
    unsigned nroots, llvm::Module *module, llvm::Type *root_ty,
    llvm::Constant *arr, llvm::BasicBlock *collect,
    std::vector<llvm::Value *> const &roots,
    std::vector<value_type> const &types,
    std::vector<llvm::Type *> const &ptr_types,
    std::vector<std::pair<llvm::Value *, llvm::Type *>> &root_ptrs) {
  auto *zero
      = llvm::ConstantInt::get(llvm::Type::getInt64Ty(module->getContext()), 0);
  for (unsigned i = 0; i < nroots; i++) {
    auto *ptr = llvm::GetElementPtrInst::CreateInBounds(
        root_ty, arr,
        {zero, llvm::ConstantInt::get(
                   llvm::Type::getInt64Ty(module->getContext()), i)},
        "", collect);
    auto *casted = new llvm::BitCastInst(
        ptr, llvm::PointerType::getUnqual(module->getContext()), "", collect);
    new llvm::StoreInst(roots[i], casted, collect);
    root_ptrs.emplace_back(casted, ptr_types[i]);
  }
}

// reads pointers from gc_roots global variable following garbage collection
// and creates phi nodes for assigning the correct new value for the roots
static void load_ptrs_for_gc(
    llvm::Module *module, llvm::BasicBlock *check_collect,
    llvm::BasicBlock *collect, llvm::BasicBlock *merge,
    std::vector<llvm::Value *> &phis, std::vector<llvm::Value *> const &roots,
    std::vector<std::pair<llvm::Value *, llvm::Type *>> const &root_ptrs,
    std::vector<value_type> const &types) {
  unsigned i = 0;
  for (auto [ptr, pointee_ty] : root_ptrs) {
    llvm::Value *loaded = new llvm::LoadInst(pointee_ty, ptr, "", collect);
    auto *phi = llvm::PHINode::Create(loaded->getType(), 2, "phi", merge);
    phi->addIncoming(loaded, collect);
    phi->addIncoming(roots[i++], check_collect);
    phis.push_back(phi);
  }
}

std::pair<std::vector<llvm::Value *>, llvm::BasicBlock *> step_function_header(
    unsigned ordinal, llvm::Module *module, kore_definition *definition,
    llvm::BasicBlock *block, llvm::BasicBlock *stuck,
    std::vector<llvm::Value *> args, std::vector<value_type> const &types) {
  auto *ptr_ty = llvm::PointerType::getUnqual(module->getContext());
  auto *finished = get_or_insert_function(
      module, "finished_rewriting",
      llvm::FunctionType::get(
          llvm::Type::getInt1Ty(module->getContext()), {}, false));
  auto *is_finished = llvm::CallInst::Create(finished, {}, "", block);
  auto *check_collect = llvm::BasicBlock::Create(
      module->getContext(), "checkCollect", block->getParent());
  llvm::BranchInst::Create(stuck, check_collect, is_finished, block);

  auto *collection = module->getOrInsertGlobal(
      "time_for_collection", llvm::Type::getInt1Ty(module->getContext()));
  llvm::cast<llvm::GlobalVariable>(collection)->setThreadLocal(true);
#ifdef __MACH__
  llvm::IRBuilder b(check_collect);
  auto *collection_address = b.CreateThreadLocalAddress(collection);
#else
  auto *collection_address = collection;
#endif
  
  auto *is_collection = new llvm::LoadInst(
      llvm::Type::getInt1Ty(module->getContext()), collection_address, "is_collection",
      check_collect);
  set_debug_loc(is_collection);
  auto *collect = llvm::BasicBlock::Create(
      module->getContext(), "isCollect", block->getParent());
  auto *merge = llvm::BasicBlock::Create(
      module->getContext(), "step", block->getParent());
  llvm::BranchInst::Create(collect, merge, is_collection, check_collect);

  unsigned nroots = 0;
  unsigned i = 0;
  std::vector<llvm::Type *> ptr_types;
  std::vector<llvm::Value *> roots;
  for (auto type : types) {
    switch (type.cat) {
    case sort_category::Map:
    case sort_category::RangeMap:
    case sort_category::List:
    case sort_category::Set:
      nroots++;
      ptr_types.push_back(ptr_ty);
      roots.push_back(args[i]);
      break;
    case sort_category::Int:
    case sort_category::Float:
    case sort_category::StringBuffer:
    case sort_category::Symbol:
    case sort_category::Variable:
      nroots++;
      ptr_types.push_back(getvalue_type(type, module));
      roots.push_back(args[i]);
      break;
    case sort_category::Bool:
    case sort_category::MInt: break;
    case sort_category::MapIter:
    case sort_category::SetIter:
    case sort_category::Uncomputed: abort();
    }
    i++;
  }
  auto *root_ty = llvm::ArrayType::get(ptr_ty, 256);
  auto *arr = module->getOrInsertGlobal("gc_roots", root_ty);
  std::vector<std::pair<llvm::Value *, llvm::Type *>> root_ptrs;

  store_ptrs_for_gc(
      nroots, module, root_ty, arr, collect, roots, types, ptr_types,
      root_ptrs);
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
    case sort_category::MapIter:
    case sort_category::SetIter:
    case sort_category::Uncomputed: abort();
    }
  }
  auto *layout_arr = llvm::ConstantArray::get(
      llvm::ArrayType::get(
          llvm::StructType::getTypeByName(
              module->getContext(), layoutitem_struct),
          elements.size()),
      elements);
  auto *layout = module->getOrInsertGlobal(
      "layout_item_rule_" + std::to_string(ordinal), layout_arr->getType());
  auto *global_var = llvm::cast<llvm::GlobalVariable>(layout);
  if (!global_var->hasInitializer()) {
    global_var->setInitializer(layout_arr);
  }
  auto *kore_collect = get_or_insert_function(
      module, "kore_collect",
      llvm::FunctionType::get(
          llvm::Type::getVoidTy(module->getContext()),
          {arr->getType(), llvm::Type::getInt8Ty(module->getContext()), ptr_ty,
           ptr_ty, llvm::Type::getInt1Ty(module->getContext())},
          false));
  auto *call = llvm::CallInst::Create(
      kore_collect,
      {arr,
       llvm::ConstantInt::get(
           llvm::Type::getInt8Ty(module->getContext()), nroots),
       llvm::ConstantExpr::getBitCast(layout, ptr_ty),
       llvm::ConstantInt::getFalse(module->getContext())},
      "", collect);
  set_debug_loc(call);
  std::vector<llvm::Value *> phis;
  load_ptrs_for_gc(
      module, check_collect, collect, merge, phis, roots, root_ptrs, types);
  llvm::BranchInst::Create(merge, collect);
  i = 0;
  unsigned root_idx = 0;
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
    case sort_category::Float: results.push_back(phis[root_idx++]); break;
    default: results.push_back(args[i]);
    }
    i++;
  }
  return std::make_pair(results, merge);
}

void make_step_function(
    kore_definition *definition, llvm::Module *module, decision_node *dt,
    bool search, bool profile_matching) {
  auto *block_type = getvalue_type({sort_category::Symbol, 0}, module);
  auto *debug_type
      = get_debug_type({sort_category::Symbol, 0}, "SortGeneratedTopCell{}");
  llvm::FunctionType *func_type = nullptr;
  std::string name;
  if (search) {
    name = "step_all";
    func_type = llvm::FunctionType::get(
        llvm::Type::getVoidTy(module->getContext()), {block_type}, false);
  } else {
    name = "k_step";
    func_type = llvm::FunctionType::get(block_type, {block_type}, false);
  }
  llvm::Function *match_func = get_or_insert_function(module, name, func_type);
  reset_debug_loc();
  if (search) {
    init_debug_function(
        name, name,
        get_debug_function_type(get_void_debug_type(), {debug_type}),
        definition, match_func);
  } else {
    init_debug_function(
        name, name, get_debug_function_type(debug_type, {debug_type}),
        definition, match_func);
  }
  match_func->setCallingConv(llvm::CallingConv::Tail);
  auto *val = match_func->arg_begin();
  llvm::BasicBlock *block
      = llvm::BasicBlock::Create(module->getContext(), "entry", match_func);
  llvm::BasicBlock *stuck
      = llvm::BasicBlock::Create(module->getContext(), "stuck", match_func);
  llvm::BasicBlock *pre_stuck
      = llvm::BasicBlock::Create(module->getContext(), "pre_stuck", match_func);
  llvm::BasicBlock *fail
      = llvm::BasicBlock::Create(module->getContext(), "fail", match_func);

  llvm::AllocaInst *choice_buffer = nullptr;
  llvm::AllocaInst *choice_depth = nullptr;
  llvm::IndirectBrInst *jump = nullptr;

  init_choice_buffer(
      dt, module, block, pre_stuck, fail, &choice_buffer, &choice_depth, &jump,
      profile_matching);
  insert_call_to_clear(block);

  llvm::AllocaInst *has_search_results = nullptr;
  if (search) {
    has_search_results = new llvm::AllocaInst(
        llvm::Type::getInt1Ty(module->getContext()), 0, "has_search_results",
        block);
    new llvm::StoreInst(
        llvm::ConstantInt::getFalse(module->getContext()), has_search_results,
        block);
  }
  init_debug_param(
      match_func, 0, "subject", {sort_category::Symbol, 0},
      "SortGeneratedTopCell{}");
  llvm::BranchInst::Create(stuck, pre_stuck);
  auto result = step_function_header(
      0, module, definition, block, stuck, {val}, {{sort_category::Symbol, 0}});
  auto *collected_val = result.first[0];
  collected_val->setName("_1");
  decision codegen(
      definition, result.second, fail, jump, choice_buffer, choice_depth,
      module, {sort_category::Symbol, 0}, nullptr, nullptr, nullptr,
      has_search_results, profile_matching);
  codegen.store(
      std::make_pair(collected_val->getName().str(), collected_val->getType()),
      collected_val);
  if (search) {
    llvm::ReturnInst::Create(module->getContext(), stuck);
  } else {
    auto *phi
        = llvm::PHINode::Create(collected_val->getType(), 2, "phi_1", stuck);
    phi->addIncoming(val, block);
    phi->addIncoming(collected_val, pre_stuck);
    llvm::ReturnInst::Create(module->getContext(), phi, stuck);
  }

  codegen(dt);
}

void make_match_reason_function_wrapper(
    kore_definition *definition, llvm::Module *module,
    kore_axiom_declaration *axiom, std::string const &name) {
  auto *block_type = getvalue_type({sort_category::Symbol, 0}, module);
  llvm::FunctionType *func_type = llvm::FunctionType::get(
      llvm::Type::getVoidTy(module->getContext()), {block_type}, false);
  std::string wrapper_name = "match_" + std::to_string(axiom->get_ordinal());
  llvm::Function *match_func
      = get_or_insert_function(module, wrapper_name, func_type);
  std::string debug_name = name;
  if (axiom->attributes().contains(attribute_set::key::Label)) {
    debug_name = axiom->attributes().get_string(attribute_set::key::Label)
                 + "_tailcc_" + ".match";
  }
  auto *debug_type
      = get_debug_type({sort_category::Symbol, 0}, "SortGeneratedTopCell{}");
  reset_debug_loc();
  init_debug_function(
      debug_name, debug_name,
      get_debug_function_type(get_void_debug_type(), {debug_type}), definition,
      match_func);
  match_func->setCallingConv(llvm::CallingConv::Tail);
  llvm::BasicBlock *entry
      = llvm::BasicBlock::Create(module->getContext(), "entry", match_func);

  auto *ci = module->getFunction(name);
  auto *call = llvm::CallInst::Create(ci, match_func->getArg(0), "", entry);
  set_debug_loc(call);

  llvm::ReturnInst::Create(module->getContext(), entry);
}

void make_match_reason_function(
    kore_definition *definition, llvm::Module *module,
    kore_axiom_declaration *axiom, decision_node *dt) {
  auto *block_type = getvalue_type({sort_category::Symbol, 0}, module);
  llvm::FunctionType *func_type = llvm::FunctionType::get(
      llvm::Type::getVoidTy(module->getContext()), {block_type}, false);
  std::string name = "intern_match_" + std::to_string(axiom->get_ordinal());
  llvm::Function *match_func = get_or_insert_function(module, name, func_type);
  std::string debug_name = name;
  if (axiom->attributes().contains(attribute_set::key::Label)) {
    debug_name
        = axiom->attributes().get_string(attribute_set::key::Label) + ".match";
  }
  auto *debug_type
      = get_debug_type({sort_category::Symbol, 0}, "SortGeneratedTopCell{}");
  reset_debug_loc();
  init_debug_function(
      debug_name, debug_name,
      get_debug_function_type(get_void_debug_type(), {debug_type}), definition,
      match_func);
  auto *val = match_func->arg_begin();
  llvm::BasicBlock *block
      = llvm::BasicBlock::Create(module->getContext(), "entry", match_func);
  llvm::BasicBlock *stuck
      = llvm::BasicBlock::Create(module->getContext(), "stuck", match_func);
  llvm::BasicBlock *pre_stuck
      = llvm::BasicBlock::Create(module->getContext(), "pre_stuck", match_func);
  llvm::BasicBlock *fail
      = llvm::BasicBlock::Create(module->getContext(), "fail", match_func);
  auto *ptr_ty = llvm::PointerType::getUnqual(module->getContext());
  llvm::PHINode *fail_subject
      = llvm::PHINode::Create(ptr_ty, 0, "subject", fail);

  // The pointer types created here for the failure pattern and sort need to be
  // given an explicit element type on LLVM 15 (LLVM_VERSION_MAJOR <= 15). This
  // is because the constants that eventually flow into the Phi nodes still get
  // type `i8*` on LLVM 15, rather than `ptr` as would be assigned here.
  //
  // In newer versions, the string constants also get type `ptr` and these
  // explicit element types become no-ops that we can remove.
  llvm::PHINode *fail_pattern
      = llvm::PHINode::Create(ptr_ty, 0, "pattern", fail);
  llvm::PHINode *fail_sort = llvm::PHINode::Create(ptr_ty, 0, "sort", fail);

  auto *call = llvm::CallInst::Create(
      get_or_insert_function(
          module, "add_match_fail_reason",
          llvm::FunctionType::get(
              llvm::Type::getVoidTy(module->getContext()),
              {fail_subject->getType(), fail_pattern->getType(),
               fail_sort->getType()},
              false)),
      {fail_subject, fail_pattern, fail_sort}, "", fail);
  set_debug_loc(call);

  llvm::AllocaInst *choice_buffer = nullptr;
  llvm::AllocaInst *choice_depth = nullptr;
  llvm::IndirectBrInst *jump = nullptr;
  init_choice_buffer(
      dt, module, block, pre_stuck, fail, &choice_buffer, &choice_depth, &jump,
      false);

  init_debug_param(
      match_func, 0, "subject", {sort_category::Symbol, 0},
      "SortGeneratedTopCell{}");
  llvm::BranchInst::Create(stuck, pre_stuck);
  val->setName("_1");
  decision codegen(
      definition, block, fail, jump, choice_buffer, choice_depth, module,
      {sort_category::Symbol, 0}, fail_subject, fail_pattern, fail_sort,
      nullptr);
  codegen.store(std::make_pair(val->getName().str(), val->getType()), val);
  llvm::ReturnInst::Create(module->getContext(), stuck);

  codegen(dt);
  make_match_reason_function_wrapper(definition, module, axiom, name);
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
kore_pattern *make_partial_term(
    kore_pattern *term, std::set<std::string> const &occurrences,
    std::string const &occurrence) {
  if (occurrences.contains(occurrence)) {
    return kore_variable_pattern::create(occurrence, term->get_sort())
        .release();
  }
  if (auto *pat = dynamic_cast<kore_composite_pattern *>(term)) {
    if (pat->get_constructor()->get_name() == "\\dv") {
      return term;
    }
    ptr<kore_composite_pattern> result
        = kore_composite_pattern::create(pat->get_constructor());
    for (unsigned i = 0; i < pat->get_arguments().size(); i++) {
      result->add_argument(ptr<kore_pattern>(make_partial_term(
          dynamic_cast<kore_pattern *>(pat->get_arguments()[i].get()),
          occurrences, "_" + std::to_string(i) + occurrence)));
    }
    return result.release();
  }
  abort();
}

void make_step_function(
    kore_axiom_declaration *axiom, kore_definition *definition,
    llvm::Module *module, partial_step res, bool profile_matching) {
  auto *block_type = getvalue_type({sort_category::Symbol, 0}, module);
  std::vector<llvm::Type *> arg_types;
  std::vector<llvm::Metadata *> debug_types;
  for (auto const &res : res.residuals) {
    auto *arg_sort
        = dynamic_cast<kore_composite_sort *>(res.pattern->get_sort().get());
    auto cat = arg_sort->get_category(definition);
    debug_types.push_back(get_debug_type(cat, ast_to_string(*arg_sort)));
    switch (cat.cat) {
    case sort_category::Map:
    case sort_category::RangeMap:
    case sort_category::List:
    case sort_category::Set:
      arg_types.push_back(llvm::PointerType::getUnqual(module->getContext()));
      break;
    default: arg_types.push_back(getvalue_type(cat, module)); break;
    }
  }
  auto *block_debug_type
      = get_debug_type({sort_category::Symbol, 0}, "SortGeneratedTopCell{}");
  llvm::FunctionType *func_type
      = llvm::FunctionType::get(block_type, arg_types, false);
  std::string name = "step_" + std::to_string(axiom->get_ordinal());
  llvm::Function *match_func = get_or_insert_function(module, name, func_type);
  reset_debug_loc();
  init_debug_function(
      name, name, get_debug_function_type(block_debug_type, debug_types),
      definition, match_func);
  match_func->setCallingConv(llvm::CallingConv::Tail);

  llvm::StringMap<llvm::Value *> stuck_subst;
  llvm::BasicBlock *block
      = llvm::BasicBlock::Create(module->getContext(), "entry", match_func);
  llvm::BasicBlock *stuck
      = llvm::BasicBlock::Create(module->getContext(), "stuck", match_func);
  llvm::BasicBlock *pre_stuck
      = llvm::BasicBlock::Create(module->getContext(), "pre_stuck", match_func);
  llvm::BasicBlock *fail
      = llvm::BasicBlock::Create(module->getContext(), "fail", match_func);

  llvm::AllocaInst *choice_buffer = nullptr;
  llvm::AllocaInst *choice_depth = nullptr;
  llvm::IndirectBrInst *jump = nullptr;
  init_choice_buffer(
      res.dt, module, block, pre_stuck, fail, &choice_buffer, &choice_depth,
      &jump, profile_matching);
  insert_call_to_clear(block);

  llvm::BranchInst::Create(stuck, pre_stuck);
  std::vector<llvm::PHINode *> phis;
  int i = 0;
  std::vector<llvm::Value *> args;
  std::vector<value_type> types;
  for (auto *val = match_func->arg_begin(); val != match_func->arg_end();
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
    init_debug_param(
        match_func, i, "_" + std::to_string(i + 1), cat, ast_to_string(*sort));
  }
  auto header = step_function_header(
      axiom->get_ordinal(), module, definition, block, stuck, args, types);
  i = 0;
  decision codegen(
      definition, header.second, fail, jump, choice_buffer, choice_depth,
      module, {sort_category::Symbol, 0}, nullptr, nullptr, nullptr, nullptr,
      profile_matching);
  for (auto *val : header.first) {
    val->setName(res.residuals[i].occurrence.substr(0, max_name_length));
    codegen.store(std::make_pair(val->getName().str(), val->getType()), val);
    stuck_subst.insert({val->getName(), phis[i]});
    phis[i++]->addIncoming(val, pre_stuck);
  }
  std::set<std::string> occurrences;
  for (auto const &residual : res.residuals) {
    occurrences.insert(residual.occurrence);
  }
  kore_pattern *partial_term = make_partial_term(
      dynamic_cast<kore_pattern *>(axiom->get_right_hand_side()), occurrences,
      "_1");
  create_term creator(stuck_subst, definition, stuck, module, false);
  llvm::Value *retval = creator(partial_term).first;
  llvm::ReturnInst::Create(
      module->getContext(), retval, creator.get_current_block());

  codegen(res.dt);
}
} // namespace kllvm
