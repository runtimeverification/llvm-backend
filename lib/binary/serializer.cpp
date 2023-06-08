#include <kllvm/binary/serializer.h>
#include <kllvm/binary/version.h>

namespace kllvm {

namespace detail {

bool is_big_endian() {
  uint32_t i = 1;
  uint8_t *c = reinterpret_cast<uint8_t *>(&i);
  return *c == 0x00;
}

} // namespace detail

serializer::serializer()
    : serializer(NONE) { }

serializer::serializer(flags f)
    : use_header_(!(f & DROP_HEADER))
    , use_arity_(!(f & DROP_ARITY))
    , buffer_{}
    , direct_string_prefix_{0x01}
    , backref_string_prefix_{0x02}
    , next_idx_(0)
    , intern_table_{} {
  if (use_header_) {
    emit_header_and_version();
    emit_zero_size();
  }
}

std::string serializer::byte_string() const {
  auto *ptr = reinterpret_cast<unsigned char const *>(buffer_.data());
  return std::string(ptr, ptr + buffer_.size());
}

void serializer::reset() {
  buffer_.clear();
  next_idx_ = 0;
  intern_table_.clear();

  if (use_header_) {
    emit_header_and_version();
    emit_zero_size();
  }
}

void serializer::reset_arity_flag() {
  use_arity_ = true;
}

void serializer::emit_header_and_version() {
  for (auto b : magic_header) {
    emit(std::byte(b));
  }

  emit(version.v_major);
  emit(version.v_minor);
  emit(version.v_patch);
}

void serializer::emit_zero_size() {
  emit(uint64_t{0});
}

void serializer::correct_emitted_size() {
  auto header_prefix_length = 11u;
  auto header_prefix_length_with_version = header_prefix_length + 8u;

  auto bytes = detail::to_bytes(
      uint64_t{buffer_.size() - header_prefix_length_with_version});

  std::copy(bytes.begin(), bytes.end(), buffer_.begin() + header_prefix_length);
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
