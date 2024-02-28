#ifndef KORE_PRINTER_H
#define KORE_PRINTER_H

#include "kllvm/ast/AST.h"

#include <iostream>
#include <string>

namespace kllvm {

sptr<kore_pattern>
addBrackets(sptr<kore_pattern> const &, pretty_print_data const &);

std::ostream &printKORE(
    std::ostream &os, std::string const &definitionPath,
    std::string const &patternPath, bool hasColor, bool filterSubst,
    bool pretty = true);

} // namespace kllvm

#endif
