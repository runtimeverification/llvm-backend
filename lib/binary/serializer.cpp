#include <kllvm/binary/serializer.h>

namespace kllvm {

serializer::serializer()
    : buffer_{}
    , direct_string_prefix_{0x01}
    , backref_string_prefix_{0x02}
    , terminated_string_prefix_{0x03}
    , next_idx_(0)
    , intern_table_{} {
  for (auto b : magic_header) {
    emit(std::byte(b));
  }
}

void serializer::emit(std::byte b) {
  buffer_.push_back(b);
  next_idx_++;
}

void serializer::emit_string(std::string const &s) {
  if (s.size() < 3) {
    emit(terminated_string_prefix_);

    for (auto c : s) {
      emit(std::byte(c));
    }

    emit(std::byte(0));
  } else if (intern_table_.find(s) == intern_table_.end()) {
    emit(direct_string_prefix_);

    intern_table_[s] = next_idx_;

    int32_t length = s.size();
    emit(length);

    for (auto c : s) {
      emit(std::byte(c));
    }
  } else {
    emit(backref_string_prefix_);

    int32_t previous = intern_table_.at(s);
    int32_t diff = (next_idx_ + 4) - previous;

    emit(diff);
  }
}

} // namespace kllvm
