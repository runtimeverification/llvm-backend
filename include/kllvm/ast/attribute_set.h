#ifndef KLLVM_ATTRIBUTES_H
#define KLLVM_ATTRIBUTES_H

#include <memory>
#include <optional>
#include <string>
#include <map>

namespace kllvm {

class kore_composite_pattern;

/**
 * Type-safe wrapper around a set of KORE attribute patterns.
 *
 * Any valid KORE pattern can be added as an attribute, but retrieval of
 * specific attribute keys is kept behind the `key` enum in order to limit the
 * use of string-typing.
 *
 * There are two escape hatches from the type-safe interface that should be used
 * with caution:
 * - Iterating over the full set of attributes stored with `.begin()` and
 *   `.end()`; for example for pretty-printing.
 * - Access to the underlying attribute storage, for use at API boundaries.
 *
 * The core backend code should always use the type-safe interface; if a new
 * attribute needs to be processed then it should be added to the whitelist
 * enum.
 */
class attribute_set {
public:
  using storage_t = std::map<
      std::string, std::shared_ptr<kore_composite_pattern>>;

  enum class key {
    Alias,
    AliasRec,
    Anywhere,
    Assoc,
    Binder,
    Bracket,
    Ceil,
    Colors,
    Comm,
    Concat,
    Constructor,
    Element,
    Format,
    FreshGenerator,
    Function,
    Functional,
    Hook,
    Idem,
    Label,
    Left,
    Location,
    Macro,
    MacroRec,
    Nat,
    NonExecutable,
    Priorities,
    Priority,
    Right,
    Simplification,
    SortInjection,
    Source,
    Subsort,
    SymbolOverload,
    Terminals,
    Total,
    Unit,
  };

  attribute_set() = default;

  /**
   * Accepts a composite pattern of the form:
   *
   *   key_name{}(args, ...)
   *
   * Any attribute will be accepted here; if the name of the key is one of the
   * known attributes for the backend, then a key is returned. Otherwise (for an
   * unknown attribute), `std::nullopt` is returned.
   */
  std::optional<attribute_set::key>
  add(std::shared_ptr<kore_composite_pattern> att);

  /**
   * Returns true if there is any attribute with the given key; the arguments of
   * that attribute are not considered.
   */
  [[nodiscard]] bool contains(key k) const;

  /**
   * Look up the attribute pattern with the specified key.
   *
   * This lookup is unchecked; use `.contains(k)` first if the attribute may not
   * be present.
   */
  [[nodiscard]] std::shared_ptr<kore_composite_pattern> const &get(key k) const;

  /**
   * Look up an attribute with the specified key that has the form:
   *
   *   key_name{}("string data")
   *
   * and extract the string data in the pattern's argument.
   */
  [[nodiscard]] std::string get_string(key k) const;

  /**
   * Get the underlying attribute table; this table will allow access to
   * attributes that are not statically whitelisted by the backend, and so
   * should only be used at library boundaries (e.g. in bindings code).
   */
  [[nodiscard]] storage_t const &underlying() const;

  /**
   * Support for iterating over the stored attributes.
   *
   * Code that uses this feature should not dispatch on specific attribute names
   * when iterating; the type-safe interface should be used to do so instead.
   */
  [[nodiscard]] storage_t::const_iterator begin() const;
  [[nodiscard]] storage_t::const_iterator end() const;

private:
  storage_t attribute_map_ = {};
};

} // namespace kllvm

#endif
