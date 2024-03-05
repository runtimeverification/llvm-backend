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

std::map<std::string, std::string> get_formats() {
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

std::map<std::string, std::string> get_terminals() {
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

std::set<std::string> get_assocs() {
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

std::set<std::string> get_comms() {
  static std::set<std::string> comms;
  static bool once = true;

  if (once) {
    comms.insert("\\and");
    comms.insert("\\or");
    once = false;
  }

  return comms;
}

std::map<std::string, std::set<std::string>> get_left_assocs() {
  static std::map<std::string, std::set<std::string>> left_assocs;
  static bool once = true;

  if (once) {
    left_assocs["kseq"].insert("kseq");
    left_assocs["append"].insert("append");
    left_assocs["\\and"].insert("\\and");
    left_assocs["\\or"].insert("\\or");
    left_assocs["\\rewrites"].insert("\\rewrites");
    once = false;
  }

  return left_assocs;
}

std::map<std::string, std::set<std::string>> get_right_assocs() {
  static std::map<std::string, std::set<std::string>> right_assocs;
  static bool once = true;

  if (once) {
    right_assocs["\\rewrites"].insert("\\rewrites");
    once = false;
  }

  return right_assocs;
}

std::map<std::string, std::set<std::string>> get_priorities() {
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

ptr<kore_definition> const &get_definition(std::string const &kompiled_dir) {
  static std::map<std::string, ptr<kore_definition>> cache;

  if (cache.find(kompiled_dir) == cache.end()) {
    kore_parser parser(kompiled_dir + std::string("/syntaxDefinition.kore"));
    cache[kompiled_dir] = parser.definition();
  }

  return cache.at(kompiled_dir);
}

std::vector<ptr<kore_declaration>> const &
get_axioms(std::string const &kompiled_dir) {
  static std::map<std::string, std::vector<ptr<kore_declaration>>> cache;

  if (cache.find(kompiled_dir) == cache.end()) {
    kore_parser parser(kompiled_dir + std::string("/macros.kore"));
    cache[kompiled_dir] = parser.declarations();
  }

  return cache.at(kompiled_dir);
}

struct preprocessed_print_data {
  pretty_print_data data;
  SymbolMap overloads;
};

// NOLINTNEXTLINE(*-cognitive-complexity)
preprocessed_print_data get_print_data(
    ptr<kore_definition> const &def,
    std::vector<ptr<kore_declaration>> const &axioms, bool has_color) {
  auto formats = get_formats();
  auto terminals = get_terminals();
  auto assocs = get_assocs();
  auto comms = get_comms();
  auto left_assoc = get_left_assocs();
  auto right_assoc = get_right_assocs();
  auto priorities = get_priorities();

  BracketMap brackets;

  std::map<std::string, std::string> hooks;
  std::map<std::string, std::vector<std::string>> colors;

  auto subsorts = def->get_subsorts();
  auto overloads = def->get_overloads();

  for (auto const &entry : def->get_symbol_declarations()) {
    std::string name = entry.first;

    if (entry.second->attributes().contains(attribute_set::key::Format)) {
      formats[name]
          = entry.second->attributes().get_string(attribute_set::key::Format);
      terminals[name] = entry.second->attributes().get_string(
          attribute_set::key::Terminals);

      if (entry.second->attributes().contains(attribute_set::key::Assoc)) {
        assocs.insert(name);
      }
      if (entry.second->attributes().contains(attribute_set::key::Comm)) {
        comms.insert(name);
      }

      if (entry.second->attributes().contains(attribute_set::key::Colors)) {
        std::string color_att
            = entry.second->attributes().get_string(attribute_set::key::Colors);
        std::vector<std::string> color;
        size_t idx = 0;
        do {
          size_t pos = color_att.find_first_of(',', idx);
          if (pos == std::string::npos) {
            color.push_back(trim(color_att.substr(idx)));
            break;
          }
          color.push_back(trim(color_att.substr(idx, pos - idx)));
          idx = pos + 1;

        } while (true);
        colors[name] = color;
      }

      if (entry.second->attributes().contains(attribute_set::key::Bracket)) {
        brackets[entry.second->get_symbol()->get_sort().get()].push_back(
            entry.second->get_symbol());
      }

      read_multimap(name, entry.second, left_assoc, attribute_set::key::Left);
      read_multimap(name, entry.second, right_assoc, attribute_set::key::Right);
      read_multimap(
          name, entry.second, priorities, attribute_set::key::Priorities);
    }
  }

  for (auto const &entry : def->get_sort_declarations()) {
    std::string name = entry.first;
    if (entry.second->attributes().contains(attribute_set::key::Hook)) {
      hooks[name]
          = entry.second->attributes().get_string(attribute_set::key::Hook);
    }
  }

  pretty_print_data data
      = {formats, colors,   terminals, priorities, left_assoc, right_assoc,
         hooks,   brackets, assocs,    comms,      subsorts,   has_color};

  return {data, overloads};
}

} // namespace

namespace kllvm {

using namespace parser;

std::ostream &print_kore(
    std::ostream &os, std::string const &definition_path,
    std::string const &pattern_path, bool has_color, bool filter_subst,
    bool pretty) {
  static std::map<std::string, preprocessed_print_data> cache;

  auto const &def = get_definition(definition_path);
  auto const &axioms = get_axioms(definition_path);

  auto get_print_data_or_cached = [&] {
    if (cache.find(definition_path) == cache.end()) {
      cache[definition_path] = get_print_data(def, axioms, has_color);
    }
    return cache.at(definition_path);
  };

  auto config = kore_pattern::load(pattern_path);
  config = config->unflatten_and_or();
  std::map<std::string, std::vector<kore_symbol *>> symbols;
  config->mark_symbols(symbols);

  for (auto const &decl : axioms) {
    auto *axiom = dynamic_cast<kore_axiom_declaration *>(decl.get());
    axiom->get_pattern()->mark_symbols(symbols);
  }

  for (auto &entry : symbols) {
    for (auto *symbol : entry.second) {
      auto *decl = def->get_symbol_declarations().at(symbol->get_name());
      symbol->instantiate_symbol(decl);
    }
  }

  auto [data, overloads] = get_print_data_or_cached();

  sptr<kore_pattern> expanded
      = config->expand_macros(data.subsorts, overloads, axioms, true);
  sptr<kore_pattern> sorted = expanded->sort_collections(data);
  sptr<kore_pattern> filtered;

  if (filter_subst) {
    std::set<std::string> vars = sorted->gather_singleton_vars();
    filtered = sorted->filter_substitution(data, vars);
    filtered = filtered->dedupe_disjuncts();
    filtered = filtered->sort_collections(data);
  } else {
    filtered = sorted;
  }

  sptr<kore_pattern> with_brackets = add_brackets(filtered, data);
  if (pretty) {
    with_brackets->pretty_print(os, data);
  } else {
    with_brackets->print(os);
  }
  os << std::endl;

  return os;
}

} // namespace kllvm
