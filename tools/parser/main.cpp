#include "kllvm/parser/KOREScanner.h"
#include "kllvm/parser/KOREParserDriver.h"

using namespace kllvm;
using namespace kllvm::parser;

int main (int argc, char **argv) {
  if (argc < 2) {
    std::cerr << "Requires input file\n";
    exit(1);
  }
 
  KOREScanner scanner(argv[1]);
  KOREParserDriver driver;
  KOREDefinition *definition;
  KOREParser parser(scanner, driver, &definition);

#if YYDEBUG
  parser.set_debug_level(1);
#endif

  parser.parse();
  definition->print(std::cout);
  return 0;
}
