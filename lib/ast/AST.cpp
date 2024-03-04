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

sptr<kore_pattern> kore_pattern::expand_macros(
    SubsortMap const &subsorts, SymbolMap const &overloads,
    std::vector<ptr<kore_declaration>> const &axioms, bool reverse) {
  std::set<size_t> appliedRules;

  std::set<std::string> macroSymbols;

  for (auto const &decl : axioms) {
    auto *axiom = dynamic_cast<kore_axiom_declaration *>(decl.get());
    auto *equals
        = dynamic_cast<kore_composite_pattern *>(axiom->get_pattern().get());
    auto lhs = equals->get_arguments()[reverse ? 1 : 0];

    if (auto *lhs_comp = dynamic_cast<kore_composite_pattern *>(lhs.get())) {
      macroSymbols.insert(lhs_comp->get_constructor()->get_name());
    }
  }

  return expand_macros(
      subsorts, overloads, axioms, reverse, appliedRules, macroSymbols);
}

bool kore_sort_variable::operator==(kore_sort const &other) const {
  if (auto const *var = dynamic_cast<kore_sort_variable const *>(&other)) {
    return var->name_ == name_;
  }
  return false;
}

void kore_composite_sort::add_argument(sptr<kore_sort> const &argument) {
  arguments_.push_back(argument);
}

bool kore_composite_sort::operator==(kore_sort const &other) const {
  if (auto const *sort = dynamic_cast<kore_composite_sort const *>(&other)) {
    if (sort->name_ != name_ || sort->arguments_.size() != arguments_.size()) {
      return false;
    }
    for (int i = 0; i < arguments_.size(); ++i) {
      if (*sort->arguments_[i] != *arguments_[i]) {
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
  for (auto &arg : arguments_) {
    auto newArg = arg->substitute(subst);
    if (newArg != arg) {
      dirty = true;
    }
    newArgs.push_back(newArg);
  }
  if (dirty) {
    sptr<kore_composite_sort> retval = create(name_);
    retval->arguments_ = newArgs;
    return retval;
  }
  return shared_from_this();
}

value_type kore_composite_sort::get_category(kore_definition *definition) {
  if (category_.cat != sort_category::Uncomputed) {
    return category_;
  }
  std::string name = get_hook(definition);
  if (name == "MINT.MInt") {
    if (auto *param = dynamic_cast<kore_composite_sort *>(arguments_[0].get())) {
      auto const &att = definition->get_sort_declarations()
                            .at(param->get_name())
                            ->attributes();
      auto const &natAtt = att.get(attribute_set::key::Nat);
      assert(natAtt->get_arguments().size() == 1);
      auto *strPattern = dynamic_cast<kore_string_pattern *>(
          natAtt->get_arguments()[0].get());
      name = name + " " + strPattern->get_contents();
    } else {
      print(std::cerr);
      abort();
    }
  }
  category_ = get_category(name);
  return category_;
}

std::string kore_composite_sort::get_hook(kore_definition *definition) const {
  auto const &att
      = definition->get_sort_declarations().at(this->get_name())->attributes();
  if (!att.contains(attribute_set::key::Hook)) {
    return "STRING.String";
  }
  auto const &hookAtt = att.get(attribute_set::key::Hook);
  assert(hookAtt->get_arguments().size() == 1);
  auto *strPattern
      = dynamic_cast<kore_string_pattern *>(hookAtt->get_arguments()[0].get());
  return strPattern->get_contents();
}

value_type kore_composite_sort::get_category(std::string const &hook_name) {
  sort_category category = sort_category::Uncomputed;
  uint64_t bits = 0;
  if (hook_name == "MAP.Map") {
    category = sort_category::Map;
  } else if (hook_name == "RANGEMAP.RangeMap") {
    category = sort_category::RangeMap;
  } else if (hook_name == "LIST.List") {
    category = sort_category::List;
  } else if (hook_name == "SET.Set") {
    category = sort_category::Set;
  } else if (hook_name == "INT.Int") {
    category = sort_category::Int;
  } else if (hook_name == "FLOAT.Float") {
    category = sort_category::Float;
  } else if (hook_name == "BUFFER.StringBuffer") {
    category = sort_category::StringBuffer;
  } else if (hook_name == "BOOL.Bool") {
    category = sort_category::Bool;
  } else if (hook_name == "KVAR.KVar") {
    category = sort_category::Variable;
    // we expect the "hook" of a MInt to be of the form "MINT.MInt N" for some
    // bitwidth N
  } else if (hook_name.substr(0, 10) == "MINT.MInt ") {
    category = sort_category::MInt;
    bits = std::stoi(hook_name.substr(10));
  } else {
    category = sort_category::Symbol;
  }
  return {category, bits};
}

void kore_symbol::add_argument(sptr<kore_sort> const &argument) {
  arguments_.push_back(argument);
}

void kore_symbol::add_formal_argument(sptr<kore_sort> const &argument) {
  formal_arguments_.push_back(argument);
}

void kore_symbol::add_sort(sptr<kore_sort> sort) {
  this->sort_ = std::move(sort);
}

bool kore_symbol::operator==(kore_symbol const &other) const {
  if (name_ != other.name_ || arguments_.size() != other.arguments_.size()) {
    return false;
  }
  for (int i = 0; i < arguments_.size(); ++i) {
    if (*arguments_[i] != *other.arguments_[i]) {
      return false;
    }
  }
  return true;
}

std::string kore_symbol::layout_string(kore_definition *definition) const {
  std::string result;
  for (auto const &arg : arguments_) {
    auto *sort = dynamic_cast<kore_composite_sort *>(arg.get());
    value_type cat = sort->get_category(definition);
    switch (cat.cat) {
    case sort_category::Map: result.push_back('1'); break;
    case sort_category::RangeMap: result.push_back('b'); break;
    case sort_category::List: result.push_back('2'); break;
    case sort_category::Set: result.push_back('3'); break;
    case sort_category::Int: result.push_back('4'); break;
    case sort_category::Float: result.push_back('5'); break;
    case sort_category::StringBuffer: result.push_back('6'); break;
    case sort_category::Bool: result.push_back('7'); break;
    case sort_category::Variable: result.push_back('8'); break;
    case sort_category::MInt:
      result.append("_" + std::to_string(cat.bits) + "_");
    case sort_category::Symbol: result.push_back('0'); break;
    case sort_category::Uncomputed: abort();
    }
  }
  return result;
}

bool kore_composite_sort::is_concrete() const {
  return std::all_of(arguments_.begin(), arguments_.end(), [](auto const &sort) {
    return sort->is_concrete();
  });
}

bool kore_symbol::is_concrete() const {
  return std::all_of(arguments_.begin(), arguments_.end(), [](auto const &sort) {
    return sort->is_concrete();
  });
}

bool kore_symbol::is_polymorphic() const {
  return std::none_of(arguments_.begin(), arguments_.end(), [](auto const &sort) {
    return sort->is_concrete();
  });
}

static std::unordered_set<std::string> builtins{
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

bool kore_symbol::is_builtin() const {
  return builtins.contains(name_);
}

void kore_symbol::instantiate_symbol(kore_symbol_declaration *decl) {
  std::vector<sptr<kore_sort>> instantiated;
  int i = 0;
  kore_sort::substitution vars;
  for (auto const &var : decl->get_object_sort_variables()) {
    vars.emplace(*var, formal_arguments_[i++]);
  }
  for (auto const &sort : decl->get_symbol()->get_arguments()) {
    instantiated.push_back(sort->substitute(vars));
  }
  auto returnSort = decl->get_symbol()->sort_;
  sort_ = returnSort->substitute(vars);

  arguments_ = instantiated;
}

std::string kore_variable::get_name() const {
  return name_;
}

std::string kore_variable_pattern::get_name() const {
  return name_->get_name();
}

void kore_composite_pattern::add_argument(sptr<kore_pattern> const &argument) {
  arguments_.push_back(argument);
}

void kore_composite_pattern::mark_symbols(
    std::map<std::string, std::vector<kore_symbol *>> &map) {
  if (!constructor_->is_builtin()) {
    if (!map.contains(constructor_->get_name())) {
      map.emplace(constructor_->get_name(), std::vector<kore_symbol *>{});
    }
    map.at(constructor_->get_name()).push_back(constructor_.get());
  }
  for (auto &arg : arguments_) {
    arg->mark_symbols(map);
  }
}

void kore_composite_pattern::mark_variables(
    std::map<std::string, kore_variable_pattern *> &map) {
  for (auto &arg : arguments_) {
    arg->mark_variables(map);
  }
}

sptr<kore_pattern>
kore_composite_pattern::substitute(substitution const &subst) {
  if (arguments_.empty()) {
    return shared_from_this();
  }
  auto ptr = kore_composite_pattern::create(constructor_.get());
  auto name = constructor_->get_name();
  if (name == "\\forall" || name == "\\exists") {
    ptr->add_argument(arguments_[0]);
    auto newSubst = subst;
    newSubst.erase(
        dynamic_cast<kore_variable_pattern *>(arguments_[0].get())->get_name());
    ptr->add_argument(arguments_[1]->substitute(newSubst));
    return ptr;
  }
  for (auto &arg : arguments_) {
    ptr->add_argument(arg->substitute(subst));
  }
  return ptr;
}

sptr<kore_pattern> kore_composite_pattern::expand_aliases(kore_definition *def) {
  if (def->get_alias_declarations().contains(constructor_->get_name())) {
    auto *alias = def->get_alias_declarations().at(constructor_->get_name());
    auto subst = alias->get_substitution(this);
    return alias->get_pattern()->substitute(subst)->expand_aliases(def);
  }
  if (arguments_.empty()) {
    return shared_from_this();
  }
  auto ptr = kore_composite_pattern::create(constructor_.get());
  for (auto &arg : arguments_) {
    ptr->add_argument(arg->expand_aliases(def));
  }
  return ptr;
}

static int indent = 0;
static bool at_new_line = true;

static void newline(std::ostream &out) {
  out << std::endl;
  at_new_line = true;
}

static void printIndent(std::ostream &out) {
  constexpr auto indent_size = 2;

  if (at_new_line) {
    for (int i = 0; i < indent_size * indent; i++) {
      out << ' ';
    }
    at_new_line = false;
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
    std::ostream &out, std::string const &color,
    pretty_print_data const &data) {
  if (data.has_color) {
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

void kore_sort_variable::pretty_print(std::ostream &out) const {
  append(out, name_);
}

void kore_composite_sort::pretty_print(std::ostream &out) const {
  append(out, name_.substr(4));
  if (!arguments_.empty()) {
    append(out, '{');
    std::string conn;
    for (auto const &sort : arguments_) {
      append(out, conn);
      sort->pretty_print(out);
      conn = ",";
    }
    append(out, '}');
  }
}

void kore_variable_pattern::pretty_print(
    std::ostream &out, pretty_print_data const &data) const {
  append(out, decodeKore(get_name().substr(3)));
  append(out, ':');
  sort_->pretty_print(out);
}

// NOLINTNEXTLINE(*-cognitive-complexity)
void kore_composite_pattern::pretty_print(
    std::ostream &out, pretty_print_data const &data) const {
  std::string name = get_constructor()->get_name();
  if (name == "\\dv") {
    auto *s = dynamic_cast<kore_composite_sort *>(
        get_constructor()->get_formal_arguments()[0].get());
    bool hasHook = data.hook.contains(s->get_name());
    auto *str = dynamic_cast<kore_string_pattern *>(arguments_[0].get());
    if (hasHook) {
      auto hook = data.hook.at(s->get_name());
      if (hook == "STRING.String") {
        append(out, enquote(str->get_contents()));
      } else if (hook == "BYTES.Bytes") {
        append(out, 'b');
        append(out, enquote(str->get_contents()));
      } else {
        append(out, str->get_contents());
      }
    } else {
      append(out, str->get_contents());
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
          if (data.has_color) {
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
          if (idx == 0 || idx > arguments_.size()) {
            abort();
          }
          kore_pattern *inner = arguments_[idx - 1].get();
          bool assoc = false;
          if (auto *app = dynamic_cast<kore_composite_pattern *>(inner)) {
            if (app->get_constructor()->get_name() == constructor_->get_name()
                && data.assoc.contains(name)) {
              assoc = true;
            }
            if (assoc) {
              for (int j = 0; j < localIndent; j++) {
                indent--;
              }
            }
            inner->pretty_print(out, data);
            if (assoc) {
              for (int j = 0; j < localIndent; j++) {
                indent++;
              }
            }
          } else {
            inner->pretty_print(out, data);
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
  static bool is_digit(char c) { return c >= '0' && c <= '9'; }

  static std::string get_chunk(std::string s, size_t slength, size_t marker) {
    std::string chunk;
    char c = s[marker];
    chunk.push_back(c);
    marker++;
    if (is_digit(c)) {
      while (marker < slength) {
        c = s[marker];
        if (!is_digit(c)) {
          break;
        }
        chunk.push_back(c);
        marker++;
      }
    } else {
      while (marker < slength) {
        c = s[marker];
        if (is_digit(c)) {
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
      std::string thisChunk = get_chunk(s1, s1length, thisMarker);
      thisMarker += thisChunk.length();
      std::string thatChunk = get_chunk(s2, s2length, thatMarker);
      thatMarker += thatChunk.length();
      int result = 0;
      if (is_digit(thisChunk[0]) && is_digit(thatChunk[0])) {
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
  for (auto const &arg : pat->get_arguments()) {
    if (auto *pat2 = dynamic_cast<kore_composite_pattern *>(arg.get())) {
      if (pat2->get_constructor()->get_name() == name) {
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
kore_composite_pattern::sort_collections(pretty_print_data const &data) {
  if (arguments_.empty()) {
    return shared_from_this();
  }
  std::string name = get_constructor()->get_name();
  if (data.comm.contains(name) && data.assoc.contains(name)) {
    std::vector<sptr<kore_pattern>> items;
    flatten(this, name, items);
    std::vector<std::pair<std::string, sptr<kore_pattern>>> printed;
    int oldIndent = indent;
    bool oldAtNewLine = at_new_line;
    at_new_line = false;
    indent = 0;
    pretty_print_data newData = data;
    newData.has_color = false;
    for (auto &item : items) {
      std::ostringstream Out;
      item = item->sort_collections(data);
      item->pretty_print(Out, newData);
      printed.emplace_back(Out.str(), item);
    }
    indent = oldIndent;
    at_new_line = oldAtNewLine;
    std::sort(printed.begin(), printed.end(), compare_first{});
    items.clear();
    for (auto &item : printed) {
      items.push_back(item.second);
    }
    sptr<kore_pattern> result = items[0];
    for (int i = 1; i < items.size(); ++i) {
      sptr<kore_composite_pattern> tmp
          = kore_composite_pattern::create(constructor_.get());
      tmp->add_argument(result);
      tmp->add_argument(items[i]);
      result = tmp;
    }
    return result;
  }
  sptr<kore_composite_pattern> result
      = kore_composite_pattern::create(constructor_.get());
  for (auto &arg : arguments_) {
    result->add_argument(arg->sort_collections(data));
  }
  return result;
}

std::set<std::string> kore_pattern::gather_singleton_vars() {
  auto counts = gather_var_counts();
  std::set<std::string> result;
  for (auto const &entry : counts) {
    if (entry.second == 1) {
      result.insert(entry.first);
    }
  }
  return result;
}

std::map<std::string, int> kore_composite_pattern::gather_var_counts() {
  std::map<std::string, int> result;
  for (auto &arg : arguments_) {
    auto childResult = arg->gather_var_counts();
    for (auto const &entry : childResult) {
      result[entry.first] += entry.second;
    }
  }
  return result;
}

sptr<kore_pattern> kore_composite_pattern::dedupe_disjuncts() {
  if (constructor_->get_name() != "\\or") {
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
        = kore_composite_pattern::create(constructor_.get());
    tmp->add_argument(result);
    tmp->add_argument(dedupedItems[i]);
    result = tmp;
  }
  return result;
}

// NOLINTNEXTLINE(*-cognitive-complexity)
sptr<kore_pattern> kore_composite_pattern::filter_substitution(
    pretty_print_data const &data, std::set<std::string> const &vars) {
  if (constructor_->get_name() == "\\equals") {
    if (auto *var = dynamic_cast<kore_variable_pattern *>(arguments_[0].get())) {
      std::ostringstream ss;
      int oldIndent = indent;
      bool oldAtNewLine = at_new_line;
      at_new_line = true;
      indent = 0;
      var->pretty_print(ss, data);
      indent = oldIndent;
      at_new_line = oldAtNewLine;
      std::string name = ss.str();
      if (vars.contains(var->get_name())
          && (name[0] == '_'
              || (name.size() > 1
                  && (name[0] == '@' || name[0] == '!' || name[0] == '?')
                  && name[1] == '_'))) {
        sptr<kore_composite_pattern> unit
            = kore_composite_pattern::create("\\top");
        unit->get_constructor()->add_formal_argument(
            constructor_->get_formal_arguments()[1]);
        return unit;
      }
      return shared_from_this();
    }
    return shared_from_this();
  }
  if (constructor_->get_name() == "\\and" || constructor_->get_name() == "\\or") {
    sptr<kore_composite_pattern> result
        = kore_composite_pattern::create(constructor_.get());
    for (auto &arg : arguments_) {
      if (constructor_->get_name() == "\\or") {
        std::set<std::string> vars = arg->gather_singleton_vars();
        result->add_argument(arg->filter_substitution(data, vars));
      } else {
        result->add_argument(arg->filter_substitution(data, vars));
      }
    }
    if (constructor_->get_name() == "\\and") {
      if (auto *composite = dynamic_cast<kore_composite_pattern *>(
              result->get_arguments()[0].get())) {
        if (composite->get_constructor()->get_name() == "\\top") {
          return result->get_arguments()[1];
        }
      }
      if (auto *composite = dynamic_cast<kore_composite_pattern *>(
              result->get_arguments()[1].get())) {
        if (composite->get_constructor()->get_name() == "\\top") {
          return result->get_arguments()[0];
        }
      }
    }
    return result;
  }
  return shared_from_this();
}

sptr<kore_pattern> kore_composite_pattern::desugar_associative() {
  if (constructor_->get_name() == "\\left-assoc") {
    if (auto *comp_arg
        = dynamic_cast<kore_composite_pattern *>(arguments_[0].get())) {
      auto accum = comp_arg->arguments_[0]->desugar_associative();

      for (auto i = 1U; i < comp_arg->arguments_.size(); i++) {
        auto new_accum
            = kore_composite_pattern::create(comp_arg->get_constructor());
        new_accum->add_argument(accum);
        new_accum->add_argument(comp_arg->arguments_[i]->desugar_associative());
        accum = ptr<kore_pattern>(new_accum.release());
      }

      return accum;
    }
  } else if (constructor_->get_name() == "\\right-assoc") {
    if (auto *comp_arg
        = dynamic_cast<kore_composite_pattern *>(arguments_[0].get())) {
      auto accum = comp_arg->arguments_[comp_arg->arguments_.size() - 1]
                       ->desugar_associative();

      for (int i = comp_arg->arguments_.size() - 2; i >= 0; i--) {
        auto new_accum
            = kore_composite_pattern::create(comp_arg->get_constructor());
        new_accum->add_argument(comp_arg->arguments_[i]->desugar_associative());
        new_accum->add_argument(accum);
        accum = ptr<kore_pattern>(new_accum.release());
      }

      return accum;
    }
  } else {
    auto result = kore_composite_pattern::create(constructor_.get());

    for (auto &arg : arguments_) {
      result->add_argument(arg->desugar_associative());
    }

    return result;
  }

  return shared_from_this();
}

sptr<kore_pattern> kore_composite_pattern::unflatten_and_or() {
  if ((constructor_->get_name() == "\\and" || constructor_->get_name() == "\\or")
      && arguments_.size() != 2) {
    if (arguments_.size() == 1) {
      return arguments_[0]->unflatten_and_or();
    }
    auto accum = arguments_[0]->unflatten_and_or();

    for (auto i = 1U; i < arguments_.size(); i++) {
      auto new_accum = kore_composite_pattern::create(constructor_.get());
      new_accum->add_argument(accum);
      new_accum->add_argument(arguments_[i]->unflatten_and_or());
      accum = ptr<kore_pattern>(new_accum.release());
    }

    return accum;
  }
  auto result = kore_composite_pattern::create(constructor_.get());

  for (auto &arg : arguments_) {
    result->add_argument(arg->unflatten_and_or());
  }

  return result;
}

sptr<kore_pattern> kore_composite_pattern::expand_macros(
    SubsortMap const &subsorts, SymbolMap const &overloads,
    std::vector<ptr<kore_declaration>> const &macros, bool reverse,
    std::set<size_t> &applied_rules,
    std::set<std::string> const &macro_symbols) {
  sptr<kore_composite_pattern> applied
      = kore_composite_pattern::create(constructor_.get());
  for (auto &arg : arguments_) {
    std::set<size_t> dummyApplied;
    applied->add_argument(arg->expand_macros(
        subsorts, overloads, macros, reverse, dummyApplied, macro_symbols));
  }

  if (macro_symbols.find(constructor_->get_name()) == macro_symbols.end()) {
    return applied;
  }

  size_t i = 0;
  for (auto const &decl : macros) {
    if ((decl->attributes().contains(attribute_set::key::Macro)
         || decl->attributes().contains(attribute_set::key::MacroRec))
        && reverse) {
      i++;
      continue;
    }
    auto *axiom = dynamic_cast<kore_axiom_declaration *>(decl.get());
    auto *equals
        = dynamic_cast<kore_composite_pattern *>(axiom->get_pattern().get());
    auto lhs = equals->arguments_[reverse ? 1 : 0];
    auto rhs = equals->arguments_[reverse ? 0 : 1];
    substitution subst;
    bool matches = lhs->matches(subst, subsorts, overloads, applied);
    if (matches
        && (decl->attributes().contains(attribute_set::key::MacroRec)
            || decl->attributes().contains(attribute_set::key::AliasRec)
            || !applied_rules.contains(i))) {
      std::set<size_t> oldAppliedRules = applied_rules;
      applied_rules.insert(i);
      auto result = rhs->substitute(subst)->expand_macros(
          subsorts, overloads, macros, reverse, applied_rules, macro_symbols);
      applied_rules = oldAppliedRules;
      return result;
    }
    i++;
  }
  return applied;
}

bool kore_variable_pattern::matches(
    substitution &subst, SubsortMap const &subsorts, SymbolMap const &overloads,
    sptr<kore_pattern> subject) {
  if (subst[name_->get_name()]) {
    return ast_to_string(*subst[name_->get_name()]) == ast_to_string(*subject);
  }
  subst[name_->get_name()] = subject;
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
  if (*subj->get_constructor() != *get_constructor()) {
    if (subj->get_constructor()->get_name() == "inj"
        && get_constructor()->get_name() == "inj") {
      if (*subj->get_constructor()->get_formal_arguments()[1]
          != *get_constructor()->get_formal_arguments()[1]) {
        return false;
      }
      sptr<kore_sort> a = subj->get_constructor()->get_formal_arguments()[0];
      sptr<kore_sort> b = get_constructor()->get_formal_arguments()[0];
      if (subsorts.contains(b.get())
          && subsorts.at(b.get()).contains(a.get())) {
        sptr<kore_composite_pattern> ba = kore_composite_pattern::create("inj");
        ba->get_constructor()->add_formal_argument(b);
        ba->get_constructor()->add_formal_argument(a);
        ba->get_constructor()->add_argument(b);
        ba->add_argument(arguments_[0]);
        return ba->matches(subst, subsorts, overloads, subj->get_arguments()[0]);
      }
      if (subsorts.contains(a.get())
          && subsorts.at(a.get()).contains(b.get())) {
        sptr<kore_composite_pattern> ab = kore_composite_pattern::create("inj");
        ab->get_constructor()->add_formal_argument(a);
        ab->get_constructor()->add_formal_argument(b);
        ab->get_constructor()->add_argument(a);
        ab->add_argument(subj->get_arguments()[0]);
        return arguments_[0]->matches(subst, subsorts, overloads, ab);
      }
      return false;
    }
    if (subj->get_constructor()->get_name() == "inj") {
      sptr<kore_pattern> child = subj->get_arguments()[0];
      if (auto *composite
          = dynamic_cast<kore_composite_pattern *>(child.get())) {
        if (overloads.contains(composite->get_constructor())
            && overloads.at(composite->get_constructor())
                   .contains(get_constructor())) {
          sptr<kore_composite_pattern> greater
              = kore_composite_pattern::create(get_constructor());
          for (int i = 0; i < arguments_.size(); i++) {
            if (*get_constructor()->get_arguments()[i]
                != *composite->get_constructor()->get_arguments()[i]) {
              sptr<kore_composite_pattern> inj
                  = kore_composite_pattern::create("inj");
              inj->get_constructor()->add_formal_argument(
                  composite->get_constructor()->get_arguments()[i]);
              inj->get_constructor()->add_formal_argument(
                  get_constructor()->get_arguments()[i]);
              inj->get_constructor()->add_argument(
                  composite->get_constructor()->get_arguments()[i]);
              inj->add_argument(composite->get_arguments()[i]);
              greater->add_argument(inj);
            } else {
              greater->add_argument(composite->get_arguments()[i]);
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
  if (subj->arguments_.size() != arguments_.size()) {
    return false;
  }
  bool match = true;
  for (int i = 0; i < subj->arguments_.size(); i++) {
    match = match
            && arguments_[i]->matches(
                subst, subsorts, overloads, subj->arguments_[i]);
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
  return subj->contents_ == contents_;
}

void kore_declaration::add_object_sort_variable(
    sptr<kore_sort_variable> const &sort_variable) {
  object_sort_variables_.push_back(sort_variable);
}

void kore_axiom_declaration::add_pattern(sptr<kore_pattern> pattern) {
  this->pattern_ = std::move(pattern);
}

bool kore_axiom_declaration::is_required() const {
  constexpr auto keys_to_drop = std::array{
      attribute_set::key::Assoc,          attribute_set::key::Comm,
      attribute_set::key::Idem,           attribute_set::key::Unit,
      attribute_set::key::Functional,     attribute_set::key::Constructor,
      attribute_set::key::Total,          attribute_set::key::Subsort,
      attribute_set::key::Ceil,           attribute_set::key::NonExecutable,
      attribute_set::key::Simplification,
  };

  return std::none_of(
      keys_to_drop.begin(), keys_to_drop.end(),
      [this](auto key) { return attributes().contains(key); });
}

bool kore_axiom_declaration::is_top_axiom() const {
  if (auto *top = dynamic_cast<kore_composite_pattern *>(pattern_.get())) {
    if (top->get_constructor()->get_name() == "\\implies"
        && top->get_arguments().size() == 2) {
      if (auto *bottomPattern = dynamic_cast<kore_composite_pattern *>(
              top->get_arguments()[0].get())) {
        if (bottomPattern->get_constructor()->get_name() == "\\bottom"
            && bottomPattern->get_arguments().empty()) {
          return true;
        }
      }
      return false;
    }
    if (top->get_constructor()->get_name() == "\\rewrites"
        && top->get_arguments().size() == 2) {
      return true;
    }
    if (top->get_constructor()->get_name() == "\\and"
        && top->get_arguments().size() == 2) {
      return true;
    }
  }
  return false;
}

void kore_alias_declaration::add_variables(
    sptr<kore_composite_pattern> variables) {
  bound_variables_ = std::move(variables);
}

void kore_alias_declaration::add_pattern(sptr<kore_pattern> pattern) {
  this->pattern_ = std::move(pattern);
}

kore_pattern::substitution
kore_alias_declaration::get_substitution(kore_composite_pattern *subject) {
  int i = 0;
  kore_pattern::substitution result;
  for (auto const &arg : bound_variables_->get_arguments()) {
    auto *var = dynamic_cast<kore_variable_pattern *>(arg.get());
    if (!var) {
      abort();
    }
    result[var->get_name()] = subject->get_arguments()[i++];
  }
  return result;
}

bool kore_symbol_declaration::is_anywhere() const {
  return attributes().contains(attribute_set::key::Anywhere);
}

void kore_module::add_declaration(sptr<kore_declaration> declaration) {
  declarations_.push_back(std::move(declaration));
}

// Pretty printer
void kore_sort_variable::print(std::ostream &out, unsigned indent) const {
  std::string Indent(indent, ' ');
  out << Indent << name_;
}

void kore_composite_sort::print(std::ostream &out, unsigned indent) const {
  std::string Indent(indent, ' ');
  out << Indent << name_ << "{";
  bool isFirst = true;
  for (auto const &Argument : arguments_) {
    if (!isFirst) {
      out << ",";
    }
    Argument->print(out);
    isFirst = false;
  }
  out << "}";
}

void kore_symbol::print(std::ostream &out, unsigned indent) const {
  print(out, indent, true);
}

void kore_symbol::print(std::ostream &out, unsigned indent, bool formal) const {
  std::string Indent(indent, ' ');
  out << Indent << name_ << "{";
  bool isFirst = true;
  for (auto const &Argument : (formal ? formal_arguments_ : arguments_)) {
    if (!isFirst) {
      out << ", ";
    }
    Argument->print(out);
    isFirst = false;
  }
  out << "}";
}

void kore_variable::print(std::ostream &out, unsigned indent) const {
  std::string Indent(indent, ' ');
  out << Indent << name_;
}

void kore_variable_pattern::print(std::ostream &out, unsigned indent) const {
  std::string Indent(indent, ' ');
  out << Indent;
  name_->print(out);
  out << " : ";
  sort_->print(out);
}

void kore_composite_pattern::print(std::ostream &out, unsigned indent) const {
  std::string Indent(indent, ' ');
  out << Indent;
  constructor_->print(out);
  out << "(";
  bool isFirst = true;
  for (auto const &Argument : arguments_) {
    if (!isFirst) {
      out << ",";
    }
    Argument->print(out);
    isFirst = false;
  }
  out << ")";
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

void kore_string_pattern::print(std::ostream &out, unsigned indent) const {
  std::string Indent(indent, ' ');
  out << Indent << "\"" << escapeString(contents_) << "\"";
}

static void printAttributeList(
    std::ostream &out, attribute_set const &attributes, unsigned indent = 0) {

  std::string Indent(indent, ' ');
  out << Indent << "[";
  bool isFirst = true;
  for (auto const &[name, pattern] : attributes) {
    if (!isFirst) {
      out << ",";
    }
    pattern->print(out);
    isFirst = false;
  }
  out << "]";
}

void kore_declaration::print_sort_variables(std::ostream &out) const {
  out << "{";
  bool isFirst = true;
  for (auto const &Variable : object_sort_variables_) {
    if (!isFirst) {
      out << ",";
    }
    Variable->print(out);
    isFirst = false;
  }
  out << "}";
}

void kore_composite_sort_declaration::print(
    std::ostream &out, unsigned indent) const {
  std::string Indent(indent, ' ');
  out << Indent << (is_hooked_ ? "hooked-sort " : "sort ") << sort_name_;
  print_sort_variables(out);
  out << " ";
  printAttributeList(out, attributes());
}

void kore_symbol_declaration::print(std::ostream &out, unsigned indent) const {
  std::string Indent(indent, ' ');
  out << Indent << (is_hooked_ ? "hooked-symbol " : "symbol ")
      << get_symbol()->get_name();
  print_sort_variables(out);
  out << "(";
  bool isFirst = true;
  for (auto const &Argument : get_symbol()->get_arguments()) {
    if (!isFirst) {
      out << ",";
    }
    Argument->print(out);
    isFirst = false;
  }
  out << ") : ";
  get_symbol()->get_sort()->print(out);
  out << " ";
  printAttributeList(out, attributes());
}

void kore_alias_declaration::print(std::ostream &out, unsigned indent) const {
  std::string Indent(indent, ' ');
  out << Indent << "alias " << get_symbol()->get_name();
  print_sort_variables(out);
  out << "(";
  bool isFirst = true;
  for (auto const &Argument : get_symbol()->get_arguments()) {
    if (!isFirst) {
      out << ",";
    }
    Argument->print(out);
    isFirst = false;
  }
  out << ") : ";
  get_symbol()->get_sort()->print(out);
  out << " where ";
  bound_variables_->print(out);
  out << " := ";
  pattern_->print(out);
  out << " ";
  printAttributeList(out, attributes());
}

void kore_axiom_declaration::print(std::ostream &out, unsigned indent) const {
  std::string Indent(indent, ' ');
  out << Indent << (is_claim() ? "claim " : "axiom ");
  print_sort_variables(out);
  pattern_->print(out);
  out << " ";
  printAttributeList(out, attributes());
}

void kore_module_import_declaration::print(
    std::ostream &out, unsigned indent) const {
  std::string Indent(indent, ' ');
  out << Indent << "import " << module_name_;
  out << " ";
  printAttributeList(out, attributes());
}

void kore_module::print(std::ostream &out, unsigned indent) const {
  std::string Indent(indent, ' ');
  out << Indent << "module " << name_ << "\n";
  bool isFirst = true;
  for (auto const &Declaration : declarations_) {
    if (!isFirst) {
      out << "\n";
    }
    Declaration->print(out, indent + 2);
    out << "\n";
    isFirst = false;
  }
  out << Indent << "endmodule\n";
  printAttributeList(out, attributes(), indent);
}

void kore_definition::print(std::ostream &out, unsigned indent) const {
  printAttributeList(out, attributes(), indent);
  out << "\n";
  for (auto const &Module : modules_) {
    out << "\n";
    Module->print(out, indent);
    out << "\n";
  }
}

// Binary serialization

void kore_variable_pattern::serialize_to(serializer &s) const {
  sort_->serialize_to(s);
  s.emit(header_byte<kore_variable_pattern>);
  name_->serialize_to(s);
}

void kore_composite_pattern::serialize_to(serializer &s) const {
  auto emit_this_arity = s.use_arity();
  s.reset_arity_flag();

  for (auto const &arg : arguments_) {
    arg->serialize_to(s);
  }

  constructor_->serialize_to(s);

  s.emit(header_byte<kore_composite_pattern>);

  if (emit_this_arity) {
    s.emit_length(arguments_.size());
  }
}

void kore_string_pattern::serialize_to(serializer &s) const {
  s.emit(header_byte<kore_string_pattern>);
  s.emit_string(contents_);
}

void kore_sort_variable::serialize_to(serializer &s) const {
  s.emit(header_byte<kore_sort_variable>);
  s.emit_string(name_);
}

void kore_composite_sort::serialize_to(serializer &s) const {
  for (auto const &arg : arguments_) {
    arg->serialize_to(s);
  }

  s.emit(header_byte<kore_composite_sort>);
  s.emit_length(arguments_.size());
  s.emit_string(name_);
}

void kore_symbol::serialize_to(serializer &s) const {
  for (auto const &arg : formal_arguments_) {
    arg->serialize_to(s);
  }

  s.emit(header_byte<kore_symbol>);
  s.emit_length(formal_arguments_.size());
  s.emit_string(name_);
}

void kore_variable::serialize_to(serializer &s) const {
  s.emit(header_byte<kore_variable>);
  s.emit_string(name_);
}

void kllvm::readMultimap(
    std::string const &name, kore_symbol_declaration *decl,
    std::map<std::string, std::set<std::string>> &output,
    attribute_set::key att_name) {
  if (decl->attributes().contains(att_name)) {
    kore_composite_pattern *att = decl->attributes().get(att_name).get();
    for (auto const &pat : att->get_arguments()) {
      auto *child = dynamic_cast<kore_composite_pattern *>(pat.get());
      output[name].insert(child->get_constructor()->get_name());
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
          vec.end(), std::make_move_iterator(composite->arguments_.begin()),
          std::make_move_iterator(composite->arguments_.end()));
    }
  }
}
