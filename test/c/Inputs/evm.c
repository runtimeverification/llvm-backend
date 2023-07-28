#include "api.h"

#include <stdio.h>

int main(int argc, char **argv) {
  if (argc <= 1) {
    return 1;
  }

  struct kllvm_c_api api = load_c_api(argv[1]);

  api.kllvm_init();

#include "llvm_calls.c"
}
