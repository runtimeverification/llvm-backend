#ifndef CREATE_STATIC_TERM_H
#define CREATE_STATIC_TERM_H

#include "kllvm/ast/AST.h"

#include "llvm/IR/Constant.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"

#include <string>
#include <utility>

namespace kllvm {

class CreateStaticTerm {
private:
  KOREDefinition *Definition;
  llvm::Module *Module;
  llvm::LLVMContext &Ctx;

  llvm::Constant *
  notInjectionCase(KORECompositePattern *constructor, llvm::Constant *val);

public:
  CreateStaticTerm(KOREDefinition *Definition, llvm::Module *Module)
      : Definition(Definition)
      , Module(Module)
      , Ctx(Module->getContext()) { }

  std::pair<llvm::Constant *, bool> operator()(KOREPattern *pattern);
  llvm::Constant *createToken(ValueType sort, std::string contents);
};

} // namespace kllvm

#endif // CREATE_STATIC_TERM_H
