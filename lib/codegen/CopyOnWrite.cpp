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
 * Multiple sorts can in thoery be hooked to the same internal implementation of
 * mutable buffers; we can't assume that SortBytes is the only one. We therefore
 * need to look at their declaration to see if it's hooked to the known internal
 * implementation.
 */
bool is_hooked_to_bytes(KOREDefinition *definition, KORESort *sort) {
  auto const *decl = sort->declaration(*definition);
  if (!decl) {
    return false;
  }

  return decl->getAttributes().count("hook")
         && decl->getStringAttribute("hook") == "BYTES.Bytes";
}

/*
 * Test whether this variable or any of its subterms could syntactically be of a
 * sort that is hooked to the backend's implementation of mutable byte arrays.
 */
bool can_contain_bytes(KOREDefinition *definition, KOREVariablePattern *var) {
  auto const &contains = definition->getSortContains();
  auto const &var_sort = var->getSort();

  if (contains.find(var_sort.get()) == contains.end()) {
    return false;
  }

  auto const &possible_contains = contains.at(var_sort.get());
  return std::any_of(
      possible_contains.begin(), possible_contains.end(),
      [&](auto *sort) { return is_hooked_to_bytes(definition, sort); });
}

/*
 * Compute the set of variables that need to be marked as copy-on-write in the
 * RHS of a rule. For a variable to be CoW, the following conditions need to be
 * met:
 *   - It is, or can syntactically contain, a term that is of a sort hooked to
 *     the backend's internal implementation of mutable byte buffers.
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
    if (can_contain_bytes(definition, var)) {
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
  auto make_cow_rec = mod->getOrInsertFunction("make_copy_on_write_rec", fn_ty);

  for (auto const &v : vars) {
    auto const &var_sort = all_vars.at(v)->getSort();
    auto actual_fn = is_hooked_to_bytes(definition, var_sort.get())
                         ? make_cow
                         : make_cow_rec;

#if LLVM_VERSION_MAJOR >= 17
    llvm::CallInst::Create(actual_fn, {subst.at(v)}, "cow_" + v, insertAtEnd);
#else
    if (subst.find(v) != subst.end()) {
      llvm::CallInst::Create(
          actual_fn, {subst.lookup(v)}, "cow_" + v, insertAtEnd);
    } else {
      assert(false && "Missing substitution for variable");
    }
#endif
  }
}

} // namespace kllvm
