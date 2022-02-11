#include "kllvm/ast/AST.h"
#include "kllvm/ast/deserializer.h"
#include "kllvm/ast/serializer.h"
#include "kllvm/parser/KOREParser.h"

#include <iostream>

using namespace kllvm;
using namespace kllvm::parser;

int main(int argc, char **argv) {
  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << " <file.kore>\n";
    return 1;
  }

  KOREParser parser(argv[1]);
  auto pat = parser.pattern();
  /* auto pat = KOREStringPattern::Create("bruce collie"); */

  pat->print(std::cerr);
  std::cerr << '\n';

  auto s = serializer();
  pat->serialize_to(s);

  for (auto byte : s.data()) {
    std::cout << uint8_t(byte);
  }

  auto new_pat = deserialize_pattern(s.data().begin(), s.data().end());
  new_pat->print(std::cerr);
  std::cerr << '\n';

  return 0;
}
