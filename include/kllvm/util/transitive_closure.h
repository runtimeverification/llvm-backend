#ifndef KLLVM_TRANSITIVE_CLOSURE_H
#define KLLVM_TRANSITIVE_CLOSURE_H

namespace kllvm {

/**
 * Compute the transitive closure of a relation specified as a map from single
 * elements to sets of elements of the same type. For example, a valid relation
 * type (for some element type `Elem`) is:
 *
 *   std::unordered_map<Elem, std::unordered_set<Elem>>
 */
template <typename Map>
Map transitive_closure(Map relations) {
  bool dirty = false;

  do {
    dirty = false;
    for (auto &entry : relations) {
      for (auto &elem : entry.second) {
        auto &relation = relations[elem];
        for (auto const &elem2 : relation) {
          dirty |= relations[entry.first].insert(elem2).second;
        }
      }
    }
  } while (dirty);

  return relations;
}

template <typename Map>
Map reflexive_closure(Map relations) {
  for (auto &[from, to_set] : relations) {
    to_set.insert(from);
  }

  return relations;
}

} // namespace kllvm

#endif
