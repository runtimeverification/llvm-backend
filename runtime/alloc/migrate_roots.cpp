#include "runtime/collect.h"

extern BlockEnumerator blockEnumerator;

extern "C" {

  void migrate(block** blockPtr);

  void migrateRoots() {
    if (!blockEnumerator) {
      return;
    }

    auto BlockIteratorPair = (*blockEnumerator)();
    block_iterator BlockStartIt = BlockIteratorPair.first;
    block_iterator BlockEndIt = BlockIteratorPair.second;
    
    for (block_iterator it =  BlockStartIt; it != BlockEndIt; ++it) {
      migrate(*it);
    }
  }
}

