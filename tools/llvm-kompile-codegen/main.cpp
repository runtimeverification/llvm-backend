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

#include <libgen.h>
#include <sys/stat.h>

using namespace kllvm;
using namespace kllvm::parser;

int main (int argc, char **argv) {
  if (argc < 4) {
    std::cerr << "Usage: llvm-kompile-codegen <def.kore> <dt.yaml> <dir>\n";
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
    makeSideConditionFunction(axiom, definition, mod.get());
    if (axiom->isFunction()) {
      makeApplyRuleFunction(axiom, definition, mod.get());
    } else {
      std::string filename = argv[3] + std::string("/") + "dt_" + std::to_string(axiom->getOrdinal()) + ".yaml";
      struct stat buf;
      if (stat(filename.c_str(), &buf) == 0) {
        auto residuals = parseYamlSpecialDecisionTree(filename, definition->getAllSymbols(), definition->getHookedSorts());
        makeApplyRuleFunction(axiom, definition, mod.get(), residuals.residuals);
        makeStepFunction(axiom, definition, mod.get(), residuals);
      } else {
        makeApplyRuleFunction(axiom, definition, mod.get(), true);
      }
    }
  }

  emitConfigParserFunctions(definition, mod.get());

  auto dt = parseYamlDecisionTree(argv[2], definition->getAllSymbols(), definition->getHookedSorts());
  makeStepFunction(definition, mod.get(), dt);

  for (auto &entry : definition->getSymbolDeclarations()) {
    auto decl = entry.second;
    std::string filename = argv[3] + std::string("/") + decl->getSymbol()->getName() + ".yaml";
    if ((decl->getAttributes().count("function") && !decl->isHooked())) {
      auto funcDt = parseYamlDecisionTree(filename, definition->getAllSymbols(), definition->getHookedSorts());
      makeEvalFunction(decl->getSymbol(), definition, mod.get(), funcDt);
    } else if (decl->isAnywhere()) {
      auto funcDt = parseYamlDecisionTree(filename, definition->getAllSymbols(), definition->getHookedSorts());
      std::ostringstream Out;
      decl->getSymbol()->print(Out);
      makeAnywhereFunction(definition->getAllSymbols().lookup(Out.str()), definition, mod.get(), funcDt);
    }
  }

  mod->print(llvm::outs(), nullptr);
  return 0;
}
