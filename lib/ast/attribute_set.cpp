#include <kllvm/ast/AST.h>
#include <kllvm/ast/attribute_set.h>

namespace kllvm {

namespace {

attribute_set::key key_from_string(std::string const &str) {
  static std::unordered_map<std::string, attribute_set::key> table = {
      {"hook", attribute_set::key::hook},
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
