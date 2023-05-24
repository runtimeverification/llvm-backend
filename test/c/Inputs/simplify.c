#include "api.h"

#include <assert.h>
#include <stdio.h>

int main(int argc, char **argv) {
  if (argc <= 3) {
    return 1;
  }

  struct kllvm_c_api api = load_c_api(argv[1]);

  api.kllvm_init();

  kore_sort *sort = api.kore_composite_sort_new("SortInt");
  kore_pattern *pat = api.kore_composite_pattern_new(argv[3]);
  assert(sort && pat && "Bad sort or pattern");

  char *data;
  size_t size;
  api.kore_simplify(pat, sort, &data, &size);

  // Do the simplification twice to make sure GC works
  api.kllvm_free_all_memory();

  api.kore_simplify(pat, sort, &data, &size);

  FILE *f = fopen(argv[2], "wb");
  if (!f) {
    return 4;
  }

  fwrite(data, size, 1, f);
  fclose(f);

  api.kore_pattern_free(pat);
  api.kore_sort_free(sort);
}
