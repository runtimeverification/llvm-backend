#include "kllvm/ast/AST.h"

#include "kllvm/ast/pattern_matching.h"

#include "kllvm/binary/deserializer.h"
#include "kllvm/binary/serializer.h"
#include "kllvm/parser/KOREParser.h"

#include <fmt/format.h>

#include <algorithm>
#include <array>
#include <cassert>
#include <cctype>
#include <cstdio>
#include <iostream>
#include <iterator>
#include <unordered_map>
#include <unordered_set>

using namespace kllvm;

sptr<kore_pattern> kore_pattern::load(std::string const &filename) {
  if (has_binary_kore_header(filename)) {
    return deserialize_pattern(filename);
  }
  return parser::kore_parser(filename).pattern();
}

std::string kllvm::decodeKore(std::string kore) {
  static std::unordered_map<std::string, char> codes;
  static bool once = true;
  if (once) {
    codes["Spce"] = ' ';
    codes["Bang"] = '!';
    codes["Quot"] = '"';
    codes["Hash"] = '#';
    codes["Dolr"] = '$';
    codes["Perc"] = '%';
    codes["And-"] = '&';
    codes["Apos"] = '\'';
    codes["LPar"] = '(';
    codes["RPar"] = ')';
    codes["Star"] = '*';
    codes["Plus"] = '+';
    codes["Comm"] = ',';
    codes["Stop"] = '.';
    codes["Slsh"] = '/';
    codes["Coln"] = ':';
    codes["SCln"] = 'l';
    codes["-LT-"] = '<';
    codes["Eqls"] = '=';
    codes["-GT-"] = '>';
    codes["Ques"] = '?';
    codes["-AT-"] = '@';
    codes["LSqB"] = '[';
    codes["RSqB"] = ']';
    codes["Bash"] = '\\';
    codes["Xor-"] = '^';
    codes["Unds"] = '_';
    codes["BQuo"] = '`';
    codes["LBra"] = '{';
    codes["Pipe"] = '|';
    codes["RBra"] = '}';
    codes["Tild"] = '~';
    once = false;
  }
  bool literal = true;
  std::string result;
  size_t i = 0;
  while (i < kore.length()) {
    if (kore[i] == '\'') {
      literal = !literal;
      i++;
    } else if (literal) {
      result.push_back(kore[i]);
      i++;
    } else {
      auto code = kore.substr(i, 4);
      result.push_back(codes[code]);
      i += 4;
    }
  }
  return result;
}

size_t kllvm::hash_value(kllvm::kore_sort const &s) {
  return hash_sort{}(s);
}

sptr<kore_pattern> kore_pattern::expandMacros(
    SubsortMap const &subsorts, SymbolMap const &overloads,
    std::vector<ptr<kore_declaration>> const &axioms, bool reverse) {
  std::set<size_t> appliedRules;

  std::set<std::string> macroSymbols;

  for (auto const &decl : axioms) {
    auto *axiom = dynamic_cast<kore_axiom_declaration *>(decl.get());
    auto *equals
        = dynamic_cast<kore_composite_pattern *>(axiom->getPattern().get());
    auto lhs = equals->getArguments()[reverse ? 1 : 0];

    if (auto *lhs_comp = dynamic_cast<kore_composite_pattern *>(lhs.get())) {
      macroSymbols.insert(lhs_comp->getConstructor()->getName());
    }
  }

  return expandMacros(
      subsorts, overloads, axioms, reverse, appliedRules, macroSymbols);
}

bool kore_sort_variable::operator==(kore_sort const &other) const {
  if (auto const *var = dynamic_cast<kore_sort_variable const *>(&other)) {
    return var->name == name;
  }
  return false;
}

void kore_composite_sort::addArgument(sptr<kore_sort> const &Argument) {
  arguments.push_back(Argument);
}

bool kore_composite_sort::operator==(kore_sort const &other) const {
  if (auto const *sort = dynamic_cast<kore_composite_sort const *>(&other)) {
    if (sort->name != name || sort->arguments.size() != arguments.size()) {
      return false;
    }
    for (int i = 0; i < arguments.size(); ++i) {
      if (*sort->arguments[i] != *arguments[i]) {
        return false;
      }
    }
    return true;
  }
  return false;
}

sptr<kore_sort> kore_composite_sort::substitute(substitution const &subst) {
  bool dirty = false;
  std::vector<sptr<kore_sort>> newArgs;
  for (auto &arg : arguments) {
    auto newArg = arg->substitute(subst);
    if (newArg != arg) {
      dirty = true;
    }
    newArgs.push_back(newArg);
  }
  if (dirty) {
    sptr<kore_composite_sort> retval = Create(name);
    retval->arguments = newArgs;
    return retval;
  }
  return shared_from_this();
}

value_type kore_composite_sort::getCategory(kore_definition *definition) {
  if (category.cat != SortCategory::Uncomputed) {
    return category;
  }
  std::string name = getHook(definition);
  if (name == "MINT.MInt") {
    if (auto *param = dynamic_cast<kore_composite_sort *>(arguments[0].get())) {
      auto const &att = definition->getSortDeclarations()
                            .at(param->getName())
                            ->attributes();
      auto const &natAtt = att.get(attribute_set::key::nat);
      assert(natAtt->getArguments().size() == 1);
      auto *strPattern
          = dynamic_cast<kore_string_pattern *>(natAtt->getArguments()[0].get());
      name = name + " " + strPattern->getContents();
    } else {
      print(std::cerr);
      abort();
    }
  }
  category = getCategory(name);
  return category;
}

std::string kore_composite_sort::getHook(kore_definition *definition) const {
  auto const &att
      = definition->getSortDeclarations().at(this->getName())->attributes();
  if (!att.contains(attribute_set::key::hook)) {
    return "STRING.String";
  }
  auto const &hookAtt = att.get(attribute_set::key::hook);
  assert(hookAtt->getArguments().size() == 1);
  auto *strPattern
      = dynamic_cast<kore_string_pattern *>(hookAtt->getArguments()[0].get());
  return strPattern->getContents();
}

value_type kore_composite_sort::getCategory(std::string const &hookName) {
  SortCategory category = SortCategory::Uncomputed;
  uint64_t bits = 0;
  if (hookName == "MAP.Map") {
    category = SortCategory::Map;
  } else if (hookName == "RANGEMAP.RangeMap") {
    category = SortCategory::RangeMap;
  } else if (hookName == "LIST.List") {
    category = SortCategory::List;
  } else if (hookName == "SET.Set") {
    category = SortCategory::Set;
  } else if (hookName == "INT.Int") {
    category = SortCategory::Int;
  } else if (hookName == "FLOAT.Float") {
    category = SortCategory::Float;
  } else if (hookName == "BUFFER.StringBuffer") {
    category = SortCategory::StringBuffer;
  } else if (hookName == "BOOL.Bool") {
    category = SortCategory::Bool;
  } else if (hookName == "KVAR.KVar") {
    category = SortCategory::Variable;
    // we expect the "hook" of a MInt to be of the form "MINT.MInt N" for some
    // bitwidth N
  } else if (hookName.substr(0, 10) == "MINT.MInt ") {
    category = SortCategory::MInt;
    bits = std::stoi(hookName.substr(10));
  } else {
    category = SortCategory::Symbol;
  }
  return {category, bits};
}

void kore_symbol::addArgument(sptr<kore_sort> const &Argument) {
  arguments.push_back(Argument);
}

void kore_symbol::addFormalArgument(sptr<kore_sort> const &Argument) {
  formalArguments.push_back(Argument);
}

void kore_symbol::addSort(sptr<kore_sort> Sort) {
  sort = std::move(Sort);
}

bool kore_symbol::operator==(kore_symbol const &other) const {
  if (name != other.name || arguments.size() != other.arguments.size()) {
    return false;
  }
  for (int i = 0; i < arguments.size(); ++i) {
    if (*arguments[i] != *other.arguments[i]) {
      return false;
    }
  }
  return true;
}

std::string kore_symbol::layoutString(kore_definition *definition) const {
  std::string result;
  for (auto const &arg : arguments) {
    auto *sort = dynamic_cast<kore_composite_sort *>(arg.get());
    value_type cat = sort->getCategory(definition);
    switch (cat.cat) {
    case SortCategory::Map: result.push_back('1'); break;
    case SortCategory::RangeMap: result.push_back('b'); break;
    case SortCategory::List: result.push_back('2'); break;
    case SortCategory::Set: result.push_back('3'); break;
    case SortCategory::Int: result.push_back('4'); break;
    case SortCategory::Float: result.push_back('5'); break;
    case SortCategory::StringBuffer: result.push_back('6'); break;
    case SortCategory::Bool: result.push_back('7'); break;
    case SortCategory::Variable: result.push_back('8'); break;
    case SortCategory::MInt:
      result.append("_" + std::to_string(cat.bits) + "_");
    case SortCategory::Symbol: result.push_back('0'); break;
    case SortCategory::Uncomputed: abort();
    }
  }
  return result;
}

bool kore_composite_sort::isConcrete() const {
  return std::all_of(arguments.begin(), arguments.end(), [](auto const &sort) {
    return sort->isConcrete();
  });
}

bool kore_symbol::isConcrete() const {
  return std::all_of(arguments.begin(), arguments.end(), [](auto const &sort) {
    return sort->isConcrete();
  });
}

bool kore_symbol::isPolymorphic() const {
  return std::none_of(arguments.begin(), arguments.end(), [](auto const &sort) {
    return sort->isConcrete();
  });
}

static std::unordered_set<std::string> BUILTINS{
    "\\and",
    "\\not",
    "\\or",
    "\\implies",
    "\\iff",
    "\\forall",
    "\\exists",
    "\\ceil",
    "\\floor",
    "\\equals",
    "\\in",
    "\\top",
    "\\bottom",
    "\\dv",
    "\\rewrites",
    "\\next",
    "\\mu",
    "\\nu",
    "weakAlwaysFinally",
    "weakExistsFinally",
    "allPathGlobally",
};

bool kore_symbol::isBuiltin() const {
  return BUILTINS.contains(name);
}

void kore_symbol::instantiateSymbol(kore_symbol_declaration *decl) {
  std::vector<sptr<kore_sort>> instantiated;
  int i = 0;
  kore_sort::substitution vars;
  for (auto const &var : decl->getObjectSortVariables()) {
    vars.emplace(*var, formalArguments[i++]);
  }
  for (auto const &sort : decl->getSymbol()->getArguments()) {
    instantiated.push_back(sort->substitute(vars));
  }
  auto returnSort = decl->getSymbol()->sort;
  sort = returnSort->substitute(vars);

  arguments = instantiated;
}

std::string kore_variable::getName() const {
  return name;
}

std::string kore_variable_pattern::getName() const {
  return name->getName();
}

void kore_composite_pattern::addArgument(sptr<kore_pattern> const &Argument) {
  arguments.push_back(Argument);
}

void kore_composite_pattern::markSymbols(
    std::map<std::string, std::vector<kore_symbol *>> &map) {
  if (!constructor->isBuiltin()) {
    if (!map.contains(constructor->getName())) {
      map.emplace(constructor->getName(), std::vector<kore_symbol *>{});
    }
    map.at(constructor->getName()).push_back(constructor.get());
  }
  for (auto &arg : arguments) {
    arg->markSymbols(map);
  }
}

void kore_composite_pattern::markVariables(
    std::map<std::string, kore_variable_pattern *> &map) {
  for (auto &arg : arguments) {
    arg->markVariables(map);
  }
}

sptr<kore_pattern> kore_composite_pattern::substitute(substitution const &subst) {
  if (arguments.empty()) {
    return shared_from_this();
  }
  auto ptr = kore_composite_pattern::Create(constructor.get());
  auto name = constructor->getName();
  if (name == "\\forall" || name == "\\exists") {
    ptr->addArgument(arguments[0]);
    auto newSubst = subst;
    newSubst.erase(
        dynamic_cast<kore_variable_pattern *>(arguments[0].get())->getName());
    ptr->addArgument(arguments[1]->substitute(newSubst));
    return ptr;
  }
  for (auto &arg : arguments) {
    ptr->addArgument(arg->substitute(subst));
  }
  return ptr;
}

sptr<kore_pattern> kore_composite_pattern::expandAliases(kore_definition *def) {
  if (def->getAliasDeclarations().contains(constructor->getName())) {
    auto *alias = def->getAliasDeclarations().at(constructor->getName());
    auto subst = alias->getSubstitution(this);
    return alias->getPattern()->substitute(subst)->expandAliases(def);
  }
  if (arguments.empty()) {
    return shared_from_this();
  }
  auto ptr = kore_composite_pattern::Create(constructor.get());
  for (auto &arg : arguments) {
    ptr->addArgument(arg->expandAliases(def));
  }
  return ptr;
}

static int indent = 0;
static bool atNewLine = true;

static void newline(std::ostream &out) {
  out << std::endl;
  atNewLine = true;
}

static void printIndent(std::ostream &out) {
  constexpr auto indent_size = 2;

  if (atNewLine) {
    for (int i = 0; i < indent_size * indent; i++) {
      out << ' ';
    }
    atNewLine = false;
  }
}

static void append(std::ostream &out, char c) {
  printIndent(out);
  out << c;
}

static void append(std::ostream &out, std::string const &str) {
  printIndent(out);
  out << str;
}

static void color(
    std::ostream &out, std::string const &color, pretty_print_data const &data) {
  if (data.hasColor) {
    static bool once = true;
    static std::map<std::string, std::string> colors;
    if (once) {
      colors["MediumOrchid"] = "\x1b[38;5;134m";
      colors["Black"] = "\x1b[38;5;16m";
      colors["DarkSeaGreen"] = "\x1b[38;5;108m";
      colors["Sienna"] = "\x1b[38;5;130m";
      colors["Gainsboro"] = "\x1b[38;5;188m";
      colors["LightCoral"] = "\x1b[38;5;210m";
      colors["DarkGrey"] = "\x1b[38;5;145m";
      colors["DodgerBlue"] = "\x1b[38;5;33m";
      colors["CarnationPink"] = "\x1b[38;5;211m";
      colors["Aquamarine"] = "\x1b[38;5;122m";
      colors["Beige"] = "\x1b[38;5;230m";
      colors["OliveDrab"] = "\x1b[38;5;64m";
      colors["YellowOrange"] = "\x1b[38;5;214m";
      colors["Mulberry"] = "\x1b[38;5;126m";
      colors["Violet"] = "\x1b[38;5;213m";
      colors["LimeGreen"] = "\x1b[38;5;40m";
      colors["PaleGoldenrod"] = "\x1b[38;5;187m";
      colors["Magenta"] = "\x1b[38;5;201m";
      colors["PowderBlue"] = "\x1b[38;5;152m";
      colors["DarkTurquoise"] = "\x1b[38;5;44m";
      colors["IndianRed"] = "\x1b[38;5;167m";
      colors["LightGray"] = "\x1b[38;5;188m";
      colors["PeachPuff"] = "\x1b[38;5;223m";
      colors["LightBlue"] = "\x1b[38;5;152m";
      colors["ProcessBlue"] = "\x1b[38;5;39m";
      colors["SpringGreen"] = "\x1b[38;5;48m";
      colors["Indigo"] = "\x1b[38;5;54m";
      colors["RedViolet"] = "\x1b[38;5;125m";
      colors["DarkRed"] = "\x1b[38;5;88m";
      colors["Wheat"] = "\x1b[38;5;223m";
      colors["DarkCyan"] = "\x1b[38;5;30m";
      colors["yellow"] = "\x1b[38;5;226m";
      colors["LawnGreen"] = "\x1b[38;5;118m";
      colors["gray"] = "\x1b[38;5;102m";
      colors["DarkOrange"] = "\x1b[38;5;208m";
      colors["Teal"] = "\x1b[38;5;30m";
      colors["Maroon"] = "\x1b[38;5;88m";
      colors["JungleGreen"] = "\x1b[38;5;37m";
      colors["Blue"] = "\x1b[38;5;21m";
      colors["Periwinkle"] = "\x1b[38;5;104m";
      colors["Moccasin"] = "\x1b[38;5;223m";
      colors["black"] = "\x1b[38;5;16m";
      colors["Chocolate"] = "\x1b[38;5;166m";
      colors["teal"] = "\x1b[38;5;30m";
      colors["SeaGreen"] = "\x1b[38;5;29m";
      colors["Thistle"] = "\x1b[38;5;182m";
      colors["Red"] = "\x1b[38;5;196m";
      colors["blue"] = "\x1b[38;5;21m";
      colors["MistyRose"] = "\x1b[38;5;224m";
      colors["purple"] = "\x1b[38;5;161m";
      colors["Crimson"] = "\x1b[38;5;197m";
      colors["DarkGray"] = "\x1b[38;5;145m";
      colors["Fuchsia"] = "\x1b[38;5;201m";
      colors["DarkBlue"] = "\x1b[38;5;18m";
      colors["red"] = "\x1b[38;5;196m";
      colors["Cornsilk"] = "\x1b[38;5;230m";
      colors["white"] = "\x1b[38;5;231m";
      colors["DarkSlateGrey"] = "\x1b[38;5;23m";
      colors["PaleGreen"] = "\x1b[38;5;120m";
      colors["DimGray"] = "\x1b[38;5;59m";
      colors["lightgray"] = "\x1b[38;5;145m";
      colors["Gold"] = "\x1b[38;5;220m";
      colors["Gray"] = "\x1b[38;5;102m";
      colors["DarkOliveGreen"] = "\x1b[38;5;58m";
      colors["LemonChiffon"] = "\x1b[38;5;230m";
      colors["brown"] = "\x1b[38;5;137m";
      colors["NavyBlue"] = "\x1b[38;5;18m";
      colors["FloralWhite"] = "\x1b[38;5;231m";
      colors["LightGoldenrod"] = "\x1b[38;5;186m";
      colors["GreenYellow"] = "\x1b[38;5;154m";
      colors["Silver"] = "\x1b[38;5;145m";
      colors["Khaki"] = "\x1b[38;5;186m";
      colors["Ivory"] = "\x1b[38;5;231m";
      colors["LightSkyBlue"] = "\x1b[38;5;117m";
      colors["DarkGreen"] = "\x1b[38;5;22m";
      colors["DarkSalmon"] = "\x1b[38;5;173m";
      colors["TealBlue"] = "\x1b[38;5;37m";
      colors["Linen"] = "\x1b[38;5;231m";
      colors["LightGoldenrodYellow"] = "\x1b[38;5;230m";
      colors["LightGreen"] = "\x1b[38;5;120m";
      colors["LightGrey"] = "\x1b[38;5;188m";
      colors["Brown"] = "\x1b[38;5;124m";
      colors["Aqua"] = "\x1b[38;5;51m";
      colors["Cerulean"] = "\x1b[38;5;39m";
      colors["Peach"] = "\x1b[38;5;209m";
      colors["Bisque"] = "\x1b[38;5;223m";
      colors["MediumBlue"] = "\x1b[38;5;20m";
      colors["BlueViolet"] = "\x1b[38;5;93m";
      colors["RubineRed"] = "\x1b[38;5;198m";
      colors["Lavender"] = "\x1b[38;5;189m";
      colors["CornflowerBlue"] = "\x1b[38;5;68m";
      colors["Goldenrod"] = "\x1b[38;5;178m";
      colors["Grey"] = "\x1b[38;5;102m";
      colors["MediumSpringGreen"] = "\x1b[38;5;49m";
      colors["DarkKhaki"] = "\x1b[38;5;143m";
      colors["green"] = "\x1b[38;5;46m";
      colors["ForestGreen"] = "\x1b[38;5;28m";
      colors["DarkOrchid"] = "\x1b[38;5;128m";
      colors["White"] = "\x1b[38;5;231m";
      colors["Purple"] = "\x1b[38;5;90m";
      colors["DarkMagenta"] = "\x1b[38;5;90m";
      colors["BlueGreen"] = "\x1b[38;5;37m";
      colors["Green"] = "\x1b[38;5;28m";
      colors["Melon"] = "\x1b[38;5;216m";
      colors["SkyBlue"] = "\x1b[38;5;117m";
      colors["Rhodamine"] = "\x1b[38;5;205m";
      colors["Apricot"] = "\x1b[38;5;216m";
      colors["RedOrange"] = "\x1b[38;5;202m";
      colors["LightSlateGray"] = "\x1b[38;5;102m";
      colors["cyan"] = "\x1b[38;5;51m";
      colors["Orange"] = "\x1b[38;5;214m";
      colors["DarkSlateGray"] = "\x1b[38;5;23m";
      colors["DimGrey"] = "\x1b[38;5;59m";
      colors["LightSeaGreen"] = "\x1b[38;5;37m";
      colors["RoyalBlue"] = "\x1b[38;5;62m";
      colors["darkgray"] = "\x1b[38;5;59m";
      colors["Sepia"] = "\x1b[38;5;52m";
      colors["DarkViolet"] = "\x1b[38;5;92m";
      colors["MediumAquamarine"] = "\x1b[38;5;79m";
      colors["MediumSlateBlue"] = "\x1b[38;5;99m";
      colors["Dandelion"] = "\x1b[38;5;214m";
      colors["MidnightBlue"] = "\x1b[38;5;18m";
      colors["SandyBrown"] = "\x1b[38;5;215m";
      colors["violet"] = "\x1b[38;5;90m";
      colors["DarkSlateBlue"] = "\x1b[38;5;61m";
      colors["DeepSkyBlue"] = "\x1b[38;5;74m";
      colors["Chartreuse"] = "\x1b[38;5;118m";
      colors["Olive"] = "\x1b[38;5;100m";
      colors["MediumPurple"] = "\x1b[38;5;98m";
      colors["Yellow"] = "\x1b[38;5;226m";
      colors["Peru"] = "\x1b[38;5;173m";
      colors["RosyBrown"] = "\x1b[38;5;138m";
      colors["pink"] = "\x1b[38;5;217m";
      colors["FireBrick"] = "\x1b[38;5;124m";
      colors["RawSienna"] = "\x1b[38;5;124m";
      colors["VioletRed"] = "\x1b[38;5;162m";
      colors["OrangeRed"] = "\x1b[38;5;202m";
      colors["Bittersweet"] = "\x1b[38;5;130m";
      colors["Turquoise"] = "\x1b[38;5;80m";
      colors["Cyan"] = "\x1b[38;5;51m";
      colors["WhiteSmoke"] = "\x1b[38;5;231m";
      colors["MediumSeaGreen"] = "\x1b[38;5;35m";
      colors["LavenderBlush"] = "\x1b[38;5;231m";
      colors["LightCyan"] = "\x1b[38;5;195m";
      colors["PineGreen"] = "\x1b[38;5;29m";
      colors["OliveGreen"] = "\x1b[38;5;28m";
      colors["SlateGray"] = "\x1b[38;5;102m";
      colors["LightSlateBlue"] = "\x1b[38;5;99m";
      colors["NavajoWhite"] = "\x1b[38;5;223m";
      colors["SlateBlue"] = "\x1b[38;5;62m";
      colors["Orchid"] = "\x1b[38;5;170m";
      colors["Tan"] = "\x1b[38;5;180m";
      colors["LightSalmon"] = "\x1b[38;5;216m";
      colors["Seashell"] = "\x1b[38;5;231m";
      colors["Snow"] = "\x1b[38;5;231m";
      colors["WildStrawberry"] = "\x1b[38;5;197m";
      colors["Tomato"] = "\x1b[38;5;203m";
      colors["RoyalPurple"] = "\x1b[38;5;61m";
      colors["LightSlateGrey"] = "\x1b[38;5;102m";
      colors["Plum"] = "\x1b[38;5;182m";
      colors["YellowGreen"] = "\x1b[38;5;112m";
      colors["olive"] = "\x1b[38;5;100m";
      colors["MintCream"] = "\x1b[38;5;231m";
      colors["PaleVioletRed"] = "\x1b[38;5;168m";
      colors["Azure"] = "\x1b[38;5;231m";
      colors["BurntOrange"] = "\x1b[38;5;208m";
      colors["Salmon"] = "\x1b[38;5;210m";
      colors["BlanchedAlmond"] = "\x1b[38;5;223m";
      colors["Pink"] = "\x1b[38;5;217m";
      colors["AliceBlue"] = "\x1b[38;5;231m";
      colors["PapayaWhip"] = "\x1b[38;5;230m";
      colors["Honeydew"] = "\x1b[38;5;231m";
      colors["MediumTurquoise"] = "\x1b[38;5;44m";
      colors["AntiqueWhite"] = "\x1b[38;5;231m";
      colors["magenta"] = "\x1b[38;5;201m";
      colors["LightPink"] = "\x1b[38;5;217m";
      colors["OldLace"] = "\x1b[38;5;231m";
      colors["CadetBlue"] = "\x1b[38;5;73m";
      colors["BurlyWood"] = "\x1b[38;5;180m";
      colors["Lime"] = "\x1b[38;5;46m";
      colors["BrickRed"] = "\x1b[38;5;124m";
      colors["Emerald"] = "\x1b[38;5;37m";
      colors["LightSteelBlue"] = "\x1b[38;5;153m";
      colors["Mahogany"] = "\x1b[38;5;124m";
      colors["GhostWhite"] = "\x1b[38;5;231m";
      colors["SteelBlue"] = "\x1b[38;5;67m";
      colors["PaleTurquoise"] = "\x1b[38;5;159m";
      colors["DarkGoldenrod"] = "\x1b[38;5;136m";
      colors["lime"] = "\x1b[38;5;154m";
      colors["DeepPink"] = "\x1b[38;5;198m";
      colors["MediumVioletRed"] = "\x1b[38;5;162m";
      colors["orange"] = "\x1b[38;5;220m";
      colors["HotPink"] = "\x1b[38;5;205m";
      colors["LightYellow"] = "\x1b[38;5;230m";
      colors["Navy"] = "\x1b[38;5;18m";
      colors["SaddleBrown"] = "\x1b[38;5;94m";
      colors["Coral"] = "\x1b[38;5;209m";
      colors["SlateGrey"] = "\x1b[38;5;102m";
      once = false;
    }
    append(out, colors[color]);
  }
}

#define RESET_COLOR "\x1b[0m"

std::string enquote(std::string const &str) {
  std::string result;
  result.push_back('"');
  for (char c : str) {
    switch (c) {
    case '\\': result.append("\\\\"); break;
    case '"': result.append("\\\""); break;
    case '\n': result.append("\\n"); break;
    case '\t': result.append("\\t"); break;
    case '\r': result.append("\\r"); break;
    case '\f': result.append("\\f"); break;
    default:
      if ((unsigned char)c >= 32 && (unsigned char)c < 127) {
        result.push_back(c);
      } else {
        fmt::format_to(
            std::back_inserter(result), "\\x{:02x}",
            static_cast<unsigned char>(c));
      }
      break;
    }
  }
  result.push_back('"');
  return result;
}

void kore_sort_variable::prettyPrint(std::ostream &out) const {
  append(out, name);
}

void kore_composite_sort::prettyPrint(std::ostream &out) const {
  append(out, name.substr(4));
  if (!arguments.empty()) {
    append(out, '{');
    std::string conn;
    for (auto const &sort : arguments) {
      append(out, conn);
      sort->prettyPrint(out);
      conn = ",";
    }
    append(out, '}');
  }
}

void kore_variable_pattern::prettyPrint(
    std::ostream &out, pretty_print_data const &data) const {
  append(out, decodeKore(getName().substr(3)));
  append(out, ':');
  sort->prettyPrint(out);
}

// NOLINTNEXTLINE(*-cognitive-complexity)
void kore_composite_pattern::prettyPrint(
    std::ostream &out, pretty_print_data const &data) const {
  std::string name = getConstructor()->getName();
  if (name == "\\dv") {
    auto *s = dynamic_cast<kore_composite_sort *>(
        getConstructor()->getFormalArguments()[0].get());
    bool hasHook = data.hook.contains(s->getName());
    auto *str = dynamic_cast<kore_string_pattern *>(arguments[0].get());
    if (hasHook) {
      auto hook = data.hook.at(s->getName());
      if (hook == "STRING.String") {
        append(out, enquote(str->getContents()));
      } else if (hook == "BYTES.Bytes") {
        append(out, 'b');
        append(out, enquote(str->getContents()));
      } else {
        append(out, str->getContents());
      }
    } else {
      append(out, str->getContents());
    }
    return;
  }
  if (data.format.contains(name)) {
    auto format = data.format.at(name);
    int localIndent = 0;
    int localColor = 0;
    for (int i = 0; i < format.length(); ++i) {
      char c = format[i];
      if (c == '%') {
        if (i == format.length() - 1) {
          abort();
        }
        char c2 = format[i + 1];
        ++i;
        switch (c2) {
        case 'n': newline(out); break;
        case 'i':
          indent++;
          localIndent++;
          break;
        case 'd':
          indent--;
          localIndent--;
          break;
        case 'c':
          if (data.colors.contains(name)) {
            if (localColor >= data.colors.at(name).size()) {
              abort();
            }
            color(out, data.colors.at(name)[localColor++], data);
          }
          break;
        case 'r':
          if (data.hasColor) {
            append(out, RESET_COLOR);
          }
          break;
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9': {
          std::string buf;
          for (; i < format.length() && format[i] >= '0' && format[i] <= '9';
               i++) {
            buf.push_back(format[i]);
          }
          i--;
          int idx = std::stoi(buf);
          if (idx == 0 || idx > arguments.size()) {
            abort();
          }
          kore_pattern *inner = arguments[idx - 1].get();
          bool assoc = false;
          if (auto *app = dynamic_cast<kore_composite_pattern *>(inner)) {
            if (app->getConstructor()->getName() == constructor->getName()
                && data.assoc.contains(name)) {
              assoc = true;
            }
            if (assoc) {
              for (int j = 0; j < localIndent; j++) {
                indent--;
              }
            }
            inner->prettyPrint(out, data);
            if (assoc) {
              for (int j = 0; j < localIndent; j++) {
                indent++;
              }
            }
          } else {
            inner->prettyPrint(out, data);
          }
          break;
        }
        default: append(out, c2);
        }
      } else {
        append(out, c);
      }
    }
  } else {
    abort();
  }
}

struct compare_first {
  static bool isDigit(char c) { return c >= '0' && c <= '9'; }

  static std::string getChunk(std::string s, size_t slength, size_t marker) {
    std::string chunk;
    char c = s[marker];
    chunk.push_back(c);
    marker++;
    if (isDigit(c)) {
      while (marker < slength) {
        c = s[marker];
        if (!isDigit(c)) {
          break;
        }
        chunk.push_back(c);
        marker++;
      }
    } else {
      while (marker < slength) {
        c = s[marker];
        if (isDigit(c)) {
          break;
        }
        chunk.push_back(c);
        marker++;
      }
    }
    return chunk;
  }

  template <typename T>
  bool operator()(std::pair<std::string, T> a, std::pair<std::string, T> b) {
    std::string s1 = a.first;
    std::string s2 = b.first;
    size_t thisMarker = 0;
    size_t thatMarker = 0;
    size_t s1length = s1.length();
    size_t s2length = s2.length();
    while (thisMarker < s1length && thatMarker < s2length) {
      std::string thisChunk = getChunk(s1, s1length, thisMarker);
      thisMarker += thisChunk.length();
      std::string thatChunk = getChunk(s2, s2length, thatMarker);
      thatMarker += thatChunk.length();
      int result = 0;
      if (isDigit(thisChunk[0]) && isDigit(thatChunk[0])) {
        size_t thisChunkLength = thisChunk.length();
        result = thisChunkLength - thatChunk.length();
        if (result == 0) {
          for (int i = 0; i < thisChunkLength; i++) {
            result = thisChunk[i] - thatChunk[i];
            if (result != 0) {
              return result < 0;
            }
          }
        }
      } else {
        result = thisChunk.compare(thatChunk);
      }
      if (result != 0) {
        return result < 0;
      }
    }
    return s1length < s2length;
  }
};

void kllvm::flatten(
    kore_composite_pattern *pat, std::string const &name,
    std::vector<sptr<kore_pattern>> &result) {
  for (auto const &arg : pat->getArguments()) {
    if (auto *pat2 = dynamic_cast<kore_composite_pattern *>(arg.get())) {
      if (pat2->getConstructor()->getName() == name) {
        flatten(pat2, name, result);
      } else {
        result.push_back(arg);
      }
    } else {
      result.push_back(arg);
    }
  }
}

sptr<kore_pattern>
kore_composite_pattern::sortCollections(pretty_print_data const &data) {
  if (arguments.empty()) {
    return shared_from_this();
  }
  std::string name = getConstructor()->getName();
  if (data.comm.contains(name) && data.assoc.contains(name)) {
    std::vector<sptr<kore_pattern>> items;
    flatten(this, name, items);
    std::vector<std::pair<std::string, sptr<kore_pattern>>> printed;
    int oldIndent = indent;
    bool oldAtNewLine = atNewLine;
    atNewLine = false;
    indent = 0;
    pretty_print_data newData = data;
    newData.hasColor = false;
    for (auto &item : items) {
      std::ostringstream Out;
      item = item->sortCollections(data);
      item->prettyPrint(Out, newData);
      printed.emplace_back(Out.str(), item);
    }
    indent = oldIndent;
    atNewLine = oldAtNewLine;
    std::sort(printed.begin(), printed.end(), compare_first{});
    items.clear();
    for (auto &item : printed) {
      items.push_back(item.second);
    }
    sptr<kore_pattern> result = items[0];
    for (int i = 1; i < items.size(); ++i) {
      sptr<kore_composite_pattern> tmp
          = kore_composite_pattern::Create(constructor.get());
      tmp->addArgument(result);
      tmp->addArgument(items[i]);
      result = tmp;
    }
    return result;
  }
  sptr<kore_composite_pattern> result
      = kore_composite_pattern::Create(constructor.get());
  for (auto &arg : arguments) {
    result->addArgument(arg->sortCollections(data));
  }
  return result;
}

std::set<std::string> kore_pattern::gatherSingletonVars() {
  auto counts = gatherVarCounts();
  std::set<std::string> result;
  for (auto const &entry : counts) {
    if (entry.second == 1) {
      result.insert(entry.first);
    }
  }
  return result;
}

std::map<std::string, int> kore_composite_pattern::gatherVarCounts() {
  std::map<std::string, int> result;
  for (auto &arg : arguments) {
    auto childResult = arg->gatherVarCounts();
    for (auto const &entry : childResult) {
      result[entry.first] += entry.second;
    }
  }
  return result;
}

sptr<kore_pattern> kore_composite_pattern::dedupeDisjuncts() {
  if (constructor->getName() != "\\or") {
    return shared_from_this();
  }
  std::vector<sptr<kore_pattern>> items;
  std::vector<sptr<kore_pattern>> dedupedItems;
  flatten(this, "\\or", items);
  std::set<std::string> printed;
  for (auto const &item : items) {
    if (printed.insert(ast_to_string(*item)).second) {
      dedupedItems.push_back(item);
    }
  }
  sptr<kore_pattern> result = dedupedItems[0];
  for (int i = 1; i < dedupedItems.size(); ++i) {
    sptr<kore_composite_pattern> tmp
        = kore_composite_pattern::Create(constructor.get());
    tmp->addArgument(result);
    tmp->addArgument(dedupedItems[i]);
    result = tmp;
  }
  return result;
}

// NOLINTNEXTLINE(*-cognitive-complexity)
sptr<kore_pattern> kore_composite_pattern::filterSubstitution(
    pretty_print_data const &data, std::set<std::string> const &vars) {
  if (constructor->getName() == "\\equals") {
    if (auto *var = dynamic_cast<kore_variable_pattern *>(arguments[0].get())) {
      std::ostringstream ss;
      int oldIndent = indent;
      bool oldAtNewLine = atNewLine;
      atNewLine = true;
      indent = 0;
      var->prettyPrint(ss, data);
      indent = oldIndent;
      atNewLine = oldAtNewLine;
      std::string name = ss.str();
      if (vars.contains(var->getName())
          && (name[0] == '_'
              || (name.size() > 1
                  && (name[0] == '@' || name[0] == '!' || name[0] == '?')
                  && name[1] == '_'))) {
        sptr<kore_composite_pattern> unit = kore_composite_pattern::Create("\\top");
        unit->getConstructor()->addFormalArgument(
            constructor->getFormalArguments()[1]);
        return unit;
      }
      return shared_from_this();
    }
    return shared_from_this();
  }
  if (constructor->getName() == "\\and" || constructor->getName() == "\\or") {
    sptr<kore_composite_pattern> result
        = kore_composite_pattern::Create(constructor.get());
    for (auto &arg : arguments) {
      if (constructor->getName() == "\\or") {
        std::set<std::string> vars = arg->gatherSingletonVars();
        result->addArgument(arg->filterSubstitution(data, vars));
      } else {
        result->addArgument(arg->filterSubstitution(data, vars));
      }
    }
    if (constructor->getName() == "\\and") {
      if (auto *composite = dynamic_cast<kore_composite_pattern *>(
              result->getArguments()[0].get())) {
        if (composite->getConstructor()->getName() == "\\top") {
          return result->getArguments()[1];
        }
      }
      if (auto *composite = dynamic_cast<kore_composite_pattern *>(
              result->getArguments()[1].get())) {
        if (composite->getConstructor()->getName() == "\\top") {
          return result->getArguments()[0];
        }
      }
    }
    return result;
  }
  return shared_from_this();
}

sptr<kore_pattern> kore_composite_pattern::desugarAssociative() {
  if (constructor->getName() == "\\left-assoc") {
    if (auto *comp_arg
        = dynamic_cast<kore_composite_pattern *>(arguments[0].get())) {
      auto accum = comp_arg->arguments[0]->desugarAssociative();

      for (auto i = 1U; i < comp_arg->arguments.size(); i++) {
        auto new_accum
            = kore_composite_pattern::Create(comp_arg->getConstructor());
        new_accum->addArgument(accum);
        new_accum->addArgument(comp_arg->arguments[i]->desugarAssociative());
        accum = ptr<kore_pattern>(new_accum.release());
      }

      return accum;
    }
  } else if (constructor->getName() == "\\right-assoc") {
    if (auto *comp_arg
        = dynamic_cast<kore_composite_pattern *>(arguments[0].get())) {
      auto accum = comp_arg->arguments[comp_arg->arguments.size() - 1]
                       ->desugarAssociative();

      for (int i = comp_arg->arguments.size() - 2; i >= 0; i--) {
        auto new_accum
            = kore_composite_pattern::Create(comp_arg->getConstructor());
        new_accum->addArgument(comp_arg->arguments[i]->desugarAssociative());
        new_accum->addArgument(accum);
        accum = ptr<kore_pattern>(new_accum.release());
      }

      return accum;
    }
  } else {
    auto result = kore_composite_pattern::Create(constructor.get());

    for (auto &arg : arguments) {
      result->addArgument(arg->desugarAssociative());
    }

    return result;
  }

  return shared_from_this();
}

sptr<kore_pattern> kore_composite_pattern::unflattenAndOr() {
  if ((constructor->getName() == "\\and" || constructor->getName() == "\\or")
      && arguments.size() != 2) {
    if (arguments.size() == 1) {
      return arguments[0]->unflattenAndOr();
    }
    auto accum = arguments[0]->unflattenAndOr();

    for (auto i = 1U; i < arguments.size(); i++) {
      auto new_accum = kore_composite_pattern::Create(constructor.get());
      new_accum->addArgument(accum);
      new_accum->addArgument(arguments[i]->unflattenAndOr());
      accum = ptr<kore_pattern>(new_accum.release());
    }

    return accum;
  }
  auto result = kore_composite_pattern::Create(constructor.get());

  for (auto &arg : arguments) {
    result->addArgument(arg->unflattenAndOr());
  }

  return result;
}

sptr<kore_pattern> kore_composite_pattern::expandMacros(
    SubsortMap const &subsorts, SymbolMap const &overloads,
    std::vector<ptr<kore_declaration>> const &macros, bool reverse,
    std::set<size_t> &appliedRules, std::set<std::string> const &macroSymbols) {
  sptr<kore_composite_pattern> applied
      = kore_composite_pattern::Create(constructor.get());
  for (auto &arg : arguments) {
    std::set<size_t> dummyApplied;
    applied->addArgument(arg->expandMacros(
        subsorts, overloads, macros, reverse, dummyApplied, macroSymbols));
  }

  if (macroSymbols.find(constructor->getName()) == macroSymbols.end()) {
    return applied;
  }

  size_t i = 0;
  for (auto const &decl : macros) {
    if ((decl->attributes().contains(attribute_set::key::macro)
         || decl->attributes().contains(attribute_set::key::macro_rec))
        && reverse) {
      i++;
      continue;
    }
    auto *axiom = dynamic_cast<kore_axiom_declaration *>(decl.get());
    auto *equals
        = dynamic_cast<kore_composite_pattern *>(axiom->getPattern().get());
    auto lhs = equals->arguments[reverse ? 1 : 0];
    auto rhs = equals->arguments[reverse ? 0 : 1];
    substitution subst;
    bool matches = lhs->matches(subst, subsorts, overloads, applied);
    if (matches
        && (decl->attributes().contains(attribute_set::key::macro_rec)
            || decl->attributes().contains(attribute_set::key::alias_rec)
            || !appliedRules.contains(i))) {
      std::set<size_t> oldAppliedRules = appliedRules;
      appliedRules.insert(i);
      auto result = rhs->substitute(subst)->expandMacros(
          subsorts, overloads, macros, reverse, appliedRules, macroSymbols);
      appliedRules = oldAppliedRules;
      return result;
    }
    i++;
  }
  return applied;
}

bool kore_variable_pattern::matches(
    substitution &subst, SubsortMap const &subsorts, SymbolMap const &overloads,
    sptr<kore_pattern> subject) {
  if (subst[name->getName()]) {
    return ast_to_string(*subst[name->getName()]) == ast_to_string(*subject);
  }
  subst[name->getName()] = subject;
  return true;
}

// NOLINTNEXTLINE(*-cognitive-complexity)
bool kore_composite_pattern::matches(
    substitution &subst, SubsortMap const &subsorts, SymbolMap const &overloads,
    sptr<kore_pattern> subject) {
  auto *subj = dynamic_cast<kore_composite_pattern *>(subject.get());
  if (!subj) {
    return false;
  }
  if (*subj->getConstructor() != *getConstructor()) {
    if (subj->getConstructor()->getName() == "inj"
        && getConstructor()->getName() == "inj") {
      if (*subj->getConstructor()->getFormalArguments()[1]
          != *getConstructor()->getFormalArguments()[1]) {
        return false;
      }
      sptr<kore_sort> a = subj->getConstructor()->getFormalArguments()[0];
      sptr<kore_sort> b = getConstructor()->getFormalArguments()[0];
      if (subsorts.contains(b.get())
          && subsorts.at(b.get()).contains(a.get())) {
        sptr<kore_composite_pattern> ba = kore_composite_pattern::Create("inj");
        ba->getConstructor()->addFormalArgument(b);
        ba->getConstructor()->addFormalArgument(a);
        ba->getConstructor()->addArgument(b);
        ba->addArgument(arguments[0]);
        return ba->matches(subst, subsorts, overloads, subj->getArguments()[0]);
      }
      if (subsorts.contains(a.get())
          && subsorts.at(a.get()).contains(b.get())) {
        sptr<kore_composite_pattern> ab = kore_composite_pattern::Create("inj");
        ab->getConstructor()->addFormalArgument(a);
        ab->getConstructor()->addFormalArgument(b);
        ab->getConstructor()->addArgument(a);
        ab->addArgument(subj->getArguments()[0]);
        return arguments[0]->matches(subst, subsorts, overloads, ab);
      }
      return false;
    }
    if (subj->getConstructor()->getName() == "inj") {
      sptr<kore_pattern> child = subj->getArguments()[0];
      if (auto *composite = dynamic_cast<kore_composite_pattern *>(child.get())) {
        if (overloads.contains(composite->getConstructor())
            && overloads.at(composite->getConstructor())
                   .contains(getConstructor())) {
          sptr<kore_composite_pattern> greater
              = kore_composite_pattern::Create(getConstructor());
          for (int i = 0; i < arguments.size(); i++) {
            if (*getConstructor()->getArguments()[i]
                != *composite->getConstructor()->getArguments()[i]) {
              sptr<kore_composite_pattern> inj
                  = kore_composite_pattern::Create("inj");
              inj->getConstructor()->addFormalArgument(
                  composite->getConstructor()->getArguments()[i]);
              inj->getConstructor()->addFormalArgument(
                  getConstructor()->getArguments()[i]);
              inj->getConstructor()->addArgument(
                  composite->getConstructor()->getArguments()[i]);
              inj->addArgument(composite->getArguments()[i]);
              greater->addArgument(inj);
            } else {
              greater->addArgument(composite->getArguments()[i]);
            }
          }
          return this->matches(subst, subsorts, overloads, greater);
        }
        return false;
      }
      return false;
    }
    return false;
  }
  if (subj->arguments.size() != arguments.size()) {
    return false;
  }
  bool match = true;
  for (int i = 0; i < subj->arguments.size(); i++) {
    match = match
            && arguments[i]->matches(
                subst, subsorts, overloads, subj->arguments[i]);
  }
  return match;
}

bool kore_string_pattern::matches(
    substitution &subst, SubsortMap const &subsorts, SymbolMap const &overloads,
    sptr<kore_pattern> subject) {
  auto *subj = dynamic_cast<kore_string_pattern *>(subject.get());
  if (!subj) {
    return false;
  }
  return subj->contents == contents;
}

void kore_declaration::addObjectSortVariable(
    sptr<kore_sort_variable> const &SortVariable) {
  objectSortVariables.push_back(SortVariable);
}

void kore_axiom_declaration::addPattern(sptr<kore_pattern> Pattern) {
  pattern = std::move(Pattern);
}

bool kore_axiom_declaration::isRequired() const {
  constexpr auto keys_to_drop = std::array{
      attribute_set::key::assoc,          attribute_set::key::comm,
      attribute_set::key::idem,           attribute_set::key::unit,
      attribute_set::key::functional,     attribute_set::key::constructor,
      attribute_set::key::total,          attribute_set::key::subsort,
      attribute_set::key::ceil,           attribute_set::key::non_executable,
      attribute_set::key::simplification,
  };

  return std::none_of(
      keys_to_drop.begin(), keys_to_drop.end(),
      [this](auto key) { return attributes().contains(key); });
}

bool kore_axiom_declaration::isTopAxiom() const {
  if (auto *top = dynamic_cast<kore_composite_pattern *>(pattern.get())) {
    if (top->getConstructor()->getName() == "\\implies"
        && top->getArguments().size() == 2) {
      if (auto *bottomPattern = dynamic_cast<kore_composite_pattern *>(
              top->getArguments()[0].get())) {
        if (bottomPattern->getConstructor()->getName() == "\\bottom"
            && bottomPattern->getArguments().empty()) {
          return true;
        }
      }
      return false;
    }
    if (top->getConstructor()->getName() == "\\rewrites"
        && top->getArguments().size() == 2) {
      return true;
    }
    if (top->getConstructor()->getName() == "\\and"
        && top->getArguments().size() == 2) {
      return true;
    }
  }
  return false;
}

void kore_alias_declaration::addVariables(sptr<kore_composite_pattern> Variables) {
  boundVariables = std::move(Variables);
}

void kore_alias_declaration::addPattern(sptr<kore_pattern> Pattern) {
  pattern = std::move(Pattern);
}

kore_pattern::substitution
kore_alias_declaration::getSubstitution(kore_composite_pattern *subject) {
  int i = 0;
  kore_pattern::substitution result;
  for (auto const &arg : boundVariables->getArguments()) {
    auto *var = dynamic_cast<kore_variable_pattern *>(arg.get());
    if (!var) {
      abort();
    }
    result[var->getName()] = subject->getArguments()[i++];
  }
  return result;
}

bool kore_symbol_declaration::isAnywhere() const {
  return attributes().contains(attribute_set::key::anywhere);
}

void kore_module::addDeclaration(sptr<kore_declaration> Declaration) {
  declarations.push_back(std::move(Declaration));
}

// Pretty printer
void kore_sort_variable::print(std::ostream &Out, unsigned indent) const {
  std::string Indent(indent, ' ');
  Out << Indent << name;
}

void kore_composite_sort::print(std::ostream &Out, unsigned indent) const {
  std::string Indent(indent, ' ');
  Out << Indent << name << "{";
  bool isFirst = true;
  for (auto const &Argument : arguments) {
    if (!isFirst) {
      Out << ",";
    }
    Argument->print(Out);
    isFirst = false;
  }
  Out << "}";
}

void kore_symbol::print(std::ostream &Out, unsigned indent) const {
  print(Out, indent, true);
}

void kore_symbol::print(std::ostream &Out, unsigned indent, bool formal) const {
  std::string Indent(indent, ' ');
  Out << Indent << name << "{";
  bool isFirst = true;
  for (auto const &Argument : (formal ? formalArguments : arguments)) {
    if (!isFirst) {
      Out << ", ";
    }
    Argument->print(Out);
    isFirst = false;
  }
  Out << "}";
}

void kore_variable::print(std::ostream &Out, unsigned indent) const {
  std::string Indent(indent, ' ');
  Out << Indent << name;
}

void kore_variable_pattern::print(std::ostream &Out, unsigned indent) const {
  std::string Indent(indent, ' ');
  Out << Indent;
  name->print(Out);
  Out << " : ";
  sort->print(Out);
}

void kore_composite_pattern::print(std::ostream &Out, unsigned indent) const {
  std::string Indent(indent, ' ');
  Out << Indent;
  constructor->print(Out);
  Out << "(";
  bool isFirst = true;
  for (auto const &Argument : arguments) {
    if (!isFirst) {
      Out << ",";
    }
    Argument->print(Out);
    isFirst = false;
  }
  Out << ")";
}

static std::string escapeString(std::string const &str) {
  auto result = std::string{};

  for (char c : str) {
    if (c == '"' || c == '\\' || !isprint(c)) {
      fmt::format_to(
          std::back_inserter(result), "\\x{:02x}",
          static_cast<unsigned char>(c));
    } else {
      result.push_back(c);
    }
  }

  return result;
}

void kore_string_pattern::print(std::ostream &Out, unsigned indent) const {
  std::string Indent(indent, ' ');
  Out << Indent << "\"" << escapeString(contents) << "\"";
}

static void printAttributeList(
    std::ostream &Out, attribute_set const &attributes, unsigned indent = 0) {

  std::string Indent(indent, ' ');
  Out << Indent << "[";
  bool isFirst = true;
  for (auto const &[name, pattern] : attributes) {
    if (!isFirst) {
      Out << ",";
    }
    pattern->print(Out);
    isFirst = false;
  }
  Out << "]";
}

void kore_declaration::printSortVariables(std::ostream &Out) const {
  Out << "{";
  bool isFirst = true;
  for (auto const &Variable : objectSortVariables) {
    if (!isFirst) {
      Out << ",";
    }
    Variable->print(Out);
    isFirst = false;
  }
  Out << "}";
}

void kore_composite_sort_declaration::print(
    std::ostream &Out, unsigned indent) const {
  std::string Indent(indent, ' ');
  Out << Indent << (_isHooked ? "hooked-sort " : "sort ") << sortName;
  printSortVariables(Out);
  Out << " ";
  printAttributeList(Out, attributes());
}

void kore_symbol_declaration::print(std::ostream &Out, unsigned indent) const {
  std::string Indent(indent, ' ');
  Out << Indent << (_isHooked ? "hooked-symbol " : "symbol ")
      << getSymbol()->getName();
  printSortVariables(Out);
  Out << "(";
  bool isFirst = true;
  for (auto const &Argument : getSymbol()->getArguments()) {
    if (!isFirst) {
      Out << ",";
    }
    Argument->print(Out);
    isFirst = false;
  }
  Out << ") : ";
  getSymbol()->getSort()->print(Out);
  Out << " ";
  printAttributeList(Out, attributes());
}

void kore_alias_declaration::print(std::ostream &Out, unsigned indent) const {
  std::string Indent(indent, ' ');
  Out << Indent << "alias " << getSymbol()->getName();
  printSortVariables(Out);
  Out << "(";
  bool isFirst = true;
  for (auto const &Argument : getSymbol()->getArguments()) {
    if (!isFirst) {
      Out << ",";
    }
    Argument->print(Out);
    isFirst = false;
  }
  Out << ") : ";
  getSymbol()->getSort()->print(Out);
  Out << " where ";
  boundVariables->print(Out);
  Out << " := ";
  pattern->print(Out);
  Out << " ";
  printAttributeList(Out, attributes());
}

void kore_axiom_declaration::print(std::ostream &Out, unsigned indent) const {
  std::string Indent(indent, ' ');
  Out << Indent << (isClaim() ? "claim " : "axiom ");
  printSortVariables(Out);
  pattern->print(Out);
  Out << " ";
  printAttributeList(Out, attributes());
}

void kore_module_import_declaration::print(
    std::ostream &Out, unsigned indent) const {
  std::string Indent(indent, ' ');
  Out << Indent << "import " << moduleName;
  Out << " ";
  printAttributeList(Out, attributes());
}

void kore_module::print(std::ostream &Out, unsigned indent) const {
  std::string Indent(indent, ' ');
  Out << Indent << "module " << name << "\n";
  bool isFirst = true;
  for (auto const &Declaration : declarations) {
    if (!isFirst) {
      Out << "\n";
    }
    Declaration->print(Out, indent + 2);
    Out << "\n";
    isFirst = false;
  }
  Out << Indent << "endmodule\n";
  printAttributeList(Out, attributes(), indent);
}

void kore_definition::print(std::ostream &Out, unsigned indent) const {
  printAttributeList(Out, attributes(), indent);
  Out << "\n";
  for (auto const &Module : modules) {
    Out << "\n";
    Module->print(Out, indent);
    Out << "\n";
  }
}

// Binary serialization

void kore_variable_pattern::serialize_to(serializer &s) const {
  sort->serialize_to(s);
  s.emit(header_byte<kore_variable_pattern>);
  name->serialize_to(s);
}

void kore_composite_pattern::serialize_to(serializer &s) const {
  auto emit_this_arity = s.use_arity();
  s.reset_arity_flag();

  for (auto const &arg : arguments) {
    arg->serialize_to(s);
  }

  constructor->serialize_to(s);

  s.emit(header_byte<kore_composite_pattern>);

  if (emit_this_arity) {
    s.emit_length(arguments.size());
  }
}

void kore_string_pattern::serialize_to(serializer &s) const {
  s.emit(header_byte<kore_string_pattern>);
  s.emit_string(contents);
}

void kore_sort_variable::serialize_to(serializer &s) const {
  s.emit(header_byte<kore_sort_variable>);
  s.emit_string(name);
}

void kore_composite_sort::serialize_to(serializer &s) const {
  for (auto const &arg : arguments) {
    arg->serialize_to(s);
  }

  s.emit(header_byte<kore_composite_sort>);
  s.emit_length(arguments.size());
  s.emit_string(name);
}

void kore_symbol::serialize_to(serializer &s) const {
  for (auto const &arg : formalArguments) {
    arg->serialize_to(s);
  }

  s.emit(header_byte<kore_symbol>);
  s.emit_length(formalArguments.size());
  s.emit_string(name);
}

void kore_variable::serialize_to(serializer &s) const {
  s.emit(header_byte<kore_variable>);
  s.emit_string(name);
}

void kllvm::readMultimap(
    std::string const &name, kore_symbol_declaration *decl,
    std::map<std::string, std::set<std::string>> &output,
    attribute_set::key attName) {
  if (decl->attributes().contains(attName)) {
    kore_composite_pattern *att = decl->attributes().get(attName).get();
    for (auto const &pat : att->getArguments()) {
      auto *child = dynamic_cast<kore_composite_pattern *>(pat.get());
      output[name].insert(child->getConstructor()->getName());
    }
  }
}

// Normally, destruction of kore_pattern would call destructor
// of all its subpatterns. This can sometimes exhaust all the stack space.
// This function deallocates a pattern iteratively, without recursion.
void kllvm::deallocateSPtrKorePattern(sptr<kore_pattern> pattern) {
  std::vector<sptr<kore_pattern>> vec;
  vec.push_back(std::move(pattern));
  while (!vec.empty()) {
    sptr<kore_pattern> curr = std::move(vec.back());
    vec.pop_back();
    if (auto composite
        = std::dynamic_pointer_cast<kore_composite_pattern>(curr)) {
      vec.insert(
          vec.end(), std::make_move_iterator(composite->arguments.begin()),
          std::make_move_iterator(composite->arguments.end()));
    }
  }
}
