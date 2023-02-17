#include "runtime/header.h"

extern "C" {
rangemap hook_RANGEMAP_element(
    SortKItem keyRangeStart, SortKItem keyRangeEnd, SortKItem value) {
  return rangemap().inserted(
      rng_map::Range<KElem>(keyRangeStart, keyRangeEnd), value);
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

SortKItem hook_RANGEMAP_lookup_null(SortRangeMap m, SortKItem key) {
  auto val = m->get_value(key);
  if (val.has_value()) {
    return val.value();
  }
  return nullptr;
}

SortKItem hook_RANGEMAP_lookup(SortRangeMap m, SortKItem key) {
  auto res = hook_RANGEMAP_lookup_null(m, key);
  if (!res) {
    KLLVM_HOOK_INVALID_ARGUMENT("Key not found for map lookup");
  }
  return res;
}

SortKItem hook_RANGEMAP_lookupOrDefault(
    SortRangeMap m, SortKItem key, SortKItem _default) {
  auto res = hook_RANGEMAP_lookup_null(m, key);
  if (!res) {
    return _default;
  }
  return res;
}

struct range {
  blockheader h;
  block *start;
  block *end;
};
static struct blockheader range_header() {
  static struct blockheader hdr = {(uint64_t)-1};
  if (hdr.hdr == -1) {
    hdr = getBlockHeaderForSymbol(
        (uint64_t)getTagForSymbolName("range{SortKItem{}, SortKItem{}}"));
  }
  return hdr;
}
SortRange hook_RANGEMAP_find_range(SortRangeMap m, SortKItem key) {
  auto val = m->get_key_value(key);
  if (val.has_value()) {
    range *ptr = (range *)koreAlloc(sizeof(range));
    ptr->h = range_header();
    ptr->start = val.value().first.start();
    ptr->end = val.value().first.end();
    return (SortRange)ptr;
  } else {
    KLLVM_HOOK_INVALID_ARGUMENT("Key not found for map lookup");
  }
}

rangemap hook_RANGEMAP_update(
    SortRangeMap m, SortKItem keyRangeStart, SortKItem keyRangeEnd,
    SortKItem value) {
  return m->inserted(rng_map::Range<KElem>(keyRangeStart, keyRangeEnd), value);
}

rangemap hook_RANGEMAP_remove(
    SortRangeMap m, SortKItem keyRangeStart, SortKItem keyRangeEnd) {
  return m->deleted(rng_map::Range<KElem>(keyRangeStart, keyRangeEnd));
}

rangemap hook_RANGEMAP_difference(SortRangeMap m1, SortRangeMap m2) {
  return m1->difference(*m2);
}

bool hook_RANGEMAP_in_keys(SortKItem key, SortRangeMap m) {
  return m->contains(key);
}

bool hook_RANGEMAP_inclusion(SortRangeMap m1, SortRangeMap m2) {
  return m1->inclusion(*m2);
}

bool hook_RANGEMAP_eq(SortRangeMap m1, SortRangeMap m2) {
  rng_map::RangeMapIterator<KElem, KElem> it1
      = rng_map::RangeMapIterator<KElem, KElem>(*m1);
  rng_map::RangeMapIterator<KElem, KElem> it2
      = rng_map::RangeMapIterator<KElem, KElem>(*m2);
  for (; it1.has_next() && it2.has_next(); ++it1, ++it2) {
    std::pair<rng_map::Range<KElem>, KElem> const &r1 = *it1;
    std::pair<rng_map::Range<KElem>, KElem> const &r2 = *it2;
    if (r1 != r2) {
      return false;
    }
  }
  if (it1.has_next() || it2.has_next()) {
    return false;
  }
  return true;
}
}
