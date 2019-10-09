#ifndef DECISION_PARSER_H
#define DECISION_PARSER_H

#include <string>

#include "kllvm/ast/AST.h"
#include "llvm/ADT/StringMap.h"

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

DecisionNode *parseYamlDecisionTreeFromString(std::string yaml, const std::map<std::string, KORESymbol *> &syms, const std::map<std::string, sptr<KORECompositeSort>> &sorts);
DecisionNode *parseYamlDecisionTree(std::string filename, const std::map<std::string, KORESymbol *> &syms, const std::map<std::string, sptr<KORECompositeSort>> &sorts);
PartialStep parseYamlSpecialDecisionTree(std::string filename, const std::map<std::string, KORESymbol *> &syms, const std::map<std::string, sptr<KORECompositeSort>> &sorts);

}

#endif // DECISION_PARSER_H
