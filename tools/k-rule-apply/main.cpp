#include <kllvm/ast/AST.h>
#include <kllvm/parser/KOREParser.h>

#include <llvm/Support/CommandLine.h>

#include <cstdlib>
#include <dlfcn.h>
#include <fstream>
#include <iostream>
#include <optional>

#include "runtime/header.h"

using namespace llvm;
using namespace kllvm;

cl::OptionCategory KRuleCat("k-rule-apply options");

cl::opt<std::string> KompiledDir(
    cl::Positional, cl::desc("<kompiled-dir>"), cl::Required,
    cl::cat(KRuleCat));

cl::opt<std::string> RuleLabel(
    cl::Positional, cl::desc("<rule_label_filename>"), cl::Required,
    cl::cat(KRuleCat));

cl::opt<std::string> KOREPatternFilename(
    cl::Positional, cl::desc("<kore_pattern_filename>"), cl::Required,
    cl::cat(KRuleCat));

cl::opt<std::string> SharedLibPath(
    cl::Positional, cl::desc("<path_to_shared_lib>"), cl::cat(KRuleCat));

std::optional<std::string> getMatchFunctionName() {
  auto definition = KompiledDir + "/definition.kore";
  // Parse the definition.kore to get the AST.
  parser::KOREParser parser(definition);
  auto kore_ast = parser.definition();
  kore_ast->preprocess();

  // Iterate through axioms and return the one with the give rulen label if exits.
  for (auto axiom : kore_ast.get()->getAxioms()) {
    if (axiom->getAttributes().size() > 0) {
      // Check if the current axiom has the attribute label.
      auto attr = axiom->getAttributes().find("label");

      if (attr != axiom->getAttributes().end()) {
        // Compare the axiom's label with the given rule label.
        if (!RuleLabel.compare(axiom->getStringAttribute("label")))
          return "intern_match_" + std::to_string(axiom->getOrdinal());
      }
    }
  }
  std::cerr << RuleLabel << "\n";
  return std::nullopt;
}

int main(int argc, char **argv) {
  cl::HideUnrelatedOptions({&KRuleCat});
  cl::ParseCommandLineOptions(argc, argv);

  // Parse the given KORE Pattern and get the block* to use as input for the
  // match function.
  parser::KOREParser parser(KOREPatternFilename);
  auto InitialConfiguration = parser.pattern();

  auto match_function_name = getMatchFunctionName();
  if (match_function_name == std::nullopt) {
    std::cerr << "Rule with label " << RuleLabel << " does not exist.\n";
    return EXIT_FAILURE;
  }

  // Open the shared library that contains the llvm match functions.
  auto handle = dlopen(SharedLibPath.c_str(), RTLD_LAZY);

  // Check if the shared library exits in the given location.
  if (!handle) {
    std::cerr << "Error: " << dlerror() << "\n";
    return EXIT_FAILURE;
  }

  // Get utils functions from the shared lib, cast them to their right type, and
  // call each with its appropriate argument if any.
  void *construct_ptr = dlsym(handle, "constructInitialConfiguration");
  if (construct_ptr == NULL) {
    std::cerr << "Error: " << dlerror() << "\n";
    dlclose(handle);
    return EXIT_FAILURE;
  }

  void *resetMatchReason_ptr = dlsym(handle, "resetMatchReason");
  if (resetMatchReason_ptr == NULL) {
    std::cerr << "Error: " << dlerror() << "\n";
    dlclose(handle);
    return EXIT_FAILURE;
  }

  void *match_function_ptr = dlsym(handle, match_function_name->c_str());
  if (match_function_ptr == NULL) {
    std::cerr << "Error: " << dlerror() << "\n";
    dlclose(handle);
    return EXIT_FAILURE;
  }

  void *matchLog_ptr = dlsym(handle, "getMatchLog");
  if (matchLog_ptr == NULL) {
    std::cerr << "Error: " << dlerror() << "\n";
    dlclose(handle);
    return EXIT_FAILURE;
  }

  void *matchLogSize_ptr = dlsym(handle, "getMatchLogSize");
  if (matchLogSize_ptr == NULL) {
    std::cerr << "Error: " << dlerror() << "\n";
    dlclose(handle);
    return EXIT_FAILURE;
  }

  void *printMatchResult_ptr = dlsym(handle, "printMatchResult");
  if (printMatchResult_ptr == NULL) {
    std::cerr << "Error: " << dlerror() << "\n";
    dlclose(handle);
    return EXIT_FAILURE;
  }

  void *initStaticObjects_ptr = dlsym(handle, "initStaticObjects");
  if (initStaticObjects_ptr == NULL) {
    std::cerr << "Error: " << dlerror() << "\n";
    dlclose(handle);
    return EXIT_FAILURE;
  }

  auto resetMatchReason = reinterpret_cast<void (*)()>(resetMatchReason_ptr);
  auto match_function = reinterpret_cast<void (*)(block *)>(match_function_ptr);
  auto constructInitialConfiguration
      = reinterpret_cast<void *(*)(const KOREPattern *)>(construct_ptr);
  auto matchLog = reinterpret_cast<MatchLog *(*)()>(matchLog_ptr);
  auto matchLogSize = reinterpret_cast<size_t (*)()>(matchLogSize_ptr);
  auto printMatchResult = reinterpret_cast<void (*)(
      std::ostream &, MatchLog *, size_t, std::string const &)>(
      printMatchResult_ptr);
  auto initStaticObjects = reinterpret_cast<void (*)()>(initStaticObjects_ptr);

  resetMatchReason();
  initStaticObjects();
  auto b = (block *)constructInitialConfiguration(InitialConfiguration.get());
  match_function(b);
  MatchLog *log = matchLog();
  size_t logSize = matchLogSize();
  printMatchResult(std::cout, log, logSize, KompiledDir);

  dlclose(handle);
  return 0;
}
