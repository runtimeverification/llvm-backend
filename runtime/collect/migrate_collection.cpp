#include "runtime/header.h"
#include "runtime/collect.h"
#include "runtime/arena.h"

#include <cstring>

void migrate_collection_node(void **nodePtr) {
  string *currBlock = struct_base(string, data, *nodePtr);
  if (youngspace_collection_id() != getArenaSemispaceIDOfObject((void *)currBlock) &&
      oldspace_collection_id() != getArenaSemispaceIDOfObject((void *)currBlock)) {
    return;
  }
  const uint64_t hdr = currBlock->h.hdr;
  bool isInYoungGen = is_in_young_gen_hdr(hdr);
  bool hasAged = hdr & YOUNG_AGE_BIT;
  bool isInOldGen = is_in_old_gen_hdr(hdr);
  if (!(isInYoungGen || (isInOldGen && collect_old))) {
    return;
  }
  bool shouldPromote = isInYoungGen && hasAged;
  uint64_t mask = shouldPromote ? NOT_YOUNG_OBJECT_BIT : YOUNG_AGE_BIT;
  bool hasForwardingAddress = hdr & FWD_PTR_BIT;
  size_t lenInBytes = get_size(hdr, 0);
  if (!hasForwardingAddress) {
    string *newBlock;
    if (shouldPromote || (isInOldGen && collect_old)) {
      newBlock = (string *)koreAllocOld(lenInBytes);
    } else {
      newBlock = (string *)koreAlloc(lenInBytes);
    }
    memcpy(newBlock, currBlock, lenInBytes);
    newBlock->h.hdr |= mask;
    *(void **)(currBlock+1) = newBlock + 1;
    currBlock->h.hdr |= FWD_PTR_BIT;
  }
  *nodePtr = *(void **)(currBlock+1);
}

struct migrate_visitor : immer::detail::rbts::visitor_base<migrate_visitor> {
  using this_t = migrate_visitor;

  template <typename Pos>
  static void visit_inner(Pos&& pos) {
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
  static void visit_leaf(Pos&& pos) {
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
void migrate_champ_traversal(NodeT *node, immer::detail::hamts::count_t depth, Fn&& fn) {
  if (depth < immer::detail::hamts::max_depth<immer::default_bits>) {
    auto datamap = node->datamap();
    if (datamap) {
      migrate_collection_node((void **)&node->impl.d.data.inner.values);
      fn(node->values(), node->values() + immer::detail::hamts::popcount(datamap));
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

void migrate_map_leaf(std::pair<KElem, KElem> *start, std::pair<KElem, KElem> *end) {
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
