#include "runtime/header.h"

#include "immer/flex_vector_transient.hpp"

extern "C" {
mapiter map_iterator(map *m) {
  return mapiter{m->impl.begin(), m};
}

block *map_iterator_next(mapiter *iter) {
  if (iter->curr == iter->map->impl.end()) {
    return nullptr;
  }
  return (iter->curr++)->first;
}

map hook_MAP_element(SortKItem key, SortKItem value) {
  return map(map_internal().set(key, value));
}

map hook_MAP_unit() {
  return map();
}

map hook_MAP_concat(SortMap m1, SortMap m2) {
  auto from = m1->impl.size() < m2->impl.size() ? m1 : m2;
  auto to = m1->impl.size() < m2->impl.size() ? m2->impl : m1->impl;
  for (auto iter = from->impl.begin(); iter != from->impl.end(); ++iter) {
    auto entry = *iter;
    if (to.find(entry.first)) {
      throw std::invalid_argument("Duplicate keys");
    }
    to = to.insert(*iter);
  }
  return map(std::move(to));
}

SortKItem hook_MAP_lookup_null(SortMap m, SortKItem key) {
  if (auto val = m->impl.find(key)) {
    return *val;
  }
  return nullptr;
}

SortKItem hook_MAP_lookup(SortMap m, SortKItem key) {
  auto res = hook_MAP_lookup_null(m, key);
  if (!res) {
    throw std::invalid_argument("key not found");
  }
  return res;
}

SortKItem
hook_MAP_lookupOrDefault(SortMap m, SortKItem key, SortKItem _default) {
  auto res = hook_MAP_lookup_null(m, key);
  if (!res) {
    return _default;
  }
  return res;
}

map hook_MAP_update(SortMap m, SortKItem key, SortKItem value) {
  return map(m->impl.set(key, value));
}

map hook_MAP_remove(SortMap m, SortKItem key) {
  return map(m->impl.erase(key));
}

map hook_MAP_difference(SortMap m1, SortMap m2) {
  auto from = m2;
  auto to = m1->impl;
  for (auto iter = from->impl.begin(); iter != from->impl.end(); ++iter) {
    auto entry = *iter;
    if (auto value = to.find(entry.first)) {
      if (*value == entry.second) {
        to = to.erase(entry.first);
      }
    }
  }
  return map(std::move(to));
}

set hook_SET_unit(void);
set hook_SET_element(block *);
set hook_SET_concat(set *, set *);

set hook_MAP_keys(SortMap m) {
  auto tmp = hook_SET_unit();
  for (auto iter = m->impl.begin(); iter != m->impl.end(); ++iter) {
    auto elem = hook_SET_element(iter->first);
    tmp = hook_SET_concat(&tmp, &elem);
  }
  return tmp;
}

list hook_MAP_keys_list(SortMap m) {
  auto tmp = list_internal().transient();
  for (auto iter = m->impl.begin(); iter != m->impl.end(); ++iter) {
    tmp.push_back(iter->first);
  }
  return list(tmp.persistent());
}

bool hook_MAP_in_keys(SortKItem key, SortMap m) {
  return m->impl.find(key);
}

list hook_MAP_values(SortMap m) {
  auto tmp = list_internal().transient();
  for (auto iter = m->impl.begin(); iter != m->impl.end(); ++iter) {
    tmp.push_back(iter->second);
  }
  return list(tmp.persistent());
}

SortKItem hook_MAP_choice(SortMap m) {
  if (m->impl.empty()) {
    throw std::invalid_argument("Map is empty");
  }
  return m->impl.begin()->first;
}

size_t hook_MAP_size_long(SortMap m) {
  return m->impl.size();
}

SortInt hook_MAP_size(SortMap m) {
  auto size = hook_MAP_size_long(m);
  mpz_t result;
  mpz_init_set_ui(result, size);
  return move_int(result);
}

bool hook_MAP_inclusion(SortMap m1, SortMap m2) {
  for (auto iter = m1->impl.begin(); iter != m1->impl.end(); ++iter) {
    auto entry = *iter;
    auto val = m2->impl.find(entry.first);
    if (!val || *val != entry.second) {
      return false;
    }
  }
  return true;
}

map hook_MAP_updateAll(SortMap m1, SortMap m2) {
  auto from = m2;
  auto to = m1->impl;
  for (auto iter = from->impl.begin(); iter != from->impl.end(); ++iter) {
    to = to.insert(*iter);
  }
  return map(std::move(to));
}

map hook_MAP_removeAll(SortMap m, SortSet s) {
  auto tmp = m->impl;
  for (auto iter = s->impl.begin(); iter != s->impl.end(); ++iter) {
    tmp = tmp.erase(*iter);
  }
  return map(std::move(tmp));
}

bool hook_MAP_eq(SortMap m1, SortMap m2) {
  return m1->impl == m2->impl;
}

void map_hash(map *m, void *hasher) {
  if (hash_enter()) {
    for (auto iter = m->impl.begin(); iter != m->impl.end(); ++iter) {
      auto entry = *iter;
      k_hash(entry.first, hasher);
      k_hash(entry.second, hasher);
    }
  }
  hash_exit();
}

void map_foreach(map *m, void(process)(block **)) {
  for (auto iter = m->impl.begin(); iter != m->impl.end(); ++iter) {
    process((block **)&iter->first);
    process((block **)&iter->second);
  }
}

map map_map(map *m, block *(process)(block *)) {
  auto tmp = m->impl;
  for (auto iter = m->impl.begin(); iter != m->impl.end(); ++iter) {
    auto entry = *iter;
    tmp = tmp.set(entry.first, process(entry.second));
  }
  return map(std::move(tmp));
}

void printMap(
    writer *file, map *m, const char *unit, const char *element,
    const char *concat) {
  size_t size = m->impl.size();
  if (size == 0) {
    sfprintf(file, "%s()", unit);
    return;
  }

  sfprintf(file, "\\left-assoc{}(%s(", concat);

  bool once = true;
  for (auto iter = m->impl.begin(); iter != m->impl.end(); ++iter) {
    if (once) {
      once = false;
    } else {
      sfprintf(file, ",");
    }

    sfprintf(file, "%s(", element);
    auto entry = *iter;
    printConfigurationInternal(file, entry.first, "SortKItem{}", false);
    sfprintf(file, ",");
    printConfigurationInternal(file, entry.second, "SortKItem{}", false);
    sfprintf(file, ")");
  }
  sfprintf(file, "))");
}
}
