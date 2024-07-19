#include "api.h"

#include <stdio.h>
#include <stdlib.h>

/**
 * The K program corresponding to this test will evaluate the `Int2Bytes` hooked
 * function when it is run; this hook ends up allocating and freeing a temporary
 * MPZ integer locally. If we're in a situation where `-flat_namespace` has been
 * enabled on macOS, it's possible for the hook to end up resolving
 * `__gmpz_clear` to a symbol defined in the host binary (rather than libgmp's
 * dynamic library!).
 *
 * This originally manifested when the HB booster loaded a C bindings library
 * (the booster statically links libgmp and so contains a symbol with this
 * name); this test is a minimised reproduction of that issue.
 */
void __gmpz_clear(void *p) {
  abort();
}

int main(int argc, char **argv) {
  if (argc <= 1) {
    return 1;
  }

  struct kllvm_c_api api = load_c_api(argv[1]);

  api.kllvm_init();

  kore_sort *sort_foo = api.kore_composite_sort_new("SortFoo");
  kore_pattern *pat = api.kore_composite_pattern_new("Lblfoo");

  kore_pattern *input = api.kore_pattern_make_interpreter_input(pat, sort_foo);

  block *term = api.kore_pattern_construct(input);
  block *after = api.take_steps(-1, term);

  printf("%s", api.kore_block_dump(after));
}
