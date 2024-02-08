#include <runtime/header.h>

#include <cstdint>
#include <iostream>
#include <memory>

extern "C" {

FILE *output_file = nullptr;
bool statistics = false;
bool binary_output = false;
bool proof_output = false;

extern int64_t steps;
extern bool safe_partial;

int32_t get_exit_code(block *);

[[noreturn]] void finish_rewriting(block *subject, bool error) {
  // This function is responsible for closing output_file when rewriting
  // finishes; because it can exit in a few different ways (exceptions,
  // std::exit etc.) it's cleaner to set up a smart pointer to do this safely
  // for us.
  [[maybe_unused]] auto closer
      = std::unique_ptr<FILE, decltype(&fclose)>(output_file, fclose);

  if (error && safe_partial) {
    throw std::runtime_error(
        "Attempted to evaluate partial function at an undefined input");
  }

  if (!output_file) {
    throw std::runtime_error(
        "Called finish_rewriting with no output file specified");
  }

  if (statistics) {
    printStatistics(output_file, steps);
  }

  if (!proof_output) {
    if (binary_output) {
      serializeConfigurationToFile(output_file, subject, true);
    } else {
      printConfiguration(output_file, subject);
    }
  }

  auto exit_code = error ? 113 : get_exit_code(subject);
  std::exit(exit_code);
}
}
