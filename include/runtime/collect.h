#ifndef RUNTIME_COLLECT_H
#define RUNTIME_COLLECT_H

#include <type_traits>
#include <iterator>
#include <vector>
#include "runtime/header.h"

struct block;
using block_iterator = std::vector<block **>::iterator;
typedef std::pair<block_iterator, block_iterator> (*BlockEnumerator)(void);

// This function is exported to the rest of the runtime to enable registering
// more GC roots other than the top cell of the configuration.
//
// Example usage:
void registerGCRootsEnumerator(BlockEnumerator);

using list_node = immer::detail::rbts::node<KElem, list::memory_policy, list::bits, list::bits_leaf>;
using list_impl = immer::detail::rbts::rrbtree<KElem, list::memory_policy, list::bits, list::bits_leaf>;
using map_node = map::iterator::node_t;
using map_impl = map::iterator::tree_t;
using set_node = set::iterator::node_t;
using set_impl = set::iterator::tree_t;

extern "C" {
  bool during_gc(void);
  extern bool collect_old;
  size_t get_size(uint64_t, uint16_t);
  void migrate_once(block **);
  void migrate_list(void *l);
  void migrate_map(void *m);
  void migrate_set(void *s);
  void migrate_collection_node(void **nodePtr);
  void setKoreMemoryFunctionsForGMP(void);
  void koreCollect(void**, uint8_t, layoutitem *);
}

#endif // RUNTIME_COLLECT_H
