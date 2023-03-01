#include "kllvm/ast/AST.h"
#include "kllvm/printer/printer.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/WithColor.h"

#include <iostream>
#include <string>
#include <unistd.h>

using namespace kllvm;
using namespace llvm;

cl::opt<std::string> DefinitionFilename(
    cl::Positional, cl::desc("<definition.kore>"), cl::Required);

cl::opt<std::string>
    PatternFilename(cl::Positional, cl::desc("<pattern.kore>"), cl::Required);

cl::opt<bool> FilterSubst(cl::Positional, cl::desc("[true|false]"));

cl::opt<std::string> ArgColor(cl::Positional, cl::desc("[true|false|auto]"));

int main(int argc, char **argv) {
  cl::PrintOptionValues();
  StringMap<cl::Option *> &Map = cl::getRegisteredOptions();
  Map["color"]->setHiddenFlag(cl::Hidden);

  cl::ParseCommandLineOptions(argc, argv);

  bool hasColor = ArgColor == "true" || (ArgColor == "auto" && isatty(1));

  printKORE(
      std::cout, DefinitionFilename, PatternFilename, hasColor, FilterSubst);
}
