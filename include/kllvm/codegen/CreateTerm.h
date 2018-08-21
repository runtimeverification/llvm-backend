#ifndef CREATE_TERM_H
#define CREATE_TERM_H

#include "kllvm/ast/AST.h"

#include "llvm/ADT/StringMap.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Value.h"

namespace kllvm {

class CreateTerm {
private:
  llvm::StringMap<llvm::Value *> &Substitution;
  KOREDefinition *Definition;
  llvm::BasicBlock *CurrentBlock;
  llvm::Module *Module;
  llvm::LLVMContext &Ctx;

  llvm::Value *createHook(KOREObjectCompositePattern *hookAtt, KOREObjectCompositePattern *pattern);
  llvm::Value *createFunctionCall(std::string name, KOREObjectCompositePattern *pattern);
  llvm::Value *createToken(SortCategory sort, std::string contents);
public:
  CreateTerm(
    llvm::StringMap<llvm::Value *> &Substitution,
    KOREDefinition *Definition,
    llvm::BasicBlock *EntryBlock,
    llvm::Module *Module) :
      Substitution(Substitution),
      Definition(Definition),
      CurrentBlock(EntryBlock),
      Module(Module),
      Ctx(Module->getContext()) {}

/* adds code to the specified basic block in the specified module which constructs
   an llvm value corresponding to the specified KORE RHS pattern and substitution in the
   specified definition, and returns the value itself. */
  llvm::Value *operator()(KOREPattern *pattern);

  llvm::BasicBlock *getCurrentBlock() const { return CurrentBlock; }
};

/* Creates a new llvm::Module with the predefined declarations common to all llvm modules
   in the llvm backend. */
std::unique_ptr<llvm::Module> newModule(std::string name, llvm::LLVMContext &Context);

llvm::StructType *getBlockType(llvm::Module *Module, KOREDefinition *definition, const KOREObjectSymbol *symbol);
llvm::Value *getBlockHeader(llvm::Module *Module, KOREDefinition *definition,
    const KOREObjectSymbol *symbol, llvm::Type *BlockType);

/* returns the llvm::Type corresponding to the type of the result of calling createTerm
   on the specified pattern. */
llvm::Type *termType(KOREPattern *pattern, llvm::StringMap<llvm::Type *> &substitution, KOREDefinition *definition, llvm::Module *Module);

/** creates a function that applies the specified rule once it has matched, and returns the name of the function. */
std::string makeApplyRuleFunction(KOREAxiomDeclaration *axiom, KOREDefinition *definition, llvm::Module *Module);

/* returns the llvm::Type corresponding to the specified KORE sort category */
llvm::Type *getValueType(SortCategory sort, llvm::Module *Module);

void addAbort(llvm::BasicBlock *block, llvm::Module *Module);

llvm::Value *allocateBlock(llvm::Type *AllocType, llvm::BasicBlock *block);
}

#endif // CREATE_TERM_H
