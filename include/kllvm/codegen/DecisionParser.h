#ifndef DECISION_PARSER_H
#define DECISION_PARSER_H

#include "kllvm/codegen/Decision.h"

namespace kllvm {

DecisionNode *parseYamlDecisionTreeFromString(std::string yaml, int numSubjects, const llvm::StringMap<KOREObjectSymbol *> &syms);
DecisionNode *parseYamlDecisionTree(std::string filename, int numSubjects, const llvm::StringMap<KOREObjectSymbol *> &syms);

}

#endif // DECISION_PARSER_H
