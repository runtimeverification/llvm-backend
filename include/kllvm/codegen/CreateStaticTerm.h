#ifndef CREATE_STATIC_TERM_H
#define CREATE_STATIC_TERM_H

#include "kllvm/ast/AST.h"

#include "llvm/IR/Constant.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"

#include <string>
#include <utility>

namespace kllvm {

class create_static_term {
private:
  kore_definition *definition_;
  llvm::Module *module_;
  llvm::LLVMContext &ctx_;

  llvm::Constant *
  notInjectionCase(kore_composite_pattern *constructor, llvm::Constant *val);

public:
  create_static_term(kore_definition *definition, llvm::Module *module)
      : definition_(definition)
      , module_(module)
      , ctx_(module->getContext()) { }

  std::pair<llvm::Constant *, bool> operator()(kore_pattern *pattern);
  llvm::Constant *createToken(value_type sort, std::string contents);
};

} // namespace kllvm

#endif // CREATE_STATIC_TERM_H
