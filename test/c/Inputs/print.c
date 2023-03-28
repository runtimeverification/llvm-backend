#include "api.h"

#include <stdio.h>

/*
  module TEST
    imports INT

    syntax Foo ::= "#foo" "[" Int "]" [format(%1 %2%n%i%3%n%d%4), klabel(foo), symbol]
  endmodule
*/

int main(int argc, char **argv) {
  if (argc <= 1) {
    return 1;
  }

  struct kllvm_c_api api = load_c_api(argv[1]);

  api.kllvm_init();

  kore_sort *sort_int = api.kore_composite_sort_new("SortInt");
  kore_sort *sort_foo = api.kore_composite_sort_new("SortFoo");

  kore_pattern *foo = api.kore_composite_pattern_new("Lblfoo");
  kore_pattern *val = api.kore_pattern_new_token("423", sort_int);

  api.kore_composite_pattern_add_argument(foo, val);

  printf("%s\n", api.kore_pattern_pretty_print(foo));

  /* char *data; */
  /* size_t size; */
  /* api.kore_simplify(pat, sort, &data, &size); */

  /* // Do the simplification twice to make sure GC works */
  /* api.kllvm_free_all_memory(); */

  /* api.kore_simplify(pat, sort, &data, &size); */

  /* FILE *f = fopen(argv[2], "wb"); */
  /* if (!f) { */
  /*   return 4; */
  /* } */

  /* fwrite(data, size, 1, f); */
  /* fclose(f); */

  /* api.kore_pattern_free(pat); */
  /* api.kore_sort_free(sort); */
}
