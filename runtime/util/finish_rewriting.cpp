#include <kllvm/binary/serializer.h>

#include <runtime/header.h>

#include <cstdint>
#include <fcntl.h>
#include <iostream>
#include <memory>
#include <sys/mman.h>

extern "C" {

FILE *output_file = nullptr;
void *proof_writer = nullptr;
bool statistics = false;
bool binary_output = false;
bool proof_output = false;
bool use_shm = false;

extern int64_t steps;
extern bool safe_partial;
extern bool proof_hint_instrumentation_slow;

int32_t get_exit_code(block *);

#define ERR_EXIT(msg)                                                          \
  do {                                                                         \
    perror(msg);                                                               \
    exit(1);                                                                   \
  } while (0)

static void set_up_shm_writer(char const *output_filename) {
  // Open existing shared memory object
  int fd = shm_open(output_filename, O_RDWR, 0);
  if (fd == -1) {
    ERR_EXIT("shm_open writer");
  }

  // Map the object into the caller's address space
  size_t shm_size = sizeof(kllvm::shm_ringbuffer);
  void *shm_object
      = mmap(nullptr, shm_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  if (shm_object == MAP_FAILED) {
    ERR_EXIT("mmap writer");
  }

  // MacOS has deprecated unnamed semaphores, so we need to use named ones
  std::string base_name(output_filename);
  std::string data_avail_sem_name = base_name + ".d";
  std::string space_avail_sem_name = base_name + ".s";

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

  // Create the proof_trace_ringbuffer_writer object
  proof_writer = new kllvm::proof_trace_ringbuffer_writer(
      shm_object, data_avail, space_avail);
}

void init_outputs(char const *output_filename) {
  if (proof_output && use_shm) {
    set_up_shm_writer(output_filename);
    return;
  }

  output_file = fopen(output_filename, "a");
  if (proof_output) {
    proof_writer = new kllvm::proof_trace_file_writer(output_file);
  }
}

[[noreturn]] void finish_rewriting(block *subject, bool error) {
  // This function is responsible for closing output_file when rewriting
  // finishes; because it can exit in a few different ways (exceptions,
  // std::exit etc.) it's cleaner to set up a smart pointer to do this safely
  // for us.
  [[maybe_unused]] auto closer
      = std::unique_ptr<FILE, decltype(&fclose)>(output_file, fclose);

  // Similar for deletinging the proof_output_buffer data structure
  auto *w = static_cast<kllvm::proof_trace_writer *>(proof_writer);
  [[maybe_unused]] auto deleter = std::unique_ptr<kllvm::proof_trace_writer>(w);

  if (error && safe_partial) {
    throw std::runtime_error(
        "Attempted to evaluate partial function at an undefined input");
  }

  if (!output_file) {
    if (!proof_output || !use_shm) {
      throw std::runtime_error(
          "Called finish_rewriting with no output file specified");
    }
  }

  if (statistics) {
    print_statistics(output_file, steps);
  }

  if (!proof_output) {
    if (binary_output) {
      serialize_configuration_to_file(output_file, subject, true, true);
    } else {
      print_configuration(output_file, subject);
    }
  } else if (!error && !proof_hint_instrumentation_slow) {
    w->write_uint64(0xFFFFFFFFFFFFFFFF);
    serialize_configuration_to_proof_writer(proof_writer, subject);
  }

  auto exit_code = error ? 113 : get_exit_code(subject);

  if (proof_output) {
    w->write_eof();
  }

  std::exit(exit_code);
}
}
