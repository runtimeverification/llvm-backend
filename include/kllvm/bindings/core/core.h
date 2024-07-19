#ifndef BINDINGS_CORE_H
#define BINDINGS_CORE_H

#include <kllvm/ast/AST.h>

#include <memory>
#include <optional>

// These headers need to be included last because they pollute a number of macro
// definitions into the global namespace.
#include <runtime/arena.h>
#include <runtime/header.h>

namespace kllvm::bindings {

std::string return_sort_for_label(std::string const &label);

std::shared_ptr<kllvm::kore_pattern> make_injection(
    std::shared_ptr<kllvm::kore_pattern> const &term,
    std::shared_ptr<kllvm::kore_sort> const &from,
    std::shared_ptr<kllvm::kore_sort> const &to);

block *construct_term(std::shared_ptr<kllvm::kore_pattern> const &pattern);

std::shared_ptr<kllvm::kore_pattern> term_to_pattern(block *term);

bool get_bool(block *term);

bool simplify_to_bool(std::shared_ptr<kllvm::kore_pattern> const &pattern);

block *simplify_to_term(
    std::shared_ptr<kllvm::kore_pattern> const &pattern,
    std::shared_ptr<kllvm::kore_sort> const &sort);

std::shared_ptr<kllvm::kore_pattern> simplify(
    std::shared_ptr<kllvm::kore_pattern> const &pattern,
    std::shared_ptr<kllvm::kore_sort> const &sort);

bool is_sort_kitem(std::shared_ptr<kllvm::kore_sort> const &sort);
bool is_sort_k(std::shared_ptr<kllvm::kore_sort> const &sort);

std::shared_ptr<kore_pattern>
evaluate_function(std::shared_ptr<kore_composite_pattern> const &term);

/**
 * Get the name of the LLVM function that attempts matching on the rule with
 * this label by parsing a KORE definition and examining its axioms.
 */
std::optional<std::string> get_match_function_name(
    std::string const &definition_path, std::string const &label);

} // namespace kllvm::bindings

#endif
