#ifndef RUNTIME_HEADER_HPP
#define RUNTIME_HEADER_HPP

#include "immer/vector.hpp"
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

  bool operator==(const KElem& other) {
    return hook_KEQUAL_eq(this->elem, other.elem);
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

using List = immer::vector<KElem, immer::memory_policy<immer::free_list_heap_policy<kore_alloc_heap>, immer::default_refcount_policy>>;

#endif


