#include "api.h"

#include <stdio.h>
#include <stdlib.h>

#define N_ARGS 3

kore_pattern *make_list_item(struct kllvm_c_api *api, char const *arg) {
  kore_sort *sort_int = api->kore_composite_sort_new("SortInt");
  kore_sort *sort_kitem = api->kore_composite_sort_new("SortKItem");

  kore_pattern *list_item = api->kore_composite_pattern_new("LblListItem");
  kore_pattern *token = api->kore_pattern_new_token(arg, sort_int);
  kore_pattern *inj
      = api->kore_pattern_new_injection(token, sort_int, sort_kitem);
  api->kore_composite_pattern_add_argument(list_item, inj);

  api->kore_pattern_free(token);
  api->kore_pattern_free(inj);
  api->kore_sort_free(sort_int);
  api->kore_sort_free(sort_kitem);

  return list_item;
}

int main(int argc, char **argv) {
  if (argc <= 1) {
    return 1;
  }

  char const *args[N_ARGS] = {"78", "12", "45"};

  struct kllvm_c_api api = load_c_api(argv[1]);

  api.kllvm_init();

  kore_sort *sort_foo = api.kore_composite_sort_new("SortFoo");

  kore_pattern *run = api.kore_composite_pattern_new("Lblrun");

  kore_pattern *concat = api.kore_composite_pattern_new("Lbl'Unds'List'Unds'");
  kore_pattern *assoc = api.kore_composite_pattern_new("\\left-assoc");

  for (int i = 0; i < N_ARGS; ++i) {
    kore_pattern *list_item = make_list_item(&api, args[i]);
    api.kore_composite_pattern_add_argument(concat, list_item);
    api.kore_pattern_free(list_item);
  }

  api.kore_composite_pattern_add_argument(assoc, concat);
  api.kore_composite_pattern_add_argument(run, assoc);

  kore_pattern *input = api.kore_pattern_make_interpreter_input(run, sort_foo);
  kore_pattern *desugared = api.kore_pattern_desugar_associative(input);

  block *term = api.kore_pattern_construct(desugared);
  block *after = api.take_steps(-1, term);

  char *output = api.kore_block_dump(after);
  printf("%s", output);
  free(output);

  api.kore_pattern_free(run);
  api.kore_pattern_free(concat);
  api.kore_pattern_free(assoc);
  api.kore_pattern_free(input);
  api.kore_pattern_free(desugared);
  api.kore_sort_free(sort_foo);
}
