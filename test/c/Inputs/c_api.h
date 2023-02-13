#ifndef KORE_C_API_H
#define KORE_C_API_H

#include <kllvm-c/kllvm-c.h>

struct c_api {
  void (*init)(void);
  void (*gc_free_all)(void);
  kore_pattern *(*composite_pattern_new)(char const *);
  kore_sort *(*composite_sort_new)(char const *);
  void (*simplify)(kore_pattern *, kore_sort *, char **, size_t *);
  void (*pattern_free)(kore_pattern *);
  void (*sort_free)(kore_sort *);
};

struct c_api load_c_api(char const *);

#endif
