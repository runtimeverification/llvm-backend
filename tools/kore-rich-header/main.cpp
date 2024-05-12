#include <kllvm/ast/AST.h>
#include <kllvm/binary/serializer.h>
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
  emit_kore_rich_header(std::cout, definition.get());
  return 0;
}
