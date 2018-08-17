#include "kllvm/codegen/CreateTerm.h"

#include <gmp.h>

#include "llvm/IRReader/IRReader.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Support/SourceMgr.h"
#include "llvm/Support/raw_ostream.h"

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
; float: %mpfr *
; string: %string *
; bytes: %string *
; string buffer: %string **
; boolean: i1
; machine integer of N bits: iN

; We also define the following LLVM structure types:

%string = type { %blockheader, [0 x i8] } ; 16-bit layout, 48-bit length, bytes
%map = type { i64, i8 *, i8 * } ; im::hashmap::HashMap
%set = type { i8 *, i8 *, i64 } ; im::hashset::HashSet
%list = type { i64, i64, i8 *, i8 *, i8 *, i8 *, i8 * } ; im::vector::Vector
%mpz = type { i32, i32, i64 * } ; mpz_t
%mpfr = type { i64, i32, i64, i64 * } ; mpfr_t
%blockheader = type { i64 } 
%block = type { %blockheader, [0 x i64 *] } ; 16-bit layout, 8-bit length, 32-bit tag, children

; The layout of a block uniquely identifies the categories of its children as
; well as how to allocate/deallocate them and whether to follow their pointers
; during gc. Roughly speaking, the following rules apply:

; %string *: malloc/free, do not follow
; iN: noop, do not follow
; %map, %set, %list: noop/drop_in_place, follow
; %block *: managed heap, follow
; %mpz *: malloc/mpz_clear->free, do not follow
; %mpfr *: malloc/mpfr_clear->free, do not follow
; %string **: malloc->malloc/free->free, do not follow

; We also automatically generate for each unique layout id a struct type
; corresponding to the actual layout of that block. For example, if we have
; the symbol symbol foo{Map{}, Int{}, Exp{}} : Exp{}, we would generate the type:

; %layoutN = type { %blockheader, [0 x i64 *], %map, %mpz *, %block * }
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
static std::string INT_STRUCT = "mpz";
static std::string FLOAT_STRUCT = "mpfr";
static std::string STRING_STRUCT = "string";
static std::string BLOCK_STRUCT = "block";
static std::string BLOCKHEADER_STRUCT = "blockheader";

llvm::Type *getValueType(SortCategory sort, llvm::Module *Module) {
  switch(sort) {
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
    return llvm::PointerType::getUnqual(llvm::PointerType::getUnqual(Module->getTypeByName(STRING_STRUCT)));
  case SortCategory::Bool:
    return llvm::Type::getInt1Ty(Module->getContext());
  case SortCategory::MInt:
    assert(false && "not implemented yet: MInt");
  case SortCategory::Symbol:
    return llvm::PointerType::getUnqual(Module->getTypeByName(BLOCK_STRUCT));
  }
}


llvm::StructType *getBlockType(llvm::Module *Module, KOREDefinition *definition, const KOREObjectSymbol *symbol, KOREObjectSymbolDeclaration *symbolDecl) {
  llvm::StructType *BlockHeaderType = Module->getTypeByName(BLOCKHEADER_STRUCT);
  llvm::ArrayType *EmptyArrayType = llvm::ArrayType::get(llvm::Type::getInt64Ty(Module->getContext()), 0);
  llvm::SmallVector<llvm::Type *, 4> Types;
  Types.push_back(BlockHeaderType);
  Types.push_back(EmptyArrayType);
  for (KOREObjectSort *arg : symbol->getArguments()) {
    auto sort = dynamic_cast<KOREObjectCompositeSort *>(arg);
    llvm::Type *type = getValueType(sort->getCategory(definition), Module);
    Types.push_back(type);
  }
  return llvm::StructType::get(Module->getContext(), Types);
}

llvm::Value *getBlockHeader(llvm::Module *Module, KOREDefinition *definition, const KOREObjectSymbol *symbol, llvm::Type *BlockType) {
  llvm::StructType *BlockHeaderType = Module->getTypeByName(BLOCKHEADER_STRUCT);
  uint64_t headerVal = symbol->getTag();
  uint64_t sizeInBytes = llvm::DataLayout(Module).getTypeAllocSize(BlockType);
  assert(sizeInBytes % 8 == 0);
  headerVal |= (sizeInBytes / 8) << 32;
  headerVal |= (uint64_t)symbol->getLayout() << 48;
  return llvm::ConstantStruct::get(BlockHeaderType, llvm::ConstantInt::get(llvm::Type::getInt64Ty(Module->getContext()), headerVal));
}

llvm::Value *allocateBlock(llvm::Type *AllocType, llvm::BasicBlock *block) {
  llvm::Instruction *Malloc = llvm::CallInst::CreateMalloc(block, llvm::Type::getInt64Ty(block->getContext()), AllocType, llvm::ConstantExpr::getSizeOf(AllocType), nullptr, nullptr);
  block->getInstList().push_back(Malloc);
  return Malloc;
}

llvm::Type *termType(KOREPattern *pattern, llvm::StringMap<llvm::Type *> &substitution, KOREDefinition *definition, llvm::Module *Module) {
  if (auto variable = dynamic_cast<KOREObjectVariablePattern *>(pattern)) {
    return substitution.lookup(variable->getName());
  } else if (auto constructor = dynamic_cast<KOREObjectCompositePattern *>(pattern)) {
    KOREObjectSymbol *symbol = constructor->getConstructor();
    assert(symbol->isConcrete() && "not supported yet: sort variables");
    if (symbol->getName() == "\\dv") {
      auto sort = dynamic_cast<KOREObjectCompositeSort *>(symbol->getArguments()[0]);
      return getValueType(sort->getCategory(definition), Module);
    }
    auto sort = dynamic_cast<KOREObjectCompositeSort *>(symbol->getSort());
    return getValueType(sort->getCategory(definition), Module);
  } else {
    assert(false && "not supported yet: meta level");
  }
}

llvm::Value *CreateTerm::createToken(SortCategory sort, std::string contents) {
  switch(sort) {
  case SortCategory::Map:
  case SortCategory::List:
  case SortCategory::Set:
    assert (false && "cannot create tokens of collection category");
  case SortCategory::Int: {
    llvm::Constant *global = Module->getOrInsertGlobal("int_" + contents, Module->getTypeByName(INT_STRUCT));
    llvm::GlobalVariable *globalVar = llvm::dyn_cast<llvm::GlobalVariable>(global);
    if (!globalVar->hasInitializer()) {
      mpz_t value;
      mpz_init_set_str(value, contents.c_str(), 10);
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
      llvm::ConstantInt *numLimbs = llvm::ConstantInt::get(llvm::Type::getInt32Ty(Ctx), size);
      llvm::Constant *mp_size = llvm::ConstantExpr::getMul(numLimbs, llvm::ConstantInt::getSigned(llvm::Type::getInt32Ty(Ctx), sign));
      globalVar->setInitializer(llvm::ConstantStruct::get(Module->getTypeByName(INT_STRUCT), {numLimbs, mp_size, llvm::ConstantExpr::getPointerCast(limbsVar, llvm::Type::getInt64PtrTy(Ctx))}));
      mpz_clear(value);
    }
    return global;
  }
  case SortCategory::Float:
  case SortCategory::StringBuffer:
  case SortCategory::MInt:
    assert(false && "not implemented yet: tokens");
  case SortCategory::Bool:
    return llvm::ConstantInt::get(llvm::Type::getInt1Ty(Ctx), contents == "true");
  case SortCategory::Symbol: {
    llvm::StructType *StringType = llvm::StructType::get(Ctx, {Module->getTypeByName(BLOCKHEADER_STRUCT), llvm::ArrayType::get(llvm::Type::getInt8Ty(Ctx), contents.size())});
    llvm::Constant *global = Module->getOrInsertGlobal("token_" + contents, StringType);
    llvm::GlobalVariable *globalVar = llvm::dyn_cast<llvm::GlobalVariable>(global);
    if (!globalVar->hasInitializer()) {
      llvm::StructType *BlockHeaderType = Module->getTypeByName(BLOCKHEADER_STRUCT);
      llvm::Constant *BlockHeader = llvm::ConstantStruct::get(BlockHeaderType, llvm::ConstantInt::get(llvm::Type::getInt64Ty(Ctx), contents.size()));
      globalVar->setInitializer(llvm::ConstantStruct::get(StringType, BlockHeader, llvm::ConstantDataArray::getString(Ctx, contents, false)));
    }
    return llvm::ConstantExpr::getPointerCast(global, llvm::PointerType::getUnqual(Module->getTypeByName(BLOCK_STRUCT)));
  }
  }
}

llvm::Value *CreateTerm::createHook(KOREObjectCompositePattern *hookAtt, KOREObjectCompositePattern *pattern) {
  assert(hookAtt->getArguments().size() == 1);
  auto strPattern = dynamic_cast<KOREMetaStringPattern *>(hookAtt->getArguments()[0]);
  std::string name = strPattern->getContents();
  if (name == "BOOL.and" || name == "BOOL.andThen") {
    assert(pattern->getArguments().size() == 2);
    llvm::Value *firstArg = (*this)(pattern->getArguments()[0]);
    llvm::BasicBlock *CondBlock = CurrentBlock;
    llvm::BasicBlock *TrueBlock = llvm::BasicBlock::Create(Ctx, "then", CurrentBlock->getParent());
    llvm::BasicBlock *MergeBlock = llvm::BasicBlock::Create(Ctx, "hook_BOOL_and", CurrentBlock->getParent());
    llvm::BranchInst *Branch = llvm::BranchInst::Create(TrueBlock, MergeBlock, firstArg, CurrentBlock);
    CurrentBlock = TrueBlock;
    llvm::Value *secondArg = (*this)(pattern->getArguments()[1]);
    Branch = llvm::BranchInst::Create(MergeBlock, CurrentBlock);
    llvm::PHINode *Phi = llvm::PHINode::Create(llvm::Type::getInt1Ty(Ctx), 2, "phi", MergeBlock);
    Phi->addIncoming(secondArg, CurrentBlock);
    Phi->addIncoming(firstArg, CondBlock);
    CurrentBlock = MergeBlock;
    return Phi;
  } else if (name == "BOOL.or" || name == "BOOL.orElse") {
    assert(pattern->getArguments().size() == 2);
    llvm::Value *firstArg = (*this)(pattern->getArguments()[0]);
    llvm::BasicBlock *CondBlock = CurrentBlock;
    llvm::BasicBlock *FalseBlock = llvm::BasicBlock::Create(Ctx, "else", CurrentBlock->getParent());
    llvm::BasicBlock *MergeBlock = llvm::BasicBlock::Create(Ctx, "hook_BOOL_or", CurrentBlock->getParent());
    llvm::BranchInst *Branch = llvm::BranchInst::Create(MergeBlock, FalseBlock, firstArg, CurrentBlock);
    CurrentBlock = FalseBlock;
    llvm::Value *secondArg = (*this)(pattern->getArguments()[1]);
    Branch = llvm::BranchInst::Create(MergeBlock, CurrentBlock);
    llvm::PHINode *Phi = llvm::PHINode::Create(llvm::Type::getInt1Ty(Ctx), 2, "phi", MergeBlock);
    Phi->addIncoming(secondArg, CurrentBlock);
    Phi->addIncoming(firstArg, CondBlock);
    CurrentBlock = MergeBlock;
    return Phi;
  } else if (name == "BOOL.not") {
    assert(pattern->getArguments().size() == 1);
    llvm::Value *arg = (*this)(pattern->getArguments()[0]);
    llvm::BinaryOperator *Not = llvm::BinaryOperator::Create(llvm::Instruction::Xor, arg, llvm::ConstantInt::get(llvm::Type::getInt1Ty(Ctx), 1), "hook_BOOL_not", CurrentBlock);
    return Not;
  } else if (name == "BOOL.implies") {
    assert(pattern->getArguments().size() == 2);
    llvm::Value *firstArg = (*this)(pattern->getArguments()[0]);
    llvm::BasicBlock *CondBlock = CurrentBlock;
    llvm::BasicBlock *TrueBlock = llvm::BasicBlock::Create(Ctx, "then", CurrentBlock->getParent());
    llvm::BasicBlock *MergeBlock = llvm::BasicBlock::Create(Ctx, "hook_BOOL_implies", CurrentBlock->getParent());
    llvm::BranchInst *Branch = llvm::BranchInst::Create(TrueBlock, MergeBlock, firstArg, CurrentBlock);
    CurrentBlock = TrueBlock;
    llvm::Value *secondArg = (*this)(pattern->getArguments()[1]);
    Branch = llvm::BranchInst::Create(MergeBlock, CurrentBlock);
    llvm::PHINode *Phi = llvm::PHINode::Create(llvm::Type::getInt1Ty(Ctx), 2, "phi", MergeBlock);
    Phi->addIncoming(secondArg, CurrentBlock);
    Phi->addIncoming(llvm::ConstantInt::get(llvm::Type::getInt1Ty(Ctx), 1), CondBlock);
    CurrentBlock = MergeBlock;
    return Phi;
  } else if (name == "BOOL.ne" || name == "BOOL.xor") {
    assert(pattern->getArguments().size() == 2);
    llvm::Value *firstArg = (*this)(pattern->getArguments()[0]);
    llvm::Value *secondArg = (*this)(pattern->getArguments()[1]);
    llvm::BinaryOperator *Xor = llvm::BinaryOperator::Create(llvm::Instruction::Xor, firstArg, secondArg, "hook_BOOL_ne", CurrentBlock);
    return Xor;
  } else if (name == "BOOL.eq") {
    assert(pattern->getArguments().size() == 2);
    llvm::Value *firstArg = (*this)(pattern->getArguments()[0]);
    llvm::Value *secondArg = (*this)(pattern->getArguments()[1]);
    llvm::ICmpInst *Eq = new llvm::ICmpInst(*CurrentBlock, llvm::CmpInst::ICMP_EQ, firstArg, secondArg, "hook_BOOL_eq");
    return Eq;
  } else if (!name.compare(0, 5, "MINT.")) {
    assert(false && "not implemented yet: MInt");
  } else {
    std::string hookName = "hook_" + name.substr(0, name.find('.')) + "_" + name.substr(name.find('.') + 1);
    return createFunctionCall(hookName, pattern);
  }
}

llvm::Value *CreateTerm::createFunctionCall(std::string name, KOREObjectCompositePattern *pattern) {
  std::vector<llvm::Value *> args;
  std::vector<llvm::Type *> types;
  auto returnSort = dynamic_cast<KOREObjectCompositeSort *>(pattern->getConstructor()->getSort());
  llvm::Type *returnType = getValueType(returnSort->getCategory(Definition), Module);
  llvm::Value *Retval;
  bool load = true;
  switch(returnSort->getCategory(Definition)) {
  case SortCategory::Int:
    Retval = allocateBlock(Module->getTypeByName(INT_STRUCT), CurrentBlock);
    load = false;
    break;
  case SortCategory::Float:
    Retval = allocateBlock(Module->getTypeByName(FLOAT_STRUCT), CurrentBlock);
    load = false;
    break;
  default:
    Retval = new llvm::AllocaInst(returnType, 0, "", CurrentBlock);
    break;
  }
  args.push_back(Retval);
  types.push_back(Retval->getType());
  int i = 0;
  for (auto sort : pattern->getConstructor()->getArguments()) {
    auto concreteSort = dynamic_cast<KOREObjectCompositeSort *>(sort);
    llvm::Value *arg = (*this)(pattern->getArguments()[i++]);
    switch(concreteSort->getCategory(Definition)) {
    case SortCategory::Map:
    case SortCategory::List:
    case SortCategory::Set: {
      llvm::AllocaInst *AllocCollection = new llvm::AllocaInst(arg->getType(), 0, "", CurrentBlock);
      llvm::StoreInst *MoveCollection = new llvm::StoreInst(arg, AllocCollection, CurrentBlock);
      args.push_back(AllocCollection);
      types.push_back(AllocCollection->getType());
      break;
    }
    default:
      args.push_back(arg);
      types.push_back(arg->getType());
      break;
    }
  }
  canGetStuck = true;
  llvm::FunctionType *funcType = llvm::FunctionType::get(llvm::Type::getInt1Ty(Ctx), types, false);
  llvm::Constant *func = Module->getOrInsertFunction(name, funcType);
  llvm::CallInst *Call = llvm::CallInst::Create(func, args, "", CurrentBlock);
  llvm::BasicBlock *MergeBlock = llvm::BasicBlock::Create(Ctx, "notstuck", CurrentBlock->getParent());
  llvm::BranchInst *Branch = llvm::BranchInst::Create(MergeBlock, StuckBlock, Call, CurrentBlock);
  CurrentBlock = MergeBlock;
  if (load) {
    return new llvm::LoadInst(Retval, "", CurrentBlock);
  } else {
    return Retval;
  }
}

llvm::Value *CreateTerm::operator()(KOREPattern *pattern) {
  if (auto variable = dynamic_cast<KOREObjectVariablePattern *>(pattern)) {
    return Substitution.lookup(variable->getName());
  } else if (auto constructor = dynamic_cast<KOREObjectCompositePattern *>(pattern)) {
    const KOREObjectSymbol *symbol = constructor->getConstructor();
    assert(symbol->isConcrete() && "not supported yet: sort variables");
    if (symbol->getName() == "\\dv") {
      auto sort = dynamic_cast<KOREObjectCompositeSort *>(symbol->getArguments()[0]);
      auto strPattern = dynamic_cast<KOREMetaStringPattern *>(constructor->getArguments()[0]);
      return createToken(sort->getCategory(Definition), strPattern->getContents());
    }
    KOREObjectSymbolDeclaration *symbolDecl = Definition->getSymbolDeclarations().lookup(symbol->getName());
    if (symbolDecl->getAttributes().count("function")) {
      if (symbolDecl->getAttributes().count("hook")) {
        return createHook(symbolDecl->getAttributes().lookup("hook"), constructor);
      } else {
        std::ostringstream Out;
        symbol->print(Out);
        return createFunctionCall("eval_" + Out.str(), constructor);
      }
    } else if (symbol->getArguments().empty()) {
      llvm::StructType *BlockType = Module->getTypeByName(BLOCK_STRUCT);
      llvm::IntToPtrInst *Cast = new llvm::IntToPtrInst(llvm::ConstantInt::get(llvm::Type::getInt64Ty(Ctx), (((uint64_t)symbol->getTag()) << 32) | 1), llvm::PointerType::getUnqual(BlockType), "", CurrentBlock);
      return Cast;
    } else {
      llvm::StructType *BlockType = getBlockType(Module, Definition, symbol, symbolDecl);
      llvm::Value *BlockHeader = getBlockHeader(Module, Definition, symbol, BlockType);
      llvm::Value *Block = allocateBlock(BlockType, CurrentBlock);
      llvm::Value *BlockHeaderPtr = llvm::GetElementPtrInst::CreateInBounds(BlockType, Block, {llvm::ConstantInt::get(llvm::Type::getInt64Ty(Ctx), 0), llvm::ConstantInt::get(llvm::Type::getInt32Ty(Ctx), 0)}, symbol->getName(), CurrentBlock);
      llvm::StoreInst *StoreBlockHeader = new llvm::StoreInst(BlockHeader, BlockHeaderPtr, CurrentBlock);
      int idx = 2;
      for (auto child : constructor->getArguments()) {
        llvm::Value *ChildValue = (*this)(child);
        llvm::Value *ChildPtr = llvm::GetElementPtrInst::CreateInBounds(BlockType, Block, {llvm::ConstantInt::get(llvm::Type::getInt64Ty(Ctx), 0), llvm::ConstantInt::get(llvm::Type::getInt32Ty(Ctx), idx++)}, "", CurrentBlock);
        llvm::StoreInst *StoreChild = new llvm::StoreInst(ChildValue, ChildPtr, CurrentBlock);
      }
      llvm::BitCastInst *Cast = new llvm::BitCastInst(Block, llvm::PointerType::getUnqual(Module->getTypeByName(BLOCK_STRUCT)), "", CurrentBlock);
      return Cast;
    }
  } else {
    assert(false && "not supported yet: meta level");
  }
}

static int nextRuleId = 0;

std::string makeApplyRuleFunction(KOREAxiomDeclaration *axiom, KOREDefinition *definition, llvm::Module *Module) {
    KOREPattern *pattern = axiom->getRightHandSide();
    llvm::StringMap<KOREObjectVariablePattern *> vars;
    pattern->markVariables(vars);
    llvm::StringMap<llvm::Type *> params;
    std::vector<llvm::Type *> paramTypes;
    std::vector<std::string> paramNames;
    for (auto iter = vars.begin(); iter != vars.end(); ++iter) {
      auto &entry = *iter;
      auto sort = dynamic_cast<KOREObjectCompositeSort *>(entry.second->getSort());
      if (!sort) {
        // TODO: sort variables
        return "";
      }
      llvm::Type *paramType = getValueType(sort->getCategory(definition), Module);
      params.insert({entry.first(), paramType});
      paramTypes.push_back(paramType);
      paramNames.push_back(entry.first());
    }
    llvm::FunctionType *funcType = llvm::FunctionType::get(termType(pattern, params, definition, Module), paramTypes, false);
    std::string name = "apply_rule_" + std::to_string(nextRuleId++);
    llvm::Constant *func = Module->getOrInsertFunction(name, funcType);
    llvm::Function *applyRule = llvm::cast<llvm::Function>(func);
    llvm::StringMap<llvm::Value *> subst;
    int i = 0;
    for (auto val = applyRule->arg_begin(); val != applyRule->arg_end(); ++val, ++i) {
      val->setName(paramNames[i]);
      subst.insert({paramNames[i], val});
    }
    llvm::BasicBlock *block = llvm::BasicBlock::Create(Module->getContext(), "entry", applyRule);
    llvm::BasicBlock *stuck = llvm::BasicBlock::Create(Module->getContext(), "stuck");
    llvm::FunctionType *AbortType = llvm::FunctionType::get(llvm::Type::getVoidTy(Module->getContext()), false);
    llvm::Function *AbortFunc = llvm::dyn_cast<llvm::Function>(Module->getOrInsertFunction("abort", AbortType));
    AbortFunc->addFnAttr(llvm::Attribute::NoReturn);
    llvm::CallInst *Abort = llvm::CallInst::Create(AbortFunc, "", stuck);
    llvm::UnreachableInst *Unreachable = new llvm::UnreachableInst(Module->getContext(), stuck);
    CreateTerm creator = CreateTerm(subst, definition, block, stuck, Module);
    llvm::Value *retval = creator(pattern);
    llvm::ReturnInst::Create(Module->getContext(), retval, creator.getCurrentBlock());
    if (creator.hasStuckBlock()) {
      stuck->insertInto(applyRule);
    }
    return name;
}

}
