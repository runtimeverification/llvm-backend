#include <kllvm/ast/AST.h>
#include <kllvm/binary/deserializer.h>
#include <kllvm/binary/serializer.h>

#include <algorithm>
#include <array>
#include <fstream>
#include <string>

namespace kllvm {

std::string file_contents(std::string const &fn, int max_bytes) {
  auto ifs = std::ifstream(fn, std::ios_base::binary);
  auto ret = std::string{};

  if (max_bytes == -1) {
    ifs.seekg(0, std::ios::end);
    max_bytes = ifs.tellg();
  }

  ret.resize(max_bytes);
  ifs.seekg(0, std::ios::beg);
  ifs.read(ret.data(), max_bytes);

  return ret;
}

bool has_binary_kore_header(std::string const &filename) {
  auto const &reference = serializer::magic_header;

  auto first_bytes = file_contents(filename, reference.size());
  return std::equal(reference.begin(), reference.end(), first_bytes.begin());
}

sptr<kore_pattern> deserialize_pattern(std::string const &filename) {
  auto data = file_contents(filename);
  return deserialize_pattern(data.begin(), data.end());
}

// NOLINTNEXTLINE(*-cognitive-complexity)
kore_header::kore_header(FILE *in) {
  // NOLINTNEXTLINE(misc-redundant-expression)
  if (fgetc(in) != 0x7f || fgetc(in) != 'K' || fgetc(in) != 'R'
      || fgetc(in) != '2') {
    throw std::runtime_error("invalid magic");
  }
  std::array<uint32_t, 4> num_entries{};
  if (fread(num_entries.data(), sizeof(uint32_t), 4, in) != 4) {
    throw std::runtime_error("invalid table header");
  }
  uint32_t version = num_entries[0];
  uint32_t nstrings = num_entries[1];
  uint32_t nsorts = num_entries[2];
  uint32_t nsymbols = num_entries[3];

  if (version != 1) {
    throw std::runtime_error("invalid binary version");
  }

  std::vector<std::string> strings;
  strings.reserve(nstrings);

  for (uint32_t i = 0; i < nstrings; ++i) {
    uint32_t len = 0;
    if (fread(&len, sizeof(uint32_t), 1, in) != 1) {
      throw std::runtime_error("invalid string table length");
    }
    std::string str;
    str.resize(len);
    if (fread(str.data(), 1, len, in) != len) {
      throw std::runtime_error("invalid string table entry");
    }
    fgetc(in);
    strings.push_back(str);
  }

  std::vector<sptr<kore_sort>> sorts;
  sorts.reserve(nsorts);

  for (uint32_t i = 0; i < nsorts; ++i) {
    uint32_t offset = 0;
    if (fread(&offset, sizeof(uint32_t), 1, in) != 1) {
      throw std::runtime_error("invalid string table offset in sort table");
    }
    uint8_t nparams = fgetc(in);
    auto sort = kore_composite_sort::create(strings[offset]);
    for (uint8_t j = 0; j < nparams; j++) {
      uint32_t param_offset = 0;
      if (fread(&param_offset, sizeof(uint32_t), 1, in) != 1
          || param_offset >= i) {
        throw std::runtime_error("invalid sort table offset in sort table");
      }
      sort->add_argument(sorts[param_offset]);
    }
    sorts.push_back(sort);
  }

  arities_.reserve(nsymbols);
  symbols_.reserve(nsymbols);

  for (uint32_t i = 0; i < nsymbols; ++i) {
    uint32_t offset = 0;
    if (fread(&offset, sizeof(uint32_t), 1, in) != 1) {
      throw std::runtime_error("invalid string table offset in symbol table");
    }
    uint8_t nparams = fgetc(in);
    uint8_t arity = fgetc(in);
    auto symbol = kore_symbol::create(strings[offset]);
    for (uint8_t j = 0; j < nparams; j++) {
      uint32_t param_offset = 0;
      if (fread(&param_offset, sizeof(uint32_t), 1, in) != 1) {
        throw std::runtime_error("invalid sort table offset in symbol table");
      }
      symbol->add_formal_argument(sorts[param_offset]);
    }
    symbols_.push_back(std::move(symbol));
    arities_.push_back(arity);
  }
}

namespace detail {

sptr<kore_pattern> read_v2(
    proof_trace_buffer &buffer, kore_header const &header,
    uint64_t &pattern_len) {
  switch (buffer.read()) {
  case 0: {
    uint64_t len;
    if (!buffer.read_uint64(len)) {
      throw std::runtime_error("invalid length");
    }
    std::string str;
    if (!buffer.read_string(str, len)) {
      throw std::runtime_error("invalid string data");
    }
    buffer.read();
    pattern_len += 2 + sizeof(len) + len;
    return kore_string_pattern::create(str);
  }
  case 1: {
    uint32_t offset;
    if (!buffer.read_uint32(offset)) {
      throw std::runtime_error("invalid offset");
    }
    auto arity = header.get_arity(offset);
    // TODO: we need to check if this PR is an `inj` symbol and adjust the
    // second sort parameter of the symbol to be equal to the sort of the
    // current pattern.
    auto symbol = header.get_symbol(offset);
    auto new_pattern = kore_composite_pattern::create(symbol);
    for (auto i = 0; i < arity; ++i) {
      auto child = read_v2(buffer, header, pattern_len);
      new_pattern->add_argument(child);
    }
    return new_pattern;
  }
  default: throw std::runtime_error("Bad term");
  }
}

} // namespace detail

} // namespace kllvm
