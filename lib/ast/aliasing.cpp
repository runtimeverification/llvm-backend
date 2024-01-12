#include "kllvm/ast/AST.h"

#include <numeric>
#include <unordered_map>
#include <unordered_set>

using namespace kllvm;

size_t KOREStringPattern::countOccurrences(std::string const &) const {
  return 0;
}

size_t KOREVariablePattern::countOccurrences(std::string const &name) const {
  return getName() == name ? 1 : 0;
}

size_t KORECompositePattern::countOccurrences(std::string const &name) const {
  return std::accumulate(
      arguments.begin(), arguments.end(), std::size_t{0},
      [&](auto acc, auto const &child) {
        return acc + child->countOccurrences(name);
      });
}

AliasInfo KOREPattern::aliasSets() {
  std::map<std::string, KOREVariablePattern *> vars;
  markVariables(vars);

  auto ret = AliasInfo{};

  for (auto const &[from, _] : vars) {
    ret.try_emplace(from);

    for (auto const &[to, _] : vars) {
      ret.at(from).insert(to);
    }
  }

  return ret;
}

AliasInfo KOREAxiomDeclaration::aliasSets() {
  auto ret = AliasInfo{};

  for (auto *lhs : getLeftHandSide()) {
    auto lhs_aliases = lhs->aliasSets();

    for (auto const &[from, alias_set] : lhs_aliases) {
      ret.try_emplace(from);
      for (auto const &to : alias_set) {
        ret.at(from).insert(to);
      }
    }
  }

  return ret;
}
