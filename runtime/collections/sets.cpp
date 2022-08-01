#include "runtime/header.h"

#include "immer/flex_vector_transient.hpp"

extern "C" {
setiter set_iterator(set *s) {
  return setiter{s->impl.begin(), s};
}

block *set_iterator_next(setiter *iter) {
  if (iter->curr == iter->set->impl.end()) {
    return nullptr;
  }
  return *(iter->curr++);
}

set hook_SET_element(SortKItem elem) {
  return set(set_internal().insert(elem));
}

set hook_SET_unit() {
  return set();
}

bool hook_SET_in(SortKItem elem, SortSet s) {
  return s->impl.count(elem);
}

set hook_SET_concat(SortSet s1, SortSet s2) {
  auto from = s1->impl.size() < s2->impl.size() ? s1 : s2;
  auto to = s1->impl.size() < s2->impl.size() ? s2->impl : s1->impl;
  for (auto iter = from->impl.begin(); iter != from->impl.end(); ++iter) {
    to = to.insert(*iter);
  }
  return set(std::move(to));
}

set hook_SET_union(SortSet s1, SortSet s2) {
  // TODO: make concat nilpotent
  return hook_SET_concat(s1, s2);
}

set hook_SET_difference(SortSet s1, SortSet s2) {
  auto from = s2;
  auto to = s1->impl;
  for (auto iter = from->impl.begin(); iter != from->impl.end(); ++iter) {
    to = to.erase(*iter);
  }
  return set(std::move(to));
}

set hook_SET_remove(SortSet s, SortKItem elem) {
  return set(s->impl.erase(elem));
}

bool hook_SET_inclusion(SortSet s1, SortSet s2) {
  for (auto iter = s1->impl.begin(); iter != s1->impl.end(); ++iter) {
    if (!s2->impl.count(*iter)) {
      return false;
    }
  }
  return true;
}

set hook_SET_intersection(SortSet s1, SortSet s2) {
  auto from = s1->impl.size() < s2->impl.size() ? s1 : s2;
  auto to = s1->impl.size() < s2->impl.size() ? s2 : s1;
  auto result = set_internal();
  for (auto iter = from->impl.begin(); iter != from->impl.end(); ++iter) {
    auto elem = *iter;
    if (to->impl.count(elem)) {
      result = result.insert(elem);
    }
  }
  return set(std::move(result));
}

SortKItem hook_SET_choice(SortSet s) {
  if (s->impl.empty()) {
    throw std::invalid_argument("Set is empty");
  }
  return *s->impl.begin();
}

size_t hook_SET_size_long(SortSet s) {
  return s->impl.size();
}

SortInt hook_SET_size(SortSet s) {
  auto size = hook_SET_size_long(s);
  mpz_t result;
  mpz_init_set_ui(result, size);
  return move_int(result);
}

list hook_SET_set2list(SortSet s) {
  auto res = list_internal().transient();
  for (auto iter = s->impl.begin(); iter != s->impl.end(); ++iter) {
    res.push_back(*iter);
  }
  return list(res.persistent());
}

set hook_SET_list2set(SortList l) {
  auto res = set_internal();
  for (auto iter = l->impl.begin(); iter != l->impl.end(); ++iter) {
    res = res.insert(*iter);
  }
  return set(std::move(res));
}

bool hook_SET_eq(SortSet s1, SortSet s2) {
  return s1->impl == s2->impl;
}

void set_hash(set *s, void *hasher) {
  if (hash_enter()) {
    for (auto iter = s->impl.begin(); iter != s->impl.end(); ++iter) {
      k_hash(*iter, hasher);
    }
  }
  hash_exit();
}

void set_foreach(set *s, void(process)(block **)) {
  for (auto iter = s->impl.begin(); iter != s->impl.end(); ++iter) {
    process((block **)&*iter);
  }
}

set set_map(set *s, block *(process)(block *)) {
  auto tmp = set_internal();
  for (auto iter = s->impl.begin(); iter != s->impl.end(); ++iter) {
    auto elem = *iter;
    tmp = tmp.insert(process(elem));
  }
  return set(std::move(tmp));
}

void printSet(
    writer *file, set *s, const char *unit, const char *element,
    const char *concat) {
  size_t size = s->impl.size();
  if (size == 0) {
    sfprintf(file, "%s()", unit);
    return;
  }

  sfprintf(file, "\\left-assoc{}(%s(", concat);

  bool once = true;
  for (auto iter = s->impl.begin(); iter != s->impl.end(); ++iter) {
    if (once) {
      once = false;
    } else {
      sfprintf(file, ",");
    }

    sfprintf(file, "%s(", element);
    printConfigurationInternal(file, *iter, "SortKItem{}", false);
    sfprintf(file, ")");
  }
  sfprintf(file, "))");
}
}
