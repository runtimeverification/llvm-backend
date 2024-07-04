#include "kllvm/ast/util.h"

#include <algorithm>
#include <string>

using namespace kllvm;

[[nodiscard]] std::optional<std::pair<std::string, uint64_t>>
kllvm::get_start_line_location(kore_axiom_declaration const &axiom) {
  if (!axiom.attributes().contains(attribute_set::key::Source)
      || !axiom.attributes().contains(attribute_set::key::Location)) {
    return std::nullopt;
  }
  auto source_att = axiom.attributes().get(attribute_set::key::Source);
  assert(source_att->get_arguments().size() == 1);

  auto str_pattern_source = std::dynamic_pointer_cast<kore_string_pattern>(
      source_att->get_arguments()[0]);
  std::string source = str_pattern_source->get_contents();

  auto location_att = axiom.attributes().get(attribute_set::key::Location);
  assert(location_att->get_arguments().size() == 1);

  auto str_pattern_loc = std::dynamic_pointer_cast<kore_string_pattern>(
      location_att->get_arguments()[0]);
  std::string location = str_pattern_loc->get_contents();

  size_t l_paren = location.find_first_of('(');
  size_t first_comma = location.find_first_of(',');
  size_t length = first_comma - l_paren - 1;
  return std::make_pair(
      source.substr(7, source.size() - 8),
      std::stoi(location.substr(l_paren + 1, length)));
}

// trim the string from the start
[[nodiscard]] std::string kllvm::trim(std::string s) {
  s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int c) {
            return !std::isspace(c);
          }));
  return s;
}
