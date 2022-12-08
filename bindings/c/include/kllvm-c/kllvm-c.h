#ifndef KLLVM_C_H
#define KLLVM_C_H

#ifdef __cplusplus
extern "C" {
#endif

/* KOREPattern */

typedef struct kore_pattern kore_pattern;

char *kore_pattern_dump(kore_pattern const *);

void kore_pattern_free(kore_pattern const *);

kore_pattern *kore_composite_pattern_new(char const *);
void kore_composite_pattern_add_argument(kore_pattern *, kore_pattern *);

kore_pattern *kore_string_pattern_new(char const *);

/* KORESort */

typedef struct kore_sort kore_sort;

char *kore_sort_dump(kore_sort const *);

void kore_sort_free(kore_sort const *);

bool kore_sort_is_concrete(kore_sort const *);

kore_sort *kore_composite_sort_new(char const *);
void kore_composite_sort_add_argument(kore_sort *, kore_sort *);

#ifdef __cplusplus
}
#endif

#endif
