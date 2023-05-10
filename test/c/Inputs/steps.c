#include "api.h"

#include <stdio.h>

/*
  module ARITHMETIC-SYNTAX
    imports UNSIGNED-INT-SYNTAX

    syntax Exp ::= Int
                 | Exp "+" Exp [left, strict]
  endmodule

  module ARITHMETIC
    imports ARITHMETIC-SYNTAX
    imports DOMAINS

    configuration
      <k> $PGM:Exp </k>

    rule A + B => A +Int B

    syntax Bool ::= isKResult(Exp) [function, symbol]
    rule isKResult(_::Int) => true
    rule isKResult(_)      => false [owise]
  endmodule
*/

int main(int argc, char **argv) {
  if (argc <= 1) {
    return 1;
  }

  struct kllvm_c_api api = load_c_api(argv[1]);

  api.kllvm_init();

  kore_pattern *pat = api.kore_pattern_parse(
      "Lbl'UndsPlusUndsUnds'ARITHMETIC-SYNTAX'Unds'Exp'Unds'Exp'Unds'Exp{}("
      "inj{SortInt{}, SortExp{}}(\\dv{SortInt{}}(\"75\")),inj{SortInt{}, "
      "SortExp{}}(\\dv{SortInt{}}(\"12\")))");

  kore_sort *sort_exp = api.kore_composite_sort_new("SortExp");
  kore_pattern *input = api.kore_pattern_make_interpreter_input(pat, sort_exp);

  block *term = api.kore_pattern_construct(input);
  block *after = api.take_steps(-1, term);

  printf("%s", api.kore_block_dump(after));
}
