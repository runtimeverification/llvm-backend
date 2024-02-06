#ifndef CREATE_TERM_H
#define CREATE_TERM_H

#include "kllvm/ast/AST.h"
#include "kllvm/codegen/DecisionParser.h"

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
  bool isAnywhereOwise;
  std::set<KOREPattern *> staticTerms;

  llvm::Value *
  alloc_arg(KORECompositePattern *pattern, int idx, std::string locationStack);
  llvm::Value *createHook(
      KORECompositePattern *hookAtt, KORECompositePattern *pattern,
      std::string const &locationStack = "0");
  llvm::Value *createFunctionCall(
      std::string const &name, KORECompositePattern *pattern, bool sret,
      bool tailcc, std::string const &locationStack = "0");
  llvm::Value *notInjectionCase(
      KORECompositePattern *constructor, llvm::Value *val,
      std::string locationStack = "0");
  bool populateStaticSet(KOREPattern *pattern);
  std::pair<llvm::Value *, bool> createAllocation(
      KOREPattern *pattern, std::string const &locationStack = "0");

public:
  CreateTerm(
      llvm::StringMap<llvm::Value *> &Substitution, KOREDefinition *Definition,
      llvm::BasicBlock *EntryBlock, llvm::Module *Module, bool isAnywhereOwise)
      : Substitution(Substitution)
      , Definition(Definition)
      , CurrentBlock(EntryBlock)
      , Module(Module)
      , Ctx(Module->getContext())
      , isAnywhereOwise(isAnywhereOwise)
      , staticTerms(std::set<KOREPattern *>()) { }

  /* adds code to the specified basic block in the specified module which
     constructs an llvm value corresponding to the specified KORE RHS pattern
     and substitution in the
     specified definition, and returns the value itself, along with a boolean
     indicating whether the resulting term could be an injection. */
  std::pair<llvm::Value *, bool> operator()(KOREPattern *pattern);

  /* creates a call instruction calling a particular LLVM function, abstracting
     certain ABI and calling convention details:
      - name: the name of the function to call
      - returnCat: the value category of the return type of the function
      - args: the arguments to pass to the function
      - sret: if true, this is a function that returns a struct constant via the
              C ABI (that is, the function actually returns void and the return value
              is via a pointer). Note that this can be set to true even if the
              function does not return a struct, in which case its value is
              ignored.
      - load: if the function returns a struct via sret and load is true,
              we load the value on return.
      - tailcc: true if the call should be made via the tailcc calling convention.
    */
  llvm::Value *createFunctionCall(
      std::string const &name, ValueType returnCat,
      std::vector<llvm::Value *> const &args, bool sret, bool tailcc,
      std::string const &locationStack = "0");

  llvm::BasicBlock *getCurrentBlock() const { return CurrentBlock; }
};

std::string escape(std::string const &str);

/* Creates a new llvm::Module with the predefined declarations common to all
   llvm modules in the llvm backend. */
std::unique_ptr<llvm::Module>
newModule(std::string const &name, llvm::LLVMContext &Context);

llvm::StructType *getBlockType(
    llvm::Module *Module, KOREDefinition *definition, KORESymbol const *symbol);
uint64_t getBlockHeaderVal(
    llvm::Module *Module, KORESymbol const *symbol, llvm::Type *BlockType);
llvm::Value *getBlockHeader(
    llvm::Module *Module, KOREDefinition *definition, KORESymbol const *symbol,
    llvm::Type *BlockType);

/* returns the llvm::Type corresponding to the type of the result of calling
   createTerm on the specified pattern. */
ValueType termType(
    KOREPattern *pattern, llvm::StringMap<ValueType> &substitution,
    KOREDefinition *definition);

/** creates a function that applies the specified rule once it has matched, and
 * returns the name of the function. */
void makeApplyRuleFunction(
    KOREAxiomDeclaration *axiom, KOREDefinition *definition,
    llvm::Module *Module, bool bigStep = false);
std::string makeApplyRuleFunction(
    KOREAxiomDeclaration *axiom, KOREDefinition *definition,
    llvm::Module *Module, std::vector<Residual> const &residuals);
/** creates a function that evaluates the side condition of the specified rule,
 * and returns the name of the function. Returns empty string if function has no
 * side condition. */
std::string makeSideConditionFunction(
    KOREAxiomDeclaration *axiom, KOREDefinition *definition,
    llvm::Module *Module);

extern std::string MAP_STRUCT;
extern std::string LIST_STRUCT;
extern std::string SET_STRUCT;
extern std::string INT_WRAPPER_STRUCT;
extern std::string INT_STRUCT;
extern std::string FLOAT_WRAPPER_STRUCT;
extern std::string FLOAT_STRUCT;
extern std::string BUFFER_STRUCT;
extern std::string BLOCK_STRUCT;
extern std::string BLOCKHEADER_STRUCT;

llvm::StructType *getBlockType(llvm::Module *Module);

llvm::Type *getArgType(ValueType cat, llvm::Module *mod);

/* returns the llvm::Type corresponding to the specified KORE sort category */
llvm::Type *getValueType(ValueType sort, llvm::Module *Module);
llvm::Type *getParamType(ValueType sort, llvm::Module *Module);

bool isCollectionSort(ValueType cat);
bool isInjectionSymbol(KOREPattern *p, KORESymbol *sym);

void addAbort(llvm::BasicBlock *block, llvm::Module *Module);

llvm::Value *allocateTerm(
    llvm::Type *AllocType, llvm::BasicBlock *block,
    char const *allocFn = "koreAlloc");
llvm::Value *allocateTerm(
    llvm::Type *AllocType, llvm::Value *Len, llvm::BasicBlock *block,
    char const *allocFn = "koreAlloc");
} // namespace kllvm

#endif // CREATE_TERM_H
