#include "runtime/header.h"
#include "runtime/collect.h"

BlockEnumerator blockEnumerator = nullptr;

void registerGCRootsEnumerator(BlockEnumerator f) {
  blockEnumerator = f;
}
