#include "kllvm/codegen/CreateTerm.h"

#include "llvm/IRReader/IRReader.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Support/SourceMgr.h"

namespace kllvm {

std::string LLVM_HEADER = R"LLVM(
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
  return llvm::parseIR(*llvm::MemoryBuffer::getMemBuffer(LLVM_HEADER), Err, Context);
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
  headerVal |= (llvm::DataLayout(Module).getTypeAllocSize(BlockType) / 8) << 32;
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
    const KOREObjectSymbol *symbol = constructor->getConstructor();
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

llvm::Value *createTerm(KOREPattern *pattern, llvm::StringMap<llvm::Value *> &substitution, KOREDefinition *definition, llvm::BasicBlock *block, llvm::Module *Module) {
  if (auto variable = dynamic_cast<KOREObjectVariablePattern *>(pattern)) {
    return Substitution.lookup(variable->getName());
  } else if (auto constructor = dynamic_cast<KOREObjectCompositePattern *>(pattern)) {
    const KOREObjectSymbol *symbol = constructor->getConstructor();
    assert(symbol->isConcrete() && "not supported yet: sort variables");
    assert(symbol->getName() != "\\dv" && "not supported yet: \\dv");
    KOREObjectSymbolDeclaration *symbolDecl = Definition->getSymbolDeclarations().lookup(symbol->getName());
    if (symbolDecl->getAttributes().count("function")) {
      assert(false && "not implemented yet: functions");
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
