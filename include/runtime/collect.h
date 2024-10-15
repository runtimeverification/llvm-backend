#ifndef RUNTIME_COLLECT_H
#define RUNTIME_COLLECT_H

#include "runtime/header.h"
#include <iterator>
#include <type_traits>
#include <vector>

struct block;
using block_iterator = std::vector<block **>::iterator;
using BlockEnumerator = std::pair<block_iterator, block_iterator> (*)();

// This function is exported to the rest of the runtime to enable registering
// more GC roots other than the top cell of the configuration.
//
// Example usage:
void register_gc_roots_enumerator(BlockEnumerator);

using list_node = immer::detail::rbts::node<
    k_elem, list::memory_policy, list::bits, list::bits_leaf>;
using list_impl = immer::detail::rbts::rrbtree<
    k_elem, list::memory_policy, list::bits, list::bits_leaf>;
using map_node = map::iterator::node_t;
using map_impl = map::iterator::tree_t;
using set_node = set::iterator::node_t;
using set_impl = set::iterator::tree_t;

extern "C" {
extern size_t numBytesLiveAtCollection[1 << AGE_WIDTH];
extern bool collect_old;
size_t get_size(uint64_t, uint16_t);
void migrate_static_roots(void);
void migrate(block **block_ptr);
void migrate_once(block **);
void migrate_list(void *l);
void migrate_map(void *m);
void migrate_rangemap(void *m);
void migrate_set(void *s);
void migrate_collection_node(void **node_ptr);
void set_kore_memory_functions_for_gmp(void);
void kore_collect(void **, uint8_t, layoutitem *, bool force = false);
void free_all_kore_mem();
}

#ifdef GC_DBG
#define INITIALIZE_AGE()                                                       \
  uint64_t age = (hdr & AGE_MASK) >> AGE_OFFSET;                               \
  uint64_t oldAge = age;
#define increment_age()                                                        \
  if (age < ((1 << AGE_WIDTH) - 1))                                            \
    age++;
#define MIGRATE_HEADER(block)                                                  \
  block->h.hdr |= shouldPromote ? NOT_YOUNG_OBJECT_BIT : 0;                    \
  block->h.hdr &= ~AGE_MASK;                                                   \
  block->h.hdr |= age << AGE_OFFSET
#else
#define INITIALIZE_AGE() bool age = hdr & AGE_MASK;
#define increment_age()
#define MIGRATE_HEADER(block)                                                  \
  block->h.hdr |= shouldPromote ? NOT_YOUNG_OBJECT_BIT : AGE_MASK
#endif

#define INITIALIZE_MIGRATE()                                                   \
  bool isInYoungGen = is_in_young_gen_hdr(hdr);                                \
  INITIALIZE_AGE() bool isInOldGen = is_in_old_gen_hdr(hdr);                   \
  if (!(isInYoungGen || (isInOldGen && collect_old))) {                        \
    return;                                                                    \
  }                                                                            \
  bool shouldPromote = isInYoungGen && age;                                    \
  increment_age() bool hasForwardingAddress = hdr & FWD_PTR_BIT

#endif // RUNTIME_COLLECT_H
