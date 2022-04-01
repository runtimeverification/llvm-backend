#include <kllvm/binary/serializer.h>

#include <iostream>

int main() {
  auto s = kllvm::serializer();

  for (auto b : s.data()) {
    std::cout << static_cast<uint8_t>(b);
  }

  return 0;
}
