#include "kllvm/codegen/CreateTerm.h"
#include "kllvm/codegen/GenAlloc.h"
#include "kllvm/codegen/Debug.h"

#include <gmp.h>
#include <iomanip>
#include <iostream>

#include "llvm/IRReader/IRReader.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Support/SourceMgr.h"
#include "llvm/Support/raw_ostream.h"
#include "runtime/header.h" //for macros

namespace kllvm {

std::string LLVM_HEADER = R"LLVM(
; Linux target
target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

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
declare void @printConfiguration(i32, %block *)
)LLVM";

std::unique_ptr<llvm::Module> newModule(std::string name, llvm::LLVMContext &Context) {
  llvm::SMDiagnostic Err;
  auto mod = llvm::parseIR(*llvm::MemoryBuffer::getMemBuffer(LLVM_HEADER), Err, Context);
  if (!mod) {
    Err.print("header.ll", llvm::errs());
  }
  return mod;
}

static std::string MAP_STRUCT = "map";
static std::string LIST_STRUCT = "list";
static std::string SET_STRUCT = "set";
static std::string INT_WRAPPER_STRUCT = "mpz_hdr";
static std::string INT_STRUCT = "mpz";
static std::string FLOAT_WRAPPER_STRUCT = "floating_hdr";
static std::string FLOAT_STRUCT = "floating";
static std::string BUFFER_STRUCT = "stringbuffer";
static std::string BLOCK_STRUCT = "block";
static std::string BLOCKHEADER_STRUCT = "blockheader";

llvm::Type *getValueType(ValueType sort, llvm::Module *Module) {
  switch(sort.cat) {
  case SortCategory::Map:
    return Module->getTypeByName(MAP_STRUCT);
  case SortCategory::List:
    return Module->getTypeByName(LIST_STRUCT);
  case SortCategory::Set:
    return Module->getTypeByName(SET_STRUCT);
  case SortCategory::Int:
    return llvm::PointerType::getUnqual(Module->getTypeByName(INT_STRUCT));
  case SortCategory::Float:
    return llvm::PointerType::getUnqual(Module->getTypeByName(FLOAT_STRUCT));
  case SortCategory::StringBuffer:
    return llvm::PointerType::getUnqual(Module->getTypeByName(BUFFER_STRUCT));
  case SortCategory::Bool:
    return llvm::Type::getInt1Ty(Module->getContext());
  case SortCategory::MInt:
    return llvm::IntegerType::get(Module->getContext(), sort.bits);
  case SortCategory::Symbol:
  case SortCategory::Variable:
    return llvm::PointerType::getUnqual(Module->getTypeByName(BLOCK_STRUCT));
  case SortCategory::Uncomputed:
    abort();
  }
}


llvm::StructType *getBlockType(llvm::Module *Module, KOREDefinition *definition, const KORESymbol *symbol) {
  llvm::StructType *BlockHeaderType = Module->getTypeByName(BLOCKHEADER_STRUCT);
  llvm::ArrayType *EmptyArrayType = llvm::ArrayType::get(llvm::Type::getInt64Ty(Module->getContext()), 0);
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

llvm::Value *getBlockHeader(llvm::Module *Module, KOREDefinition *definition, const KORESymbol *symbol, llvm::Type *BlockType) {
  llvm::StructType *BlockHeaderType = Module->getTypeByName(BLOCKHEADER_STRUCT);
  uint64_t headerVal = symbol->getTag();
  uint64_t sizeInBytes = llvm::DataLayout(Module).getTypeAllocSize(BlockType);
  assert(sizeInBytes % 8 == 0);
  headerVal |= (sizeInBytes / 8) << 32;
  headerVal |= (uint64_t)symbol->getLayout() << LAYOUT_OFFSET;
  return llvm::ConstantStruct::get(BlockHeaderType, llvm::ConstantInt::get(llvm::Type::getInt64Ty(Module->getContext()), headerVal));
}

llvm::Value *allocateTerm(llvm::Type *AllocType, llvm::BasicBlock *block, const char *allocFn) {
  return allocateTerm(AllocType, llvm::ConstantExpr::getSizeOf(AllocType), block, allocFn);
}

llvm::Value *allocateTerm(llvm::Type *AllocType, llvm::Value *Len, llvm::BasicBlock *block, const char *allocFn) {
  llvm::Instruction *Malloc = llvm::CallInst::CreateMalloc(block, llvm::Type::getInt64Ty(block->getContext()), AllocType, Len, nullptr, koreHeapAlloc(allocFn, block->getModule()));
  setDebugLoc(&block->getInstList().back());
  block->getInstList().push_back(Malloc);
  return Malloc;
}

ValueType termType(KOREPattern *pattern, llvm::StringMap<ValueType> &substitution, KOREDefinition *definition) {
  if (auto variable = dynamic_cast<KOREVariablePattern *>(pattern)) {
    return substitution.lookup(variable->getName());
  } else if (auto constructor = dynamic_cast<KORECompositePattern *>(pattern)) {
    KORESymbol *symbol = constructor->getConstructor();
    assert(symbol->isConcrete() && "not supported yet: sort variables");
    if (symbol->getName() == "\\dv") {
      auto sort = dynamic_cast<KORECompositeSort *>(symbol->getFormalArguments()[0].get());
      return sort->getCategory(definition);
    }
    auto sort = dynamic_cast<KORECompositeSort *>(symbol->getSort().get());
    return sort->getCategory(definition);
  } else {
    assert(false && "not supported yet: meta level");
    abort();
  }
}

std::string escape(std::string str) {
  std::stringstream os;
  os << std::setfill('0') << std::setw(2) << std::hex;
  for (char c : str) {
    unsigned char uc = c;
    os << (int)uc;
  }
  return os.str();
}

llvm::Value *CreateTerm::createToken(ValueType sort, std::string contents) {
  switch(sort.cat) {
  case SortCategory::Map:
  case SortCategory::List:
  case SortCategory::Set:
    assert (false && "cannot create tokens of collection category");
  case SortCategory::Int: {
    llvm::Constant *global = Module->getOrInsertGlobal("int_" + contents, Module->getTypeByName(INT_WRAPPER_STRUCT));
    llvm::GlobalVariable *globalVar = llvm::dyn_cast<llvm::GlobalVariable>(global);
    if (!globalVar->hasInitializer()) {
      mpz_t value;
      const char * dataStart = contents.at(0) == '+' ? contents.c_str() + 1 : contents.c_str();
      mpz_init_set_str(value, dataStart, 10);
      size_t size = mpz_size(value);
      int sign = mpz_sgn(value);
      llvm::ArrayType *limbsType = llvm::ArrayType::get(llvm::Type::getInt64Ty(Ctx), size);
      llvm::Constant *limbs = Module->getOrInsertGlobal("int_" + contents + "_limbs", limbsType);
      llvm::GlobalVariable *limbsVar = llvm::dyn_cast<llvm::GlobalVariable>(limbs);
      std::vector<llvm::Constant *> allocdLimbs;
      for (size_t i = 0; i < size; i++) {
        allocdLimbs.push_back(llvm::ConstantInt::get(llvm::Type::getInt64Ty(Ctx), value->_mp_d[i]));
      }
      limbsVar->setInitializer(llvm::ConstantArray::get(limbsType, allocdLimbs));
      llvm::Constant *hdr = llvm::ConstantStruct::get(Module->getTypeByName(BLOCKHEADER_STRUCT), llvm::ConstantInt::get(llvm::Type::getInt64Ty(Ctx), sizeof(mpz_hdr) - sizeof(blockheader) | NOT_YOUNG_OBJECT_BIT));
      llvm::ConstantInt *numLimbs = llvm::ConstantInt::get(llvm::Type::getInt32Ty(Ctx), size);
      llvm::Constant *mp_size = llvm::ConstantExpr::getMul(numLimbs, llvm::ConstantInt::getSigned(llvm::Type::getInt32Ty(Ctx), sign));
      globalVar->setInitializer(llvm::ConstantStruct::get(
        Module->getTypeByName(INT_WRAPPER_STRUCT), hdr,
        llvm::ConstantStruct::get(Module->getTypeByName(INT_STRUCT), numLimbs, mp_size, llvm::ConstantExpr::getPointerCast(limbsVar, llvm::Type::getInt64PtrTy(Ctx)))));
      mpz_clear(value);
    }
    std::vector<llvm::Constant *> Idxs = {llvm::ConstantInt::get(llvm::Type::getInt64Ty(Ctx), 0), llvm::ConstantInt::get(llvm::Type::getInt32Ty(Ctx), 1)};
    return llvm::ConstantExpr::getInBoundsGetElementPtr(Module->getTypeByName(INT_WRAPPER_STRUCT), globalVar, Idxs);
  }
  case SortCategory::Float: {
    llvm::Constant *global = Module->getOrInsertGlobal("float_" + contents, Module->getTypeByName(FLOAT_WRAPPER_STRUCT));
    llvm::GlobalVariable *globalVar = llvm::dyn_cast<llvm::GlobalVariable>(global);
    if (!globalVar->hasInitializer()) {
      size_t prec, exp;
      const char last = contents.back();
      if (last == 'f' || last == 'F' || last == 'y') {
        prec = 24;
        exp = 8;
      } else {
        size_t has_prec = contents.find_first_of("pP");
        if (has_prec == std::string::npos) {
           prec = 53;
           exp = 11;
        } else {
          size_t exp_idx = contents.find_first_of("xX");
          std::string prec_str = contents.substr(has_prec+1, exp_idx-has_prec);
          std::string exp_str = contents.substr(exp_idx+1);
          prec = atoll(prec_str.c_str());
          exp = atoll(exp_str.c_str());
        }
      }
      mpfr_t value;
      mpfr_init2(value, prec);
      int retValue;
      if (contents == "+Infinity" || contents == "-Infinity") {
        retValue = mpfr_set_str(value, contents.c_str(), 10, MPFR_RNDN);
      } else {
        size_t last = contents.find_last_of("fFdDpP");
        std::string str_value = contents.substr(0, last);
        retValue = mpfr_set_str(value, str_value.c_str(), 10, MPFR_RNDN);
      }
      if (retValue != 0) {
        throw std::invalid_argument("Can't convert to float");
      }
      size_t size = (prec + 63) / 64;
      llvm::ArrayType *limbsType = llvm::ArrayType::get(llvm::Type::getInt64Ty(Ctx), size);
      llvm::Constant *limbs = Module->getOrInsertGlobal("float_" + contents + "_limbs", limbsType);
      llvm::GlobalVariable *limbsVar = llvm::dyn_cast<llvm::GlobalVariable>(limbs);
      std::vector<llvm::Constant *> allocdLimbs;
      for (size_t i = 0; i < size; i++) {
        allocdLimbs.push_back(llvm::ConstantInt::get(llvm::Type::getInt64Ty(Ctx), value->_mpfr_d[i]));
      }
      limbsVar->setInitializer(llvm::ConstantArray::get(limbsType, allocdLimbs));
      llvm::Constant *hdr = llvm::ConstantStruct::get(Module->getTypeByName(BLOCKHEADER_STRUCT), llvm::ConstantInt::get(llvm::Type::getInt64Ty(Ctx), (sizeof(floating_hdr) - sizeof(blockheader)) | NOT_YOUNG_OBJECT_BIT));
      llvm::Constant *expbits = llvm::ConstantInt::get(llvm::Type::getInt64Ty(Ctx), exp);
      llvm::Constant *mpfr_prec = llvm::ConstantInt::get(llvm::Type::getInt64Ty(Ctx), prec);
      llvm::Constant *mpfr_sign = llvm::ConstantInt::getSigned(llvm::Type::getInt32Ty(Ctx), value->_mpfr_sign);
      llvm::Constant *mpfr_exp = llvm::ConstantInt::getSigned(llvm::Type::getInt64Ty(Ctx), value->_mpfr_exp);
      // create struct floating with an exponent range and an __mpfr_struct. Note that we are assuming the format of the struct, but it's unlikely to change except possibly between major releases
      // which happen less than once every couple years, because the C++ ABI depends on it. We are also assuming that the host and target have the same arch, but since we don't yet support
      // cross compiling anyway, that's a safe assumption.
      globalVar->setInitializer(llvm::ConstantStruct::get(
        Module->getTypeByName(FLOAT_WRAPPER_STRUCT), hdr,
        llvm::ConstantStruct::get(Module->getTypeByName(FLOAT_STRUCT), expbits, llvm::ConstantStruct::getAnon({mpfr_prec, mpfr_sign, mpfr_exp, llvm::ConstantExpr::getPointerCast(limbsVar, llvm::Type::getInt64PtrTy(Ctx))}))));
      mpfr_clear(value);
    }
    std::vector<llvm::Constant *> Idxs = {llvm::ConstantInt::get(llvm::Type::getInt64Ty(Ctx), 0), llvm::ConstantInt::get(llvm::Type::getInt32Ty(Ctx), 1)};
    return llvm::ConstantExpr::getInBoundsGetElementPtr(Module->getTypeByName(FLOAT_WRAPPER_STRUCT), globalVar, Idxs);
  }
  case SortCategory::StringBuffer:
  case SortCategory::MInt:
    assert(false && "not implemented yet: tokens");
  case SortCategory::Bool:
    return llvm::ConstantInt::get(llvm::Type::getInt1Ty(Ctx), contents == "true");
  case SortCategory::Variable:
  case SortCategory::Symbol: {
    llvm::StructType *StringType = llvm::StructType::get(Ctx, {Module->getTypeByName(BLOCKHEADER_STRUCT), llvm::ArrayType::get(llvm::Type::getInt8Ty(Ctx), contents.size())});
    llvm::Constant *global = Module->getOrInsertGlobal("token_" + escape(contents), StringType);
    llvm::GlobalVariable *globalVar = llvm::dyn_cast<llvm::GlobalVariable>(global);
    if (!globalVar->hasInitializer()) {
      llvm::StructType *BlockHeaderType = Module->getTypeByName(BLOCKHEADER_STRUCT);
      // this object does not live on the young generation, so we need to set the correct gc bit.
      llvm::Constant *BlockHeader = llvm::ConstantStruct::get(BlockHeaderType, llvm::ConstantInt::get(llvm::Type::getInt64Ty(Ctx), contents.size() | NOT_YOUNG_OBJECT_BIT));
      globalVar->setInitializer(llvm::ConstantStruct::get(StringType, BlockHeader, llvm::ConstantDataArray::getString(Ctx, contents, false)));
    }
    return llvm::ConstantExpr::getPointerCast(global, llvm::PointerType::getUnqual(Module->getTypeByName(BLOCK_STRUCT)));
  }
  case SortCategory::Uncomputed:
    abort();
  }
}

llvm::Value *CreateTerm::createHook(KORECompositePattern *hookAtt, KORECompositePattern *pattern) {
  assert(hookAtt->getArguments().size() == 1);
  auto strPattern = dynamic_cast<KOREStringPattern *>(hookAtt->getArguments()[0].get());
  std::string name = strPattern->getContents();
  if (name == "BOOL.and" || name == "BOOL.andThen") {
    assert(pattern->getArguments().size() == 2);
    llvm::Value *firstArg = (*this)(pattern->getArguments()[0].get()).first;
    llvm::BasicBlock *CondBlock = CurrentBlock;
    llvm::BasicBlock *TrueBlock = llvm::BasicBlock::Create(Ctx, "then", CurrentBlock->getParent());
    llvm::BasicBlock *MergeBlock = llvm::BasicBlock::Create(Ctx, "hook_BOOL_and", CurrentBlock->getParent());
    llvm::BranchInst::Create(TrueBlock, MergeBlock, firstArg, CurrentBlock);
    CurrentBlock = TrueBlock;
    llvm::Value *secondArg = (*this)(pattern->getArguments()[1].get()).first;
    llvm::BranchInst::Create(MergeBlock, CurrentBlock);
    llvm::PHINode *Phi = llvm::PHINode::Create(llvm::Type::getInt1Ty(Ctx), 2, "phi", MergeBlock);
    Phi->addIncoming(secondArg, CurrentBlock);
    Phi->addIncoming(firstArg, CondBlock);
    CurrentBlock = MergeBlock;
    return Phi;
  } else if (name == "BOOL.or" || name == "BOOL.orElse") {
    assert(pattern->getArguments().size() == 2);
    llvm::Value *firstArg = (*this)(pattern->getArguments()[0].get()).first;
    llvm::BasicBlock *CondBlock = CurrentBlock;
    llvm::BasicBlock *FalseBlock = llvm::BasicBlock::Create(Ctx, "else", CurrentBlock->getParent());
    llvm::BasicBlock *MergeBlock = llvm::BasicBlock::Create(Ctx, "hook_BOOL_or", CurrentBlock->getParent());
    llvm::BranchInst::Create(MergeBlock, FalseBlock, firstArg, CurrentBlock);
    CurrentBlock = FalseBlock;
    llvm::Value *secondArg = (*this)(pattern->getArguments()[1].get()).first;
    llvm::BranchInst::Create(MergeBlock, CurrentBlock);
    llvm::PHINode *Phi = llvm::PHINode::Create(llvm::Type::getInt1Ty(Ctx), 2, "phi", MergeBlock);
    Phi->addIncoming(secondArg, CurrentBlock);
    Phi->addIncoming(firstArg, CondBlock);
    CurrentBlock = MergeBlock;
    return Phi;
  } else if (name == "BOOL.not") {
    assert(pattern->getArguments().size() == 1);
    llvm::Value *arg = (*this)(pattern->getArguments()[0].get()).first;
    llvm::BinaryOperator *Not = llvm::BinaryOperator::Create(llvm::Instruction::Xor, arg, llvm::ConstantInt::get(llvm::Type::getInt1Ty(Ctx), 1), "hook_BOOL_not", CurrentBlock);
    return Not;
  } else if (name == "BOOL.implies") {
    assert(pattern->getArguments().size() == 2);
    llvm::Value *firstArg = (*this)(pattern->getArguments()[0].get()).first;
    llvm::BasicBlock *CondBlock = CurrentBlock;
    llvm::BasicBlock *TrueBlock = llvm::BasicBlock::Create(Ctx, "then", CurrentBlock->getParent());
    llvm::BasicBlock *MergeBlock = llvm::BasicBlock::Create(Ctx, "hook_BOOL_implies", CurrentBlock->getParent());
    llvm::BranchInst::Create(TrueBlock, MergeBlock, firstArg, CurrentBlock);
    CurrentBlock = TrueBlock;
    llvm::Value *secondArg = (*this)(pattern->getArguments()[1].get()).first;
    llvm::BranchInst::Create(MergeBlock, CurrentBlock);
    llvm::PHINode *Phi = llvm::PHINode::Create(llvm::Type::getInt1Ty(Ctx), 2, "phi", MergeBlock);
    Phi->addIncoming(secondArg, CurrentBlock);
    Phi->addIncoming(llvm::ConstantInt::get(llvm::Type::getInt1Ty(Ctx), 1), CondBlock);
    CurrentBlock = MergeBlock;
    return Phi;
  } else if (name == "BOOL.ne" || name == "BOOL.xor") {
    assert(pattern->getArguments().size() == 2);
    llvm::Value *firstArg = (*this)(pattern->getArguments()[0].get()).first;
    llvm::Value *secondArg = (*this)(pattern->getArguments()[1].get()).first;
    llvm::BinaryOperator *Xor = llvm::BinaryOperator::Create(llvm::Instruction::Xor, firstArg, secondArg, "hook_BOOL_ne", CurrentBlock);
    return Xor;
  } else if (name == "BOOL.eq") {
    assert(pattern->getArguments().size() == 2);
    llvm::Value *firstArg = (*this)(pattern->getArguments()[0].get()).first;
    llvm::Value *secondArg = (*this)(pattern->getArguments()[1].get()).first;
    llvm::ICmpInst *Eq = new llvm::ICmpInst(*CurrentBlock, llvm::CmpInst::ICMP_EQ, firstArg, secondArg, "hook_BOOL_eq");
    return Eq;
  } else if (name == "KEQUAL.ite") {
    assert(pattern->getArguments().size() == 3);
    llvm::Value *cond = (*this)(pattern->getArguments()[0].get()).first;
    llvm::BasicBlock *TrueBlock = llvm::BasicBlock::Create(Ctx, "then", CurrentBlock->getParent());
    llvm::BasicBlock *FalseBlock = llvm::BasicBlock::Create(Ctx, "else", CurrentBlock->getParent());
    llvm::BasicBlock *MergeBlock = llvm::BasicBlock::Create(Ctx, "hook_KEQUAL_ite", CurrentBlock->getParent());
    llvm::BranchInst::Create(TrueBlock, FalseBlock, cond, CurrentBlock);
    CurrentBlock = TrueBlock;
    llvm::Value *trueArg = (*this)(pattern->getArguments()[1].get()).first;
    llvm::BasicBlock *NewTrueBlock = CurrentBlock;
    CurrentBlock = FalseBlock;
    llvm::Value *falseArg = (*this)(pattern->getArguments()[2].get()).first;
    if (trueArg->getType()->isPointerTy() && !falseArg->getType()->isPointerTy()) {
      llvm::AllocaInst *AllocCollection = new llvm::AllocaInst(falseArg->getType(), 0, "", CurrentBlock);
      new llvm::StoreInst(falseArg, AllocCollection, CurrentBlock);
      falseArg = AllocCollection;
    } else if (!trueArg->getType()->isPointerTy() && falseArg->getType()->isPointerTy()) {
      llvm::AllocaInst *AllocCollection = new llvm::AllocaInst(trueArg->getType(), 0, "", NewTrueBlock);
      new llvm::StoreInst(trueArg, AllocCollection, NewTrueBlock);
      trueArg = AllocCollection;
    }
    llvm::BranchInst::Create(MergeBlock, CurrentBlock);
    llvm::BranchInst::Create(MergeBlock, NewTrueBlock);
    llvm::PHINode *Phi = llvm::PHINode::Create(trueArg->getType(), 2, "phi", MergeBlock);
    Phi->addIncoming(trueArg, NewTrueBlock);
    Phi->addIncoming(falseArg, CurrentBlock);
    CurrentBlock = MergeBlock;
    return Phi;
  } else if (!name.compare(0, 5, "MINT.")) {
    assert(false && "not implemented yet: MInt");
    abort();
  } else {
    std::string domain = name.substr(0, name.find('.'));
    if (domain == "ARRAY") {
      // array is not really hooked in llvm, it's implemented in K
      std::ostringstream Out;
      pattern->getConstructor()->print(Out, 0, false);
      return createFunctionCall("eval_" + Out.str(), pattern, false, true);
    }
    std::string hookName = "hook_" + domain + "_" + name.substr(name.find('.') + 1);
    return createFunctionCall(hookName, pattern, true, false);
  }
}

// we use fastcc calling convention for apply_rule_* and eval_* functions so that the
// -tailcallopt LLVM pass can be used to make K functions tail recursive when their K
// definitions are tail recursive.
llvm::Value *CreateTerm::createFunctionCall(std::string name, KORECompositePattern *pattern, bool sret, bool fastcc) {
  std::vector<llvm::Value *> args;
  auto returnSort = dynamic_cast<KORECompositeSort *>(pattern->getConstructor()->getSort().get());
  auto returnCat = returnSort->getCategory(Definition);
  int i = 0;
  for (auto sort : pattern->getConstructor()->getArguments()) {
    auto concreteSort = dynamic_cast<KORECompositeSort *>(sort.get());
    llvm::Value *arg = (*this)(pattern->getArguments()[i++].get()).first;
    switch(concreteSort->getCategory(Definition).cat) {
    case SortCategory::Map:
    case SortCategory::List:
    case SortCategory::Set: {
      if (!arg->getType()->isPointerTy()) {
        llvm::AllocaInst *AllocCollection = new llvm::AllocaInst(arg->getType(), 0, "", CurrentBlock);
        new llvm::StoreInst(arg, AllocCollection, CurrentBlock);
        args.push_back(AllocCollection);
      } else {
        args.push_back(arg);
      }
      break;
    }
    default:
      args.push_back(arg);
      break;
    }
  }
  return createFunctionCall(name, returnCat, args, sret, fastcc);
}

llvm::Value *CreateTerm::createFunctionCall(std::string name, ValueType returnCat, std::vector<llvm::Value *> &args, bool sret, bool fastcc) {
  llvm::Type *returnType = getValueType(returnCat, Module);
  std::vector<llvm::Type *> types;
  bool collection = false;
  switch (returnCat.cat) {
  case SortCategory::Map:
  case SortCategory::List:
  case SortCategory::Set:
    collection = true;
    break;
  default:
    sret = false; 
    break;
  }
  llvm::Value *AllocSret;
  for (int i = 0; i < args.size(); i++) {
    llvm::Value *arg = args[i];
    types.push_back(arg->getType());
  }
  if (sret) {
    // we don't use alloca here because the tail call optimization pass for llvm doesn't handle correctly functions with alloca
    AllocSret = allocateTerm(returnType, CurrentBlock, "koreAllocAlwaysGC");
    args.insert(args.begin(), AllocSret);
    types.insert(types.begin(), AllocSret->getType());
    returnType = llvm::Type::getVoidTy(Ctx);
  } else if (collection) {
    returnType = llvm::PointerType::getUnqual(returnType);
  }
 
  llvm::FunctionType *funcType = llvm::FunctionType::get(returnType, types, false);
  llvm::Constant *constant = Module->getOrInsertFunction(name, funcType);
  llvm::Function *func = llvm::dyn_cast<llvm::Function>(constant);
  if (!func) {
    constant->print(llvm::errs());
    abort();
  }
  auto call = llvm::CallInst::Create(func, args, "", CurrentBlock);
  setDebugLoc(call);
  if (fastcc) {
    call->setCallingConv(llvm::CallingConv::Fast);
  }
  if (sret) {
    func->arg_begin()->addAttr(llvm::Attribute::StructRet);
    call->addParamAttr(0, llvm::Attribute::StructRet);
    return AllocSret;
  }
  return call;
}

/* create a term, given the assumption that the created term will not be a triangle injection pair */
llvm::Value *CreateTerm::notInjectionCase(KORECompositePattern *constructor, llvm::Value *val) {
  const KORESymbol *symbol = constructor->getConstructor();
  KORESymbolDeclaration *symbolDecl = Definition->getSymbolDeclarations().at(symbol->getName());
  llvm::StructType *BlockType = getBlockType(Module, Definition, symbol);
  llvm::Value *BlockHeader = getBlockHeader(Module, Definition, symbol, BlockType);
  llvm::Value *Block = allocateTerm(BlockType, CurrentBlock);
  llvm::Value *BlockHeaderPtr = llvm::GetElementPtrInst::CreateInBounds(BlockType, Block, {llvm::ConstantInt::get(llvm::Type::getInt64Ty(Ctx), 0), llvm::ConstantInt::get(llvm::Type::getInt32Ty(Ctx), 0)}, symbol->getName(), CurrentBlock);
  new llvm::StoreInst(BlockHeader, BlockHeaderPtr, CurrentBlock);
  int idx = 2;
  for (auto &child : constructor->getArguments()) {
    llvm::Value *ChildValue;
    if (idx == 2 && val != nullptr) {
      ChildValue = val;
    } else {
      ChildValue = (*this)(child.get()).first;
    }
    llvm::Value *ChildPtr = llvm::GetElementPtrInst::CreateInBounds(BlockType, Block, {llvm::ConstantInt::get(llvm::Type::getInt64Ty(Ctx), 0), llvm::ConstantInt::get(llvm::Type::getInt32Ty(Ctx), idx++)}, "", CurrentBlock);
    if (ChildValue->getType() == ChildPtr->getType()) {
      ChildValue = new llvm::LoadInst(ChildValue, "", CurrentBlock);
    }
    new llvm::StoreInst(ChildValue, ChildPtr, CurrentBlock);
  }
  auto BlockPtr = llvm::PointerType::getUnqual(Module->getTypeByName(BLOCK_STRUCT));
  auto bitcast = new llvm::BitCastInst(Block, BlockPtr, "", CurrentBlock);
  if (symbolDecl->getAttributes().count("binder")) {
    auto call = llvm::CallInst::Create(Module->getOrInsertFunction("debruijnize", BlockPtr, BlockPtr), bitcast, "withIndices", CurrentBlock);
    setDebugLoc(call);
    return call; 
  } else {
    return bitcast;
  }
}

// returns a value and a boolean indicating whether that value could be an injection
std::pair<llvm::Value *, bool> CreateTerm::operator()(KOREPattern *pattern) {
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
    if (symbol->getName() == "\\dv") {
      auto sort = dynamic_cast<KORECompositeSort *>(symbol->getFormalArguments()[0].get());
      auto strPattern = dynamic_cast<KOREStringPattern *>(constructor->getArguments()[0].get());
      return std::make_pair(createToken(sort->getCategory(Definition), strPattern->getContents()), false);
    }
    KORESymbolDeclaration *symbolDecl = Definition->getSymbolDeclarations().at(symbol->getName());
    if (symbolDecl->getAttributes().count("function") || (symbolDecl->getAttributes().count("anywhere") && !isAnywhereOwise)) {
      if (symbolDecl->getAttributes().count("hook")) {
        return std::make_pair(createHook(symbolDecl->getAttributes().at("hook").get(), constructor), true);
      } else {
        std::ostringstream Out;
        symbol->print(Out, 0, false);
        return std::make_pair(createFunctionCall("eval_" + Out.str(), constructor, false, true), true);
      }
    } else if (symbol->getArguments().empty()) {
      llvm::StructType *BlockType = Module->getTypeByName(BLOCK_STRUCT);
      llvm::IntToPtrInst *Cast = new llvm::IntToPtrInst(llvm::ConstantInt::get(llvm::Type::getInt64Ty(Ctx), (((uint64_t)symbol->getTag()) << 32) | 1), llvm::PointerType::getUnqual(BlockType), "", CurrentBlock);
      return std::make_pair(Cast,false);
    } else if (symbolDecl->getAttributes().count("sortInjection")
        && dynamic_cast<KORECompositeSort *>(symbol->getArguments()[0].get())->getCategory(Definition).cat == SortCategory::Symbol) {
      std::pair<llvm::Value *, bool> val = (*this)(constructor->getArguments()[0].get());
      if (val.second) {
        llvm::Instruction *Tag = llvm::CallInst::Create(Module->getOrInsertFunction("getTag", llvm::Type::getInt32Ty(Ctx), getValueType({SortCategory::Symbol, 0}, Module)), val.first, "tag", CurrentBlock);
        setDebugLoc(Tag);
        auto inj = Definition->getInjSymbol();
        auto NotStringBlock = llvm::BasicBlock::Create(Ctx, "notString", CurrentBlock->getParent());
        auto GeBlock = llvm::BasicBlock::Create(Ctx, "geFirst", CurrentBlock->getParent());
        auto FalseBlock = llvm::BasicBlock::Create(Ctx, "notInjection", CurrentBlock->getParent());
        auto TrueBlock = llvm::BasicBlock::Create(Ctx, "merge", CurrentBlock->getParent());
        auto cmp = new llvm::ICmpInst(*CurrentBlock, llvm::CmpInst::ICMP_NE, Tag,
            llvm::ConstantInt::getSigned(llvm::Type::getInt32Ty(Ctx), -1));
        llvm::BranchInst::Create(NotStringBlock, FalseBlock, cmp, CurrentBlock);

        CurrentBlock = NotStringBlock;
        cmp = new llvm::ICmpInst(*CurrentBlock, llvm::CmpInst::ICMP_UGE, Tag,
            llvm::ConstantInt::get(llvm::Type::getInt32Ty(Ctx), inj->getFirstTag()));
        llvm::BranchInst::Create(GeBlock, FalseBlock, cmp, CurrentBlock);
  
        CurrentBlock = GeBlock;
        cmp = new llvm::ICmpInst(*CurrentBlock, llvm::CmpInst::ICMP_ULE, Tag,
            llvm::ConstantInt::get(llvm::Type::getInt32Ty(Ctx), inj->getLastTag()));
        llvm::BranchInst::Create(TrueBlock, FalseBlock, cmp, CurrentBlock);
   
        CurrentBlock = FalseBlock;
        auto Cast = notInjectionCase(constructor, val.first);
        llvm::BranchInst::Create(TrueBlock, CurrentBlock);
  
        CurrentBlock = TrueBlock;
        llvm::PHINode *Phi = llvm::PHINode::Create(Cast->getType(), 2, "phi", CurrentBlock);
        Phi->addIncoming(Cast, FalseBlock);
        Phi->addIncoming(val.first, GeBlock);
        return std::make_pair(Phi, true);
      } else {
        return std::make_pair(notInjectionCase(constructor, val.first), true);
      }
    } else {
     return std::make_pair(notInjectionCase(constructor, nullptr), false);
    }
  } else {
    assert(false && "not supported yet: meta level");
    abort();
  }
}

void addAbort(llvm::BasicBlock *block, llvm::Module *Module) {
    llvm::FunctionType *AbortType = llvm::FunctionType::get(llvm::Type::getVoidTy(Module->getContext()), false);
    llvm::Function *AbortFunc = llvm::dyn_cast<llvm::Function>(Module->getOrInsertFunction("abort", AbortType));
    AbortFunc->addFnAttr(llvm::Attribute::NoReturn);
    llvm::CallInst::Create(AbortFunc, "", block);
    new llvm::UnreachableInst(Module->getContext(), block);
}

bool makeFunction(std::string name, KOREPattern *pattern, KOREDefinition *definition, llvm::Module *Module, bool fastcc, bool bigStep, KOREAxiomDeclaration *axiom, std::string postfix) {
    std::map<std::string, KOREVariablePattern *> vars;
    pattern->markVariables(vars);
    llvm::StringMap<ValueType> params;
    std::vector<llvm::Type *> paramTypes;
    std::vector<std::string> paramNames;
    std::vector<llvm::Metadata *> debugArgs;
    for (auto iter = vars.begin(); iter != vars.end(); ++iter) {
      auto &entry = *iter;
      auto sort = dynamic_cast<KORECompositeSort *>(entry.second->getSort().get());
      if (!sort) {
        // TODO: sort variables
        return false;
      }
      auto cat = sort->getCategory(definition);
      llvm::Type *paramType = getValueType(cat, Module);
      debugArgs.push_back(getDebugType(cat));
      switch(cat.cat) {
      case SortCategory::Map:
      case SortCategory::List:
      case SortCategory::Set:
        paramType = llvm::PointerType::getUnqual(paramType);
        break;
      default:
        break;
      }
      
      params.insert({entry.first, cat});
      paramTypes.push_back(paramType);
      paramNames.push_back(entry.first);
    }
    ValueType returnCat = termType(pattern, params, definition);
    auto returnType = getValueType(returnCat, Module);
    switch(returnCat.cat) {
    case SortCategory::Map:
    case SortCategory::List:
    case SortCategory::Set:
      returnType = llvm::PointerType::getUnqual(returnType);
      break;
    default:
      break;
    }
    llvm::FunctionType *funcType = llvm::FunctionType::get(returnType, paramTypes, false);
    llvm::Constant *func = Module->getOrInsertFunction(name, funcType);
    llvm::Function *applyRule = llvm::dyn_cast<llvm::Function>(func);
    initDebugAxiom(axiom->getAttributes());
    std::string debugName = name;
    if (axiom->getAttributes().count("label")) {
      KORECompositePattern *labelAtt = axiom->getAttributes().at("label").get();
      assert(labelAtt->getArguments().size() == 1);
      auto strPattern = dynamic_cast<KOREStringPattern *>(labelAtt->getArguments()[0].get());
      std::string label = strPattern->getContents();
      debugName = label + postfix;
    }
    initDebugFunction(debugName, debugName, getDebugFunctionType(getDebugType(returnCat), debugArgs), definition, applyRule);
    if (!applyRule) {
      func->print(llvm::errs());
      abort();
    }
    if (fastcc) {
      applyRule->setCallingConv(llvm::CallingConv::Fast);
    }
    llvm::StringMap<llvm::Value *> subst;
    llvm::BasicBlock *block = llvm::BasicBlock::Create(Module->getContext(), "entry", applyRule);
    int i = 0;
    for (auto val = applyRule->arg_begin(); val != applyRule->arg_end(); ++val, ++i) {
      subst.insert({paramNames[i], val});
      initDebugParam(applyRule, i, paramNames[i], params[paramNames[i]]);
    }
    CreateTerm creator = CreateTerm(subst, definition, block, Module, false);
    llvm::Value *retval = creator(pattern).first;
    if (funcType->getReturnType() == llvm::PointerType::getUnqual(retval->getType())) {
      auto tempAlloc = allocateTerm(retval->getType(), creator.getCurrentBlock(), "koreAllocAlwaysGC");
      new llvm::StoreInst(retval, tempAlloc, creator.getCurrentBlock());
      retval = tempAlloc;
    }
    if (bigStep) {
      llvm::Type *blockType = getValueType({SortCategory::Symbol, 0}, Module);
      llvm::Constant *step = Module->getOrInsertFunction("step", llvm::FunctionType::get(blockType, {blockType}, false));
      auto call = llvm::CallInst::Create(step, {retval}, "", creator.getCurrentBlock());
      setDebugLoc(call);
      call->setCallingConv(llvm::CallingConv::Fast);
      retval = call;
    }
    llvm::ReturnInst::Create(Module->getContext(), retval, creator.getCurrentBlock());
    return true;
}

std::string makeApplyRuleFunction(KOREAxiomDeclaration *axiom, KOREDefinition *definition, llvm::Module *Module, bool bigStep) {
    KOREPattern *pattern = axiom->getRightHandSide();
    std::string name = "apply_rule_" + std::to_string(axiom->getOrdinal());
    if (makeFunction(name, pattern, definition, Module, true, bigStep, axiom, ".rhs")) {
      return name;
    }
    return "";
}

std::string makeApplyRuleFunction(KOREAxiomDeclaration *axiom, KOREDefinition *definition, llvm::Module *Module, std::vector<Residual> residuals) {
    std::map<std::string, KOREVariablePattern *> vars;
    for (auto residual : residuals) {
      residual.pattern->markVariables(vars);
    }
    llvm::StringMap<ValueType> params;
    std::vector<llvm::Type *> paramTypes;
    std::vector<std::string> paramNames;
    std::vector<llvm::Metadata *> debugArgs;
    for (auto iter = vars.begin(); iter != vars.end(); ++iter) {
      auto &entry = *iter;
      auto sort = dynamic_cast<KORECompositeSort *>(entry.second->getSort().get());
      if (!sort) {
        // TODO: sort variables
        return "";
      }
      auto cat = sort->getCategory(definition);
      llvm::Type *paramType = getValueType(cat, Module);
      debugArgs.push_back(getDebugType(cat));
      switch(cat.cat) {
      case SortCategory::Map:
      case SortCategory::List:
      case SortCategory::Set:
        paramType = llvm::PointerType::getUnqual(paramType);
        break;
      default:
        break;
      }

      params.insert({entry.first, cat});
      paramTypes.push_back(paramType);
      paramNames.push_back(entry.first);
    }
    llvm::FunctionType *funcType = llvm::FunctionType::get(getValueType({SortCategory::Symbol, 0}, Module), paramTypes, false);
    std::string name = "apply_rule_" + std::to_string(axiom->getOrdinal());
    llvm::Constant *func = Module->getOrInsertFunction(name, funcType);
    llvm::Function *applyRule = llvm::dyn_cast<llvm::Function>(func);
    initDebugAxiom(axiom->getAttributes());
    initDebugFunction(name, name, getDebugFunctionType(getDebugType({SortCategory::Symbol, 0}), debugArgs), definition, applyRule);
    if (!applyRule) {
      printf("%lu\n", residuals.size());
      func->print(llvm::errs());
      abort();
    }
    applyRule->setCallingConv(llvm::CallingConv::Fast);
    llvm::StringMap<llvm::Value *> subst;
    llvm::BasicBlock *block = llvm::BasicBlock::Create(Module->getContext(), "entry", applyRule);
    int i = 0;
    for (auto val = applyRule->arg_begin(); val != applyRule->arg_end(); ++val, ++i) {
      subst.insert({paramNames[i], val});
      initDebugParam(applyRule, i, paramNames[i], params[paramNames[i]]);
    }
    CreateTerm creator = CreateTerm(subst, definition, block, Module, false);
    std::vector<llvm::Value *> args;
    std::vector<llvm::Type *> types;
    for (auto residual : residuals) {
      llvm::Value *arg = creator(residual.pattern).first;
      auto sort = dynamic_cast<KORECompositeSort *>(residual.pattern->getSort().get());
      auto cat = sort->getCategory(definition);
      switch (cat.cat) {
      case SortCategory::Map:
      case SortCategory::List:
      case SortCategory::Set:
        if (!arg->getType()->isPointerTy()) {
          auto ptr = allocateTerm(arg->getType(), creator.getCurrentBlock(), "koreAllocAlwaysGC");
          new llvm::StoreInst(arg, ptr, creator.getCurrentBlock());
          arg = ptr;
        }
        break;
      default:
        break;
      }
      args.push_back(arg);
      types.push_back(arg->getType());
    }
    llvm::Type *blockType = getValueType({SortCategory::Symbol, 0}, Module);
    llvm::Constant *step = Module->getOrInsertFunction("step_" + std::to_string(axiom->getOrdinal()), llvm::FunctionType::get(blockType, types, false));
    auto retval = llvm::CallInst::Create(step, args, "", creator.getCurrentBlock());
    setDebugLoc(retval);
    retval->setCallingConv(llvm::CallingConv::Fast);
    llvm::ReturnInst::Create(Module->getContext(), retval, creator.getCurrentBlock());
    return name;
}

std::string makeSideConditionFunction(KOREAxiomDeclaration *axiom, KOREDefinition *definition, llvm::Module *Module) {
    KOREPattern *pattern = axiom->getRequires();
    if (!pattern) {
      return "";
    }
    std::string name = "side_condition_" + std::to_string(axiom->getOrdinal());
    if (makeFunction(name, pattern, definition, Module, false, false, axiom, ".sc")) {
      return name;
    }
    return "";
}


}
