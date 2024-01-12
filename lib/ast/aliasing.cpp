#include "kllvm/ast/AST.h"

#include <numeric>
#include <unordered_map>
#include <unordered_set>

using namespace kllvm;

template <typename Container>
AliasInfo mergeAliasSets(Container const &patterns) {
  auto ret = AliasInfo{};

  for (auto const &pat : patterns) {
    auto const &alias_info = pat->aliasSets();
    for (auto const &[from, to_set] : alias_info) {
      ret.try_emplace(from);
      for (auto const &to : to_set) {
        ret.at(from).insert(to);
      }
    }
  }

  return ret;
}

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

AliasInfo KOREStringPattern::aliasSets() {
  return {};
}

AliasInfo KOREVariablePattern::aliasSets() {
  return {{getName(), {getName()}}};
}

AliasInfo KORECompositePattern::aliasSets() {
  if (constructor->getName() == R"(\and)") {
    auto ret = AliasInfo{};

    auto vars = std::map<std::string, KOREVariablePattern *>{};
    markVariables(vars);

    for (auto const &[from, _] : vars) {
      ret.try_emplace(from);

      for (auto const &[to, _] : vars) {
        ret.at(from).insert(to);
      }
    }

    return ret;
  }

  return mergeAliasSets(arguments);
}

AliasInfo KOREAxiomDeclaration::aliasSets() const {
  return mergeAliasSets(getLeftHandSide());
}
