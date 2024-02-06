#include <kllvm/bindings/core/core.h>

using namespace kllvm;

/*
 * These declarations are internal to the backend and aren't exposed explicitly
 * through any header files, so we pull them in manually here.
 */
extern "C" {
void *constructInitialConfiguration(KOREPattern const *);
}

namespace kllvm::bindings {

std::string return_sort_for_label(std::string const &label) {
  auto tag = getTagForSymbolName(label.c_str());
  return getReturnSortForTag(tag);
}

std::shared_ptr<KOREPattern> make_rawTerm(
    std::shared_ptr<KOREPattern> const &term,
    std::shared_ptr<KORESort> const &from,
    std::shared_ptr<KORESort> const &to) {
  auto inj = make_injection(term, from, to);

  auto rawTerm_sym = KORESymbol::Create("rawTerm");

  auto rawTerm = KORECompositePattern::Create(std::move(rawTerm_sym));
  rawTerm->addArgument(inj);

  return rawTerm;
}

std::shared_ptr<KOREPattern> make_injection(
    std::shared_ptr<KOREPattern> const &term,
    std::shared_ptr<KORESort> const &from,
    std::shared_ptr<KORESort> const &to) {
  auto inj_sym = KORESymbol::Create("inj");

  inj_sym->addFormalArgument(from);
  inj_sym->addFormalArgument(to);

  auto inj = KORECompositePattern::Create(std::move(inj_sym));
  inj->addArgument(term);

  return inj;
}

block *construct_term(std::shared_ptr<KOREPattern> const &pattern) {
  return static_cast<block *>(constructInitialConfiguration(pattern.get()));
}

std::shared_ptr<KOREPattern> term_to_pattern(block *term) {
  return termToKorePattern(term);
}

bool get_bool(block *term) {
  assert((((uintptr_t)term) & 1) == 0);
  return *(bool *)term->children;
}

bool simplify_to_bool(std::shared_ptr<KOREPattern> const &pattern) {
  auto bool_sort = KORECompositeSort::Create("SortBool");
  auto kitem_sort = KORECompositeSort::Create("SortKItem");

  auto inj = make_injection(pattern, bool_sort, kitem_sort);
  return get_bool(construct_term(inj));
}

block *simplify_to_term(
    std::shared_ptr<KOREPattern> const &pattern,
    std::shared_ptr<KORESort> const &sort) {
  auto kitem_sort = KORECompositeSort::Create("SortKItem");

  if (is_sort_kitem(sort) || is_sort_k(sort)) {
    return construct_term(pattern);
  }
  auto rawTerm = make_rawTerm(pattern, sort, kitem_sort);
  return construct_term(rawTerm);
}

std::shared_ptr<KOREPattern> simplify(
    std::shared_ptr<KOREPattern> const &pattern,
    std::shared_ptr<KORESort> const &sort) {
  return term_to_pattern(simplify_to_term(pattern, sort));
}

std::shared_ptr<KOREPattern>
evaluate_function(std::shared_ptr<KORECompositePattern> const &term) {
  auto term_args = std::vector<void *>{};
  for (auto const &arg : term->getArguments()) {
    term_args.push_back(static_cast<void *>(construct_term(arg)));
  }

  auto label = ast_to_string(*term->getConstructor());
  auto tag = getTagForSymbolName(label.c_str());
  auto const *return_sort = getReturnSortForTag(tag);
  auto *result = evaluateFunctionSymbol(tag, term_args.data());

  return sortedTermToKorePattern(static_cast<block *>(result), return_sort);
}

bool is_sort_kitem(std::shared_ptr<KORESort> const &sort) {
  if (auto composite = std::dynamic_pointer_cast<KORECompositeSort>(sort)) {
    return composite->getName() == "SortKItem";
  }

  return false;
}

bool is_sort_k(std::shared_ptr<KORESort> const &sort) {
  if (auto composite = std::dynamic_pointer_cast<KORECompositeSort>(sort)) {
    return composite->getName() == "SortK";
  }

  return false;
}

} // namespace kllvm::bindings
