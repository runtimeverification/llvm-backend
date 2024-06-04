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
  llvm::StructType *block_type = get_block_type(module_, definition_, symbol);

  std::stringstream kore_string;
  constructor->print(kore_string);
  llvm::Constant *block
      = module_->getOrInsertGlobal(kore_string.str(), block_type);
  auto *global_var = llvm::dyn_cast<llvm::GlobalVariable>(block);

  if (!global_var->hasInitializer()) {
    std::vector<llvm::Constant *> block_vals;

    llvm::StructType *block_header_type = llvm::StructType::getTypeByName(
        module_->getContext(), blockheader_struct);
    uint64_t header_val = get_block_header_val(module_, symbol, block_type)
                          | NOT_YOUNG_OBJECT_BIT;
    llvm::Constant *block_header = llvm::ConstantStruct::get(
        block_header_type,
        llvm::ConstantInt::get(
            llvm::Type::getInt64Ty(module_->getContext()), header_val));
    block_vals.push_back(block_header);

    llvm::ArrayType *empty_array_type = llvm::ArrayType::get(
        llvm::Type::getInt64Ty(module_->getContext()), 0);
    block_vals.push_back(llvm::ConstantArray::get(
        empty_array_type, llvm::ArrayRef<llvm::Constant *>()));

    int idx = 0;
    for (auto const &child : constructor->get_arguments()) {
      auto *sort = dynamic_cast<kore_composite_sort *>(
          symbol->get_arguments()[idx].get());
      auto cat = sort->get_category(definition_);
      if (is_collection_sort(cat)) {
        block_vals.push_back(get_offset_of_member(
            module_, block_header_type,
            get_block_offset(definition_, symbol, idx)));
      }
      llvm::Constant *child_value = nullptr;
      if (idx++ == 0 && val != nullptr) {
        child_value = val;
      } else {
        child_value = (*this)(child.get()).first;
      }
      block_vals.push_back(child_value);
    }

    global_var->setInitializer(llvm::ConstantExpr::getBitCast(
        llvm::ConstantStruct::get(block_type, block_vals), block_type));
  }

  std::vector<llvm::Constant *> idxs
      = {llvm::ConstantInt::get(llvm::Type::getInt64Ty(ctx_), 0)};
  return llvm::ConstantExpr::getBitCast(
      llvm::ConstantExpr::getInBoundsGetElementPtr(
          block_type, global_var, idxs),
      llvm::PointerType::getUnqual(llvm::StructType::getTypeByName(
          module_->getContext(), block_struct)));
}

std::pair<llvm::Constant *, bool>
create_static_term::operator()(kore_pattern *pattern) {
  if (auto *constructor = dynamic_cast<kore_composite_pattern *>(pattern)) {
    kore_symbol const *symbol = constructor->get_constructor();
    assert(symbol->is_concrete() && "not supported yet: sort variables");
    if (symbol->get_name() == "\\dv") {
      auto *sort = dynamic_cast<kore_composite_sort *>(
          symbol->get_formal_arguments()[0].get());
      auto *str_pattern = dynamic_cast<kore_string_pattern *>(
          constructor->get_arguments()[0].get());
      return std::make_pair(
          create_token(
              sort->get_category(definition_), str_pattern->get_contents()),
          false);
    }
    if (symbol->get_arguments().empty()) {
      llvm::StructType *block_type = llvm::StructType::getTypeByName(
          module_->getContext(), block_struct);
      llvm::Constant *cast = llvm::ConstantExpr::getIntToPtr(
          llvm::ConstantInt::get(
              llvm::Type::getInt64Ty(ctx_),
              (((uint64_t)symbol->get_tag()) << 32) | 1),
          llvm::PointerType::getUnqual(block_type));
      return std::make_pair(cast, false);
    }
    kore_symbol_declaration *symbol_decl
        = definition_->get_symbol_declarations().at(symbol->get_name());
    if (symbol_decl->attributes().contains(attribute_set::key::SortInjection)
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
    auto *global_var = llvm::dyn_cast<llvm::GlobalVariable>(global);
    if (!global_var->hasInitializer()) {
      mpz_t value;
      char const *data_start
          = contents.at(0) == '+' ? contents.c_str() + 1 : contents.c_str();
      mpz_init_set_str(value, data_start, 10);
      size_t size = mpz_size(value);
      int sign = mpz_sgn(value);
      llvm::ArrayType *limbs_type
          = llvm::ArrayType::get(llvm::Type::getInt64Ty(ctx_), size);
      llvm::Constant *limbs = module_->getOrInsertGlobal(
          "int_" + contents + "_limbs", limbs_type);
      auto *limbs_var = llvm::dyn_cast<llvm::GlobalVariable>(limbs);
      std::vector<llvm::Constant *> allocd_limbs;
      for (size_t i = 0; i < size; i++) {
        allocd_limbs.push_back(llvm::ConstantInt::get(
            llvm::Type::getInt64Ty(ctx_), value->_mp_d[i]));
      }
      limbs_var->setInitializer(
          llvm::ConstantArray::get(limbs_type, allocd_limbs));
      llvm::Constant *hdr = llvm::ConstantStruct::get(
          llvm::StructType::getTypeByName(
              module_->getContext(), blockheader_struct),
          llvm::ConstantInt::get(
              llvm::Type::getInt64Ty(ctx_),
              sizeof(mpz_hdr) - sizeof(blockheader) | NOT_YOUNG_OBJECT_BIT));
      llvm::ConstantInt *num_limbs
          = llvm::ConstantInt::get(llvm::Type::getInt32Ty(ctx_), size);
      llvm::Constant *mp_size = llvm::ConstantExpr::getMul(
          num_limbs,
          llvm::ConstantInt::getSigned(llvm::Type::getInt32Ty(ctx_), sign));
      global_var->setInitializer(llvm::ConstantStruct::get(
          llvm::StructType::getTypeByName(
              module_->getContext(), int_wrapper_struct),
          hdr,
          llvm::ConstantStruct::get(
              llvm::StructType::getTypeByName(
                  module_->getContext(), int_struct),
              num_limbs, mp_size,
              llvm::ConstantExpr::getPointerCast(
                  limbs_var, llvm::PointerType::getUnqual(ctx_)))));
      mpz_clear(value);
    }
    std::vector<llvm::Constant *> idxs
        = {llvm::ConstantInt::get(llvm::Type::getInt64Ty(ctx_), 0),
           llvm::ConstantInt::get(llvm::Type::getInt32Ty(ctx_), 1)};
    return llvm::ConstantExpr::getInBoundsGetElementPtr(
        llvm::StructType::getTypeByName(
            module_->getContext(), int_wrapper_struct),
        global_var, idxs);
  }
  case sort_category::Float: {
    llvm::Constant *global = module_->getOrInsertGlobal(
        "float_" + contents, llvm::StructType::getTypeByName(
                                 module_->getContext(), float_wrapper_struct));
    auto *global_var = llvm::dyn_cast<llvm::GlobalVariable>(global);
    if (!global_var->hasInitializer()) {
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
      int ret_value = 0;
      if (contents == "+Infinity" || contents == "-Infinity"
          || contents == "Infinity") {
        ret_value = mpfr_set_str(value, contents.c_str(), 10, MPFR_RNDN);
      } else {
        size_t last = contents.find_last_of("fFdDpP");
        std::string str_value = contents.substr(0, last);
        ret_value = mpfr_set_str(value, str_value.c_str(), 10, MPFR_RNDN);
      }
      if (ret_value != 0) {
        throw std::invalid_argument("Can't convert to float");
      }
      size_t size = (prec + 63) / 64;
      llvm::ArrayType *limbs_type
          = llvm::ArrayType::get(llvm::Type::getInt64Ty(ctx_), size);
      llvm::Constant *limbs = module_->getOrInsertGlobal(
          "float_" + contents + "_limbs", limbs_type);
      auto *limbs_var = llvm::dyn_cast<llvm::GlobalVariable>(limbs);
      std::vector<llvm::Constant *> allocd_limbs;
      for (size_t i = 0; i < size; i++) {
        allocd_limbs.push_back(llvm::ConstantInt::get(
            llvm::Type::getInt64Ty(ctx_), value->_mpfr_d[i]));
      }
      limbs_var->setInitializer(
          llvm::ConstantArray::get(limbs_type, allocd_limbs));
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
      global_var->setInitializer(llvm::ConstantStruct::get(
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
                       limbs_var, llvm::PointerType::getUnqual(ctx_))}))));
      mpfr_clear(value);
    }
    std::vector<llvm::Constant *> idxs
        = {llvm::ConstantInt::get(llvm::Type::getInt64Ty(ctx_), 0),
           llvm::ConstantInt::get(llvm::Type::getInt32Ty(ctx_), 1)};
    return llvm::ConstantExpr::getInBoundsGetElementPtr(
        llvm::StructType::getTypeByName(
            module_->getContext(), float_wrapper_struct),
        global_var, idxs);
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
    llvm::StructType *string_type = llvm::StructType::get(
        ctx_,
        {llvm::StructType::getTypeByName(
             module_->getContext(), blockheader_struct),
         llvm::ArrayType::get(llvm::Type::getInt8Ty(ctx_), contents.size())});
    llvm::Constant *global
        = module_->getOrInsertGlobal("token_" + escape(contents), string_type);
    auto *global_var = llvm::dyn_cast<llvm::GlobalVariable>(global);
    if (!global_var->hasInitializer()) {
      llvm::StructType *block_header_type = llvm::StructType::getTypeByName(
          module_->getContext(), blockheader_struct);
      // this object does not live on the young generation, so we need to set
      // the correct gc bit.
      llvm::Constant *block_header = llvm::ConstantStruct::get(
          block_header_type, llvm::ConstantInt::get(
                                 llvm::Type::getInt64Ty(ctx_),
                                 contents.size() | NOT_YOUNG_OBJECT_BIT));
      global_var->setInitializer(llvm::ConstantStruct::get(
          string_type, block_header,
          llvm::ConstantDataArray::getString(ctx_, contents, false)));
    }
    return llvm::ConstantExpr::getPointerCast(
        global, llvm::PointerType::getUnqual(llvm::StructType::getTypeByName(
                    module_->getContext(), block_struct)));
  }
  case sort_category::SetIter:
  case sort_category::MapIter:
  case sort_category::Uncomputed: abort();
  }
}
// NOLINTEND(*-cognitive-complexity)

} // namespace kllvm
