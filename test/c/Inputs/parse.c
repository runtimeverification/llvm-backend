#include <assert.h>
#include <dlfcn.h>
#include <kllvm-c/kllvm-c.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef kore_pattern *parse_t(char const *);
typedef char *dump_t(kore_pattern const *);

int main(int argc, char **argv) {
  if (argc <= 1) {
    return 1;
  }

  void *lib = dlopen(argv[1], RTLD_NOW);
  if (!lib) {
    return 2;
  }

  parse_t *parse = (parse_t *)dlsym(lib, "kore_pattern_parse");
  dump_t *dump = (dump_t *)dlsym(lib, "kore_pattern_dump");

  if (!parse || !dump) {
    return 3;
  }

  /*
    module TEST
      syntax Foo ::= foo() | bar()
      rule foo() => bar()
    endmodule
  */

  kore_pattern *pat = parse("Lblfoo{}()");
  printf("%s", dump(pat));
}
