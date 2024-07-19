#include "api.h"

#include <stdio.h>
#include <stdlib.h>

void __gmpz_clear(void *p) {
  abort();
}

int main(int argc, char **argv) {
  if (argc <= 1) {
    return 1;
  }

  struct kllvm_c_api api = load_c_api(argv[1]);

  api.kllvm_init();

  kore_sort *sort_foo = api.kore_composite_sort_new("SortFoo");
  kore_pattern *pat = api.kore_composite_pattern_new("Lblfoo");

  kore_pattern *input = api.kore_pattern_make_interpreter_input(pat, sort_foo);

  block *term = api.kore_pattern_construct(input);
  block *after = api.take_steps(-1, term);

  printf("%s", api.kore_block_dump(after));
}
