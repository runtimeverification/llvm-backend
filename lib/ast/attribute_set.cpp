#include <kllvm/ast/AST.h>
#include <kllvm/ast/attribute_set.h>

namespace kllvm {

namespace {

attribute_set::key key_from_string(std::string const &str) {
  static std::unordered_map<std::string, attribute_set::key> table = {
      {"org'Stop'kframework'Stop'attributes'Stop'Source",
       attribute_set::key::source},
      {"org'Stop'kframework'Stop'attributes'Stop'Location",
       attribute_set::key::location},
      {"hook", attribute_set::key::hook},
      {"function", attribute_set::key::function},
      {"format", attribute_set::key::format},
      {"assoc", attribute_set::key::assoc},
      {"comm", attribute_set::key::comm},
      {"colors", attribute_set::key::colors},
      {"bracket", attribute_set::key::bracket},
      {"anywhere", attribute_set::key::anywhere},
      {"binder", attribute_set::key::binder},
      {"concat", attribute_set::key::concat},
      {"unit", attribute_set::key::unit},
      {"element", attribute_set::key::element},
      {"sortInjection", attribute_set::key::sort_injection},
      {"label", attribute_set::key::label},
      {"nat", attribute_set::key::nat},
      {"macro", attribute_set::key::macro},
      {"macro_rec", attribute_set::key::macro_rec},
      {"alias", attribute_set::key::alias},
      {"alias_rec", attribute_set::key::alias_rec},
      {"left", attribute_set::key::left},
      {"right", attribute_set::key::right},
      {"priorities", attribute_set::key::priorities},
      {"subsort", attribute_set::key::subsort},
      {"overload", attribute_set::key::overload},
      {"freshGenerator", attribute_set::key::fresh_generator},
      {"priority", attribute_set::key::priority},
      {"terminals", attribute_set::key::terminals},
  };

  return table.at(str);
}

} // namespace

attribute_set::key
attribute_set::add(std::shared_ptr<KORECompositePattern> att) {
  auto key = key_from_string(att->getConstructor()->getName());
  attribute_map_.emplace(key, std::move(att));
  return key;
}

bool attribute_set::contains(attribute_set::key k) const {
  return attribute_map_.find(k) != attribute_map_.end();
}

std::shared_ptr<KORECompositePattern> const &
attribute_set::get(attribute_set::key k) const {
  return attribute_map_.at(k);
}

std::string attribute_set::get_string(attribute_set::key k) const {
  auto const &attribute_pattern = get(k);
  assert(attribute_pattern->getArguments().size() == 1);

  auto const &string_arg = std::dynamic_pointer_cast<KOREStringPattern>(
      attribute_pattern->getArguments()[0]);

  return string_arg->getContents();
}

} // namespace kllvm
