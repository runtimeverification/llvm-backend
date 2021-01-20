#include "kllvm/parser/KOREScanner.h"
#include "kllvm/parser/KOREParser.h"

#include <iostream>

using namespace kllvm;
using namespace kllvm::parser;

sptr<KOREPattern> addBrackets(sptr<KOREPattern>, PrettyPrintData const&);

int main (int argc, char **argv) {
  if (argc != 3 && argc != 4) {
    std::cerr << "usage: " << argv[0] << " <definition.kore> <pattern.kore> [true|false|auto] [true|false]" << std::endl;
  }

  bool hasColor, filterSubst;
  if (argc >= 4) {
    std::string arg = argv[3];
    hasColor = arg == "true" || (arg == "auto" && isatty(1));
  } else {
    hasColor = isatty(1);
  }
  if (argc >= 5) {
    std::string arg = argv[4];
    filterSubst = arg == "true";
  } else {
    filterSubst = true;
  }

  std::map<std::string, std::string> formats;
  formats["kseq"] = "%1 ~> %2";
  formats["append"] = "%1 ~> %2";
  formats["dotk"] = ".";
  formats["inj"] = "%1";
  formats["\\bottom"] = "#False";
  formats["\\top"] = "#True";
  formats["\\not"] = "#Not ( %1 )";
  formats["\\ceil"] = "#Ceil ( %1 )";
  formats["\\floor"] = "#Floor ( %1 )";
  formats["\\equals"] = "{%i%n%1%d%n#Equals%i%n%2%d%n}";
  formats["\\and"] = "%i%1%d%n#And%n%i%2%d";
  formats["\\or"] = "%i%1%d%n#Or%n%i%2%d";
  formats["\\implies"] = "%i%1%d%n#Implies%n%i%2%d";
  formats["\\exists"] = "#Exists %1 . %2";
  formats["\\forall"] = "#Forall %1 . %2";
  formats["\\rewrites"] = "%1 => %2";
  formats["\\weakExistsFinally"] = "#wEF ( %1 )";
  formats["\\allPathGlobally"] = "#AG ( %1 )";
  formats["bracket"] = "( %1 )";

  std::map<std::string, std::string> terminals;
  terminals["kseq"] = "010";
  terminals["append"] = "010";
  terminals["dotk"] = "1";
  terminals["inj"] = "0";
  terminals["\\bottom"] = "1";
  terminals["\\top"] = "1";
  terminals["\\not"] = "101";
  terminals["\\ceil"] = "101";
  terminals["\\floor"] = "101";
  terminals["\\equals"] = "10101";
  terminals["\\and"] = "010";
  terminals["\\or"] = "010";
  terminals["\\implies"] = "010";
  terminals["\\exists"] = "1010";
  terminals["\\forall"] = "1010";
  terminals["\\rewrites"] = "010";
  terminals["\\weakExistsFinally"] = "101";
  terminals["\\allPathGlobally"] = "101";
  terminals["bracket"] = "101";

  BracketMap brackets;

  std::map<std::string, std::string> hooks;
  std::set<std::string> assocs;
  assocs.insert("kseq");
  assocs.insert("\\and");
  assocs.insert("\\or");
  std::set<std::string> comms;
  comms.insert("\\and");
  comms.insert("\\or");
  std::map<std::string, std::vector<std::string>> colors;

  std::map<std::string, std::set<std::string>> priorities, leftAssoc, rightAssoc;
  leftAssoc["kseq"].insert("kseq");
  leftAssoc["append"].insert("append");
  leftAssoc["\\and"].insert("\\and");
  leftAssoc["\\or"].insert("\\or");
  leftAssoc["\\rewrites"].insert("\\rewrites");
  rightAssoc["\\rewrites"].insert("\\rewrites");
  priorities["\\implies"].insert("\\exists");
  priorities["\\or"].insert("\\exists");
  priorities["\\and"].insert("\\exists");
  priorities["\\equals"].insert("\\exists");
  priorities["\\floor"].insert("\\exists");
  priorities["\\ceil"].insert("\\exists");
  priorities["\\not"].insert("\\exists");
  priorities["\\bottom"].insert("\\exists");
  priorities["\\top"].insert("\\exists");
  priorities["\\implies"].insert("\\forall");
  priorities["\\or"].insert("\\forall");
  priorities["\\and"].insert("\\forall");
  priorities["\\equals"].insert("\\forall");
  priorities["\\floor"].insert("\\forall");
  priorities["\\ceil"].insert("\\forall");
  priorities["\\not"].insert("\\forall");
  priorities["\\bottom"].insert("\\forall");
  priorities["\\top"].insert("\\forall");
  priorities["\\or"].insert("\\implies");
  priorities["\\and"].insert("\\implies");
  priorities["\\equals"].insert("\\implies");
  priorities["\\floor"].insert("\\implies");
  priorities["\\ceil"].insert("\\implies");
  priorities["\\not"].insert("\\implies");
  priorities["\\bottom"].insert("\\implies");
  priorities["\\top"].insert("\\implies");
  priorities["\\and"].insert("\\or");
  priorities["\\equals"].insert("\\or");
  priorities["\\floor"].insert("\\or");
  priorities["\\ceil"].insert("\\or");
  priorities["\\not"].insert("\\or");
  priorities["\\bottom"].insert("\\or");
  priorities["\\top"].insert("\\or");
  priorities["\\equals"].insert("\\and");
  priorities["\\floor"].insert("\\and");
  priorities["\\ceil"].insert("\\and");
  priorities["\\not"].insert("\\and");
  priorities["\\bottom"].insert("\\and");
  priorities["\\top"].insert("\\and");
  priorities["\\floor"].insert("\\equals");
  priorities["\\ceil"].insert("\\equals");
  priorities["\\not"].insert("\\equals");
  priorities["\\bottom"].insert("\\equals");
  priorities["\\top"].insert("\\equals");

  SubsortMap subsorts;
  SymbolMap overloads;

  KOREParser parser(argv[1] + std::string("/syntaxDefinition.kore"));
  ptr<KOREDefinition> def = parser.definition();

  for (auto &entry : def->getSymbolDeclarations()) {
    std::string name = entry.first;

    if (entry.second->getAttributes().count("format")) {
      formats[name] = entry.second->getStringAttribute("format");
      terminals[name] = entry.second->getStringAttribute("terminals");

      if (entry.second->getAttributes().count("assoc")) {
        assocs.insert(name);
      }
      if (entry.second->getAttributes().count("comm")) {
        comms.insert(name);
      }

      if (entry.second->getAttributes().count("colors")) {
        std::string colorAtt = entry.second->getStringAttribute("colors");
        std::vector<std::string> color;
        size_t idx = 0;
        do {
          size_t pos = colorAtt.find_first_of(',', idx);
          if (pos == std::string::npos) {
            color.push_back(colorAtt.substr(idx));
            break;
          } else {
            color.push_back(colorAtt.substr(idx, pos));
            idx = pos + 1;
          }
        } while (true);
        colors[name] = color;
      }

      if (entry.second->getAttributes().count("bracket")) {
        brackets[entry.second->getSymbol()->getSort().get()].push_back(entry.second->getSymbol());
      }

      readMultimap(name, entry.second, leftAssoc, "left");
      readMultimap(name, entry.second, rightAssoc, "right");
      readMultimap(name, entry.second, priorities, "priorities");
    }
  }

  for (auto &entry : def->getSortDeclarations()) {
    std::string name = entry.first;
    if (entry.second->getAttributes().count("hook")) {
      hooks[name] = entry.second->getStringAttribute("hook");
    }
  }

  for (auto axiom : def->getAxioms()) {
    if (axiom->getAttributes().count("subsort")) {
      KORECompositePattern *att = axiom->getAttributes().at("subsort").get();
      KORESort *innerSort = att->getConstructor()->getFormalArguments()[0].get();
      KORESort *outerSort = att->getConstructor()->getFormalArguments()[1].get();
      subsorts[innerSort].insert(outerSort);
    }
    if (axiom->getAttributes().count("overload")) {
      KORECompositePattern *att = axiom->getAttributes().at("overload").get();
      KORESymbol *innerSymbol = dynamic_cast<KORECompositePattern *>(att->getArguments()[1].get())->getConstructor();
      KORESymbol *outerSymbol = dynamic_cast<KORECompositePattern *>(att->getArguments()[0].get())->getConstructor();
      overloads[innerSymbol].insert(outerSymbol);
    }
  }

  subsorts = transitiveClosure(subsorts);
  overloads = transitiveClosure(overloads);

  KOREParser parser2(argv[1] + std::string("/macros.kore"));
  std::vector<ptr<KOREDeclaration>> axioms = parser2.declarations();

  KOREParser parser3(argv[2]);
  sptr<KOREPattern> config = parser3.pattern();
  std::map<std::string, std::vector<KORESymbol *>> symbols;
  config->markSymbols(symbols);
  for (auto &decl : axioms) {
    auto axiom = dynamic_cast<KOREAxiomDeclaration *>(decl.get());
    axiom->getPattern()->markSymbols(symbols);
  }

  for (auto iter = symbols.begin(); iter != symbols.end(); ++iter) {
    auto &entry = *iter;
    for (auto iter = entry.second.begin(); iter != entry.second.end(); ++iter) {
      KORESymbol *symbol = *iter;
      auto decl = def->getSymbolDeclarations().at(symbol->getName());
      symbol->instantiateSymbol(decl);
    }
  }

  PrettyPrintData data = {formats, colors, terminals, priorities, leftAssoc, rightAssoc, hooks, brackets, assocs, comms, subsorts, hasColor};

  sptr<KOREPattern> expanded = config->expandMacros(subsorts, overloads, axioms, true);
  sptr<KOREPattern> sorted = expanded->sortCollections(data);
  sptr<KOREPattern> filtered;
  if (filterSubst) {
    filtered = sorted->filterSubstitution(data);
  } else {
    filtered = sorted;
  }
  sptr<KOREPattern> withBrackets = addBrackets(filtered, data);
  withBrackets->prettyPrint(std::cout, data);
  std::cout << std::endl;

  def.release(); // so we don't waste time calling delete a bunch of times
}
