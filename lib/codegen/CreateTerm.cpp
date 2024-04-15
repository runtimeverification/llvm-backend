#include "kllvm/codegen/CreateTerm.h"
#include "kllvm/codegen/CreateStaticTerm.h"
#include "kllvm/codegen/Debug.h"
#include "kllvm/codegen/ProofEvent.h"
#include "kllvm/codegen/Util.h"

#include <fmt/format.h>
#include <gmp.h>
#include <iomanip>
#include <iostream>

#include "runtime/header.h" //for macros

#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IRReader/IRReader.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Support/SourceMgr.h"
#include "llvm/Support/raw_ostream.h"

namespace kllvm {

using namespace fmt::literals;

namespace {
std::string llvm_header() {
  auto target_dependent = fmt::format(
      R"LLVM(
; Target determined during CMake build
target datalayout = "{datalayout}"
target triple = "{triple}"
  )LLVM",
      "datalayout"_a = BACKEND_TARGET_DATALAYOUT,
      "triple"_a = BACKEND_TARGET_TRIPLE);

  auto const *rest =
      R"LLVM(
; K types in LLVM

; A K value in the LLVM backend can be one of the following values:

; an uninterpreted domain value: \dv{Id}("foo")
; a symbol with 0 or more arguments: foo{}()
; a map: dotMap{}()
; an associative list: dotList{}()
; a set: dotSet{}()
; an array: dotArray{}()
; an arbitrary precision integer: \dv{Int}("0")
; an arbitrary precision float: \dv{Float}("0.0")
; a domain string: \dv{String}("\"\"")
; a byte array: \dv{Bytes}("b\"\"")
; a string buffer: \dv{StringBuffer}("")
; a domain boolean: \dv{Bool}("false")
; a machine integer: \dv{MInt}("0p8")

; For each type, a value of that type has the following llvm type:

; token: %string *
; symbol with 0 arguments: i32
; symbol with 1 or more arguments: %block *
; map: %map
; list: %list
; set: %set
; array: %list
; integer: %mpz *
; float: %floating *
; string: %string *
; bytes: %string *
; string buffer: %stringbuffer *
; boolean: i1
; machine integer of N bits: iN

; We also define the following LLVM structure types:

%string = type { %blockheader, [0 x i8] } ; 10-bit layout, 4-bit gc flags, 10 unused bits, 40-bit length (or buffer capacity for string pointed by stringbuffers), bytes
%stringbuffer = type { i64, i64, %string* } ; 10-bit layout, 4-bit gc flags, 10 unused bits, 40-bit length, string length, current contents
%map = type { { i8 *, i64 } } ; immer::map
%rangemap = type { { { { { i32 (...)**, i32, i64 }*, { { i32 (...)**, i32, i32 }* } } } } } ; rng_map::RangeMap
%set = type { { i8 *, i64 } } ; immer::set
%iter = type { { i8 *, i8 *, i32, [14 x i8**] }, { { i8 *, i64 } } } ; immer::map_iter / immer::set_iter
%list = type { { i64, i32, i8 *, i8 * } } ; immer::flex_vector
%mpz = type { i32, i32, i64 * } ; mpz_t
%mpz_hdr = type { %blockheader, %mpz } ; 10-bit layout, 4-bit gc flags, 10 unused bits, 40-bit length, mpz_t
%floating = type { i64, { i64, i32, i64, i64 * } } ; exp, mpfr_t
%floating_hdr = type { %blockheader, %floating } ; 10-bit layout, 4-bit gc flags, 10 unused bits, 40-bit length, floating
%blockheader = type { i64 }
%block = type { %blockheader, [0 x i64 *] } ; 16-bit layout, 8-bit length, 32-bit tag, children

%layout = type { i8, %layoutitem* } ; number of children, array of children
%layoutitem = type { i64, i16 } ; offset, category

; The layout of a block uniquely identifies the categories of its children as
; well as how to allocate/deallocate them and whether to follow their pointers
; during gc. Roughly speaking, the following rules apply:

; %string *: malloc/free, do not follow
; iN: noop, do not follow
; %map, %set, %list: noop/drop_in_place, follow
; %block *: managed heap, follow
; %mpz *: malloc/mpz_clear->free, do not follow
; %floating *: malloc/mpfr_clear->free, do not follow
; %stringbuffer *: malloc->malloc/free->free, do not follow

; We also automatically generate for each unique layout id a struct type
; corresponding to the actual layout of that block. For example, if we have
; the symbol symbol foo{Map{}, Int{}, Exp{}} : Exp{}, we would generate the type:

; %layoutN = type { %blockheader, [0 x i64 *], %map, %mpz *, %block * }

; Interface to the configuration parser
declare %block* @parse_configuration(i8*)
declare void @print_configuration(i8 *, %block *)
)LLVM";
  return target_dependent + rest;
}
} // namespace

std::unique_ptr<llvm::Module>
new_module(std::string const &name, llvm::LLVMContext &context) {
  llvm::SMDiagnostic err;
  auto mod = llvm::parseIR(
      *llvm::MemoryBuffer::getMemBuffer(llvm_header()), err, context);
  if (!mod) {
    err.print("header.ll", llvm::errs());
  }
  return mod;
}

std::string map_struct = "map";
std::string rangemap_struct = "rangemap";
std::string list_struct = "list";
std::string set_struct = "set";
std::string int_wrapper_struct = "mpz_hdr";
std::string int_struct = "mpz";
std::string float_wrapper_struct = "floating_hdr";
std::string float_struct = "floating";
std::string buffer_struct = "stringbuffer";
std::string block_struct = "block";
std::string blockheader_struct = "blockheader";

llvm::Type *get_param_type(value_type sort, llvm::Module *module) {
  llvm::Type *type = getvalue_type(sort, module);
  switch (sort.cat) {
  case sort_category::Map:
  case sort_category::RangeMap:
  case sort_category::List:
  case sort_category::Set: type = llvm::PointerType::getUnqual(type); break;
  default: break;
  }
  return type;
}

llvm::StructType *get_block_type(llvm::Module *module) {
  return llvm::StructType::getTypeByName(module->getContext(), block_struct);
}

llvm::Type *getvalue_type(value_type sort, llvm::Module *module) {
  switch (sort.cat) {
  case sort_category::Map:
    return llvm::StructType::getTypeByName(module->getContext(), map_struct);
  case sort_category::RangeMap:
    return llvm::StructType::getTypeByName(
        module->getContext(), rangemap_struct);
  case sort_category::List:
    return llvm::StructType::getTypeByName(module->getContext(), list_struct);
  case sort_category::Set:
    return llvm::StructType::getTypeByName(module->getContext(), set_struct);
  case sort_category::Int:
    return llvm::PointerType::getUnqual(
        llvm::StructType::getTypeByName(module->getContext(), int_struct));
  case sort_category::Float:
    return llvm::PointerType::getUnqual(
        llvm::StructType::getTypeByName(module->getContext(), float_struct));
  case sort_category::StringBuffer:
    return llvm::PointerType::getUnqual(
        llvm::StructType::getTypeByName(module->getContext(), buffer_struct));
  case sort_category::Bool: return llvm::Type::getInt1Ty(module->getContext());
  case sort_category::MInt:
    return llvm::IntegerType::get(module->getContext(), sort.bits);
  case sort_category::Symbol:
  case sort_category::Variable:
    return llvm::PointerType::getUnqual(
        llvm::StructType::getTypeByName(module->getContext(), block_struct));
  case sort_category::Uncomputed: abort();
  }
}

llvm::StructType *get_block_type(
    llvm::Module *module, kore_definition *definition,
    kore_symbol const *symbol) {
  llvm::StructType *block_header_type = llvm::StructType::getTypeByName(
      module->getContext(), blockheader_struct);
  llvm::ArrayType *empty_array_type
      = llvm::ArrayType::get(llvm::Type::getInt64Ty(module->getContext()), 0);
  llvm::SmallVector<llvm::Type *, 4> types;
  types.push_back(block_header_type);
  types.push_back(empty_array_type);
  for (auto const &arg : symbol->get_arguments()) {
    auto *sort = dynamic_cast<kore_composite_sort *>(arg.get());
    auto cat = sort->get_category(definition);
    switch (cat.cat) {
    case sort_category::Map:
    case sort_category::RangeMap:
    case sort_category::List:
    case sort_category::Set:
      types.push_back(llvm::Type::getInt64Ty(module->getContext()));
      break;
    default: break;
    }
    llvm::Type *type = getvalue_type(sort->get_category(definition), module);
    types.push_back(type);
  }
  return llvm::StructType::get(module->getContext(), types);
}

uint64_t get_block_offset(
    kore_definition *definition, kore_symbol const *symbol, int idx) {
  uint64_t result = 2;
  int i = 0;
  for (auto const &arg : symbol->get_arguments()) {
    auto *sort = dynamic_cast<kore_composite_sort *>(arg.get());
    auto cat = sort->get_category(definition);
    switch (cat.cat) {
    case sort_category::Map:
    case sort_category::RangeMap:
    case sort_category::List:
    case sort_category::Set:
      if (i == idx) {
        return result + 1;
      }
      result += 2;
      break;
    default:
      if (i == idx) {
        return result;
      }
      result += 1;
      break;
    }
    i++;
  }
  throw std::invalid_argument("idx not within bounds of symbol");
}

uint64_t get_block_header_val(
    llvm::Module *module, kore_symbol const *symbol, llvm::Type *block_type) {
  uint64_t header_val = symbol->get_tag();
  uint64_t size_in_bytes
      = llvm::DataLayout(module).getTypeAllocSize(block_type);
  assert(size_in_bytes % 8 == 0);
  header_val |= (size_in_bytes / 8) << 32;
  header_val |= (uint64_t)symbol->get_layout() << LAYOUT_OFFSET;
  return header_val;
}

llvm::Value *get_block_header(
    llvm::Module *module, kore_definition *definition,
    kore_symbol const *symbol, llvm::Type *block_type) {
  llvm::StructType *block_header_type = llvm::StructType::getTypeByName(
      module->getContext(), blockheader_struct);
  uint64_t header_val = get_block_header_val(module, symbol, block_type);
  return llvm::ConstantStruct::get(
      block_header_type,
      llvm::ConstantInt::get(
          llvm::Type::getInt64Ty(module->getContext()), header_val));
}

llvm::Value *allocate_term(
    llvm::Type *alloc_type, llvm::BasicBlock *block, char const *alloc_fn) {
  return allocate_term(
      alloc_type, llvm::ConstantExpr::getSizeOf(alloc_type), block, alloc_fn);
}

llvm::Value *allocate_term(
    llvm::Type *alloc_type, llvm::Value *len, llvm::BasicBlock *block,
    char const *alloc_fn) {
  llvm::Instruction *malloc = llvm::CallInst::CreateMalloc(
      block, llvm::Type::getInt64Ty(block->getContext()), alloc_type, len,
      nullptr, kore_heap_alloc(alloc_fn, block->getModule()));

  if (!block->empty()) {
    set_debug_loc(&block->back());
  }

#if LLVM_VERSION_MAJOR < 16
  malloc->insertAfter(&block->back());
#else
  malloc->insertInto(block, block->end());
#endif

  return malloc;
}

value_type term_type(
    kore_pattern *pattern, llvm::StringMap<value_type> &substitution,
    kore_definition *definition) {
  if (auto *variable = dynamic_cast<kore_variable_pattern *>(pattern)) {
    return substitution.lookup(variable->get_name());
  }
  if (auto *constructor = dynamic_cast<kore_composite_pattern *>(pattern)) {
    kore_symbol *symbol = constructor->get_constructor();
    assert(symbol->is_concrete() && "not supported yet: sort variables");
    if (symbol->get_name() == "\\dv") {
      auto *sort = dynamic_cast<kore_composite_sort *>(
          symbol->get_formal_arguments()[0].get());
      return sort->get_category(definition);
    }
    auto *sort = dynamic_cast<kore_composite_sort *>(symbol->get_sort().get());
    return sort->get_category(definition);
  }
  assert(false && "not supported yet: meta level");
  abort();
}

sptr<kore_sort> term_sort(kore_pattern *pattern) {
  if (auto *variable = dynamic_cast<kore_variable_pattern *>(pattern)) {
    return variable->get_sort();
  }
  if (auto *constructor = dynamic_cast<kore_composite_pattern *>(pattern)) {
    kore_symbol *symbol = constructor->get_constructor();
    assert(symbol->is_concrete() && "not supported yet: sort variables");
    if (symbol->get_name() == "\\dv") {
      return symbol->get_formal_arguments()[0];
    }
    return symbol->get_sort();
  }
  assert(false && "not supported yet: meta level");
  abort();
}

llvm::Value *create_term::alloc_arg(
    kore_composite_pattern *pattern, int idx, bool is_hook_arg,
    std::string const &location_stack) {
  kore_pattern *p = pattern->get_arguments()[idx].get();
  std::string new_location = fmt::format("{}:{}", location_stack, idx);
  if (is_injection_symbol(p, definition_->get_inj_symbol())) {
    new_location = location_stack;
  }
  llvm::Value *ret = create_allocation(p, new_location).first;
  auto *sort = dynamic_cast<kore_composite_sort *>(p->get_sort().get());
  proof_event e(definition_, module_);
  current_block_ = e.argument(ret, sort, is_hook_arg, current_block_);
  return ret;
}

std::string escape(std::string const &str) {
  std::stringstream os;
  os << std::setfill('0') << std::setw(2) << std::hex;
  for (char c : str) {
    unsigned char uc = c;
    os << (int)uc;
  }
  return os.str();
}

// NOLINTNEXTLINE(*-cognitive-complexity)
llvm::Value *create_term::create_hook(
    kore_composite_pattern *hook_att, kore_composite_pattern *pattern,
    std::string const &location_stack) {
  assert(hook_att->get_arguments().size() == 1);
  auto *str_pattern
      = dynamic_cast<kore_string_pattern *>(hook_att->get_arguments()[0].get());
  std::string name = str_pattern->get_contents();
  if (name == "BOOL.and" || name == "BOOL.andThen") {
    assert(pattern->get_arguments().size() == 2);
    llvm::Value *first_arg = alloc_arg(pattern, 0, true, location_stack);
    llvm::BasicBlock *cond_block = current_block_;
    llvm::BasicBlock *true_block
        = llvm::BasicBlock::Create(ctx_, "then", current_block_->getParent());
    llvm::BasicBlock *merge_block = llvm::BasicBlock::Create(
        ctx_, "hook_BOOL_and", current_block_->getParent());
    llvm::BranchInst::Create(
        true_block, merge_block, first_arg, current_block_);
    current_block_ = true_block;
    llvm::Value *second_arg = alloc_arg(pattern, 1, true, location_stack);
    llvm::BranchInst::Create(merge_block, current_block_);
    llvm::PHINode *phi = llvm::PHINode::Create(
        llvm::Type::getInt1Ty(ctx_), 2, "phi", merge_block);
    phi->addIncoming(second_arg, current_block_);
    phi->addIncoming(first_arg, cond_block);
    current_block_ = merge_block;
    return phi;
  }
  if (name == "BOOL.or" || name == "BOOL.orElse") {
    assert(pattern->get_arguments().size() == 2);
    llvm::Value *first_arg = alloc_arg(pattern, 0, true, location_stack);
    llvm::BasicBlock *cond_block = current_block_;
    llvm::BasicBlock *false_block
        = llvm::BasicBlock::Create(ctx_, "else", current_block_->getParent());
    llvm::BasicBlock *merge_block = llvm::BasicBlock::Create(
        ctx_, "hook_BOOL_or", current_block_->getParent());
    llvm::BranchInst::Create(
        merge_block, false_block, first_arg, current_block_);
    current_block_ = false_block;
    llvm::Value *second_arg = alloc_arg(pattern, 1, true, location_stack);
    llvm::BranchInst::Create(merge_block, current_block_);
    llvm::PHINode *phi = llvm::PHINode::Create(
        llvm::Type::getInt1Ty(ctx_), 2, "phi", merge_block);
    phi->addIncoming(second_arg, current_block_);
    phi->addIncoming(first_arg, cond_block);
    current_block_ = merge_block;
    return phi;
  }
  if (name == "BOOL.not") {
    assert(pattern->get_arguments().size() == 1);
    llvm::Value *arg = alloc_arg(pattern, 0, true, location_stack);
    llvm::BinaryOperator *neg = llvm::BinaryOperator::Create(
        llvm::Instruction::Xor, arg,
        llvm::ConstantInt::get(llvm::Type::getInt1Ty(ctx_), 1), "hook_BOOL_not",
        current_block_);
    return neg;
  }
  if (name == "BOOL.implies") {
    assert(pattern->get_arguments().size() == 2);
    llvm::Value *first_arg = alloc_arg(pattern, 0, true, location_stack);
    llvm::BasicBlock *cond_block = current_block_;
    llvm::BasicBlock *true_block
        = llvm::BasicBlock::Create(ctx_, "then", current_block_->getParent());
    llvm::BasicBlock *merge_block = llvm::BasicBlock::Create(
        ctx_, "hook_BOOL_implies", current_block_->getParent());
    llvm::BranchInst::Create(
        true_block, merge_block, first_arg, current_block_);
    current_block_ = true_block;
    llvm::Value *second_arg = alloc_arg(pattern, 1, true, location_stack);
    llvm::BranchInst::Create(merge_block, current_block_);
    llvm::PHINode *phi = llvm::PHINode::Create(
        llvm::Type::getInt1Ty(ctx_), 2, "phi", merge_block);
    phi->addIncoming(second_arg, current_block_);
    phi->addIncoming(
        llvm::ConstantInt::get(llvm::Type::getInt1Ty(ctx_), 1), cond_block);
    current_block_ = merge_block;
    return phi;
  }
  if (name == "BOOL.ne" || name == "BOOL.xor") {
    assert(pattern->get_arguments().size() == 2);
    llvm::Value *first_arg = alloc_arg(pattern, 0, true, location_stack);
    llvm::Value *second_arg = alloc_arg(pattern, 1, true, location_stack);
    llvm::BinaryOperator *xor_op = llvm::BinaryOperator::Create(
        llvm::Instruction::Xor, first_arg, second_arg, "hook_BOOL_ne",
        current_block_);
    return xor_op;
  }
  if (name == "BOOL.eq") {
    assert(pattern->get_arguments().size() == 2);
    llvm::Value *first_arg = alloc_arg(pattern, 0, true, location_stack);
    llvm::Value *second_arg = alloc_arg(pattern, 1, true, location_stack);
    auto *eq = new llvm::ICmpInst(
        *current_block_, llvm::CmpInst::ICMP_EQ, first_arg, second_arg,
        "hook_BOOL_eq");
    return eq;
  }
  if (name == "KEQUAL.ite") {
    assert(pattern->get_arguments().size() == 3);
    llvm::Value *cond = alloc_arg(pattern, 0, true, location_stack);
    llvm::BasicBlock *true_block
        = llvm::BasicBlock::Create(ctx_, "then", current_block_->getParent());
    llvm::BasicBlock *false_block
        = llvm::BasicBlock::Create(ctx_, "else", current_block_->getParent());
    llvm::BasicBlock *merge_block = llvm::BasicBlock::Create(
        ctx_, "hook_KEQUAL_ite", current_block_->getParent());
    llvm::BranchInst::Create(true_block, false_block, cond, current_block_);
    current_block_ = true_block;
    llvm::Value *true_arg = alloc_arg(pattern, 1, true, location_stack);
    llvm::BasicBlock *new_true_block = current_block_;
    current_block_ = false_block;
    llvm::Value *false_arg = alloc_arg(pattern, 2, true, location_stack);
    if (true_arg->getType()->isPointerTy()
        && !false_arg->getType()->isPointerTy()) {
      auto *alloc_collection
          = new llvm::AllocaInst(false_arg->getType(), 0, "", current_block_);
      new llvm::StoreInst(false_arg, alloc_collection, current_block_);
      false_arg = alloc_collection;
    } else if (
        !true_arg->getType()->isPointerTy()
        && false_arg->getType()->isPointerTy()) {
      auto *alloc_collection
          = new llvm::AllocaInst(true_arg->getType(), 0, "", new_true_block);
      new llvm::StoreInst(true_arg, alloc_collection, new_true_block);
      true_arg = alloc_collection;
    }
    llvm::BranchInst::Create(merge_block, current_block_);
    llvm::BranchInst::Create(merge_block, new_true_block);
    llvm::PHINode *phi
        = llvm::PHINode::Create(true_arg->getType(), 2, "phi", merge_block);
    phi->addIncoming(true_arg, new_true_block);
    phi->addIncoming(false_arg, current_block_);
    current_block_ = merge_block;
    return phi;
  }
  if (name == "MINT.uvalue") {
    llvm::Value *mint = alloc_arg(pattern, 0, true, location_stack);
    value_type cat = dynamic_cast<kore_composite_sort *>(
                         pattern->get_constructor()->get_arguments()[0].get())
                         ->get_category(definition_);
    auto *type = getvalue_type(cat, module_);
    size_t nwords = (cat.bits + 63) / 64;
    if (nwords == 0) {
      auto *static_term = new create_static_term(definition_, module_);
      return static_term->create_token({sort_category::Int, 0}, "0");
    }
    auto *ptr = allocate_term(
        llvm::Type::getInt64Ty(ctx_),
        llvm::ConstantInt::get(llvm::Type::getInt64Ty(ctx_), nwords * 8),
        current_block_, "kore_alloc_always_gc");
    if (nwords == 1) {
      llvm::Value *word = nullptr;
      if (cat.bits == 64) {
        word = mint;
      } else {
        word = new llvm::ZExtInst(
            mint, llvm::Type::getInt64Ty(ctx_), "word", current_block_);
      }
      new llvm::StoreInst(word, ptr, current_block_);
    } else { // nwords >= 2
      llvm::Value *ptr2 = ptr;
      llvm::Value *accum = mint;
      for (size_t i = 0; i < nwords; i++) {
        auto *word = new llvm::TruncInst(
            accum, llvm::Type::getInt64Ty(ctx_), "word", current_block_);
        new llvm::StoreInst(word, ptr2, current_block_);
        ptr2 = llvm::GetElementPtrInst::Create(
            llvm::Type::getInt64Ty(ctx_), ptr2,
            {llvm::ConstantInt::get(llvm::Type::getInt64Ty(ctx_), 1)}, "ptr",
            current_block_);
        accum = llvm::BinaryOperator::Create(
            llvm::Instruction::LShr, accum, llvm::ConstantInt::get(type, 64),
            "shift", current_block_);
      }
    }
    auto *result = llvm::CallInst::Create(
        get_or_insert_function(
            module_, "hook_MINT_import",
            getvalue_type({sort_category::Int, 0}, module_),
            llvm::Type::getInt64PtrTy(ctx_), llvm::Type::getInt64Ty(ctx_),
            llvm::Type::getInt1Ty(ctx_)),
        {ptr, llvm::ConstantInt::get(llvm::Type::getInt64Ty(ctx_), cat.bits),
         llvm::ConstantInt::getFalse(ctx_)},
        "hook_MINT_uvalue", current_block_);
    set_debug_loc(result);
    return result;
  }
  if (name == "MINT.svalue") {
    llvm::Value *mint = alloc_arg(pattern, 0, true, location_stack);
    value_type cat = dynamic_cast<kore_composite_sort *>(
                         pattern->get_constructor()->get_arguments()[0].get())
                         ->get_category(definition_);
    auto *type = getvalue_type(cat, module_);
    size_t nwords = (cat.bits + 63) / 64;
    if (nwords == 0) {
      auto *static_term = new create_static_term(definition_, module_);
      return static_term->create_token({sort_category::Int, 0}, "0");
    }
    auto *ptr = allocate_term(
        llvm::Type::getInt64Ty(ctx_),
        llvm::ConstantInt::get(llvm::Type::getInt64Ty(ctx_), nwords * 8),
        current_block_, "kore_alloc_always_gc");
    if (nwords == 1) {
      llvm::Value *word = nullptr;
      if (cat.bits == 64) {
        word = mint;
      } else {
        word = new llvm::SExtInst(
            mint, llvm::Type::getInt64Ty(ctx_), "word", current_block_);
      }
      new llvm::StoreInst(word, ptr, current_block_);
    } else { // nwords >= 2
      llvm::Value *ptr2 = ptr;
      llvm::Value *accum = mint;
      for (size_t i = 0; i < nwords; i++) {
        auto *word = new llvm::TruncInst(
            accum, llvm::Type::getInt64Ty(ctx_), "word", current_block_);
        new llvm::StoreInst(word, ptr2, current_block_);
        ptr2 = llvm::GetElementPtrInst::Create(
            llvm::Type::getInt64Ty(ctx_), ptr2,
            {llvm::ConstantInt::get(llvm::Type::getInt64Ty(ctx_), 1)}, "ptr",
            current_block_);
        accum = llvm::BinaryOperator::Create(
            llvm::Instruction::AShr, accum, llvm::ConstantInt::get(type, 64),
            "shift", current_block_);
      }
    }
    auto *result = llvm::CallInst::Create(
        get_or_insert_function(
            module_, "hook_MINT_import",
            getvalue_type({sort_category::Int, 0}, module_),
            llvm::Type::getInt64PtrTy(ctx_), llvm::Type::getInt64Ty(ctx_),
            llvm::Type::getInt1Ty(ctx_)),
        {ptr, llvm::ConstantInt::get(llvm::Type::getInt64Ty(ctx_), cat.bits),
         llvm::ConstantInt::getTrue(ctx_)},
        "hook_MINT_svalue", current_block_);
    set_debug_loc(result);
    return result;
  }
  if (name == "MINT.integer") {
    llvm::Value *mpz = alloc_arg(pattern, 0, true, location_stack);
    value_type cat = dynamic_cast<kore_composite_sort *>(
                         pattern->get_constructor()->get_sort().get())
                         ->get_category(definition_);
    auto *type = getvalue_type(cat, module_);
    llvm::Instruction *ptr = llvm::CallInst::Create(
        get_or_insert_function(
            module_, "hook_MINT_export", llvm::Type::getInt64PtrTy(ctx_),
            getvalue_type({sort_category::Int, 0}, module_),
            llvm::Type::getInt64Ty(ctx_)),
        {mpz, llvm::ConstantInt::get(llvm::Type::getInt64Ty(ctx_), cat.bits)},
        "ptr", current_block_);
    set_debug_loc(ptr);
    size_t nwords = (cat.bits + 63) / 64;
    llvm::Value *result = llvm::ConstantInt::get(type, 0);
    if (nwords == 0) {
      return result;
    }

    if (nwords == 1) {
      auto *word = new llvm::LoadInst(
          llvm::Type::getInt64Ty(ctx_), ptr, "word", current_block_);
      if (cat.bits == 64) {
        return word;
      }

      return new llvm::TruncInst(
          word, type, "hook_MINT_integer", current_block_);
    }
    // nwords >= 2
    for (size_t i = 0; i < nwords; i++) {
      auto *word = new llvm::LoadInst(
          llvm::Type::getInt64Ty(ctx_), ptr, "word", current_block_);
      auto *zext = new llvm::ZExtInst(word, type, "extended", current_block_);
      auto *shl = llvm::BinaryOperator::Create(
          llvm::Instruction::Shl, result, llvm::ConstantInt::get(type, 64),
          "shift", current_block_);
      result = llvm::BinaryOperator::Create(
          llvm::Instruction::Or, shl, zext, "or", current_block_);
      ptr = llvm::GetElementPtrInst::Create(
          llvm::Type::getInt64Ty(ctx_), ptr,
          {llvm::ConstantInt::get(llvm::Type::getInt64Ty(ctx_), 1)}, "ptr",
          current_block_);
    }
    return result;
  }
  if (name == "MINT.neg") {
    llvm::Value *in = alloc_arg(pattern, 0, true, location_stack);
    return llvm::BinaryOperator::CreateNeg(in, "hook_MINT_neg", current_block_);
  }
  if (name == "MINT.not") {
    llvm::Value *in = alloc_arg(pattern, 0, true, location_stack);
    return llvm::BinaryOperator::CreateNot(in, "hook_MINT_not", current_block_);
#define MINT_CMP(hookname, inst)                                               \
  }                                                                            \
  if (name == "MINT." #hookname) {                                             \
    llvm::Value *first = alloc_arg(pattern, 0, true, location_stack);          \
    llvm::Value *second = alloc_arg(pattern, 1, true, location_stack);         \
  return new llvm::ICmpInst(                                                   \
      *current_block_, llvm::CmpInst::inst, first, second,                     \
      "hook_MINT_" #hookname)
    MINT_CMP(eq, ICMP_EQ);
    MINT_CMP(ne, ICMP_NE);
    MINT_CMP(ult, ICMP_ULT);
    MINT_CMP(ule, ICMP_ULE);
    MINT_CMP(ugt, ICMP_UGT);
    MINT_CMP(uge, ICMP_UGE);
    MINT_CMP(slt, ICMP_SLT);
    MINT_CMP(sle, ICMP_SLE);
    MINT_CMP(sgt, ICMP_SGT);
    MINT_CMP(sge, ICMP_SGE);
#define MINT_BINOP(hookname, inst)                                             \
  }                                                                            \
  if (name == "MINT." #hookname) {                                             \
    llvm::Value *first = alloc_arg(pattern, 0, true, location_stack);          \
    llvm::Value *second = alloc_arg(pattern, 1, true, location_stack);         \
  return llvm::BinaryOperator::Create(                                         \
      llvm::Instruction::inst, first, second, "hook_MINT_" #hookname,          \
      current_block_)
    MINT_BINOP(xor, Xor);
    MINT_BINOP(or, Or);
    MINT_BINOP(and, And);
    MINT_BINOP(shl, Shl);
    MINT_BINOP(lshr, LShr);
    MINT_BINOP(ashr, AShr);
    MINT_BINOP(add, Add);
    MINT_BINOP(sub, Sub);
    MINT_BINOP(mul, Mul);
    MINT_BINOP(sdiv, SDiv);
    MINT_BINOP(udiv, UDiv);
    MINT_BINOP(srem, SRem);
    MINT_BINOP(urem, URem);
  }
  if (!name.compare(0, 5, "MINT.")) {
    std::cerr << name << std::endl;
    assert(false && "not implemented yet: MInt");
    abort();
  }
  std::string hook_name = "hook_" + name.substr(0, name.find('.')) + "_"
                          + name.substr(name.find('.') + 1);
  return create_function_call(
      hook_name, pattern, true, false, true, location_stack);
}

// We use tailcc calling convention for apply_rule_* and eval_* functions to
// make these K functions tail recursive when their K definitions are tail
// recursive.
llvm::Value *create_term::create_function_call(
    std::string const &name, kore_composite_pattern *pattern, bool sret,
    bool tailcc, bool is_hook, std::string const &location_stack) {
  auto event = proof_event(definition_, module_);

  current_block_
      = event.function_event_pre(current_block_, pattern, location_stack);

  std::vector<llvm::Value *> args;
  auto *return_sort = dynamic_cast<kore_composite_sort *>(
      pattern->get_constructor()->get_sort().get());
  auto return_cat = return_sort->get_category(definition_);
  int i = 0;
  for (auto const &sort : pattern->get_constructor()->get_arguments()) {
    auto *concrete_sort = dynamic_cast<kore_composite_sort *>(sort.get());
    llvm::Value *arg = alloc_arg(pattern, i, false, location_stack);
    i++;
    switch (concrete_sort->get_category(definition_).cat) {
    case sort_category::Map:
    case sort_category::RangeMap:
    case sort_category::List:
    case sort_category::Set: {
      if (!arg->getType()->isPointerTy()) {
        auto *alloc_collection
            = new llvm::AllocaInst(arg->getType(), 0, "", current_block_);
        new llvm::StoreInst(arg, alloc_collection, current_block_);
        args.push_back(alloc_collection);
      } else {
        args.push_back(arg);
      }
      break;
    }
    default: args.push_back(arg); break;
    }
  }

  current_block_ = event.function_event_post(current_block_);

  if (is_hook) {
    int i = 0;
    for (auto const &p : pattern->get_arguments()) {
      auto *sort = dynamic_cast<kore_composite_sort *>(p->get_sort().get());
      proof_event e(definition_, module_);
      current_block_ = e.argument(args[i], sort, true, current_block_);
      i++;
    }
  }

  return create_function_call(
      name, return_cat, args, sret, tailcc, location_stack);
}

llvm::Value *create_term::create_function_call(
    std::string const &name, value_type return_cat,
    std::vector<llvm::Value *> const &args, bool sret, bool tailcc,
    std::string const &location_stack) {
  llvm::Type *return_type = getvalue_type(return_cat, module_);
  std::vector<llvm::Type *> types;
  bool collection = false;
  switch (return_cat.cat) {
  case sort_category::Map:
  case sort_category::RangeMap:
  case sort_category::List:
  case sort_category::Set: collection = true; break;
  default: sret = false; break;
  }
  llvm::Value *alloc_sret = nullptr;
  types.reserve(args.size());
  for (auto *arg : args) {
    types.push_back(arg->getType());
  }
  std::vector<llvm::Value *> real_args = args;
  llvm::Type *sret_type = nullptr;
  if (sret) {
    // we don't use alloca here because the tail call optimization pass for llvm
    // doesn't handle correctly functions with alloca
    alloc_sret
        = allocate_term(return_type, current_block_, "kore_alloc_always_gc");
    sret_type = return_type;
    real_args.insert(real_args.begin(), alloc_sret);
    types.insert(types.begin(), alloc_sret->getType());
    return_type = llvm::Type::getVoidTy(ctx_);
  } else if (collection) {
    return_type = llvm::PointerType::getUnqual(return_type);
  }

  llvm::FunctionType *func_type
      = llvm::FunctionType::get(return_type, types, false);
  llvm::Function *func = get_or_insert_function(module_, name, func_type);

  auto *call = llvm::CallInst::Create(func, real_args, "", current_block_);
  set_debug_loc(call);
  if (tailcc) {
    call->setCallingConv(llvm::CallingConv::Tail);
  }
  if (sret) {
    llvm::Attribute sret_attr
        = llvm::Attribute::get(ctx_, llvm::Attribute::StructRet, sret_type);
    func->arg_begin()->addAttr(sret_attr);
    call->addParamAttr(0, sret_attr);
    return alloc_sret;
  }
  return call;
}

/* create a term, given the assumption that the created term will not be a
 * triangle injection pair */
llvm::Value *create_term::not_injection_case(
    kore_composite_pattern *constructor, llvm::Value *val,
    std::string const &location_stack) {
  kore_symbol const *symbol = constructor->get_constructor();
  kore_symbol_declaration *symbol_decl
      = definition_->get_symbol_declarations().at(symbol->get_name());
  llvm::StructType *block_type = get_block_type(module_, definition_, symbol);
  llvm::Value *block_header
      = get_block_header(module_, definition_, symbol, block_type);
  int idx = 2;
  std::vector<llvm::Value *> children;
  for (auto const &child : constructor->get_arguments()) {
    llvm::Value *child_value = nullptr;
    if (idx == 2 && val != nullptr) {
      child_value = val;
    } else {
      std::string new_location = fmt::format("{}:{}", location_stack, idx - 2);
      if (is_injection_symbol(child.get(), definition_->get_inj_symbol())) {
        new_location = location_stack;
      }
      child_value = create_allocation(child.get(), new_location).first;
    }

    auto *sort = dynamic_cast<kore_composite_sort *>(child->get_sort().get());
    if (sort && is_collection_sort(sort->get_category(definition_))) {
      child_value = new llvm::LoadInst(
          block_type->elements()[idx], child_value, "", current_block_);
    }
    children.push_back(child_value);
    idx++;
  }
  llvm::Value *block = allocate_term(block_type, current_block_);
  llvm::Value *block_header_ptr = llvm::GetElementPtrInst::CreateInBounds(
      block_type, block,
      {llvm::ConstantInt::get(llvm::Type::getInt64Ty(ctx_), 0),
       llvm::ConstantInt::get(llvm::Type::getInt32Ty(ctx_), 0)},
      symbol->get_name(), current_block_);
  new llvm::StoreInst(block_header, block_header_ptr, current_block_);
  idx = 2;
  for (auto &child_value : children) {
    llvm::Value *child_ptr = llvm::GetElementPtrInst::CreateInBounds(
        block_type, block,
        {llvm::ConstantInt::get(llvm::Type::getInt64Ty(ctx_), 0),
         llvm::ConstantInt::get(llvm::Type::getInt32Ty(ctx_), idx++)},
        "", current_block_);
    new llvm::StoreInst(child_value, child_ptr, current_block_);
  }

  auto *block_ptr = llvm::PointerType::getUnqual(
      llvm::StructType::getTypeByName(module_->getContext(), block_struct));
  auto *bitcast = new llvm::BitCastInst(block, block_ptr, "", current_block_);
  if (symbol_decl->attributes().contains(attribute_set::key::Binder)) {
    auto *call = llvm::CallInst::Create(
        get_or_insert_function(module_, "debruijnize", block_ptr, block_ptr),
        bitcast, "withIndices", current_block_);
    set_debug_loc(call);
    return call;
  }
  return bitcast;
}

// returns a value and a boolean indicating whether that value could be an
// injection
std::pair<llvm::Value *, bool> create_term::operator()(kore_pattern *pattern) {
  populate_static_set(pattern);

  return create_allocation(pattern);
}

bool create_term::populate_static_set(kore_pattern *pattern) {
  bool can_be_static = true;

  if (auto *variable = dynamic_cast<kore_variable_pattern *>(pattern)) {
    can_be_static = false;
  } else if (
      auto *constructor = dynamic_cast<kore_composite_pattern *>(pattern)) {
    kore_symbol const *symbol = constructor->get_constructor();
    if (symbol->get_name() != "\\dv") {
      kore_symbol_declaration *symbol_decl
          = definition_->get_symbol_declarations().at(symbol->get_name());
      if (symbol_decl->attributes().contains(attribute_set::key::Function)
          || (symbol_decl->attributes().contains(attribute_set::key::Anywhere)
              && !is_anywhere_owise_)) {
        can_be_static = false;
      }
      for (auto const &sptr_next_pattern : constructor->get_arguments()) {
        kore_pattern *next_pattern = sptr_next_pattern.get();
        can_be_static &= populate_static_set(next_pattern);
      }
    }
  }

  if (can_be_static) {
    static_terms_.insert(pattern);
  }

  return can_be_static;
}

std::pair<llvm::Value *, bool> create_term::create_allocation(
    kore_pattern *pattern, std::string const &location_stack) {
  if (static_terms_.contains(pattern)) {
    auto *static_term = new create_static_term(definition_, module_);
    return (*static_term)(pattern);
  }

  if (auto *variable = dynamic_cast<kore_variable_pattern *>(pattern)) {
    auto *val = substitution_.lookup(variable->get_name());
    if (!val) {
      std::cerr << variable->get_name() << std::endl;
      abort();
    }
    return std::make_pair(val, true);
  }
  if (auto *constructor = dynamic_cast<kore_composite_pattern *>(pattern)) {
    kore_symbol const *symbol = constructor->get_constructor();
    assert(symbol->is_concrete() && "not supported yet: sort variables");
    kore_symbol_declaration *symbol_decl
        = definition_->get_symbol_declarations().at(symbol->get_name());
    if (symbol_decl->attributes().contains(attribute_set::key::Function)
        || (symbol_decl->attributes().contains(attribute_set::key::Anywhere)
            && !is_anywhere_owise_)) {
      if (symbol_decl->attributes().contains(attribute_set::key::Hook)) {
        auto *sort = dynamic_cast<kore_composite_sort *>(
            constructor->get_sort().get());
        auto *str_pattern = dynamic_cast<kore_string_pattern *>(
            symbol_decl->attributes()
                .get(attribute_set::key::Hook)
                ->get_arguments()[0]
                .get());
        std::string name = str_pattern->get_contents();

        proof_event p(definition_, module_);
        current_block_ = p.hook_event_pre(name, current_block_, location_stack);
        llvm::Value *val = create_hook(
            symbol_decl->attributes().get(attribute_set::key::Hook).get(),
            constructor, location_stack);
        current_block_ = p.hook_event_post(val, sort, current_block_);

        return std::make_pair(val, true);
      }
      auto fn_name = fmt::format("eval_{}", ast_to_string(*symbol, 0, false));
      return std::make_pair(
          create_function_call(
              fn_name, constructor, false, true, false, location_stack),
          true);
    }
    if (auto cat
        = dynamic_cast<kore_composite_sort *>(symbol->get_arguments()[0].get())
              ->get_category(definition_)
              .cat;
        symbol_decl->attributes().contains(attribute_set::key::SortInjection)
        && (cat == sort_category::Symbol)) {
      std::pair<llvm::Value *, bool> val = create_allocation(
          constructor->get_arguments()[0].get(), location_stack);
      if (val.second) {
        llvm::Instruction *tag = llvm::CallInst::Create(
            get_or_insert_function(
                module_, "get_tag", llvm::Type::getInt32Ty(ctx_),
                getvalue_type({sort_category::Symbol, 0}, module_)),
            val.first, "tag", current_block_);
        set_debug_loc(tag);
        auto *inj = definition_->get_inj_symbol();
        auto *not_string_block = llvm::BasicBlock::Create(
            ctx_, "notString", current_block_->getParent());
        auto *ge_block = llvm::BasicBlock::Create(
            ctx_, "geFirst", current_block_->getParent());
        auto *false_block = llvm::BasicBlock::Create(
            ctx_, "notInjection", current_block_->getParent());
        auto *true_block = llvm::BasicBlock::Create(
            ctx_, "merge", current_block_->getParent());
        auto *cmp = new llvm::ICmpInst(
            *current_block_, llvm::CmpInst::ICMP_NE, tag,
            llvm::ConstantInt::getSigned(llvm::Type::getInt32Ty(ctx_), -1));
        llvm::BranchInst::Create(
            not_string_block, false_block, cmp, current_block_);

        current_block_ = not_string_block;
        cmp = new llvm::ICmpInst(
            *current_block_, llvm::CmpInst::ICMP_UGE, tag,
            llvm::ConstantInt::get(
                llvm::Type::getInt32Ty(ctx_), inj->get_first_tag()));
        llvm::BranchInst::Create(ge_block, false_block, cmp, current_block_);

        current_block_ = ge_block;
        cmp = new llvm::ICmpInst(
            *current_block_, llvm::CmpInst::ICMP_ULE, tag,
            llvm::ConstantInt::get(
                llvm::Type::getInt32Ty(ctx_), inj->get_last_tag()));
        llvm::BranchInst::Create(true_block, false_block, cmp, current_block_);

        current_block_ = false_block;
        auto *cast = not_injection_case(constructor, val.first, location_stack);
        llvm::BranchInst::Create(true_block, current_block_);

        current_block_ = true_block;
        llvm::PHINode *phi
            = llvm::PHINode::Create(cast->getType(), 2, "phi", current_block_);
        phi->addIncoming(cast, false_block);
        phi->addIncoming(val.first, ge_block);
        return std::make_pair(phi, true);
      }
      return std::make_pair(
          not_injection_case(constructor, val.first, location_stack), true);
    }
    return std::make_pair(
        not_injection_case(constructor, nullptr, location_stack), false);
  }
  assert(false && "not supported yet: meta level");
  abort();
}

void add_abort(llvm::BasicBlock *block, llvm::Module *module) {
  llvm::FunctionType *abort_type = llvm::FunctionType::get(
      llvm::Type::getVoidTy(module->getContext()), false);
  llvm::Function *abort_func
      = get_or_insert_function(module, "abort", abort_type);
  abort_func->addFnAttr(llvm::Attribute::NoReturn);
  llvm::CallInst::Create(abort_func, "", block);
  new llvm::UnreachableInst(module->getContext(), block);
}

bool make_function(
    std::string const &name, kore_pattern *pattern, kore_definition *definition,
    llvm::Module *module, bool tailcc, bool big_step, bool apply,
    kore_axiom_declaration *axiom, std::string const &postfix) {
  std::map<std::string, kore_variable_pattern *> vars;
  if (apply) {
    for (kore_pattern *lhs : axiom->get_left_hand_side()) {
      lhs->mark_variables(vars);
    }
  }
  pattern->mark_variables(vars);
  llvm::StringMap<value_type> params;
  std::vector<llvm::Type *> param_types;
  std::vector<std::string> param_names;
  std::vector<llvm::Metadata *> debug_args;
  for (auto &entry : vars) {
    auto *sort
        = dynamic_cast<kore_composite_sort *>(entry.second->get_sort().get());
    if (!sort) {
      // TODO: sort variables
      return false;
    }
    auto cat = sort->get_category(definition);
    llvm::Type *param_type = getvalue_type(cat, module);
    debug_args.push_back(get_debug_type(cat, ast_to_string(*sort)));
    switch (cat.cat) {
    case sort_category::Map:
    case sort_category::RangeMap:
    case sort_category::List:
    case sort_category::Set:
      param_type = llvm::PointerType::getUnqual(param_type);
      break;
    default: break;
    }

    params.insert({entry.first, cat});
    param_types.push_back(param_type);
    param_names.push_back(entry.first);
  }
  value_type return_cat = term_type(pattern, params, definition);
  auto *return_type = getvalue_type(return_cat, module);
  switch (return_cat.cat) {
  case sort_category::Map:
  case sort_category::RangeMap:
  case sort_category::List:
  case sort_category::Set:
    return_type = llvm::PointerType::getUnqual(return_type);
    break;
  default: break;
  }
  llvm::FunctionType *func_type
      = llvm::FunctionType::get(return_type, param_types, false);
  llvm::Function *apply_rule = get_or_insert_function(module, name, func_type);
  init_debug_axiom(axiom->attributes());
  std::string debug_name = name;
  if (axiom->attributes().contains(attribute_set::key::Label)) {
    debug_name
        = axiom->attributes().get_string(attribute_set::key::Label) + postfix;
  }
  init_debug_function(
      debug_name, debug_name,
      get_debug_function_type(
          get_debug_type(return_cat, ast_to_string(*term_sort(pattern))),
          debug_args),
      definition, apply_rule);
  if (tailcc) {
    apply_rule->setCallingConv(llvm::CallingConv::Tail);
  }
  llvm::StringMap<llvm::Value *> subst;
  llvm::BasicBlock *block
      = llvm::BasicBlock::Create(module->getContext(), "entry", apply_rule);
  int i = 0;
  for (auto *val = apply_rule->arg_begin(); val != apply_rule->arg_end();
       ++val, ++i) {
    subst.insert({param_names[i], val});
    if (debug_args[i]) {
      init_debug_param(
          apply_rule, i, param_names[i], params[param_names[i]],
          llvm::cast<llvm::DIType>(debug_args[i])->getName().str());
    }
  }
  create_term creator = create_term(subst, definition, block, module, false);
  llvm::Value *retval = creator(pattern).first;

  auto *current_block = creator.get_current_block();
  if (apply && big_step) {
    current_block = proof_event(definition, module)
                        .rewrite_event_post(axiom, retval, current_block);
  }

  if (big_step) {
    llvm::Type *block_type = getvalue_type({sort_category::Symbol, 0}, module);
    llvm::Function *step = get_or_insert_function(
        module, "k_step",
        llvm::FunctionType::get(block_type, {block_type}, false));
    auto *call = llvm::CallInst::Create(step, {retval}, "", current_block);
    set_debug_loc(call);
    call->setCallingConv(llvm::CallingConv::Tail);
    retval = call;
  }
  auto *ret
      = llvm::ReturnInst::Create(module->getContext(), retval, current_block);
  set_debug_loc(ret);
  return true;
}

void make_apply_rule_function(
    kore_axiom_declaration *axiom, kore_definition *definition,
    llvm::Module *module, bool big_step) {
  kore_pattern *pattern = axiom->get_right_hand_side();
  std::string name = "apply_rule_" + std::to_string(axiom->get_ordinal());
  make_function(
      name, pattern, definition, module, true, big_step, true, axiom, ".rhs");
  if (big_step) {
    make_function(
        name + "_search", pattern, definition, module, true, false, true, axiom,
        ".rhs");
  }
}

std::string make_apply_rule_function(
    kore_axiom_declaration *axiom, kore_definition *definition,
    llvm::Module *module, std::vector<residual> const &residuals) {
  std::map<std::string, kore_variable_pattern *> vars;
  for (auto residual : residuals) {
    residual.pattern->mark_variables(vars);
  }
  for (kore_pattern *lhs : axiom->get_left_hand_side()) {
    lhs->mark_variables(vars);
  }
  llvm::StringMap<value_type> params;
  std::vector<llvm::Type *> param_types;
  std::vector<std::string> param_names;
  std::vector<llvm::Metadata *> debug_args;
  for (auto &entry : vars) {
    auto *sort
        = dynamic_cast<kore_composite_sort *>(entry.second->get_sort().get());
    if (!sort) {
      // TODO: sort variables
      return "";
    }
    auto cat = sort->get_category(definition);
    llvm::Type *param_type = getvalue_type(cat, module);
    debug_args.push_back(get_debug_type(cat, ast_to_string(*sort)));
    switch (cat.cat) {
    case sort_category::Map:
    case sort_category::RangeMap:
    case sort_category::List:
    case sort_category::Set:
      param_type = llvm::PointerType::getUnqual(param_type);
      break;
    default: break;
    }

    params.insert({entry.first, cat});
    param_types.push_back(param_type);
    param_names.push_back(entry.first);
  }
  llvm::FunctionType *func_type = llvm::FunctionType::get(
      getvalue_type({sort_category::Symbol, 0}, module), param_types, false);
  std::string name = "apply_rule_" + std::to_string(axiom->get_ordinal());

  make_function(
      name + "_search", axiom->get_right_hand_side(), definition, module, true,
      false, true, axiom, ".rhs");

  llvm::Function *apply_rule = get_or_insert_function(module, name, func_type);
  init_debug_axiom(axiom->attributes());
  init_debug_function(
      name, name,
      get_debug_function_type(
          get_debug_type({sort_category::Symbol, 0}, "SortGeneratedTopCell{}"),
          debug_args),
      definition, apply_rule);
  apply_rule->setCallingConv(llvm::CallingConv::Tail);
  llvm::StringMap<llvm::Value *> subst;
  llvm::BasicBlock *block
      = llvm::BasicBlock::Create(module->getContext(), "entry", apply_rule);
  int i = 0;
  for (auto *val = apply_rule->arg_begin(); val != apply_rule->arg_end();
       ++val, ++i) {
    subst.insert({param_names[i], val});
    if (debug_args[i]) {
      init_debug_param(
          apply_rule, i, param_names[i], params[param_names[i]],
          llvm::cast<llvm::DIType>(debug_args[i])->getName().str());
    }
  }
  create_term creator = create_term(subst, definition, block, module, false);
  std::vector<llvm::Value *> args;
  std::vector<llvm::Type *> types;
  for (auto const &residual : residuals) {
    llvm::Value *arg = creator(residual.pattern).first;
    auto *sort = dynamic_cast<kore_composite_sort *>(
        residual.pattern->get_sort().get());
    auto cat = sort->get_category(definition);
    switch (cat.cat) {
    case sort_category::Map:
    case sort_category::RangeMap:
    case sort_category::List:
    case sort_category::Set:
      if (!arg->getType()->isPointerTy()) {
        auto *ptr = allocate_term(
            arg->getType(), creator.get_current_block(),
            "kore_alloc_always_gc");
        new llvm::StoreInst(arg, ptr, creator.get_current_block());
        arg = ptr;
      }
      break;
    default: break;
    }
    args.push_back(arg);
    types.push_back(arg->getType());
  }
  llvm::Type *block_type = getvalue_type({sort_category::Symbol, 0}, module);
  llvm::Function *step = get_or_insert_function(
      module, "step_" + std::to_string(axiom->get_ordinal()),
      llvm::FunctionType::get(block_type, types, false));
  auto *retval
      = llvm::CallInst::Create(step, args, "", creator.get_current_block());
  set_debug_loc(retval);
  retval->setCallingConv(llvm::CallingConv::Tail);
  llvm::ReturnInst::Create(
      module->getContext(), retval, creator.get_current_block());
  return name;
}

std::string make_side_condition_function(
    kore_axiom_declaration *axiom, kore_definition *definition,
    llvm::Module *module) {
  kore_pattern *pattern = axiom->get_requires();

  if (!pattern) {
    return "";
  }
  std::string name = "side_condition_" + std::to_string(axiom->get_ordinal());
  if (make_function(
          name, pattern, definition, module, false, false, false, axiom,
          ".sc")) {
    return name;
  }
  return "";
}

llvm::Type *get_arg_type(value_type cat, llvm::Module *mod) {
  switch (cat.cat) {
  case sort_category::Bool:
  case sort_category::MInt:
  case sort_category::Map:
  case sort_category::RangeMap:
  case sort_category::List:
  case sort_category::Set: {
    return getvalue_type(cat, mod);
  }
  case sort_category::Int:
    return llvm::StructType::getTypeByName(mod->getContext(), int_struct);
  case sort_category::Float:
    return llvm::StructType::getTypeByName(mod->getContext(), float_struct);
  case sort_category::StringBuffer:
    return llvm::StructType::getTypeByName(mod->getContext(), buffer_struct);
  case sort_category::Symbol:
  case sort_category::Variable: {
    return get_block_type(mod);
  }

  case sort_category::Uncomputed:
  default: {
    abort();
  }
  }
}

bool is_collection_sort(value_type cat) {
  switch (cat.cat) {
  case sort_category::Map:
  case sort_category::RangeMap:
  case sort_category::List:
  case sort_category::Set: return true;
  default: return false;
  }
}

bool is_injection_symbol(kore_pattern *p, kore_symbol *sym) {
  if (auto *constructor = dynamic_cast<kore_composite_pattern *>(p)) {
    kore_symbol const *symbol = constructor->get_constructor();
    if (symbol->get_name() == sym->get_name()) {
      return true;
    }
  }
  return false;
}

} // namespace kllvm
