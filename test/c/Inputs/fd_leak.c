#include "api.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
  if (argc <= 2) {
    return 1;
  }

  struct kllvm_c_api api = load_c_api(argv[1]);

  FILE *init_fp = fopen(argv[2], "r");
  assert(init_fp);

  fseek(init_fp, 0, SEEK_END);
  long init_size = ftell(init_fp);

  fseek(init_fp, 0, SEEK_SET);

  char *buf = malloc(init_size + 1);
  assert(buf);

  size_t read = fread(buf, 1, init_size, init_fp);
  buf[init_size] = '\0';

  for (int i = 0; i < 10000; ++i) {
    kore_pattern *pat = api.kore_pattern_parse(buf);
    api.kore_pattern_free(pat);
  }

  free(buf);
}
