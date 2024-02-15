#ifndef KLLVM_ATTRIBUTES_H
#define KLLVM_ATTRIBUTES_H

#include <memory>
#include <string>
#include <unordered_map>

namespace kllvm {

class KORECompositePattern;

/**
 * Type-safe wrapper around a set of KORE attribute patterns.
 */
class attribute_set {
public:
  enum class key {
    source,
    location,
    hook,
    function,
    format,
    assoc,
    comm,
    colors,
    bracket,
    anywhere,
    binder,
    concat,
    unit,
    element,
    sort_injection,
    label,
    nat,
    macro,
    macro_rec,
    alias,
    alias_rec,
    left,
    right,
    priorities,
    subsort,
    overload,
    fresh_generator,
  };

  attribute_set() = default;

  /**
   * Accepts a composite pattern of the form:
   *
   *   key_name{}(args, ...)
   *
   * The constructor name (`key_name` here) is parsed into a value of the `key`
   * enum; any unknown attribute names will be rejected.
   */
  key add(std::shared_ptr<KORECompositePattern> att);

  /**
   * Returns true if there is any attribute with the given key; the arguments of
   * that attribute are not considered.
   */
  bool contains(key k) const;

  /**
   * Look up the attribute pattern with the specified key.
   *
   * This lookup is unchecked; use `.contains(k)` first if the attribute may not
   * be present.
   */
  std::shared_ptr<KORECompositePattern> const &get(key k) const;

  /**
   * Look up an attribute with the specified key that has the form:
   *
   *   key_name{}("string data")
   *
   * and extract the 
   */
  std::string get_string(key k) const;

private:
  std::unordered_map<key, std::shared_ptr<KORECompositePattern>> attribute_map_
      = {};
};

} // namespace kllvm

#endif
