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
  // and it is full iff read_pos == (write_pos+1)%size.
  static constexpr size_t size = 4096;

  // Ringbuffer capacity in bytes.
  // As commented above, the capacity is always equal to size-1.
  static constexpr size_t capacity = size - 1;

  // The default size in bytes for put and get operations.
  static constexpr size_t buffered_access_sz = 64;
  static_assert(buffered_access_sz <= capacity);

private:
  bool eof_{false};
  size_t read_pos_{0};
  size_t write_pos_{0};
  std::array<uint8_t, size> buffer_;

public:
  shm_ringbuffer();

  ~shm_ringbuffer() = default;

  // Returns the current size of the data contained in the ringbuffer.
  [[nodiscard]] size_t data_size() const;

  // Write EOF to the ring buffer. Further writes after this is called are
  // undefined behavior.
  void put_eof();

  // Returns true when eof has been written in the ringbuffer. At that point,
  // the ringbuffer may still contain data, but no further writes can happen.
  [[nodiscard]] bool eof() const;

  // Add data to the ringbuffer. The behavior is undefined if the buffer does not
  // have enough remaining space to fit the data or if EOF has been written to the
  // ringbuffer. The behavior is also undefined if the data pointer passed to this
  // function does not point to a contiguous memory chunk of the corresponding
  // size.
  void put(uint8_t const *data, size_t count = buffered_access_sz);

  // Get and remove data from the ringbuffer. The behavior is undefined if more
  // data is requested than it is currently available in the ringbuffer. The
  // behavior is also undefined if the data pointer passed to this function does
  // not point to a contiguous memory chunk of the corresponding size.
  void get(uint8_t *data, size_t count = buffered_access_sz);
};

} // namespace kllvm

#endif
