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

#include <fstream>

using namespace kllvm;
using namespace kllvm::parser;

std::string getFilename(std::map<std::string, std::string> index, char **argv, KOREObjectSymbolDeclaration *decl) {
  return argv[3] + std::string("/") + index.at(decl->getSymbol()->getName());
}

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
    if (auto top = dynamic_cast<const KOREObjectCompositePattern *>(axiom->getPattern())) {
      for (auto pattern : top->getArguments()) {
        if (auto objPattern = dynamic_cast<KOREObjectPattern *>(pattern)) {
          std::string name;
          if (auto var = dynamic_cast<KOREObjectVariablePattern *>(objPattern)) {
            name = var->getName();
          } else if (auto comp = dynamic_cast<KOREObjectCompositePattern *>(objPattern)) {
            name = comp->getConstructor()->getName();
          }
          auto aliasMap = definition->getAliasDeclarations();
          if (aliasMap.count(name)) {
            auto aliasDecl = aliasMap.at(name);
            auto objSortVars = aliasDecl->getObjectSortVariables();
            auto args = aliasDecl->getSymbol()->getArguments();
            auto subst = std::unordered_map<KOREObjectSortVariable, KOREObjectSort *, HashSort>{};
            for (auto var : objSortVars) {
              subst.insert({*var, objPattern->getSort()});
            }
            objPattern->getSort()->substitute(subst);
          }
        }
      }
    }
    makeSideConditionFunction(axiom, definition, mod.get());
    if (!axiom->isTopAxiom()) {
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

  std::map<std::string, std::string> index;

  std::ifstream in(argv[3] + std::string("/index.txt"));

  std::string line;
  while(std::getline(in, line)) {
    size_t delim = line.find('\t');
    index[line.substr(0, delim)] = line.substr(delim+1);
  }

  in.close();

  for (auto &entry : definition->getSymbols()) {
    auto symbol = entry.second;
    auto decl = definition->getSymbolDeclarations().at(symbol->getName());
    if ((decl->getAttributes().count("function") && !decl->isHooked())) {
      std::string filename = getFilename(index, argv, decl);
      auto funcDt = parseYamlDecisionTree(filename, definition->getAllSymbols(), definition->getHookedSorts());
      makeEvalFunction(decl->getSymbol(), definition, mod.get(), funcDt);
    } else if (decl->isAnywhere()) {
      std::string filename = getFilename(index, argv, decl);
      auto funcDt = parseYamlDecisionTree(filename, definition->getAllSymbols(), definition->getHookedSorts());
      std::ostringstream Out;
      decl->getSymbol()->print(Out);
      makeAnywhereFunction(definition->getAllSymbols().at(Out.str()), definition, mod.get(), funcDt);
    }
  }

  mod->print(llvm::outs(), nullptr);
  return 0;
}
