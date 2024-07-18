#include "shims.h"

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

int main(int argc, char **argv) {
  cl::HideUnrelatedOptions({&k_rule_cat});
  cl::ParseCommandLineOptions(argc, argv);

  // Open the shared library that contains the llvm match functions.
  auto *handle = dlopen(shared_lib_path.c_str(), RTLD_LAZY);

  // Check if the shared library exits in the given location.
  if (!handle) {
    std::cerr << "Error: " << dlerror() << "\n";
    return EXIT_FAILURE;
  }

  auto match_function_name = get_match_function_name(
      kompiled_dir + "/definition.kore", rule_label, handle);
  if (!match_function_name) {
    std::cerr << "Rule with label " << rule_label << " does not exist.\n";
    return EXIT_FAILURE;
  }

  // Get util function from the shared lib, cast it to its right type, and call
  // with its appropriate argument if any.
  void *match_function_ptr = dlsym(handle, match_function_name);
  if (match_function_ptr == nullptr) {
    std::cerr << "Error: " << dlerror() << "\n";
    dlclose(handle);
    return EXIT_FAILURE;
  }

  // NOLINTNEXTLINE(*-reinterpret-cast)
  auto match_function = reinterpret_cast<void (*)(block *)>(match_function_ptr);

  reset_match_reason(handle);
  init_static_objects(handle);
  auto *b = parse_initial_configuration(kore_pattern_filename, handle);
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

  size_t log_size = getmatch_log_size(handle);
  if (log_size == -1) {
    std::cerr << "Error: " << dlerror() << "\n";
    return EXIT_FAILURE;
  }

  print_match_result(
      std::cout, (match_log *)log, log_size, kompiled_dir, handle);

  dlclose(handle);
  return 0;
}
