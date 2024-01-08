#include <kllvm/binary/ProofTraceParser.h>

#include <llvm/Support/CommandLine.h>

#include <string>

using namespace llvm;
using namespace kllvm;

cl::OptionCategory KoreProofTraceCat("kore-proof-trace options");

cl::opt<std::string> InputFilename(
    cl::Positional, cl::desc("<input file>"), cl::Required,
    cl::cat(KoreProofTraceCat));

cl::opt<bool> VerboseOutput(
    "verbose",
    llvm::cl::desc("Print verbose information about the input proof trace"),
    llvm::cl::cat(KoreProofTraceCat));

int main(int argc, char **argv) {
  cl::HideUnrelatedOptions({&KoreProofTraceCat});
  cl::ParseCommandLineOptions(argc, argv);

  ProofTraceParser Parser(VerboseOutput);
  auto Trace = Parser.parse_proof_trace_from_file(InputFilename);
  if (Trace.has_value()) {
    return 0;
  }

  return 1;
}
