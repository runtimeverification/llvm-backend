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
    exit(1);                                                                   \
  } while (0)

int main(int argc, char **argv) {
  cl::HideUnrelatedOptions({&kore_proof_trace_shm_writer_cat});
  cl::ParseCommandLineOptions(argc, argv);

  // Open existing shared memory object
  int fd = shm_open(shm_filename.c_str(), O_RDWR, 0);
  if (fd == -1) {
    ERR_EXIT("shm_open writer");
  }

  // Map the object into the caller's address space
  size_t shm_size = sizeof(shm_ringbuffer);
  void *shm_object
      = mmap(nullptr, shm_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  if (shm_object == MAP_FAILED) {
    ERR_EXIT("mmap writer");
  }

  // Cast shared memory object to a shm_ringbuffer.
  auto *shm_buffer = reinterpret_cast<shm_ringbuffer *>(shm_object);

  // MacOS has deprecated unnamed semaphores, so we need to use named ones
  std::string data_avail_sem_name = shm_filename + ".d";
  std::string space_avail_sem_name = shm_filename + ".s";

  // Open existing semaphores
  // NOLINTNEXTLINE(*-pro-type-vararg)
  sem_t *data_avail = sem_open(data_avail_sem_name.c_str(), 0);
  if (data_avail == SEM_FAILED) {
    ERR_EXIT("sem_init data_avail writer");
  }
  // NOLINTNEXTLINE(*-pro-type-vararg)
  sem_t *space_avail = sem_open(space_avail_sem_name.c_str(), 0);
  if (space_avail == SEM_FAILED) {
    ERR_EXIT("sem_init space_avail writer");
  }

  // Copy the trace file into the shared memory object
  std::ifstream file(input_filename, std::ios_base::binary);
  while (true) {
    char c = 0;
    file.read(&c, 1);
    if (file.eof()) {
      break;
    }
    sem_wait(space_avail);
    shm_buffer->put((uint8_t *)&c);
    sem_post(data_avail);
  }
  shm_buffer->put_eof();

  // Close semaphores
  if (sem_close(data_avail) == -1) {
    ERR_EXIT("sem_close data reader");
  }
  if (sem_close(space_avail) == -1) {
    ERR_EXIT("sem_close space reader");
  }

  // Exit
  return 0;
}
