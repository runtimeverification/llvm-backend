#include "runtime/arena.h"
#include "runtime/collect.h"
#include "runtime/header.h"

#include <cstring>

void migrate_collection_node(void **node_ptr) {
  string *curr_block = STRUCT_BASE(string, data, *node_ptr);
  if (!is_heap_block(curr_block)) {
    return;
  }
  if (youngspace_collection_id()
          != arena::get_arena_semispace_id_of_object((void *)curr_block)
      && oldspace_collection_id()
             != arena::get_arena_semispace_id_of_object((void *)curr_block)) {
    return;
  }
  uint64_t const hdr = curr_block->h.hdr;
  INITIALIZE_MIGRATE();
  size_t len_in_bytes = get_size(hdr, 0);
  if (!hasForwardingAddress) {
    string *new_block = nullptr;
    if (shouldPromote || (isInOldGen && collect_old)) {
      new_block = (string *)kore_alloc_old(len_in_bytes);
    } else {
      new_block = (string *)kore_alloc(len_in_bytes);
    }
#ifdef GC_DBG
    numBytesLiveAtCollection[oldAge] += len_in_bytes;
#endif
    memcpy(new_block, curr_block, len_in_bytes);
    MIGRATE_HEADER(new_block);
    *(void **)(curr_block + 1) = new_block + 1;
    curr_block->h.hdr |= FWD_PTR_BIT;
  }
  *node_ptr = *(void **)(curr_block + 1);
}

struct migrate_visitor : immer::detail::rbts::visitor_base<migrate_visitor> {
  using this_t = migrate_visitor;

  template <typename Pos>
  static void visit_inner(Pos &&pos) {
    for (size_t i = 0; i < pos.count(); i++) {
      void **node = (void **)pos.node()->inner() + i;
      migrate_collection_node(node);
    }
    if (auto &relaxed = pos.node()->impl.d.data.inner.relaxed) {
      migrate_collection_node((void **)&relaxed);
    }
    pos.each(this_t{});
  }

  template <typename Pos>
  static void visit_leaf(Pos &&pos) {
    for (size_t i = 0; i < pos.count(); i++) {
      block **element = (block **)pos.node()->leaf() + i;
      migrate_once(element);
    }
  }
};

void migrate_list(void *l) {
  auto const &impl = ((list *)l)->impl();
  migrate_collection_node((void **)&impl.root);
  migrate_collection_node((void **)&impl.tail);
  if (auto &relaxed = impl.root->impl.d.data.inner.relaxed) {
    migrate_collection_node((void **)&relaxed);
  }
  impl.traverse(migrate_visitor{});
}

template <typename Fn, typename NodeT>
void migrate_champ_traversal(
    NodeT *node, immer::detail::hamts::count_t depth, Fn &&fn) {
  if (depth < immer::detail::hamts::max_depth<immer::default_bits>) {
    auto datamap = node->datamap();
    if (datamap) {
      migrate_collection_node((void **)&node->impl.d.data.inner.values);
      fn(node->values(),
         node->values() + immer::detail::hamts::popcount(datamap));
    }
    auto nodemap = node->nodemap();
    if (nodemap) {
      auto fst = node->children();
      auto lst = fst + immer::detail::hamts::popcount(nodemap);
      for (; fst != lst; ++fst) {
        migrate_collection_node((void **)fst);
        migrate_champ_traversal(*fst, depth + 1, fn);
      }
    }
  } else {
    fn(node->collisions(), node->collisions() + node->collision_count());
  }
}

void migrate_map_leaf(
    std::pair<k_elem, k_elem> *start, std::pair<k_elem, k_elem> *end) {
  for (auto *it = start; it != end; ++it) {
    migrate_once(&it->first.elem);
    migrate_once(&it->second.elem);
  }
}

void migrate_set_leaf(k_elem *start, k_elem *end) {
  for (auto *it = start; it != end; ++it) {
    migrate_once(&it->elem);
  }
}

void migrate_set(void *s) {
  auto const &impl = ((set *)s)->impl();
  migrate_collection_node((void **)&impl.root);
  migrate_champ_traversal(impl.root, 0, migrate_set_leaf);
}

void migrate_map(void *m) {
  auto const &impl = ((map *)m)->impl();
  migrate_collection_node((void **)&impl.root);
  migrate_champ_traversal(impl.root, 0, migrate_map_leaf);
}

using treemap = rb_tree::RBTree<rng_map::Range<k_elem>, k_elem>;
void migrate_treemap(treemap t) {
  if (t.empty()) {
    return;
  }
  auto &data = t.root_data_mutable();
  migrate_once(&data.first.start_mutable().elem);
  migrate_once(&data.first.end_mutable().elem);
  migrate_once(&data.second.elem);
  migrate_treemap(t.left());
  migrate_treemap(t.right());
}

void migrate_rangemap(void *m) {
  migrate_treemap(((rangemap *)m)->treemap());
}
