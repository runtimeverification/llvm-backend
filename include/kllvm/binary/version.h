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
  constexpr binary_version(int16_t maj, int16_t min, int16_t patch)
      : major(maj)
      , minor(min)
      , patch(patch) { }

  int16_t major;
  int16_t minor;
  int16_t patch;
};

constexpr bool operator==(binary_version a, binary_version b) {
  return std::tie(a.major, a.minor, a.patch)
         == std::tie(b.major, b.minor, b.patch);
}

constexpr bool operator!=(binary_version a, binary_version b) {
  return !(a == b);
}

/**
 * Two versions are compatible if they have identical major and minor
 * components; they may differ in the patch component.
 */
constexpr bool are_compatible(binary_version a, binary_version b) {
  return std::tie(a.major, a.minor) == std::tie(b.major, b.minor);
}

} // namespace kllvm

#endif
