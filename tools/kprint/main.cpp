#include "kllvm/parser/KOREScanner.h"
#include "kllvm/parser/KOREParser.h"

#include <iostream>

using namespace kllvm;
using namespace kllvm::parser;

sptr<KOREPattern> addBrackets(sptr<KOREPattern>, PrettyPrintData const&);

void readMultimap(std::string name, KORESymbolDeclaration *decl, std::map<std::string, std::set<std::string>> &output, std::string attName) {
  if (decl->getAttributes().count(attName)) {
    KORECompositePattern *att = decl->getAttributes().at(attName).get();
    for (auto &pat : att->getArguments()) {
      auto child = dynamic_cast<KORECompositePattern *>(pat.get());
      output[name].insert(child->getConstructor()->getName());
    }
  }
}

SubsortMap transitiveClosure(SubsortMap relations) {
  bool dirty = false;
  for (auto &entry : relations) {
    SortSet newSucc;
    for (auto &elem : entry.second) {
      auto &relation = relations[elem];
      for (auto elem2 : relation) {
        dirty |= relations[entry.first].insert(elem2).second;
      }
    }
  }
  if (dirty)
    return transitiveClosure(relations);
  else
    return relations;
}

int main (int argc, char **argv) {
  if (argc != 3 && argc != 4) {
    std::cerr << "usage: " << argv[0] << " <definition.kore> <pattern.kore> [true|false]" << std::endl;
  }

  bool hasColor;
  if (argc == 4) {
    std::string arg = argv[3];
    hasColor = arg == "true";
  } else {
    hasColor = isatty(1);
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
  formats["bracket"] = "(%1)";

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

  std::map<std::string, KORESymbol *> brackets;

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
  priorities["\\exists"].insert("\\implies");
  priorities["\\exists"].insert("\\or");
  priorities["\\exists"].insert("\\and");
  priorities["\\exists"].insert("\\equals");
  priorities["\\exists"].insert("\\floor");
  priorities["\\exists"].insert("\\ceil");
  priorities["\\exists"].insert("\\not");
  priorities["\\exists"].insert("\\bottom");
  priorities["\\exists"].insert("\\top");
  priorities["\\forall"].insert("\\implies");
  priorities["\\forall"].insert("\\or");
  priorities["\\forall"].insert("\\and");
  priorities["\\forall"].insert("\\equals");
  priorities["\\forall"].insert("\\floor");
  priorities["\\forall"].insert("\\ceil");
  priorities["\\forall"].insert("\\not");
  priorities["\\forall"].insert("\\bottom");
  priorities["\\forall"].insert("\\top");
  priorities["\\implies"].insert("\\or");
  priorities["\\implies"].insert("\\and");
  priorities["\\implies"].insert("\\equals");
  priorities["\\implies"].insert("\\floor");
  priorities["\\implies"].insert("\\ceil");
  priorities["\\implies"].insert("\\not");
  priorities["\\implies"].insert("\\bottom");
  priorities["\\implies"].insert("\\top");
  priorities["\\or"].insert("\\and");
  priorities["\\or"].insert("\\equals");
  priorities["\\or"].insert("\\floor");
  priorities["\\or"].insert("\\ceil");
  priorities["\\or"].insert("\\not");
  priorities["\\or"].insert("\\bottom");
  priorities["\\or"].insert("\\top");
  priorities["\\and"].insert("\\equals");
  priorities["\\and"].insert("\\floor");
  priorities["\\and"].insert("\\ceil");
  priorities["\\and"].insert("\\not");
  priorities["\\and"].insert("\\bottom");
  priorities["\\and"].insert("\\top");
  priorities["\\equals"].insert("\\floor");
  priorities["\\equals"].insert("\\ceil");
  priorities["\\equals"].insert("\\not");
  priorities["\\equals"].insert("\\bottom");
  priorities["\\equals"].insert("\\top");

  SubsortMap subsorts;

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
        if (auto compositeSort = dynamic_cast<KORECompositeSort *>(entry.second->getSymbol()->getSort().get())) {
          brackets[compositeSort->getName()] = entry.second->getSymbol();
        }
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
  }

  subsorts = transitiveClosure(subsorts);

  KOREParser parser2(argv[2]);
  sptr<KOREPattern> config = parser2.pattern();
  std::map<std::string, std::vector<KORESymbol *>> symbols;
  config->markSymbols(symbols);

  for (auto iter = symbols.begin(); iter != symbols.end(); ++iter) {
    auto &entry = *iter;
    for (auto iter = entry.second.begin(); iter != entry.second.end(); ++iter) {
      KORESymbol *symbol = *iter;
      auto decl = def->getSymbolDeclarations().at(symbol->getName());
      symbol->instantiateSymbol(decl);
    }
  }

  PrettyPrintData data = {formats, colors, terminals, priorities, leftAssoc, rightAssoc, hooks, brackets, assocs, comms, subsorts, hasColor};

  sptr<KOREPattern> withBrackets = addBrackets(config, data);
  sptr<KOREPattern> sorted = withBrackets->sortCollections(data);
  sorted->prettyPrint(std::cout, data);
  std::cout << std::endl;

  def.release(); // so we don't waste time calling delete a bunch of times
}
