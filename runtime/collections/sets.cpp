#include "runtime/header.h"

#include "immer/flex_vector_transient.hpp"

extern "C" {
  setiter set_iterator(set *set) {
    return setiter{set->begin(), set};
  }

  block *set_iterator_next(setiter *iter) {
    if (iter->curr == iter->set->end()) {
      return nullptr;
    }
    return *(iter->curr++);
  }

  set hook_SET_element(block *elem) {
    return set().insert(elem);
  }

  set hook_SET_unit() {
    return set();
  }

  bool hook_SET_in(block *elem, set *set) {
    return set->count(elem);
  }

  set hook_SET_concat(set *s1, set *s2) {
    auto from = s1->size() < s2->size() ? s1 : s2;
    auto to = s1->size() < s2->size() ? *s2 : *s1;
    for (auto iter = from->begin(); iter != from->end(); ++iter) {
      to = to.insert(*iter);
    }
    return to;
  }

  set hook_SET_difference(set *s1, set *s2) {
    auto from = s2;
    auto to = *s1;
    for (auto iter = from->begin(); iter != from->end(); ++iter) {
	to = to.erase(*iter);
    }
    return to;
  }

  set hook_SET_remove(set *s, block *elem) {
    return s->erase(elem);
  }

  bool hook_SET_inclusion(set *s1, set *s2) {
    for (auto iter = s1->begin(); iter != s1->end(); ++iter) {
      if (!s2->count(*iter)) {
	return false;
      }
    }
    return true;
  }

  set hook_SET_intersection(set *s1, set *s2) {
    auto from = s1->size() < s2->size() ? s1 : s2;
    auto to = s1->size() < s2->size() ? s2 : s1;
    auto result = set();
    for (auto iter = from->begin(); iter != from->end(); ++iter) {
      auto elem = *iter;
      if (to->count(elem)) {
	result = result.insert(elem);
      }
    }
    return result;
  }

  block *hook_SET_choice(set *s) {
    if (s->empty()) {
      throw std::invalid_argument("Set is empty");
    }
    return *s->begin();
  }

  size_t hook_SET_size_long(set *s) {
    return s->size();
  }

  mpz_ptr hook_SET_size(set *s) {
    auto size = hook_SET_size_long(s);
    mpz_t result;
    mpz_init_set_ui(result, size);
    return move_int(result);
  }

  list hook_SET_set2list(set *s) {
    auto res = list().transient();
    for (auto iter = s->begin(); iter != s->end(); ++iter) {
      res.push_back(*iter);
    }
    return res.persistent();
  }

  set hook_SET_list2set(list *l) {
    auto res = set();
    for (auto iter = l->begin(); iter != l->end(); ++iter) {
      res = res.insert(*iter);
    }
    return res;
  }

  bool hook_SET_eq(set *s1, set *s2) {
    return (*s1) == (*s2);
  }

  void set_hash(set *s, void *hasher) {
    if (hash_enter()) {
      for (auto iter = s->begin(); iter != s->end(); ++iter) {
        k_hash(*iter, hasher);
      }
    }
    hash_exit();
  }

  void set_foreach(set *set, void(process)(block **)) {
    for (auto iter = set->begin(); iter != set->end(); ++iter) {
      process((block **)&*iter);
    }
  }

  set set_map(set *s, block *(process)(block *)) {
    auto tmp = set();
    for (auto iter = s->begin(); iter != s->end(); ++iter) {
      auto elem = *iter;
      tmp = tmp.insert(process(elem));
    }
    return tmp;
  }

  void printSet(writer *file, set *set, const char *unit, const char *element, const char *concat) {
    size_t size = set->size();
    if (size == 0) {
      sfprintf(file, "%s()", unit);
      return;
    }

    int i = 1;
    for (auto iter = set->begin(); iter != set->end(); ++iter) {
      if (i < size) {
        sfprintf(file, "%s(", concat);
      }

      sfprintf(file, "%s(", element);
      auto elem = *iter;
      printConfigurationInternal(file, elem, "KItem", false);
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
