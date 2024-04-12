#include <kllvm/binary/ProofTraceParser.h>

#include <llvm/Support/CommandLine.h>

#include <string>

using namespace llvm;
using namespace kllvm;

cl::OptionCategory kore_proof_trace_test_cat("kore-proof-trace options");

cl::opt<std::string> input_filename(
    cl::Positional, cl::desc("<input file>"), cl::Required,
    cl::cat(kore_proof_trace_test_cat));

cl::opt<bool> slow(
    "slow",
    llvm::cl::desc(
        "test proof trace generated with slow instrumentation option"),
    cl::cat(kore_proof_trace_test_cat));

int main(int argc, char **argv) {
  cl::HideUnrelatedOptions({&kore_proof_trace_test_cat});
  cl::ParseCommandLineOptions(argc, argv);

  proof_trace_parser parser(false, false);
  auto trace = parser.parse_proof_trace_from_file(input_filename);
  if (!trace.has_value()) {
    return 1;
  }

  // check that there is a initial configuration
  if (!(trace->get_initial_config().is_pattern()
        && trace->get_initial_config().getkore_pattern())) {
    return 1;
  }

  // check that the trace after the initial configuration has correct length
  if (slow && trace->get_trace().size() != 4U) {
    return 1;
  }
  if (!slow && trace->get_trace().size() != 3U) {
    return 1;
  }

  // check that the first event is the rewrite a() => b()
  auto const rule1 = std::dynamic_pointer_cast<llvm_rule_event>(
      trace->get_trace()[0].get_step_event());
  if (!rule1) {
    return 1;
  }
  if (rule1->get_rule_ordinal() != 95) {
    return 1;
  }

  // only in slow instrumentation, check that the next event is a configuration
  if (slow
      && !(
          trace->get_trace()[1].is_pattern()
          && trace->get_trace()[1].getkore_pattern())) {
    return 1;
  }

  // check that the next event is the rewrite b() => c()
  auto const rule2 = std::dynamic_pointer_cast<llvm_rule_event>(
      trace->get_trace()[slow ? 2 : 1].get_step_event());
  if (!rule2) {
    return 1;
  }
  if (rule2->get_rule_ordinal() != 96) {
    return 1;
  }

  // check that the last event is a configuration
  if (!(trace->get_trace()[slow ? 3 : 2].is_pattern()
        && trace->get_trace()[slow ? 3 : 2].getkore_pattern())) {
    return 1;
  }

  return 0;
}
