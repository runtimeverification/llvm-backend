#include <assert.h>
#include <dlfcn.h>
#include <kllvm-c/kllvm-c.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef kore_pattern *new_comp_t(char const *);
typedef bool simplify_t(kore_pattern *);

#define ASSERT_SIMPLIFY(c, v)                                                  \
  do {                                                                         \
    int len = strlen((c)) + 4;                                                 \
    char *buf = (char *)malloc(len);                                           \
    snprintf(buf, len, "Lbl%s", (c));                                          \
    assert(simplify(new_comp(buf)) == (v) && c "failed!");                     \
    free(buf);                                                                 \
  } while (false);

int main(int argc, char **argv) {
  if (argc <= 1) {
    return 1;
  }

  void *lib = dlopen(argv[1], RTLD_NOW);
  if (!lib) {
    return 2;
  }

  new_comp_t *new_comp = (new_comp_t *)dlsym(lib, "kore_composite_pattern_new");
  simplify_t *simplify = (simplify_t *)dlsym(lib, "kore_simplify_bool");

  if (!new_comp || !simplify) {
    return 3;
  }

  /*
    module TEST
      imports BOOL

      syntax Bool ::= foo() [function, klabel(foo), symbol]
                    | bar() [function, klabel(bar), symbol]
                    | baz() [function, klabel(baz), symbol]

      rule foo() => true
      rule bar() => false
      rule baz() => foo() andBool bar()
    endmodule
  */

  ASSERT_SIMPLIFY("foo", true);
  ASSERT_SIMPLIFY("bar", false);
  ASSERT_SIMPLIFY("baz", false);
}
