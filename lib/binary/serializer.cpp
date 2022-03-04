#include <kllvm/binary/serializer.h>

#include <arpa/inet.h>
#include <cassert>
#include <limits>

namespace kllvm {

namespace detail {
bool is_big_endian() {
  return htonl(365) == 365;
}
} // namespace detail

serializer::serializer()
    : buffer_{}
    , direct_string_prefix_{0x01}
    , backref_string_prefix_{0x02}
    , next_idx_(0)
    , intern_table_{} {
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
    int64_t previous = intern_table_.at(s);
    int64_t diff = (next_idx_ + 5) - previous;

    if (diff <= std::numeric_limits<int32_t>::max()) {
      emit(backref_string_prefix_);
      emit(int32_t(diff));
    } else {
      emit_direct_string(s);
    }
  }
}

void serializer::emit_direct_string(std::string const &s) {
  emit(direct_string_prefix_);

  intern_table_[s] = next_idx_;

  assert(
      s.size() <= std::numeric_limits<int32_t>::max()
      && "String too large to be serialized");
  emit(int32_t(s.size()));

  for (auto c : s) {
    emit(std::byte(c));
  }
}

} // namespace kllvm
