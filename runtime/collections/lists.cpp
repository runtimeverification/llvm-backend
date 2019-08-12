#include "runtime/header.h"

#include "immer/vector.hpp"
#include "immer/vector_transient.hpp"

using List = immer::vector<block *>;

extern "C" {
  List hook_LIST_unit() {
    return List();
  }

  List hook_LIST_element(block * value) {
    return List{value};
  }

  List hok_LIST_concat(List * l1, List * l2) {
    auto tmp = l1->transient();
    for (auto iter = l2->begin(); iter != l2->end(); iter++) {
      tmp.push_back(*iter);
    }
    return tmp.persistent();
  }

  bool hook_LIST_in(block * value, List * list) {
    for (auto iter = list->begin(); iter != list->end(); iter++) {
      if ((*iter) == value) {
        return true;
      }
    }
    return false;
  }

  bool hook_LIST_in_keys(mpz_t index, List * list) {
    if (!mpz_fits_ulong_p(index)) {
      throw std::invalid_argument("Index is too large");
    }
    size_t idx = mpz_get_ui(index);
    return idx < list->size();
  }

  block * hook_LIST_get_long(List * list, mpz_t index) {
    if (!mpz_fits_slong_p(index)) {
      throw std::invalid_argument("Index is too large");
    }
    size_t size = list->size();
    size_t idx = mpz_get_ui(index);
    size_t abs_idx = idx < 0 ? size + idx : idx;

    return list->at(abs_idx);
  }

  block * hook_LIST_get(List * list, mpz_t index) {
    return hook_LIST_get_long(list, index);
  }

  bool hook_LIST_eq(List * l1, List * l2) {
    return (*l1) == (*l2);
  }

  void list_hash(List * l, void * h) {
  }
}
