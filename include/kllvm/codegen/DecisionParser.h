#ifndef DECISION_PARSER_H
#define DECISION_PARSER_H

#include "kllvm/codegen/Decision.h"

namespace kllvm {

DecisionNode *parseYamlDecisionTreeFromString(std::string yaml, int numSubjects, const llvm::StringMap<KOREObjectSymbol *> &syms, const llvm::StringMap<KOREObjectCompositeSort *> &sorts);
DecisionNode *parseYamlDecisionTree(std::string filename, int numSubjects, const llvm::StringMap<KOREObjectSymbol *> &syms, const llvm::StringMap<KOREObjectCompositeSort *> &sorts);

}

#endif // DECISION_PARSER_H
