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

  kore_pattern *good_call = api.kore_pattern_parse(
      "Lblfoo{}(\\dv{SortInt{}}(\"2\"), \\dv{SortInt{}}(\"2\"))");

  kore_pattern *bad_call = api.kore_pattern_parse(
      "Lblfoo{}(\\dv{SortInt{}}(\"2\"), \\dv{SortInt{}}(\"0\"))");

  kore_error *err = api.kore_error_new();

  char *data;
  size_t size;

  api.kore_simplify(err, good_call, sort_int, &data, &size);
  assert(api.kore_error_is_success(err));

  FILE *f = fopen(argv[2], "wb");
  if (!f) {
    return 4;
  }

  fwrite(data, size, 1, f);
  fclose(f);

  api.kore_simplify(err, bad_call, sort_int, &data, &size);
  assert(!api.kore_error_is_success(err));

  printf("%s\n", api.kore_error_message(err));

  api.kore_error_free(err);
  api.kore_pattern_free(good_call);
  api.kore_pattern_free(bad_call);
  api.kore_sort_free(sort_int);
}
