#include <kllvm/binary/ProofTraceParser.h>

#include <llvm/Support/CommandLine.h>

#include <string>

using namespace llvm;
using namespace kllvm;

cl::OptionCategory KoreProofTraceTestCat("kore-proof-trace options");

cl::opt<std::string> InputFilename(
    cl::Positional, cl::desc("<input file>"), cl::Required,
    cl::cat(KoreProofTraceTestCat));

int main(int argc, char **argv) {
  cl::HideUnrelatedOptions({&KoreProofTraceTestCat});
  cl::ParseCommandLineOptions(argc, argv);

  ProofTraceParser Parser(false);
  auto Trace = Parser.parse_proof_trace_from_file(InputFilename);
  if (!Trace.has_value()) {
    return 1;
  }

  // check that there is a initial configuration
  if (!(Trace->getInitialConfig().isPattern()
        && Trace->getInitialConfig().getKOREPattern())) {
    return 1;
  }

  // check that the trace after the initial configuration is 4 events long
  if (Trace->getTrace().size() != 4U) {
    return 1;
  }

  // check that the first event is the rewrite a() => b()
  const auto Rule1 = std::dynamic_pointer_cast<LLVMRuleEvent>(
      Trace->getTrace()[0].getStepEvent());
  if (!Rule1) {
    return 1;
  }
  if (Rule1->getRuleOrdinal() != 95) {
    return 1;
  }

  // check that the second event is a configuration
  if (!(Trace->getTrace()[1].isPattern()
        && Trace->getTrace()[1].getKOREPattern())) {
    return 1;
  }

  // check that the third event is the rewrite b() => c()
  const auto Rule2 = std::dynamic_pointer_cast<LLVMRuleEvent>(
      Trace->getTrace()[2].getStepEvent());
  if (!Rule2) {
    return 1;
  }
  if (Rule2->getRuleOrdinal() != 96) {
    return 1;
  }

  // check that the fourth event is a configuration
  if (!(Trace->getTrace()[3].isPattern()
        && Trace->getTrace()[3].getKOREPattern())) {
    return 1;
  }

  return 0;
}
