#include "runtime/header.h"

#include "immer/flex_vector_transient.hpp"

extern "C" {
rangemap
hook_RANGEMAP_element(SortKItem start, SortKItem end, SortKItem value) {
  return rangemap().inserted(rng_map::Range<k_elem>(start, end), value);
}

struct range {
  blockheader h;
  block *start;
  block *end;
};
static struct blockheader range_header() {
  static struct blockheader hdr = {(uint64_t)-1};
  if (hdr.hdr == -1) {
    hdr = get_block_header_for_symbol(
        (uint64_t)get_tag_for_symbol_name("LblRangemap'Coln'Range{}"));
  }
  return hdr;
}
rangemap hook_RANGEMAP_elementRng(SortRange rng, SortKItem value) {
  auto *ptr = (range *)rng;
  return hook_RANGEMAP_element(ptr->start, ptr->end, value);
}

rangemap hook_RANGEMAP_unit() {
  return {};
}

rangemap hook_RANGEMAP_concat(SortRangeMap m1, SortRangeMap m2) {
  auto *mfirst = m1->size() >= m2->size() ? m1 : m2;
  auto *msec = m1->size() >= m2->size() ? m2 : m1;
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
  auto *res = hook_RANGEMAP_lookup_null(m, key);
  if (!res) {
    KLLVM_HOOK_INVALID_ARGUMENT("Key not found for range map lookup");
  }
  return res;
}

SortKItem
hook_RANGEMAP_lookupOrDefault(SortRangeMap m, SortKItem key, SortKItem def) {
  auto *res = hook_RANGEMAP_lookup_null(m, key);
  if (!res) {
    return def;
  }
  return res;
}

SortRange hook_RANGEMAP_find_range(SortRangeMap m, SortKItem key) {
  auto val = m->get_key_value(key);
  if (val.has_value()) {
    auto *ptr = (range *)kore_alloc(sizeof(range));
    ptr->h = range_header();
    ptr->start = val.value().first.start();
    ptr->end = val.value().first.end();
    return (SortRange)ptr;
  }
  KLLVM_HOOK_INVALID_ARGUMENT("Key not found for range map lookup");
}

rangemap hook_RANGEMAP_update(
    SortRangeMap m, SortKItem start, SortKItem end, SortKItem value) {
  return m->inserted(rng_map::Range<k_elem>(start, end), value);
}

rangemap
hook_RANGEMAP_updateRng(SortRangeMap m, SortRange rng, SortKItem value) {
  auto *ptr = (range *)rng;
  return hook_RANGEMAP_update(m, ptr->start, ptr->end, value);
}

rangemap hook_RANGEMAP_remove(SortRangeMap m, SortKItem start, SortKItem end) {
  return m->deleted(rng_map::Range<k_elem>(start, end));
}

rangemap hook_RANGEMAP_removeRng(SortRangeMap m, SortRange rng) {
  auto *ptr = (range *)rng;
  return hook_RANGEMAP_remove(m, ptr->start, ptr->end);
}

rangemap hook_RANGEMAP_difference(SortRangeMap m1, SortRangeMap m2) {
  return m1->difference(*m2);
}

set hook_SET_unit(void);
set hook_SET_element(block *);
set hook_SET_concat(set *, set *);

struct inj_range2kitem {
  blockheader h;
  range *child;
};
static struct blockheader inj_range2kitem_header() {
  static struct blockheader hdr = {(uint64_t)-1};
  if (hdr.hdr == -1) {
    hdr = get_block_header_for_symbol(
        (uint64_t)get_tag_for_symbol_name("inj{SortRange{}, SortKItem{}}"));
  }
  return hdr;
}
set hook_RANGEMAP_keys(SortRangeMap m) {
  auto tmp = hook_SET_unit();
  for (auto iter = rng_map::ConstRangeMapIterator<k_elem, k_elem>(*m);
       iter.has_next(); ++iter) {
    auto *ptr = (range *)kore_alloc(sizeof(range));
    ptr->h = range_header();
    ptr->start = iter->first.start();
    ptr->end = iter->first.end();
    auto *inj_ptr = (inj_range2kitem *)kore_alloc(sizeof(inj_range2kitem));
    inj_ptr->h = inj_range2kitem_header();
    inj_ptr->child = ptr;
    auto elem = hook_SET_element((SortKItem)inj_ptr);
    tmp = hook_SET_concat(&tmp, &elem);
  }
  return tmp;
}

list hook_RANGEMAP_keys_list(SortRangeMap m) {
  auto tmp = list().transient();
  for (auto iter = rng_map::ConstRangeMapIterator<k_elem, k_elem>(*m);
       iter.has_next(); ++iter) {
    auto *ptr = (range *)kore_alloc(sizeof(range));
    ptr->h = range_header();
    ptr->start = iter->first.start();
    ptr->end = iter->first.end();
    auto *inj_ptr = (inj_range2kitem *)kore_alloc(sizeof(inj_range2kitem));
    inj_ptr->h = inj_range2kitem_header();
    inj_ptr->child = ptr;
    tmp.push_back((SortKItem)inj_ptr);
  }
  return tmp.persistent();
}

bool hook_RANGEMAP_in_keys(SortKItem key, SortRangeMap m) {
  return m->contains(key);
}

list hook_RANGEMAP_values(SortRangeMap m) {
  auto tmp = list().transient();
  for (auto iter = rng_map::ConstRangeMapIterator<k_elem, k_elem>(*m);
       iter.has_next(); ++iter) {
    tmp.push_back(iter->second);
  }
  return tmp.persistent();
}

SortKItem hook_RANGEMAP_choice(SortRangeMap m) {
  if (m->empty()) {
    KLLVM_HOOK_INVALID_ARGUMENT("Cannot choose from an empty range map");
  }
  block *elem = m->treemap().root_data().first.start();
  return (SortKItem)elem;
}

SortRange hook_RANGEMAP_choiceRng(SortRangeMap m) {
  if (m->empty()) {
    KLLVM_HOOK_INVALID_ARGUMENT("Cannot choose from an empty range map");
  }
  auto pair = m->treemap().root_data();
  auto *ptr = (range *)kore_alloc(sizeof(range));
  ptr->h = range_header();
  ptr->start = pair.first.start();
  ptr->end = pair.first.end();
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
  auto *from = m2;
  auto to = *m1;
  for (auto iter = rng_map::ConstRangeMapIterator<k_elem, k_elem>(*from);
       iter.has_next(); ++iter) {
    to = to.inserted(iter->first, iter->second);
  }
  return to;
}

rangemap hook_RANGEMAP_removeAll(SortRangeMap map, SortSet set) {
  auto tmp = *map;
  for (auto iter = set->begin(); iter != set->end(); ++iter) {
    auto *b_ptr = (block *)iter->elem;
    auto *r_ptr = (range *)(b_ptr->children[0]);
    tmp = tmp.deleted(rng_map::Range<k_elem>(r_ptr->start, r_ptr->end));
  }
  return tmp;
}

bool hook_RANGEMAP_eq(SortRangeMap m1, SortRangeMap m2) {
  auto it1 = rng_map::ConstRangeMapIterator<k_elem, k_elem>(*m1);
  auto it2 = rng_map::ConstRangeMapIterator<k_elem, k_elem>(*m2);
  for (; it1.has_next() && it2.has_next(); ++it1, ++it2) {
    std::pair<rng_map::Range<k_elem>, k_elem> const &r1 = *it1;
    std::pair<rng_map::Range<k_elem>, k_elem> const &r2 = *it2;
    if (r1 != r2) {
      return false;
    }
  }
  return !(it1.has_next() || it2.has_next());
}

void rangemap_hash(rangemap *m, void *hasher) {
  if (hash_enter()) {
    for (auto iter = rng_map::ConstRangeMapIterator<k_elem, k_elem>(*m);
         iter.has_next(); ++iter) {
      auto entry = *iter;
      k_hash(entry.first.start(), hasher);
      k_hash(entry.first.end(), hasher);
      k_hash(entry.second, hasher);
    }
  }
  hash_exit();
}

rangemap rangemap_map(rangemap *map, block *(process)(block *)) {
  auto tmp = *map;
  for (auto iter = rng_map::ConstRangeMapIterator<k_elem, k_elem>(*map);
       iter.has_next(); ++iter) {
    auto entry = *iter;
    tmp = tmp.inserted(entry.first, process(entry.second));
  }
  return tmp;
}

void print_range_map(
    writer *file, rangemap *map, char const *unit, char const *element,
    char const *concat, void *state) {
  size_t size = map->size();
  if (size == 0) {
    sfprintf(file, "%s()", unit);
    return;
  }

  auto tag = get_tag_for_symbol_name(element);
  auto *arg_sorts = get_argument_sorts_for_tag(tag);

  sfprintf(file, "\\left-assoc{}(%s(", concat);

  bool once = true;
  for (auto iter = rng_map::ConstRangeMapIterator<k_elem, k_elem>(*map);
       iter.has_next(); ++iter) {
    if (once) {
      once = false;
    } else {
      sfprintf(file, ",");
    }

    sfprintf(file, "%s(", element);
    auto entry = *iter;
    sfprintf(file, "LblRangemap'Coln'Range{}(");
    print_configuration_internal(
        file, entry.first.start(), "SortKItem{}", false, state);
    sfprintf(file, ",");
    print_configuration_internal(
        file, entry.first.end(), "SortKItem{}", false, state);
    sfprintf(file, "),");
    print_configuration_internal(
        file, entry.second, arg_sorts[1], false, state);
    sfprintf(file, ")");
  }
  sfprintf(file, "))");
}
}
