#include <kllvm/codegen/CreateTerm.h>

#include <kllvm/ast/AST.h>

#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Instructions.h>

#include <map>
#include <numeric>
#include <string>
#include <unordered_set>

namespace kllvm {

namespace {

/*
 * Compute the set of variables that need to be marked as copy-on-write in the
 * RHS of a rule. For a variable to be CoW, the following conditions need to be
 * met:
 *   - It is of sort Bytes (the only mutable sort implemented by the LLVM
 *     backend).
 *   - It appears more than once on the RHS of a rule, allowing for possible
 *     aliasing. For example, consider the following LHS pattern:
 *
 *       c(B:Bytes) #as C
 *
 *     Here, if both B and C appear on the RHS, then B must be marked as CoW (as
 *     the RHS term including C includes a second reference to B).
 */
std::unordered_set<std::string> copyOnWriteVariables(
    KOREDefinition *definition, KOREAxiomDeclaration *axiom,
    std::map<std::string, KOREVariablePattern *> const &all_vars) {
  auto byte_vars = std::unordered_set<std::string>{};

  for (auto const &[_, var] : all_vars) {
    if (auto sort
        = std::dynamic_pointer_cast<KORECompositeSort>(var->getSort());
        sort->getName() == "SortBytes") {
      byte_vars.insert(var->getName());
    }
  }

  if (byte_vars.empty()) {
    return {};
  }

  auto cow_vars = std::unordered_set<std::string>{};

  auto all_aliases = axiom->aliasSets();
  auto *rhs = axiom->getRightHandSide();

  for (auto const &var : byte_vars) {
    auto const &aliases = all_aliases.at(var);

    auto total = std::accumulate(
        aliases.begin(), aliases.end(), size_t{0},
        [&](auto acc, auto const &other) {
          return acc + rhs->countOccurrences(other);
        });

    if (total > 1) {
      cow_vars.insert(var);
    }
  }

  return cow_vars;
}

} // namespace

void makeCopyOnWriteCalls(
    llvm::BasicBlock *insertAtEnd, KOREDefinition *definition,
    KOREAxiomDeclaration *axiom,
    std::map<std::string, KOREVariablePattern *> const &all_vars,
    llvm::StringMap<llvm::Value *> const &subst) {
  auto vars = copyOnWriteVariables(definition, axiom, all_vars);

  auto &ctx = insertAtEnd->getContext();
  auto *mod = insertAtEnd->getModule();

  auto *ptr_ty = llvm::PointerType::getUnqual(ctx);
  auto *void_ty = llvm::Type::getVoidTy(ctx);

  auto *fn_ty = llvm::FunctionType::get(void_ty, {ptr_ty}, false);

  auto make_cow = mod->getOrInsertFunction("make_copy_on_write", fn_ty);

  for (auto const &v : vars) {
#if LLVM_VERSION_MAJOR >= 17
    llvm::CallInst::Create(make_cow, {subst.at(v)}, "cow_" + v, insertAtEnd);
#else
    if (subst.find(v) != subst.end()) {
      llvm::CallInst::Create(
          make_cow, {subst.lookup(v)}, "cow_" + v, insertAtEnd);
    } else {
      assert(false && "Missing substitution for variable");
    }
#endif
  }
}

} // namespace kllvm
