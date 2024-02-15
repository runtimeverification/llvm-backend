#include <kllvm/ast/AST.h>
#include <kllvm/ast/attribute_set.h>

#include <optional>

namespace kllvm {

namespace {

std::unordered_map<attribute_set::key, std::string> const &attribute_table() {
  static std::unordered_map<attribute_set::key, std::string> table = {
      {attribute_set::key::source,
       "org'Stop'kframework'Stop'attributes'Stop'Source"},
      {attribute_set::key::location,
       "org'Stop'kframework'Stop'attributes'Stop'Location"},
      {attribute_set::key::hook, "hook"},
      {attribute_set::key::function, "function"},
      {attribute_set::key::format, "format"},
      {attribute_set::key::assoc, "assoc"},
      {attribute_set::key::comm, "comm"},
      {attribute_set::key::colors, "colors"},
      {attribute_set::key::bracket, "bracket"},
      {attribute_set::key::anywhere, "anywhere"},
      {attribute_set::key::binder, "binder"},
      {attribute_set::key::concat, "concat"},
      {attribute_set::key::unit, "unit"},
      {attribute_set::key::element, "element"},
      {attribute_set::key::sort_injection, "sortInjection"},
      {attribute_set::key::label, "label"},
      {attribute_set::key::nat, "nat"},
      {attribute_set::key::macro, "macro"},
      {attribute_set::key::macro_rec, "macro_rec"},
      {attribute_set::key::alias, "alias"},
      {attribute_set::key::alias_rec, "alias_rec"},
      {attribute_set::key::left, "left"},
      {attribute_set::key::right, "right"},
      {attribute_set::key::priorities, "priorities"},
      {attribute_set::key::subsort, "subsort"},
      {attribute_set::key::overload, "overload"},
      {attribute_set::key::fresh_generator, "freshGenerator"},
      {attribute_set::key::priority, "priority"},
      {attribute_set::key::terminals, "terminals"},
      {attribute_set::key::idem, "idem"},
      {attribute_set::key::functional, "functional"},
      {attribute_set::key::constructor, "constructor"},
      {attribute_set::key::total, "total"},
      {attribute_set::key::ceil, "ceil"},
      {attribute_set::key::non_executable, "non-executable"},
      {attribute_set::key::simplification, "simplification"},
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
attribute_set::add(std::shared_ptr<KORECompositePattern> att) {
  auto name = att->getConstructor()->getName();
  attribute_map_.emplace(name, std::move(att));
  return string_to_key(name);
}

bool attribute_set::contains(attribute_set::key k) const {
  return attribute_map_.find(key_to_string(k)) != attribute_map_.end();
}

std::shared_ptr<KORECompositePattern> const &
attribute_set::get(attribute_set::key k) const {
  return attribute_map_.at(key_to_string(k));
}

std::string attribute_set::get_string(attribute_set::key k) const {
  auto const &attribute_pattern = get(k);
  assert(attribute_pattern->getArguments().size() == 1);

  auto const &string_arg = std::dynamic_pointer_cast<KOREStringPattern>(
      attribute_pattern->getArguments()[0]);

  return string_arg->getContents();
}

} // namespace kllvm
