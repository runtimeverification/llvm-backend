#include "kllvm/ast/AST.h"
#include "kllvm/printer/printer.h"

#include <iostream>
#include <string>
#include <unistd.h>

using namespace kllvm;

int main(int argc, char **argv) {
  if (argc != 3 && argc != 4 && argc != 5) {
    std::cerr
        << "usage: " << argv[0]
        << " <definition.kore> <pattern.kore> [true|false|auto] [true|false]"
        << std::endl;
  }

  bool hasColor, filterSubst;
  if (argc >= 4) {
    std::string arg = argv[3];
    hasColor = arg == "true" || (arg == "auto" && isatty(1));
  } else {
    hasColor = isatty(1);
  }
  if (argc >= 5) {
    std::string arg = argv[4];
    filterSubst = arg == "true";
  } else {
    filterSubst = true;
  }

  printKORE(std::cout, argv[1], argv[2], hasColor, filterSubst);
}
