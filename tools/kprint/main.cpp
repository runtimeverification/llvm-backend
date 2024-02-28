#include <kllvm/ast/AST.h>
#include <kllvm/printer/printer.h>

#include <llvm/Support/CommandLine.h>

#include <iostream>
#include <string>
#include <unistd.h>

using namespace kllvm;
using namespace llvm;

cl::OptionCategory k_print_cat("kprint options");

cl::opt<std::string> definition_filename(
    cl::Positional, cl::desc("<definition.kore>"), cl::Required,
    cl::cat(k_print_cat));

cl::opt<std::string> pattern_filename(
    cl::Positional, cl::desc("<pattern.kore>"), cl::Required,
    cl::cat(k_print_cat));

cl::opt<std::string> arg_color(
    cl::Positional, cl::desc("[true|false|auto]"), cl::init("auto"),
    cl::cat(k_print_cat));

cl::opt<bool> filter_subst(
    cl::Positional, cl::desc("[true|false]"), cl::init(true),
    cl::cat(k_print_cat));

cl::opt<bool> print_as_kore(
    "kore",
    cl::desc("Perform unparsing, but print KORE rather than surface syntax"),
    cl::cat(k_print_cat));

int main(int argc, char **argv) {
  cl::HideUnrelatedOptions({&k_print_cat});
  cl::ParseCommandLineOptions(argc, argv);

  bool has_color = arg_color == "true" || (arg_color == "auto" && isatty(1));

  printKORE(
      std::cout, definition_filename, pattern_filename, has_color, filter_subst,
      !print_as_kore);
}
