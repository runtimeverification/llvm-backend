#include <cstdio>

#include "runtime/header.h"

#include "immer/vector.hpp"
#include "immer/vector_transient.hpp"

using List = immer::vector<block *>;

extern "C" {
  mpz_ptr move_int(mpz_t);
  bool hook_KEQUAL_eq(block *, block *);

  List hook_LIST_unit() {
    return List();
  }

  List hook_LIST_element(block * value) {
    return List{value};
  }

  List hook_LIST_concat(List * l1, List * l2) {
    auto tmp = l1->transient();
    for (auto iter = l2->begin(); iter != l2->end(); ++iter) {
      tmp.push_back(*iter);
    }
    return tmp.persistent();
  }

  bool hook_LIST_in(block * value, List * list) {
    for (auto iter = list->begin(); iter != list->end(); ++iter) {
      if (hook_KEQUAL_eq(*iter, value)) {
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
    return list->at(abs_idx);
  }

  block * hook_LIST_get(List * list, mpz_t index) {
    if (!mpz_fits_slong_p(index)) {
      throw std::invalid_argument("Index is too large for get");
    }
    ssize_t idx = mpz_get_si(index);
    return hook_LIST_get_long(list, idx);
  }

  List hook_LIST_range_long(List * list, size_t front, size_t back) {
    size_t size = list->size();

    if (size < size - back) {
      throw std::out_of_range("Index out of range range_long");
    }

    auto tmp = List().transient();
    for (int i = front; i < front + back; ++i) {
      tmp.push_back((*list)[i]);
    }

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

  bool hook_LIST_eq(List * l1, List * l2) {
    return (*l1) == (*l2);
  }

  void list_hash(List * l, void * h) {
  }

  void list_foreach(List * list, void (process)(block **)) {
    block * t;
    for (auto iter = list->begin(); iter != list->end(); ++iter) {
      t = *iter;
      process(&t);
    }
  }

  List list_map(List * list, block * (process)(block *)) {
    auto tmp = List().transient();

    for (auto iter = list->begin(); iter != list->end(); ++iter) {
      tmp.push_back(process(*iter));
    }

    return tmp.persistent();
  }

  void printList(FILE * file, struct vector * l, const char * unit, const char * element, const char * concat) {
    List * list = (List *) l;
    size_t size = list->size();

    if (size == 0) {
      fprintf(file, "%s()", unit);
      return;
    }

    int i = 1;
    for (auto iter = list->begin(); iter != list->end(); ++iter) {
      if (i < size) {
        fprintf(file, "%s(", concat);
      }

      fprintf(file, "%s(", element);
      printConfigurationInternal(file, *iter, "KItem", false);
      fprintf(file, ")");

      if (i < size) {
        fprintf(file, ",");
      }

      ++i;
    }

    for (auto j = 0; j < size - 1; ++j) {
      fprintf(file, ")");
    }

  }
}
