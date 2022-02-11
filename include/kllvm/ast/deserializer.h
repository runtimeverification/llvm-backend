#ifndef AST_DESERIALIZER_H
#define AST_DESERIALIZER_H

#include <kllvm/ast/AST.h>

#include <cstddef>
#include <cstring>
#include <vector>

#include <iostream>

namespace kllvm {

namespace detail {

template <typename T>
constexpr T from_bytes(std::byte const *ptr) {
  auto ret = T{};
  std::memcpy(&ret, ptr, sizeof(T));
  return ret;
}

template <typename T, typename It>
T read(It &ptr) {
  auto val = from_bytes<T>(&*ptr);
  ptr += sizeof(T);
  return val;
}

template <typename It>
std::string read_string(It &ptr) {
  switch (uint8_t(*ptr)) {

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

  default: throw std::runtime_error("Internal parsing exception");
  }
}

template <typename It>
sptr<KOREVariable> read_variable(It &ptr) {
  if (*ptr == header_byte<KOREVariable>) {
    ++ptr;
    return KOREVariable::Create(read_string(ptr));
  }

  return nullptr;
}

template <typename It>
sptr<KORESort> read_sort(It &ptr) {
  if (*ptr == header_byte<KORESortVariable>) {
    ++ptr;
    return KORESortVariable::Create(read_string(ptr));
  } else if (*ptr == header_byte<KORECompositeSortStart>) {
    ++ptr;

    auto args = std::vector<sptr<KORESort>>{};

    while (*ptr != header_byte<KORECompositeSort>) {
      args.push_back(read_sort(ptr));
    }

    ++ptr;

    auto ret = KORECompositeSort::Create(read_string(ptr));
    for (auto const &arg : args) {
      ret->addArgument(arg);
    }

    return ret;
  }

  return nullptr;
}

template <typename It>
ptr<KORESymbol> read_symbol(It &ptr) {
  auto args = std::vector<sptr<KORESort>>{};
  auto formals = std::vector<sptr<KORESort>>{};
  auto return_sort = sptr<KORESort>{nullptr};

  while (*ptr != header_byte<KORESymbolArguments>) {
    args.push_back(read_sort(ptr));
  }
  ++ptr;

  while (*ptr != header_byte<KORESymbolFormals>) {
    formals.push_back(read_sort(ptr));
  }
  ++ptr;

  if (*ptr != header_byte<KORESymbolReturn>) {
    return_sort = read_sort(ptr);
  }
  ++ptr;

  ++ptr;
  auto name = read_string(ptr);

  auto ret = KORESymbol::Create(name);
  for (auto const &arg : args) {
    ret->addArgument(arg);
  }

  for (auto const &arg : formals) {
    ret->addFormalArgument(arg);
  }

  if (return_sort) {
    ret->addSort(return_sort);
  }

  return ret;
}

template <typename It>
sptr<KOREPattern> read_pattern(It &ptr) {
  if (*ptr == header_byte<KOREStringPattern>) {
    ++ptr;
    return KOREStringPattern::Create(read_string(ptr));
  } else if (*ptr == header_byte<KOREVariablePattern>) {
    ++ptr;
    auto var = read_variable(ptr);
    auto sort = read_sort(ptr);

    return KOREVariablePattern::Create(var->getName(), sort);
  } else if (*ptr == header_byte<KORECompositePatternStart>) {
    ++ptr;

    auto args = std::vector<sptr<KOREPattern>>{};

    while (*ptr != header_byte<KORECompositePattern>) {
      args.push_back(read_pattern(ptr));
    }

    ++ptr;

    auto ctor = read_symbol(ptr);
    auto ret = KORECompositePattern::Create(std::move(ctor));

    for (auto const &arg : args) {
      ret->addArgument(arg);
    }

    return ret;
  }

  return nullptr;
}

} // namespace detail

template <typename It>
sptr<KOREPattern> deserialize_pattern(It begin, It end) {
  return detail::read_pattern(begin);
}

} // namespace kllvm

#endif
