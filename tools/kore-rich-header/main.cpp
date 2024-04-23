#include <kllvm/ast/AST.h>
#include <kllvm/parser/KOREParser.h>

#include <cstdint>
#include <iostream>

using namespace kllvm;
using namespace kllvm::parser;

int main(int argc, char **argv) {
  char *filename = argv[1];
  kore_parser parser(filename);
  ptr<kore_definition> definition = parser.definition();
  definition->preprocess();

  const uint32_t version = 1;
  const uint32_t num_tags = definition->get_symbols().size();
  const uint32_t num_sorts = definition->get_all_sorts().size();
  const uint32_t num_strings = num_tags + num_sorts + 1;
  const uint32_t num_symbols = num_tags + num_sorts;

  std::cout.write("\177KR2", 4);
  std::cout.write(reinterpret_cast<char const *>(&version), 4);
  std::cout.write(reinterpret_cast<char const *>(&num_strings), 4);
  std::cout.write(reinterpret_cast<char const *>(&num_sorts), 4);
  std::cout.write(reinterpret_cast<char const *>(&num_symbols), 4);

  for (uint32_t i = 0; i < num_tags; i++) {
    auto name = definition->get_symbols().at(i)->get_name();
    const uint32_t len = name.size();
    std::cout.write(reinterpret_cast<char const *>(&len), 4);
    std::cout << name;
    std::cout.put('\000');
  }
  for (uint32_t i = 0; i < num_sorts; i++) {
    auto name = definition->get_all_sorts()[i]->get_name();
    const uint32_t len = name.size();
    std::cout.write(reinterpret_cast<char const *>(&len), 4);
    std::cout << name;
    std::cout.put('\000');
  }
  auto const *name = "\\dv";
  const uint32_t len = 3;
  std::cout.write(reinterpret_cast<char const *>(&len), 4);
  std::cout.write(name, 4);

  for (uint32_t i = num_tags; i < num_sorts + num_tags; i++) {
    std::cout.write(reinterpret_cast<char const *>(&i), 4);
    if (!definition->get_all_sorts()[i]->get_arguments().empty()) {
      throw std::runtime_error(
          "cannot yet serialize sorts with sort parameters");
    }
    std::cout.put('\000');
  }

  for (uint32_t i = 0; i < num_tags; i++) {
    std::cout.write(reinterpret_cast<char const *>(&i), 4);
    auto const &symbol = definition->get_symbols().at(i);
    int const num_params = symbol->get_formal_arguments().size();
    std::cout.put((uint8_t)num_params);
    std::cout.put((uint8_t)symbol->get_arguments().size());
    for (int j = 0; j < num_params; j++) {
      uint32_t ordinal = dynamic_cast<kore_composite_sort *>(
                             symbol->get_formal_arguments()[j].get())
                             ->get_ordinal();
      std::cout.write(reinterpret_cast<char const *>(&ordinal), 4);
    }
  }
  return 0;
}
