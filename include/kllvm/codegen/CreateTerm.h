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
  llvm::Value *createFunctionCall(std::string name, KOREObjectCompositePattern *pattern, bool sret, bool fastcc);
  llvm::Value *notInjectionCase(KOREObjectCompositePattern *constructor, llvm::Value *val);
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
  std::pair<llvm::Value *, bool> operator()(KOREPattern *pattern);
  llvm::Value *createToken(ValueType sort, std::string contents);
  /* creates a call instructin calling a particular llvm function, abstracting certain abi and calling convention details. 
   * name: the nmae of the function to call in llvm
   * returnCat: the value category of the return type of the function
   * args: the arguments to pass to the functgion
   * sret: if true, this is a function that returns a struct constant via the C abi, ie, 
   * the function actually returns void and the return value is via a pointe. Note that this 
   * can be set to true even if the function does not return a struct, in which case its value
   * is ignored.
   * load: if the function returns a struct via sret, then if load is true, we load the value 
   * fastcc: true if we should use the fastcc calling convention
   * returned from the function before returning it. */
  llvm::Value *createFunctionCall(std::string name, ValueType returnCat, std::vector<llvm::Value *> &args, bool sret, bool fastcc);

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
/** creates a function that evaluates the side condition of the specified rule, and returns the name of the function. Returns empty string if function has no side condition. */
std::string makeSideConditionFunction(KOREAxiomDeclaration *axiom, KOREDefinition *definition, llvm::Module *Module);

/* returns the llvm::Type corresponding to the specified KORE sort category */
llvm::Type *getValueType(ValueType sort, llvm::Module *Module);

void addAbort(llvm::BasicBlock *block, llvm::Module *Module);

llvm::Value *allocateTerm(llvm::Type *AllocType, llvm::BasicBlock *block, const char *allocFn = "koreAlloc");
llvm::Value *allocateTerm(llvm::Type *AllocType, llvm::Value *Len, llvm::BasicBlock *block, const char *allocFn = "koreAlloc");
}

#endif // CREATE_TERM_H
