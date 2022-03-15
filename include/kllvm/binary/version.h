#ifndef BINARY_VERSION_H
#define BINARY_VERSION_H

#include <cstdint>
#include <tuple>

namespace kllvm {

/**
 * Represents a MAJOR.MINOR.PATCH semantic version for the KORE binary term
 * format.
 */
struct binary_version {
  constexpr binary_version(int16_t maj, int16_t min, int16_t v_patch)
      : v_major(maj)
      , v_minor(min)
      , v_patch(v_patch) { }

  int16_t v_major;
  int16_t v_minor;
  int16_t v_patch;
};

constexpr bool operator==(binary_version a, binary_version b) {
  return std::tie(a.v_major, a.v_minor, a.v_patch)
         == std::tie(b.v_major, b.v_minor, b.v_patch);
}

constexpr bool operator!=(binary_version a, binary_version b) {
  return !(a == b);
}

/**
 * Two versions are compatible if they have identical v_major and v_minor
 * components; they may differ in the v_patch component.
 */
constexpr bool are_compatible(binary_version a, binary_version b) {
  return std::tie(a.v_major, a.v_minor) == std::tie(b.v_major, b.v_minor);
}

} // namespace kllvm

#endif
