#include "auxiliar.h"

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
  if (!match_function_name.has_value()) {
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

  // Get util function from the shared lib, cast it to its right type, and call
  // with its appropriate argument if any.
  void *match_function_ptr = dlsym(handle, match_function_name->c_str());
  if (match_function_ptr == nullptr) {
    std::cerr << "Error: " << dlerror() << "\n";
    dlclose(handle);
    return EXIT_FAILURE;
  }
  auto match_function = reinterpret_cast<void (*)(block *)>(match_function_ptr);

  resetMatchReason(handle);
  initStaticObjects(handle);
  auto b = constructInitialConfiguration(InitialConfiguration.get(), handle);
  if (b == nullptr) {
    std::cerr << "Error: " << dlerror() << "\n";
    return EXIT_FAILURE;
  }

  match_function((block *)b);
  auto log = getMatchLog(handle);
  if (log == nullptr) {
    std::cerr << "Error: " << dlerror() << "\n";
    return EXIT_FAILURE;
  }

  size_t logSize = getMatchLogSize(handle);
  if (logSize == -1) {
    std::cerr << "Error: " << dlerror() << "\n";
    return EXIT_FAILURE;
  }

  printMatchResult(std::cout, (MatchLog *)log, logSize, KompiledDir, handle);

  dlclose(handle);
  return 0;
}
