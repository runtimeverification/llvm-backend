#include <cstdio>

#include "runtime/header.h"

#include "immer/flex_vector_transient.hpp"

extern "C" {
list hook_LIST_unit() {
  return list();
}

list hook_LIST_element(SortKItem value) {
  return list{value};
}

list hook_LIST_concat(SortList l1, SortList l2) {
  if (l2->size() < 32) {
    auto tmp = l1->transient();
    for (auto iter = l2->begin(); iter != l2->end(); ++iter) {
      tmp.push_back(*iter);
    }
    return tmp.persistent();
  } else {
    return (*l1) + (*l2);
  }
}

bool hook_LIST_in(SortKItem value, SortList list) {
  for (auto iter = list->begin(); iter != list->end(); ++iter) {
    if (hook_KEQUAL_eq(*iter, value)) {
      return true;
    }
  }
  return false;
}

bool hook_LIST_in_keys(SortInt index, SortList list) {
  if (!mpz_fits_ulong_p(index)) {
    KLLVM_HOOK_INVALID_ARGUMENT(
        "Index is too large for in_keys: {}", intToString(index));
  }
  size_t idx = mpz_get_ui(index);
  return idx < list->size();
}

SortKItem hook_LIST_get_long(SortList list, ssize_t idx) {
  size_t size = list->size();
  size_t abs_idx = idx < 0 ? (long)size + idx : idx;
  return list->at(abs_idx);
}

SortKItem hook_LIST_get(SortList list, SortInt index) {
  if (!mpz_fits_slong_p(index)) {
    KLLVM_HOOK_INVALID_ARGUMENT(
        "Index is too large for get: {}", intToString(index));
  }
  ssize_t idx = mpz_get_si(index);
  return hook_LIST_get_long(list, idx);
}

SortKItem hook_LIST_lookup(SortList list, SortInt index) {
  return hook_LIST_get(list, index);
}

list hook_LIST_range_long(SortList list, size_t front, size_t back) {
  size_t size = list->size();

  if (size < front + back) {
    KLLVM_HOOK_INVALID_ARGUMENT(
        "Index out of range range_long: size={}, front={}, back={}", size,
        front, back);
  }

  auto tmp = list->transient();
  tmp.drop(front);
  tmp.take(size - back - front);
  return tmp.persistent();
}

list hook_LIST_range(SortList list, SortInt from_front, SortInt from_back) {
  if (!mpz_fits_ulong_p(from_front) || !mpz_fits_ulong_p(from_back)) {
    KLLVM_HOOK_INVALID_ARGUMENT(
        "Range index too large for range: front={}, back={}",
        intToString(from_front), intToString(from_back));
  }

  size_t front = mpz_get_ui(from_front);
  size_t back = mpz_get_ui(from_back);

  return hook_LIST_range_long(list, front, back);
}

size_t hook_LIST_size_long(SortList list) {
  return list->size();
}

SortInt hook_LIST_size(SortList list) {
  mpz_t size;
  mpz_init_set_ui(size, list->size());
  return move_int(size);
}

list hook_LIST_make(SortInt len, SortKItem value) {
  if (!mpz_fits_ulong_p(len)) {
    KLLVM_HOOK_INVALID_ARGUMENT(
        "Length is too large for make: {}", intToString(len));
  }

  size_t length = mpz_get_ui(len);
  return list(length, value);
}

list hook_LIST_update(SortList list, SortInt index, SortKItem value) {
  if (!mpz_fits_ulong_p(index)) {
    KLLVM_HOOK_INVALID_ARGUMENT(
        "Length is too large for update: {}", intToString(index));
  }

  size_t idx = mpz_get_ui(index);
  if (idx >= list->size()) {
    KLLVM_HOOK_INVALID_ARGUMENT(
        "Index out of range for update: index={}, size={}", idx, list->size());
  }

  return list->set(idx, value);
}

list hook_LIST_updateAll(SortList l1, SortInt index, SortList l2) {
  if (!mpz_fits_ulong_p(index)) {
    KLLVM_HOOK_INVALID_ARGUMENT(
        "Length is too large for updateAll: {}", intToString(index));
  }

  size_t idx = mpz_get_ui(index);
  size_t size = l1->size();
  size_t size2 = l2->size();
  if (idx != 0 && size2 != 0) {
    if (idx + size2 - 1 >= size) {
      KLLVM_HOOK_INVALID_ARGUMENT(
          "Index out of range for updateAll: index={}, dest_size={}, "
          "src_size={}",
          idx, size, size2);
    }
  }

  if (size2 < 32) {
    auto tmp = l1->transient();

    for (int i = idx, j = 0; j < size2; ++i, ++j) {
      tmp.set(i, l2->at(j));
    }

    return tmp.persistent();
  } else {
    auto tmp = l1->transient();
    tmp.take(idx);
    tmp.append(l2->transient());
    auto tmp2 = l1->transient();
    tmp2.drop(idx + size2);
    tmp.append(tmp2);
    return tmp.persistent();
  }
}

list hook_LIST_fill(SortList l, SortInt index, SortInt len, SortKItem val) {
  if (!mpz_fits_ulong_p(index)) {
    KLLVM_HOOK_INVALID_ARGUMENT(
        "Index is too large for fill: {}", intToString(index));
  }

  if (!mpz_fits_ulong_p(len)) {
    KLLVM_HOOK_INVALID_ARGUMENT(
        "Length is too large for fill: {}", intToString(len));
  }

  size_t idx = mpz_get_ui(index);
  size_t length = mpz_get_ui(len);

  if (idx != 0 && length != 0) {
    if (idx + length - 1 >= l->size()) {
      KLLVM_HOOK_INVALID_ARGUMENT(
          "Index out of range for fill: index={}, length={}, size={}", idx,
          length, l->size());
    }
  }

  if (length < 32) {
    auto tmp = l->transient();

    for (auto i = idx; i < idx + length; ++i) {
      tmp.set(i, val);
    }

    return tmp.persistent();
  } else {
    auto tmp = l->transient();
    tmp.take(idx);
    auto l2 = list{length, val}.transient();
    tmp.append(l2);
    auto tmp2 = l->transient();
    tmp2.drop(idx + length);
    tmp.append(tmp2);
    return tmp.persistent();
  }
}

bool hook_LIST_eq(SortList l1, SortList l2) {
  return (*l1) == (*l2);
}

void list_hash(list *l, void *hasher) {
  if (hash_enter()) {
    for (auto iter = l->begin(); iter != l->end(); ++iter) {
      k_hash(*iter, hasher);
    }
  }
  hash_exit();
}

void list_foreach(list *list, void(process)(block **)) {
  for (auto iter = list->begin(); iter != list->end(); ++iter) {
    process((block **)&*iter);
  }
}

list list_map(list *l, block *(process)(block *)) {
  auto tmp = list().transient();

  for (auto iter = l->begin(); iter != l->end(); ++iter) {
    tmp.push_back(process(*iter));
  }

  return tmp.persistent();
}

list list_push_back(list *list, block *value) {
  return list->push_back(value);
}

void printList(
    writer *file, list *list, const char *unit, const char *element,
    const char *concat, void *state) {
  size_t size = list->size();
  if (size == 0) {
    sfprintf(file, "%s()", unit);
    return;
  }

  sfprintf(file, "\\left-assoc{}(%s(", concat);

  bool once = true;
  for (auto iter = list->begin(); iter != list->end(); ++iter) {
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
