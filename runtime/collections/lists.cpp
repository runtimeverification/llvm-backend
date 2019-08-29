#include <cstdio>

#include "runtime/header.h"
#include "runtime/header.hpp"

#include "immer/flex_vector_transient.hpp"

extern "C" {
  mpz_ptr move_int(mpz_t);

  List hook_LIST_unit() {
    return List();
  }

  List hook_LIST_element(block * value) {
    return List{value};
  }

  List hook_LIST_concat(List * l1, List * l2) {
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

  bool hook_LIST_in(block * value, List * list) {
    for (auto iter = list->begin(); iter != list->end(); ++iter) {
      if (hook_KEQUAL_eq(iter->elem, value)) {
        return true;
      }
    }
    return false;
  }

  bool hook_LIST_in_keys(mpz_t index, List * list) {
    if (!mpz_fits_ulong_p(index)) {
      throw std::invalid_argument("Index is too large for in_keys");
    }
    size_t idx = mpz_get_ui(index);
    return idx < list->size();
  }

  block * hook_LIST_get_long(List * list, ssize_t idx) {
    size_t size = list->size();
    size_t abs_idx = idx < 0 ? (long) size + idx : idx;
    return list->at(abs_idx).elem;
  }

  block * hook_LIST_get(List * list, mpz_t index) {
    if (!mpz_fits_slong_p(index)) {
      throw std::invalid_argument("Index is too large for get");
    }
    ssize_t idx = mpz_get_si(index);
    return hook_LIST_get_long(list, idx);
  }

  block * hook_LIST_lookup(List * list, mpz_t index) {
    return hook_LIST_get(list, index);
  }

  List hook_LIST_range_long(List * list, size_t front, size_t back) {
    size_t size = list->size();

    if (size < front + back) {
      throw std::out_of_range("Index out of range range_long");
    }

    auto tmp = list->transient();
    tmp.drop(front);
    tmp.take(size-back-front);
    return tmp.persistent();
  }

  List hook_LIST_range(List * list, mpz_t from_front, mpz_t from_back) {
    if (!mpz_fits_ulong_p(from_front) || !mpz_fits_ulong_p(from_back)) {
      throw std::invalid_argument("Range index too large for range");
    }

    size_t front = mpz_get_ui(from_front);
    size_t back = mpz_get_ui(from_back);

    return hook_LIST_range_long(list, front, back);
  }

  size_t hook_LIST_size_long(List * list) {
    return list->size();
  }

  mpz_ptr hook_LIST_size(List * list) {
    mpz_t size;
    mpz_init_set_ui(size, list->size());
    return move_int(size);
  }

  List hook_LIST_make(mpz_t len, block * value) {
    if (!mpz_fits_ulong_p(len)) {
      throw std::invalid_argument("Length is too large for make");
    }

    size_t length = mpz_get_ui(len);
    return List(length, value);
  }

  List hook_LIST_update(List * list, mpz_t index, block * value) {
    if (!mpz_fits_ulong_p(index)) {
      throw std::invalid_argument("Length is too large for update");
    }
    
    size_t idx = mpz_get_ui(index);
    if (idx >= list->size()) {
      throw std::invalid_argument("Index out of range for update");
    }

    return list->set(idx, value);
  }

  List hook_LIST_updateAll(List * l1, mpz_t index, List * l2) {
    if (!mpz_fits_ulong_p(index)) {
      throw std::invalid_argument("Length is too large for updateAll");
    }

    size_t idx = mpz_get_ui(index);
    size_t size = l1->size();
    size_t size2 = l2->size();
    if (idx != 0 && size2 != 0) {
      if (idx + size2 - 1 >= size) {
        throw std::invalid_argument("Index out of range for updateAll");
      }
    }

    auto tmp = l1->transient();

    for (int i = idx, j = 0; j < size2; ++i, ++j) {
      tmp.set(i, l2->at(j));
    }

    return tmp.persistent();
  }

  List hook_LIST_fill(List * l, mpz_t index, mpz_t len, block * val) {
    if (!mpz_fits_ulong_p(index)) {
      throw std::invalid_argument("Index is too large for fill");
    }

    if (!mpz_fits_ulong_p(len)) {
      throw std::invalid_argument("Length is too large for fill");
    }

    size_t idx = mpz_get_ui(index);
    size_t length = mpz_get_ui(len);

    if (idx != 0 && length != 0) {
      if (idx + length - 1 >= l->size()) {
        throw std::out_of_range("Index out of range for fill");
      }
    }

    auto tmp = l->transient();

    for (auto i = idx; i < idx + length; ++i) {
      tmp.set(i, val);
    }

    return tmp.persistent();
  }

  bool hook_LIST_eq(List * l1, List * l2) {
    return (*l1) == (*l2);
  }

  void list_hash(List * l, void * h) {
  }

  void list_foreach(List * list, void (process)(block **)) {
    for (auto iter = list->begin(); iter != list->end(); ++iter) {
      process((block **)&iter->elem);
    }
  }

  List list_map(List * list, block * (process)(block *)) {
    auto tmp = List().transient();

    for (auto iter = list->begin(); iter != list->end(); ++iter) {
      tmp.push_back(process(iter->elem));
    }

    return tmp.persistent();
  }

  List list_push_back(List * list, block * value) {
    return list->push_back(value);
  }

  void printList(writer * file, struct list * l, const char * unit, const char * element, const char * concat) {
    List * list = (List *) l;
    size_t size = list->size();

    if (size == 0) {
      sfprintf(file, "%s()", unit);
      return;
    }

    int i = 1;
    for (auto iter = list->begin(); iter != list->end(); ++iter) {
      if (i < size) {
        sfprintf(file, "%s(", concat);
      }

      sfprintf(file, "%s(", element);
      printConfigurationInternal(file, iter->elem, "KItem", false);
      sfprintf(file, ")");

      if (i < size) {
        sfprintf(file, ",");
      }

      ++i;
    }

    for (auto j = 0; j < size - 1; ++j) {
      sfprintf(file, ")");
    }

  }
}
