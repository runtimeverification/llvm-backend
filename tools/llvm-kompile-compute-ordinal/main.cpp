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

cl::OptionCategory ordinal_cat("llvm-kompile-compute-ordinal options");

cl::opt<std::string> kompiled_dir(
    cl::Positional, cl::desc("<kompiled-dir>"), cl::Required,
    cl::cat(ordinal_cat));

cl::opt<int> line(
    cl::Positional, cl::desc("<line>"), cl::Required, cl::cat(ordinal_cat));

cl::opt<bool> is_k_line(
    "k-line",
    cl::desc("The tool will look for the line passed as an argument in the K "
             "definition"),
    cl::init(false), cl::cat(ordinal_cat));

std::optional<int64_t>
get_k_ordinal(std::string const &definition, int const &line) {
  // Parse the definition.kore to get the AST.
  kllvm::parser::kore_parser parser(definition);
  auto kore_ast = parser.definition();
  kore_ast->preprocess();

  // Iterate through axioms.
  for (auto *axiom : kore_ast->get_axioms()) {
    if (axiom->attributes().contains(attribute_set::key::Location)) {
      auto start_line = get_start_line_location(*axiom);
      if (start_line == line) {
        return axiom->get_ordinal();
      }
    }
  }

  return std::nullopt;
}

std::optional<int64_t>
get_kore_ordinal(std::string const &definition, int const &line) {

  int64_t line_num = 0;
  int64_t ordinal_num = 0;

  std::ifstream file(definition);
  std::string l;

  while (std::getline(file, l)) {
    line_num++;
    l = trim(l);
    if (l.starts_with("axiom")) {
      if (line_num == line) {
        return ordinal_num;
      }
      ordinal_num++;
    }
  }

  return std::nullopt;
}

int main(int argc, char **argv) {
  cl::HideUnrelatedOptions({&ordinal_cat});
  cl::ParseCommandLineOptions(argc, argv);

  auto definition = kompiled_dir + "/definition.kore";

  auto location = is_k_line ? get_k_ordinal(definition, line)
                            : get_kore_ordinal(definition, line);

  if (location) {
    std::cout << *location << "\n";
    return 0;
  }

  return 1;
}
