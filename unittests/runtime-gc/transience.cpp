#include <boost/test/unit_test.hpp>
#include <gmp.h>

#include <cstdint>

#include "runtime/alloc.h"
#include "runtime/arena.h"
#include "runtime/collect.h"
#include "runtime/header.h"

// Regression test for the immer gc_transience_policy ownership-token
// recycling bug.
//
// Transient list operations stamp the nodes they create with an ownership
// token (the address of a small kore-heap allocation) so that later
// operations by the same transient may mutate those nodes in place. The
// young-generation bump allocator restarts at its semispace base on every
// collection, so token addresses are recycled once their owner is gone. If a
// node's stamp survived garbage collection, a future transient whose fresh
// token landed on the recycled address would pass can_mutate on a node it
// does not own and mutate shared data in place.
//
// This test arms that collision deterministically: build a small list (its
// tail leaf is stamped by the final concat's transient), migrate it through
// two collections so the young allocator returns to the original semispace
// with the allocation pointer reset below the stamp's address, pad the young
// space so the next token allocation lands exactly on the stamped address,
// and take drop-the-head slice of the list (the compiled form of a
// `ListItem(X) REST` match). With stale stamps preserved, the slice shifts
// the shared leaf in place and corrupts the kept list; with stamps cleared
// during migration, the slice copies and the kept list is unharmed.

extern "C" {

extern thread_local constinit arena youngspace;

void init_static_objects(void);

list hook_LIST_element(block *);
list hook_LIST_concat(list *, list *);
list hook_LIST_range_long(list *, size_t, size_t);
block *hook_LIST_get_long(list *, ssize_t);
size_t hook_LIST_size_long(list *);

// Symbols normally provided by the kompiled definition. None of them are
// exercised by this test beyond satisfying the linker; the k_elem values we
// store are tagged constants that the collector treats as leaves.
bool hook_KEQUAL_eq(block *b1, block *b2) {
  return b1 == b2;
}

bool hook_KEQUAL_lt(block *b1, block *b2) {
  return b1 < b2;
}

size_t hash_k(block *kitem) {
  return (size_t)kitem;
}

void k_hash(block *, void *) { }

bool hash_enter(void) {
  return true;
}

void hash_exit(void) { }

mpz_ptr move_int(mpz_t i) {
  mpz_ptr result = (mpz_ptr)malloc(sizeof(__mpz_struct));
  *result = *i;
  return result;
}

layout *get_layout_data(uint16_t) {
  return nullptr;
}

uint32_t get_tag_for_symbol_name(char const *) {
  return 0;
}

struct blockheader get_block_header_for_symbol(uint32_t) {
  return blockheader{0};
}

char const **get_argument_sorts_for_tag(uint32_t) {
  return nullptr;
}

void print_configuration_internal(
    writer *, block *, char const *, bool, void *) { }

SortStringBuffer
hook_BUFFER_concat_raw(SortStringBuffer, char const *, uint64_t) {
  __builtin_unreachable();
}

thread_local gmp_randstate_t kllvm_rand_state;
thread_local constinit bool kllvm_rand_state_initialized = false;
}

namespace kllvm {
std::string get_raw_symbol_name(sort_category) {
  return "";
}
} // namespace kllvm

namespace {

block *tagged_elem(uintptr_t i) {
  // Low bit set marks a leaf block: never dereferenced, never migrated.
  return (block *)((i << 32) | 1);
}

void *tail_stamp(list const &l) {
  return list_node::ownee(l.impl().tail).token_.v;
}

// One collection's worth of the young-generation lifecycle for a single
// list root: flip the semispaces (resetting the new allocation space to its
// base) and migrate the list's nodes, exactly as kore_collect does for a
// list cell in the configuration.
void collect_young(list &l) {
  kore_alloc_swap(false);
  migrate_list(&l);
}

} // namespace

BOOST_AUTO_TEST_SUITE(TransienceTest)

BOOST_AUTO_TEST_CASE(recycled_token_does_not_mutate_shared_nodes) {
  init_static_objects();

  // Leave room in the original semispace so that the migrations performed
  // by collect_young below (which allocate from the semispace base) cannot
  // reach the stamp's offset before we re-arm it.
  kore_alloc(16384);

  // Build [1, 2, 3, 4, 5, 6] through the hooks; every concat runs a
  // transient, so the final tail leaf carries the last transient's token.
  list l = hook_LIST_element(tagged_elem(1));
  for (uintptr_t i = 2; i <= 6; ++i) {
    list elem = hook_LIST_element(tagged_elem(i));
    l = hook_LIST_concat(&l, &elem);
  }
  BOOST_REQUIRE_EQUAL(hook_LIST_size_long(&l), 6);

  void *stamp = tail_stamp(l);
  BOOST_REQUIRE(stamp != nullptr);

  // Two collections: the tail survives both (promoted to the old
  // generation by the second), and the young allocator is back at the base
  // of the semispace the stamp's token was allocated in.
  collect_young(l);
  collect_young(l);

  // Pad the young space so the next token allocation lands exactly on the
  // stale stamp's address. A token is the `data` field of a small string
  // block, sizeof(blockheader) past the start of its allocation.
  char *next_alloc = youngspace.end_ptr();
  ptrdiff_t gap = (char *)stamp - sizeof(blockheader) - next_alloc;
  BOOST_REQUIRE(gap >= 0);
  if (gap > 0) {
    kore_alloc(gap);
  }

  // The compiled form of a `ListItem(X) REST` match: drop the head. Its
  // transient's token is the first young allocation, i.e. the recycled
  // stamp address.
  list rest = hook_LIST_range_long(&l, 1, 0);

  // Prove the collision was armed: the result's tail is stamped with the
  // fresh token, which must have landed on the recycled address.
  BOOST_REQUIRE_EQUAL(tail_stamp(rest), stamp);

  BOOST_REQUIRE_EQUAL(hook_LIST_size_long(&rest), 5);
  for (uintptr_t i = 0; i < 5; ++i) {
    BOOST_CHECK_EQUAL(hook_LIST_get_long(&rest, i), tagged_elem(i + 2));
  }

  // The kept list must be untouched. If a stale stamp survived migration,
  // the drop above mutated the shared tail in place and this reads
  // [2, 3, 4, 5, 6, 6].
  BOOST_REQUIRE_EQUAL(hook_LIST_size_long(&l), 6);
  for (uintptr_t i = 0; i < 6; ++i) {
    BOOST_CHECK_EQUAL(hook_LIST_get_long(&l, i), tagged_elem(i + 1));
  }
}

BOOST_AUTO_TEST_SUITE_END()
