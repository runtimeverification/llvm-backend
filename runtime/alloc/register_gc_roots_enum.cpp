#include <vector>

#include "runtime/collect.h"
#include "runtime/header.h"

std::vector<BlockEnumerator> block_enumerators;

void registerGCRootsEnumerator(BlockEnumerator f) {
  block_enumerators.push_back(f);
}
