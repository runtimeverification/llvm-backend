#include <kllvm/ast/AST.h>
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

cl::opt<std::string> ordinal(
    cl::Positional, cl::desc("<ordinal>"), cl::Required, cl::cat(loc_cat));

cl::opt<bool> is_k_line(
    cl::Positional, cl::desc("[is-k-line]"), cl::init(false), cl::cat(loc_cat));

int64_t get_location(kore_axiom_declaration *axiom) {
  auto *location_att
      = axiom->attributes().get(attribute_set::key::Location).get();
  assert(location_att->get_arguments().size() == 1);

  auto *str_pattern = dynamic_cast<kore_string_pattern *>(
      location_att->get_arguments()[0].get());
  std::string location = str_pattern->get_contents();

  size_t l_paren = location.find_first_of('(');
  size_t first_comma = location.find_first_of(',');
  size_t length = first_comma - l_paren - 1;
  return std::stoi(location.substr(l_paren + 1, length));
}

int64_t get_k_location(std::string &definition) {
  // Parse the definition.kore to get the AST.
  kllvm::parser::kore_parser parser(definition);
  auto kore_ast = parser.definition();
  kore_ast.get()->preprocess();

  // Iterate through axioms.
  auto *axiom = kore_ast.get()->get_axiom_by_ordinal(std::stoi(ordinal));
  if (axiom) {
    auto start_line = get_location(axiom);
    return start_line;
  }

  return -1;
}

int64_t get_kore_location(std::string &definition) {

  int64_t line_num = 0;
  int64_t ordinal_num = 0;

  std::ifstream file(definition);
  std::string line;

  while (std::getline(file, line)) {
    line_num++;
    if (line.starts_with("  axiom") || line.starts_with("    axiom")) {
      if (ordinal_num == std::stoi(ordinal)) {
        file.close();
        return line_num;
      }
      ordinal_num++;
    }
  }

  file.close();
  return -1;
}

int main(int argc, char **argv) {
  cl::HideUnrelatedOptions({&loc_cat});
  cl::ParseCommandLineOptions(argc, argv);

  auto definition = kompiled_dir + "/definition.kore";

  int64_t location
      = is_k_line ? get_k_location(definition) : get_kore_location(definition);

  if (location != -1) {
    std::cout << location << "\n";
    return 0;
  }

  return 1;
}
