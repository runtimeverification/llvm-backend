#include "kllvm/codegen/CreateTerm.h"
#include "kllvm/codegen/EmitConfigParser.h"
#include "kllvm/parser/KOREScanner.h"
#include "kllvm/parser/KOREParserDriver.h"

#include "llvm/IR/Instructions.h"
#include "llvm/IR/Constants.h"
#include "llvm/Support/raw_ostream.h"

using namespace kllvm;
using namespace kllvm::parser;

int main (int argc, char **argv) {
  if (argc < 2) {
    std::cerr << "Usage: test_configparser <file>\n";
    exit(1);
  }

  KOREScanner scanner(argv[1]);
  KOREParserDriver driver;
  KOREDefinition *definition;
  KOREParser parser(scanner, driver, &definition);
  parser.parse();
  definition->preprocess();

  // make a module
  llvm::LLVMContext Context;
  std::unique_ptr<llvm::Module> mod = newModule("test", Context);

  emitConfigParserFunctions(definition, mod.get());
  mod->print(llvm::outs(), nullptr);
  return 0;
}
