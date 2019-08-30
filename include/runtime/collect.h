#ifndef RUNTIME_COLLECT_H
#define RUNTIME_COLLECT_H

#include <type_traits>
#include <iterator>
#include <vector>

struct block;
using block_iterator = std::vector<block **>::iterator;
typedef std::pair<block_iterator, block_iterator> (*BlockEnumerator)(void);

// This function is exported to the rest of the runtime to enable registering
// more GC roots other than the top cell of the configuration.
//
// Example usage:
void registerGCRootsEnumerator(BlockEnumerator);

extern "C" {
  bool during_gc(void);
}

#endif // RUNTIME_COLLECT_H
