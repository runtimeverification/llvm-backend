#ifndef KLLVM_C_H
#define KLLVM_C_H

extern "C" {

typedef struct kore_composite_pattern kore_composite_pattern;

kore_composite_pattern *kore_composite_pattern_new(char const *);
void kore_composite_pattern_dump(kore_composite_pattern const *);
void kore_composite_pattern_add_argument(
    kore_composite_pattern *, kore_composite_pattern *);
void kore_composite_pattern_free(kore_composite_pattern const *);
}

#endif
