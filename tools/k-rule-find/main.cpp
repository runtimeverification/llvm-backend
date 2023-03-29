#include <kllvm/ast/AST.h>
#include <kllvm/codegen/Debug.h>
#include <kllvm/parser/KOREParser.h>
#include <runtime/header.h>

#include <llvm/Support/CommandLine.h>

#include <cstdlib>
#include <dlfcn.h>
#include <iostream>

using namespace llvm;
using namespace kllvm;

struct Location {
  std::string filename;
  int64_t line;
  int64_t column;
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

  size_t first_comma = location.find_first_of(',');
  int64_t lineNumber = std::stoi(location.substr(9, first_comma - 9));
  int64_t columnNumber = std::stoi(location.substr(
      first_comma + 1,
      location.find_first_of(',', first_comma + 1) - first_comma - 1));

  return {location, lineNumber, columnNumber};
}

Location parseLocation(std::string loc) {
  std::string filename;
  int64_t line, column = -1;
  size_t pos = loc.find(":");
  if (pos == std::string::npos) {
    std::cerr
        << "Rule's location must me in the format: defintion.k:line[:column]\n";
    exit(EXIT_FAILURE);
  }
  filename = loc.substr(0, pos);

  std::string lineColumn = loc.substr(pos + 1);
  size_t pos_lc = lineColumn.find(":");

  // If another “:” isn’t found, the tool assumes no column number was given.
  if (pos_lc == std::string::npos) {
    line = stoi(lineColumn);
  } else {
    line = stoi(lineColumn.substr(0, pos_lc));
    column = stoi(lineColumn.substr(pos_lc + 1));
  }

  return {filename, line, column};
}

int main(int argc, char **argv) {
  cl::HideUnrelatedOptions({&KRuleCat});
  cl::ParseCommandLineOptions(argc, argv);

  auto loc = parseLocation(RuleLocation);
  auto definition = KompiledDir + "/definition.kore";
  std::string rule_label = "";

  // Parse the definition.kore to get the AST.
  kllvm::parser::KOREParser parser(definition);
  auto kore_ast = parser.definition();

  // Iterate through axioms.
  for (auto axiom : kore_ast.get()->getAxioms()) {
    if (axiom->getAttributes().count(SOURCE_ATT)) {
      auto source = getSource(axiom);
      if (source.find(loc.filename) != std::string::npos) {
        auto source_loc = getLocation(axiom);
        if (loc.line == source_loc.line
            && (loc.column == -1 || loc.column == source_loc.column)) {
          rule_label = axiom->getStringAttribute("label");
        }
      }
    }
  }

  // Output the result or the error message.
  if (rule_label.empty()) {
    std::cerr << "Error: Couldn't find rule label within the given location.\n";
  } else {
    std::cout << rule_label << "\n";
  }

  return 0;
}
