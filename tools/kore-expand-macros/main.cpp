#include "kllvm/ast/AST.h"
#include "kllvm/parser/KOREParser.h"
#include "kllvm/parser/location.h"

#include <algorithm>
#include <iostream>
#include <list>
#include <map>
#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

using namespace kllvm;
using namespace kllvm::parser;

int main(int argc, char **argv) {
  if (argc != 3) {
    std::cerr << "usage: " << argv[0] << " <kompiled-dir> <pattern.kore>"
              << std::endl;
    return 1;
  }

  kore_parser parser(argv[1] + std::string("/syntaxDefinition.kore"));
  ptr<kore_definition> def = parser.definition();

  auto subsorts = def->get_subsorts();
  auto overloads = def->get_overloads();

  kore_parser parser2(argv[1] + std::string("/macros.kore"));
  std::vector<ptr<kore_declaration>> axioms = parser2.declarations();
  std::sort(
      axioms.begin(), axioms.end(),
      [](ptr<kore_declaration> const &l, ptr<kore_declaration> const &r) {
        std::string l_str
            = l->attributes().get_string(attribute_set::key::Priority);
        std::string r_str
            = r->attributes().get_string(attribute_set::key::Priority);
        int l_int = std::stoi(l_str);
        int r_int = std::stoi(r_str);
        return l_int < r_int;
      });

  auto config = kore_pattern::load(argv[2]);
  std::map<std::string, std::vector<kore_symbol *>> symbols;
  config->mark_symbols(symbols);
  for (auto &decl : axioms) {
    auto *axiom = dynamic_cast<kore_axiom_declaration *>(decl.get());
    axiom->get_pattern()->mark_symbols(symbols);
  }

  for (auto &entry : symbols) {
    for (auto *symbol : entry.second) {
      auto *decl = def->get_symbol_declarations().at(symbol->get_name());
      symbol->instantiate_symbol(decl);
    }
  }

  auto expanded
      = axioms.empty()
            ? config
            : config->expand_macros(subsorts, overloads, axioms, false);

  expanded->print(std::cout);
  std::cout << std::endl;

  // NOLINTNEXTLINE(*-unused-return-value)
  def.release(); // so we don't waste time calling delete a bunch of times
}
