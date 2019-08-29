#ifndef RUNTIME_HEADER_HPP
#define RUNTIME_HEADER_HPP

#include "immer/flex_vector.hpp"
#include "immer/map.hpp"
#include "runtime/alloc.h"
#include <memory>

extern "C" {
  bool hook_KEQUAL_eq(block *, block *);
  bool during_gc(void);
}

class KElem {
public:
  KElem(block * elem) {
    this->elem = elem;
  }

  bool operator==(const KElem& other) const {
    return hook_KEQUAL_eq(this->elem, other.elem);
  }

  bool operator!=(const KElem& other) const {
    return !(*this == other);
  }

  block * elem;
};

struct kore_alloc_heap {

  template <typename... Tags>
  static void *allocate(size_t size, Tags...) {
    if (during_gc()) {
      return ::operator new(size);
    } else {
      return koreAllocNoGC(size);
    }
  }

  static void deallocate(size_t size, void *data) {
    if (during_gc()) {
      ::operator delete(data);
    }
  }
};

using List = immer::flex_vector<KElem, immer::memory_policy<immer::heap_policy<kore_alloc_heap>, immer::no_refcount_policy>>;
using Map = immer::map<KElem, KElem, immer::memory_policy<immer::heap_policy<kore_alloc_heap>, immer::no_refcount_policy>>;

#endif


