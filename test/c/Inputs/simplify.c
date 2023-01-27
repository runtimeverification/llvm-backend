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
typedef void free_all_t(void);
typedef void init_t(void);
typedef void pattern_free_t(kore_pattern *);
typedef void sort_free_t(kore_sort *);

int main(int argc, char **argv) {
  if (argc <= 3) {
    return 1;
  }

  void *lib = dlopen(argv[1], RTLD_NOW);
  if (!lib) {
    return 2;
  }

  new_comp_t *new_comp = (new_comp_t *)dlsym(lib, "kore_composite_pattern_new");
  new_sort_t *new_sort = (new_sort_t *)dlsym(lib, "kore_composite_sort_new");
  simplify_t *simplify = (simplify_t *)dlsym(lib, "kore_simplify");
  free_all_t *free_all = (free_all_t *)dlsym(lib, "kllvm_free_all_memory");
  init_t *init = (init_t *)dlsym(lib, "kllvm_init");

  pattern_free_t *pattern_free
      = (pattern_free_t *)dlsym(lib, "kore_pattern_free");
  sort_free_t *sort_free = (sort_free_t *)dlsym(lib, "kore_sort_free");

  if (!new_comp || !new_sort || !simplify || !free_all || !init) {
    return 3;
  }

  init();

  kore_sort *sort = new_sort("SortInt");

  /*
    module TEST
      imports INT

      syntax Int ::= foo() [function, klabel(foo), symbol]
                   | bar() [function, klabel(bar), symbol]

      rule foo() => 328
      rule bar() => 562
    endmodule
  */
  kore_pattern *pat = new_comp(argv[3]);
  assert(sort && pat && "Bad sort or pattern");

  char *data;
  size_t size;
  simplify(pat, sort, &data, &size);

  // Do the simplification twice to make sure GC works
  free_all();

  // Workaround while patterns can't be reused
  kore_pattern *pat_2 = new_comp(argv[3]);
  simplify(pat_2, sort, &data, &size);

  FILE *f = fopen(argv[2], "wb");
  if (!f) {
    return 4;
  }

  fwrite(data, size, 1, f);
  fclose(f);

  pattern_free(pat);
  pattern_free(pat_2);
  sort_free(sort);
}
