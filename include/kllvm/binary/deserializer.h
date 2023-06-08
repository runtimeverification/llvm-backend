#ifndef AST_DESERIALIZER_H
#define AST_DESERIALIZER_H

#include <kllvm/ast/AST.h>
#include <kllvm/binary/serializer.h>
#include <kllvm/binary/version.h>

#include <cstddef>
#include <cstring>
#include <vector>

#include <iostream>

namespace kllvm {

namespace detail {

template <typename It>
std::byte peek(It const &it) {
  return std::byte(*it);
}

template <typename T>
constexpr T from_bytes(std::byte const *ptr) {
  auto ret = T{};

  if (is_big_endian()) {
    for (auto i = 0; i < sizeof(T); ++i) {
      reinterpret_cast<std::byte *>(&ret)[sizeof(T) - i - 1] = ptr[i];
    }
  } else {
    std::memcpy(&ret, ptr, sizeof(T));
  }

  return ret;
}

template <typename T, typename It>
T read(It &ptr, It end) {
  assert(ptr + sizeof(T) <= end);

  auto val = from_bytes<T>(reinterpret_cast<std::byte const *>(&*ptr));
  ptr += sizeof(T);
  return val;
}

template <typename It>
binary_version read_version(It &ptr, It end) {
  auto v_major = detail::read<int16_t>(ptr, end);
  auto v_minor = detail::read<int16_t>(ptr, end);
  auto v_patch = detail::read<int16_t>(ptr, end);
  return binary_version(v_major, v_minor, v_patch);
}

template <typename It>
uint64_t read_pattern_size_unchecked(It &ptr, It end) {
  return detail::read<uint64_t>(ptr, end);
}

template <typename It>
uint64_t read_pattern_size(It &ptr, It end, binary_version version) {
  if (version >= binary_version(1, 2, 0)) {
    return read_pattern_size_unchecked(ptr, end);
  }

  return 0u;
}

template <typename It>
uint64_t read_length(It &ptr, It end, binary_version version, int v1_bytes) {
  if (version.compatible(binary_version(1, 0, 0))) {
    uint64_t ret = 0;

    for (auto i = 0; i < v1_bytes; ++i) {
      ret <<= 8;
      ret = ret | read<uint8_t>(ptr, end);
    }

    return ret;
  } else {
    uint64_t ret = 0;
    auto should_continue = true;
    auto steps = 0;

    while (should_continue) {
      assert(ptr != end && "Invalid variable-length field");
      assert(steps < 9 && "No terminating byte in variable-length field");

      auto chunk = peek(ptr);
      auto cont_bit = std::byte(0x80);
      should_continue = static_cast<bool>(chunk & cont_bit);

      chunk = chunk & ~cont_bit;
      ret = ret | (uint64_t(chunk) << (7 * steps));

      ++steps;
      ++ptr;
    }

    return ret;
  }
}

template <typename It>
std::string read_string(It &ptr, It end, binary_version version) {
  switch (uint8_t(peek(ptr))) {

  case 0x01: {
    ++ptr;
    auto len = read_length(ptr, end, version, 4);
    auto ret = std::string((char *)&*ptr, (char *)(&*ptr + len));

    ptr += len;
    return ret;
  }

  case 0x02: {
    ++ptr;
    auto backref = read_length(ptr, end, version, 4);
    auto begin = ptr - backref;
    auto len = read_length(begin, end, version, 4);

    return std::string((char *)&*begin, (char *)(&*begin + len));
  }

  default: throw std::runtime_error("Internal parsing exception");
  }
}

template <typename It>
sptr<KOREVariable> read_variable(It &ptr, It end, binary_version version) {
  if (peek(ptr) == header_byte<KOREVariable>) {
    ++ptr;
    return KOREVariable::Create(read_string(ptr, end, version));
  }

  return nullptr;
}

template <typename It>
ptr<KORESymbol> read_symbol(
    It &ptr, It end, std::vector<sptr<KORESort>> &sort_stack,
    binary_version version) {
  auto arity = read_length(ptr, end, version, 2);

  auto name = read_string(ptr, end, version);
  auto symbol = KORESymbol::Create(name);

  auto start_idx = sort_stack.size() - arity;
  for (auto i = start_idx; i < sort_stack.size(); ++i) {
    symbol->addFormalArgument(sort_stack[i]);
  }

  for (auto i = 0; i < arity; ++i) {
    sort_stack.pop_back();
  }

  return symbol;
}

template <typename It>
sptr<KORESort> read_composite_sort(
    It &ptr, It end, std::vector<sptr<KORESort>> &sort_stack,
    binary_version version) {
  auto arity = read_length(ptr, end, version, 2);
  auto new_sort = KORECompositeSort::Create(read_string(ptr, end, version));

  for (auto i = sort_stack.size() - arity; i < sort_stack.size(); ++i) {
    new_sort->addArgument(sort_stack[i]);
  }

  for (auto i = 0; i < arity; ++i) {
    sort_stack.pop_back();
  }

  return new_sort;
}

template <typename It>
sptr<KOREPattern> read(It &ptr, It end, binary_version version) {
  auto term_stack = std::vector<sptr<KOREPattern>>{};
  auto sort_stack = std::vector<sptr<KORESort>>{};
  auto symbol = kllvm::ptr<KORESymbol>{};

  while (ptr < end) {
    switch (peek(ptr)) {

    case header_byte<KOREStringPattern>:
      ++ptr;
      term_stack.push_back(
          KOREStringPattern::Create(read_string(ptr, end, version)));
      break;

    case header_byte<KORECompositePattern>: {
      ++ptr;
      auto new_pattern = KORECompositePattern::Create(std::move(symbol));
      symbol = nullptr;

      auto arity = read_length(ptr, end, version, 2);
      for (auto i = term_stack.size() - arity; i < term_stack.size(); ++i) {
        new_pattern->addArgument(term_stack[i]);
      }

      for (auto i = 0; i < arity; ++i) {
        term_stack.pop_back();
      }
      term_stack.emplace_back(new_pattern.release());
      break;
    }

    case header_byte<KOREVariablePattern>: {
      ++ptr;
      auto name = read_variable(ptr, end, version);
      auto sort = sort_stack.back();
      sort_stack.pop_back();

      term_stack.push_back(KOREVariablePattern::Create(name->getName(), sort));
      break;
    }

    case header_byte<KORESymbol>: {
      ++ptr;
      symbol = read_symbol(ptr, end, sort_stack, version);
      break;
    }

    case header_byte<KORESortVariable>: {
      ++ptr;

      auto name = read_string(ptr, end, version);
      sort_stack.push_back(KORESortVariable::Create(name));
      break;
    }

    case header_byte<KORECompositeSort>: {
      ++ptr;
      sort_stack.push_back(read_composite_sort(ptr, end, sort_stack, version));
      break;
    }

    default: std::cerr << "Bad term " << int(*ptr) << '\n'; return nullptr;
    }
  }

  if (term_stack.size() != 1) {
    return nullptr;
  }

  return term_stack[0];
}

} // namespace detail

std::string file_contents(std::string const &fn, int max_bytes = -1);

template <typename It>
sptr<KOREPattern> deserialize_pattern(It begin, It end) {
  // Try to parse the file even if the magic header isn't correct; by the time
  // we're here we already know that we're trying to parse a binary KORE file.
  // The header itself gets used by the application when detecting binary vs.
  // textual.
  for (auto i = 0; i < serializer::magic_header.size(); ++i) {
    detail::read<char>(begin, end);
  }

  auto version = detail::read_version(begin, end);
  auto total_size = detail::read_pattern_size(begin, end, version);

  if (total_size > 0 && std::distance(begin, end) > total_size) {
    end = std::next(begin, total_size);
  }

  return detail::read(begin, end, version);
}

bool has_binary_kore_header(std::string const &filename);
sptr<KOREPattern> deserialize_pattern(std::string const &filename);

} // namespace kllvm

#endif
