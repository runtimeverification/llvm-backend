#ifndef AST_SERIALIZER_H
#define AST_SERIALIZER_H

#include <array>
#include <cstddef>
#include <cstring>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <vector>

namespace kllvm {

namespace detail {

bool is_big_endian();

template <typename T>
std::array<std::byte, sizeof(T)> to_bytes(T val) {
  auto bytes = std::array<std::byte, sizeof(T)>{};

  if (is_big_endian()) {
    for (auto i = 0; i < sizeof(T); ++i) {
      bytes[i] = reinterpret_cast<std::byte *>(&val)[sizeof(T) - i - 1];
    }
  } else {
    std::memcpy(bytes.data(), &val, sizeof(T));
  }

  return bytes;
}

} // namespace detail

/**
 * A serializer provides a thin abstraction over a byte buffer and string
 * interning table.
 */
class serializer {
public:
  static constexpr auto magic_header = std::array{'\x7f', 'K', 'O', 'R', 'E'};
  static constexpr auto version = std::array{1, 0, 0};

  serializer();

  /**
   * Emit a single byte or sequence of bytes to the output buffer.
   *
   * Includes an overload to emit the underlying bytes of an object of
   * fundamental type. Any more complex types should be decomposed before being
   * written to the buffer.
   */
  void emit(std::byte b);

  template <typename It>
  void emit(It begin, It end);

  template <
      typename T,
      typename = std::enable_if_t<std::is_fundamental_v<std::decay_t<T>>>>
  void emit(T t);

  /**
   * Emit a string to the output buffer, using the interning table to either
   * emit the contents directly, or to emit a backreference to a previous copy
   * of the same string in the buffer.
   */
  void emit_string(std::string const &s);

  std::vector<std::byte> const &data() { return buffer_; }

private:
  std::vector<std::byte> buffer_;
  std::byte direct_string_prefix_;
  std::byte backref_string_prefix_;

  int64_t next_idx_;
  std::unordered_map<std::string, int64_t> intern_table_;

  /**
   * Emit a string directly to the output buffer and update the interning table,
   * regardless of whether the string has already been interned or not.
   */
  void emit_direct_string(std::string const &s);
};

template <typename It>
void serializer::emit(It begin, It end) {
  for (auto it = begin; it != end; ++it) {
    emit(*it);
  }
}

template <typename T, typename>
void serializer::emit(T val) {
  auto bytes = detail::to_bytes(val);
  emit(bytes.begin(), bytes.end());
}

} // namespace kllvm

#endif
