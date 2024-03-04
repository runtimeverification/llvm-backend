#include "auxiliar.h"

using namespace llvm;
using namespace kllvm;

cl::OptionCategory k_rule_cat("k-rule-apply options");

cl::opt<std::string> kompiled_dir(
    cl::Positional, cl::desc("<kompiled-dir>"), cl::Required,
    cl::cat(k_rule_cat));

cl::opt<std::string> rule_label(
    cl::Positional, cl::desc("<rule_label_filename>"), cl::Required,
    cl::cat(k_rule_cat));

cl::opt<std::string> kore_pattern_filename(
    cl::Positional, cl::desc("<kore_pattern_filename>"), cl::Required,
    cl::cat(k_rule_cat));

cl::opt<std::string> shared_lib_path(
    cl::Positional, cl::desc("<path_to_shared_lib>"), cl::cat(k_rule_cat));

std::optional<std::string> getMatchFunctionName() {
  auto definition = kompiled_dir + "/definition.kore";
  // Parse the definition.kore to get the AST.
  parser::kore_parser parser(definition);
  auto kore_ast = parser.definition();
  kore_ast->preprocess();

  // Iterate through axioms and return the one with the give rulen label if exits.
  for (auto *axiom : kore_ast.get()->get_axioms()) {
    // Check if the current axiom has the attribute label.
    if (axiom->attributes().contains(attribute_set::key::Label)) {
      // Compare the axiom's label with the given rule label.
      if (rule_label
          == axiom->attributes().get_string(attribute_set::key::Label)) {
        return "intern_match_" + std::to_string(axiom->get_ordinal());
      }
    }
  }
  std::cerr << rule_label << "\n";
  return std::nullopt;
}

int main(int argc, char **argv) {
  cl::HideUnrelatedOptions({&k_rule_cat});
  cl::ParseCommandLineOptions(argc, argv);

  // Parse the given KORE Pattern and get the block* to use as input for the
  // match function.
  parser::kore_parser parser(kore_pattern_filename.getValue());
  auto initial_configuration = parser.pattern();

  auto match_function_name = getMatchFunctionName();
  if (!match_function_name.has_value()) {
    std::cerr << "Rule with label " << rule_label << " does not exist.\n";
    return EXIT_FAILURE;
  }

  // Open the shared library that contains the llvm match functions.
  auto *handle = dlopen(shared_lib_path.c_str(), RTLD_LAZY);

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

  // NOLINTNEXTLINE(*-reinterpret-cast)
  auto match_function = reinterpret_cast<void (*)(block *)>(match_function_ptr);

  resetMatchReason(handle);
  initStaticObjects(handle);
  auto *b = constructInitialConfiguration(initial_configuration.get(), handle);
  if (b == nullptr) {
    std::cerr << "Error: " << dlerror() << "\n";
    return EXIT_FAILURE;
  }

  match_function((block *)b);
  auto *log = getmatch_log(handle);
  if (log == nullptr) {
    std::cerr << "Error: " << dlerror() << "\n";
    return EXIT_FAILURE;
  }

  size_t log_size = getmatch_logSize(handle);
  if (log_size == -1) {
    std::cerr << "Error: " << dlerror() << "\n";
    return EXIT_FAILURE;
  }

  printMatchResult(std::cout, (match_log *)log, log_size, kompiled_dir, handle);

  dlclose(handle);
  return 0;
}
