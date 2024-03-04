#include <kllvm/ast/AST.h>
#include <kllvm/codegen/Debug.h>
#include <kllvm/parser/KOREParser.h>

#include <llvm/Support/CommandLine.h>

#include <cstdlib>
#include <iostream>

using namespace llvm;
using namespace kllvm;

struct location {
  std::string filename;
  int64_t start_line;
  int64_t end_line;
  int64_t start_column;
  int64_t end_column;
};

cl::OptionCategory k_rule_cat("k-rule-find options");

cl::opt<std::string> kompiled_dir(
    cl::Positional, cl::desc("<kompiled-dir>"), cl::Required,
    cl::cat(k_rule_cat));

cl::opt<std::string> rule_location(
    cl::Positional, cl::desc("<filename.k:line[:column]>"), cl::Required,
    cl::cat(k_rule_cat));

std::string get_source(kore_axiom_declaration *axiom) {
  auto *source_att = axiom->attributes().get(attribute_set::key::Source).get();
  assert(source_att->get_arguments().size() == 1);

  auto *str_pattern
      = dynamic_cast<kore_string_pattern *>(source_att->get_arguments()[0].get());
  return str_pattern->get_contents();
}

location get_location(kore_axiom_declaration *axiom) {
  auto *location_att
      = axiom->attributes().get(attribute_set::key::Location).get();
  assert(location_att->get_arguments().size() == 1);

  auto *str_pattern
      = dynamic_cast<kore_string_pattern *>(location_att->get_arguments()[0].get());
  std::string location = str_pattern->get_contents();

  size_t l_paren = location.find_first_of('(');
  size_t first_comma = location.find_first_of(',');
  size_t length = first_comma - l_paren - 1;
  int64_t start_line = std::stoi(location.substr(l_paren + 1, length));

  size_t second_comma = location.find_first_of(',', first_comma + 1);
  length = second_comma - first_comma - 1;
  int64_t start_column = std::stoi(location.substr(first_comma + 1, length));

  size_t third_comma = location.find_last_of(',');
  length = third_comma - second_comma - 1;
  int64_t end_line = std::stoi(location.substr(second_comma + 1, length));

  length = location.find_last_of(')');
  int64_t end_column = std::stoi(location.substr(third_comma + 1, length));

  return {location, start_line, end_line, start_column, end_column};
}

location parse_location(std::string const &loc) {
  size_t pos = loc.find(':');
  if (pos == std::string::npos) {
    std::cerr << "Rule's location must be in the format: "
                 "definition.k:line[:column]\n";
    exit(EXIT_FAILURE);
  }

  std::string line_column = loc.substr(pos + 1);
  size_t pos_lc = line_column.find(':');

  // If another “:” isn’t found, the tool assumes no column number was given.
  int64_t line = 0;
  int64_t column = -1;
  if (pos_lc == std::string::npos) {
    line = stoi(line_column);
  } else {
    line = stoi(line_column.substr(0, pos_lc));
    column = stoi(line_column.substr(pos_lc + 1));
  }

  return {loc.substr(0, pos), line, line, column, column};
}

bool check_ranges(
    location const &param, location const &file, bool check_column) {
  auto line
      = param.start_line >= file.start_line && param.end_line <= file.end_line;
  auto column = param.start_column >= file.start_column
                && param.end_column <= file.end_column;
  return check_column ? line && column : line;
}

int main(int argc, char **argv) {
  cl::HideUnrelatedOptions({&k_rule_cat});
  cl::ParseCommandLineOptions(argc, argv);

  auto loc = parse_location(rule_location);
  auto definition = kompiled_dir + "/definition.kore";
  std::vector<std::string> rule_labels;

  // Parse the definition.kore to get the AST.
  kllvm::parser::kore_parser parser(definition);
  auto kore_ast = parser.definition();

  // Iterate through axioms.
  for (auto *axiom : kore_ast.get()->get_axioms()) {
    if (axiom->attributes().contains(attribute_set::key::Source)) {
      auto source = get_source(axiom);
      if (source.find(loc.filename) != std::string::npos) {
        auto source_loc = get_location(axiom);
        if (check_ranges(loc, source_loc, loc.start_column != -1)) {
          rule_labels.push_back(
              axiom->attributes().get_string(attribute_set::key::Label));
        }
      }
    }
  }

  // Output the result or the error message.
  if (rule_labels.empty()) {
    std::cerr << "Error: Couldn't find rule label within the given location.\n";
  } else {
    for (auto const &rule_label : rule_labels) {
      std::cout << rule_label << "\n";
    }
  }

  return 0;
}
