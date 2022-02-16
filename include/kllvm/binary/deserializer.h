#ifndef AST_DESERIALIZER_H
#define AST_DESERIALIZER_H

#include <kllvm/ast/AST.h>

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
T read(It &ptr) {
  auto val = from_bytes<T>(reinterpret_cast<std::byte const *>(&*ptr));
  ptr += sizeof(T);
  return val;
}

template <typename It>
std::string read_string(It &ptr) {
  switch (uint8_t(peek(ptr))) {

  case 0x01: {
    ++ptr;
    auto len = read<int32_t>(ptr);
    auto ret = std::string((char *)(&*ptr), (char *)(&*(ptr + len)));
    ptr += len;
    return ret;
  }

  case 0x02: {
    ++ptr;
    auto backref = read<int32_t>(ptr);
    auto begin = ptr - backref;

    auto len = read<int32_t>(begin);
    return std::string((char *)(&*begin), (char *)(&*(begin + len)));
  }

  case 0x03: {
    ++ptr;
    auto ret = std::string((char *)(&*ptr));
    ptr += ret.size() + 1;
    return ret;
  }

  default: throw std::runtime_error("Internal parsing exception");
  }
}

template <typename It>
sptr<KOREVariable> read_variable(It &ptr) {
  if (peek(ptr) == header_byte<KOREVariable>) {
    ++ptr;
    return KOREVariable::Create(read_string(ptr));
  }

  return nullptr;
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
      term_stack.push_back(KOREStringPattern::Create(read_string(ptr)));
      break;

    case header_byte<KOREVariablePattern>: {
      ++ptr;
      assert(sort_stack.size() == 0);
      auto sort = sort_stack[0];
      sort_stack.clear();
      term_stack.push_back(
          KOREVariablePattern::Create(read_variable(ptr)->getName(), sort));
      break;
    }

    case header_byte<KORECompositePattern>: {
      ++ptr;
      auto new_pattern = KORECompositePattern::Create(std::move(symbol));
      symbol = nullptr;

      auto arity = read<int16_t>(ptr);
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
      auto args_arity = read<int16_t>(ptr);
      auto formal_arity = read<int16_t>(ptr);
      auto return_arity = read<int16_t>(ptr);

      auto name = read_string(ptr);
      symbol = KORESymbol::Create(name);

      auto total_arity = args_arity + formal_arity + return_arity;
      auto start_idx = sort_stack.size() - total_arity;

      for (auto i = 0; i < args_arity; ++i) {
        symbol->addArgument(sort_stack[start_idx + i]);
      }

      for (auto i = 0; i < formal_arity; ++i) {
        symbol->addFormalArgument(sort_stack[start_idx + args_arity + i]);
      }

      if (return_arity > 0) {
        symbol->addSort(sort_stack[sort_stack.size() - 1]);
      }

      for (auto i = 0; i < total_arity; ++i) {
        sort_stack.pop_back();
      }

      break;
    }

    case header_byte<KORECompositeSort>: {
      ++ptr;

      auto arity = read<int16_t>(ptr);
      auto new_sort = KORECompositeSort::Create(read_string(ptr));

      for (auto i = sort_stack.size() - arity; i < sort_stack.size(); ++i) {
        new_sort->addArgument(sort_stack[i]);
      }

      for (auto i = 0; i < arity; ++i) {
        sort_stack.pop_back();
      }
      sort_stack.push_back(new_sort);
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

template <typename It>
sptr<KOREPattern> deserialize_pattern(It begin, It end) {
  begin += 4;
  return detail::read_v2(begin, end);
}

} // namespace kllvm

#endif
