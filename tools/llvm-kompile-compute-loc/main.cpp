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

cl::opt<int> ordinal(
    cl::Positional, cl::desc("<ordinal>"), cl::Required, cl::cat(loc_cat));

cl::opt<bool> is_k_line(
    "k-line",
    cl::desc("The tool will look for the line passed as an argument in the K "
             "definition"),
    cl::init(false), cl::cat(loc_cat));

std::optional<int64_t> get_start_line_location(kore_axiom_declaration const &axiom) {
  auto location_att = axiom.attributes().get(attribute_set::key::Location);
  assert(location_att->get_arguments().size() == 1);

  auto str_pattern = std::dynamic_pointer_cast<kore_string_pattern>(
      location_att->get_arguments()[0]);
  std::string location = str_pattern->get_contents();

  size_t l_paren = location.find_first_of('(');
  size_t first_comma = location.find_first_of(',');
  size_t length = first_comma - l_paren - 1;
  return std::stoi(location.substr(l_paren + 1, length));
}

std::optional<int64_t>
get_k_location(std::string &definition, int const &line) {
  // Parse the definition.kore to get the AST.
  kllvm::parser::kore_parser parser(definition);
  auto kore_ast = parser.definition();
  kore_ast->preprocess();

  // Iterate through axioms.
  auto *axiom = kore_ast->get_axiom_by_ordinal(ordinal);
  if (axiom) {
    auto start_line = get_start_line_location(*axiom);
    return start_line;
  }

  return std::nullopt;
}

// trim the string from the start
std::string trim(std::string s) {
  s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int c) {
            return !std::isspace(c);
          }));
  return s;
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
