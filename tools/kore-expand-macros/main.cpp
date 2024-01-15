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

  SubsortMap subsorts;
  SymbolMap overloads;

  KOREParser parser(argv[1] + std::string("/syntaxDefinition.kore"));
  ptr<KOREDefinition> def = parser.definition();

  for (auto *axiom : def->getAxioms()) {
    if (axiom->getAttributes().count("subsort")) {
      KORECompositePattern *att = axiom->getAttributes().at("subsort").get();
      KORESort *innerSort
          = att->getConstructor()->getFormalArguments()[0].get();
      KORESort *outerSort
          = att->getConstructor()->getFormalArguments()[1].get();
      subsorts[innerSort].insert(outerSort);
    }
    if (axiom->getAttributes().count("overload")) {
      KORECompositePattern *att = axiom->getAttributes().at("overload").get();
      KORESymbol *innerSymbol
          = dynamic_cast<KORECompositePattern *>(att->getArguments()[1].get())
                ->getConstructor();
      KORESymbol *outerSymbol
          = dynamic_cast<KORECompositePattern *>(att->getArguments()[0].get())
                ->getConstructor();
      overloads[innerSymbol].insert(outerSymbol);
    }
  }

  subsorts = transitiveClosure(subsorts);
  overloads = transitiveClosure(overloads);

  KOREParser parser2(argv[1] + std::string("/macros.kore"));
  std::vector<ptr<KOREDeclaration>> axioms = parser2.declarations();
  std::sort(
      axioms.begin(), axioms.end(),
      [](const ptr<KOREDeclaration> &l, const ptr<KOREDeclaration> &r) {
        std::string lStr = l->getStringAttribute("priority");
        std::string rStr = r->getStringAttribute("priority");
        int lInt = std::stoi(lStr);
        int rInt = std::stoi(rStr);
        return lInt < rInt;
      });

  auto config = KOREPattern::load(argv[2]);
  std::map<std::string, std::vector<KORESymbol *>> symbols;
  config->markSymbols(symbols);
  for (auto &decl : axioms) {
    auto *axiom = dynamic_cast<KOREAxiomDeclaration *>(decl.get());
    axiom->getPattern()->markSymbols(symbols);
  }

  for (auto &entry : symbols) {
    for (auto *symbol : entry.second) {
      auto *decl = def->getSymbolDeclarations().at(symbol->getName());
      symbol->instantiateSymbol(decl);
    }
  }

  auto expanded
      = axioms.empty()
            ? config
            : config->expandMacros(subsorts, overloads, axioms, false);

  expanded->print(std::cout);
  std::cout << std::endl;

  def.release(); // so we don't waste time calling delete a bunch of times
}
