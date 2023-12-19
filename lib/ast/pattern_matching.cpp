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
 * getPatterns(\top()) = []
 * getPatterns(\and(\in(_, X), Y) = X : getPatterns(Y)
 */
std::vector<KOREPattern *>
getPatterns(KOREPattern *pat, std::vector<KOREPattern *> &result) {
  if (auto composite = dynamic_cast<KORECompositePattern *>(pat)) {
    if (composite->getConstructor()->getName() == "\\top"
        && composite->getArguments().size() == 0) {
      return result;
    } else if (
        composite->getConstructor()->getName() == "\\and"
        && composite->getArguments().size() == 2) {
      if (auto firstChild = dynamic_cast<KORECompositePattern *>(
              composite->getArguments()[0].get())) {
        if (firstChild->getConstructor()->getName() == "\\in"
            && firstChild->getArguments().size() == 2) {
          result.push_back(firstChild->getArguments()[1].get());
          return getPatterns(composite->getArguments()[1].get(), result);
        }
      }
    }
  }
  assert(false && "could not read \\in patterns on lhs");
  abort();
}

/*
 * getBuiltin(_(X, Y)) = if X is not a builtin then X else Y
 */
std::optional<std::shared_ptr<KOREPattern>>
getBuiltin(std::shared_ptr<KOREPattern> const &term) {
  auto comp = std::dynamic_pointer_cast<KORECompositePattern>(term);
  if (!comp) {
    return std::nullopt;
  }

  auto lhs = std::dynamic_pointer_cast<KORECompositePattern>(
      comp->getArguments()[0]);
  if (!lhs) {
    return std::nullopt;
  }

  if (!lhs->getConstructor()->isBuiltin()) {
    return lhs;
  } else {
    return comp->getArguments()[1];
  }
}

} // namespace

/*
 * lhs(\rewrites(\and(\equals(_, _), X), _)) = [X]
 * lhs(\rewrites(\and(X, \equals(_, _)), _)) = [X]
 * lhs(\rewrites(\and(\top(), X), _)) = [X]
 * lhs(\rewrites(\and(X, \top()), _)) = [X]
 * lhs(\rewrites(\and(\not(_), \and(\equals(_, _), X)), _)) = [X]
 * lhs(\rewrites(\and(\not(_), \and(\top(), X)), _)) = [X]
 * lhs(\equals(_(Xs), _)) = Xs
 * lhs(\implies(\and(\equals(_, _), X), _)) = getPatterns(X)
 * lhs(\implies(\and(\top(), X), _)) = getPatterns(X)
 * lhs(\implies(\and(\not(_), \and(\equals(_, _), X)), _)) = getPatterns(X)
 * lhs(\implies(\and(\not(_), \and(\top(), X)), _)) = getPatterns(X)
 * lhs(\implies(\top(), \equals(_(Xs), _))) = Xs
 * lhs(\implies(\equals(_, _), \equals(_(Xs), _))) = Xs
 */
std::vector<KOREPattern *> KOREAxiomDeclaration::getLeftHandSide() const {
  if (auto top = dynamic_cast<KORECompositePattern *>(pattern.get())) {
    if (top->getConstructor()->getName() == "\\rewrites"
        && top->getArguments().size() == 2) {
      if (auto andPattern = dynamic_cast<KORECompositePattern *>(
              top->getArguments()[0].get())) {
        if (andPattern->getConstructor()->getName() == "\\and"
            && andPattern->getArguments().size() == 2) {
          if (auto firstChild = dynamic_cast<KORECompositePattern *>(
                  andPattern->getArguments()[0].get())) {
            if (firstChild->getConstructor()->getName() == "\\equals"
                && firstChild->getArguments().size() == 2) {
              return {andPattern->getArguments()[1].get()};
            } else if (
                firstChild->getConstructor()->getName() == "\\top"
                && firstChild->getArguments().size() == 0) {
              return {andPattern->getArguments()[1].get()};
            } else {
              if (auto secondChild = dynamic_cast<KORECompositePattern *>(
                      andPattern->getArguments()[1].get())) {
                if (secondChild->getConstructor()->getName() == "\\equals"
                    && secondChild->getArguments().size() == 2) {
                  return {firstChild};
                } else if (
                    secondChild->getConstructor()->getName() == "\\top"
                    && secondChild->getArguments().size() == 0) {
                  return {firstChild};
                } else {
                  if (firstChild->getConstructor()->getName() == "\\not"
                      && firstChild->getArguments().size() == 1
                      && secondChild->getConstructor()->getName() == "\\and"
                      && secondChild->getArguments().size() == 2) {
                    if (auto inner = dynamic_cast<KORECompositePattern *>(
                            secondChild->getArguments()[0].get())) {
                      if (inner->getConstructor()->getName() == "\\equals"
                          && inner->getArguments().size() == 2) {
                        return {secondChild->getArguments()[1].get()};
                      } else if (
                          inner->getConstructor()->getName() == "\\top"
                          && inner->getArguments().size() == 0) {
                        return {secondChild->getArguments()[1].get()};
                      }
                    }
                  }
                }
              }
            }
          }
        }
      }
    } else if (
        top->getConstructor()->getName() == "\\equals"
        && top->getArguments().size() == 2) {
      if (auto firstChild = dynamic_cast<KORECompositePattern *>(
              top->getArguments()[0].get())) {
        std::vector<KOREPattern *> result;
        for (auto &sptr : firstChild->getArguments()) {
          result.push_back(sptr.get());
        }
        return result;
      }
    } else if (
        top->getConstructor()->getName() == "\\implies"
        && top->getArguments().size() == 2) {
      if (auto firstChild = dynamic_cast<KORECompositePattern *>(
              top->getArguments()[0].get())) {
        if (firstChild->getConstructor()->getName() == "\\and"
            && firstChild->getArguments().size() == 2) {
          auto lhsAnd = firstChild;
          if (auto innerFirst = dynamic_cast<KORECompositePattern *>(
                  firstChild->getArguments()[0].get())) {
            if (innerFirst->getConstructor()->getName() == "\\not"
                && innerFirst->getArguments().size() == 1) {
              if (auto innerSecond = dynamic_cast<KORECompositePattern *>(
                      firstChild->getArguments()[1].get())) {
                lhsAnd = innerSecond;
              }
            }
          }
          if (auto sideCondition = dynamic_cast<KORECompositePattern *>(
                  lhsAnd->getArguments()[0].get())) {
            if (sideCondition->getConstructor()->getName() == "\\equals"
                && sideCondition->getArguments().size() == 2) {
              std::vector<KOREPattern *> result;
              return getPatterns(lhsAnd->getArguments()[1].get(), result);
            } else if (
                sideCondition->getConstructor()->getName() == "\\top"
                && sideCondition->getArguments().size() == 0) {
              std::vector<KOREPattern *> result;
              return getPatterns(lhsAnd->getArguments()[1].get(), result);
            }
          }
        } else if (
            (firstChild->getConstructor()->getName() == "\\top"
             && firstChild->getArguments().size() == 0)
            || (firstChild->getConstructor()->getName() == "\\equals"
                && firstChild->getArguments().size() == 2)) {
          if (auto secondChild = dynamic_cast<KORECompositePattern *>(
                  top->getArguments()[1].get())) {
            if (secondChild->getConstructor()->getName() == "\\equals"
                && secondChild->getArguments().size() == 2) {
              if (auto lhs = dynamic_cast<KORECompositePattern *>(
                      secondChild->getArguments()[0].get())) {
                std::vector<KOREPattern *> result;
                for (auto &sptr : lhs->getArguments()) {
                  result.push_back(sptr.get());
                }
                return result;
              }
            }
          }
        }
      }
    }
  }
  assert(false && "could not compute left hand side of axiom");
  abort();
}

/*
 * 0: rhs(\implies(_, \equals(_, \and(X, _)))) = X
 * 1: rhs(\equals(_, X)) = X
 * 2: rhs(\rewrites(_, \and(X, Y))) = getBuiltin(\and(X, Y))
 */
KOREPattern *KOREAxiomDeclaration::getRightHandSide() const {
  auto p0 = implies(any, equals_(any, and_(X, any)));
  auto p1 = equals_(any, X);
  auto p2 = rewrites(any, subject(and_(any, any)));

  auto patterns = match_first(p0, p1, matcher(p2, getBuiltin));
  auto [any_match, result] = patterns.match(pattern);

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
KOREPattern *KOREAxiomDeclaration::getRequires() const {
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
  auto [any_match, result] = patterns.match(pattern);

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
sptr<KOREPattern> kllvm::stripRawTerm(sptr<KOREPattern> const &term) {
  auto [success, inner] = "rawTerm"_p("inj"_p(subject(any))).match(term);
  if (success && inner) {
    return inner;
  }

  return term;
}
