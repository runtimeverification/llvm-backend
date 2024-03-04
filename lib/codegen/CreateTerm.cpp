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
declare %block* @parseConfiguration(i8*)
declare void @printConfiguration(i8 *, %block *)
)LLVM";
  return target_dependent + rest;
}
} // namespace

std::unique_ptr<llvm::Module>
newModule(std::string const &name, llvm::LLVMContext &context) {
  llvm::SMDiagnostic Err;
  auto mod = llvm::parseIR(
      *llvm::MemoryBuffer::getMemBuffer(llvm_header()), Err, context);
  if (!mod) {
    Err.print("header.ll", llvm::errs());
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

llvm::Type *getParamType(value_type sort, llvm::Module *module) {
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

llvm::StructType *getBlockType(llvm::Module *module) {
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

llvm::StructType *getBlockType(
    llvm::Module *module, kore_definition *definition,
    kore_symbol const *symbol) {
  llvm::StructType *BlockHeaderType = llvm::StructType::getTypeByName(
      module->getContext(), blockheader_struct);
  llvm::ArrayType *EmptyArrayType
      = llvm::ArrayType::get(llvm::Type::getInt64Ty(module->getContext()), 0);
  llvm::SmallVector<llvm::Type *, 4> Types;
  Types.push_back(BlockHeaderType);
  Types.push_back(EmptyArrayType);
  for (auto const &arg : symbol->get_arguments()) {
    auto *sort = dynamic_cast<kore_composite_sort *>(arg.get());
    llvm::Type *type = getvalue_type(sort->get_category(definition), module);
    Types.push_back(type);
  }
  return llvm::StructType::get(module->getContext(), Types);
}

uint64_t getBlockHeaderVal(
    llvm::Module *module, kore_symbol const *symbol, llvm::Type *block_type) {
  uint64_t headerVal = symbol->get_tag();
  uint64_t sizeInBytes = llvm::DataLayout(module).getTypeAllocSize(block_type);
  assert(sizeInBytes % 8 == 0);
  headerVal |= (sizeInBytes / 8) << 32;
  headerVal |= (uint64_t)symbol->get_layout() << LAYOUT_OFFSET;
  return headerVal;
}

llvm::Value *getBlockHeader(
    llvm::Module *module, kore_definition *definition,
    kore_symbol const *symbol, llvm::Type *block_type) {
  llvm::StructType *BlockHeaderType = llvm::StructType::getTypeByName(
      module->getContext(), blockheader_struct);
  uint64_t headerVal = getBlockHeaderVal(module, symbol, block_type);
  return llvm::ConstantStruct::get(
      BlockHeaderType,
      llvm::ConstantInt::get(
          llvm::Type::getInt64Ty(module->getContext()), headerVal));
}

llvm::Value *allocateTerm(
    llvm::Type *alloc_type, llvm::BasicBlock *block, char const *alloc_fn) {
  return allocateTerm(
      alloc_type, llvm::ConstantExpr::getSizeOf(alloc_type), block, alloc_fn);
}

llvm::Value *allocateTerm(
    llvm::Type *alloc_type, llvm::Value *len, llvm::BasicBlock *block,
    char const *alloc_fn) {
  llvm::Instruction *Malloc = llvm::CallInst::CreateMalloc(
      block, llvm::Type::getInt64Ty(block->getContext()), alloc_type, len,
      nullptr, koreHeapAlloc(alloc_fn, block->getModule()));

  if (!block->empty()) {
    setDebugLoc(&block->back());
  }

#if LLVM_VERSION_MAJOR < 16
  Malloc->insertAfter(&block->back());
#else
  Malloc->insertInto(block, block->end());
#endif

  return Malloc;
}

value_type termType(
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

sptr<kore_sort> termSort(kore_pattern *pattern) {
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
    kore_composite_pattern *pattern, int idx,
    std::string const &location_stack) {
  kore_pattern *p = pattern->get_arguments()[idx].get();
  std::string newLocation = fmt::format("{}:{}", location_stack, idx);
  if (isInjectionSymbol(p, definition_->get_inj_symbol())) {
    newLocation = location_stack;
  }
  llvm::Value *ret = create_allocation(p, newLocation).first;
  auto *sort = dynamic_cast<kore_composite_sort *>(p->get_sort().get());
  proof_event e(definition_, module_);
  current_block_ = e.hook_arg(ret, sort, current_block_);
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
    std::string const &locationStack) {
  assert(hook_att->get_arguments().size() == 1);
  auto *strPattern
      = dynamic_cast<kore_string_pattern *>(hook_att->get_arguments()[0].get());
  std::string name = strPattern->get_contents();
  if (name == "BOOL.and" || name == "BOOL.andThen") {
    assert(pattern->get_arguments().size() == 2);
    llvm::Value *firstArg = alloc_arg(pattern, 0, locationStack);
    llvm::BasicBlock *CondBlock = current_block_;
    llvm::BasicBlock *TrueBlock
        = llvm::BasicBlock::Create(ctx_, "then", current_block_->getParent());
    llvm::BasicBlock *MergeBlock = llvm::BasicBlock::Create(
        ctx_, "hook_BOOL_and", current_block_->getParent());
    llvm::BranchInst::Create(TrueBlock, MergeBlock, firstArg, current_block_);
    current_block_ = TrueBlock;
    llvm::Value *secondArg = alloc_arg(pattern, 1, locationStack);
    llvm::BranchInst::Create(MergeBlock, current_block_);
    llvm::PHINode *Phi = llvm::PHINode::Create(
        llvm::Type::getInt1Ty(ctx_), 2, "phi", MergeBlock);
    Phi->addIncoming(secondArg, current_block_);
    Phi->addIncoming(firstArg, CondBlock);
    current_block_ = MergeBlock;
    return Phi;
  }
  if (name == "BOOL.or" || name == "BOOL.orElse") {
    assert(pattern->get_arguments().size() == 2);
    llvm::Value *firstArg = alloc_arg(pattern, 0, locationStack);
    llvm::BasicBlock *CondBlock = current_block_;
    llvm::BasicBlock *FalseBlock
        = llvm::BasicBlock::Create(ctx_, "else", current_block_->getParent());
    llvm::BasicBlock *MergeBlock = llvm::BasicBlock::Create(
        ctx_, "hook_BOOL_or", current_block_->getParent());
    llvm::BranchInst::Create(MergeBlock, FalseBlock, firstArg, current_block_);
    current_block_ = FalseBlock;
    llvm::Value *secondArg = alloc_arg(pattern, 1, locationStack);
    llvm::BranchInst::Create(MergeBlock, current_block_);
    llvm::PHINode *Phi = llvm::PHINode::Create(
        llvm::Type::getInt1Ty(ctx_), 2, "phi", MergeBlock);
    Phi->addIncoming(secondArg, current_block_);
    Phi->addIncoming(firstArg, CondBlock);
    current_block_ = MergeBlock;
    return Phi;
  }
  if (name == "BOOL.not") {
    assert(pattern->get_arguments().size() == 1);
    llvm::Value *arg = alloc_arg(pattern, 0, locationStack);
    llvm::BinaryOperator *Not = llvm::BinaryOperator::Create(
        llvm::Instruction::Xor, arg,
        llvm::ConstantInt::get(llvm::Type::getInt1Ty(ctx_), 1), "hook_BOOL_not",
        current_block_);
    return Not;
  }
  if (name == "BOOL.implies") {
    assert(pattern->get_arguments().size() == 2);
    llvm::Value *firstArg = alloc_arg(pattern, 0, locationStack);
    llvm::BasicBlock *CondBlock = current_block_;
    llvm::BasicBlock *TrueBlock
        = llvm::BasicBlock::Create(ctx_, "then", current_block_->getParent());
    llvm::BasicBlock *MergeBlock = llvm::BasicBlock::Create(
        ctx_, "hook_BOOL_implies", current_block_->getParent());
    llvm::BranchInst::Create(TrueBlock, MergeBlock, firstArg, current_block_);
    current_block_ = TrueBlock;
    llvm::Value *secondArg = alloc_arg(pattern, 1, locationStack);
    llvm::BranchInst::Create(MergeBlock, current_block_);
    llvm::PHINode *Phi = llvm::PHINode::Create(
        llvm::Type::getInt1Ty(ctx_), 2, "phi", MergeBlock);
    Phi->addIncoming(secondArg, current_block_);
    Phi->addIncoming(
        llvm::ConstantInt::get(llvm::Type::getInt1Ty(ctx_), 1), CondBlock);
    current_block_ = MergeBlock;
    return Phi;
  }
  if (name == "BOOL.ne" || name == "BOOL.xor") {
    assert(pattern->get_arguments().size() == 2);
    llvm::Value *firstArg = alloc_arg(pattern, 0, locationStack);
    llvm::Value *secondArg = alloc_arg(pattern, 1, locationStack);
    llvm::BinaryOperator *Xor = llvm::BinaryOperator::Create(
        llvm::Instruction::Xor, firstArg, secondArg, "hook_BOOL_ne",
        current_block_);
    return Xor;
  }
  if (name == "BOOL.eq") {
    assert(pattern->get_arguments().size() == 2);
    llvm::Value *firstArg = alloc_arg(pattern, 0, locationStack);
    llvm::Value *secondArg = alloc_arg(pattern, 1, locationStack);
    auto *Eq = new llvm::ICmpInst(
        *current_block_, llvm::CmpInst::ICMP_EQ, firstArg, secondArg,
        "hook_BOOL_eq");
    return Eq;
  }
  if (name == "KEQUAL.ite") {
    assert(pattern->get_arguments().size() == 3);
    llvm::Value *cond = alloc_arg(pattern, 0, locationStack);
    llvm::BasicBlock *TrueBlock
        = llvm::BasicBlock::Create(ctx_, "then", current_block_->getParent());
    llvm::BasicBlock *FalseBlock
        = llvm::BasicBlock::Create(ctx_, "else", current_block_->getParent());
    llvm::BasicBlock *MergeBlock = llvm::BasicBlock::Create(
        ctx_, "hook_KEQUAL_ite", current_block_->getParent());
    llvm::BranchInst::Create(TrueBlock, FalseBlock, cond, current_block_);
    current_block_ = TrueBlock;
    llvm::Value *trueArg = alloc_arg(pattern, 1, locationStack);
    llvm::BasicBlock *NewTrueBlock = current_block_;
    current_block_ = FalseBlock;
    llvm::Value *falseArg = alloc_arg(pattern, 2, locationStack);
    if (trueArg->getType()->isPointerTy()
        && !falseArg->getType()->isPointerTy()) {
      auto *AllocCollection
          = new llvm::AllocaInst(falseArg->getType(), 0, "", current_block_);
      new llvm::StoreInst(falseArg, AllocCollection, current_block_);
      falseArg = AllocCollection;
    } else if (
        !trueArg->getType()->isPointerTy()
        && falseArg->getType()->isPointerTy()) {
      auto *AllocCollection
          = new llvm::AllocaInst(trueArg->getType(), 0, "", NewTrueBlock);
      new llvm::StoreInst(trueArg, AllocCollection, NewTrueBlock);
      trueArg = AllocCollection;
    }
    llvm::BranchInst::Create(MergeBlock, current_block_);
    llvm::BranchInst::Create(MergeBlock, NewTrueBlock);
    llvm::PHINode *Phi
        = llvm::PHINode::Create(trueArg->getType(), 2, "phi", MergeBlock);
    Phi->addIncoming(trueArg, NewTrueBlock);
    Phi->addIncoming(falseArg, current_block_);
    current_block_ = MergeBlock;
    return Phi;
  }
  if (name == "MINT.uvalue") {
    llvm::Value *mint = alloc_arg(pattern, 0, locationStack);
    value_type cat = dynamic_cast<kore_composite_sort *>(
                         pattern->get_constructor()->get_arguments()[0].get())
                         ->get_category(definition_);
    auto *Type = getvalue_type(cat, module_);
    size_t nwords = (cat.bits + 63) / 64;
    if (nwords == 0) {
      auto *staticTerm = new create_static_term(definition_, module_);
      return staticTerm->create_token({sort_category::Int, 0}, "0");
    }
    auto *Ptr = allocateTerm(
        llvm::Type::getInt64Ty(ctx_),
        llvm::ConstantInt::get(llvm::Type::getInt64Ty(ctx_), nwords * 8),
        current_block_, "koreAllocAlwaysGC");
    if (nwords == 1) {
      llvm::Value *Word = nullptr;
      if (cat.bits == 64) {
        Word = mint;
      } else {
        Word = new llvm::ZExtInst(
            mint, llvm::Type::getInt64Ty(ctx_), "word", current_block_);
      }
      new llvm::StoreInst(Word, Ptr, current_block_);
    } else { // nwords >= 2
      llvm::Value *Ptr2 = Ptr;
      llvm::Value *accum = mint;
      for (size_t i = 0; i < nwords; i++) {
        auto *Word = new llvm::TruncInst(
            accum, llvm::Type::getInt64Ty(ctx_), "word", current_block_);
        new llvm::StoreInst(Word, Ptr2, current_block_);
        Ptr2 = llvm::GetElementPtrInst::Create(
            llvm::Type::getInt64Ty(ctx_), Ptr2,
            {llvm::ConstantInt::get(llvm::Type::getInt64Ty(ctx_), 1)}, "ptr",
            current_block_);
        accum = llvm::BinaryOperator::Create(
            llvm::Instruction::LShr, accum, llvm::ConstantInt::get(Type, 64),
            "shift", current_block_);
      }
    }
    auto *result = llvm::CallInst::Create(
        getOrInsertFunction(
            module_, "hook_MINT_import",
            getvalue_type({sort_category::Int, 0}, module_),
            llvm::Type::getInt64PtrTy(ctx_), llvm::Type::getInt64Ty(ctx_),
            llvm::Type::getInt1Ty(ctx_)),
        {Ptr, llvm::ConstantInt::get(llvm::Type::getInt64Ty(ctx_), cat.bits),
         llvm::ConstantInt::getFalse(ctx_)},
        "hook_MINT_uvalue", current_block_);
    setDebugLoc(result);
    return result;
  }
  if (name == "MINT.svalue") {
    llvm::Value *mint = alloc_arg(pattern, 0, locationStack);
    value_type cat = dynamic_cast<kore_composite_sort *>(
                         pattern->get_constructor()->get_arguments()[0].get())
                         ->get_category(definition_);
    auto *Type = getvalue_type(cat, module_);
    size_t nwords = (cat.bits + 63) / 64;
    if (nwords == 0) {
      auto *staticTerm = new create_static_term(definition_, module_);
      return staticTerm->create_token({sort_category::Int, 0}, "0");
    }
    auto *Ptr = allocateTerm(
        llvm::Type::getInt64Ty(ctx_),
        llvm::ConstantInt::get(llvm::Type::getInt64Ty(ctx_), nwords * 8),
        current_block_, "koreAllocAlwaysGC");
    if (nwords == 1) {
      llvm::Value *Word = nullptr;
      if (cat.bits == 64) {
        Word = mint;
      } else {
        Word = new llvm::SExtInst(
            mint, llvm::Type::getInt64Ty(ctx_), "word", current_block_);
      }
      new llvm::StoreInst(Word, Ptr, current_block_);
    } else { // nwords >= 2
      llvm::Value *Ptr2 = Ptr;
      llvm::Value *accum = mint;
      for (size_t i = 0; i < nwords; i++) {
        auto *Word = new llvm::TruncInst(
            accum, llvm::Type::getInt64Ty(ctx_), "word", current_block_);
        new llvm::StoreInst(Word, Ptr2, current_block_);
        Ptr2 = llvm::GetElementPtrInst::Create(
            llvm::Type::getInt64Ty(ctx_), Ptr2,
            {llvm::ConstantInt::get(llvm::Type::getInt64Ty(ctx_), 1)}, "ptr",
            current_block_);
        accum = llvm::BinaryOperator::Create(
            llvm::Instruction::AShr, accum, llvm::ConstantInt::get(Type, 64),
            "shift", current_block_);
      }
    }
    auto *result = llvm::CallInst::Create(
        getOrInsertFunction(
            module_, "hook_MINT_import",
            getvalue_type({sort_category::Int, 0}, module_),
            llvm::Type::getInt64PtrTy(ctx_), llvm::Type::getInt64Ty(ctx_),
            llvm::Type::getInt1Ty(ctx_)),
        {Ptr, llvm::ConstantInt::get(llvm::Type::getInt64Ty(ctx_), cat.bits),
         llvm::ConstantInt::getTrue(ctx_)},
        "hook_MINT_svalue", current_block_);
    setDebugLoc(result);
    return result;
  }
  if (name == "MINT.integer") {
    llvm::Value *mpz = alloc_arg(pattern, 0, locationStack);
    value_type cat = dynamic_cast<kore_composite_sort *>(
                         pattern->get_constructor()->get_sort().get())
                         ->get_category(definition_);
    auto *Type = getvalue_type(cat, module_);
    llvm::Instruction *Ptr = llvm::CallInst::Create(
        getOrInsertFunction(
            module_, "hook_MINT_export", llvm::Type::getInt64PtrTy(ctx_),
            getvalue_type({sort_category::Int, 0}, module_),
            llvm::Type::getInt64Ty(ctx_)),
        {mpz, llvm::ConstantInt::get(llvm::Type::getInt64Ty(ctx_), cat.bits)},
        "ptr", current_block_);
    setDebugLoc(Ptr);
    size_t nwords = (cat.bits + 63) / 64;
    llvm::Value *result = llvm::ConstantInt::get(Type, 0);
    if (nwords == 0) {
      return result;
    }

    if (nwords == 1) {
      auto *Word = new llvm::LoadInst(
          llvm::Type::getInt64Ty(ctx_), Ptr, "word", current_block_);
      if (cat.bits == 64) {
        return Word;
      }

      return new llvm::TruncInst(
          Word, Type, "hook_MINT_integer", current_block_);
    }
    // nwords >= 2
    for (size_t i = 0; i < nwords; i++) {
      auto *Word = new llvm::LoadInst(
          llvm::Type::getInt64Ty(ctx_), Ptr, "word", current_block_);
      auto *Zext = new llvm::ZExtInst(Word, Type, "extended", current_block_);
      auto *Shl = llvm::BinaryOperator::Create(
          llvm::Instruction::Shl, result, llvm::ConstantInt::get(Type, 64),
          "shift", current_block_);
      result = llvm::BinaryOperator::Create(
          llvm::Instruction::Or, Shl, Zext, "or", current_block_);
      Ptr = llvm::GetElementPtrInst::Create(
          llvm::Type::getInt64Ty(ctx_), Ptr,
          {llvm::ConstantInt::get(llvm::Type::getInt64Ty(ctx_), 1)}, "ptr",
          current_block_);
    }
    return result;
  }
  if (name == "MINT.neg") {
    llvm::Value *in = alloc_arg(pattern, 0, locationStack);
    return llvm::BinaryOperator::CreateNeg(in, "hook_MINT_neg", current_block_);
  }
  if (name == "MINT.not") {
    llvm::Value *in = alloc_arg(pattern, 0, locationStack);
    return llvm::BinaryOperator::CreateNot(in, "hook_MINT_not", current_block_);
#define MINT_CMP(hookname, inst)                                               \
  }                                                                            \
  if (name == "MINT." #hookname) {                                             \
    llvm::Value *first = alloc_arg(pattern, 0, locationStack);                 \
    llvm::Value *second = alloc_arg(pattern, 1, locationStack);                \
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
    llvm::Value *first = alloc_arg(pattern, 0, locationStack);                 \
    llvm::Value *second = alloc_arg(pattern, 1, locationStack);                \
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
  std::string hookName = "hook_" + name.substr(0, name.find('.')) + "_"
                         + name.substr(name.find('.') + 1);
  return create_function_call(hookName, pattern, true, false, locationStack);
}

// We use tailcc calling convention for apply_rule_* and eval_* functions to
// make these K functions tail recursive when their K definitions are tail
// recursive.
llvm::Value *create_term::create_function_call(
    std::string const &name, kore_composite_pattern *pattern, bool sret,
    bool tailcc, std::string const &location_stack) {
  auto event = proof_event(definition_, module_);

  current_block_
      = event.function_event_pre(current_block_, pattern, location_stack);

  std::vector<llvm::Value *> args;
  auto *returnSort = dynamic_cast<kore_composite_sort *>(
      pattern->get_constructor()->get_sort().get());
  auto returnCat = returnSort->get_category(definition_);
  int i = 0;
  for (auto const &sort : pattern->get_constructor()->get_arguments()) {
    auto *concreteSort = dynamic_cast<kore_composite_sort *>(sort.get());
    llvm::Value *arg = alloc_arg(pattern, i, location_stack);
    i++;
    switch (concreteSort->get_category(definition_).cat) {
    case sort_category::Map:
    case sort_category::RangeMap:
    case sort_category::List:
    case sort_category::Set: {
      if (!arg->getType()->isPointerTy()) {
        auto *AllocCollection
            = new llvm::AllocaInst(arg->getType(), 0, "", current_block_);
        new llvm::StoreInst(arg, AllocCollection, current_block_);
        args.push_back(AllocCollection);
      } else {
        args.push_back(arg);
      }
      break;
    }
    default: args.push_back(arg); break;
    }
  }

  current_block_ = event.function_event_post(current_block_);

  return create_function_call(
      name, returnCat, args, sret, tailcc, location_stack);
}

llvm::Value *create_term::create_function_call(
    std::string const &name, value_type return_cat,
    std::vector<llvm::Value *> const &args, bool sret, bool tailcc,
    std::string const &location_stack) {
  llvm::Type *returnType = getvalue_type(return_cat, module_);
  std::vector<llvm::Type *> types;
  bool collection = false;
  switch (return_cat.cat) {
  case sort_category::Map:
  case sort_category::RangeMap:
  case sort_category::List:
  case sort_category::Set: collection = true; break;
  default: sret = false; break;
  }
  llvm::Value *AllocSret = nullptr;
  types.reserve(args.size());
  for (auto *arg : args) {
    types.push_back(arg->getType());
  }
  std::vector<llvm::Value *> realArgs = args;
  llvm::Type *sretType = nullptr;
  if (sret) {
    // we don't use alloca here because the tail call optimization pass for llvm
    // doesn't handle correctly functions with alloca
    AllocSret = allocateTerm(returnType, current_block_, "koreAllocAlwaysGC");
    sretType = returnType;
    realArgs.insert(realArgs.begin(), AllocSret);
    types.insert(types.begin(), AllocSret->getType());
    returnType = llvm::Type::getVoidTy(ctx_);
  } else if (collection) {
    returnType = llvm::PointerType::getUnqual(returnType);
  }

  llvm::FunctionType *funcType
      = llvm::FunctionType::get(returnType, types, false);
  llvm::Function *func = getOrInsertFunction(module_, name, funcType);

  auto *call = llvm::CallInst::Create(func, realArgs, "", current_block_);
  setDebugLoc(call);
  if (tailcc) {
    call->setCallingConv(llvm::CallingConv::Tail);
  }
  if (sret) {
    llvm::Attribute sretAttr
        = llvm::Attribute::get(ctx_, llvm::Attribute::StructRet, sretType);
    func->arg_begin()->addAttr(sretAttr);
    call->addParamAttr(0, sretAttr);
    return AllocSret;
  }
  return call;
}

/* create a term, given the assumption that the created term will not be a
 * triangle injection pair */
llvm::Value *create_term::not_injection_case(
    kore_composite_pattern *constructor, llvm::Value *val,
    std::string const &location_stack) {
  kore_symbol const *symbol = constructor->get_constructor();
  kore_symbol_declaration *symbolDecl
      = definition_->get_symbol_declarations().at(symbol->get_name());
  llvm::StructType *BlockType = getBlockType(module_, definition_, symbol);
  llvm::Value *BlockHeader
      = getBlockHeader(module_, definition_, symbol, BlockType);
  int idx = 2;
  std::vector<llvm::Value *> children;
  for (auto const &child : constructor->get_arguments()) {
    llvm::Value *ChildValue = nullptr;
    if (idx == 2 && val != nullptr) {
      ChildValue = val;
    } else {
      std::string newLocation = fmt::format("{}:{}", location_stack, idx - 2);
      if (isInjectionSymbol(child.get(), definition_->get_inj_symbol())) {
        newLocation = location_stack;
      }
      ChildValue = create_allocation(child.get(), newLocation).first;
    }

    auto *sort = dynamic_cast<kore_composite_sort *>(child->get_sort().get());
    if (sort && isCollectionSort(sort->get_category(definition_))) {
      ChildValue = new llvm::LoadInst(
          BlockType->elements()[idx], ChildValue, "", current_block_);
    }
    children.push_back(ChildValue);
    idx++;
  }
  llvm::Value *Block = allocateTerm(BlockType, current_block_);
  llvm::Value *BlockHeaderPtr = llvm::GetElementPtrInst::CreateInBounds(
      BlockType, Block,
      {llvm::ConstantInt::get(llvm::Type::getInt64Ty(ctx_), 0),
       llvm::ConstantInt::get(llvm::Type::getInt32Ty(ctx_), 0)},
      symbol->get_name(), current_block_);
  new llvm::StoreInst(BlockHeader, BlockHeaderPtr, current_block_);
  idx = 2;
  for (auto &ChildValue : children) {
    llvm::Value *ChildPtr = llvm::GetElementPtrInst::CreateInBounds(
        BlockType, Block,
        {llvm::ConstantInt::get(llvm::Type::getInt64Ty(ctx_), 0),
         llvm::ConstantInt::get(llvm::Type::getInt32Ty(ctx_), idx++)},
        "", current_block_);
    new llvm::StoreInst(ChildValue, ChildPtr, current_block_);
  }

  auto *BlockPtr = llvm::PointerType::getUnqual(
      llvm::StructType::getTypeByName(module_->getContext(), block_struct));
  auto *bitcast = new llvm::BitCastInst(Block, BlockPtr, "", current_block_);
  if (symbolDecl->attributes().contains(attribute_set::key::Binder)) {
    auto *call = llvm::CallInst::Create(
        getOrInsertFunction(module_, "debruijnize", BlockPtr, BlockPtr),
        bitcast, "withIndices", current_block_);
    setDebugLoc(call);
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
      kore_symbol_declaration *symbolDecl
          = definition_->get_symbol_declarations().at(symbol->get_name());
      if (symbolDecl->attributes().contains(attribute_set::key::Function)
          || (symbolDecl->attributes().contains(attribute_set::key::Anywhere)
              && !is_anywhere_owise_)) {
        can_be_static = false;
      }
      for (auto const &sptr_nextPattern : constructor->get_arguments()) {
        kore_pattern *nextPattern = sptr_nextPattern.get();
        can_be_static &= populate_static_set(nextPattern);
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
    auto *staticTerm = new create_static_term(definition_, module_);
    return (*staticTerm)(pattern);
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
    kore_symbol_declaration *symbolDecl
        = definition_->get_symbol_declarations().at(symbol->get_name());
    if (symbolDecl->attributes().contains(attribute_set::key::Function)
        || (symbolDecl->attributes().contains(attribute_set::key::Anywhere)
            && !is_anywhere_owise_)) {
      if (symbolDecl->attributes().contains(attribute_set::key::Hook)) {
        auto *sort
            = dynamic_cast<kore_composite_sort *>(constructor->get_sort().get());
        auto *strPattern = dynamic_cast<kore_string_pattern *>(
            symbolDecl->attributes()
                .get(attribute_set::key::Hook)
                ->get_arguments()[0]
                .get());
        std::string name = strPattern->get_contents();

        proof_event p(definition_, module_);
        current_block_ = p.hook_event_pre(name, current_block_, location_stack);
        llvm::Value *val = create_hook(
            symbolDecl->attributes().get(attribute_set::key::Hook).get(),
            constructor, location_stack);
        current_block_ = p.hook_event_post(val, sort, current_block_);

        return std::make_pair(val, true);
      }
      auto fn_name = fmt::format("eval_{}", ast_to_string(*symbol, 0, false));
      return std::make_pair(
          create_function_call(fn_name, constructor, false, true, location_stack),
          true);
    }
    if (auto cat
        = dynamic_cast<kore_composite_sort *>(symbol->get_arguments()[0].get())
              ->get_category(definition_)
              .cat;
        symbolDecl->attributes().contains(attribute_set::key::SortInjection)
        && (cat == sort_category::Symbol)) {
      std::pair<llvm::Value *, bool> val = create_allocation(
          constructor->get_arguments()[0].get(), location_stack);
      if (val.second) {
        llvm::Instruction *Tag = llvm::CallInst::Create(
            getOrInsertFunction(
                module_, "getTag", llvm::Type::getInt32Ty(ctx_),
                getvalue_type({sort_category::Symbol, 0}, module_)),
            val.first, "tag", current_block_);
        setDebugLoc(Tag);
        auto *inj = definition_->get_inj_symbol();
        auto *NotStringBlock = llvm::BasicBlock::Create(
            ctx_, "notString", current_block_->getParent());
        auto *GeBlock = llvm::BasicBlock::Create(
            ctx_, "geFirst", current_block_->getParent());
        auto *FalseBlock = llvm::BasicBlock::Create(
            ctx_, "notInjection", current_block_->getParent());
        auto *TrueBlock = llvm::BasicBlock::Create(
            ctx_, "merge", current_block_->getParent());
        auto *cmp = new llvm::ICmpInst(
            *current_block_, llvm::CmpInst::ICMP_NE, Tag,
            llvm::ConstantInt::getSigned(llvm::Type::getInt32Ty(ctx_), -1));
        llvm::BranchInst::Create(
            NotStringBlock, FalseBlock, cmp, current_block_);

        current_block_ = NotStringBlock;
        cmp = new llvm::ICmpInst(
            *current_block_, llvm::CmpInst::ICMP_UGE, Tag,
            llvm::ConstantInt::get(
                llvm::Type::getInt32Ty(ctx_), inj->get_first_tag()));
        llvm::BranchInst::Create(GeBlock, FalseBlock, cmp, current_block_);

        current_block_ = GeBlock;
        cmp = new llvm::ICmpInst(
            *current_block_, llvm::CmpInst::ICMP_ULE, Tag,
            llvm::ConstantInt::get(
                llvm::Type::getInt32Ty(ctx_), inj->get_last_tag()));
        llvm::BranchInst::Create(TrueBlock, FalseBlock, cmp, current_block_);

        current_block_ = FalseBlock;
        auto *Cast = not_injection_case(constructor, val.first, location_stack);
        llvm::BranchInst::Create(TrueBlock, current_block_);

        current_block_ = TrueBlock;
        llvm::PHINode *Phi
            = llvm::PHINode::Create(Cast->getType(), 2, "phi", current_block_);
        Phi->addIncoming(Cast, FalseBlock);
        Phi->addIncoming(val.first, GeBlock);
        return std::make_pair(Phi, true);
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

void addAbort(llvm::BasicBlock *block, llvm::Module *module) {
  llvm::FunctionType *AbortType = llvm::FunctionType::get(
      llvm::Type::getVoidTy(module->getContext()), false);
  llvm::Function *AbortFunc = getOrInsertFunction(module, "abort", AbortType);
  AbortFunc->addFnAttr(llvm::Attribute::NoReturn);
  llvm::CallInst::Create(AbortFunc, "", block);
  new llvm::UnreachableInst(module->getContext(), block);
}

bool makeFunction(
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
  std::vector<llvm::Type *> paramTypes;
  std::vector<std::string> paramNames;
  std::vector<llvm::Metadata *> debugArgs;
  for (auto &entry : vars) {
    auto *sort
        = dynamic_cast<kore_composite_sort *>(entry.second->get_sort().get());
    if (!sort) {
      // TODO: sort variables
      return false;
    }
    auto cat = sort->get_category(definition);
    llvm::Type *paramType = getvalue_type(cat, module);
    debugArgs.push_back(getDebugType(cat, ast_to_string(*sort)));
    switch (cat.cat) {
    case sort_category::Map:
    case sort_category::RangeMap:
    case sort_category::List:
    case sort_category::Set:
      paramType = llvm::PointerType::getUnqual(paramType);
      break;
    default: break;
    }

    params.insert({entry.first, cat});
    paramTypes.push_back(paramType);
    paramNames.push_back(entry.first);
  }
  value_type returnCat = termType(pattern, params, definition);
  auto *returnType = getvalue_type(returnCat, module);
  switch (returnCat.cat) {
  case sort_category::Map:
  case sort_category::RangeMap:
  case sort_category::List:
  case sort_category::Set:
    returnType = llvm::PointerType::getUnqual(returnType);
    break;
  default: break;
  }
  llvm::FunctionType *funcType
      = llvm::FunctionType::get(returnType, paramTypes, false);
  llvm::Function *applyRule = getOrInsertFunction(module, name, funcType);
  initDebugAxiom(axiom->attributes());
  std::string debugName = name;
  if (axiom->attributes().contains(attribute_set::key::Label)) {
    debugName
        = axiom->attributes().get_string(attribute_set::key::Label) + postfix;
  }
  initDebugFunction(
      debugName, debugName,
      getDebugFunctionType(
          getDebugType(returnCat, ast_to_string(*termSort(pattern))),
          debugArgs),
      definition, applyRule);
  if (tailcc) {
    applyRule->setCallingConv(llvm::CallingConv::Tail);
  }
  llvm::StringMap<llvm::Value *> subst;
  llvm::BasicBlock *block
      = llvm::BasicBlock::Create(module->getContext(), "entry", applyRule);
  int i = 0;
  for (auto *val = applyRule->arg_begin(); val != applyRule->arg_end();
       ++val, ++i) {
    subst.insert({paramNames[i], val});
    if (debugArgs[i]) {
      initDebugParam(
          applyRule, i, paramNames[i], params[paramNames[i]],
          llvm::cast<llvm::DIType>(debugArgs[i])->getName().str());
    }
  }
  create_term creator = create_term(subst, definition, block, module, false);
  llvm::Value *retval = creator(pattern).first;

  auto *current_block_ = creator.get_current_block();
  if (apply && big_step) {
    current_block_ = proof_event(definition, module)
                         .rewrite_event_post(axiom, retval, current_block_);
  }

  if (big_step) {
    llvm::Type *blockType = getvalue_type({sort_category::Symbol, 0}, module);
    llvm::Function *step = getOrInsertFunction(
        module, "k_step",
        llvm::FunctionType::get(blockType, {blockType}, false));
    auto *call = llvm::CallInst::Create(step, {retval}, "", current_block_);
    setDebugLoc(call);
    call->setCallingConv(llvm::CallingConv::Tail);
    retval = call;
  }
  auto *ret
      = llvm::ReturnInst::Create(module->getContext(), retval, current_block_);
  setDebugLoc(ret);
  return true;
}

void makeApplyRuleFunction(
    kore_axiom_declaration *axiom, kore_definition *definition,
    llvm::Module *module, bool big_step) {
  kore_pattern *pattern = axiom->get_right_hand_side();
  std::string name = "apply_rule_" + std::to_string(axiom->get_ordinal());
  makeFunction(
      name, pattern, definition, module, true, big_step, true, axiom, ".rhs");
  if (big_step) {
    makeFunction(
        name + "_search", pattern, definition, module, true, false, true, axiom,
        ".rhs");
  }
}

std::string makeApplyRuleFunction(
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
  std::vector<llvm::Type *> paramTypes;
  std::vector<std::string> paramNames;
  std::vector<llvm::Metadata *> debugArgs;
  for (auto &entry : vars) {
    auto *sort
        = dynamic_cast<kore_composite_sort *>(entry.second->get_sort().get());
    if (!sort) {
      // TODO: sort variables
      return "";
    }
    auto cat = sort->get_category(definition);
    llvm::Type *paramType = getvalue_type(cat, module);
    debugArgs.push_back(getDebugType(cat, ast_to_string(*sort)));
    switch (cat.cat) {
    case sort_category::Map:
    case sort_category::RangeMap:
    case sort_category::List:
    case sort_category::Set:
      paramType = llvm::PointerType::getUnqual(paramType);
      break;
    default: break;
    }

    params.insert({entry.first, cat});
    paramTypes.push_back(paramType);
    paramNames.push_back(entry.first);
  }
  llvm::FunctionType *funcType = llvm::FunctionType::get(
      getvalue_type({sort_category::Symbol, 0}, module), paramTypes, false);
  std::string name = "apply_rule_" + std::to_string(axiom->get_ordinal());

  makeFunction(
      name + "_search", axiom->get_right_hand_side(), definition, module, true,
      false, true, axiom, ".rhs");

  llvm::Function *applyRule = getOrInsertFunction(module, name, funcType);
  initDebugAxiom(axiom->attributes());
  initDebugFunction(
      name, name,
      getDebugFunctionType(
          getDebugType({sort_category::Symbol, 0}, "SortGeneratedTopCell{}"),
          debugArgs),
      definition, applyRule);
  applyRule->setCallingConv(llvm::CallingConv::Tail);
  llvm::StringMap<llvm::Value *> subst;
  llvm::BasicBlock *block
      = llvm::BasicBlock::Create(module->getContext(), "entry", applyRule);
  int i = 0;
  for (auto *val = applyRule->arg_begin(); val != applyRule->arg_end();
       ++val, ++i) {
    subst.insert({paramNames[i], val});
    if (debugArgs[i]) {
      initDebugParam(
          applyRule, i, paramNames[i], params[paramNames[i]],
          llvm::cast<llvm::DIType>(debugArgs[i])->getName().str());
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
        auto *ptr = allocateTerm(
            arg->getType(), creator.get_current_block(), "koreAllocAlwaysGC");
        new llvm::StoreInst(arg, ptr, creator.get_current_block());
        arg = ptr;
      }
      break;
    default: break;
    }
    args.push_back(arg);
    types.push_back(arg->getType());
  }
  llvm::Type *blockType = getvalue_type({sort_category::Symbol, 0}, module);
  llvm::Function *step = getOrInsertFunction(
      module, "step_" + std::to_string(axiom->get_ordinal()),
      llvm::FunctionType::get(blockType, types, false));
  auto *retval
      = llvm::CallInst::Create(step, args, "", creator.get_current_block());
  setDebugLoc(retval);
  retval->setCallingConv(llvm::CallingConv::Tail);
  llvm::ReturnInst::Create(
      module->getContext(), retval, creator.get_current_block());
  return name;
}

std::string makeSideConditionFunction(
    kore_axiom_declaration *axiom, kore_definition *definition,
    llvm::Module *module) {
  kore_pattern *pattern = axiom->get_requires();

  if (!pattern) {
    return "";
  }
  std::string name = "side_condition_" + std::to_string(axiom->get_ordinal());
  if (makeFunction(
          name, pattern, definition, module, false, false, false, axiom,
          ".sc")) {
    return name;
  }
  return "";
}

llvm::Type *getArgType(value_type cat, llvm::Module *mod) {
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
    return getBlockType(mod);
  }

  case sort_category::Uncomputed:
  default: {
    abort();
  }
  }
}

bool isCollectionSort(value_type cat) {
  switch (cat.cat) {
  case sort_category::Map:
  case sort_category::RangeMap:
  case sort_category::List:
  case sort_category::Set: return true;
  default: return false;
  }
}

bool isInjectionSymbol(kore_pattern *p, kore_symbol *sym) {
  if (auto *constructor = dynamic_cast<kore_composite_pattern *>(p)) {
    kore_symbol const *symbol = constructor->get_constructor();
    if (symbol->get_name() == sym->get_name()) {
      return true;
    }
  }
  return false;
}

} // namespace kllvm
