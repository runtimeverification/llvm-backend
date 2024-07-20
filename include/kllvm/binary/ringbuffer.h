#ifndef RINGBUFFER_H
#define RINGBUFFER_H

#include <array>
#include <cstdint>
#include <cstdlib>
#include <semaphore.h>

namespace kllvm {

// Simple ringbuffer struct, intended to live in shared memory and operated by a
// reader and a writer prpocess.

// Ringbuffer size in bytes.
// NOTE: In order to easily distinguish between when the buffer is full versus
// empty, we maintain the invariant that the capacity of the buffer is one byte
// less than its size. This way, the buffer is empty iff read_pos == write_pos,
// and it is full iff read_pos == (write_pos+1)%RINGBUFFER_SIZE.
constexpr size_t ringbuffer_size = 1024;

// Ringbuffer capacity in bytes.
// As commented above, the capacity is always equal to RINGBUFFER_SIZE-1.
constexpr size_t ringbuffer_capacity = ringbuffer_size - 1;

struct shm_ringbuffer_t {
  bool eof;
  size_t read_pos;
  size_t write_pos;
  std::array<uint8_t, ringbuffer_size> buffer;
};

// Initialize the ringbuffer.
void ringbuffer_init(shm_ringbuffer_t &buf);

// Write EOF to the ring buffer. Further writes after this is called are
// undefined behavior.
void ringbuffer_put_eof(shm_ringbuffer_t &buf);

// Returns true when the ringbuffer is empty and the EOF has been written, and
// false otherwise. As commented above, the ringbuffer is empty iff
// read_pos == write_pos.
bool ringbuffer_eof(shm_ringbuffer_t const &buf);

// Add data to the ringbuffer. The behavior is undefined if the buffer does not
// have enough remaining space to fit the data or if EOF has been written to the
// ringbuffer. The behavior is also undefined if the data pointer passed to this
// function does not point to a contiguous memory chunk of the corresponding
// size.
void ringbuffer_put(
    shm_ringbuffer_t &buf, uint8_t const *data, size_t count = 1);

// Get and remove data from the ringbuffer. The behavior is undefined if more
// data is requested than it is currently available in the ringbuffer. The
// behavior is also undefined if the data pointer passed to this function does
// not point to a contiguous memory chunk of the corresponding size.
void ringbuffer_get(shm_ringbuffer_t &buf, uint8_t *data, size_t count = 1);

} // namespace kllvm

#endif
