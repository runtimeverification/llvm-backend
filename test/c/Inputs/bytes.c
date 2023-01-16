#include <assert.h>
#include <dlfcn.h>
#include <kllvm-c/kllvm-c.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef kore_pattern *new_comp_t(char const *);
typedef kore_pattern *new_token_len_t(char const *, size_t, kore_sort const *);
typedef void *add_pattern_arg_t(kore_pattern *, kore_pattern *);
typedef kore_sort *new_sort_t(char const *);
typedef void simplify_t(kore_pattern *, kore_sort *, char **, size_t *);

int main(int argc, char **argv) {
  if (argc <= 2) {
    return 1;
  }

  void *lib = dlopen(argv[1], RTLD_NOW);
  if (!lib) {
    return 2;
  }

  new_comp_t *new_comp = (new_comp_t *)dlsym(lib, "kore_composite_pattern_new");
  new_token_len_t *new_token_len
      = (new_token_len_t *)dlsym(lib, "kore_pattern_new_token_with_len");
  add_pattern_arg_t *add_arg
      = (add_pattern_arg_t *)dlsym(lib, "kore_composite_pattern_add_argument");
  new_sort_t *new_sort = (new_sort_t *)dlsym(lib, "kore_composite_sort_new");
  simplify_t *simplify = (simplify_t *)dlsym(lib, "kore_simplify");

  if (!new_comp || !new_token_len || !new_sort || !simplify || !add_arg) {
    return 3;
  }

  /*
    module TEST
        imports BYTES

        syntax Bytes ::= foo(Bytes) [function, klabel(foo), symbol]
        rule foo(B) => B
    endmodule
  */
  kore_pattern *pat = new_comp("Lblfoo");
  kore_sort *bytes = new_sort("SortBytes");
  assert(bytes && pat && "Bad sort or pattern");

  char bytes_data[4] = {'\x00', '\x01', '\x00', '\x02'};
  kore_pattern *token = new_token_len(bytes_data, 4, bytes);
  add_arg(pat, token);

  char *data;
  size_t size;
  simplify(pat, bytes, &data, &size);

  FILE *f = fopen(argv[2], "wb");
  if (!f) {
    return 4;
  }

  fwrite(data, size, 1, f);
  fclose(f);
}
