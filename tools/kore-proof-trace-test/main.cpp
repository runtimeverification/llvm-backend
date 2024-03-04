#include <kllvm/binary/ProofTraceParser.h>

#include <llvm/Support/CommandLine.h>

#include <string>

using namespace llvm;
using namespace kllvm;

cl::OptionCategory kore_proof_trace_test_cat("kore-proof-trace options");

cl::opt<std::string> input_filename(
    cl::Positional, cl::desc("<input file>"), cl::Required,
    cl::cat(kore_proof_trace_test_cat));

int main(int argc, char **argv) {
  cl::HideUnrelatedOptions({&kore_proof_trace_test_cat});
  cl::ParseCommandLineOptions(argc, argv);

  proof_trace_parser Parser(false);
  auto Trace = Parser.parse_proof_trace_from_file(input_filename);
  if (!Trace.has_value()) {
    return 1;
  }

  // check that there is a initial configuration
  if (!(Trace->get_initial_config().is_pattern()
        && Trace->get_initial_config().getkore_pattern())) {
    return 1;
  }

  // check that the trace after the initial configuration is 4 events long
  if (Trace->get_trace().size() != 4U) {
    return 1;
  }

  // check that the first event is the rewrite a() => b()
  auto const Rule1 = std::dynamic_pointer_cast<llvm_rule_event>(
      Trace->get_trace()[0].get_step_event());
  if (!Rule1) {
    return 1;
  }
  if (Rule1->get_rule_ordinal() != 95) {
    return 1;
  }

  // check that the second event is a configuration
  if (!(Trace->get_trace()[1].is_pattern()
        && Trace->get_trace()[1].getkore_pattern())) {
    return 1;
  }

  // check that the third event is the rewrite b() => c()
  auto const Rule2 = std::dynamic_pointer_cast<llvm_rule_event>(
      Trace->get_trace()[2].get_step_event());
  if (!Rule2) {
    return 1;
  }
  if (Rule2->get_rule_ordinal() != 96) {
    return 1;
  }

  // check that the fourth event is a configuration
  if (!(Trace->get_trace()[3].is_pattern()
        && Trace->get_trace()[3].getkore_pattern())) {
    return 1;
  }

  return 0;
}
