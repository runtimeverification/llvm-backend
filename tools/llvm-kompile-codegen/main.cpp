#include <kllvm/ast/AST.h>
#include <kllvm/codegen/ApplyPasses.h>
#include <kllvm/codegen/CreateTerm.h>
#include <kllvm/codegen/Debug.h>
#include <kllvm/codegen/Decision.h>
#include <kllvm/codegen/DecisionParser.h>
#include <kllvm/codegen/EmitConfigParser.h>
#include <kllvm/codegen/Options.h>
#include <kllvm/parser/KOREParser.h>
#include <kllvm/parser/location.h>

#include <llvm/Bitcode/BitcodeWriter.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/Support/CommandLine.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Support/raw_ostream.h>

#include <fmt/format.h>

#include <libgen.h>
#include <sys/stat.h>

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <list>
#include <map>
#include <memory>
#include <string>
#include <unordered_map>
#include <utility>

using namespace llvm;
using namespace kllvm;
using namespace kllvm::parser;

namespace fs = std::filesystem;

cl::OptionCategory CodegenToolCat("llvm-kompile-codegen options");

cl::opt<std::string> Definition(
    cl::Positional, cl::desc("<definition.kore>"), cl::Required,
    cl::cat(CodegenToolCat));

cl::opt<std::string> DecisionTree(
    cl::Positional, cl::desc("<dt.yaml>"), cl::Required,
    cl::cat(CodegenToolCat));

cl::opt<std::string> Directory(
    cl::Positional, cl::desc("<dir>"), cl::Required, cl::cat(CodegenToolCat));

cl::opt<std::string> OutputFile(
    "output", cl::desc("Output file path"), cl::init("-"),
    cl::cat(CodegenToolCat));

cl::alias OutputFileAlias(
    "o", cl::desc("Alias for --output"), cl::aliasopt(OutputFile),
    cl::cat(CodegenToolCat));

namespace {

fs::path dt_dir() {
  return Directory.getValue();
}

fs::path get_indexed_filename(
    std::map<std::string, std::string> const &index,
    KORESymbolDeclaration *decl) {
  return dt_dir() / index.at(decl->getSymbol()->getName());
}

std::map<std::string, std::string> read_index_file() {
  auto index = std::map<std::string, std::string>{};
  auto in = std::ifstream(dt_dir() / "index.txt");

  auto line = std::string{};
  while (std::getline(in, line)) {
    size_t delim = line.find('\t');
    index[line.substr(0, delim)] = line.substr(delim + 1);
  }

  return index;
}

template <typename F>
void perform_output(F &&action) {
  if (OutputFile == "-") {
    std::invoke(std::forward<F>(action), llvm::outs());
  } else {
    auto err = std::error_code{};
    auto os = raw_fd_ostream(OutputFile, err, sys::fs::FA_Write);

    if (err) {
      throw std::runtime_error(
          fmt::format("Error opening file {}: {}", OutputFile, err.message()));
    }

    std::invoke(std::forward<F>(action), os);
  }
}

void initialize_llvm() {
  InitializeAllTargetInfos();
  InitializeAllTargets();
  InitializeAllTargetMCs();
  InitializeAllAsmParsers();
  InitializeAllAsmPrinters();
}

} // namespace

int main(int argc, char **argv) {
  initialize_llvm();

  cl::HideUnrelatedOptions({&CodegenToolCat, &CodegenLibCat});
  cl::ParseCommandLineOptions(argc, argv);

  validate_codegen_args(OutputFile == "-");

  KOREParser parser(Definition);
  ptr<KOREDefinition> definition = parser.definition();
  definition->preprocess();

  llvm::LLVMContext Context;
  std::unique_ptr<llvm::Module> mod = newModule("definition", Context);

  if (Debug) {
    initDebugInfo(mod.get(), Definition);
  }

  auto kompiled_dir = fs::absolute(Definition.getValue()).parent_path();
  addKompiledDirSymbol(Context, kompiled_dir, mod.get(), Debug);

  for (auto *axiom : definition->getAxioms()) {
    makeSideConditionFunction(axiom, definition.get(), mod.get());
    if (!axiom->isTopAxiom()) {
      makeApplyRuleFunction(axiom, definition.get(), mod.get());
    } else {
      auto dt_filename
          = dt_dir() / fmt::format("dt_{}.yaml", axiom->getOrdinal());
      if (fs::exists(dt_filename) && !ProofHintInstrumentation) {
        auto residuals = parseYamlSpecialDecisionTree(
            mod.get(), dt_filename, definition->getAllSymbols(),
            definition->getHookedSorts());
        makeApplyRuleFunction(
            axiom, definition.get(), mod.get(), residuals.residuals);
        makeStepFunction(axiom, definition.get(), mod.get(), residuals);
      } else {
        makeApplyRuleFunction(axiom, definition.get(), mod.get(), true);
      }

      auto match_filename
          = dt_dir() / fmt::format("match_{}.yaml", axiom->getOrdinal());
      if (fs::exists(match_filename)) {
        auto *dt = parseYamlDecisionTree(
            mod.get(), match_filename, definition->getAllSymbols(),
            definition->getHookedSorts());
        makeMatchReasonFunction(definition.get(), mod.get(), axiom, dt);
      }
    }
  }

  emitConfigParserFunctions(definition.get(), mod.get());

  auto *dt = parseYamlDecisionTree(
      mod.get(), DecisionTree, definition->getAllSymbols(),
      definition->getHookedSorts());
  makeStepFunction(definition.get(), mod.get(), dt, false);
  auto *dtSearch = parseYamlDecisionTree(
      mod.get(), dt_dir() / "dt-search.yaml", definition->getAllSymbols(),
      definition->getHookedSorts());
  makeStepFunction(definition.get(), mod.get(), dtSearch, true);

  auto index = read_index_file();
  for (const auto &entry : definition->getSymbols()) {
    auto *symbol = entry.second;
    auto *decl = definition->getSymbolDeclarations().at(symbol->getName());
    if (decl->getAttributes().count("function") && !decl->isHooked()) {
      auto filename = get_indexed_filename(index, decl);
      auto *funcDt = parseYamlDecisionTree(
          mod.get(), filename, definition->getAllSymbols(),
          definition->getHookedSorts());
      makeEvalFunction(decl->getSymbol(), definition.get(), mod.get(), funcDt);
    } else if (decl->isAnywhere()) {
      auto filename = get_indexed_filename(index, decl);
      auto *funcDt = parseYamlDecisionTree(
          mod.get(), filename, definition->getAllSymbols(),
          definition->getHookedSorts());

      makeAnywhereFunction(
          definition->getAllSymbols().at(ast_to_string(*decl->getSymbol())),
          definition.get(), mod.get(), funcDt);
    }
  }

  if (Debug) {
    finalizeDebugInfo();
  }

  if (!NoOptimize) {
    apply_kllvm_opt_passes(*mod);
  }

  perform_output([&](auto &os) {
    if (EmitObject) {
      generate_object_file(*mod, os);
    } else {
      if (BinaryIR) {
        WriteBitcodeToFile(*mod, os);
      } else {
        mod->print(os, nullptr);
      }
    }
  });

  return 0;
}
