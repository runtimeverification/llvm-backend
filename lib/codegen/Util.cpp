#include "kllvm/codegen/Util.h"
#include "kllvm/codegen/CreateTerm.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/raw_ostream.h"

namespace kllvm {

// this is an adaptation of a function of the same name in llvm/lib/IR/Function.cpp
// that unfortunately was not made public by llvm. We use it to assist with
// the process of generating function names for "intrinsics" that we need to generate
// in order to deal with the limitations of the garbage collection API in llvm
std::string getMangledTypeStr(llvm::Type *Ty, bool &HasUnnamedType) {
  using namespace llvm;
  std::string Result;
  if (PointerType* PTyp = dyn_cast<PointerType>(Ty)) {
    Result += "p" + std::to_string(PTyp->getAddressSpace()) +
              getMangledTypeStr(PTyp->getElementType(), HasUnnamedType);
  } else if (ArrayType* ATyp = dyn_cast<ArrayType>(Ty)) {
    Result += "a" + std::to_string(ATyp->getNumElements()) +
              getMangledTypeStr(ATyp->getElementType(), HasUnnamedType);
  } else if (StructType *STyp = dyn_cast<StructType>(Ty)) {
    if (!STyp->isLiteral()) {
      Result += "s_";
      if (STyp->hasName())
        Result += STyp->getName();
      else
        HasUnnamedType = true;
    } else {
      Result += "sl_";
      for (auto Elem : STyp->elements())
        Result += getMangledTypeStr(Elem, HasUnnamedType);
    }
    // Ensure nested structs are distinguishable.
    Result += "s";
  } else if (FunctionType *FT = dyn_cast<FunctionType>(Ty)) {
    Result += "f_" + getMangledTypeStr(FT->getReturnType(), HasUnnamedType);
    for (size_t i = 0; i < FT->getNumParams(); i++)
      Result += getMangledTypeStr(FT->getParamType(i), HasUnnamedType);
    if (FT->isVarArg())
      Result += "vararg";
    // Ensure nested function types are distinguishable.
    Result += "f";
  } else if (VectorType* VTy = dyn_cast<VectorType>(Ty)) {
    ElementCount EC = VTy->getElementCount();
    if (EC.isScalable())
      Result += "nx";
    Result += "v" + std::to_string(EC.getKnownMinValue()) +
              getMangledTypeStr(VTy->getElementType(), HasUnnamedType);
  } else if (Ty) {
    switch (Ty->getTypeID()) {
    default: llvm_unreachable("Unhandled type");
    case Type::VoidTyID:      Result += "isVoid";   break;
    case Type::MetadataTyID:  Result += "Metadata"; break;
    case Type::HalfTyID:      Result += "f16";      break;
    case Type::BFloatTyID:    Result += "bf16";     break;
    case Type::FloatTyID:     Result += "f32";      break;
    case Type::DoubleTyID:    Result += "f64";      break;
    case Type::X86_FP80TyID:  Result += "f80";      break;
    case Type::FP128TyID:     Result += "f128";     break;
    case Type::PPC_FP128TyID: Result += "ppcf128";  break;
    case Type::X86_MMXTyID:   Result += "x86mmx";   break;
    case Type::X86_AMXTyID:   Result += "x86amx";   break;
    case Type::IntegerTyID:
      Result += "i" + std::to_string(cast<IntegerType>(Ty)->getBitWidth());
      break;
    }
  }
  return Result;
}

llvm::Function* koreHeapAlloc(ValueType Cat, std::string name, llvm::Module *module) {
  llvm::Type *size_type = llvm::Type::getInt64Ty(module->getContext());
  llvm::Type *Ty = getParamType(Cat, module);
  auto allocType = llvm::FunctionType::get(Ty, llvm::ArrayRef<llvm::Type*>(size_type), false);
  bool hasUnnamed = false;
  auto result = getOrInsertFunction(module, name + "_" + getMangledTypeStr(Ty, hasUnnamed), allocType);
  result->setReturnDoesNotAlias();
  return result;
}

llvm::Function* koreHeapAlloc(std::string name, llvm::Module *module) {
  llvm::Type *size_type = llvm::Type::getInt64Ty(module->getContext());
  auto allocType = llvm::FunctionType::get(llvm::Type::getInt8PtrTy(module->getContext()), llvm::ArrayRef<llvm::Type*>(size_type), false);
  auto result = getOrInsertFunction(module, name, allocType);
  result->setReturnDoesNotAlias();
  return result;
}

llvm::Function *castToFunctionOrAbort(llvm::Value* value) {
  llvm::Function *func = llvm::dyn_cast<llvm::Function>(value);
  if (!func) {
    value->print(llvm::errs());
    abort();
  }
  return func;
}

llvm::StructType *getTypeByName(llvm::Module *module, std::string name) {
  llvm::StructType *t;
#if LLVM_VERSION_MAJOR >= 12
  t = llvm::StructType::getTypeByName(module->getContext(), name);
#else
  t = module->getTypeByName(name);
#endif
  return t;
}



}
