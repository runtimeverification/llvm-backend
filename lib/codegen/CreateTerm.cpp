#include "kllvm/codegen/CreateTerm.h"
#include "kllvm/codegen/CreateStaticTerm.h"
#include "kllvm/codegen/Debug.h"
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

std::string LLVM_HEADER = R"LLVM(
; Target determined during CMake build
target datalayout = "@BACKEND_TARGET_DATALAYOUT@"
target triple = "@BACKEND_TARGET_TRIPLE@"

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

void writeUInt64(
    llvm::Value *outputFile, llvm::Module *Module, uint64_t value,
    llvm::BasicBlock *Block);

std::unique_ptr<llvm::Module>
newModule(std::string name, llvm::LLVMContext &Context) {
  llvm::SMDiagnostic Err;
  auto mod = llvm::parseIR(
      *llvm::MemoryBuffer::getMemBuffer(LLVM_HEADER), Err, Context);
  if (!mod) {
    Err.print("header.ll", llvm::errs());
  }
  return mod;
}

static std::string KOMPILED_DIR = "kompiled_directory";

void addKompiledDirSymbol(
    llvm::LLVMContext &Context, std::string dir, llvm::Module *mod,
    bool debug) {
  auto Str = llvm::ConstantDataArray::getString(Context, dir, true);
  auto global = mod->getOrInsertGlobal(KOMPILED_DIR, Str->getType());
  llvm::GlobalVariable *globalVar
      = llvm::dyn_cast<llvm::GlobalVariable>(global);
  if (!globalVar->hasInitializer()) {
    globalVar->setInitializer(Str);
  }

  if (debug) {
    initDebugGlobal(KOMPILED_DIR, getCharDebugType(), globalVar);
  }
}

std::string MAP_STRUCT = "map";
std::string RANGEMAP_STRUCT = "rangemap";
std::string LIST_STRUCT = "list";
std::string SET_STRUCT = "set";
std::string INT_WRAPPER_STRUCT = "mpz_hdr";
std::string INT_STRUCT = "mpz";
std::string FLOAT_WRAPPER_STRUCT = "floating_hdr";
std::string FLOAT_STRUCT = "floating";
std::string BUFFER_STRUCT = "stringbuffer";
std::string BLOCK_STRUCT = "block";
std::string BLOCKHEADER_STRUCT = "blockheader";

llvm::Type *getParamType(ValueType sort, llvm::Module *Module) {
  llvm::Type *type = getValueType(sort, Module);
  switch (sort.cat) {
  case SortCategory::Map:
  case SortCategory::RangeMap:
  case SortCategory::List:
  case SortCategory::Set: type = llvm::PointerType::getUnqual(type); break;
  default: break;
  }
  return type;
}

llvm::StructType *getBlockType(llvm::Module *Module) {
  return getTypeByName(Module, BLOCK_STRUCT);
}

llvm::Type *getValueType(ValueType sort, llvm::Module *Module) {
  switch (sort.cat) {
  case SortCategory::Map: return getTypeByName(Module, MAP_STRUCT);
  case SortCategory::RangeMap: return getTypeByName(Module, RANGEMAP_STRUCT);
  case SortCategory::List: return getTypeByName(Module, LIST_STRUCT);
  case SortCategory::Set: return getTypeByName(Module, SET_STRUCT);
  case SortCategory::Int:
    return llvm::PointerType::getUnqual(getTypeByName(Module, INT_STRUCT));
  case SortCategory::Float:
    return llvm::PointerType::getUnqual(getTypeByName(Module, FLOAT_STRUCT));
  case SortCategory::StringBuffer:
    return llvm::PointerType::getUnqual(getTypeByName(Module, BUFFER_STRUCT));
  case SortCategory::Bool: return llvm::Type::getInt1Ty(Module->getContext());
  case SortCategory::MInt:
    return llvm::IntegerType::get(Module->getContext(), sort.bits);
  case SortCategory::Symbol:
  case SortCategory::Variable:
    return llvm::PointerType::getUnqual(getTypeByName(Module, BLOCK_STRUCT));
  case SortCategory::Uncomputed: abort();
  }
}

llvm::StructType *getBlockType(
    llvm::Module *Module, KOREDefinition *definition,
    const KORESymbol *symbol) {
  llvm::StructType *BlockHeaderType = getTypeByName(Module, BLOCKHEADER_STRUCT);
  llvm::ArrayType *EmptyArrayType
      = llvm::ArrayType::get(llvm::Type::getInt64Ty(Module->getContext()), 0);
  llvm::SmallVector<llvm::Type *, 4> Types;
  Types.push_back(BlockHeaderType);
  Types.push_back(EmptyArrayType);
  for (auto &arg : symbol->getArguments()) {
    auto sort = dynamic_cast<KORECompositeSort *>(arg.get());
    llvm::Type *type = getValueType(sort->getCategory(definition), Module);
    Types.push_back(type);
  }
  return llvm::StructType::get(Module->getContext(), Types);
}

uint64_t getBlockHeaderVal(
    llvm::Module *Module, const KORESymbol *symbol, llvm::Type *BlockType) {
  uint64_t headerVal = symbol->getTag();
  uint64_t sizeInBytes = llvm::DataLayout(Module).getTypeAllocSize(BlockType);
  assert(sizeInBytes % 8 == 0);
  headerVal |= (sizeInBytes / 8) << 32;
  headerVal |= (uint64_t)symbol->getLayout() << LAYOUT_OFFSET;
  return headerVal;
}

llvm::Value *getBlockHeader(
    llvm::Module *Module, KOREDefinition *definition, const KORESymbol *symbol,
    llvm::Type *BlockType) {
  llvm::StructType *BlockHeaderType = getTypeByName(Module, BLOCKHEADER_STRUCT);
  uint64_t headerVal = getBlockHeaderVal(Module, symbol, BlockType);
  return llvm::ConstantStruct::get(
      BlockHeaderType,
      llvm::ConstantInt::get(
          llvm::Type::getInt64Ty(Module->getContext()), headerVal));
}

llvm::Value *allocateTerm(
    llvm::Type *AllocType, llvm::BasicBlock *block, const char *allocFn) {
  return allocateTerm(
      AllocType, llvm::ConstantExpr::getSizeOf(AllocType), block, allocFn);
}

llvm::Value *allocateTerm(
    llvm::Type *AllocType, llvm::Value *Len, llvm::BasicBlock *block,
    const char *allocFn) {
  llvm::Instruction *Malloc = llvm::CallInst::CreateMalloc(
      block, llvm::Type::getInt64Ty(block->getContext()), AllocType, Len,
      nullptr, koreHeapAlloc(allocFn, block->getModule()));

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

ValueType termType(
    KOREPattern *pattern, llvm::StringMap<ValueType> &substitution,
    KOREDefinition *definition) {
  if (auto variable = dynamic_cast<KOREVariablePattern *>(pattern)) {
    return substitution.lookup(variable->getName());
  } else if (auto constructor = dynamic_cast<KORECompositePattern *>(pattern)) {
    KORESymbol *symbol = constructor->getConstructor();
    assert(symbol->isConcrete() && "not supported yet: sort variables");
    if (symbol->getName() == "\\dv") {
      auto sort = dynamic_cast<KORECompositeSort *>(
          symbol->getFormalArguments()[0].get());
      return sort->getCategory(definition);
    }
    auto sort = dynamic_cast<KORECompositeSort *>(symbol->getSort().get());
    return sort->getCategory(definition);
  } else {
    assert(false && "not supported yet: meta level");
    abort();
  }
}

sptr<KORESort> termSort(KOREPattern *pattern) {
  if (auto variable = dynamic_cast<KOREVariablePattern *>(pattern)) {
    return variable->getSort();
  } else if (auto constructor = dynamic_cast<KORECompositePattern *>(pattern)) {
    KORESymbol *symbol = constructor->getConstructor();
    assert(symbol->isConcrete() && "not supported yet: sort variables");
    if (symbol->getName() == "\\dv") {
      return symbol->getFormalArguments()[0];
    }
    return symbol->getSort();
  } else {
    assert(false && "not supported yet: meta level");
    abort();
  }
}

#define ALLOC_ARG(x)                                                           \
  createAllocation(                                                            \
      pattern->getArguments()[x].get(),                                        \
      fmt::format("{}:{}", locationStack, x))                                  \
      .first

std::string escape(std::string str) {
  std::stringstream os;
  os << std::setfill('0') << std::setw(2) << std::hex;
  for (char c : str) {
    unsigned char uc = c;
    os << (int)uc;
  }
  return os.str();
}

llvm::Value *CreateTerm::createHook(
    KORECompositePattern *hookAtt, KORECompositePattern *pattern,
    std::string locationStack) {
  assert(hookAtt->getArguments().size() == 1);
  auto strPattern
      = dynamic_cast<KOREStringPattern *>(hookAtt->getArguments()[0].get());
  std::string name = strPattern->getContents();
  if (name == "BOOL.and" || name == "BOOL.andThen") {
    assert(pattern->getArguments().size() == 2);
    llvm::Value *firstArg = ALLOC_ARG(0);
    llvm::BasicBlock *CondBlock = CurrentBlock;
    llvm::BasicBlock *TrueBlock
        = llvm::BasicBlock::Create(Ctx, "then", CurrentBlock->getParent());
    llvm::BasicBlock *MergeBlock = llvm::BasicBlock::Create(
        Ctx, "hook_BOOL_and", CurrentBlock->getParent());
    llvm::BranchInst::Create(TrueBlock, MergeBlock, firstArg, CurrentBlock);
    CurrentBlock = TrueBlock;
    llvm::Value *secondArg = ALLOC_ARG(1);
    llvm::BranchInst::Create(MergeBlock, CurrentBlock);
    llvm::PHINode *Phi = llvm::PHINode::Create(
        llvm::Type::getInt1Ty(Ctx), 2, "phi", MergeBlock);
    Phi->addIncoming(secondArg, CurrentBlock);
    Phi->addIncoming(firstArg, CondBlock);
    CurrentBlock = MergeBlock;
    return Phi;
  } else if (name == "BOOL.or" || name == "BOOL.orElse") {
    assert(pattern->getArguments().size() == 2);
    llvm::Value *firstArg = ALLOC_ARG(0);
    llvm::BasicBlock *CondBlock = CurrentBlock;
    llvm::BasicBlock *FalseBlock
        = llvm::BasicBlock::Create(Ctx, "else", CurrentBlock->getParent());
    llvm::BasicBlock *MergeBlock = llvm::BasicBlock::Create(
        Ctx, "hook_BOOL_or", CurrentBlock->getParent());
    llvm::BranchInst::Create(MergeBlock, FalseBlock, firstArg, CurrentBlock);
    CurrentBlock = FalseBlock;
    llvm::Value *secondArg = ALLOC_ARG(1);
    llvm::BranchInst::Create(MergeBlock, CurrentBlock);
    llvm::PHINode *Phi = llvm::PHINode::Create(
        llvm::Type::getInt1Ty(Ctx), 2, "phi", MergeBlock);
    Phi->addIncoming(secondArg, CurrentBlock);
    Phi->addIncoming(firstArg, CondBlock);
    CurrentBlock = MergeBlock;
    return Phi;
  } else if (name == "BOOL.not") {
    assert(pattern->getArguments().size() == 1);
    llvm::Value *arg = ALLOC_ARG(0);
    llvm::BinaryOperator *Not = llvm::BinaryOperator::Create(
        llvm::Instruction::Xor, arg,
        llvm::ConstantInt::get(llvm::Type::getInt1Ty(Ctx), 1), "hook_BOOL_not",
        CurrentBlock);
    return Not;
  } else if (name == "BOOL.implies") {
    assert(pattern->getArguments().size() == 2);
    llvm::Value *firstArg = ALLOC_ARG(0);
    llvm::BasicBlock *CondBlock = CurrentBlock;
    llvm::BasicBlock *TrueBlock
        = llvm::BasicBlock::Create(Ctx, "then", CurrentBlock->getParent());
    llvm::BasicBlock *MergeBlock = llvm::BasicBlock::Create(
        Ctx, "hook_BOOL_implies", CurrentBlock->getParent());
    llvm::BranchInst::Create(TrueBlock, MergeBlock, firstArg, CurrentBlock);
    CurrentBlock = TrueBlock;
    llvm::Value *secondArg = ALLOC_ARG(1);
    llvm::BranchInst::Create(MergeBlock, CurrentBlock);
    llvm::PHINode *Phi = llvm::PHINode::Create(
        llvm::Type::getInt1Ty(Ctx), 2, "phi", MergeBlock);
    Phi->addIncoming(secondArg, CurrentBlock);
    Phi->addIncoming(
        llvm::ConstantInt::get(llvm::Type::getInt1Ty(Ctx), 1), CondBlock);
    CurrentBlock = MergeBlock;
    return Phi;
  } else if (name == "BOOL.ne" || name == "BOOL.xor") {
    assert(pattern->getArguments().size() == 2);
    llvm::Value *firstArg = ALLOC_ARG(0);
    llvm::Value *secondArg = ALLOC_ARG(1);
    llvm::BinaryOperator *Xor = llvm::BinaryOperator::Create(
        llvm::Instruction::Xor, firstArg, secondArg, "hook_BOOL_ne",
        CurrentBlock);
    return Xor;
  } else if (name == "BOOL.eq") {
    assert(pattern->getArguments().size() == 2);
    llvm::Value *firstArg = ALLOC_ARG(0);
    llvm::Value *secondArg = ALLOC_ARG(1);
    llvm::ICmpInst *Eq = new llvm::ICmpInst(
        *CurrentBlock, llvm::CmpInst::ICMP_EQ, firstArg, secondArg,
        "hook_BOOL_eq");
    return Eq;
  } else if (name == "KEQUAL.ite") {
    assert(pattern->getArguments().size() == 3);
    llvm::Value *cond = ALLOC_ARG(0);
    llvm::BasicBlock *TrueBlock
        = llvm::BasicBlock::Create(Ctx, "then", CurrentBlock->getParent());
    llvm::BasicBlock *FalseBlock
        = llvm::BasicBlock::Create(Ctx, "else", CurrentBlock->getParent());
    llvm::BasicBlock *MergeBlock = llvm::BasicBlock::Create(
        Ctx, "hook_KEQUAL_ite", CurrentBlock->getParent());
    llvm::BranchInst::Create(TrueBlock, FalseBlock, cond, CurrentBlock);
    CurrentBlock = TrueBlock;
    llvm::Value *trueArg = ALLOC_ARG(1);
    llvm::BasicBlock *NewTrueBlock = CurrentBlock;
    CurrentBlock = FalseBlock;
    llvm::Value *falseArg = ALLOC_ARG(2);
    if (trueArg->getType()->isPointerTy()
        && !falseArg->getType()->isPointerTy()) {
      llvm::AllocaInst *AllocCollection
          = new llvm::AllocaInst(falseArg->getType(), 0, "", CurrentBlock);
      new llvm::StoreInst(falseArg, AllocCollection, CurrentBlock);
      falseArg = AllocCollection;
    } else if (
        !trueArg->getType()->isPointerTy()
        && falseArg->getType()->isPointerTy()) {
      llvm::AllocaInst *AllocCollection
          = new llvm::AllocaInst(trueArg->getType(), 0, "", NewTrueBlock);
      new llvm::StoreInst(trueArg, AllocCollection, NewTrueBlock);
      trueArg = AllocCollection;
    }
    llvm::BranchInst::Create(MergeBlock, CurrentBlock);
    llvm::BranchInst::Create(MergeBlock, NewTrueBlock);
    llvm::PHINode *Phi
        = llvm::PHINode::Create(trueArg->getType(), 2, "phi", MergeBlock);
    Phi->addIncoming(trueArg, NewTrueBlock);
    Phi->addIncoming(falseArg, CurrentBlock);
    CurrentBlock = MergeBlock;
    return Phi;
  } else if (name == "MINT.uvalue") {
    llvm::Value *mint = ALLOC_ARG(0);
    ValueType cat = dynamic_cast<KORECompositeSort *>(
                        pattern->getConstructor()->getArguments()[0].get())
                        ->getCategory(Definition);
    auto Type = getValueType(cat, Module);
    size_t nwords = (cat.bits + 63) / 64;
    if (nwords == 0) {
      auto staticTerm = new CreateStaticTerm(Definition, Module);
      return staticTerm->createToken({SortCategory::Int, 0}, "0");
    }
    auto Ptr = allocateTerm(
        llvm::Type::getInt64Ty(Ctx),
        llvm::ConstantInt::get(llvm::Type::getInt64Ty(Ctx), nwords * 8),
        CurrentBlock, "koreAllocAlwaysGC");
    if (nwords == 1) {
      llvm::Value *Word;
      if (cat.bits == 64) {
        Word = mint;
      } else {
        Word = new llvm::ZExtInst(
            mint, llvm::Type::getInt64Ty(Ctx), "word", CurrentBlock);
      }
      new llvm::StoreInst(Word, Ptr, CurrentBlock);
    } else { // nwords >= 2
      llvm::Value *Ptr2 = Ptr;
      llvm::Value *accum = mint;
      for (size_t i = 0; i < nwords; i++) {
        auto Word = new llvm::TruncInst(
            accum, llvm::Type::getInt64Ty(Ctx), "word", CurrentBlock);
        new llvm::StoreInst(Word, Ptr2, CurrentBlock);
        Ptr2 = llvm::GetElementPtrInst::Create(
            llvm::Type::getInt64Ty(Ctx), Ptr2,
            {llvm::ConstantInt::get(llvm::Type::getInt64Ty(Ctx), 1)}, "ptr",
            CurrentBlock);
        accum = llvm::BinaryOperator::Create(
            llvm::Instruction::LShr, accum, llvm::ConstantInt::get(Type, 64),
            "shift", CurrentBlock);
      }
    }
    auto result = llvm::CallInst::Create(
        getOrInsertFunction(
            Module, "hook_MINT_import",
            getValueType({SortCategory::Int, 0}, Module),
            llvm::Type::getInt64PtrTy(Ctx), llvm::Type::getInt64Ty(Ctx),
            llvm::Type::getInt1Ty(Ctx)),
        {Ptr, llvm::ConstantInt::get(llvm::Type::getInt64Ty(Ctx), cat.bits),
         llvm::ConstantInt::getFalse(Ctx)},
        "hook_MINT_uvalue", CurrentBlock);
    setDebugLoc(result);
    return result;
  } else if (name == "MINT.svalue") {
    llvm::Value *mint = ALLOC_ARG(0);
    ValueType cat = dynamic_cast<KORECompositeSort *>(
                        pattern->getConstructor()->getArguments()[0].get())
                        ->getCategory(Definition);
    auto Type = getValueType(cat, Module);
    size_t nwords = (cat.bits + 63) / 64;
    if (nwords == 0) {
      auto staticTerm = new CreateStaticTerm(Definition, Module);
      return staticTerm->createToken({SortCategory::Int, 0}, "0");
    }
    auto Ptr = allocateTerm(
        llvm::Type::getInt64Ty(Ctx),
        llvm::ConstantInt::get(llvm::Type::getInt64Ty(Ctx), nwords * 8),
        CurrentBlock, "koreAllocAlwaysGC");
    if (nwords == 1) {
      llvm::Value *Word;
      if (cat.bits == 64) {
        Word = mint;
      } else {
        Word = new llvm::SExtInst(
            mint, llvm::Type::getInt64Ty(Ctx), "word", CurrentBlock);
      }
      new llvm::StoreInst(Word, Ptr, CurrentBlock);
    } else { // nwords >= 2
      llvm::Value *Ptr2 = Ptr;
      llvm::Value *accum = mint;
      for (size_t i = 0; i < nwords; i++) {
        auto Word = new llvm::TruncInst(
            accum, llvm::Type::getInt64Ty(Ctx), "word", CurrentBlock);
        new llvm::StoreInst(Word, Ptr2, CurrentBlock);
        Ptr2 = llvm::GetElementPtrInst::Create(
            llvm::Type::getInt64Ty(Ctx), Ptr2,
            {llvm::ConstantInt::get(llvm::Type::getInt64Ty(Ctx), 1)}, "ptr",
            CurrentBlock);
        accum = llvm::BinaryOperator::Create(
            llvm::Instruction::AShr, accum, llvm::ConstantInt::get(Type, 64),
            "shift", CurrentBlock);
      }
    }
    auto result = llvm::CallInst::Create(
        getOrInsertFunction(
            Module, "hook_MINT_import",
            getValueType({SortCategory::Int, 0}, Module),
            llvm::Type::getInt64PtrTy(Ctx), llvm::Type::getInt64Ty(Ctx),
            llvm::Type::getInt1Ty(Ctx)),
        {Ptr, llvm::ConstantInt::get(llvm::Type::getInt64Ty(Ctx), cat.bits),
         llvm::ConstantInt::getTrue(Ctx)},
        "hook_MINT_svalue", CurrentBlock);
    setDebugLoc(result);
    return result;
  } else if (name == "MINT.integer") {
    llvm::Value *mpz = ALLOC_ARG(0);
    ValueType cat = dynamic_cast<KORECompositeSort *>(
                        pattern->getConstructor()->getSort().get())
                        ->getCategory(Definition);
    auto Type = getValueType(cat, Module);
    llvm::Instruction *Ptr = llvm::CallInst::Create(
        getOrInsertFunction(
            Module, "hook_MINT_export", llvm::Type::getInt64PtrTy(Ctx),
            getValueType({SortCategory::Int, 0}, Module),
            llvm::Type::getInt64Ty(Ctx)),
        {mpz, llvm::ConstantInt::get(llvm::Type::getInt64Ty(Ctx), cat.bits)},
        "ptr", CurrentBlock);
    setDebugLoc(Ptr);
    size_t nwords = (cat.bits + 63) / 64;
    llvm::Value *result = llvm::ConstantInt::get(Type, 0);
    if (nwords == 0) {
      return result;
    } else if (nwords == 1) {
      auto Word = new llvm::LoadInst(
          llvm::Type::getInt64Ty(Ctx), Ptr, "word", CurrentBlock);
      if (cat.bits == 64) {
        return Word;
      } else {
        return new llvm::TruncInst(
            Word, Type, "hook_MINT_integer", CurrentBlock);
      }
    } else { // nwords >= 2
      for (size_t i = 0; i < nwords; i++) {
        auto Word = new llvm::LoadInst(
            llvm::Type::getInt64Ty(Ctx), Ptr, "word", CurrentBlock);
        auto Zext = new llvm::ZExtInst(Word, Type, "extended", CurrentBlock);
        auto Shl = llvm::BinaryOperator::Create(
            llvm::Instruction::Shl, result, llvm::ConstantInt::get(Type, 64),
            "shift", CurrentBlock);
        result = llvm::BinaryOperator::Create(
            llvm::Instruction::Or, Shl, Zext, "or", CurrentBlock);
        Ptr = llvm::GetElementPtrInst::Create(
            llvm::Type::getInt64Ty(Ctx), Ptr,
            {llvm::ConstantInt::get(llvm::Type::getInt64Ty(Ctx), 1)}, "ptr",
            CurrentBlock);
      }
      return result;
    }
  } else if (name == "MINT.neg") {
    llvm::Value *in = ALLOC_ARG(0);
    return llvm::BinaryOperator::CreateNeg(in, "hook_MINT_neg", CurrentBlock);
  } else if (name == "MINT.not") {
    llvm::Value *in = ALLOC_ARG(0);
    return llvm::BinaryOperator::CreateNot(in, "hook_MINT_not", CurrentBlock);
#define MINT_CMP(hookname, inst)                                               \
  }                                                                            \
  else if (name == "MINT." #hookname) {                                        \
    llvm::Value *first = ALLOC_ARG(0);                                         \
    llvm::Value *second = ALLOC_ARG(1);                                        \
    return new llvm::ICmpInst(                                                 \
        *CurrentBlock, llvm::CmpInst::inst, first, second,                     \
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
  else if (name == "MINT." #hookname) {                                        \
    llvm::Value *first = ALLOC_ARG(0);                                         \
    llvm::Value *second = ALLOC_ARG(1);                                        \
    return llvm::BinaryOperator::Create(                                       \
        llvm::Instruction::inst, first, second, "hook_MINT_" #hookname,        \
        CurrentBlock)
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
  } else if (!name.compare(0, 5, "MINT.")) {
    std::cerr << name << std::endl;
    assert(false && "not implemented yet: MInt");
    abort();
  } else {
    std::string domain = name.substr(0, name.find('.'));
    if (domain == "ARRAY") {
      // array is not really hooked in llvm, it's implemented in K
      std::ostringstream Out;
      pattern->getConstructor()->print(Out, 0, false);
      return createFunctionCall(
          "eval_" + Out.str(), pattern, false, true, locationStack);
    }
    std::string hookName
        = "hook_" + domain + "_" + name.substr(name.find('.') + 1);
    return createFunctionCall(hookName, pattern, true, false, locationStack);
  }
}

// We use tailcc calling convention for apply_rule_* and eval_* functions to
// make these K functions tail recursive when their K definitions are tail
// recursive.
llvm::Value *CreateTerm::createFunctionCall(
    std::string name, KORECompositePattern *pattern, bool sret, bool tailcc,
    std::string locationStack) {
  std::vector<llvm::Value *> args;
  auto returnSort = dynamic_cast<KORECompositeSort *>(
      pattern->getConstructor()->getSort().get());
  auto returnCat = returnSort->getCategory(Definition);
  int i = 0;
  for (auto sort : pattern->getConstructor()->getArguments()) {
    auto concreteSort = dynamic_cast<KORECompositeSort *>(sort.get());
    llvm::Value *arg = ALLOC_ARG(i);
    i++;
    switch (concreteSort->getCategory(Definition).cat) {
    case SortCategory::Map:
    case SortCategory::RangeMap:
    case SortCategory::List:
    case SortCategory::Set: {
      if (!arg->getType()->isPointerTy()) {
        llvm::AllocaInst *AllocCollection
            = new llvm::AllocaInst(arg->getType(), 0, "", CurrentBlock);
        new llvm::StoreInst(arg, AllocCollection, CurrentBlock);
        args.push_back(AllocCollection);
      } else {
        args.push_back(arg);
      }
      break;
    }
    default: args.push_back(arg); break;
    }
  }

  llvm::Function *func = CurrentBlock->getParent();

  auto ProofOutputFlag = Module->getOrInsertGlobal(
      "proof_output", llvm::Type::getInt1Ty(Module->getContext()));
  auto OutputFileName = Module->getOrInsertGlobal(
      "output_file", llvm::Type::getInt8PtrTy(Module->getContext()));
  auto proofOutput = new llvm::LoadInst(
      llvm::Type::getInt1Ty(Module->getContext()), ProofOutputFlag,
      "proof_output", CurrentBlock);
  llvm::BasicBlock *TrueBlock
      = llvm::BasicBlock::Create(Module->getContext(), "if", func);
  auto outputFile = new llvm::LoadInst(
      llvm::Type::getInt8PtrTy(Module->getContext()), OutputFileName, "output",
      TrueBlock);
  auto ir = new llvm::IRBuilder(TrueBlock);
  llvm::BasicBlock *MergeBlock
      = llvm::BasicBlock::Create(Module->getContext(), "tail", func);
  llvm::BranchInst::Create(TrueBlock, MergeBlock, proofOutput, CurrentBlock);

  std::ostringstream symbolName;
  pattern->getConstructor()->print(symbolName);

  auto symbolString
      = ir->CreateGlobalStringPtr(symbolName.str(), "", 0, Module);
  auto positionString = ir->CreateGlobalStringPtr(locationStack, "", 0, Module);
  writeUInt64(outputFile, Module, 0xdddddddddddddddd, TrueBlock);
  ir->CreateCall(
      getOrInsertFunction(
          Module, "printVariableToFile",
          llvm::Type::getVoidTy(Module->getContext()),
          llvm::Type::getInt8PtrTy(Module->getContext()),
          llvm::Type::getInt8PtrTy(Module->getContext())),
      {outputFile, symbolString});
  ir->CreateCall(
      getOrInsertFunction(
          Module, "printVariableToFile",
          llvm::Type::getVoidTy(Module->getContext()),
          llvm::Type::getInt8PtrTy(Module->getContext()),
          llvm::Type::getInt8PtrTy(Module->getContext())),
      {outputFile, positionString});

  llvm::BranchInst::Create(MergeBlock, TrueBlock);
  CurrentBlock = MergeBlock;

  return createFunctionCall(name, returnCat, args, sret, tailcc, locationStack);
}

llvm::Value *CreateTerm::createFunctionCall(
    std::string name, ValueType returnCat,
    const std::vector<llvm::Value *> &args, bool sret, bool tailcc,
    std::string locationStack) {
  llvm::Type *returnType = getValueType(returnCat, Module);
  std::vector<llvm::Type *> types;
  bool collection = false;
  switch (returnCat.cat) {
  case SortCategory::Map:
  case SortCategory::RangeMap:
  case SortCategory::List:
  case SortCategory::Set: collection = true; break;
  default: sret = false; break;
  }
  llvm::Value *AllocSret;
  for (int i = 0; i < args.size(); i++) {
    llvm::Value *arg = args[i];
    types.push_back(arg->getType());
  }
  std::vector<llvm::Value *> realArgs = args;
  llvm::Type *sretType;
  if (sret) {
    // we don't use alloca here because the tail call optimization pass for llvm
    // doesn't handle correctly functions with alloca
    AllocSret = allocateTerm(returnType, CurrentBlock, "koreAllocAlwaysGC");
    sretType = returnType;
    realArgs.insert(realArgs.begin(), AllocSret);
    types.insert(types.begin(), AllocSret->getType());
    returnType = llvm::Type::getVoidTy(Ctx);
  } else if (collection) {
    returnType = llvm::PointerType::getUnqual(returnType);
  }

  llvm::FunctionType *funcType
      = llvm::FunctionType::get(returnType, types, false);
  llvm::Function *func = getOrInsertFunction(Module, name, funcType);

  auto call = llvm::CallInst::Create(func, realArgs, "", CurrentBlock);
  setDebugLoc(call);
  if (tailcc) {
    call->setCallingConv(llvm::CallingConv::Tail);
  }
  if (sret) {
#if LLVM_VERSION_MAJOR >= 12
    llvm::Attribute sretAttr
        = llvm::Attribute::get(Ctx, llvm::Attribute::StructRet, sretType);
#else
    (void)sretType;
    llvm::Attribute sretAttr
        = llvm::Attribute::get(Ctx, llvm::Attribute::StructRet);
#endif
    func->arg_begin()->addAttr(sretAttr);
    call->addParamAttr(0, sretAttr);
    return AllocSret;
  }
  return call;
}

/* create a term, given the assumption that the created term will not be a
 * triangle injection pair */
llvm::Value *CreateTerm::notInjectionCase(
    KORECompositePattern *constructor, llvm::Value *val,
    std::string locationStack) {
  const KORESymbol *symbol = constructor->getConstructor();
  KORESymbolDeclaration *symbolDecl
      = Definition->getSymbolDeclarations().at(symbol->getName());
  llvm::StructType *BlockType = getBlockType(Module, Definition, symbol);
  llvm::Value *BlockHeader
      = getBlockHeader(Module, Definition, symbol, BlockType);
  int idx = 2;
  std::vector<llvm::Value *> children;
  for (auto &child : constructor->getArguments()) {
    llvm::Value *ChildValue;
    if (idx == 2 && val != nullptr) {
      ChildValue = val;
    } else {
      ChildValue
          = createAllocation(
                child.get(), fmt::format("{}:{}", locationStack, idx - 2))
                .first;
    }

    auto sort = dynamic_cast<KORECompositeSort *>(child->getSort().get());
    if (sort && isCollectionSort(sort->getCategory(Definition))) {
      ChildValue = new llvm::LoadInst(
          BlockType->elements()[idx], ChildValue, "", CurrentBlock);
    }
    children.push_back(ChildValue);
    idx++;
  }
  llvm::Value *Block = allocateTerm(BlockType, CurrentBlock);
  llvm::Value *BlockHeaderPtr = llvm::GetElementPtrInst::CreateInBounds(
      BlockType, Block,
      {llvm::ConstantInt::get(llvm::Type::getInt64Ty(Ctx), 0),
       llvm::ConstantInt::get(llvm::Type::getInt32Ty(Ctx), 0)},
      symbol->getName(), CurrentBlock);
  new llvm::StoreInst(BlockHeader, BlockHeaderPtr, CurrentBlock);
  idx = 2;
  for (auto &ChildValue : children) {
    llvm::Value *ChildPtr = llvm::GetElementPtrInst::CreateInBounds(
        BlockType, Block,
        {llvm::ConstantInt::get(llvm::Type::getInt64Ty(Ctx), 0),
         llvm::ConstantInt::get(llvm::Type::getInt32Ty(Ctx), idx++)},
        "", CurrentBlock);
    new llvm::StoreInst(ChildValue, ChildPtr, CurrentBlock);
  }

  auto BlockPtr
      = llvm::PointerType::getUnqual(getTypeByName(Module, BLOCK_STRUCT));
  auto bitcast = new llvm::BitCastInst(Block, BlockPtr, "", CurrentBlock);
  if (symbolDecl->getAttributes().count("binder")) {
    auto call = llvm::CallInst::Create(
        getOrInsertFunction(Module, "debruijnize", BlockPtr, BlockPtr), bitcast,
        "withIndices", CurrentBlock);
    setDebugLoc(call);
    return call;
  } else {
    return bitcast;
  }
}

// returns a value and a boolean indicating whether that value could be an
// injection
std::pair<llvm::Value *, bool> CreateTerm::operator()(KOREPattern *pattern) {
  populateStaticSet(pattern);

  return createAllocation(pattern);
}

bool CreateTerm::populateStaticSet(KOREPattern *pattern) {
  bool can_be_static = true;

  if (auto variable = dynamic_cast<KOREVariablePattern *>(pattern)) {
    can_be_static = false;
  } else if (auto constructor = dynamic_cast<KORECompositePattern *>(pattern)) {
    const KORESymbol *symbol = constructor->getConstructor();
    if (symbol->getName() != "\\dv") {
      KORESymbolDeclaration *symbolDecl
          = Definition->getSymbolDeclarations().at(symbol->getName());
      if (symbolDecl->getAttributes().count("function")
          || (symbolDecl->getAttributes().count("anywhere")
              && !isAnywhereOwise)) {
        can_be_static = false;
      }
      for (auto sptr_nextPattern : constructor->getArguments()) {
        KOREPattern *nextPattern = sptr_nextPattern.get();
        can_be_static &= populateStaticSet(nextPattern);
      }
    }
  }

  if (can_be_static) {
    staticTerms.insert(pattern);
  }

  return can_be_static;
}

std::pair<llvm::Value *, bool>
CreateTerm::createAllocation(KOREPattern *pattern, std::string locationStack) {
  if (staticTerms.count(pattern)) {
    auto staticTerm = new CreateStaticTerm(Definition, Module);
    return (*staticTerm)(pattern);
  }

  if (auto variable = dynamic_cast<KOREVariablePattern *>(pattern)) {
    auto val = Substitution.lookup(variable->getName());
    if (!val) {
      std::cerr << variable->getName() << std::endl;
      abort();
    }
    return std::make_pair(val, true);
  } else if (auto constructor = dynamic_cast<KORECompositePattern *>(pattern)) {
    const KORESymbol *symbol = constructor->getConstructor();
    assert(symbol->isConcrete() && "not supported yet: sort variables");
    KORESymbolDeclaration *symbolDecl
        = Definition->getSymbolDeclarations().at(symbol->getName());
    if (symbolDecl->getAttributes().count("function")
        || (symbolDecl->getAttributes().count("anywhere")
            && !isAnywhereOwise)) {
      if (symbolDecl->getAttributes().count("hook")) {
        return std::make_pair(
            createHook(
                symbolDecl->getAttributes().at("hook").get(), constructor,
                locationStack),
            true);
      } else {
        std::ostringstream Out;
        symbol->print(Out, 0, false);
        return std::make_pair(
            createFunctionCall(
                "eval_" + Out.str(), constructor, false, true, locationStack),
            true);
      }
    } else if (auto cat = dynamic_cast<KORECompositeSort *>(
                              symbol->getArguments()[0].get())
                              ->getCategory(Definition)
                              .cat;
               symbolDecl->getAttributes().count("sortInjection")
               && (cat == SortCategory::Symbol)) {
      std::pair<llvm::Value *, bool> val = createAllocation(
          constructor->getArguments()[0].get(),
          fmt::format("{}:0", locationStack));
      if (val.second) {
        llvm::Instruction *Tag = llvm::CallInst::Create(
            getOrInsertFunction(
                Module, "getTag", llvm::Type::getInt32Ty(Ctx),
                getValueType({SortCategory::Symbol, 0}, Module)),
            val.first, "tag", CurrentBlock);
        setDebugLoc(Tag);
        auto inj = Definition->getInjSymbol();
        auto NotStringBlock = llvm::BasicBlock::Create(
            Ctx, "notString", CurrentBlock->getParent());
        auto GeBlock = llvm::BasicBlock::Create(
            Ctx, "geFirst", CurrentBlock->getParent());
        auto FalseBlock = llvm::BasicBlock::Create(
            Ctx, "notInjection", CurrentBlock->getParent());
        auto TrueBlock
            = llvm::BasicBlock::Create(Ctx, "merge", CurrentBlock->getParent());
        auto cmp = new llvm::ICmpInst(
            *CurrentBlock, llvm::CmpInst::ICMP_NE, Tag,
            llvm::ConstantInt::getSigned(llvm::Type::getInt32Ty(Ctx), -1));
        llvm::BranchInst::Create(NotStringBlock, FalseBlock, cmp, CurrentBlock);

        CurrentBlock = NotStringBlock;
        cmp = new llvm::ICmpInst(
            *CurrentBlock, llvm::CmpInst::ICMP_UGE, Tag,
            llvm::ConstantInt::get(
                llvm::Type::getInt32Ty(Ctx), inj->getFirstTag()));
        llvm::BranchInst::Create(GeBlock, FalseBlock, cmp, CurrentBlock);

        CurrentBlock = GeBlock;
        cmp = new llvm::ICmpInst(
            *CurrentBlock, llvm::CmpInst::ICMP_ULE, Tag,
            llvm::ConstantInt::get(
                llvm::Type::getInt32Ty(Ctx), inj->getLastTag()));
        llvm::BranchInst::Create(TrueBlock, FalseBlock, cmp, CurrentBlock);

        CurrentBlock = FalseBlock;
        auto Cast = notInjectionCase(constructor, val.first, locationStack);
        llvm::BranchInst::Create(TrueBlock, CurrentBlock);

        CurrentBlock = TrueBlock;
        llvm::PHINode *Phi
            = llvm::PHINode::Create(Cast->getType(), 2, "phi", CurrentBlock);
        Phi->addIncoming(Cast, FalseBlock);
        Phi->addIncoming(val.first, GeBlock);
        return std::make_pair(Phi, true);
      } else {
        return std::make_pair(
            notInjectionCase(constructor, val.first, locationStack), true);
      }
    } else {
      return std::make_pair(
          notInjectionCase(constructor, nullptr, locationStack), false);
    }
  } else {
    assert(false && "not supported yet: meta level");
    abort();
  }
}

void addAbort(llvm::BasicBlock *block, llvm::Module *Module) {
  llvm::FunctionType *AbortType = llvm::FunctionType::get(
      llvm::Type::getVoidTy(Module->getContext()), false);
  llvm::Function *AbortFunc = getOrInsertFunction(Module, "abort", AbortType);
  AbortFunc->addFnAttr(llvm::Attribute::NoReturn);
  llvm::CallInst::Create(AbortFunc, "", block);
  new llvm::UnreachableInst(Module->getContext(), block);
}

void writeUInt64(
    llvm::Value *outputFile, llvm::Module *Module, uint64_t value,
    llvm::BasicBlock *Block) {
  llvm::CallInst::Create(
      getOrInsertFunction(
          Module, "writeUInt64ToFile",
          llvm::Type::getVoidTy(Module->getContext()),
          llvm::Type::getInt8PtrTy(Module->getContext()),
          llvm::Type::getInt64Ty(Module->getContext())),
      {outputFile, llvm::ConstantInt::get(
                       llvm::Type::getInt64Ty(Module->getContext()), value)},
      "", Block);
}

bool makeFunction(
    std::string name, KOREPattern *pattern, KOREDefinition *definition,
    llvm::Module *Module, bool tailcc, bool bigStep, bool apply,
    KOREAxiomDeclaration *axiom, std::string postfix) {
  std::map<std::string, KOREVariablePattern *> vars;
  if (apply) {
    for (KOREPattern *lhs : axiom->getLeftHandSide()) {
      lhs->markVariables(vars);
    }
  }
  pattern->markVariables(vars);
  llvm::StringMap<ValueType> params;
  std::vector<llvm::Type *> paramTypes;
  std::vector<std::string> paramNames;
  std::vector<llvm::Metadata *> debugArgs;
  for (auto iter = vars.begin(); iter != vars.end(); ++iter) {
    auto &entry = *iter;
    auto sort
        = dynamic_cast<KORECompositeSort *>(entry.second->getSort().get());
    if (!sort) {
      // TODO: sort variables
      return false;
    }
    auto cat = sort->getCategory(definition);
    std::ostringstream Out;
    sort->print(Out);
    llvm::Type *paramType = getValueType(cat, Module);
    debugArgs.push_back(getDebugType(cat, Out.str()));
    switch (cat.cat) {
    case SortCategory::Map:
    case SortCategory::RangeMap:
    case SortCategory::List:
    case SortCategory::Set:
      paramType = llvm::PointerType::getUnqual(paramType);
      break;
    default: break;
    }

    params.insert({entry.first, cat});
    paramTypes.push_back(paramType);
    paramNames.push_back(entry.first);
  }
  ValueType returnCat = termType(pattern, params, definition);
  auto returnType = getValueType(returnCat, Module);
  switch (returnCat.cat) {
  case SortCategory::Map:
  case SortCategory::RangeMap:
  case SortCategory::List:
  case SortCategory::Set:
    returnType = llvm::PointerType::getUnqual(returnType);
    break;
  default: break;
  }
  llvm::FunctionType *funcType
      = llvm::FunctionType::get(returnType, paramTypes, false);
  llvm::Function *applyRule = getOrInsertFunction(Module, name, funcType);
  initDebugAxiom(axiom->getAttributes());
  std::string debugName = name;
  if (axiom->getAttributes().count("label")) {
    debugName = axiom->getStringAttribute("label") + postfix;
  }
  std::ostringstream Out;
  termSort(pattern)->print(Out);
  initDebugFunction(
      debugName, debugName,
      getDebugFunctionType(getDebugType(returnCat, Out.str()), debugArgs),
      definition, applyRule);
  if (tailcc) {
    applyRule->setCallingConv(llvm::CallingConv::Tail);
  }
  llvm::StringMap<llvm::Value *> subst;
  llvm::BasicBlock *block
      = llvm::BasicBlock::Create(Module->getContext(), "entry", applyRule);
  int i = 0;
  for (auto val = applyRule->arg_begin(); val != applyRule->arg_end();
       ++val, ++i) {
    subst.insert({paramNames[i], val});
    if (debugArgs[i]) {
      initDebugParam(
          applyRule, i, paramNames[i], params[paramNames[i]],
          llvm::dyn_cast<llvm::DIType>(debugArgs[i])->getName().str());
    }
  }
  CreateTerm creator = CreateTerm(subst, definition, block, Module, false);
  llvm::Value *retval = creator(pattern).first;

  auto CurrentBlock = creator.getCurrentBlock();
  if (apply && bigStep) {
    auto ProofOutputFlag = Module->getOrInsertGlobal(
        "proof_output", llvm::Type::getInt1Ty(Module->getContext()));
    auto OutputFileName = Module->getOrInsertGlobal(
        "output_file", llvm::Type::getInt8PtrTy(Module->getContext()));
    auto proofOutput = new llvm::LoadInst(
        llvm::Type::getInt1Ty(Module->getContext()), ProofOutputFlag,
        "proof_output", CurrentBlock);
    llvm::BasicBlock *TrueBlock
        = llvm::BasicBlock::Create(Module->getContext(), "if", applyRule);
    auto ir = new llvm::IRBuilder(TrueBlock);
    llvm::BasicBlock *MergeBlock
        = llvm::BasicBlock::Create(Module->getContext(), "tail", applyRule);
    llvm::BranchInst::Create(TrueBlock, MergeBlock, proofOutput, CurrentBlock);
    auto outputFile = new llvm::LoadInst(
        llvm::Type::getInt8PtrTy(Module->getContext()), OutputFileName,
        "output", TrueBlock);

    writeUInt64(outputFile, Module, 0xffffffffffffffff, TrueBlock);
    ir->CreateCall(
        getOrInsertFunction(
            Module, "serializeConfigurationToFile",
            llvm::Type::getVoidTy(Module->getContext()),
            llvm::Type::getInt8PtrTy(Module->getContext()),
            getValueType({SortCategory::Symbol, 0}, Module)),
        {outputFile, retval});
    writeUInt64(outputFile, Module, 0xcccccccccccccccc, TrueBlock);

    llvm::BranchInst::Create(MergeBlock, TrueBlock);
    CurrentBlock = MergeBlock;
  }

  if (bigStep) {
    llvm::Type *blockType = getValueType({SortCategory::Symbol, 0}, Module);
    llvm::Function *step = getOrInsertFunction(
        Module, "k_step",
        llvm::FunctionType::get(blockType, {blockType}, false));
    auto call = llvm::CallInst::Create(step, {retval}, "", CurrentBlock);
    setDebugLoc(call);
    call->setCallingConv(llvm::CallingConv::Tail);
    retval = call;
  }
  auto ret
      = llvm::ReturnInst::Create(Module->getContext(), retval, CurrentBlock);
  setDebugLoc(ret);
  return true;
}

void makeApplyRuleFunction(
    KOREAxiomDeclaration *axiom, KOREDefinition *definition,
    llvm::Module *Module, bool bigStep) {
  KOREPattern *pattern = axiom->getRightHandSide();
  std::string name = "apply_rule_" + std::to_string(axiom->getOrdinal());
  makeFunction(
      name, pattern, definition, Module, true, bigStep, true, axiom, ".rhs");
  if (bigStep) {
    makeFunction(
        name + "_search", pattern, definition, Module, true, false, true, axiom,
        ".rhs");
  }
}

std::string makeApplyRuleFunction(
    KOREAxiomDeclaration *axiom, KOREDefinition *definition,
    llvm::Module *Module, std::vector<Residual> residuals) {
  std::map<std::string, KOREVariablePattern *> vars;
  for (auto residual : residuals) {
    residual.pattern->markVariables(vars);
  }
  for (KOREPattern *lhs : axiom->getLeftHandSide()) {
    lhs->markVariables(vars);
  }
  llvm::StringMap<ValueType> params;
  std::vector<llvm::Type *> paramTypes;
  std::vector<std::string> paramNames;
  std::vector<llvm::Metadata *> debugArgs;
  for (auto iter = vars.begin(); iter != vars.end(); ++iter) {
    auto &entry = *iter;
    auto sort
        = dynamic_cast<KORECompositeSort *>(entry.second->getSort().get());
    if (!sort) {
      // TODO: sort variables
      return "";
    }
    auto cat = sort->getCategory(definition);
    std::ostringstream Out;
    sort->print(Out);
    llvm::Type *paramType = getValueType(cat, Module);
    debugArgs.push_back(getDebugType(cat, Out.str()));
    switch (cat.cat) {
    case SortCategory::Map:
    case SortCategory::RangeMap:
    case SortCategory::List:
    case SortCategory::Set:
      paramType = llvm::PointerType::getUnqual(paramType);
      break;
    default: break;
    }

    params.insert({entry.first, cat});
    paramTypes.push_back(paramType);
    paramNames.push_back(entry.first);
  }
  llvm::FunctionType *funcType = llvm::FunctionType::get(
      getValueType({SortCategory::Symbol, 0}, Module), paramTypes, false);
  std::string name = "apply_rule_" + std::to_string(axiom->getOrdinal());

  makeFunction(
      name + "_search", axiom->getRightHandSide(), definition, Module, true,
      false, true, axiom, ".rhs");

  llvm::Function *applyRule = getOrInsertFunction(Module, name, funcType);
  initDebugAxiom(axiom->getAttributes());
  initDebugFunction(
      name, name,
      getDebugFunctionType(
          getDebugType({SortCategory::Symbol, 0}, "SortGeneratedTopCell{}"),
          debugArgs),
      definition, applyRule);
  applyRule->setCallingConv(llvm::CallingConv::Tail);
  llvm::StringMap<llvm::Value *> subst;
  llvm::BasicBlock *block
      = llvm::BasicBlock::Create(Module->getContext(), "entry", applyRule);
  int i = 0;
  for (auto val = applyRule->arg_begin(); val != applyRule->arg_end();
       ++val, ++i) {
    subst.insert({paramNames[i], val});
    if (debugArgs[i]) {
      initDebugParam(
          applyRule, i, paramNames[i], params[paramNames[i]],
          llvm::dyn_cast<llvm::DIType>(debugArgs[i])->getName().str());
    }
  }
  CreateTerm creator = CreateTerm(subst, definition, block, Module, false);
  std::vector<llvm::Value *> args;
  std::vector<llvm::Type *> types;
  for (auto residual : residuals) {
    llvm::Value *arg = creator(residual.pattern).first;
    auto sort
        = dynamic_cast<KORECompositeSort *>(residual.pattern->getSort().get());
    auto cat = sort->getCategory(definition);
    switch (cat.cat) {
    case SortCategory::Map:
    case SortCategory::RangeMap:
    case SortCategory::List:
    case SortCategory::Set:
      if (!arg->getType()->isPointerTy()) {
        auto ptr = allocateTerm(
            arg->getType(), creator.getCurrentBlock(), "koreAllocAlwaysGC");
        new llvm::StoreInst(arg, ptr, creator.getCurrentBlock());
        arg = ptr;
      }
      break;
    default: break;
    }
    args.push_back(arg);
    types.push_back(arg->getType());
  }
  llvm::Type *blockType = getValueType({SortCategory::Symbol, 0}, Module);
  llvm::Function *step = getOrInsertFunction(
      Module, "step_" + std::to_string(axiom->getOrdinal()),
      llvm::FunctionType::get(blockType, types, false));
  auto retval
      = llvm::CallInst::Create(step, args, "", creator.getCurrentBlock());
  setDebugLoc(retval);
  retval->setCallingConv(llvm::CallingConv::Tail);
  llvm::ReturnInst::Create(
      Module->getContext(), retval, creator.getCurrentBlock());
  return name;
}

std::string makeSideConditionFunction(
    KOREAxiomDeclaration *axiom, KOREDefinition *definition,
    llvm::Module *Module) {
  KOREPattern *pattern = axiom->getRequires();

  if (!pattern) {
    return "";
  }
  std::string name = "side_condition_" + std::to_string(axiom->getOrdinal());
  if (makeFunction(
          name, pattern, definition, Module, false, false, false, axiom,
          ".sc")) {
    return name;
  }
  return "";
}

llvm::Type *getArgType(ValueType cat, llvm::Module *mod) {
  switch (cat.cat) {
  case SortCategory::Bool:
  case SortCategory::MInt:
  case SortCategory::Map:
  case SortCategory::RangeMap:
  case SortCategory::List:
  case SortCategory::Set: {
    return getValueType(cat, mod);
  }
  case SortCategory::Int: return getTypeByName(mod, INT_STRUCT);
  case SortCategory::Float: return getTypeByName(mod, FLOAT_STRUCT);
  case SortCategory::StringBuffer: return getTypeByName(mod, BUFFER_STRUCT);
  case SortCategory::Symbol:
  case SortCategory::Variable: {
    return getBlockType(mod);
  }

  case SortCategory::Uncomputed:
  default: {
    abort();
  }
  }
}

bool isCollectionSort(ValueType cat) {
  switch (cat.cat) {
  case SortCategory::Map:
  case SortCategory::RangeMap:
  case SortCategory::List:
  case SortCategory::Set: return true;
  default: return false;
  }
}

} // namespace kllvm
