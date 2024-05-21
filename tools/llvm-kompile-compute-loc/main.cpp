#include <kllvm/ast/AST.h>
#include <kllvm/ast/util.h>
#include <kllvm/parser/KOREParser.h>

#include <llvm/Support/CommandLine.h>

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>

using namespace llvm;
using namespace kllvm;

cl::OptionCategory loc_cat("llvm-kompile-compute-loc options");

cl::opt<std::string> kompiled_dir(
    cl::Positional, cl::desc("<kompiled-dir>"), cl::Required, cl::cat(loc_cat));

cl::opt<int> ordinal(
    cl::Positional, cl::desc("<ordinal>"), cl::Required, cl::cat(loc_cat));

cl::opt<bool> is_k_line(
    "k-line",
    cl::desc("The tool will look for the line passed as an argument in the K "
             "definition"),
    cl::init(false), cl::cat(loc_cat));

std::optional<int64_t>
get_k_location(std::string &definition, int const &ordinal) {
  // Parse the definition.kore to get the AST.
  kllvm::parser::kore_parser parser(definition);
  auto kore_ast = parser.definition();
  kore_ast->preprocess();

  auto axiom = kore_ast->get_axiom_by_ordinal(ordinal);
  return get_start_line_location(axiom);
}

std::optional<int64_t>
get_kore_location(std::string &definition, int const &ordinal) {

  int64_t line_num = 0;
  int64_t ordinal_num = 0;

  std::ifstream file(definition);
  std::string line;

  while (std::getline(file, line)) {
    line_num++;
    line = trim(line);
    if (line.starts_with("axiom")) {
      if (ordinal_num == ordinal) {
        return line_num;
      }
      ordinal_num++;
    }
  }

  return std::nullopt;
}

int main(int argc, char **argv) {
  cl::HideUnrelatedOptions({&loc_cat});
  cl::ParseCommandLineOptions(argc, argv);

  auto definition = kompiled_dir + "/definition.kore";

  auto location = is_k_line ? get_k_location(definition, ordinal)
                            : get_kore_location(definition, ordinal);

  if (location) {
    std::cout << *location << "\n";
    return 0;
  }

  return 1;
}
