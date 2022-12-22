#include "runtime/header.h"

extern "C" {
rangemap hook_RANGEMAP_element(SortKItem keyRangeStart, SortKItem keyRangeEnd, SortKItem value) {
  return rangemap().inserted(rng_map::Range<KElem>(keyRangeStart, keyRangeEnd), value);
}

rangemap hook_RANGEMAP_unit() {
  return rangemap();
}

size_t hook_RANGEMAP_size_long(SortRangeMap m) {
  return m->size();
}

SortInt hook_RANGEMAP_size(SortRangeMap m) {
  auto size = hook_RANGEMAP_size_long(m);
  mpz_t result;
  mpz_init_set_ui(result, size);
  return move_int(result);
}

rangemap hook_RANGEMAP_concat(SortRangeMap m1, SortRangeMap m2) {
  auto mfirst = m1->size() >= m2->size() ? m1 : m2;
  auto msec = m1->size() >= m2->size() ? m2 : m1;
  return mfirst->concat(*msec);
}

}
