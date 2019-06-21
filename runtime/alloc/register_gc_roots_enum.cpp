#include <vector>

#include "runtime/header.h"
#include "runtime/collect.h"

std::vector<BlockEnumerator> blockEnumerators;

void registerGCRootsEnumerator(BlockEnumerator f) {
  blockEnumerators.push_back(f);
}
