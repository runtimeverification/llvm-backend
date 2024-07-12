#include <kllvm/binary/ProofTraceParser.h>
#include <kllvm/parser/KOREParser.h>

#include <llvm/Support/CommandLine.h>

#include <fcntl.h>
#include <fstream>
#include <string>
#include <sys/mman.h>

using namespace llvm;
using namespace kllvm;

cl::OptionCategory kore_proof_trace_cat("kore-proof-trace options");

cl::opt<std::string> header_path(
    cl::Positional, cl::desc("<header file>"), cl::Required,
    cl::cat(kore_proof_trace_cat));

cl::opt<std::string> input_filename(
    cl::Positional, cl::desc("<input file>"), cl::Required,
    cl::cat(kore_proof_trace_cat));

cl::opt<std::string> kore_filename(
    cl::Positional, cl::desc("[kore definition file]"), cl::Optional,
    cl::cat(kore_proof_trace_cat));

cl::opt<bool> verbose_output(
    "verbose",
    llvm::cl::desc("Print verbose information about the input proof trace"),
    llvm::cl::cat(kore_proof_trace_cat));

cl::opt<bool> expand_terms_in_output(
    "expand-terms", llvm::cl::desc("Expand KORE terms in the verbose output"),
    llvm::cl::cat(kore_proof_trace_cat));

cl::opt<bool> use_streaming_parser(
    "streaming-parser",
    llvm::cl::desc("Use streaming event parser to parse trace"),
    llvm::cl::cat(kore_proof_trace_cat));

cl::opt<bool> use_shared_memory(
    "shared-memory", cl::desc("Use shared memory parser to parse trace"),
    cl::cat(kore_proof_trace_cat));

#define errExit(msg)                                                           \
  do {                                                                         \
    perror(msg);                                                               \
    exit(EXIT_FAILURE);                                                        \
  } while (0)

int main(int argc, char **argv) {
  cl::HideUnrelatedOptions({&kore_proof_trace_cat});
  cl::ParseCommandLineOptions(argc, argv);

  FILE *in = fopen(header_path.getValue().c_str(), "r");
  kore_header header(in);
  fclose(in);

  if (use_streaming_parser) {
    std::ifstream file(input_filename, std::ios_base::binary);
    llvm_rewrite_trace_iterator it(
        std::make_unique<proof_trace_file_buffer>(std::move(file)), header);
    if (verbose_output) {
      it.print(std::cout, expand_terms_in_output);
    }
    return 0;
  }

  std::optional<kore_definition> kore_def;

  if (!kore_filename.empty()) {
    std::fstream kore_file(kore_filename);
    kore_def
        = std::make_optional(*parser::kore_parser(kore_filename).definition());
    kore_def->preprocess();
  }

  proof_trace_parser parser(
      verbose_output, expand_terms_in_output, header, kore_def);

  if (use_shared_memory) {
    // Create shared memory object and set its size
    int fd = shm_open(input_filename.c_str(), O_CREAT | O_TRUNC | O_RDWR, 0600);
    if (fd == -1) {
      errExit("shm_open");
    }
    size_t shm_size = sizeof(shm_ringbuffer_t);
    if (ftruncate(fd, shm_size) == -1) {
      errExit("ftruncate");
    }

    // Map the object into the caller's address space
    shm_ringbuffer_t *shm_buffer = (shm_ringbuffer_t *)mmap(
        nullptr, shm_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (shm_buffer == MAP_FAILED) {
      errExit("mmap");
    }

    // Initialize ringbuffer
    ringbuffer_init(*shm_buffer);

    // Initialize semaphores
    if (sem_init(&shm_buffer->data_avail, 1, 0) == -1) {
      errExit("sem_init-data_avail");
    }
    if (sem_init(&shm_buffer->space_avail, 1, RINGBUFFER_CAPACITY) == -1) {
      errExit("sem_init-space_avail");
    }

    // Do parsing
    auto trace = parser.parse_proof_trace_from_shmem(shm_buffer);

    // Unlink the shared memory object
    shm_unlink(input_filename.c_str());

    // Exit
    if (trace.has_value()) {
      return 0;
    }
    return 1;
  }

  auto trace = parser.parse_proof_trace_from_file(input_filename);
  if (trace.has_value()) {
    return 0;
  }

  return 1;
}
