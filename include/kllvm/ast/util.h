#include <kllvm/ast/AST.h>

#include <string>

using namespace kllvm;

[[nodiscard]] std::optional<int64_t>
get_start_line_location(kore_axiom_declaration const &axiom) {
  auto location_att = axiom.attributes().get(attribute_set::key::Location);
  assert(location_att->get_arguments().size() == 1);

  auto str_pattern = std::dynamic_pointer_cast<kore_string_pattern>(
      location_att->get_arguments()[0]);
  std::string location = str_pattern->get_contents();

  size_t l_paren = location.find_first_of('(');
  size_t first_comma = location.find_first_of(',');
  size_t length = first_comma - l_paren - 1;
  return std::stoi(location.substr(l_paren + 1, length));
}

// trim the string from the start
[[nodiscard]] std::string trim(std::string s) {
  s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int c) {
            return !std::isspace(c);
          }));
  return s;
}