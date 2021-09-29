#ifndef RUNTIME_COLLECT_H
#define RUNTIME_COLLECT_H

#include "runtime/header.h"
#include <iterator>
#include <map>
#include <type_traits>
#include <vector>

struct block;
using block_iterator = std::vector<block **>::iterator;
typedef std::pair<block_iterator, block_iterator> (*BlockEnumerator)(void);

// This function is exported to the rest of the runtime to enable registering
// more GC roots other than the top cell of the configuration.
//
// Example usage:
void registerGCRootsEnumerator(BlockEnumerator);

using list_node = immer::detail::rbts::node<
    KElem, list::memory_policy, list::bits, list::bits_leaf>;
using list_impl = immer::detail::rbts::rrbtree<
    KElem, list::memory_policy, list::bits, list::bits_leaf>;
using map_node = map::iterator::node_t;
using map_impl = map::iterator::tree_t;
using set_node = set::iterator::node_t;
using set_impl = set::iterator::tree_t;

void parseStackMap(void);

struct gc_relocation {
  layoutitem base;
  uint64_t derived_offset;
};

extern std::map<void *, std::vector<gc_relocation>> StackMap;

extern "C" {
extern size_t numBytesLiveAtCollection[1 << AGE_WIDTH];
bool during_gc(void);
extern bool collect_old;
size_t get_size(uint64_t, uint16_t);
void migrate_once(block **);
void migrate_list(void *l);
void migrate_map(void *m);
void migrate_set(void *s);
void migrate_collection_node(void **nodePtr);
void setKoreMemoryFunctionsForGMP(void);
void koreCollect(bool afterStep);
}

#ifdef GC_DBG
#define initialize_age()                                                       \
  uint64_t age = (hdr & AGE_MASK) >> AGE_OFFSET;                               \
  uint64_t oldAge = age;
#define increment_age()                                                        \
  if (age < ((1 << AGE_WIDTH) - 1))                                            \
    age++;
#define migrate_header(block)                                                  \
  block->h.hdr |= shouldPromote ? NOT_YOUNG_OBJECT_BIT : 0;                    \
  block->h.hdr &= ~AGE_MASK;                                                   \
  block->h.hdr |= age << AGE_OFFSET
#else
#define initialize_age() bool age = hdr & AGE_MASK;
#define increment_age()
#define migrate_header(block)                                                  \
  block->h.hdr |= shouldPromote ? NOT_YOUNG_OBJECT_BIT : AGE_MASK
#endif

#define initialize_migrate()                                                   \
  bool isInYoungGen = is_in_young_gen_hdr(hdr);                                \
  initialize_age() bool isInOldGen = is_in_old_gen_hdr(hdr);                   \
  if (!(isInYoungGen || (isInOldGen && collect_old))) {                        \
    return;                                                                    \
  }                                                                            \
  bool shouldPromote = isInYoungGen && age;                                    \
  increment_age() bool hasForwardingAddress = hdr & FWD_PTR_BIT

#endif // RUNTIME_COLLECT_H
