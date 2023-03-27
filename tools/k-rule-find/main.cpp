#include "kllvm/ast/AST.h"
#include "kllvm/parser/KOREParser.h"
#include "llvm/Support/CommandLine.h"

#include <dlfcn.h>
#include <iostream>

#include "runtime/header.h"

using namespace llvm;

struct Location {
  char *filename;
  int64_t line;
  int64_t column;
};

cl::OptionCategory KRuleCat("k-rule-find options");

cl::opt<std::string> KOREPatternFilename(
    cl::Positional, cl::desc("<kore_patten_filename>"), cl::Required,
    cl::cat(KRuleCat));

cl::opt<std::string> RuleLocation(
    cl::Positional, cl::desc("<filename.k:line[:column]>"), cl::Required,
    cl::cat(KRuleCat));

cl::opt<std::string> SharedLibPath(
    cl::Positional, cl::desc("<path_to_shared_lib>"), cl::cat(KRuleCat));

Location parseLocation(std::string loc) {
  char *filename;
  int64_t line, column = -1;
  size_t pos = loc.find(":");
  if (pos == std::string::npos) {
    std::cerr
        << "Rule's location must me in the format: defintion.k:line[:column]\n";
    exit(EXIT_FAILURE);
  }
  filename = (char *)loc.substr(0, pos).c_str();

  std::string lineColumn = loc.substr(pos + 1);
  size_t pos_lc = lineColumn.find(":");

  // If another “:” isn’t found, the tool assumes no column number was given.
  if (pos_lc == std::string::npos) {
    line = stoi(lineColumn);
  } else {
    line = stoi(lineColumn.substr(0, pos_lc));
    column = stoi(lineColumn.substr(pos_lc + 1));
  }

  return {filename, line, column};
}

int main(int argc, char **argv) {
  cl::HideUnrelatedOptions({&KRuleCat});
  cl::ParseCommandLineOptions(argc, argv);

  auto loc = parseLocation(RuleLocation);

  // Open the shared library that contains the llvm match functions.
  auto handle = dlopen(SharedLibPath.c_str(), RTLD_LAZY);

  // Check if the shared library exits in the given location.
  if (!handle) {
    std::cerr << "Error: " << dlerror() << "\n";
    return EXIT_FAILURE;
  }

  // Parse the given KORE Pattern and get the block* to use as input for the
  // match function.
  kllvm::parser::KOREParser parser(KOREPatternFilename);
  auto InitialConfiguration = parser.pattern();

  // Get the constructInitialConfiguration utils function from the function,
  // cast it to right type, and call it to get the block pointer.
  void *construct_ptr = dlsym(
      handle, "_Z29constructInitialConfigurationPKN5kllvm11KOREPatternE");
  if (construct_ptr == NULL) {
    std::cerr << "Couldn't find the constructInitialConfiguration on the given "
                 "shared lib.\n";
    return EXIT_FAILURE;
  }
  auto constructInitialConfiguration
      = reinterpret_cast<void *(*)(const kllvm::KOREPattern *)>(construct_ptr);
  auto b = (block *)constructInitialConfiguration(InitialConfiguration.get());

  // Get the llvm match function pointer and cast it to right type.
  void *llvm_function_ptr = dlsym(handle, "llvm_match_function");
  if (llvm_function_ptr == NULL) {
    std::cerr
        << "Couldn't find the llvm_match_function on the given shared lib.\n";
    return EXIT_FAILURE;
  }
  auto match_function
      = reinterpret_cast<void (*)(block *, char *, int64_t, int64_t)>(
          llvm_function_ptr);

  // Call the function to check if the given KORE pattern matches with the given
  // rule.
  match_function(b, loc.filename, loc.line, loc.column);

  return 0;
}
