#ifndef PROOF_TRACE_VALIDATOR_H
#define PROOF_TRACE_VALIDATOR_H

#include <kllvm/binary/deserializer.h>

#include <fmt/format.h>

#include <iostream>
#include <string>

namespace kllvm {

namespace detail {

constexpr uint64_t word(uint8_t b) {
  uint64_t w = uint64_t(b);
  for (auto i = 0; i < 7; ++i) {
    w = (w << 8u) | w;
  }
  return w;
}

} // namespace detail

class ProofTraceValidator {

private:
  unsigned depth;
  bool verbose;

  void print(std::string const &message) const {
    if (!verbose) {
      return;
    }

    for (auto i = 0; i < depth; i++) {
      std::cout << "  ";
    }
    std::cout << message << std::endl;
  }

  // Caller needs to check that there are at least 8 bytes remaining in the
  // stream before peeking
  template <typename It>
  uint64_t peek_word(It const &it) {
    return detail::from_bytes<uint64_t>(
        reinterpret_cast<std::byte const *>(&*it));
  }

  template <typename It>
  bool validate_word(It &ptr, It end, uint64_t w) {
    if (std::distance(ptr, end) < sizeof(uint64_t)) {
      return false;
    }

    return detail::read<uint64_t>(ptr, end) == w;
  }

  template <typename It>
  bool read_uint32(It &ptr, It end, uint32_t &i) {
    if (std::distance(ptr, end) < sizeof(uint32_t)) {
      return false;
    }

    i = detail::read<uint32_t>(ptr, end);
    return true;
  }

  template <typename It>
  bool read_uint64(It &ptr, It end, uint64_t &i) {
    if (std::distance(ptr, end) < sizeof(uint64_t)) {
      return false;
    }

    i = detail::read<uint64_t>(ptr, end);
    return true;
  }

  template <typename It>
  bool read_string(It &ptr, It end, std::string &str) {
    while (ptr != end) {
      char c = detail::read<char>(ptr, end);
      if (c == '\0') {
        return true;
      }
      str.push_back(c);
    }

    return false;
  }

  template <typename It>
  bool validate_ordinal(It &ptr, It end, uint64_t &ordinal) {
    return read_uint64(ptr, end, ordinal);
  }

  template <typename It>
  bool validate_arity(It &ptr, It end, uint64_t &arity) {
    return read_uint64(ptr, end, arity);
  }

  template <typename It>
  bool validate_kore_term(It &ptr, It end, uint64_t &pattern_len) {
    if (std::distance(ptr, end) < 11u) {
      return false;
    }
    if (detail::read<char>(ptr, end) != '\x7F'
        || detail::read<char>(ptr, end) != 'K'
        || detail::read<char>(ptr, end) != 'O'
        || detail::read<char>(ptr, end) != 'R'
        || detail::read<char>(ptr, end) != 'E') {
      return false;
    }
    ptr += 6;

    if (!read_uint64(ptr, end, pattern_len)) {
      return false;
    }

    if (std::distance(ptr, end) < pattern_len) {
      return false;
    }
    ptr += pattern_len;

    return true;
  }

  template <typename It>
  bool validate_name(It &ptr, It end, std::string &name) {
    return read_string(ptr, end, name);
  }

  template <typename It>
  bool validate_location(It &ptr, It end, std::string &location) {
    return read_string(ptr, end, location);
  }

  template <typename It>
  bool validate_header(It &ptr, It end) {
    if (std::distance(ptr, end) < 4u) {
      return false;
    }
    if (detail::read<char>(ptr, end) != 'H'
        || detail::read<char>(ptr, end) != 'I'
        || detail::read<char>(ptr, end) != 'N'
        || detail::read<char>(ptr, end) != 'T') {
      return false;
    }

    uint32_t version;
    if (!read_uint32(ptr, end, version)) {
      return false;
    }

    if (version != 3) {
      return false;
    }

    print(fmt::format("version: {}", version));

    return true;
  }

  template <typename It>
  bool validate_variable(It &ptr, It end) {
    std::string name;
    if (!validate_name(ptr, end, name)) {
      return false;
    }

    uint64_t pattern_len;
    if (!validate_kore_term(ptr, end, pattern_len)) {
      return false;
    }

    print(fmt::format("  {} = kore[{}]", name, pattern_len));

    return validate_word(ptr, end, detail::word(0xCC));
  }

  template <typename It>
  bool validate_hook(It &ptr, It end) {
    if (!validate_word(ptr, end, detail::word(0xAA))) {
      return false;
    }

    std::string name;
    if (!validate_name(ptr, end, name)) {
      return false;
    }

    std::string location;
    if (!validate_location(ptr, end, location)) {
      return false;
    }

    print(fmt::format("hook: {} ({})", name, location));

    while (std::distance(ptr, end) < 8u
           || peek_word(ptr) != detail::word(0xBB)) {
      if (!validate_argument(ptr, end)) {
        return false;
      }
    }

    if (!validate_word(ptr, end, detail::word(0xBB))) {
      return false;
    }

    uint64_t pattern_len;
    if (!validate_kore_term(ptr, end, pattern_len)) {
      return false;
    }

    print(fmt::format("hook result: kore[{}]", pattern_len));

    return true;
  }

  template <typename It>
  bool validate_function(It &ptr, It end) {
    if (!validate_word(ptr, end, detail::word(0xDD))) {
      return false;
    }

    std::string name;
    if (!validate_name(ptr, end, name)) {
      return false;
    }

    std::string location;
    if (!validate_location(ptr, end, location)) {
      return false;
    }

    print(fmt::format("function: {} ({})", name, location));

    while (std::distance(ptr, end) < 8u
           || peek_word(ptr) != detail::word(0x11)) {
      if (!validate_argument(ptr, end)) {
        return false;
      }
    }

    return validate_word(ptr, end, detail::word(0x11));
  }

  template <typename It>
  bool validate_config(It &ptr, It end) {
    if (!validate_word(ptr, end, detail::word(0xFF))) {
      return false;
    }

    uint64_t pattern_len;
    if (!validate_kore_term(ptr, end, pattern_len)) {
      return false;
    }

    print(fmt::format("config: kore[{}]", pattern_len));

    return validate_word(ptr, end, detail::word(0xCC));
  }

  template <typename It>
  bool validate_rule(It &ptr, It end) {
    if (!validate_word(ptr, end, detail::word(0x22))) {
      return false;
    }

    uint64_t ordinal;
    if (!validate_ordinal(ptr, end, ordinal)) {
      return false;
    }

    uint64_t arity;
    if (!validate_arity(ptr, end, arity)) {
      return false;
    }

    print(fmt::format("rule: {} {}", ordinal, arity));

    for (auto i = 0; i < arity; i++) {
      if (!validate_variable(ptr, end)) {
        return false;
      }
    }

    return true;
  }

  template <typename It>
  bool validate_side_condition(It &ptr, It end) {
    if (!validate_word(ptr, end, detail::word(0xEE))) {
      return false;
    }

    uint64_t ordinal;
    if (!validate_ordinal(ptr, end, ordinal)) {
      return false;
    }

    uint64_t arity;
    if (!validate_arity(ptr, end, arity)) {
      return false;
    }

    print(fmt::format("side condition: {} {}", ordinal, arity));

    for (auto i = 0; i < arity; i++) {
      if (!validate_variable(ptr, end)) {
        return false;
      }
    }

    return true;
  }

  template <typename It>
  bool validate_argument(It &ptr, It end) {
    depth++;

    if (std::distance(ptr, end) >= 1u
        && detail::peek(ptr) == std::byte('\x7F')) {
      uint64_t pattern_len;
      if (!validate_kore_term(ptr, end, pattern_len)) {
        return false;
      }

      print(fmt::format("arg: kore[{}]", pattern_len));
      depth--;

      return true;
    }

    if (std::distance(ptr, end) < 8u) {
      return false;
    }

    switch (peek_word(ptr)) {

    case detail::word(0xAA): {
      if (!validate_hook(ptr, end)) {
        return false;
      }
      depth--;
      return true;
    }

    case detail::word(0xDD): {
      if (!validate_function(ptr, end)) {
        return false;
      }
      depth--;
      return true;
    }

    case detail::word(0x22): {
      if (!validate_rule(ptr, end)) {
        return false;
      }
      depth--;
      return true;
    }

    default: return false;
    }
  }

  template <typename It>
  bool validate_event(It &ptr, It end) {
    if (std::distance(ptr, end) < 8u) {
      return false;
    }

    switch (peek_word(ptr)) {

    case detail::word(0xAA): return validate_hook(ptr, end);

    case detail::word(0xDD): return validate_function(ptr, end);

    case detail::word(0xFF): return validate_config(ptr, end);

    case detail::word(0x22): return validate_rule(ptr, end);

    case detail::word(0xEE): return validate_side_condition(ptr, end);

    default: return false;
    }
  }

  template <typename It>
  bool validate_trace(It &ptr, It end) {
    if (!validate_header(ptr, end)) {
      return false;
    }

    while (ptr != end) {
      if (!validate_event(ptr, end)) {
        return false;
      }
    }

    return true;
  }

public:
  ProofTraceValidator(bool _verbose);

  bool validate_proof_trace(std::string const &filename);
};

} // namespace kllvm

#endif
