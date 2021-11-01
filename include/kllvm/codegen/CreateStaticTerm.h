#ifndef CREATE_STATIC_TERM_H
#define CREATE_STATIC_TERM_H

#include "kllvm/ast/AST.h"
#include "kllvm/codegen/CreateTerm.h"
#include "kllvm/codegen/DecisionParser.h"

#include "llvm/ADT/StringMap.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Value.h"

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
