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

sptr<KOREPattern> deserialize_pattern(std::string const &filename) {
  auto data = file_contents(filename);
  return deserialize_pattern(data.begin(), data.end());
}

} // namespace kllvm
