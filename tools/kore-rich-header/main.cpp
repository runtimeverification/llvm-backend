#include <kllvm/ast/AST.h>
#include <kllvm/binary/serializer.h>
#include <kllvm/parser/KOREParser.h>

#include <llvm/Support/CommandLine.h>

#include <cstdint>
#include <fstream>
#include <iostream>

using namespace llvm;
using namespace kllvm;
using namespace kllvm::parser;

cl::OptionCategory rich_header_cat("kore-rich-header options");

cl::opt<std::string> input(
    cl::Positional, cl::desc("<kore-definition>"), cl::Required,
    cl::cat(rich_header_cat));

cl::opt<std::string> output(
    "o", cl::desc("Output file path"), cl::value_desc("filepath"),
    cl::cat(rich_header_cat));

cl::alias output_alias(
    "output", cl::desc("Alias for -o"), cl::value_desc("filepath"),
    cl::cat(rich_header_cat), cl::aliasopt(output));

int main(int argc, char **argv) {
  cl::HideUnrelatedOptions({&rich_header_cat});
  cl::ParseCommandLineOptions(argc, argv);

  kore_parser parser(input);
  auto definition = parser.definition();
  definition->preprocess();

  if (output.empty()) {
    emit_kore_rich_header(std::cout, definition.get());
  } else {
    std::ofstream out(output);
    emit_kore_rich_header(out, definition.get());
  }

  return 0;
}
