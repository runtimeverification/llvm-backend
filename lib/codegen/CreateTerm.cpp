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

llvm::Type *getValueType(std::string sort, llvm::Module *Module) {
  if (sort == "Map") {
    return Module->getTypeByName("map");
  } else if (sort == "List") {
    return Module->getTypeByName("list");
  } else if (sort == "Set") {
    return Module->getTypeByName("set");
  } else if (sort == "Array") {
    return Module->getTypeByName("list");
  } else if (sort == "Int") {
    return llvm::PointerType::get(Module->getTypeByName("mpz"), 0);
  } else if (sort == "Float") {
    return llvm::PointerType::get(Module->getTypeByName("mpfr"), 0);
  } else if (sort == "StringBuffer") {
    return llvm::PointerType::get(llvm::PointerType::get(Module->getTypeByName("string"), 0), 0);
  } else if (sort == "Bool") {
    return llvm::IntegerType::get(Module->getContext(), 1);
  } else if (sort == "MInt") {
    assert(false && "not implemented yet: MInt");
  } else {
    return llvm::PointerType::get(Module->getTypeByName("block"), 0);
  }
}


llvm::StructType *getBlockType(llvm::Module *Module, const KOREObjectSymbol *symbol, KOREObjectSymbolDeclaration *symbolDecl) {
  llvm::StructType *BlockHeaderType = Module->getTypeByName("blockheader");
  llvm::ArrayType *EmptyArrayType = llvm::ArrayType::get(llvm::IntegerType::get(Module->getContext(), 64), 0);
  llvm::SmallVector<llvm::Type *, 4> Types;
  Types.push_back(BlockHeaderType);
  Types.push_back(EmptyArrayType);
  for (KOREObjectSort *arg : symbol->getArguments()) {
    auto sort = dynamic_cast<KOREObjectCompositeSort *>(arg);
    llvm::Type *type = getValueType(sort->getName(), Module);
    Types.push_back(type);
  }
  return llvm::StructType::get(Module->getContext(), Types);
}

llvm::Value *getBlockHeader(llvm::Module *Module, const KOREObjectSymbol *symbol) {
  llvm::StructType *BlockHeaderType = Module->getTypeByName("blockheader");
  uint64_t headerVal = symbol->getTag();
  headerVal |= symbol->length() << 32;
  headerVal |= symbol->getLayout() << 48;
  return llvm::ConstantStruct::get(BlockHeaderType, llvm::ConstantInt::get(llvm::IntegerType::get(Module->getContext(), 64), headerVal));
}

llvm::Value *allocateBlock(const KOREObjectSymbol *symbol, llvm::Type *AllocType, llvm::BasicBlock *block) {
  llvm::Instruction *Malloc = llvm::CallInst::CreateMalloc(block, llvm::IntegerType::get(block->getContext(), 64), AllocType, llvm::ConstantExpr::getSizeOf(AllocType), nullptr, nullptr);
  block->getInstList().push_back(Malloc);
  return Malloc;
}

llvm::Type *termType(KOREPattern *pattern, llvm::StringMap<llvm::Value *> &substitution, KOREDefinition *definition, llvm::LLVMContext &Context, llvm::Module *Module) {
  if (auto variable = dynamic_cast<KOREObjectVariablePattern *>(pattern)) {
    return substitution.lookup(variable->getName())->getType();
  } else if (auto constructor = dynamic_cast<KOREObjectCompositePattern *>(pattern)) {
    const KOREObjectSymbol *symbol = constructor->getConstructor();
    assert(symbol->isConcrete() && "not supported yet: sort variables");
    assert(symbol->getName() != "\\dv" && "not supported yet: \\dv");
    KOREObjectSymbolDeclaration *symbolDecl = definition->getSymbolDeclarations().lookup(symbol->getName());
    if (symbolDecl->getAttributes().count("function")) {
      assert(false && "not implemented yet: functions");
    }
    return llvm::PointerType::get(Module->getTypeByName("block"), 0);
  } else {
    assert(false && "not supported yet: meta level");
  }
}
  

llvm::Value *createTerm(KOREPattern *pattern, llvm::StringMap<llvm::Value *> &substitution, KOREDefinition *definition, llvm::BasicBlock *block, llvm::Module *Module) {
  if (auto variable = dynamic_cast<KOREObjectVariablePattern *>(pattern)) {
    return substitution.lookup(variable->getName());
  } else if (auto constructor = dynamic_cast<KOREObjectCompositePattern *>(pattern)) {
    const KOREObjectSymbol *symbol = constructor->getConstructor();
    assert(symbol->isConcrete() && "not supported yet: sort variables");
    assert(symbol->getName() != "\\dv" && "not supported yet: \\dv");
    KOREObjectSymbolDeclaration *symbolDecl = definition->getSymbolDeclarations().lookup(symbol->getName());
    llvm::LLVMContext &Context = block->getContext();
    if (symbolDecl->getAttributes().count("function")) {
      assert(false && "not implemented yet: functions");
    } else if (symbol->getArguments().empty()) {
      llvm::StructType *BlockType = Module->getTypeByName("block");
      llvm::IntToPtrInst *Cast = new llvm::IntToPtrInst(llvm::ConstantInt::get(llvm::IntegerType::get(Context, 32), symbol->getTag()), llvm::PointerType::get(BlockType, 0), "", block);
      return Cast;
    } else {
      llvm::StructType *BlockType = getBlockType(Module, symbol, symbolDecl);
      llvm::Value *BlockHeader = getBlockHeader(Module, symbol);
      llvm::Value *Block = allocateBlock(symbol, BlockType, block);
      llvm::Value *BlockHeaderPtr = llvm::GetElementPtrInst::CreateInBounds(BlockType, Block, {llvm::ConstantInt::get(llvm::IntegerType::get(Context, 64), 0), llvm::ConstantInt::get(llvm::IntegerType::get(Context, 32), 0)}, "", block);
      llvm::StoreInst *StoreBlockHeader = new llvm::StoreInst(BlockHeader, BlockHeaderPtr, block);
      int idx = 2;
      for (auto child : constructor->getArguments()) {
        llvm::Value *ChildValue = createTerm(child, substitution, definition, block, Module);
        llvm::Value *ChildPtr = llvm::GetElementPtrInst::CreateInBounds(BlockType, Block, {llvm::ConstantInt::get(llvm::IntegerType::get(Context, 64), 0), llvm::ConstantInt::get(llvm::IntegerType::get(Context, 32), idx)}, "", block);
        llvm::StoreInst *StoreChild = new llvm::StoreInst(ChildValue, ChildPtr, block);
      }
      llvm::BitCastInst *Cast = new llvm::BitCastInst(Block, llvm::PointerType::get(Module->getTypeByName("block"), 0), "", block);
      return Cast;
    }
  } else {
    assert(false && "not supported yet: meta level");
  }
}

}
