#include <kllvm/ast/AST.h>
#include <kllvm/ast/attribute_set.h>

#include <optional>

namespace kllvm {

namespace {

std::unordered_map<attribute_set::key, std::string> const &attribute_table() {
  static std::unordered_map<attribute_set::key, std::string> table = {
      {attribute_set::key::Alias, "alias"},
      {attribute_set::key::AliasRec, "alias-rec"},
      {attribute_set::key::Anywhere, "anywhere"},
      {attribute_set::key::Assoc, "assoc"},
      {attribute_set::key::Binder, "binder"},
      {attribute_set::key::Bracket, "bracket"},
      {attribute_set::key::Ceil, "ceil"},
      {attribute_set::key::Colors, "colors"},
      {attribute_set::key::Comm, "comm"},
      {attribute_set::key::Concat, "concat"},
      {attribute_set::key::Constructor, "constructor"},
      {attribute_set::key::Element, "element"},
      {attribute_set::key::Format, "format"},
      {attribute_set::key::FreshGenerator, "freshGenerator"},
      {attribute_set::key::Function, "function"},
      {attribute_set::key::Functional, "functional"},
      {attribute_set::key::Hook, "hook"},
      {attribute_set::key::Idem, "idem"},
      {attribute_set::key::Label, "label"},
      {attribute_set::key::Left, "left"},
      {attribute_set::key::Location,
       "org'Stop'kframework'Stop'attributes'Stop'Location"},
      {attribute_set::key::Macro, "macro"},
      {attribute_set::key::MacroRec, "macro-rec"},
      {attribute_set::key::Nat, "nat"},
      {attribute_set::key::NonExecutable, "non-executable"},
      {attribute_set::key::Priorities, "priorities"},
      {attribute_set::key::Priority, "priority"},
      {attribute_set::key::Right, "right"},
      {attribute_set::key::Simplification, "simplification"},
      {attribute_set::key::SortInjection, "sortInjection"},
      {attribute_set::key::Source,
       "org'Stop'kframework'Stop'attributes'Stop'Source"},
      {attribute_set::key::Subsort, "subsort"},
      {attribute_set::key::SymbolOverload, "symbol-overload"},
      {attribute_set::key::Terminals, "terminals"},
      {attribute_set::key::Total, "total"},
      {attribute_set::key::Unit, "unit"},
  };
  return table;
}

std::string key_to_string(attribute_set::key key) {
  return attribute_table().at(key);
}

std::optional<attribute_set::key> string_to_key(std::string const &name) {
  static std::unordered_map<std::string, attribute_set::key> table = {};
  if (table.empty()) {
    auto const &reverse = attribute_table();
    for (auto const &[k, str] : reverse) {
      table.emplace(str, k);
    }
  }

  if (table.find(name) != table.end()) {
    return table.at(name);
  }

  return std::nullopt;
}

} // namespace

std::optional<attribute_set::key>
attribute_set::add(std::shared_ptr<kore_composite_pattern> att) {
  auto name = att->get_constructor()->get_name();
  attribute_map_.emplace(name, std::move(att));
  return string_to_key(name);
}

bool attribute_set::contains(attribute_set::key k) const {
  return attribute_map_.find(key_to_string(k)) != attribute_map_.end();
}

std::shared_ptr<kore_composite_pattern> const &
attribute_set::get(attribute_set::key k) const {
  return attribute_map_.at(key_to_string(k));
}

std::string attribute_set::get_string(attribute_set::key k) const {
  auto const &attribute_pattern = get(k);
  assert(attribute_pattern->get_arguments().size() == 1);

  auto const &string_arg = std::dynamic_pointer_cast<kore_string_pattern>(
      attribute_pattern->get_arguments()[0]);

  return string_arg->get_contents();
}

attribute_set::storage_t const &attribute_set::underlying() const {
  return attribute_map_;
}

attribute_set::storage_t::const_iterator attribute_set::begin() const {
  return attribute_map_.cbegin();
}

attribute_set::storage_t::const_iterator attribute_set::end() const {
  return attribute_map_.cend();
}

} // namespace kllvm
