#include <cstdio>

#include "runtime/header.h"

#include "immer/flex_vector_transient.hpp"

extern "C" {
list hook_LIST_unit() {
  return list();
}

list hook_LIST_element(SortKItem value) {
  return list(list_internal{value});
}

list hook_LIST_concat(SortList l1, SortList l2) {
  if (l2->impl.size() < 32) {
    auto tmp = l1->impl.transient();
    for (auto iter = l2->impl.begin(); iter != l2->impl.end(); ++iter) {
      tmp.push_back(*iter);
    }
    return list(tmp.persistent());
  } else {
    return list(l1->impl + l2->impl);
  }
}

bool hook_LIST_in(SortKItem value, SortList l) {
  for (auto iter = l->impl.begin(); iter != l->impl.end(); ++iter) {
    if (hook_KEQUAL_eq(*iter, value)) {
      return true;
    }
  }
  return false;
}

bool hook_LIST_in_keys(SortInt index, SortList l) {
  if (!mpz_fits_ulong_p(index)) {
    throw std::invalid_argument("Index is too large for in_keys");
  }
  size_t idx = mpz_get_ui(index);
  return idx < l->impl.size();
}

SortKItem hook_LIST_get_long(SortList l, ssize_t idx) {
  size_t size = l->impl.size();
  size_t abs_idx = idx < 0 ? (long)size + idx : idx;
  return l->impl.at(abs_idx);
}

SortKItem hook_LIST_get(SortList l, SortInt index) {
  if (!mpz_fits_slong_p(index)) {
    throw std::invalid_argument("Index is too large for get");
  }
  ssize_t idx = mpz_get_si(index);
  return hook_LIST_get_long(l, idx);
}

SortKItem hook_LIST_lookup(SortList l, SortInt index) {
  return hook_LIST_get(l, index);
}

list hook_LIST_range_long(SortList l, size_t front, size_t back) {
  size_t size = l->impl.size();

  if (size < front + back) {
    throw std::out_of_range("Index out of range range_long");
  }

  auto tmp = l->impl.transient();
  tmp.drop(front);
  tmp.take(size - back - front);
  return list(tmp.persistent());
}

list hook_LIST_range(SortList l, SortInt from_front, SortInt from_back) {
  if (!mpz_fits_ulong_p(from_front) || !mpz_fits_ulong_p(from_back)) {
    throw std::invalid_argument("Range index too large for range");
  }

  size_t front = mpz_get_ui(from_front);
  size_t back = mpz_get_ui(from_back);

  return hook_LIST_range_long(l, front, back);
}

size_t hook_LIST_size_long(SortList l) {
  return l->impl.size();
}

SortInt hook_LIST_size(SortList l) {
  mpz_t size;
  mpz_init_set_ui(size, l->impl.size());
  return move_int(size);
}

list hook_LIST_make(SortInt len, SortKItem value) {
  if (!mpz_fits_ulong_p(len)) {
    throw std::invalid_argument("Length is too large for make");
  }

  size_t length = mpz_get_ui(len);
  return list(list_internal(length, value));
}

list hook_LIST_update(SortList l, SortInt index, SortKItem value) {
  if (!mpz_fits_ulong_p(index)) {
    throw std::invalid_argument("Length is too large for update");
  }

  size_t idx = mpz_get_ui(index);
  if (idx >= l->impl.size()) {
    throw std::invalid_argument("Index out of range for update");
  }

  return list(l->impl.set(idx, value));
}

list hook_LIST_updateAll(SortList l1, SortInt index, SortList l2) {
  if (!mpz_fits_ulong_p(index)) {
    throw std::invalid_argument("Length is too large for updateAll");
  }

  size_t idx = mpz_get_ui(index);
  size_t size = l1->impl.size();
  size_t size2 = l2->impl.size();
  if (idx != 0 && size2 != 0) {
    if (idx + size2 - 1 >= size) {
      throw std::invalid_argument("Index out of range for updateAll");
    }
  }

  if (size2 < 32) {
    auto tmp = l1->impl.transient();

    for (int i = idx, j = 0; j < size2; ++i, ++j) {
      tmp.set(i, l2->impl.at(j));
    }

    return list(tmp.persistent());
  } else {
    auto tmp = l1->impl.transient();
    tmp.take(idx);
    tmp.append(l2->impl.transient());
    auto tmp2 = l1->impl.transient();
    tmp2.drop(idx + size2);
    tmp.append(tmp2);
    return list(tmp.persistent());
  }
}

list hook_LIST_fill(SortList l, SortInt index, SortInt len, SortKItem val) {
  if (!mpz_fits_ulong_p(index)) {
    throw std::invalid_argument("Index is too large for fill");
  }

  if (!mpz_fits_ulong_p(len)) {
    throw std::invalid_argument("Length is too large for fill");
  }

  size_t idx = mpz_get_ui(index);
  size_t length = mpz_get_ui(len);

  if (idx != 0 && length != 0) {
    if (idx + length - 1 >= l->impl.size()) {
      throw std::out_of_range("Index out of range for fill");
    }
  }

  if (length < 32) {
    auto tmp = l->impl.transient();

    for (auto i = idx; i < idx + length; ++i) {
      tmp.set(i, val);
    }

    return list(tmp.persistent());
  } else {
    auto tmp = l->impl.transient();
    tmp.take(idx);
    auto l2 = list_internal{length, val}.transient();
    tmp.append(l2);
    auto tmp2 = l->impl.transient();
    tmp2.drop(idx + length);
    tmp.append(tmp2);
    return list(tmp.persistent());
  }
}

bool hook_LIST_eq(SortList l1, SortList l2) {
  return l1->impl == l2->impl;
}

void list_hash(list *l, void *hasher) {
  if (hash_enter()) {
    for (auto iter = l->impl.begin(); iter != l->impl.end(); ++iter) {
      k_hash(*iter, hasher);
    }
  }
  hash_exit();
}

void list_foreach(list *list, void(process)(block **)) {
  for (auto iter = list->impl.begin(); iter != list->impl.end(); ++iter) {
    process((block **)&*iter);
  }
}

list list_map(list *l, block *(process)(block *)) {
  auto tmp = list_internal().transient();

  for (auto iter = l->impl.begin(); iter != l->impl.end(); ++iter) {
    tmp.push_back(process(*iter));
  }

  return list(tmp.persistent());
}

list list_push_back(list *l, block *value) {
  return list(l->impl.push_back(value));
}

void printList(
    writer *file, list *l, const char *unit, const char *element,
    const char *concat) {
  size_t size = l->impl.size();
  if (size == 0) {
    sfprintf(file, "%s()", unit);
    return;
  }

  sfprintf(file, "\\left-assoc{}(%s(", concat);

  bool once = true;
  for (auto iter = l->impl.begin(); iter != l->impl.end(); ++iter) {
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
