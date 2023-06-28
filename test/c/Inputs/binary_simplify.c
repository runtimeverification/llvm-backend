#include "api.h"

#include <assert.h>
#include <stdio.h>

int main(int argc, char **argv) {
  if (argc <= 2) {
    return 1;
  }

  struct kllvm_c_api api = load_c_api(argv[1]);

  api.kllvm_init();

  kore_sort *sort_int = api.kore_composite_sort_new("SortInt");
  kore_sort *sort_foo = api.kore_composite_sort_new("SortFoo");

  kore_pattern *pat = api.kore_composite_pattern_new("Lblfoo");
  assert(sort_int && pat && "Bad sort or pattern");

  kore_pattern *a = api.kore_pattern_new_token("12", sort_int);
  api.kore_composite_pattern_add_argument(pat, a);
  api.kore_pattern_free(a);

  kore_pattern *b = api.kore_pattern_new_token("5", sort_int);
  api.kore_composite_pattern_add_argument(pat, b);
  api.kore_pattern_free(b);

  char *data_in, *data_out;
  size_t size_in, size_out;

  api.kore_pattern_serialize(pat, &data_in, &size_in);
  api.kore_simplify_binary(data_in, size_in, sort_foo, &data_out, &size_out);

  FILE *f = fopen(argv[2], "wb");
  if (!f) {
    return 4;
  }

  fwrite(data_out, size_out, 1, f);
  fclose(f);

  api.kore_pattern_free(pat);
  api.kore_sort_free(sort_int);
  api.kore_sort_free(sort_foo);
}
