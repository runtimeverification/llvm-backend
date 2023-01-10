#include <assert.h>
#include <dlfcn.h>
#include <kllvm-c/kllvm-c.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef kore_pattern *new_comp_t(char const *);
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
  new_sort_t *new_sort = (new_sort_t *)dlsym(lib, "kore_composite_sort_new");
  simplify_t *simplify = (simplify_t *)dlsym(lib, "kore_simplify");

  if (!new_comp || !new_sort || !simplify) {
    return 3;
  }

  kore_sort *sort = new_sort("SortKItem");

  /*
    module TEST
      imports INT

      syntax KItem ::= foo() [function, klabel(foo), symbol]
                     | bar() [klabel(bar), symbol]

      rule foo() => bar()
    endmodule
  */
  kore_pattern *pat = new_comp("Lblfoo");
  assert(sort && pat && "Bad sort or pattern");

  char *data;
  size_t size;
  simplify(pat, sort, &data, &size);

  FILE *f = fopen(argv[2], "wb");
  if (!f) {
    return 4;
  }

  fwrite(data, size, 1, f);
  fclose(f);
}
