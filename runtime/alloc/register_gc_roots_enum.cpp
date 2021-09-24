#include <vector>

#include "runtime/collect.h"
#include "runtime/header.h"

std::vector<BlockEnumerator> blockEnumerators;

void registerGCRootsEnumerator(BlockEnumerator f) {
  blockEnumerators.push_back(f);
}
