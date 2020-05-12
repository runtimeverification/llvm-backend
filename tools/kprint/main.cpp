#include "kllvm/parser/KOREScanner.h"
#include "kllvm/parser/KOREParser.h"

#include <iostream>

using namespace kllvm;
using namespace kllvm::parser;

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

  std::map<std::string, std::string> hooks;
  std::set<std::string> assocs;
  assocs.insert("kseq");
  assocs.insert("\\and");
  assocs.insert("\\or");
  std::set<std::string> comms;
  comms.insert("\\and");
  comms.insert("\\or");
  std::map<std::string, std::vector<std::string>> colors;

  KOREParser parser(argv[1]);
  ptr<KOREDefinition> def = parser.definition();

  for (auto &entry : def->getSymbolDeclarations()) {
    std::string name = entry.first;

    if (entry.second->getAttributes().count("format")) {
      formats[name] = entry.second->getStringAttribute("format");

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
    }
  }

  for (auto &entry : def->getSortDeclarations()) {
    std::string name = entry.first;
    if (entry.second->getAttributes().count("hook")) {
      hooks[name] = entry.second->getStringAttribute("hook");
    }
  }

  KOREParser parser2(argv[2]);
  sptr<KOREPattern> config = parser2.pattern();

  PrettyPrintData data = {formats, colors, hooks, assocs, comms, hasColor};

  sptr<KOREPattern> sorted = config->sortCollections(data);
  sorted->prettyPrint(std::cout, data);
  std::cout << std::endl;

  def.release(); // so we don't waste time calling delete a bunch of times
}
