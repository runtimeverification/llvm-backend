#ifndef BINDINGS_CORE_H
#define BINDINGS_CORE_H

#include <kllvm/ast/AST.h>

#include <memory>

// These headers need to be included last because they pollute a number of macro
// definitions into the global namespace.
#include <runtime/arena.h>
#include <runtime/header.h>

namespace kllvm::bindings {

std::string return_sort_for_label(std::string const &label);

std::shared_ptr<kllvm::KOREPattern> make_injection(
    std::shared_ptr<kllvm::KOREPattern> term,
    std::shared_ptr<kllvm::KORESort> from, std::shared_ptr<kllvm::KORESort> to);

block *construct_term(std::shared_ptr<kllvm::KOREPattern> const &pattern);

std::shared_ptr<kllvm::KOREPattern> term_to_pattern(block *term);

bool get_bool(block *term);

bool simplify_to_bool(std::shared_ptr<kllvm::KOREPattern> pattern);

block *simplify_to_term(
    std::shared_ptr<kllvm::KOREPattern> pattern,
    std::shared_ptr<kllvm::KORESort> sort);

std::shared_ptr<kllvm::KOREPattern> simplify(
    std::shared_ptr<kllvm::KOREPattern> pattern,
    std::shared_ptr<kllvm::KORESort> sort);

bool is_sort_kitem(std::shared_ptr<kllvm::KORESort> const &sort);
bool is_sort_k(std::shared_ptr<kllvm::KORESort> const &sort);

std::shared_ptr<kllvm::KOREPattern> evaluate_function(
    std::string const &label,
    std::vector<std::shared_ptr<kllvm::KOREPattern>> const &args);

} // namespace kllvm::bindings

#endif
