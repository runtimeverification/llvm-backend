#include <vector>

#include "runtime/collect.h"

extern std::vector<BlockEnumerator> blockEnumerators;


extern "C" {

  void migrate(block** blockPtr);

  void migrateRoots() {
    auto &l = list_impl::empty();
    migrate_list((void *)&l);
    auto &s = set_impl::empty();
    migrate_set((void *)&s);
    auto &m = map_impl::empty();
    migrate_map((void *)&m);
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

