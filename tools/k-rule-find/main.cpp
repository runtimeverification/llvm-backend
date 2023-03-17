#include <kllvm/binary/serializer.h>

#include <llvm/ADT/DenseMapInfo.h>
#include <llvm/ADT/StringRef.h>
#include <llvm/Support/CommandLine.h>

#include <cstdint>
#include <iostream>
#include <vector>

using namespace llvm;

cl::OptionCategory KRuleCat("k-rule-find options");

cl::opt<uint64_t> DefinitionDir(
    cl::Positional, cl::desc("<definition-dir>"), cl::Required,
    cl::cat(KRuleCat));

cl::opt<uint64_t> FilenameInput(
    cl::Positional, cl::desc("<filename>"), cl::Required, cl::cat(KRuleCat));

cl::opt<uint64_t> LineNumber(
    cl::Positional, cl::desc("<line-number>"), cl::Required, cl::cat(KRuleCat));

cl::opt<uint64_t> ColumnNumber(
    cl::Positional, cl::desc("<column-number>"), cl::cat(KRuleCat));

cl::opt<uint64_t> KTerm(
    cl::Positional, cl::desc("<k-term>"), cl::Required, cl::cat(KRuleCat));

int main(int argc, char **argv) {
  cl::HideUnrelatedOptions({&KRuleCat});
  cl::ParseCommandLineOptions(argc, argv);

  return 0;
}
