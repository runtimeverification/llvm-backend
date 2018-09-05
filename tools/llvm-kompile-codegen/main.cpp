#include "kllvm/codegen/CreateTerm.h"
#include "kllvm/codegen/Decision.h"
#include "kllvm/codegen/DecisionParser.h"
#include "kllvm/codegen/EmitConfigParser.h"
#include "kllvm/parser/KOREScanner.h"
#include "kllvm/parser/KOREParserDriver.h"

#include "llvm/IR/Instructions.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/raw_ostream.h"

using namespace kllvm;
using namespace kllvm::parser;

int main (int argc, char **argv) {
  if (argc < 3) {
    std::cerr << "Usage: llvm-kompile-codegen <def.kore> <dt.yaml>\n";
    exit(1);
  }

  KOREScanner scanner(argv[1]);
  KOREParserDriver driver;
  KOREDefinition *definition;
  KOREParser parser(scanner, driver, &definition);
  parser.parse();
  definition->preprocess();

  llvm::LLVMContext Context;

  std::unique_ptr<llvm::Module> mod = newModule("definition", Context);

  for (auto axiom : definition->getAxioms()) {
    makeApplyRuleFunction(axiom, definition, mod.get());
    makeSideConditionFunction(axiom, definition, mod.get());
  }

  emitConfigParserFunctions(definition, mod.get());

  auto dt = parseYamlDecisionTree(argv[2], 1, definition->getAllSymbols());
  makeStepFunction(definition, mod.get(), dt);

  mod->print(llvm::outs(), nullptr);
  return 0;
}
