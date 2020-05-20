#include "kllvm/parser/KOREScanner.h"
#include "kllvm/parser/KOREParser.h"

#include <iostream>
#include <fstream>

using namespace kllvm;
using namespace kllvm::parser;

void readMap(std::map<std::string, std::string> &result, std::ifstream &is) {
  std::string line;
  while(std::getline(is, line)) {
    std::string att;
    if (!std::getline(is, att)) {
      break;
    }
    result[line] = att;
  }
}


int main (int argc, char **argv) {
  if (argc != 3 && argc != 4) {
    std::cerr << "usage: " << argv[0] << " <kompiled-dir> <pattern.kore> [true|false]" << std::endl;
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

  // load information about definition written by k frontend
  std::ifstream iformats(argv[1] + std::string("/format-att.txt"));
  std::ifstream ihooks(argv[1] + std::string("/hook-att.txt"));
  std::ifstream iassocs(argv[1] + std::string("/assoc-att.txt"));
  std::ifstream icomms(argv[1] + std::string("/comm-att.txt"));
  std::ifstream icolors(argv[1] + std::string("/color-att.txt"));

  readMap(formats, iformats);
  readMap(hooks, ihooks);

  std::string line;
  while(std::getline(iassocs, line)) {
    assocs.insert(line);
  }
  while(std::getline(icomms, line)) {
    comms.insert(line);
  }

  while(std::getline(icolors, line)) {
    std::string color;
    while(std::getline(icolors, color)) {
      if (color.empty()) {
        break;
      }
      colors[line].push_back(color);
    }
  }

  KOREParser parser2(argv[2]);
  sptr<KOREPattern> config = parser2.pattern();

  PrettyPrintData data = {formats, colors, hooks, assocs, comms, hasColor};

  sptr<KOREPattern> sorted = config->sortCollections(data);
  sorted->prettyPrint(std::cout, data);
  std::cout << std::endl;
}
