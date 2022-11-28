#include <iostream>

extern "C" bool hello(char const *name) {
  std::cout << "Hello, " << name << "!\n";
  return true;
}
