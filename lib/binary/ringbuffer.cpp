#include <kllvm/binary/ringbuffer.h>

#include <cassert>
#include <cstring>

namespace kllvm {

shm_ringbuffer::shm_ringbuffer()
    : buffer_() { }

size_t shm_ringbuffer::data_size() const {
  if (write_pos_ < read_pos_) {
    return size - (read_pos_ - write_pos_);
  }

  return write_pos_ - read_pos_;
}

void shm_ringbuffer::put_eof() {
  assert(!eof_);
  eof_ = true;
}

bool shm_ringbuffer::eof() const {
  // NOTE: for synchronization purposes, it is important that the eof_ field is
  // checked first. Typically, the reader process will call this, so we want to
  // avoid a race where the writer updates buf.writer_pos after the reader has
  // accessed it but before the reader has fully evaluated the condition. If
  // eof_ is checked first, and due to short-circuiting, we know that if eof_ is
  // true, the writer will not do any further updates to the write_pos_ field,
  // and if eof_ is false, the reader will not access write_pos_ at all.
  return eof_ && write_pos_ == read_pos_;
}

void shm_ringbuffer::put(uint8_t const *data, size_t count) {
  assert(!eof_);
  assert(data);

  // check if we need to wrap to the start of the ringbuffer
  size_t no_wrap_size = size - write_pos_;
  size_t rest_count = count;
  if (count > no_wrap_size) {
    // if yes, do a first copy to reach the end of the ringbuffer and wrap to
    // start
    memcpy(buffer_.data() + write_pos_, data, no_wrap_size);
    write_pos_ = 0;
    data += no_wrap_size;
    rest_count = count - no_wrap_size;
  }

  // copy the (rest of the) data
  memcpy(buffer_.data() + write_pos_, data, rest_count);
  write_pos_ += rest_count;
  if (write_pos_ == size) {
    write_pos_ = 0;
  }
}

void shm_ringbuffer::get(uint8_t *data, size_t count) {
  assert(data);

  // check if we need to wrap to the start of the ringbuffer
  size_t no_wrap_size = size - read_pos_;
  size_t rest_count = count;
  if (count > no_wrap_size) {
    // if yes, do a first copy to reach the end of the ringbuffer and wrap to
    // start
    memcpy(data, buffer_.data() + read_pos_, no_wrap_size);
    read_pos_ = 0;
    data += no_wrap_size;
    rest_count = count - no_wrap_size;
  }

  // copy the (rest of the) data
  memcpy(data, buffer_.data() + read_pos_, rest_count);
  read_pos_ += rest_count;
  if (read_pos_ == size) {
    read_pos_ = 0;
  }
}

} // namespace kllvm
