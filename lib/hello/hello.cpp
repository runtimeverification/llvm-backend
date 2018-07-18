#include "kllvm/hello/hello.h"
#include "llvm/ADT/APInt.h"

int compute_sample(int a) {
  return llvm::APInt(128, a, true).getSExtValue();
}
