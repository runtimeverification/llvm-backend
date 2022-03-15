#include <kllvm/binary/serializer.h>

#include <llvm/Support/CommandLine.h>

#include <iostream>

using namespace llvm;

cl::opt<uint64_t> Arity(cl::Positional, cl::desc("<arity>"), cl::Required);

int main(int argc, char **argv) {
  cl::ParseCommandLineOptions(argc, argv);

  auto s = kllvm::serializer(kllvm::serializer::DROP_HEADER);
  s.emit_length(Arity);

  for (auto b : s.data()) {
    std::cout << static_cast<uint8_t>(b);
  }

  return 0;
}
