#ifndef KLLVM_C_H
#define KLLVM_C_H

#ifdef __cplusplus
extern "C" {
#endif

/* Opaque types */

typedef struct kore_pattern kore_pattern;
typedef struct kore_sort kore_sort;
typedef struct kore_symbol kore_symbol;
typedef struct block block;

/* KOREPattern */

char *kore_pattern_dump(kore_pattern const *);

void kore_pattern_free(kore_pattern const *);

kore_pattern *kore_pattern_new_token(char const *, kore_sort const *);
kore_pattern *kore_pattern_new_injection(
    kore_pattern *, kore_sort const *, kore_sort const *);
kore_pattern *kore_pattern_make_interpreter_input(kore_pattern *);

kore_pattern *kore_composite_pattern_new(char const *);
kore_pattern *kore_composite_pattern_from_symbol(kore_symbol *);
void kore_composite_pattern_add_argument(kore_pattern *, kore_pattern *);

kore_pattern *kore_string_pattern_new(char const *);

block *kore_pattern_construct(kore_pattern const *);

/* KORESort */

char *kore_sort_dump(kore_sort const *);

void kore_sort_free(kore_sort const *);

bool kore_sort_is_concrete(kore_sort const *);

kore_sort *kore_composite_sort_new(char const *);
void kore_composite_sort_add_argument(kore_sort *, kore_sort *);

/* KORESymbol */

kore_symbol *kore_symbol_new(char const *);

void kore_symbol_free(kore_symbol const *);

void kore_symbol_add_formal_argument(kore_symbol *, kore_sort const *);

#ifdef __cplusplus
}
#endif

#endif
