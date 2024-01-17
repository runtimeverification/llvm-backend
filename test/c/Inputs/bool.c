#include "api.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ASSERT_SIMPLIFY(c, v)                                                  \
  do {                                                                         \
    int len = strlen((c)) + 4;                                                 \
    char *buf = (char *)malloc(len);                                           \
    snprintf(buf, len, "Lbl%s", (c));                                          \
    kore_pattern *pat = api.kore_composite_pattern_new(buf);                   \
    kore_error *err = api.kore_error_new();                                    \
    bool result = api.kore_simplify_bool(err, pat);                            \
    assert(api.kore_error_is_success(err));                                    \
    assert(result == (v) && c "failed!");                                      \
    api.kore_pattern_free(pat);                                                \
    api.kore_error_free(err);                                                  \
    free(buf);                                                                 \
  } while (false);

int main(int argc, char **argv) {
  if (argc <= 1) {
    return 1;
  }

  struct kllvm_c_api api = load_c_api(argv[1]);

  ASSERT_SIMPLIFY("foo", true);
  ASSERT_SIMPLIFY("bar", false);
  ASSERT_SIMPLIFY("baz", false);
}
