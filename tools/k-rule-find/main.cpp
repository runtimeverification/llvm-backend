#include <kllvm/ast/AST.h>
#include <kllvm/codegen/Debug.h>
#include <kllvm/parser/KOREParser.h>

#include <llvm/Support/CommandLine.h>

#include <cstdlib>
#include <iostream>

using namespace llvm;
using namespace kllvm;

struct Location {
  std::string filename;
  int64_t start_line;
  int64_t end_line;
  int64_t start_column;
  int64_t end_column;
};

cl::OptionCategory KRuleCat("k-rule-find options");

cl::opt<std::string> KompiledDir(
    cl::Positional, cl::desc("<kompiled-dir>"), cl::Required,
    cl::cat(KRuleCat));

cl::opt<std::string> RuleLocation(
    cl::Positional, cl::desc("<filename.k:line[:column]>"), cl::Required,
    cl::cat(KRuleCat));

std::string getSource(KOREAxiomDeclaration *axiom) {
  auto *sourceAtt = axiom->getAttributes().at(SOURCE_ATT).get();
  assert(sourceAtt->getArguments().size() == 1);

  auto strPattern
      = dynamic_cast<KOREStringPattern *>(sourceAtt->getArguments()[0].get());
  return strPattern->getContents();
}

Location getLocation(KOREAxiomDeclaration *axiom) {
  auto *locationAtt = axiom->getAttributes().at(LOCATION_ATT).get();
  assert(locationAtt->getArguments().size() == 1);

  auto strPattern
      = dynamic_cast<KOREStringPattern *>(locationAtt->getArguments()[0].get());
  std::string location = strPattern->getContents();

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

Location parseLocation(std::string loc) {
  size_t pos = loc.find(':');
  if (pos == std::string::npos) {
    std::cerr << "Rule's location must be in the format: "
                 "definition.k:line[:column]\n";
    exit(EXIT_FAILURE);
  }

  std::string lineColumn = loc.substr(pos + 1);
  size_t pos_lc = lineColumn.find(':');

  // If another “:” isn’t found, the tool assumes no column number was given.
  int64_t line, column = -1;
  if (pos_lc == std::string::npos) {
    line = stoi(lineColumn);
  } else {
    line = stoi(lineColumn.substr(0, pos_lc));
    column = stoi(lineColumn.substr(pos_lc + 1));
  }

  return {loc.substr(0, pos), line, line, column, column};
}

bool checkRanges(Location param, Location file, bool checkColumn) {
  auto line
      = param.start_line >= file.start_line && param.end_line <= file.end_line;
  auto column = param.start_column >= file.start_column
                && param.end_column <= file.end_column;
  return checkColumn ? line && column : line;
}

int main(int argc, char **argv) {
  cl::HideUnrelatedOptions({&KRuleCat});
  cl::ParseCommandLineOptions(argc, argv);

  auto loc = parseLocation(RuleLocation);
  auto definition = KompiledDir + "/definition.kore";
  std::vector<std::string> rule_labels;

  // Parse the definition.kore to get the AST.
  kllvm::parser::KOREParser parser(definition);
  auto kore_ast = parser.definition();

  // Iterate through axioms.
  for (auto axiom : kore_ast.get()->getAxioms()) {
    if (axiom->getAttributes().count(SOURCE_ATT)) {
      auto source = getSource(axiom);
      if (source.find(loc.filename) != std::string::npos) {
        auto source_loc = getLocation(axiom);
        if (checkRanges(loc, source_loc, loc.start_column != -1)) {
          rule_labels.push_back(axiom->getStringAttribute("label"));
        }
      }
    }
  }

  // Output the result or the error message.
  if (rule_labels.empty()) {
    std::cerr << "Error: Couldn't find rule label within the given location.\n";
  } else {
    for (auto const &rule_label : rule_labels)
      std::cout << rule_label << "\n";
  }

  return 0;
}
