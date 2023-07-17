#include "api.h"

#include <assert.h>
#include <stdio.h>

int main(int argc, char **argv) {
  if (argc <= 2) {
    return 1;
  }

  struct kllvm_c_api api = load_c_api(argv[1]);

  kore_pattern *pat = api.kore_composite_pattern_new("Lblfoo");
  kore_sort *bytes = api.kore_composite_sort_new("SortBytes");
  assert(bytes && pat && "Bad sort or pattern");

  char bytes_data[4] = {'\x00', '\x01', '\x00', '\x02'};
  kore_pattern *token
      = api.kore_pattern_new_token_with_len(bytes_data, 4, bytes);
  api.kore_composite_pattern_add_argument(pat, token);

  char *data;
  size_t size;
  api.kore_simplify(pat, bytes, &data, &size);

  FILE *f = fopen(argv[2], "wb");
  if (!f) {
    return 4;
  }

  fwrite(data, size, 1, f);
  fclose(f);
}
