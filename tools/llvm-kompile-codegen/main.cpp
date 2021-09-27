#include "kllvm/codegen/CreateTerm.h"
#include "kllvm/codegen/Debug.h"
#include "kllvm/codegen/Decision.h"
#include "kllvm/codegen/DecisionParser.h"
#include "kllvm/codegen/EmitConfigParser.h"
#include "kllvm/parser/KOREParser.h"
#include "kllvm/parser/KOREScanner.h"

#include "llvm/IR/Instructions.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/raw_ostream.h"

#include <libgen.h>
#include <sys/stat.h>

#include <fstream>
#include <iostream>

using namespace kllvm;
using namespace kllvm::parser;

std::string getFilename(
    std::map<std::string, std::string> index, char **argv,
    KORESymbolDeclaration *decl) {
  return argv[3] + std::string("/") + index.at(decl->getSymbol()->getName());
}

int main(int argc, char **argv) {
  if (argc < 5) {
    std::cerr
        << "Usage: llvm-kompile-codegen <def.kore> <dt.yaml> <dir> [1|0]\n";
    exit(1);
  }

  CODEGEN_DEBUG = atoi(argv[4]);

  KOREParser parser(argv[1]);
  ptr<KOREDefinition> definition = parser.definition();
  definition->preprocess();

  llvm::LLVMContext Context;

  char *realPath = realpath(argv[1], NULL);

  std::unique_ptr<llvm::Module> mod = newModule("definition", Context);

  if (CODEGEN_DEBUG) {
    initDebugInfo(mod.get(), argv[1]);
  }

  addKompiledDirSymbol(Context, dirname(realPath), mod.get(), CODEGEN_DEBUG);

  for (auto axiom : definition->getAxioms()) {
    makeSideConditionFunction(axiom, definition.get(), mod.get());
    if (!axiom->isTopAxiom()) {
      makeApplyRuleFunction(axiom, definition.get(), mod.get());
    } else {
      std::string filename = argv[3] + std::string("/") + "dt_"
                             + std::to_string(axiom->getOrdinal()) + ".yaml";
      struct stat buf;
      if (stat(filename.c_str(), &buf) == 0) {
        auto residuals = parseYamlSpecialDecisionTree(
            mod.get(), filename, definition->getAllSymbols(),
            definition->getHookedSorts());
        makeApplyRuleFunction(
            axiom, definition.get(), mod.get(), residuals.residuals);
        makeStepFunction(axiom, definition.get(), mod.get(), residuals);
      } else {
        makeApplyRuleFunction(axiom, definition.get(), mod.get(), true);
      }
      filename = argv[3] + std::string("/") + "match_"
                 + std::to_string(axiom->getOrdinal()) + ".yaml";
      if (stat(filename.c_str(), &buf) == 0) {
        auto dt = parseYamlDecisionTree(
            mod.get(), filename, definition->getAllSymbols(),
            definition->getHookedSorts());
        makeMatchReasonFunction(definition.get(), mod.get(), axiom, dt);
      }
    }
  }

  emitConfigParserFunctions(definition.get(), mod.get());

  auto dt = parseYamlDecisionTree(
      mod.get(), argv[2], definition->getAllSymbols(),
      definition->getHookedSorts());
  makeStepFunction(definition.get(), mod.get(), dt, false);
  auto dtSearch = parseYamlDecisionTree(
      mod.get(), argv[3] + std::string("/") + "dt-search.yaml",
      definition->getAllSymbols(), definition->getHookedSorts());
  makeStepFunction(definition.get(), mod.get(), dtSearch, true);

  std::map<std::string, std::string> index;

  std::ifstream in(argv[3] + std::string("/index.txt"));

  std::string line;
  while (std::getline(in, line)) {
    size_t delim = line.find('\t');
    index[line.substr(0, delim)] = line.substr(delim + 1);
  }

  in.close();

  for (auto &entry : definition->getSymbols()) {
    auto symbol = entry.second;
    auto decl = definition->getSymbolDeclarations().at(symbol->getName());
    if ((decl->getAttributes().count("function") && !decl->isHooked())) {
      std::string filename = getFilename(index, argv, decl);
      auto funcDt = parseYamlDecisionTree(
          mod.get(), filename, definition->getAllSymbols(),
          definition->getHookedSorts());
      makeEvalFunction(decl->getSymbol(), definition.get(), mod.get(), funcDt);
    } else if (decl->isAnywhere()) {
      std::string filename = getFilename(index, argv, decl);
      auto funcDt = parseYamlDecisionTree(
          mod.get(), filename, definition->getAllSymbols(),
          definition->getHookedSorts());
      std::ostringstream Out;
      decl->getSymbol()->print(Out);
      makeAnywhereFunction(
          definition->getAllSymbols().at(Out.str()), definition.get(),
          mod.get(), funcDt);
    }
  }

#ifdef __APPLE__
  // apple symbols are mangled slightly with an underscore in front, so we need
  // to adjust the name of the __LLVM_StackMaps symbol slightly
  auto StackMap = mod->getOrInsertGlobal(
      "__LLVM_StackMaps", llvm::Type::getInt8Ty(Context));
  auto StackMapGlobal = llvm::cast<llvm::GlobalVariable>(StackMap);
  StackMapGlobal->setName("_LLVM_StackMaps");
#endif

  if (CODEGEN_DEBUG) {
    finalizeDebugInfo();
  }

  mod->print(llvm::outs(), nullptr);
  return 0;
}
