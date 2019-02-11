#ifndef DECISION_PARSER_H
#define DECISION_PARSER_H

#include "kllvm/codegen/Decision.h"

namespace kllvm {

DecisionNode *parseYamlDecisionTreeFromString(std::string yaml, const std::map<std::string, KOREObjectSymbol *> &syms, const std::map<std::string, KOREObjectCompositeSort *> &sorts);
DecisionNode *parseYamlDecisionTree(std::string filename, const std::map<std::string, KOREObjectSymbol *> &syms, const std::map<std::string, KOREObjectCompositeSort *> &sorts);

}

#endif // DECISION_PARSER_H
