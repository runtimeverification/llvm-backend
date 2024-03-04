#include <vector>

#include "runtime/collect.h"

extern std::vector<BlockEnumerator> block_enumerators;

extern gmp_randstate_t kllvm_rand_state;
extern bool kllvm_rand_state_initialized;

extern "C" {

void migrate(block **block_ptr);

void migrateRoots() {
  auto &l1 = list_impl::empty_root();
  migrate_collection_node((void **)&l1);
  auto &l2 = list_impl::empty_tail();
  migrate_collection_node((void **)&l2);
  auto &s = set_impl::empty();
  migrate_collection_node((void **)&s);
  auto &m = map_impl::empty();
  migrate_collection_node((void **)&m);
  if (kllvm_rand_state_initialized) {
    auto &rand = kllvm_rand_state->_mp_seed->_mp_d;
    string *limbs = STRUCT_BASE(string, data, rand);
    migrate((block **)&limbs);
    rand = (mp_limb_t *)limbs->data;
  }
  if (block_enumerators.empty()) {
    return;
  }

  for (auto &blockEnumerator : block_enumerators) {
    auto BlockIteratorPair = (*blockEnumerator)();
    auto BlockStartIt = BlockIteratorPair.first;
    auto BlockEndIt = BlockIteratorPair.second;

    for (auto it = BlockStartIt; it != BlockEndIt; ++it) {
      migrate(*it);
    }
  }
}
}
