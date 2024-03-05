#include <kllvm/binary/ProofTraceParser.h>

#include <llvm/Support/CommandLine.h>

#include <string>

using namespace llvm;
using namespace kllvm;

cl::OptionCategory kore_proof_trace_cat("kore-proof-trace options");

cl::opt<std::string> input_filename(
    cl::Positional, cl::desc("<input file>"), cl::Required,
    cl::cat(kore_proof_trace_cat));

cl::opt<bool> verbose_output(
    "verbose",
    llvm::cl::desc("Print verbose information about the input proof trace"),
    llvm::cl::cat(kore_proof_trace_cat));

int main(int argc, char **argv) {
  cl::HideUnrelatedOptions({&kore_proof_trace_cat});
  cl::ParseCommandLineOptions(argc, argv);

  proof_trace_parser parser(verbose_output);
  auto trace = parser.parse_proof_trace_from_file(input_filename);
  if (trace.has_value()) {
    return 0;
  }

  return 1;
}
