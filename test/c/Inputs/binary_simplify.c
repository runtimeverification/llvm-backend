#include <assert.h>
#include <dlfcn.h>
#include <kllvm-c/kllvm-c.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef kore_pattern *new_comp_t(char const *);
typedef kore_pattern *new_token_t(char const *, kore_sort const *);
typedef void *add_arg_t(kore_pattern *, kore_pattern const *);
typedef kore_sort *new_sort_t(char const *);
typedef void simplify_t(char *, size_t, kore_sort const *, char **, size_t *);
typedef void free_all_t(void);
typedef void init_t(void);
typedef void pattern_free_t(kore_pattern *);
typedef void sort_free_t(kore_sort *);
typedef void serialize_t(kore_pattern const *, char **, size_t *);

int main(int argc, char **argv) {
  if (argc <= 2) {
    return 1;
  }

  void *lib = dlopen(argv[1], RTLD_NOW);
  if (!lib) {
    return 2;
  }

  new_comp_t *new_comp = dlsym(lib, "kore_composite_pattern_new");
  new_token_t *new_token = dlsym(lib, "kore_pattern_new_token");
  add_arg_t *add_arg = dlsym(lib, "kore_composite_pattern_add_argument");
  new_sort_t *new_sort = dlsym(lib, "kore_composite_sort_new");
  simplify_t *simplify = dlsym(lib, "kore_simplify_binary");
  free_all_t *free_all = dlsym(lib, "kllvm_free_all_memory");
  init_t *init = dlsym(lib, "kllvm_init");
  serialize_t *serialize = dlsym(lib, "kore_pattern_serialize");

  pattern_free_t *pattern_free
      = (pattern_free_t *)dlsym(lib, "kore_pattern_free");
  sort_free_t *sort_free = (sort_free_t *)dlsym(lib, "kore_sort_free");

  if (!new_comp || !new_token || !add_arg || !new_sort || !simplify || !free_all
      || !init || !serialize) {
    return 3;
  }

  init();

  kore_sort *sort_int = new_sort("SortInt");
  kore_sort *sort_foo = new_sort("SortFoo");

  /*
    module TEST
      imports INT

      syntax Foo ::= bar(Int)      [klabel(bar), symbol]
                   | foo(Int, Int) [function, klabel(foo), symbol]

      rule foo(A, 0) => bar(A)
      rule foo(A, B) => foo(A +Int 1, absInt(B) -Int 1) [owise]
    endmodule
  */
  kore_pattern *pat = new_comp("Lblfoo");
  assert(sort_int && pat && "Bad sort or pattern");

  kore_pattern *a = new_token("12", sort_int);
  add_arg(pat, a);
  pattern_free(a);

  kore_pattern *b = new_token("5", sort_int);
  add_arg(pat, b);
  pattern_free(b);

  char *data_in, *data_out;
  size_t size_in, size_out;

  serialize(pat, &data_in, &size_in);
  simplify(data_in, size_in, sort_foo, &data_out, &size_out);

  FILE *f = fopen(argv[2], "wb");
  if (!f) {
    return 4;
  }

  fwrite(data_out, size_out, 1, f);
  fclose(f);

  pattern_free(pat);
  sort_free(sort_int);
  sort_free(sort_foo);
}
