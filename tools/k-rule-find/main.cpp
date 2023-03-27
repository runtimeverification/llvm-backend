
#include "kllvm/parser/KOREParser.h"
#include "llvm/Support/CommandLine.h"

#include <dlfcn.h>
#include <iostream>

#include "runtime/header.h"

using namespace llvm;

cl::OptionCategory KRuleCat("k-rule-find options");

cl::opt<std::string> KTermFilename(
    cl::Positional, cl::desc("<kore_term_filename>"), cl::Required,
    cl::cat(KRuleCat));

cl::opt<std::string> Location(
    cl::Positional, cl::desc("<filename.k:line:column>"), cl::Required,
    cl::cat(KRuleCat));

cl::opt<std::string> SharedLibPath(
    cl::Positional, cl::desc("<path_to_shared_lib>"), cl::cat(KRuleCat));

void parseLocation(
    std::string loc, std::string &filename, int64_t &line, int64_t &column) {
  size_t pos = loc.find(":");
  if (pos == std::string::npos) {
    std::cerr
        << "Rule's location must me in the format: defintion.k:line:column\n";
    exit(EXIT_FAILURE);
  }
  filename = loc.substr(0, pos);

  std::string lineColumn = loc.substr(pos + 1);
  size_t pos_lc = lineColumn.find(":");

  if (pos_lc == std::string::npos) {
    std::cerr
        << "Rule's location must me in the format: defintion.k:line:column\n";
    exit(EXIT_FAILURE);
  }

  line = stoi(lineColumn.substr(0, pos_lc));
  column = stoi(lineColumn.substr(pos_lc + 1));
}

int main(int argc, char **argv) {
  cl::HideUnrelatedOptions({&KRuleCat});
  cl::ParseCommandLineOptions(argc, argv);

  std::string filename;
  int64_t lineNumber, columnNumber;
  parseLocation(Location, filename, lineNumber, columnNumber);

  std::cout << filename << " : " << lineNumber << " : " << columnNumber << "\n";

  // Open the shared library that contains the llvm match functions.
  auto handle = dlopen(SharedLibPath.c_str(), RTLD_LAZY);

  // Check if the shared library exits in the given location.
  if (!handle) {
    std::cerr << "Error: " << dlerror() << "\n";
    return EXIT_FAILURE;
  }

  // Parse the given KTerm and get the block* to use as input for the match
  // function.
  kllvm::parser::KOREParser parser(KTermFilename);
  auto InitialConfiguration = parser.pattern();
  //auto b = (block *)constructInitialConfiguration(InitialConfiguration.get());
  block *b = nullptr;

  // Get the llvm match function pointer and cast it to right type.
  void *llvm_function_ptr = dlsym(handle, "llvm_match_function");
  if (llvm_function_ptr == NULL) {
    std::cerr
        << "Couldn't find the llvm_match_function on the given shared lib.\n";
    return EXIT_FAILURE;
  }
  auto match_function
      = reinterpret_cast<void (*)(block *, std::string, int64_t, int64_t)>(
          llvm_function_ptr);

  // Call the function to check if the given K Term matches with the given rule.
  match_function(b, filename, lineNumber, columnNumber);

  return 0;
}
