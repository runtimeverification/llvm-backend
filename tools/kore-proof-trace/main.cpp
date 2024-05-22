#include <kllvm/binary/ProofTraceParser.h>

#include <llvm/Support/CommandLine.h>

#include <fstream>
#include <string>

using namespace llvm;
using namespace kllvm;

cl::OptionCategory kore_proof_trace_cat("kore-proof-trace options");

cl::opt<std::string> header_path(
    cl::Positional, cl::desc("<header file>"), cl::Required,
    cl::cat(kore_proof_trace_cat));

cl::opt<std::string> input_filename(
    cl::Positional, cl::desc("<input file>"), cl::Required,
    cl::cat(kore_proof_trace_cat));

cl::opt<bool> verbose_output(
    "verbose",
    llvm::cl::desc("Print verbose information about the input proof trace"),
    llvm::cl::cat(kore_proof_trace_cat));

cl::opt<bool> expand_terms_in_output(
    "expand-terms", llvm::cl::desc("Expand KORE terms in the verbose output"),
    llvm::cl::cat(kore_proof_trace_cat));

cl::opt<bool> use_streaming_parser(
    "streaming-parser",
    llvm::cl::desc("Use streaming event parser to parse trace"),
    llvm::cl::cat(kore_proof_trace_cat));

int main(int argc, char **argv) {
  cl::HideUnrelatedOptions({&kore_proof_trace_cat});
  cl::ParseCommandLineOptions(argc, argv);

  FILE *in = fopen(header_path.getValue().c_str(), "r");
  kore_header header(in);
  fclose(in);

  if (use_streaming_parser) {
    std::ifstream file(input_filename, std::ios_base::binary);
    proof_trace_file_buffer buffer(file);
    llvm_rewrite_trace_iterator it(buffer, header);
    if (verbose_output) {
      it.print(std::cout, expand_terms_in_output);
    }
    return 0;
  }

  proof_trace_parser parser(verbose_output, expand_terms_in_output, header);
  auto trace = parser.parse_proof_trace_from_file(input_filename);
  if (trace.has_value()) {
    return 0;
  }

  return 1;
}
