#include "api.h"

#include <stdio.h>

/*
  module TEST
    syntax Foo ::= foo() | bar()
    rule foo() => bar()
  endmodule
*/

int main(int argc, char **argv) {
  if (argc <= 1) {
    return 1;
  }

  struct kllvm_c_api api = load_c_api(argv[1]);

  kore_pattern *pat = api.kore_pattern_parse("Lblfoo{}()");
  printf("%s", api.kore_pattern_dump(pat));
}
