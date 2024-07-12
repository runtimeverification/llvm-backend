#include <kllvm/binary/ringbuffer.h>

#include <cassert>
#include <cstring>

namespace kllvm {

void ringbuffer_init(shm_ringbuffer_t &buf) {
  buf.write_pos = 0;
  buf.read_pos = 0;
  buf.eof = false;
}

void ringbuffer_put_eof(shm_ringbuffer_t &buf) {
  assert(!buf.eof);
  buf.eof = true;
}

bool ringbuffer_eof(shm_ringbuffer_t const &buf) {
  // NOTE: for synchronization purposes, it is important that the buf.eof field
  // is checked first. Typically, the reader process will call this, so we want
  // to avoid a race where the writer updates buf.writer_pos after the reader
  // has accessed it but before the reader has fully evaluated the condition.
  // If buf.eof is checked first, and due to short-circuiting, we know that if
  // buf.eof is true, the writer will not do any further updates to the
  // buf.write_pos field, and if buf.eof is false, the reader will not access
  // buf.write_pos at all.
  return buf.eof && buf.write_pos == buf.read_pos;
}

void ringbuffer_put(shm_ringbuffer_t &buf, uint8_t const *data, size_t count) {
  assert(!buf.eof);
  assert(data);

  // check if we need to wrap to the start of the ringbuffer
  size_t no_wrap_size = RINGBUFFER_SIZE - buf.write_pos;
  size_t rest_count = count;
  if (count > no_wrap_size) {
    // if yes, do a first copy to reach the end of the ringbuffer and wrap to
    // start
    memcpy(buf.buffer + buf.write_pos, data, no_wrap_size);
    buf.write_pos = 0;
    data += no_wrap_size;
    rest_count = count - no_wrap_size;
  }

  // copy the (rest of the) data
  memcpy(buf.buffer + buf.write_pos, data, rest_count);
  buf.write_pos += rest_count;
  if (buf.write_pos == RINGBUFFER_SIZE) {
    buf.write_pos = 0;
  }
}

void ringbuffer_get(shm_ringbuffer_t &buf, uint8_t *data, size_t count) {
  assert(data);

  // check if we need to wrap to the start of the ringbuffer
  size_t no_wrap_size = RINGBUFFER_SIZE - buf.read_pos;
  size_t rest_count = count;
  if (count > no_wrap_size) {
    // if yes, do a first copy to reach the end of the ringbuffer and wrap to
    // start
    memcpy(data, buf.buffer + buf.read_pos, no_wrap_size);
    buf.read_pos = 0;
    data += no_wrap_size;
    rest_count = count - no_wrap_size;
  }

  // copy the (rest of the) data
  memcpy(data, buf.buffer + buf.read_pos, rest_count);
  buf.read_pos += rest_count;
  if (buf.read_pos == RINGBUFFER_SIZE) {
    buf.read_pos = 0;
  }
}

} // namespace kllvm
