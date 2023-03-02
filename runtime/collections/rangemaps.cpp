#include "runtime/header.h"

#include "immer/flex_vector_transient.hpp"

extern "C" {
rangemap
hook_RANGEMAP_element(SortKItem start, SortKItem end, SortKItem value) {
  return rangemap().inserted(rng_map::Range<KElem>(start, end), value);
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
rangemap hook_RANGEMAP_elementRng(SortKItem rng, SortKItem value) {
  range *ptr = (range *)rng;
  return hook_RANGEMAP_element(ptr->start, ptr->end, value);
}

rangemap hook_RANGEMAP_unit() {
  return rangemap();
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
    SortRangeMap m, SortKItem start, SortKItem end, SortKItem value) {
  return m->inserted(rng_map::Range<KElem>(start, end), value);
}

rangemap
hook_RANGEMAP_updateRng(SortRangeMap m, SortKItem rng, SortKItem value) {
  range *ptr = (range *)rng;
  return hook_RANGEMAP_update(m, ptr->start, ptr->end, value);
}

rangemap hook_RANGEMAP_remove(SortRangeMap m, SortKItem start, SortKItem end) {
  return m->deleted(rng_map::Range<KElem>(start, end));
}

rangemap hook_RANGEMAP_removeRng(SortRangeMap m, SortKItem rng) {
  range *ptr = (range *)rng;
  return hook_RANGEMAP_remove(m, ptr->start, ptr->end);
}

rangemap hook_RANGEMAP_difference(SortRangeMap m1, SortRangeMap m2) {
  return m1->difference(*m2);
}

set hook_SET_unit(void);
set hook_SET_element(block *);
set hook_SET_concat(set *, set *);

set hook_RANGEMAP_keys(SortRangeMap m) {
  auto tmp = hook_SET_unit();
  for (auto iter = rng_map::ConstRangeMapIterator<KElem, KElem>(*m);
       iter.has_next(); ++iter) {
    range *ptr = (range *)koreAlloc(sizeof(range));
    ptr->h = range_header();
    ptr->start = iter->first.start();
    ptr->end = iter->first.end();
    auto elem = hook_SET_element((SortRange)ptr);
    tmp = hook_SET_concat(&tmp, &elem);
  }
  return tmp;
}

list hook_RANGEMAP_keys_list(SortRangeMap m) {
  auto tmp = list().transient();
  for (auto iter = rng_map::ConstRangeMapIterator<KElem, KElem>(*m);
       iter.has_next(); ++iter) {
    range *ptr = (range *)koreAlloc(sizeof(range));
    ptr->h = range_header();
    ptr->start = iter->first.start();
    ptr->end = iter->first.end();
    tmp.push_back((SortRange)ptr);
  }
  return tmp.persistent();
}

bool hook_RANGEMAP_in_keys(SortKItem key, SortRangeMap m) {
  return m->contains(key);
}

list hook_RANGEMAP_values(SortRangeMap m) {
  auto tmp = list().transient();
  for (auto iter = rng_map::ConstRangeMapIterator<KElem, KElem>(*m);
       iter.has_next(); ++iter) {
    tmp.push_back(iter->second);
  }
  return tmp.persistent();
}

SortKItem hook_RANGEMAP_choice(SortRangeMap m) {
  if (m->empty()) {
    KLLVM_HOOK_INVALID_ARGUMENT("Cannot choose from an empty map");
  }
  auto iter = rng_map::ConstRangeMapIterator<KElem, KElem>(*m);
  block *elem = iter->first.start();
  return (SortKItem)elem;
}

SortKItem hook_RANGEMAP_choiceRng(SortRangeMap m) {
  if (m->empty()) {
    KLLVM_HOOK_INVALID_ARGUMENT("Cannot choose from an empty map");
  }
  auto iter = rng_map::ConstRangeMapIterator<KElem, KElem>(*m);
  range *ptr = (range *)koreAlloc(sizeof(range));
  ptr->h = range_header();
  ptr->start = iter->first.start();
  ptr->end = iter->first.end();
  return (SortRange)ptr;
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

bool hook_RANGEMAP_inclusion(SortRangeMap m1, SortRangeMap m2) {
  return m1->inclusion(*m2);
}

rangemap hook_RANGEMAP_updateAll(SortRangeMap m1, SortRangeMap m2) {
  auto from = m2;
  auto to = *m1;
  for (auto iter = rng_map::ConstRangeMapIterator<KElem, KElem>(*from);
       iter.has_next(); ++iter) {
    to = to.inserted(iter->first, iter->second);
  }
  return to;
}

rangemap hook_RANGEMAP_removeAll(SortRangeMap map, SortSet set) {
  auto tmp = *map;
  for (auto iter = set->begin(); iter != set->end(); ++iter) {
    range *ptr = (range *)iter->elem;
    tmp = tmp.deleted(rng_map::Range<KElem>(ptr->start, ptr->end));
  }
  return tmp;
}

bool hook_RANGEMAP_eq(SortRangeMap m1, SortRangeMap m2) {
  auto it1 = rng_map::ConstRangeMapIterator<KElem, KElem>(*m1);
  auto it2 = rng_map::ConstRangeMapIterator<KElem, KElem>(*m2);
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

void rangemap_hash(rangemap *m, void *hasher) {
  if (hash_enter()) {
    for (auto iter = rng_map::ConstRangeMapIterator<KElem, KElem>(*m);
         iter.has_next(); ++iter) {
      auto entry = *iter;
      k_hash(entry.first.start(), hasher);
      k_hash(entry.first.end(), hasher);
      k_hash(entry.second, hasher);
    }
  }
  hash_exit();
}

void rangemap_foreach(rangemap *map, void(process)(block **)) {
  for (auto iter = rng_map::RangeMapIterator<KElem, KElem>(*map);
       iter.has_next(); ++iter) {
    process((block **)&iter->first.start());
    process((block **)&iter->first.end());
    process((block **)&iter->second);
  }
}

rangemap rangemap_map(rangemap *map, block *(process)(block *)) {
  auto tmp = *map;
  for (auto iter = rng_map::ConstRangeMapIterator<KElem, KElem>(*map);
       iter.has_next(); ++iter) {
    auto entry = *iter;
    tmp = tmp.inserted(entry.first, process(entry.second));
  }
  return tmp;
}

void printRangeMap(
    writer *file, rangemap *map, const char *unit, const char *element,
    const char *concat) {
  size_t size = map->size();
  if (size == 0) {
    sfprintf(file, "%s()", unit);
    return;
  }

  sfprintf(file, "\\left-assoc{}(%s(", concat);

  bool once = true;
  for (auto iter = rng_map::ConstRangeMapIterator<KElem, KElem>(*map);
       iter.has_next(); ++iter) {
    if (once) {
      once = false;
    } else {
      sfprintf(file, ",");
    }

    sfprintf(file, "%s(", element);
    auto entry = *iter;
    printConfigurationInternal(file, entry.first.start(), "SortKItem{}", false);
    sfprintf(file, ",");
    printConfigurationInternal(file, entry.first.end(), "SortKItem{}", false);
    sfprintf(file, ",");
    printConfigurationInternal(file, entry.second, "SortKItem{}", false);
    sfprintf(file, ")");
  }
  sfprintf(file, "))");
}
}
