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

  ProofTraceParser Parser(false, 3u);
  LLVMRewriteTrace Trace;
  if (!Parser.parse_proof_trace(InputFilename, Trace)) {
    return 1;
  }

  // check that there is a initial configuration
  if (!(Trace.initialConfig.isPattern()
        && Trace.initialConfig.getKOREPattern())) {
    return 1;
  }

  // check that the trace after the initial configuration is 4 events long
  if (Trace.trace.size() != 4u) {
    return 1;
  }

  // check that the first event is the rewrite a() => b()
  const auto *Rule1 = dynamic_cast<LLVMRuleEvent const *>(
      Trace.trace[0].getStepEvent().get());
  if (!Rule1) {
    return 1;
  }
  if (Rule1->getRuleOrdinal() != 95) {
    return 1;
  }

  // check that the second event is a configuration
  if (!(Trace.trace[1].isPattern() && Trace.trace[1].getKOREPattern())) {
    return 1;
  }

  // check that the third event is the rewrite b() => c()
  const auto *Rule2 = dynamic_cast<LLVMRuleEvent const *>(
      Trace.trace[2].getStepEvent().get());
  if (!Rule2) {
    return 1;
  }
  if (Rule2->getRuleOrdinal() != 96) {
    return 1;
  }

  // check that the fourth event is a configuration
  if (!(Trace.trace[3].isPattern() && Trace.trace[3].getKOREPattern())) {
    return 1;
  }

  return 0;
}
