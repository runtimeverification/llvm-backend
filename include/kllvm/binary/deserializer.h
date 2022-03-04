#ifndef AST_DESERIALIZER_H
#define AST_DESERIALIZER_H

#include <kllvm/ast/AST.h>
#include <kllvm/binary/serializer.h>

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
  std::memcpy(&ret, ptr, sizeof(T));
  return ret;
}

template <typename T, typename It>
T read(It &ptr, It end) {
  auto val = from_bytes<T>(reinterpret_cast<std::byte const *>(&*ptr));
  ptr += sizeof(T);
  return val;
}

template <typename It>
std::string read_string(It &ptr, It end) {
  switch (uint8_t(peek(ptr))) {

  case 0x01: {
    ++ptr;
    auto len = read<int32_t>(ptr, end);
    auto ret = std::string((char *)&*ptr, (char *)(&*ptr + len));

    ptr += len;
    return ret;
  }

  case 0x02: {
    ++ptr;
    auto backref = read<int32_t>(ptr, end);
    auto begin = ptr - backref;
    auto len = read<int32_t>(begin, end);

    return std::string((char *)&*begin, (char *)(&*begin + len));
  }

  default: throw std::runtime_error("Internal parsing exception");
  }
}

template <typename It>
sptr<KOREVariable> read_variable(It &ptr, It end) {
  if (peek(ptr) == header_byte<KOREVariable>) {
    ++ptr;
    return KOREVariable::Create(read_string(ptr, end));
  }

  return nullptr;
}

template <typename It>
ptr<KORESymbol>
read_symbol(It &ptr, It end, std::vector<sptr<KORESort>> &sort_stack) {
  auto arity = read<int16_t>(ptr, end);

  auto name = read_string(ptr, end);
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
sptr<KORESort>
read_composite_sort(It &ptr, It end, std::vector<sptr<KORESort>> &sort_stack) {
  auto arity = read<int16_t>(ptr, end);
  auto new_sort = KORECompositeSort::Create(read_string(ptr, end));

  for (auto i = sort_stack.size() - arity; i < sort_stack.size(); ++i) {
    new_sort->addArgument(sort_stack[i]);
  }

  for (auto i = 0; i < arity; ++i) {
    sort_stack.pop_back();
  }

  return new_sort;
}

template <typename It>
sptr<KOREPattern> read_v2(It &ptr, It end) {
  auto term_stack = std::vector<sptr<KOREPattern>>{};
  auto sort_stack = std::vector<sptr<KORESort>>{};
  auto symbol = kllvm::ptr<KORESymbol>{};

  while (ptr < end) {
    switch (peek(ptr)) {

    case header_byte<KOREStringPattern>:
      ++ptr;
      term_stack.push_back(KOREStringPattern::Create(read_string(ptr, end)));
      break;

    case header_byte<KORECompositePattern>: {
      ++ptr;
      auto new_pattern = KORECompositePattern::Create(std::move(symbol));
      symbol = nullptr;

      auto arity = read<int16_t>(ptr, end);
      for (auto i = term_stack.size() - arity; i < term_stack.size(); ++i) {
        new_pattern->addArgument(term_stack[i]);
      }

      for (auto i = 0; i < arity; ++i) {
        term_stack.pop_back();
      }
      term_stack.emplace_back(new_pattern.release());
      break;
    }

    case header_byte<KORESymbol>: {
      ++ptr;
      symbol = read_symbol(ptr, end, sort_stack);
      break;
    }

    case header_byte<KORESortVariable>: {
      ++ptr;

      auto name = read_string(ptr, end);
      sort_stack.push_back(KORESortVariable::Create(name));
      break;
    }

    case header_byte<KORECompositeSort>: {
      ++ptr;
      sort_stack.push_back(read_composite_sort(ptr, end, sort_stack));
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

  // When we end up with multiple versions of the format, we'll need to dispatch
  // on these version components. For now, just skip over the values and ignore
  // them.
  /* auto v_major = */ detail::read<int16_t>(begin, end);
  /* auto v_minor = */ detail::read<int16_t>(begin, end);
  /* auto v_patch = */ detail::read<int16_t>(begin, end);

  return detail::read_v2(begin, end);
}

bool has_binary_kore_header(std::string const &filename);
sptr<KOREPattern> deserialize_pattern(std::string const &filename);

} // namespace kllvm

#endif
