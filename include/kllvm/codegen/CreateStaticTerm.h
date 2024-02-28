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
  kore_definition *Definition;
  llvm::Module *Module;
  llvm::LLVMContext &Ctx;

  llvm::Constant *
  notInjectionCase(kore_composite_pattern *constructor, llvm::Constant *val);

public:
  create_static_term(kore_definition *Definition, llvm::Module *Module)
      : Definition(Definition)
      , Module(Module)
      , Ctx(Module->getContext()) { }

  std::pair<llvm::Constant *, bool> operator()(kore_pattern *pattern);
  llvm::Constant *createToken(value_type sort, std::string contents);
};

} // namespace kllvm

#endif // CREATE_STATIC_TERM_H
