#include <kllvm/binary/ringbuffer.h>

#include <llvm/Support/CommandLine.h>

#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <string>
#include <sys/mman.h>

using namespace llvm;
using namespace kllvm;

cl::OptionCategory
    kore_proof_trace_shm_writer_cat("kore-proof-trace-shm-writer options");

cl::opt<std::string> input_filename(
    cl::Positional, cl::desc("<input file>"), cl::Required,
    cl::cat(kore_proof_trace_shm_writer_cat));

cl::opt<std::string> shm_filename(
    cl::Positional, cl::desc("<shared memory file>"), cl::Required,
    cl::cat(kore_proof_trace_shm_writer_cat));

#define ERR_EXIT(msg)                                                          \
  do {                                                                         \
    perror(msg);                                                               \
    exit(EXIT_FAILURE);                                                        \
  } while (0)

int main(int argc, char **argv) {
  cl::HideUnrelatedOptions({&kore_proof_trace_shm_writer_cat});
  cl::ParseCommandLineOptions(argc, argv);

  // Open existing shared memory object
  int fd = shm_open(shm_filename.c_str(), O_RDWR, 0);
  if (fd == -1) {
    ERR_EXIT("shm_open");
  }

  // Map the object into the caller's address space
  size_t shm_size = sizeof(shm_ringbuffer_t);
  auto *shm_buffer = (shm_ringbuffer_t *)mmap(
      nullptr, shm_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  if (shm_buffer == MAP_FAILED) {
    ERR_EXIT("mmap");
  }

  // Copy the trace file into the shared memory object
  std::ifstream file(input_filename, std::ios_base::binary);
  while (true) {
    char c = 0;
    file.read(&c, 1);
    if (file.eof()) {
      break;
    }
    sem_wait(&shm_buffer->space_avail);
    ringbuffer_put(*shm_buffer, (uint8_t *)&c);
    sem_post(&shm_buffer->data_avail);
  }
  ringbuffer_put_eof(*shm_buffer);

  // Exit
  return 0;
}
