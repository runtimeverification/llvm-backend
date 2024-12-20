#include <runtime/header.h>
#include <runtime/proof_trace_writer.h>

#include <cstdint>
#include <fcntl.h>
#include <iostream>
#include <memory>
#include <sys/mman.h>
#include <fstream>
#include <sys/time.h>
#include <filesystem>

//
//	We can't set the CLOCK_PROCESS_CPUTIME_ID timer so we will
//	read it before and after and compute the difference.
//
timespec before_time;

void
start_timing()
{
  int r = clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &before_time);
  if (r) {
    perror("clock_gettime() failed");
    abort();
  }
}

extern "C" {


char *input_filename;
FILE *output_file = nullptr;
void *proof_writer = nullptr;
bool statistics = false;
bool binary_output = false;
bool proof_output = false;
size_t proof_chunk_size = 0;

uint64_t get_steps();
extern bool safe_partial;
extern bool proof_hint_instrumentation_slow;

int32_t get_exit_code(block *);

void init_outputs(char const *output_filename) {
  if (proof_output) {
    proof_writer
        = new proof_trace_file_writer(output_filename, proof_chunk_size);
    return;
  }
  output_file = fopen(output_filename, "a");
}

void
finish_timing()
{
  timespec after_time;
  int r = clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &after_time);
  if (r) {
    perror("clock_gettime() failed");
    abort();
  }
  //
  //	Convert everything to nanoseconds.
  //
  const int64_t before_nsec = before_time.tv_nsec +
    1'000'000'000LL * before_time.tv_sec;
  const int64_t after_nsec = after_time.tv_nsec +
    1'000'000'000LL * after_time.tv_sec;
  //
  //	Compute the number of rewrites per second as a double.
  //
  const int64_t elapsed_nsec = after_nsec - before_nsec;
  const int64_t steps = get_steps();
  const double stepsPerSecond = (1'000'000'000.0 * steps) / elapsed_nsec;
  //
  //	Specific to my home directory.
  //
  std::filesystem::path timing_path("/home/steven/tmp");
  //
  //	Construct name for timing file.
  //
  extern char *input_filename;
  std::filesystem::path input_path(input_filename);
  timing_path /= input_path.filename().replace_extension("timing");
  std::fstream fs(timing_path, std::fstream::out);
  //
  //	Print information.
  //
  fs << stepsPerSecond <<
    " elapsed_nsec=" << elapsed_nsec <<
    " steps=" << steps <<
    std::endl;
}

[[noreturn]] void finish_rewriting(block *subject, bool error) {
  // This function is responsible for closing output_file when rewriting
  // finishes; because it can exit in a few different ways (exceptions,
  // std::exit etc.) it's cleaner to set up a smart pointer to do this safely
  // for us.
  [[maybe_unused]] auto closer
      = std::unique_ptr<FILE, decltype(&fclose)>(output_file, fclose);

  // Similar for deletinging the proof_output_buffer data structure
  auto *w = static_cast<proof_trace_writer *>(proof_writer);
  [[maybe_unused]] auto deleter = std::unique_ptr<proof_trace_writer>(w);

  if (error && safe_partial) {
    throw std::runtime_error(
        "Attempted to evaluate partial function at an undefined input");
  }

  if (!output_file) {
    throw std::runtime_error(
        "Called finish_rewriting with no output file specified");
  }
  finish_timing();
  
  if (statistics) {
    uint64_t steps = get_steps();
    print_statistics(output_file, steps);
  }

  if (!proof_output) {
    if (binary_output) {
      serialize_configuration_to_file(output_file, subject, true, true);
    } else {
      print_configuration(output_file, subject);
    }
  } else if (!error && !proof_hint_instrumentation_slow) {
    write_configuration_to_proof_trace(proof_writer, subject, false);
  }

  bool was_proof_output = proof_output;
  proof_output = false;

  auto exit_code = error ? 113 : get_exit_code(subject);

  if (was_proof_output) {
    w->end_of_trace();
  }

  std::exit(exit_code);
}
}
