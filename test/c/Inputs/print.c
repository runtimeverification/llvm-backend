#include "api.h"

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
  if (argc <= 1) {
    return 1;
  }

  struct kllvm_c_api api = load_c_api(argv[1]);

  api.kllvm_init();

  kore_sort *sort_int = api.kore_composite_sort_new("SortInt");

  kore_pattern *foo = api.kore_composite_pattern_new("Lblfoo");
  kore_pattern *val = api.kore_pattern_new_token("423", sort_int);
  api.kore_sort_free(sort_int);

  api.kore_composite_pattern_add_argument(foo, val);
  api.kore_pattern_free(val);

  char *printed = api.kore_pattern_pretty_print(foo);
  printf("%s", printed);
  free(printed);

  api.kore_pattern_free(foo);
}
