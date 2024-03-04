#include "kllvm/codegen/CreateStaticTerm.h"

#include "kllvm/codegen/CreateTerm.h"
#include "kllvm/codegen/Util.h"

#include <llvm/ADT/ArrayRef.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/GlobalVariable.h>
#include <llvm/IR/Type.h>
#include <llvm/Support/Casting.h>

#include <gmp.h>
#include <mpfr.h>

#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <unordered_map>
#include <vector>

#include "config/macros.h"
#include "runtime/header.h"

namespace kllvm {

/* create a term, given the assumption that the created term will not be a
 * triangle injection pair */
llvm::Constant *create_static_term::not_injection_case(
    kore_composite_pattern *constructor, llvm::Constant *val) {
  kore_symbol const *symbol = constructor->get_constructor();
  llvm::StructType *BlockType = getBlockType(module_, definition_, symbol);

  std::stringstream koreString;
  constructor->print(koreString);
  llvm::Constant *Block
      = module_->getOrInsertGlobal(koreString.str(), BlockType);
  auto *globalVar = llvm::dyn_cast<llvm::GlobalVariable>(Block);

  if (!globalVar->hasInitializer()) {
    std::vector<llvm::Constant *> blockVals;

    llvm::StructType *BlockHeaderType = llvm::StructType::getTypeByName(
        module_->getContext(), blockheader_struct);
    uint64_t headerVal
        = getBlockHeaderVal(module_, symbol, BlockType) | NOT_YOUNG_OBJECT_BIT;
    llvm::Constant *BlockHeader = llvm::ConstantStruct::get(
        BlockHeaderType,
        llvm::ConstantInt::get(
            llvm::Type::getInt64Ty(module_->getContext()), headerVal));
    blockVals.push_back(BlockHeader);

    llvm::ArrayType *EmptyArrayType
        = llvm::ArrayType::get(llvm::Type::getInt64Ty(module_->getContext()), 0);
    blockVals.push_back(llvm::ConstantArray::get(
        EmptyArrayType, llvm::ArrayRef<llvm::Constant *>()));

    int idx = 2;
    for (auto const &child : constructor->get_arguments()) {
      llvm::Constant *ChildValue = nullptr;
      if (idx++ == 2 && val != nullptr) {
        ChildValue = val;
      } else {
        ChildValue = (*this)(child.get()).first;
      }
      blockVals.push_back(ChildValue);
    }

    globalVar->setInitializer(llvm::ConstantExpr::getBitCast(
        llvm::ConstantStruct::get(BlockType, blockVals), BlockType));
  }

  std::vector<llvm::Constant *> Idxs
      = {llvm::ConstantInt::get(llvm::Type::getInt64Ty(ctx_), 0)};
  return llvm::ConstantExpr::getBitCast(
      llvm::ConstantExpr::getInBoundsGetElementPtr(BlockType, globalVar, Idxs),
      llvm::PointerType::getUnqual(
          llvm::StructType::getTypeByName(module_->getContext(), block_struct)));
}

std::pair<llvm::Constant *, bool>
create_static_term::operator()(kore_pattern *pattern) {
  if (auto *constructor = dynamic_cast<kore_composite_pattern *>(pattern)) {
    kore_symbol const *symbol = constructor->get_constructor();
    assert(symbol->is_concrete() && "not supported yet: sort variables");
    if (symbol->get_name() == "\\dv") {
      auto *sort = dynamic_cast<kore_composite_sort *>(
          symbol->get_formal_arguments()[0].get());
      auto *strPattern = dynamic_cast<kore_string_pattern *>(
          constructor->get_arguments()[0].get());
      return std::make_pair(
          create_token(sort->get_category(definition_), strPattern->get_contents()),
          false);
    }
    if (symbol->get_arguments().empty()) {
      llvm::StructType *BlockType
          = llvm::StructType::getTypeByName(module_->getContext(), block_struct);
      llvm::Constant *Cast = llvm::ConstantExpr::getIntToPtr(
          llvm::ConstantInt::get(
              llvm::Type::getInt64Ty(ctx_),
              (((uint64_t)symbol->get_tag()) << 32) | 1),
          llvm::PointerType::getUnqual(BlockType));
      return std::make_pair(Cast, false);
    }
    kore_symbol_declaration *symbolDecl
        = definition_->get_symbol_declarations().at(symbol->get_name());
    if (symbolDecl->attributes().contains(attribute_set::key::SortInjection)
        && dynamic_cast<kore_composite_sort *>(symbol->get_arguments()[0].get())
                   ->get_category(definition_)
                   .cat
               == sort_category::Symbol) {
      std::pair<llvm::Constant *, bool> val
          = (*this)(constructor->get_arguments()[0].get());
      if (val.second) {
        uint32_t tag = symbol->get_tag();
        kore_symbol *inj = definition_->get_inj_symbol();
        if (tag != (uint32_t)-1 && tag >= inj->get_first_tag()
            && tag <= inj->get_last_tag()) {
          return std::make_pair(val.first, true);
        }
        return std::make_pair(not_injection_case(constructor, val.first), true);
      }
      return std::make_pair(not_injection_case(constructor, val.first), true);
    }
    return std::make_pair(not_injection_case(constructor, nullptr), false);
  }
  assert(false && "Something went wrong when trying to allocate a static term");
  abort();
}

// NOLINTBEGIN(*-cognitive-complexity)
llvm::Constant *
create_static_term::create_token(value_type sort, std::string contents) {
  switch (sort.cat) {
  case sort_category::Map:
  case sort_category::RangeMap:
  case sort_category::List:
  case sort_category::Set:
    assert(false && "cannot create tokens of collection category");
  case sort_category::Int: {
    llvm::Constant *global = module_->getOrInsertGlobal(
        "int_" + contents, llvm::StructType::getTypeByName(
                               module_->getContext(), int_wrapper_struct));
    auto *globalVar = llvm::dyn_cast<llvm::GlobalVariable>(global);
    if (!globalVar->hasInitializer()) {
      mpz_t value;
      char const *dataStart
          = contents.at(0) == '+' ? contents.c_str() + 1 : contents.c_str();
      mpz_init_set_str(value, dataStart, 10);
      size_t size = mpz_size(value);
      int sign = mpz_sgn(value);
      llvm::ArrayType *limbsType
          = llvm::ArrayType::get(llvm::Type::getInt64Ty(ctx_), size);
      llvm::Constant *limbs
          = module_->getOrInsertGlobal("int_" + contents + "_limbs", limbsType);
      auto *limbsVar = llvm::dyn_cast<llvm::GlobalVariable>(limbs);
      std::vector<llvm::Constant *> allocdLimbs;
      for (size_t i = 0; i < size; i++) {
        allocdLimbs.push_back(llvm::ConstantInt::get(
            llvm::Type::getInt64Ty(ctx_), value->_mp_d[i]));
      }
      limbsVar->setInitializer(
          llvm::ConstantArray::get(limbsType, allocdLimbs));
      llvm::Constant *hdr = llvm::ConstantStruct::get(
          llvm::StructType::getTypeByName(
              module_->getContext(), blockheader_struct),
          llvm::ConstantInt::get(
              llvm::Type::getInt64Ty(ctx_),
              sizeof(mpz_hdr) - sizeof(blockheader) | NOT_YOUNG_OBJECT_BIT));
      llvm::ConstantInt *numLimbs
          = llvm::ConstantInt::get(llvm::Type::getInt32Ty(ctx_), size);
      llvm::Constant *mp_size = llvm::ConstantExpr::getMul(
          numLimbs,
          llvm::ConstantInt::getSigned(llvm::Type::getInt32Ty(ctx_), sign));
      globalVar->setInitializer(llvm::ConstantStruct::get(
          llvm::StructType::getTypeByName(
              module_->getContext(), int_wrapper_struct),
          hdr,
          llvm::ConstantStruct::get(
              llvm::StructType::getTypeByName(module_->getContext(), int_struct),
              numLimbs, mp_size,
              llvm::ConstantExpr::getPointerCast(
                  limbsVar, llvm::Type::getInt64PtrTy(ctx_)))));
      mpz_clear(value);
    }
    std::vector<llvm::Constant *> Idxs
        = {llvm::ConstantInt::get(llvm::Type::getInt64Ty(ctx_), 0),
           llvm::ConstantInt::get(llvm::Type::getInt32Ty(ctx_), 1)};
    return llvm::ConstantExpr::getInBoundsGetElementPtr(
        llvm::StructType::getTypeByName(
            module_->getContext(), int_wrapper_struct),
        globalVar, Idxs);
  }
  case sort_category::Float: {
    llvm::Constant *global = module_->getOrInsertGlobal(
        "float_" + contents, llvm::StructType::getTypeByName(
                                 module_->getContext(), float_wrapper_struct));
    auto *globalVar = llvm::dyn_cast<llvm::GlobalVariable>(global);
    if (!globalVar->hasInitializer()) {
      size_t prec = 0;
      size_t exp = 0;
      char const last = contents.back();
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
      int retValue = 0;
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
          = llvm::ArrayType::get(llvm::Type::getInt64Ty(ctx_), size);
      llvm::Constant *limbs = module_->getOrInsertGlobal(
          "float_" + contents + "_limbs", limbsType);
      auto *limbsVar = llvm::dyn_cast<llvm::GlobalVariable>(limbs);
      std::vector<llvm::Constant *> allocdLimbs;
      for (size_t i = 0; i < size; i++) {
        allocdLimbs.push_back(llvm::ConstantInt::get(
            llvm::Type::getInt64Ty(ctx_), value->_mpfr_d[i]));
      }
      limbsVar->setInitializer(
          llvm::ConstantArray::get(limbsType, allocdLimbs));
      llvm::Constant *hdr = llvm::ConstantStruct::get(
          llvm::StructType::getTypeByName(
              module_->getContext(), blockheader_struct),
          llvm::ConstantInt::get(
              llvm::Type::getInt64Ty(ctx_),
              (sizeof(floating_hdr) - sizeof(blockheader))
                  | NOT_YOUNG_OBJECT_BIT));
      llvm::Constant *expbits
          = llvm::ConstantInt::get(llvm::Type::getInt64Ty(ctx_), exp);
      llvm::Constant *mpfr_prec
          = llvm::ConstantInt::get(llvm::Type::getInt64Ty(ctx_), prec);
      llvm::Constant *mpfr_sign = llvm::ConstantInt::getSigned(
          llvm::Type::getInt32Ty(ctx_), value->_mpfr_sign);
      llvm::Constant *mpfr_exp = llvm::ConstantInt::getSigned(
          llvm::Type::getInt64Ty(ctx_), value->_mpfr_exp);
      // create struct floating with an exponent range and an __mpfr_struct.
      // Note that we are assuming the format of the struct, but it's unlikely
      // to change except possibly between major releases which happen less than
      // once every couple years, because the C++ ABI depends on it. We are also
      // assuming that the host and target have the same arch, but since we
      // don't yet support cross compiling anyway, that's a safe assumption.
      globalVar->setInitializer(llvm::ConstantStruct::get(
          llvm::StructType::getTypeByName(
              module_->getContext(), float_wrapper_struct),
          hdr,
          llvm::ConstantStruct::get(
              llvm::StructType::getTypeByName(
                  module_->getContext(), float_struct),
              expbits,
              llvm::ConstantStruct::getAnon(
                  {mpfr_prec, mpfr_sign, mpfr_exp,
                   llvm::ConstantExpr::getPointerCast(
                       limbsVar, llvm::Type::getInt64PtrTy(ctx_))}))));
      mpfr_clear(value);
    }
    std::vector<llvm::Constant *> Idxs
        = {llvm::ConstantInt::get(llvm::Type::getInt64Ty(ctx_), 0),
           llvm::ConstantInt::get(llvm::Type::getInt32Ty(ctx_), 1)};
    return llvm::ConstantExpr::getInBoundsGetElementPtr(
        llvm::StructType::getTypeByName(
            module_->getContext(), float_wrapper_struct),
        globalVar, Idxs);
  }
  case sort_category::StringBuffer:
    assert(false && "not implemented yet: tokens");
  case sort_category::MInt: {
    size_t idx = contents.find_first_of("pP");
    assert(idx != std::string::npos);
    uint64_t bits = std::stoi(contents.substr(idx + 1));
    return llvm::ConstantInt::get(
        llvm::IntegerType::get(ctx_, bits), contents.substr(0, idx), 10);
  }
  case sort_category::Bool:
    return llvm::ConstantInt::get(
        llvm::Type::getInt1Ty(ctx_), contents == "true");
  case sort_category::Variable:
  case sort_category::Symbol: {
    llvm::StructType *StringType = llvm::StructType::get(
        ctx_,
        {llvm::StructType::getTypeByName(
             module_->getContext(), blockheader_struct),
         llvm::ArrayType::get(llvm::Type::getInt8Ty(ctx_), contents.size())});
    llvm::Constant *global
        = module_->getOrInsertGlobal("token_" + escape(contents), StringType);
    auto *globalVar = llvm::dyn_cast<llvm::GlobalVariable>(global);
    if (!globalVar->hasInitializer()) {
      llvm::StructType *BlockHeaderType = llvm::StructType::getTypeByName(
          module_->getContext(), blockheader_struct);
      // this object does not live on the young generation, so we need to set
      // the correct gc bit.
      llvm::Constant *BlockHeader = llvm::ConstantStruct::get(
          BlockHeaderType, llvm::ConstantInt::get(
                               llvm::Type::getInt64Ty(ctx_),
                               contents.size() | NOT_YOUNG_OBJECT_BIT));
      globalVar->setInitializer(llvm::ConstantStruct::get(
          StringType, BlockHeader,
          llvm::ConstantDataArray::getString(ctx_, contents, false)));
    }
    return llvm::ConstantExpr::getPointerCast(
        global, llvm::PointerType::getUnqual(llvm::StructType::getTypeByName(
                    module_->getContext(), block_struct)));
  }
  case sort_category::Uncomputed: abort();
  }
}
// NOLINTEND(*-cognitive-complexity)

} // namespace kllvm
