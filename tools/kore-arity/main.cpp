#include <kllvm/binary/serializer.h>

#include <llvm/ADT/DenseMapInfo.h>
#include <llvm/ADT/StringRef.h>
#include <llvm/Support/CommandLine.h>

#include <cstdint>
#include <iostream>
#include <vector>

using namespace llvm;

cl::OptionCategory kore_arity_cat("kore-arity options");

cl::opt<uint64_t> arity(
    cl::Positional, cl::desc("<arity>"), cl::Required, cl::cat(kore_arity_cat));

int main(int argc, char **argv) {
  cl::HideUnrelatedOptions({&kore_arity_cat});
  cl::ParseCommandLineOptions(argc, argv);

  auto s = kllvm::serializer(kllvm::serializer::DropHeader);
  s.emit_length(arity);

  for (auto b : s.data()) {
    std::cout << static_cast<uint8_t>(b);
  }

  return 0;
}
