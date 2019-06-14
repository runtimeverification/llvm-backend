#include "runtime/header.h"
#include "runtime/collect.h"

static BlockEnumerator blockEnumerator = nullptr;

void registerGCRootsEnumerator(BlockEnumerator f) {
  blockEnumerator = f;
}

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

