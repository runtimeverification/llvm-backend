#include <assert.h>
#include <dlfcn.h>
#include <kllvm-c/kllvm-c.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef void init_t(void);
typedef kore_pattern *parse_t(char const *);
typedef char *dump_t(kore_pattern const *);
typedef char *block_dump_t(block *);
typedef kore_sort *new_sort_t(char const *);
typedef kore_pattern *make_input_t(kore_pattern const *, kore_sort const *);
typedef block *construct_t(kore_pattern const *);
typedef block *step_t(int64_t, block *);

int main(int argc, char **argv) {
  if (argc <= 1) {
    return 1;
  }

  void *lib = dlopen(argv[1], RTLD_NOW);
  if (!lib) {
    return 2;
  }

  init_t *init = (init_t *)dlsym(lib, "kllvm_init");
  parse_t *parse = (parse_t *)dlsym(lib, "kore_pattern_parse");
  dump_t *dump = (dump_t *)dlsym(lib, "kore_pattern_dump");
  block_dump_t *block_dump = (block_dump_t *)dlsym(lib, "kore_block_dump");
  new_sort_t *new_sort = (new_sort_t *)dlsym(lib, "kore_composite_sort_new");
  make_input_t *make_input
      = (make_input_t *)dlsym(lib, "kore_pattern_make_interpreter_input");
  construct_t *construct = (construct_t *)dlsym(lib, "kore_pattern_construct");
  step_t *steps = (step_t *)dlsym(lib, "take_steps");

  if (!init || !parse || !dump || !block_dump || !new_sort || !make_input
      || !construct || !steps) {
    return 3;
  }

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

  init();

  kore_pattern *pat = parse(
      "Lbl'UndsPlusUndsUnds'ARITHMETIC-SYNTAX'Unds'Exp'Unds'Exp'Unds'Exp{}("
      "inj{SortInt{}, SortExp{}}(\\dv{SortInt{}}(\"75\")),inj{SortInt{}, "
      "SortExp{}}(\\dv{SortInt{}}(\"12\")))");

  kore_sort *sort_exp = new_sort("SortExp");
  kore_pattern *input = make_input(pat, sort_exp);

  block *term = construct(input);
  block *after = steps(-1, term);

  printf("%s", block_dump(after));
}
