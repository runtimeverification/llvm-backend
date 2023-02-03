#include <kllvm/binary/serializer.h>

#include <cstdint>
#include <iostream>
#include <vector>

int main() {
  auto s = kllvm::serializer();

  for (auto b : s.data()) {
    std::cout << static_cast<uint8_t>(b);
  }

  return 0;
}
