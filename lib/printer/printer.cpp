#include "kllvm/printer/printer.h"

#include "kllvm/ast/AST.h"
#include "kllvm/parser/KOREParser.h"
#include "kllvm/parser/location.h"

#include <cstddef>
#include <list>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

namespace {

using namespace kllvm;
using namespace kllvm::parser;

std::string const WHITESPACE = " \n\r\t\f\v";

std::string ltrim(std::string const &s) {
  size_t start = s.find_first_not_of(WHITESPACE);
  return (start == std::string::npos) ? "" : s.substr(start);
}

std::string rtrim(std::string const &s) {
  size_t end = s.find_last_not_of(WHITESPACE);
  return (end == std::string::npos) ? "" : s.substr(0, end + 1);
}

std::string trim(std::string const &s) {
  return rtrim(ltrim(s));
}

std::map<std::string, std::string> getFormats() {
  static std::map<std::string, std::string> formats;
  static bool once = true;

  if (once) {
    formats["kseq"] = "%1 ~> %2";
    formats["append"] = "%1 ~> %2";
    formats["dotk"] = ".K";
    formats["inj"] = "%1";
    formats["\\bottom"] = "#Bottom";
    formats["\\top"] = "#Top";
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
    formats["weakAlwaysFinally"] = "#wAF ( %1 )";
    formats["weakExistsFinally"] = "#wEF ( %1 )";
    formats["allPathGlobally"] = "#AG ( %1 )";
    formats["bracket"] = "( %1 )";
    once = false;
  }

  return formats;
}

std::map<std::string, std::string> getTerminals() {
  static std::map<std::string, std::string> terminals;
  static bool once = true;

  if (once) {
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
    terminals["weakAlwaysFinally"] = "101";
    terminals["weakExistsFinally"] = "101";
    terminals["allPathGlobally"] = "101";
    terminals["bracket"] = "101";
    once = false;
  }

  return terminals;
}

std::set<std::string> getAssocs() {
  static std::set<std::string> assocs;
  static bool once = true;

  if (once) {
    assocs.insert("kseq");
    assocs.insert("\\and");
    assocs.insert("\\or");
    once = false;
  }

  return assocs;
}

std::set<std::string> getComms() {
  static std::set<std::string> comms;
  static bool once = true;

  if (once) {
    comms.insert("\\and");
    comms.insert("\\or");
    once = false;
  }

  return comms;
}

std::map<std::string, std::set<std::string>> getLeftAssocs() {
  static std::map<std::string, std::set<std::string>> leftAssocs;
  static bool once = true;

  if (once) {
    leftAssocs["kseq"].insert("kseq");
    leftAssocs["append"].insert("append");
    leftAssocs["\\and"].insert("\\and");
    leftAssocs["\\or"].insert("\\or");
    leftAssocs["\\rewrites"].insert("\\rewrites");
    once = false;
  }

  return leftAssocs;
}

std::map<std::string, std::set<std::string>> getRightAssocs() {
  static std::map<std::string, std::set<std::string>> rightAssocs;
  static bool once = true;

  if (once) {
    rightAssocs["\\rewrites"].insert("\\rewrites");
    once = false;
  }

  return rightAssocs;
}

std::map<std::string, std::set<std::string>> getPriorities() {
  static std::map<std::string, std::set<std::string>> priorities;
  static bool once = true;

  if (once) {
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
    once = false;
  }

  return priorities;
}

ptr<KOREDefinition> const &getDefinition(std::string const &kompiledDir) {
  static std::map<std::string, ptr<KOREDefinition>> cache;

  if (cache.find(kompiledDir) == cache.end()) {
    KOREParser parser(kompiledDir + std::string("/syntaxDefinition.kore"));
    cache[kompiledDir] = parser.definition();
  }

  return cache.at(kompiledDir);
}

std::vector<ptr<KOREDeclaration>> const &
getAxioms(std::string const &kompiledDir) {
  static std::map<std::string, std::vector<ptr<KOREDeclaration>>> cache;

  if (cache.find(kompiledDir) == cache.end()) {
    KOREParser parser(kompiledDir + std::string("/macros.kore"));
    cache[kompiledDir] = parser.declarations();
  }

  return cache.at(kompiledDir);
}

struct PreprocessedPrintData {
  PrettyPrintData data;
  SymbolMap overloads;
};

// NOLINTNEXTLINE(*-cognitive-complexity)
PreprocessedPrintData getPrintData(
    ptr<KOREDefinition> const &def,
    std::vector<ptr<KOREDeclaration>> const &axioms, bool hasColor) {
  auto formats = getFormats();
  auto terminals = getTerminals();
  auto assocs = getAssocs();
  auto comms = getComms();
  auto leftAssoc = getLeftAssocs();
  auto rightAssoc = getRightAssocs();
  auto priorities = getPriorities();

  BracketMap brackets;

  std::map<std::string, std::string> hooks;
  std::map<std::string, std::vector<std::string>> colors;

  auto subsorts = def->getSubsorts();
  auto overloads = def->getOverloads();

  for (auto const &entry : def->getSymbolDeclarations()) {
    std::string name = entry.first;

    if (entry.second->attributes().contains(attribute_set::key::format)) {
      formats[name]
          = entry.second->attributes().get_string(attribute_set::key::format);
      terminals[name] = entry.second->attributes().get_string(
          attribute_set::key::terminals);

      if (entry.second->attributes().contains(attribute_set::key::assoc)) {
        assocs.insert(name);
      }
      if (entry.second->attributes().contains(attribute_set::key::comm)) {
        comms.insert(name);
      }

      if (entry.second->attributes().contains(attribute_set::key::colors)) {
        std::string colorAtt
            = entry.second->attributes().get_string(attribute_set::key::colors);
        std::vector<std::string> color;
        size_t idx = 0;
        do {
          size_t pos = colorAtt.find_first_of(',', idx);
          if (pos == std::string::npos) {
            color.push_back(trim(colorAtt.substr(idx)));
            break;
          }
          color.push_back(trim(colorAtt.substr(idx, pos - idx)));
          idx = pos + 1;

        } while (true);
        colors[name] = color;
      }

      if (entry.second->attributes().contains(attribute_set::key::bracket)) {
        brackets[entry.second->getSymbol()->getSort().get()].push_back(
            entry.second->getSymbol());
      }

      readMultimap(name, entry.second, leftAssoc, attribute_set::key::left);
      readMultimap(name, entry.second, rightAssoc, attribute_set::key::right);
      readMultimap(
          name, entry.second, priorities, attribute_set::key::priorities);
    }
  }

  for (auto const &entry : def->getSortDeclarations()) {
    std::string name = entry.first;
    if (entry.second->attributes().contains(attribute_set::key::hook)) {
      hooks[name]
          = entry.second->attributes().get_string(attribute_set::key::hook);
    }
  }

  PrettyPrintData data
      = {formats, colors,   terminals, priorities, leftAssoc, rightAssoc,
         hooks,   brackets, assocs,    comms,      subsorts,  hasColor};

  return {data, overloads};
}

} // namespace

namespace kllvm {

using namespace parser;

std::ostream &printKORE(
    std::ostream &os, std::string const &definitionPath,
    std::string const &patternPath, bool hasColor, bool filterSubst,
    bool pretty) {
  static std::map<std::string, PreprocessedPrintData> cache;

  auto const &def = getDefinition(definitionPath);
  auto const &axioms = getAxioms(definitionPath);

  auto getPrintDataOrCached = [&] {
    if (cache.find(definitionPath) == cache.end()) {
      cache[definitionPath] = getPrintData(def, axioms, hasColor);
    }
    return cache.at(definitionPath);
  };

  auto config = KOREPattern::load(patternPath);
  config = config->unflattenAndOr();
  std::map<std::string, std::vector<KORESymbol *>> symbols;
  config->markSymbols(symbols);

  for (auto const &decl : axioms) {
    auto *axiom = dynamic_cast<KOREAxiomDeclaration *>(decl.get());
    axiom->getPattern()->markSymbols(symbols);
  }

  for (auto &entry : symbols) {
    for (auto *symbol : entry.second) {
      auto *decl = def->getSymbolDeclarations().at(symbol->getName());
      symbol->instantiateSymbol(decl);
    }
  }

  auto [data, overloads] = getPrintDataOrCached();

  sptr<KOREPattern> expanded
      = config->expandMacros(data.subsorts, overloads, axioms, true);
  sptr<KOREPattern> sorted = expanded->sortCollections(data);
  sptr<KOREPattern> filtered;

  if (filterSubst) {
    std::set<std::string> vars = sorted->gatherSingletonVars();
    filtered = sorted->filterSubstitution(data, vars);
    filtered = filtered->dedupeDisjuncts();
    filtered = filtered->sortCollections(data);
  } else {
    filtered = sorted;
  }

  sptr<KOREPattern> withBrackets = addBrackets(filtered, data);
  if (pretty) {
    withBrackets->prettyPrint(os, data);
  } else {
    withBrackets->print(os);
  }
  os << std::endl;

  return os;
}

} // namespace kllvm
