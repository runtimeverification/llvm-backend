#include <kllvm/binary/ProofTraceParser.h>
#include <kllvm/parser/KOREParser.h>

#include <llvm/Support/CommandLine.h>

#include <fcntl.h>
#include <fstream>
#include <string>
#include <sys/mman.h>
#include <sys/stat.h>

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

#define ERR_EXIT(msg)                                                          \
  do {                                                                         \
    perror(msg);                                                               \
    exit(1);                                                                   \
  } while (0)

// NOLINTNEXTLINE(*-cognitive-complexity)
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
    // Unlink any existing shared memory object with the same name
    shm_unlink(input_filename.c_str());

    // Open shared memory object
    int fd = shm_open(
        input_filename.c_str(), O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR);
    if (fd == -1) {
      ERR_EXIT("shm_open reader");
    }

    // Set the size of the shared memory object
    size_t shm_size = sizeof(shm_ringbuffer);
    if (ftruncate(fd, shm_size) == -1) {
      ERR_EXIT("ftruncate reader");
    }

    // Map the object into the caller's address space
    void *shm_object
        = mmap(nullptr, shm_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (shm_object == MAP_FAILED) {
      ERR_EXIT("mmap reader");
    }

    // MacOS has deprecated unnamed semaphores, so we need to use named ones
    std::string data_avail_sem_name = input_filename + ".d";
    std::string space_avail_sem_name = input_filename + ".s";

    // Unlink any existing semaphores with the same names
    sem_unlink(data_avail_sem_name.c_str());
    sem_unlink(space_avail_sem_name.c_str());

    // Initialize semaphores
    // NOLINTNEXTLINE(*-pro-type-vararg)
    sem_t *data_avail = sem_open(
        data_avail_sem_name.c_str(), O_CREAT | O_EXCL, S_IRUSR | S_IWUSR, 0);
    if (data_avail == SEM_FAILED) {
      ERR_EXIT("sem_init data_avail reader");
    }
    // NOLINTNEXTLINE(*-pro-type-vararg)
    sem_t *space_avail = sem_open(
        space_avail_sem_name.c_str(), O_CREAT | O_EXCL, S_IRUSR | S_IWUSR,
        shm_ringbuffer::capacity);
    if (space_avail == SEM_FAILED) {
      ERR_EXIT("sem_init space_avail reader");
    }

    // Do parsing
    auto trace = parser.parse_proof_trace_from_shmem(
        shm_object, data_avail, space_avail);

    // Close semaphores
    if (sem_close(data_avail) == -1) {
      ERR_EXIT("sem_close data reader");
    }
    if (sem_close(space_avail) == -1) {
      ERR_EXIT("sem_close space reader");
    }

    // Unlink the shared memory object and semaphores
    if (sem_unlink(data_avail_sem_name.c_str()) == -1) {
      ERR_EXIT("sem_unlink data reader");
    }
    if (sem_unlink(space_avail_sem_name.c_str()) == -1) {
      ERR_EXIT("sem_unlink space reader");
    }
    if (shm_unlink(input_filename.c_str()) == -1) {
      ERR_EXIT("shm_unlink reader");
    }

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
