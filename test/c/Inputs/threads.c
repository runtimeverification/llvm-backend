#include <assert.h>
#include <kllvm-c/kllvm-c.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "c_api.h"

/*
  module TEST
    imports INT

    syntax Int ::= foo() [function, klabel(foo), symbol]
                 | bar() [function, klabel(bar), symbol]

    rule foo() => 328
    rule bar() => 562
  endmodule
*/

static const int N = 500;

static pthread_t thread_a, thread_b;
static char *data_out_a, *data_out_b;
static size_t size_out_a, size_out_b;

void *work_a(void *api_ptr) {
  struct c_api api = *(struct c_api *)api_ptr;

  kore_sort *sort = api.composite_sort_new("SortInt");
  kore_pattern *pat = api.composite_pattern_new("Lblfoo");
  assert(sort && pat && "Bad sort or pattern");

  for (int i = 0; i < N; ++i) {
    api.simplify(pat, sort, &data_out_a, &size_out_a);
    /* api.gc_free_all(); */
  }

  api.pattern_free(pat);
  api.sort_free(sort);

  return NULL;
}

void *work_b(void *api_ptr) {
  struct c_api api = *(struct c_api *)api_ptr;

  kore_sort *sort = api.composite_sort_new("SortInt");
  kore_pattern *pat = api.composite_pattern_new("Lblbar");
  assert(sort && pat && "Bad sort or pattern");

  for (int i = 0; i < N; ++i) {
    api.simplify(pat, sort, &data_out_b, &size_out_b);
    /* api.gc_free_all(); */
  }

  api.pattern_free(pat);
  api.sort_free(sort);

  return NULL;
}

static void do_output(char const *path, char *data, size_t size) {
  FILE *f = fopen(path, "wb");
  assert(f && "Couldn't open file");

  fwrite(data, size, 1, f);
  fclose(f);
}

int main(int argc, char **argv) {
  if (argc <= 3) {
    return 1;
  }

  struct c_api api = load_c_api(argv[1]);

  api.init();

  pthread_create(&thread_a, NULL, work_a, (void *)&api);
  pthread_create(&thread_b, NULL, work_b, (void *)&api);

  pthread_join(thread_a, NULL);
  pthread_join(thread_b, NULL);

  do_output(argv[2], data_out_a, size_out_a);
  do_output(argv[3], data_out_b, size_out_b);
}
