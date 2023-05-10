#include "api.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
  if (argc <= 2) {
    return 1;
  }

  struct kllvm_c_api api = load_c_api(argv[1]);

  api.kllvm_init();

  kore_pattern *pat = api.kore_pattern_parse_file(argv[2]);

  block *init = api.kore_pattern_construct(pat);
  block *end = api.take_steps(-1, init);

  kore_pattern *end_pat = api.kore_pattern_from_block(end);
  printf("%s\n", api.kore_pattern_dump(end_pat));

  api.kore_pattern_free(pat);
  api.kore_pattern_free(end_pat);
}
