#ifndef KORE_PRINTER_H
#define KORE_PRINTER_H

#include "kllvm/ast/AST.h"

#include <iostream>
#include <string>

namespace kllvm {

sptr<kore_pattern>
add_brackets(sptr<kore_pattern> const &, pretty_print_data const &);

std::ostream &print_kore(
    std::ostream &os, std::string const &definition_path,
    std::string const &pattern_path, bool has_color, bool filter_subst,
    bool pretty = true);

} // namespace kllvm

#endif
