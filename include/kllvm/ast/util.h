#ifndef UTIL_H
#define UTIL_H

#include <kllvm/ast/AST.h>

namespace kllvm {

/*
 * This file contains utility functions that are used in multiple tools and
 * are not specific to any one tool. These functions are used to manipulate
 * the AST and other data structures.
*/

[[nodiscard]] std::optional<std::pair<std::string, uint64_t>>
get_start_line_location(kore_axiom_declaration const &axiom);

[[nodiscard]] std::string trim(std::string s);
} // namespace kllvm

#endif // UTIL_H
