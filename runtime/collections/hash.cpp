#include <boost/functional/hash.hpp>

#include "runtime/header.h"

extern "C" {
  void add_hash64(void *h, uint64_t data) {
    size_t *hash = (size_t *)h;
    boost::hash_combine(*hash, data);
  }

  void add_hash8(void *h, uint8_t data) {
    size_t *hash = (size_t *)h;
    boost::hash_combine(*hash, data);
  }

  size_t hash_k(block *term) {
    size_t hash = 0;
    k_hash(term, &hash);
    return hash; 
  }
}
