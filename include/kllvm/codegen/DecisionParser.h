#ifndef DECISION_PARSER_H
#define DECISION_PARSER_H

#include "kllvm/ast/AST.h"

#include <llvm/IR/Module.h>

#include <map>
#include <string>
#include <vector>

namespace kllvm {

class decision_node;

struct residual {
  std::string occurrence;
  kore_pattern *pattern{};
};

struct partial_step {
  decision_node *dt{};
  std::vector<residual> residuals;
};

decision_node *parse_yamldecision_tree_from_string(
    llvm::Module *, std::string const &yaml,
    std::map<std::string, kore_symbol *> const &syms,
    std::map<value_type, sptr<kore_composite_sort>> const &sorts);
decision_node *parse_yamldecision_tree(
    llvm::Module *, std::string const &filename,
    std::map<std::string, kore_symbol *> const &syms,
    std::map<value_type, sptr<kore_composite_sort>> const &sorts);
partial_step parse_yaml_specialdecision_tree(
    llvm::Module *, std::string const &filename,
    std::map<std::string, kore_symbol *> const &syms,
    std::map<value_type, sptr<kore_composite_sort>> const &sorts);

} // namespace kllvm

#endif // DECISION_PARSER_H
