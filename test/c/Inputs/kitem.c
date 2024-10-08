#include "api.h"

#include <assert.h>
#include <stdio.h>

int main(int argc, char **argv) {
  if (argc <= 2) {
    return 1;
  }

  struct kllvm_c_api api = load_c_api(argv[1]);

  kore_sort *sort = api.kore_composite_sort_new("SortKItem");

  kore_pattern *pat = api.kore_composite_pattern_new("Lblfoo");
  assert(sort && pat && "Bad sort or pattern");

  char *data;
  size_t size;
  api.kore_simplify(NULL, pat, sort, &data, &size);

  FILE *f = fopen(argv[2], "wb");
  if (!f) {
    return 4;
  }

  fwrite(data, size, 1, f);
  fclose(f);
}
