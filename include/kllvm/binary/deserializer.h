#ifndef AST_DESERIALIZER_H
#define AST_DESERIALIZER_H

#include <kllvm/ast/AST.h>
#include <kllvm/binary/serializer.h>
#include <kllvm/binary/version.h>

#include <cstddef>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <iostream>
#include <vector>

namespace kllvm {

class kore_header {
private:
  std::vector<uint8_t> arities_;
  std::vector<ptr<kore_symbol>> symbols_;

public:
  kore_header(FILE *in);
  static std::unique_ptr<kore_header> create(std::string const &path) {
    FILE *f = fopen(path.c_str(), "rb");
    auto *result = new kore_header(f);
    fclose(f);
    return std::unique_ptr<kore_header>(result);
  }

  [[nodiscard]] uint8_t get_arity(uint32_t offset) const {
    return arities_[offset];
  };
  [[nodiscard]] kore_symbol *get_symbol(uint32_t offset) const {
    return symbols_[offset].get();
  };
};

class proof_trace_buffer {
public:
  virtual bool read(void *ptr, size_t len) = 0;
  virtual int read(void) = 0;
  virtual bool has_word(void) = 0;
  virtual bool eof(void) = 0;
  virtual int peek(void) = 0;
  virtual uint64_t peek_word(void) = 0;
  bool check_word(uint64_t w) {
    uint64_t next = 0;
    if (read_uint64(next)) {
      return next == w;
    }
    return false;
  }
  virtual bool read_uint32(uint32_t &i) = 0;
  virtual bool read_uint64(uint64_t &i) = 0;
  virtual bool read_string(std::string &str) = 0;
  virtual bool read_string(std::string &str, size_t len) = 0;
  bool read_bool(bool &b) {
    if (eof()) {
      return false;
    }
    b = read();
    return true;
  }
};

class proof_trace_memory_buffer : public proof_trace_buffer {
private:
  char const *ptr_;
  char const *const end_;

public:
  proof_trace_memory_buffer(char const *ptr, char const *end)
      : ptr_(ptr)
      , end_(end) { }

  bool read(void *out, size_t len) override {
    if (end_ - ptr_ < len) {
      return false;
    }
    memcpy(out, ptr_, len);
    ptr_ += len;
    return true;
  }

  int read(void) override {
    if (ptr_ == end_) {
      return -1;
    }
    return *(ptr_++);
  }

  bool has_word(void) override { return end_ - ptr_ >= 8; }

  bool eof(void) override { return ptr_ == end_; }

  int peek(void) override {
    if (eof()) {
      return -1;
    }
    return *ptr_;
  }

  uint64_t peek_word(void) override { return *(uint64_t *)ptr_; }

  bool read_uint32(uint32_t &i) override {
    if (end_ - ptr_ < sizeof(uint32_t)) {
      return false;
    }
    i = *(uint32_t *)ptr_;
    ptr_ += sizeof(uint32_t);
    return true;
  }

  bool read_uint64(uint64_t &i) override {
    if (end_ - ptr_ < sizeof(uint64_t)) {
      return false;
    }
    i = *(uint64_t *)ptr_;
    ptr_ += sizeof(uint64_t);
    return true;
  }

  bool read_string(std::string &str) override {
    size_t len = strnlen(ptr_, end_ - ptr_);
    if (len == end_ - ptr_) {
      return false;
    }
    str.resize(len);
    memcpy(str.data(), ptr_, len);
    ptr_ += len + 1;
    return true;
  }

  bool read_string(std::string &str, size_t len) override {
    if (end_ - ptr_ < len) {
      return false;
    }
    str.resize(len);
    memcpy(str.data(), ptr_, len);
    ptr_ += len;
    return true;
  }
};

class proof_trace_file_buffer : public proof_trace_buffer {
private:
  std::ifstream &file_;

public:
  proof_trace_file_buffer(std::ifstream &file)
      : file_(file) { }

  bool read(void *ptr, size_t len) override {
    file_.read((char *)ptr, len);
    return !file_.fail();
  }

  int read(void) override { return file_.get(); }

  bool has_word(void) override {
    if (eof()) {
      return false;
    }
    std::streampos pos = file_.tellg();
    file_.seekg(0, std::ios::end);
    std::streamoff off = file_.tellg() - pos;
    file_.seekg(pos);
    return off >= 8;
  }

  bool eof(void) override { return file_.eof() || file_.peek() == EOF; }

  int peek(void) override { return file_.peek(); }

  uint64_t peek_word(void) override {
    uint64_t word;
    file_.read((char *)&word, sizeof(word));
    file_.seekg(-sizeof(word), std::ios::cur);
    return word;
  }

  bool read_uint32(uint32_t &i) override {
    file_.read((char *)&i, sizeof(i));
    return !file_.fail();
  }

  bool read_uint64(uint64_t &i) override {
    file_.read((char *)&i, sizeof(i));
    return !file_.fail();
  }

  bool read_string(std::string &str) override {
    std::getline(file_, str, '\0');
    return !file_.fail() && !file_.eof();
  }

  bool read_string(std::string &str, size_t len) override {
    str.resize(len);
    file_.read(str.data(), len);
    return !file_.fail();
  }
};

namespace detail {

template <typename It>
char peek(It const &it) {
  return *it;
}

template <typename T>
constexpr T from_bytes(char const *ptr) {
  auto ret = T{};

  if (is_big_endian()) {
    for (auto i = 0; i < sizeof(T); ++i) {
      reinterpret_cast<char *>(&ret)[sizeof(T) - i - 1] = ptr[i];
    }
  } else {
    std::memcpy(&ret, ptr, sizeof(T));
  }

  return ret;
}

template <typename T, typename It>
T read(It &ptr, It end) {
  assert(ptr + sizeof(T) <= end);

  auto val = from_bytes<T>(reinterpret_cast<char const *>(&*ptr));
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

  return 0U;
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
  }
  uint64_t ret = 0;
  auto should_continue = true;
  auto steps = 0;

  while (should_continue) {
    assert(ptr != end && "Invalid variable-length field");
    assert(steps < 9 && "No terminating byte in variable-length field");

    auto chunk = peek(ptr);
    auto cont_bit = uint8_t{0x80};
    should_continue = static_cast<bool>(chunk & cont_bit);

    chunk = chunk & ~cont_bit;
    ret = ret | (uint64_t(chunk) << (7 * steps));

    ++steps;
    ++ptr;
  }

  return ret;
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

    return {(char *)&*begin, (char *)(&*begin + len)};
  }

  default: throw std::runtime_error("Internal parsing exception");
  }
}

template <typename It>
sptr<kore_variable> read_variable(It &ptr, It end, binary_version version) {
  if (peek(ptr) == header_byte<kore_variable>) {
    ++ptr;
    return kore_variable::create(read_string(ptr, end, version));
  }

  return nullptr;
}

template <typename It>
ptr<kore_symbol> read_symbol(
    It &ptr, It end, std::vector<sptr<kore_sort>> &sort_stack,
    binary_version version) {
  auto arity = read_length(ptr, end, version, 2);

  auto name = read_string(ptr, end, version);
  auto symbol = kore_symbol::create(name);

  auto start_idx = sort_stack.size() - arity;
  for (auto i = start_idx; i < sort_stack.size(); ++i) {
    symbol->add_formal_argument(sort_stack[i]);
  }

  for (auto i = 0; i < arity; ++i) {
    sort_stack.pop_back();
  }

  return symbol;
}

template <typename It>
sptr<kore_sort> read_composite_sort(
    It &ptr, It end, std::vector<sptr<kore_sort>> &sort_stack,
    binary_version version) {
  auto arity = read_length(ptr, end, version, 2);
  auto new_sort = kore_composite_sort::create(read_string(ptr, end, version));

  for (auto i = sort_stack.size() - arity; i < sort_stack.size(); ++i) {
    new_sort->add_argument(sort_stack[i]);
  }

  for (auto i = 0; i < arity; ++i) {
    sort_stack.pop_back();
  }

  return new_sort;
}

template <typename It>
sptr<kore_pattern> read(It &ptr, It end, binary_version version) {
  auto term_stack = std::vector<sptr<kore_pattern>>{};
  auto sort_stack = std::vector<sptr<kore_sort>>{};
  auto symbol = kllvm::ptr<kore_symbol>{};

  while (ptr < end) {
    switch (peek(ptr)) {

    case header_byte<kore_string_pattern>:
      ++ptr;
      term_stack.push_back(
          kore_string_pattern::create(read_string(ptr, end, version)));
      break;

    case header_byte<kore_composite_pattern>: {
      ++ptr;
      auto new_pattern = kore_composite_pattern::create(std::move(symbol));
      symbol = nullptr;

      auto arity = read_length(ptr, end, version, 2);
      for (auto i = term_stack.size() - arity; i < term_stack.size(); ++i) {
        new_pattern->add_argument(term_stack[i]);
      }

      for (auto i = 0; i < arity; ++i) {
        term_stack.pop_back();
      }
      term_stack.emplace_back(new_pattern.release());
      break;
    }

    case header_byte<kore_variable_pattern>: {
      ++ptr;
      auto name = read_variable(ptr, end, version);
      auto sort = sort_stack.back();
      sort_stack.pop_back();

      term_stack.push_back(
          kore_variable_pattern::create(name->get_name(), sort));
      break;
    }

    case header_byte<kore_symbol>: {
      ++ptr;
      symbol = read_symbol(ptr, end, sort_stack, version);
      break;
    }

    case header_byte<kore_sort_variable>: {
      ++ptr;

      auto name = read_string(ptr, end, version);
      sort_stack.push_back(kore_sort_variable::create(name));
      break;
    }

    case header_byte<kore_composite_sort>: {
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

sptr<kore_pattern> read_v2(
    proof_trace_buffer &buffer, kore_header const &header,
    uint64_t &pattern_len);

} // namespace detail

std::string file_contents(std::string const &fn, int max_bytes = -1);

template <typename It>
sptr<kore_pattern>
deserialize_pattern(It begin, It end, bool should_strip_raw_term = true) {
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

  auto result = detail::read(begin, end, version);

  if (should_strip_raw_term) {
    return strip_raw_term(result);
  }

  return result;
}

bool has_binary_kore_header(std::string const &filename);
sptr<kore_pattern> deserialize_pattern(std::string const &filename);

} // namespace kllvm

#endif
