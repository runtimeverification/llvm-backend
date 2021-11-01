#include "kllvm/codegen/CreateStaticTerm.h"
#include "kllvm/codegen/Util.h"

#include <iomanip>

#include "runtime/header.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IRReader/IRReader.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Support/SourceMgr.h"
#include "llvm/Support/raw_ostream.h"

namespace kllvm {

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

extern std::string escape(std::string str);

std::pair<llvm::Value *, bool>
CreateStaticTerm::operator()(KOREPattern *pattern) {
  if (auto constructor = dynamic_cast<KORECompositePattern *>(pattern)) {
    const KORESymbol *symbol = constructor->getConstructor();
    assert(symbol->isConcrete() && "not supported yet: sort variables");
    if (symbol->getName() == "\\dv") {
      auto sort = dynamic_cast<KORECompositeSort *>(
          symbol->getFormalArguments()[0].get());
      auto strPattern = dynamic_cast<KOREStringPattern *>(
          constructor->getArguments()[0].get());
      return std::make_pair(
          createToken(sort->getCategory(Definition), strPattern->getContents()),
          false);
    }
  }
  assert(false && "Something went wrong when trying to allocate a static term");
  abort();
}

llvm::Value *
CreateStaticTerm::createToken(ValueType sort, std::string contents) {
  switch (sort.cat) {
  case SortCategory::Map:
  case SortCategory::List:
  case SortCategory::Set:
    assert(false && "cannot create tokens of collection category");
  case SortCategory::Int: {
    llvm::Constant *global = Module->getOrInsertGlobal(
        "int_" + contents, getTypeByName(Module, INT_WRAPPER_STRUCT));
    llvm::GlobalVariable *globalVar
        = llvm::dyn_cast<llvm::GlobalVariable>(global);
    if (!globalVar->hasInitializer()) {
      mpz_t value;
      const char *dataStart
          = contents.at(0) == '+' ? contents.c_str() + 1 : contents.c_str();
      mpz_init_set_str(value, dataStart, 10);
      size_t size = mpz_size(value);
      int sign = mpz_sgn(value);
      llvm::ArrayType *limbsType
          = llvm::ArrayType::get(llvm::Type::getInt64Ty(Ctx), size);
      llvm::Constant *limbs
          = Module->getOrInsertGlobal("int_" + contents + "_limbs", limbsType);
      llvm::GlobalVariable *limbsVar
          = llvm::dyn_cast<llvm::GlobalVariable>(limbs);
      std::vector<llvm::Constant *> allocdLimbs;
      for (size_t i = 0; i < size; i++) {
        allocdLimbs.push_back(llvm::ConstantInt::get(
            llvm::Type::getInt64Ty(Ctx), value->_mp_d[i]));
      }
      limbsVar->setInitializer(
          llvm::ConstantArray::get(limbsType, allocdLimbs));
      llvm::Constant *hdr = llvm::ConstantStruct::get(
          getTypeByName(Module, BLOCKHEADER_STRUCT),
          llvm::ConstantInt::get(
              llvm::Type::getInt64Ty(Ctx),
              sizeof(mpz_hdr) - sizeof(blockheader) | NOT_YOUNG_OBJECT_BIT));
      llvm::ConstantInt *numLimbs
          = llvm::ConstantInt::get(llvm::Type::getInt32Ty(Ctx), size);
      llvm::Constant *mp_size = llvm::ConstantExpr::getMul(
          numLimbs,
          llvm::ConstantInt::getSigned(llvm::Type::getInt32Ty(Ctx), sign));
      globalVar->setInitializer(llvm::ConstantStruct::get(
          getTypeByName(Module, INT_WRAPPER_STRUCT), hdr,
          llvm::ConstantStruct::get(
              getTypeByName(Module, INT_STRUCT), numLimbs, mp_size,
              llvm::ConstantExpr::getPointerCast(
                  limbsVar, llvm::Type::getInt64PtrTy(Ctx)))));
      mpz_clear(value);
    }
    std::vector<llvm::Constant *> Idxs
        = {llvm::ConstantInt::get(llvm::Type::getInt64Ty(Ctx), 0),
           llvm::ConstantInt::get(llvm::Type::getInt32Ty(Ctx), 1)};
    return llvm::ConstantExpr::getInBoundsGetElementPtr(
        getTypeByName(Module, INT_WRAPPER_STRUCT), globalVar, Idxs);
  }
  case SortCategory::Float: {
    llvm::Constant *global = Module->getOrInsertGlobal(
        "float_" + contents, getTypeByName(Module, FLOAT_WRAPPER_STRUCT));
    llvm::GlobalVariable *globalVar
        = llvm::dyn_cast<llvm::GlobalVariable>(global);
    if (!globalVar->hasInitializer()) {
      size_t prec, exp;
      const char last = contents.back();
      if (last == 'f' || last == 'F') {
        prec = 24;
        exp = 8;
      } else {
        size_t has_prec = contents.find_first_of("pP");
        if (has_prec == std::string::npos) {
          prec = 53;
          exp = 11;
        } else {
          size_t exp_idx = contents.find_first_of("xX");
          std::string prec_str
              = contents.substr(has_prec + 1, exp_idx - has_prec);
          std::string exp_str = contents.substr(exp_idx + 1);
          prec = atoll(prec_str.c_str());
          exp = atoll(exp_str.c_str());
        }
      }
      mpfr_t value;
      mpfr_init2(value, prec);
      int retValue;
      if (contents == "+Infinity" || contents == "-Infinity"
          || contents == "Infinity") {
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
      llvm::ArrayType *limbsType
          = llvm::ArrayType::get(llvm::Type::getInt64Ty(Ctx), size);
      llvm::Constant *limbs = Module->getOrInsertGlobal(
          "float_" + contents + "_limbs", limbsType);
      llvm::GlobalVariable *limbsVar
          = llvm::dyn_cast<llvm::GlobalVariable>(limbs);
      std::vector<llvm::Constant *> allocdLimbs;
      for (size_t i = 0; i < size; i++) {
        allocdLimbs.push_back(llvm::ConstantInt::get(
            llvm::Type::getInt64Ty(Ctx), value->_mpfr_d[i]));
      }
      limbsVar->setInitializer(
          llvm::ConstantArray::get(limbsType, allocdLimbs));
      llvm::Constant *hdr = llvm::ConstantStruct::get(
          getTypeByName(Module, BLOCKHEADER_STRUCT),
          llvm::ConstantInt::get(
              llvm::Type::getInt64Ty(Ctx),
              (sizeof(floating_hdr) - sizeof(blockheader))
                  | NOT_YOUNG_OBJECT_BIT));
      llvm::Constant *expbits
          = llvm::ConstantInt::get(llvm::Type::getInt64Ty(Ctx), exp);
      llvm::Constant *mpfr_prec
          = llvm::ConstantInt::get(llvm::Type::getInt64Ty(Ctx), prec);
      llvm::Constant *mpfr_sign = llvm::ConstantInt::getSigned(
          llvm::Type::getInt32Ty(Ctx), value->_mpfr_sign);
      llvm::Constant *mpfr_exp = llvm::ConstantInt::getSigned(
          llvm::Type::getInt64Ty(Ctx), value->_mpfr_exp);
      // create struct floating with an exponent range and an __mpfr_struct.
      // Note that we are assuming the format of the struct, but it's unlikely
      // to change except possibly between major releases which happen less than
      // once every couple years, because the C++ ABI depends on it. We are also
      // assuming that the host and target have the same arch, but since we
      // don't yet support cross compiling anyway, that's a safe assumption.
      globalVar->setInitializer(llvm::ConstantStruct::get(
          getTypeByName(Module, FLOAT_WRAPPER_STRUCT), hdr,
          llvm::ConstantStruct::get(
              getTypeByName(Module, FLOAT_STRUCT), expbits,
              llvm::ConstantStruct::getAnon(
                  {mpfr_prec, mpfr_sign, mpfr_exp,
                   llvm::ConstantExpr::getPointerCast(
                       limbsVar, llvm::Type::getInt64PtrTy(Ctx))}))));
      mpfr_clear(value);
    }
    std::vector<llvm::Constant *> Idxs
        = {llvm::ConstantInt::get(llvm::Type::getInt64Ty(Ctx), 0),
           llvm::ConstantInt::get(llvm::Type::getInt32Ty(Ctx), 1)};
    return llvm::ConstantExpr::getInBoundsGetElementPtr(
        getTypeByName(Module, FLOAT_WRAPPER_STRUCT), globalVar, Idxs);
  }
  case SortCategory::StringBuffer:
    assert(false && "not implemented yet: tokens");
  case SortCategory::MInt: {
    size_t idx = contents.find_first_of("pP");
    assert(idx != std::string::npos);
    uint64_t bits = std::stoi(contents.substr(idx + 1));
    return llvm::ConstantInt::get(
        llvm::IntegerType::get(Ctx, bits), contents.substr(0, idx), 10);
  }
  case SortCategory::Bool:
    return llvm::ConstantInt::get(
        llvm::Type::getInt1Ty(Ctx), contents == "true");
  case SortCategory::Variable:
  case SortCategory::Symbol: {
    llvm::StructType *StringType = llvm::StructType::get(
        Ctx,
        {getTypeByName(Module, BLOCKHEADER_STRUCT),
         llvm::ArrayType::get(llvm::Type::getInt8Ty(Ctx), contents.size())});
    llvm::Constant *global
        = Module->getOrInsertGlobal("token_" + escape(contents), StringType);
    llvm::GlobalVariable *globalVar
        = llvm::dyn_cast<llvm::GlobalVariable>(global);
    if (!globalVar->hasInitializer()) {
      llvm::StructType *BlockHeaderType
          = getTypeByName(Module, BLOCKHEADER_STRUCT);
      // this object does not live on the young generation, so we need to set
      // the correct gc bit.
      llvm::Constant *BlockHeader = llvm::ConstantStruct::get(
          BlockHeaderType, llvm::ConstantInt::get(
                               llvm::Type::getInt64Ty(Ctx),
                               contents.size() | NOT_YOUNG_OBJECT_BIT));
      globalVar->setInitializer(llvm::ConstantStruct::get(
          StringType, BlockHeader,
          llvm::ConstantDataArray::getString(Ctx, contents, false)));
    }
    return llvm::ConstantExpr::getPointerCast(
        global,
        llvm::PointerType::getUnqual(getTypeByName(Module, BLOCK_STRUCT)));
  }
  case SortCategory::Uncomputed: abort();
  }
}

} // namespace kllvm
