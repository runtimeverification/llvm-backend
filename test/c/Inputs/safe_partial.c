#include "api.h"

#include <assert.h>
#include <stdio.h>

void test_safe_eval(struct kllvm_c_api *api, char const *pattern) {
  kore_pattern *one = api->kore_pattern_parse(pattern);
  kore_sort *sort_int = api->kore_composite_sort_new("SortInt");

  kore_error *err = api->kore_error_new();

  char *data;
  size_t size;
  api->kore_simplify(err, one, sort_int, &data, &size);

  assert(
      !api->kore_error_is_success(err)
      && "Shouldn't be able to evaluate pattern");

  api->kore_pattern_free(one);
  api->kore_sort_free(sort_int);
  api->kore_error_free(err);
}

int main(int argc, char **argv) {
  if (argc <= 1) {
    return 1;
  }

  struct kllvm_c_api api = load_c_api(argv[1]);

  api.kllvm_init();

  test_safe_eval(&api, "Lblfoo{}(\\dv{SortInt{}}(\"1\"))");
  test_safe_eval(&api, "Lblbar{}()");
}
