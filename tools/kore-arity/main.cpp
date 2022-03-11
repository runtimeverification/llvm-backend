#include <kllvm/binary/serializer.h>

#include <llvm/Support/CommandLine.h>

#include <iostream>

using namespace llvm;

cl::opt<int> Arity(cl::Positional, cl::desc("<arity>"), cl::Required);

int main(int argc, char **argv) {
  cl::ParseCommandLineOptions(argc, argv);

  if (Arity > std::numeric_limits<int16_t>::max()) {
    std::cerr << "Arity " << Arity
              << " is too large for the current binary KORE format (16 bits)\n";
    return 1;
  }

  auto bytes = kllvm::detail::to_bytes(static_cast<int16_t>(Arity));
  for (auto b : bytes) {
    std::cout << static_cast<uint8_t>(b);
  }

  return 0;
}
