#include "api.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
  if (argc <= 1) {
    return 1;
  }

  struct kllvm_c_api api = load_c_api(argv[1]);
  api.kllvm_init();

  kore_sort *sort_int = api.kore_composite_sort_new("SortInt");
  kore_pattern *arg = api.kore_pattern_new_token("234", sort_int);

  kore_pattern *foo = api.kore_composite_pattern_new("Lblfoo");
  api.kore_composite_pattern_add_argument(foo, arg);

  block *arg_term = api.kore_pattern_construct(foo);

  block *result = api.kore_evaluate_function("Lblf{}", &arg_term);
  printf("%s\n", api.kore_block_dump(result));
}
