#ifndef RINGBUFFER_H
#define RINGBUFFER_H

#include <array>
#include <cstdint>
#include <cstdlib>
#include <semaphore.h>

namespace kllvm {

// Simple ringbuffer class, intended to live in shared memory and operated by a
// reader and a writer process.

class shm_ringbuffer {
public:
  // Ringbuffer size in bytes.
  // NOTE: In order to easily distinguish between when the buffer is full versus
  // empty, we maintain the invariant that the capacity of the buffer is one byte
  // less than its size. This way, the buffer is empty iff read_pos == write_pos,
  // and it is full iff read_pos == (write_pos+1)%RINGBUFFER_SIZE.
  static constexpr size_t size = 1024;

  // Ringbuffer capacity in bytes.
  // As commented above, the capacity is always equal to RINGBUFFER_SIZE-1.
  static constexpr size_t capacity = size - 1;

private:
  bool eof_;
  size_t read_pos_;
  size_t write_pos_;
  std::array<uint8_t, size> buffer_;

public:
  shm_ringbuffer();

  // Write EOF to the ring buffer. Further writes after this is called are
  // undefined behavior.
  void put_eof();

  // Returns true when the ringbuffer is empty and the EOF has been written, and
  // false otherwise. As commented above, the ringbuffer is empty iff
  // read_pos == write_pos.
  bool eof() const;

  // Add data to the ringbuffer. The behavior is undefined if the buffer does not
  // have enough remaining space to fit the data or if EOF has been written to the
  // ringbuffer. The behavior is also undefined if the data pointer passed to this
  // function does not point to a contiguous memory chunk of the corresponding
  // size.
  void put(uint8_t const *data, size_t count = 1);

  // Get and remove data from the ringbuffer. The behavior is undefined if more
  // data is requested than it is currently available in the ringbuffer. The
  // behavior is also undefined if the data pointer passed to this function does
  // not point to a contiguous memory chunk of the corresponding size.
  void get(uint8_t *data, size_t count = 1);
};

} // namespace kllvm

#endif
