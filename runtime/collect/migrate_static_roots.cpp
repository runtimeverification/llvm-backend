#include <vector>

#include "runtime/collect.h"

extern std::vector<BlockEnumerator> block_enumerators;

extern gmp_randstate_t kllvm_rand_state;
extern bool kllvm_rand_state_initialized;

extern "C" {

void migrate_static_roots() {
  if (kllvm_rand_state_initialized) {
    auto &rand = kllvm_rand_state->_mp_seed->_mp_d;
    string *limbs = STRUCT_BASE(string, data, rand);
    migrate((block **)&limbs);
    rand = (mp_limb_t *)limbs->data;
  }
  if (block_enumerators.empty()) {
    return;
  }

  for (auto &block_enumerator : block_enumerators) {
    auto block_iterator_pair = (*block_enumerator)();
    auto block_start_it = block_iterator_pair.first;
    auto block_end_it = block_iterator_pair.second;

    for (auto it = block_start_it; it != block_end_it; ++it) {
      migrate(*it);
    }
  }
}
}
