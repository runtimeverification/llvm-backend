#ifndef AST_SERIALIZER_H
#define AST_SERIALIZER_H

#include <kllvm/ast/AST.h>
#include <kllvm/binary/version.h>

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <vector>

namespace kllvm {

namespace detail {

bool is_big_endian();

} // namespace detail

/**
 * A serializer provides a thin abstraction over a byte buffer and string
 * interning table.
 */
class serializer {
public:
  enum flags {
    NONE = 0,
    DropHeader = 1,
    DropArity = 2,
    DropBoth = 3,
    NoIntern = 4,
  };

  static constexpr auto magic_header = std::array{'\x7f', 'K', 'O', 'R', 'E'};
  static constexpr auto version = binary_version(1, 2, 0);

  serializer();
  serializer(flags f);

  /**
   * Emit a single byte or sequence of bytes to the output buffer.
   *
   * Includes an overload to emit the underlying bytes of an object of
   * fundamental type. Any more complex types should be decomposed before being
   * written to the buffer.
   */
  void emit(char b);

  template <
      typename T,
      typename = std::enable_if_t<std::is_fundamental_v<std::decay_t<T>>>>
  void emit(T val);

  /**
   * Emit a string to the output buffer, using the interning table to either
   * emit the contents directly, or to emit a backreference to a previous copy
   * of the same string in the buffer.
   */
  void emit_string(std::string const &s);

  /**
   * Emit a variable-length length field to the buffer as a sequence of
   * continued bytes.
   *
   * Returns the number of bytes emitted.
   */
  int emit_length(uint64_t len);

  /**
   * Set the previously-emitted bytes following the header to reflect the actual
   * number of bytes currently contained in the buffer.
   */
  void correct_emitted_size();

  std::string const &data() { return buffer_; }

  /**
   * Return a copy of the bytes currently stored by this serializer as a string,
   * for compatibility with interfaces that don't deal with vectors of bytes.
   */
  [[nodiscard]] std::string byte_string() const;

  /**
   * Reset the state of the serializer back to its newly-constructed state, with
   * only the KORE header and version number in its buffer.
   */
  void reset();

  /**
   * Call when recursing into child nodes of a composite pattern so that only
   * the topmost arity is dropped.
   */
  void reset_arity_flag();
  [[nodiscard]] bool use_arity() const { return use_arity_; }

private:
  bool use_header_;
  bool use_arity_;
  bool use_intern_;

  std::string buffer_;
  char direct_string_prefix_;
  char backref_string_prefix_;

  uint64_t next_idx_;
  std::unordered_map<std::string, uint64_t> intern_table_;

  /**
   * Emit the standard \xf7KORE prefix and version number to the buffer.
   */
  void emit_header_and_version();

  /**
   * Emit 8 zero bytes to the buffer; these bytes can optionally later be
   * mutated to encode the actual size of a serialized pattern.
   */
  void emit_zero_size();

  /**
   * Emit a string directly to the output buffer and update the interning table,
   * regardless of whether the string has already been interned or not.
   */
  void emit_direct_string(std::string const &s);

  /**
   * Calculate the number of continued bytes required to serialize this value as
   * a length field, without actually doing so.
   */
  static int required_chunks(uint64_t len);
};

template <typename T, typename>
void serializer::emit(T val) {
  buffer_.append(reinterpret_cast<char *>(&val), sizeof(T));
  next_idx_ += sizeof(T);
}

void emit_kore_rich_header(std::ostream &os, kore_definition *definition);

} // namespace kllvm

#endif
