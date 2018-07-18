#include "kllvm/parser/KOREScanner.h"

using namespace kllvm::parser;

int main (int argc, char **argv) {
  if (argc < 2) {
    std::cerr << "Requires input file\n";
    exit(1);
  }
 
  KOREScanner scanner(argv[1]);
  KOREParser parser(scanner);

#if YYDEBUG
  parser.set_debug_level(1);
#endif

  return parser.parse();
  return 0;
}
