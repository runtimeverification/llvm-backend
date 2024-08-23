#ifndef KLLVM_PROOF_TRACE_UTILS_CPP
#define KLLVM_PROOF_TRACE_UTILS_CPP

#include <kllvm/ast/AST.h>
#include <kllvm/binary/ProofTraceParser.h>

namespace kllvm {

/*
 * This file contains utility functions that are used to pretty print
 * the Proof Trace and manipulate its data structures.
*/

std::vector<int> parse_relative_location(std::string location);

sptr<kore_composite_pattern> replace_argument(
    sptr<kore_composite_pattern> &current_config,
    sptr<kore_composite_pattern> &function, std::vector<int> positions);

llvm_event *build_post_function_event(
    sptr<kore_composite_pattern> current_config,
    sptr<llvm_function_event> &function_event, bool expand_terms);

} // namespace kllvm

#endif // KLLVM_PROOF_TRACE_UTILS_CPP
