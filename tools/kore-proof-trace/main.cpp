#include <kllvm/binary/ProofTraceParser.h>
#include <kllvm/parser/KOREParser.h>

#include <llvm/Support/CommandLine.h>

#include <fcntl.h>
#include <fstream>
#include <string>
#include <sys/mman.h>
#include <sys/stat.h>

using namespace llvm;
using namespace kllvm;

cl::OptionCategory kore_proof_trace_cat("kore-proof-trace options");

cl::opt<std::string> header_path(
    cl::Positional, cl::desc("<header file>"), cl::Required,
    cl::cat(kore_proof_trace_cat));

cl::opt<std::string> input_filename(
    cl::Positional, cl::desc("<input file>"), cl::Required,
    cl::cat(kore_proof_trace_cat));

cl::opt<std::string> kore_filename(
    cl::Positional, cl::desc("[kore definition file]"), cl::Optional,
    cl::cat(kore_proof_trace_cat));

cl::opt<bool> verbose_output(
    "verbose",
    llvm::cl::desc("Print verbose information about the input proof trace"),
    llvm::cl::cat(kore_proof_trace_cat));

cl::opt<bool> intermediate_configs(
    "intermediate-configs",
    llvm::cl::desc(
        "Print intermediate configurations before function evaluations"),
    llvm::cl::cat(kore_proof_trace_cat), llvm::cl::init(false));

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
  auto header = std::make_shared<kore_header>(in);
  fclose(in);

  if (use_streaming_parser) {
    std::ifstream file(input_filename, std::ios_base::binary);
    llvm_rewrite_trace_iterator it(
        std::make_unique<proof_trace_file_buffer>(std::move(file)), header);
    if (verbose_output) {
      it.print(std::cout, expand_terms_in_output, 0U, intermediate_configs);
    }
    return 0;
  }

  std::optional<kore_definition> kore_def;

  if (!kore_filename.empty()) {
    std::fstream kore_file(kore_filename);
    kore_def
        = std::make_optional(*parser::kore_parser(kore_filename).definition());
    kore_def->preprocess();
  }

  proof_trace_parser parser(
      verbose_output, expand_terms_in_output, header, kore_def);

  auto trace = parser.parse_proof_trace_from_file(
      input_filename, intermediate_configs);
  if (trace.has_value()) {
    return 0;
  }

  return 1;
}
