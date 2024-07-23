#include <boost/test/unit_test.hpp>
#include <kllvm/binary/ringbuffer.h>

using namespace kllvm;

BOOST_AUTO_TEST_SUITE(RingbufferTest)

BOOST_AUTO_TEST_CASE(smoke) {
  shm_ringbuffer buffer;
  std::string message = "hello world";

  BOOST_CHECK(!buffer.eof());

  buffer.put((uint8_t *)message.data(), message.size());
  BOOST_CHECK(!buffer.eof());

  buffer.put_eof();
  BOOST_CHECK(!buffer.eof());

  std::string read_data;
  read_data.resize(message.size());
  buffer.get((uint8_t *)read_data.data(), read_data.size());
  BOOST_CHECK(read_data.compare(message) == 0);
  BOOST_CHECK(buffer.eof());
}

BOOST_AUTO_TEST_CASE(write_read_loop) {
  shm_ringbuffer buffer;
  std::string message = "abcdefghijklmnopq"; // messsage size is 17

  std::string read_data;
  read_data.resize(message.size());
  size_t read_chunk_size = message.size() / 3;
  size_t read_small_chunk_size = message.size() % 3;

  unsigned iters = 2 * (shm_ringbuffer::size / message.size());

  size_t read_offs = 0;
  unsigned iter = 0;
  while (iter < iters) {
    if (message.size() <= shm_ringbuffer::capacity - buffer.data_size()) {
      buffer.put((uint8_t *)message.data(), message.size());
      iter++;
    }

    if (read_offs + read_chunk_size <= message.size()) {
      BOOST_CHECK(read_chunk_size <= buffer.data_size());
      buffer.get((uint8_t *)read_data.data() + read_offs, read_chunk_size);
      read_offs += read_chunk_size;
    } else {
      BOOST_CHECK(read_small_chunk_size <= buffer.data_size());
      buffer.get(
          (uint8_t *)read_data.data() + read_offs, read_small_chunk_size);
      BOOST_CHECK(read_data.compare(message) == 0);
      BOOST_CHECK(read_offs + read_small_chunk_size == message.size());
      read_offs = 0;
    }
  }

  buffer.put_eof();
  BOOST_CHECK(!buffer.eof());

  while (!buffer.eof()) {
    if (read_offs + read_chunk_size <= message.size()) {
      BOOST_CHECK(read_chunk_size <= buffer.data_size());
      buffer.get((uint8_t *)read_data.data() + read_offs, read_chunk_size);
      read_offs += read_chunk_size;
    } else {
      BOOST_CHECK(read_small_chunk_size <= buffer.data_size());
      buffer.get(
          (uint8_t *)read_data.data() + read_offs, read_small_chunk_size);
      BOOST_CHECK(read_data.compare(message) == 0);
      BOOST_CHECK(read_offs + read_small_chunk_size == message.size());
      read_offs = 0;
    }
  }
}

BOOST_AUTO_TEST_SUITE_END()
