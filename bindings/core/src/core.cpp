#include <kllvm/bindings/core/core.h>

using namespace kllvm;

/*
 * These declarations are internal to the backend and aren't exposed explicitly
 * through any header files, so we pull them in manually here.
 */
extern "C" {
void *construct_initial_configuration(kore_pattern const *);
}

namespace kllvm::bindings {

std::string return_sort_for_label(std::string const &label) {
  auto tag = get_tag_for_symbol_name(label.c_str());
  return get_return_sort_for_tag(tag);
}

std::shared_ptr<kore_pattern> make_raw_term(
    std::shared_ptr<kore_pattern> const &term,
    std::shared_ptr<kore_sort> const &from,
    std::shared_ptr<kore_sort> const &to) {
  auto inj = make_injection(term, from, to);

  auto raw_term_sym = kore_symbol::create("rawTerm");

  auto raw_term = kore_composite_pattern::create(std::move(raw_term_sym));
  raw_term->add_argument(inj);

  return raw_term;
}

std::shared_ptr<kore_pattern> make_injection(
    std::shared_ptr<kore_pattern> const &term,
    std::shared_ptr<kore_sort> const &from,
    std::shared_ptr<kore_sort> const &to) {
  auto inj_sym = kore_symbol::create("inj");

  inj_sym->add_formal_argument(from);
  inj_sym->add_formal_argument(to);

  auto inj = kore_composite_pattern::create(std::move(inj_sym));
  inj->add_argument(term);

  return inj;
}

block *construct_term(std::shared_ptr<kore_pattern> const &pattern) {
  return static_cast<block *>(construct_initial_configuration(pattern.get()));
}

std::shared_ptr<kore_pattern> term_to_pattern(block *term) {
  return term_to_kore_pattern(term);
}

bool get_bool(block *term) {
  assert((((uintptr_t)term) & 1) == 0);
  return *(bool *)term->children;
}

bool simplify_to_bool(std::shared_ptr<kore_pattern> const &pattern) {
  auto bool_sort = kore_composite_sort::create("SortBool");
  auto kitem_sort = kore_composite_sort::create("SortKItem");

  auto inj = make_injection(pattern, bool_sort, kitem_sort);
  return get_bool(construct_term(inj));
}

block *simplify_to_term(
    std::shared_ptr<kore_pattern> const &pattern,
    std::shared_ptr<kore_sort> const &sort) {
  auto kitem_sort = kore_composite_sort::create("SortKItem");

  if (is_sort_kitem(sort) || is_sort_k(sort)) {
    return construct_term(pattern);
  }
  auto raw_term = make_raw_term(pattern, sort, kitem_sort);
  return construct_term(raw_term);
}

std::shared_ptr<kore_pattern> simplify(
    std::shared_ptr<kore_pattern> const &pattern,
    std::shared_ptr<kore_sort> const &sort) {
  return term_to_pattern(simplify_to_term(pattern, sort));
}

std::shared_ptr<kore_pattern>
evaluate_function(std::shared_ptr<kore_composite_pattern> const &term) {
  auto term_args = std::vector<void *>{};
  for (auto const &arg : term->get_arguments()) {
    term_args.push_back(static_cast<void *>(construct_term(arg)));
  }

  auto label = ast_to_string(*term->get_constructor());
  auto tag = get_tag_for_symbol_name(label.c_str());
  auto const *return_sort = get_return_sort_for_tag(tag);
  auto *result = evaluate_function_symbol(tag, term_args.data());

  return sorted_term_to_kore_pattern(static_cast<block *>(result), return_sort);
}

bool is_sort_kitem(std::shared_ptr<kore_sort> const &sort) {
  if (auto composite = std::dynamic_pointer_cast<kore_composite_sort>(sort)) {
    return composite->get_name() == "SortKItem";
  }

  return false;
}

bool is_sort_k(std::shared_ptr<kore_sort> const &sort) {
  if (auto composite = std::dynamic_pointer_cast<kore_composite_sort>(sort)) {
    return composite->get_name() == "SortK";
  }

  return false;
}

} // namespace kllvm::bindings
