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

  return 0;
}
