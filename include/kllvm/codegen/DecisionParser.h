#ifndef DECISION_PARSER_H
#define DECISION_PARSER_H

#include "kllvm/ast/AST.h"

#include <llvm/IR/Module.h>

#include <map>
#include <string>
#include <vector>

namespace kllvm {

class DecisionNode;

struct Residual {
  std::string occurrence;
  KOREPattern *pattern;
};

struct PartialStep {
  DecisionNode *dt;
  std::vector<Residual> residuals;
};

DecisionNode *parseYamlDecisionTreeFromString(
    llvm::Module *, std::string const &yaml,
    std::map<std::string, KORESymbol *> const &syms,
    std::map<ValueType, sptr<KORECompositeSort>> const &sorts);
DecisionNode *parseYamlDecisionTree(
    llvm::Module *, std::string const &filename,
    std::map<std::string, KORESymbol *> const &syms,
    std::map<ValueType, sptr<KORECompositeSort>> const &sorts);
PartialStep parseYamlSpecialDecisionTree(
    llvm::Module *, std::string const &filename,
    std::map<std::string, KORESymbol *> const &syms,
    std::map<ValueType, sptr<KORECompositeSort>> const &sorts);

} // namespace kllvm

#endif // DECISION_PARSER_H
