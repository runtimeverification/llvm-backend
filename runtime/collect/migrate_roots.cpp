#include <vector>

#include "runtime/collect.h"

extern std::vector<BlockEnumerator> blockEnumerators;

extern gmp_randstate_t kllvm_randState;
extern bool kllvm_randStateInitialized;

extern "C" {

  void migrate(block** blockPtr);

  void migrateRoots() {
    auto &l = list_impl::empty();
    migrate_list((void *)&l);
    auto &s = set_impl::empty();
    migrate_set((void *)&s);
    auto &m = map_impl::empty();
    migrate_map((void *)&m);
    if (kllvm_randStateInitialized) {
      auto &rand = kllvm_randState->_mp_seed->_mp_d;
      string *limbs = struct_base(string, data, rand);
      migrate((block **)&limbs);
      rand = (mp_limb_t *)limbs->data;
    }
    if (blockEnumerators.empty()) {
      return;
    }

    for (auto iter = blockEnumerators.begin(); iter != blockEnumerators.end(); iter++) {
      auto BlockIteratorPair = (*(*iter))();
      block_iterator BlockStartIt = BlockIteratorPair.first;
      block_iterator BlockEndIt = BlockIteratorPair.second;
      
      for (block_iterator it =  BlockStartIt; it != BlockEndIt; ++it) {
        migrate(*it);
      }
    }
  }
}

