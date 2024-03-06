#include <kllvm/ast/AST.h>
#include <kllvm/ast/pattern_matching.h>

using namespace kllvm;
using namespace kllvm::pattern_matching;
using namespace kllvm::pattern_matching::literals;

namespace {

auto implies = R"(\implies)"_p;
auto and_ = R"(\and)"_p;
auto equals_ = R"(\equals)"_p;
auto not_ = R"(\not)"_p;
auto rewrites = R"(\rewrites)"_p;
auto top = R"(\top)"_p;
auto X = subject(any);

/*
 * get_patterns(\top()) = []
 * get_patterns(\and(\in(_, X), Y)) = X : get_patterns(Y)
 */
std::vector<kore_pattern *>
get_patterns_impl(kore_pattern *pat, std::vector<kore_pattern *> &result) {
  if (auto *composite = dynamic_cast<kore_composite_pattern *>(pat)) {
    if (composite->get_constructor()->get_name() == "\\top"
        && composite->get_arguments().empty()) {
      return result;
    }
    if (composite->get_constructor()->get_name() == "\\and"
        && composite->get_arguments().size() == 2) {
      if (auto *first_child = dynamic_cast<kore_composite_pattern *>(
              composite->get_arguments()[0].get())) {
        if (first_child->get_constructor()->get_name() == "\\in"
            && first_child->get_arguments().size() == 2) {
          result.push_back(first_child->get_arguments()[1].get());
          return get_patterns_impl(composite->get_arguments()[1].get(), result);
        }
      }
    }
  }
  assert(false && "could not read \\in patterns on lhs");
  abort();
}

std::optional<std::vector<kore_pattern *>>
get_patterns(std::shared_ptr<kore_pattern> const &term) {
  auto result = std::vector<kore_pattern *>{};
  return get_patterns_impl(term.get(), result);
}

/*
 * get_builtin(_(X, Y)) = if X is not a builtin then X else Y
 */
std::optional<std::shared_ptr<kore_pattern>>
get_builtin(std::shared_ptr<kore_pattern> const &term) {
  auto comp = std::dynamic_pointer_cast<kore_composite_pattern>(term);
  if (!comp) {
    return std::nullopt;
  }

  auto lhs = std::dynamic_pointer_cast<kore_composite_pattern>(
      comp->get_arguments()[0]);
  if (!lhs) {
    return std::nullopt;
  }

  if (!lhs->get_constructor()->is_builtin()) {
    return lhs;
  }
  return comp->get_arguments()[1];
}

[[maybe_unused]] std::optional<std::vector<kore_pattern *>>
get_singleton(std::shared_ptr<kore_pattern> const &term) {
  return std::vector{term.get()};
}

std::optional<std::vector<kore_pattern *>>
get_arguments(std::shared_ptr<kore_pattern> const &term) {
  if (auto comp = std::dynamic_pointer_cast<kore_composite_pattern>(term)) {
    auto result = std::vector<kore_pattern *>{};
    for (auto const &arg : comp->get_arguments()) {
      result.push_back(arg.get());
    }
    return result;
  }

  return std::nullopt;
}

} // namespace

/*
 *  0: lhs(\rewrites(\and(\equals(_, _), X), _)) = [X]
 *  1: lhs(\rewrites(\and(X, \equals(_, _)), _)) = [X]
 *  2: lhs(\rewrites(\and(\top(), X), _)) = [X]
 *  3: lhs(\rewrites(\and(X, \top()), _)) = [X]
 *  4: lhs(\rewrites(\and(\not(_), \and(\equals(_, _), X)), _)) = [X]
 *  5: lhs(\rewrites(\and(\not(_), \and(\top(), X)), _)) = [X]
 *  6: lhs(\equals(_(Xs), _)) = Xs
 *  7: lhs(\implies(\and(\equals(_, _), X), _)) = get_patterns(X)
 *  8: lhs(\implies(\and(\top(), X), _)) = get_patterns(X)
 *  9: lhs(\implies(\and(\not(_), \and(\equals(_, _), X)), _)) = get_patterns(X)
 * 10: lhs(\implies(\and(\not(_), \and(\top(), X)), _)) = get_patterns(X)
 * 11: lhs(\implies(\top(), \equals(_(Xs), _))) = Xs
 * 12: lhs(\implies(\equals(_, _), \equals(_(Xs), _))) = Xs
 */
std::vector<kore_pattern *> kore_axiom_declaration::get_left_hand_side() const {
  auto p0 = rewrites(and_(equals_(any, any), X), any);
  auto p1 = rewrites(and_(X, equals_(any, any)), any);
  auto p2 = rewrites(and_(top(), X), any);
  auto p3 = rewrites(and_(X, top()), any);
  auto p4 = rewrites(and_(not_(any), and_(equals_(any, any), X)), any);
  auto p5 = rewrites(and_(not_(any), and_(top(), X)), any);
  auto p6 = equals_(X, any);
  auto p7 = implies(and_(equals_(any, any), X), any);
  auto p8 = implies(and_(top(), X), any);
  auto p9 = implies(and_(not_(any), and_(equals_(any, any), X)), any);
  auto p10 = implies(and_(not_(any), and_(top(), X)), any);
  auto p11 = implies(top(), equals_(X, any));
  auto p12 = implies(equals_(any, any), equals_(X, any));

  auto patterns = match_first(
      map(p0, get_singleton), map(p1, get_singleton), map(p2, get_singleton),
      map(p3, get_singleton), map(p4, get_singleton), map(p5, get_singleton),
      map(p6, get_arguments), map(p7, get_patterns), map(p8, get_patterns),
      map(p9, get_patterns), map(p10, get_patterns), map(p11, get_arguments),
      map(p12, get_arguments));

  auto [any_match, result] = patterns.match(pattern_);

  if (result) {
    return *result;
  }

  assert(false && "could not compute left hand side of axiom");
  abort();
}

/*
 * 0: rhs(\implies(_, \equals(_, \and(X, _)))) = X
 * 1: rhs(\equals(_, X)) = X
 * 2: rhs(\rewrites(_, \and(X, Y))) = get_builtin(\and(X, Y))
 */
kore_pattern *kore_axiom_declaration::get_right_hand_side() const {
  auto p0 = implies(any, equals_(any, and_(X, any)));
  auto p1 = equals_(any, X);
  auto p2 = rewrites(any, subject(and_(any, any)));

  auto patterns = match_first(p0, p1, map(p2, get_builtin));
  auto [any_match, result] = patterns.match(pattern_);

  if (result) {
    return result->get();
  }

  assert(false && "Invalid axiom");
  abort();
}

/**
 *  0: requires(\implies(\and(\not(_), \and(\top(), _)), _)) = nullptr
 *  1: requires(\implies(\and(\not(_), \and(\equals(X, _), _)), _)) = X
 *  2: requires(\implies(\and(\top(), _), _)) = nullptr
 *  3: requires(\implies(\and(\equals(X, _), _), _)) = X
 *  4: requires(\equals(_, _)) = nullptr
 *  5: requires(\rewrites(\and(\equals(X, _), _), _)) = X
 *  6: requires(\rewrites(\and(\top(), _), _)) = nullptr
 *  7: requires(\rewrites(\and(\not(_), \and(\equals(X, _), _)), _) = X
 *  8: requires(\rewrites(\and(\not(_), \and(\top(), _)), _) = nullptr
 *  9: requires(\rewrites(\and(_, \equals(X, _)), _)) = X
 * 10: requires(\rewrites(\and(_, \top()), _)) = nullptr
 */
kore_pattern *kore_axiom_declaration::get_requires() const {
  auto p0 = implies(and_(not_(any), and_(top(), any)), any);
  auto p1 = implies(and_(not_(any), and_(equals_(X, any), any)), any);
  auto p2 = implies(and_(top(), any), any);
  auto p3 = implies(and_(equals_(X, any), any), any);
  auto p4 = equals_(any, any);
  auto p5 = rewrites(and_(equals_(X, any), any), any);
  auto p6 = rewrites(and_(top(), any), any);
  auto p7 = rewrites(and_(not_(any), and_(equals_(X, any), any)), any);
  auto p8 = rewrites(and_(not_(any), and_(top(), any)), any);
  auto p9 = rewrites(and_(any, equals_(X, any)), any);
  auto p10 = rewrites(and_(any, top()), any);

  auto patterns = match_first(p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10);
  auto [any_match, result] = patterns.match(pattern_);

  if (result) {
    return result->get();
  }

  if (any_match) {
    return nullptr;
  }

  assert(false && "Invalid axiom");
  abort();
}

/*
 * strip(rawTerm{}(inj{S, SortKItem{}}(X))) = X
 */
sptr<kore_pattern> kllvm::strip_raw_term(sptr<kore_pattern> const &term) {
  auto [success, inner] = "rawTerm"_p("inj"_p(subject(any))).match(term);
  if (success && inner) {
    return inner;
  }

  return term;
}
