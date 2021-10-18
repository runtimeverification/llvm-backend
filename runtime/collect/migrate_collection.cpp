#include "runtime/arena.h"
#include "runtime/collect.h"
#include "runtime/header.h"

#include <cstring>

void migrate_collection_node(void **nodePtr) {
  string *currBlock = struct_base(string, data, *nodePtr);
  if (youngspace_collection_id()
          != getArenaSemispaceIDOfObject((void *)currBlock)
      && oldspace_collection_id()
             != getArenaSemispaceIDOfObject((void *)currBlock)) {
    return;
  }
  const uint64_t hdr = currBlock->h.hdr;
  initialize_migrate();
  size_t lenInBytes = get_size(hdr, 0);
  if (!hasForwardingAddress) {
    string *newBlock;
    if (shouldPromote || (isInOldGen && collect_old)) {
      newBlock = (string *)koreAllocOld(lenInBytes);
    } else {
      newBlock = (string *)koreAlloc(lenInBytes);
    }
#ifdef GC_DBG
    numBytesLiveAtCollection[oldAge] += lenInBytes;
#endif
    memcpy(newBlock, currBlock, lenInBytes);
    migrate_header(newBlock);
    *(void **)(currBlock + 1) = newBlock + 1;
    currBlock->h.hdr |= FWD_PTR_BIT;
  }
  *nodePtr = *(void **)(currBlock + 1);
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
  auto &impl = ((list *)l)->impl();
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
    std::pair<KElem, KElem> *start, std::pair<KElem, KElem> *end) {
  for (auto it = start; it != end; ++it) {
    migrate_once(&it->first.elem);
    migrate_once(&it->second.elem);
  }
}

void migrate_set_leaf(KElem *start, KElem *end) {
  for (auto it = start; it != end; ++it) {
    migrate_once(&it->elem);
  }
}

void migrate_set(void *s) {
  auto &impl = ((set *)s)->impl();
  migrate_collection_node((void **)&impl.root);
  migrate_champ_traversal(impl.root, 0, migrate_set_leaf);
}

void migrate_map(void *m) {
  auto &impl = ((map *)m)->impl();
  migrate_collection_node((void **)&impl.root);
  migrate_champ_traversal(impl.root, 0, migrate_map_leaf);
}

template <typename Iter, typename Elem, typename Node>
void evacuate_iter(void *i) {
  auto it = (Iter *)i;
  struct iter {
    Elem *cur_;
    Elem *end_;
    immer::detail::hamts::count_t depth_;
    immer::detail::hamts::count_t cur_off_;
    Node *const
        *path_[immer::detail::hamts::max_depth<immer::default_bits> + 1];
    std::uint8_t
        path_off_[immer::detail::hamts::max_depth<immer::default_bits>];
  };
  auto impl = (iter *)&it->curr;
  // impl->path_[0] always points to the same address at which the root of the
  // map or set is located. This is because impl->path_[0] is always taken to be
  // equal to a pointer to the pointer to the root of the collection. Because
  // this map/list/set is always allocated in the kore heap inline to the block
  // that it is a child of, and because of the particular behavior of the
  // migrate function when migrating regular blocks, we know that
  // *impl->path_[0] can be used in order to compute the base pointer and the
  // derived offset of the map pointer within its parent block
  block **root_ptr = (block **)impl->path_[0];
  if ((uintptr_t)*root_ptr >= 256) {
    impl->path_[0] = (Node *const *)(*root_ptr + 1);
  } else {
    // in words
    auto derived_offset = (size_t)*root_ptr;
    auto relocated_base_ptr = *(root_ptr - derived_offset - 1);
    impl->path_[0] = (Node *const *)(relocated_base_ptr + derived_offset);
  }
  for (size_t i = 1; i <= impl->depth_; i++) {
    auto derived_ptr = impl->path_[i];
    auto buffer_ptr = derived_ptr - impl->path_off_[i - 1];
    auto base_ptr = struct_base(Node, impl.d.data.inner.buffer, buffer_ptr);
    auto derived_offset = (char *)derived_ptr - (char *)base_ptr;
    migrate_collection_node((void **)&base_ptr);
    auto relocated_derived_ptr
        = (Node *const *)((char *)base_ptr + derived_offset);
    impl->path_[i] = relocated_derived_ptr;
  }
  auto derived_ptr = impl->cur_;
  if (derived_ptr) {
    auto base_ptr = derived_ptr - impl->cur_off_;
    auto derived_offset = (char *)derived_ptr - (char *)base_ptr;
    auto end_offset = impl->end_ - impl->cur_;
    auto child = *impl->path_[impl->depth_];
    Elem *relocated_base_ptr;
    if (impl->depth_ < immer::detail::hamts::max_depth<immer::default_bits>) {
      relocated_base_ptr = child->values();
    } else {
      relocated_base_ptr = child->collisions();
    }
    auto relocated_derived_ptr
        = (Elem *)((char *)relocated_base_ptr + derived_offset);
    impl->cur_ = relocated_derived_ptr;
    impl->end_ = impl->cur_ + end_offset;
  }
}

void evacuate_setiter(void *i) {
  evacuate_iter<setiter, KElem, set::iterator::node_t>(i);
}

void evacuate_mapiter(void *i) {
  evacuate_iter<mapiter, std::pair<KElem, KElem>, map::iterator::node_t>(i);
}
