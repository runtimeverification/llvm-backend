#include <kllvm/ast/AST.h>
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

cl::opt<std::string> line(
    cl::Positional, cl::desc("<line>"), cl::Required, cl::cat(ordinal_cat));

cl::opt<bool> is_k_line(
    cl::Positional, cl::desc("[is-k-line]"), cl::init(false),
    cl::cat(ordinal_cat));

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

int64_t get_k_ordinal(std::string &definition) {
  // Parse the definition.kore to get the AST.
  kllvm::parser::kore_parser parser(definition);
  auto kore_ast = parser.definition();
  kore_ast.get()->preprocess();

  // Iterate through axioms.
  for (auto *axiom : kore_ast.get()->get_axioms()) {
    if (axiom->attributes().contains(attribute_set::key::Location)) {
      auto start_line = get_location(axiom);
      if (start_line == stoi(line)) {
        return axiom->get_ordinal();
      }
    }
  }

  return -1;
}

// trim the string from the start
inline void trim(std::string &s) {
  s.erase(
      s.begin(),
      std::find_if(
          s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
}

int64_t get_kore_ordinal(std::string &definition) {

  int64_t line_num = 0;
  int64_t ordinal_num = 0;

  std::ifstream file(definition);
  std::string l;

  while (std::getline(file, l)) {
    line_num++;
    trim(l);
    if (l.starts_with("axiom")) {
      if (line_num == std::stoi(line)) {
        file.close();
        return ordinal_num;
      }
      ordinal_num++;
    }
  }

  file.close();
  return -1;
}

int main(int argc, char **argv) {
  cl::HideUnrelatedOptions({&ordinal_cat});
  cl::ParseCommandLineOptions(argc, argv);

  auto definition = kompiled_dir + "/definition.kore";

  int64_t location
      = is_k_line ? get_k_ordinal(definition) : get_kore_ordinal(definition);

  if (location != -1) {
    std::cout << location << "\n";
    return 0;
  }

  return 1;
}
