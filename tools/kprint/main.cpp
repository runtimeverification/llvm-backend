#include "kllvm/ast/AST.h"
#include "kllvm/printer/printer.h"
#include "llvm/Support/CommandLine.h"

#include <iostream>
#include <string>
#include <unistd.h>

using namespace kllvm;
using namespace llvm;

cl::opt<std::string> DefinitionFilename(
    cl::Positional, cl::desc("<definition.kore>"), cl::Required);

cl::opt<std::string>
    PatternFilename(cl::Positional, cl::desc("<pattern.kore>"), cl::Required);

cl::opt<bool> FilterSubst("subs", cl::desc("Filter Substitutions"), cl::init("-"));

int main(int argc, char **argv) {  
  cl::ParseCommandLineOptions(argc, argv);
  printKORE(std::cout, argv[1], argv[2], isatty(1), FilterSubst);
}
