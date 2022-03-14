#include <kllvm/binary/serializer.h>

#include <cassert>
#include <iostream>
#include <limits>

namespace kllvm {

namespace detail {

bool is_big_endian() {
  uint32_t i = 1;
  uint8_t *c = reinterpret_cast<uint8_t *>(&i);
  return *c == 0x00;
}

} // namespace detail

serializer::serializer()
    : buffer_{}
    , direct_string_prefix_{0x01}
    , backref_string_prefix_{0x02}
    , next_idx_(0)
    , intern_table_{} {
  emit_header_and_version();
}

void serializer::reset() {
  buffer_.clear();
  next_idx_ = 0;
  intern_table_.clear();
  emit_header_and_version();
}

void serializer::emit_header_and_version() {
  for (auto b : magic_header) {
    emit(std::byte(b));
  }

  for (auto version_part : version) {
    emit(int16_t(version_part));
  }
}

void serializer::emit(std::byte b) {
  buffer_.push_back(b);
  next_idx_++;
}

void serializer::emit_string(std::string const &s) {
  if (intern_table_.find(s) == intern_table_.end()) {
    emit_direct_string(s);
  } else {
    emit(backref_string_prefix_);

    uint64_t previous = intern_table_.at(s);

    // This iteration makes sure that the diff field correctly encodes the
    // offset relative to _itself_ as well as to the prefix.
    uint64_t base_diff = next_idx_ - previous;
    auto chunks = required_chunks(base_diff + required_chunks(base_diff));

    emit_length(base_diff + chunks);
  }
}

int serializer::emit_length(uint64_t len) {
  auto emitted = 0;

  do {
    uint8_t chunk = len & 0x7F;
    len >>= 7;

    if (len > 0) {
      chunk = chunk | 0x80;
    }

    emit(chunk);
    emitted++;
  } while (len > 0);

  return emitted;
}

int serializer::required_chunks(uint64_t len) const {
  auto ret = 0;
  do {
    ++ret;
  } while (len >>= 7);
  return ret;
}

void serializer::emit_direct_string(std::string const &s) {
  emit(direct_string_prefix_);

  intern_table_[s] = next_idx_;

  emit_length(s.size());

  for (auto c : s) {
    emit(std::byte(c));
  }
}

} // namespace kllvm
