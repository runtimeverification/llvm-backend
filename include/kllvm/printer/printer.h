#ifndef KORE_PRINTER_H
#define KORE_PRINTER_H

#include "kllvm/parser/KOREParser.h"

#include <iostream>

namespace kllvm {

sptr<KOREPattern> addBrackets(sptr<KOREPattern>, PrettyPrintData const &);

std::ostream &printKORE(
    std::ostream &os, std::string const &definitionPath,
    std::string const &patternPath, bool hasColor, bool filterSubst);

} // namespace kllvm

#endif
