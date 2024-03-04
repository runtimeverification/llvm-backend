#include <vector>

#include "runtime/collect.h"
#include "runtime/header.h"

std::vector<BlockEnumerator> block_enumerators;

void register_gc_roots_enumerator(BlockEnumerator f) {
  block_enumerators.push_back(f);
}
