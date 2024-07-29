#include <kllvm/binary/serializer.h>
#include <kllvm/binary/version.h>

namespace kllvm {

namespace detail {

bool is_big_endian() {
  uint32_t i = 1;
  auto *c = static_cast<uint8_t *>(static_cast<void *>(&i));
  return *c == 0x00;
}

} // namespace detail

serializer::serializer()
    : serializer(NONE) { }

serializer::serializer(flags f)
    : use_header_(!(f & DropHeader))
    , use_arity_(!(f & DropArity))
    , use_intern_(!(f & NoIntern))
    , direct_string_prefix_{0x01}
    , backref_string_prefix_{0x02}
    , next_idx_(0) {
  if (use_header_) {
    emit_header_and_version();
    emit_zero_size();
  }
}

std::string serializer::byte_string() const {
  auto ret = std::string{};
  ret.reserve(buffer_.size());

  for (auto byte : buffer_) {
    ret.push_back(static_cast<char>(byte));
  }

  return ret;
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
    emit(b);
  }

  emit(version.v_major);
  emit(version.v_minor);
  emit(version.v_patch);
}

void serializer::emit_zero_size() {
  emit(uint64_t{0});
}

void serializer::correct_emitted_size() {
  auto header_prefix_length = 11U;
  auto header_prefix_length_with_version = header_prefix_length + 8U;

  uint64_t new_size = buffer_.size() - header_prefix_length_with_version;
  std::copy(
      reinterpret_cast<char *>(&new_size),
      reinterpret_cast<char *>(&new_size + 1),
      buffer_.begin() + header_prefix_length);
}

void serializer::emit(char b) {
  buffer_.push_back(b);
  next_idx_++;
}

void serializer::emit_string(std::string const &s) {
  if (!use_intern_) {
    emit_direct_string(s);
    return;
  }
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

int serializer::required_chunks(uint64_t len) {
  auto ret = 0;
  do {
    ++ret;
  } while (len >>= 7);
  return ret;
}

void serializer::emit_direct_string(std::string const &s) {
  emit(direct_string_prefix_);

  if (use_intern_) {
    intern_table_[s] = next_idx_;
  }

  emit_length(s.size());
  buffer_.append(s);
  next_idx_ += s.size();
}

void emit_kore_rich_header(std::ostream &os, kore_definition *definition) {
  const uint32_t version = 1;
  const uint32_t num_tags = definition->get_symbols().size();
  const uint32_t num_sorts = definition->get_all_sorts().size();
  const uint32_t num_strings = num_tags + num_sorts + 1;
  const uint32_t num_symbols = num_tags + num_sorts;

  os.write("\177KR2", 4);
  os.write(reinterpret_cast<char const *>(&version), 4);
  os.write(reinterpret_cast<char const *>(&num_strings), 4);
  os.write(reinterpret_cast<char const *>(&num_sorts), 4);
  os.write(reinterpret_cast<char const *>(&num_symbols), 4);

  for (uint32_t i = 0; i < num_tags; i++) {
    auto name = definition->get_symbols().at(i)->get_name();
    const uint32_t len = name.size();
    os.write(reinterpret_cast<char const *>(&len), 4);
    os << name;
    os.put('\000');
  }
  for (uint32_t i = 0; i < num_sorts; i++) {
    auto name = definition->get_all_sorts()[i]->get_name();
    const uint32_t len = name.size();
    os.write(reinterpret_cast<char const *>(&len), 4);
    os << name;
    os.put('\000');
  }
  auto const *name = "\\dv";
  const uint32_t len = 3;
  os.write(reinterpret_cast<char const *>(&len), 4);
  os.write(name, 4);

  for (uint32_t i = 0; i < num_sorts; i++) {
    uint32_t idx = i + num_tags;
    os.write(reinterpret_cast<char const *>(&idx), 4);
    if (!definition->get_all_sorts()[i]->get_arguments().empty()) {
      throw std::runtime_error(
          "cannot yet serialize sorts with sort parameters");
    }
    os.put('\000');
  }

  for (uint32_t i = 0; i < num_tags; i++) {
    os.write(reinterpret_cast<char const *>(&i), 4);
    auto const &symbol = definition->get_symbols().at(i);
    int const num_params = symbol->get_formal_arguments().size();
    os.put((uint8_t)num_params);
    os.put((uint8_t)symbol->get_arguments().size());
    for (int j = 0; j < num_params; j++) {
      uint32_t ordinal = dynamic_cast<kore_composite_sort *>(
                             symbol->get_formal_arguments()[j].get())
                             ->get_ordinal();
      os.write(reinterpret_cast<char const *>(&ordinal), 4);
    }
  }
  for (uint32_t i = 0; i < num_sorts; i++) {
    uint32_t idx = num_strings - 1;
    os.write(reinterpret_cast<char const *>(&idx), 4);
    int const num_params = 1;
    os.put((uint8_t)num_params);
    os.put((uint8_t)num_params);
    uint32_t ordinal = definition->get_all_sorts()[i]->get_ordinal();
    os.write(reinterpret_cast<char const *>(&ordinal), 4);
  }
}

void proof_trace_file_writer::write(void const *ptr, size_t len) {
  fwrite(ptr, len, 1, file_);
}

void proof_trace_file_writer::write_string(char const *str, size_t len) {
  fwrite(str, 1, len, file_);
}

void proof_trace_file_writer::write_string(char const *str) {
  fputs(str, file_);
}

} // namespace kllvm
