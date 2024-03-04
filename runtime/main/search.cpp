#include <cstring>
#include <unordered_set>

#include "runtime/header.h"

extern "C" {
void init_static_objects(void);
uint64_t get_steps(void);
}

std::unordered_set<block *, hash_block, k_eq> take_search_steps(
    bool executeToBranch, int64_t depth, int64_t bound, block *subject);
void print_configurations(
    FILE *file, std::unordered_set<block *, hash_block, k_eq> results);

void serialize_configurations(
    FILE *file, std::unordered_set<block *, hash_block, k_eq> results);

static bool hasStatistics = false;
static bool binaryOutput = false;
static bool executeToBranch = false;
static int64_t bound = -1;

void parse_flags(int argc, char **argv) {
  for (int i = 4; i < argc; ++i) {
    if (strcmp(argv[i], "--statistics") == 0) {
      hasStatistics = true;
    }
    if (strcmp(argv[i], "--binary-output") == 0) {
      binaryOutput = true;
    }
    if (strcmp(argv[i], "--bound") == 0) {
      bound = std::stoll(argv[i + 1]);
      ++i;
    }
    if (strcmp(argv[i], "--execute-to-branch") == 0) {
      executeToBranch = true;
    }
  }
}

int main(int argc, char **argv) {
  char *filename = argv[1];
  int64_t depth = atol(argv[2]);
  char *output = argv[3];

  parse_flags(argc, argv);

  init_static_objects();

  block *input = parse_configuration(filename);
  std::unordered_set<block *, hash_block, k_eq> results
      = take_search_steps(executeToBranch, depth, bound, input);
  FILE *file = fopen(output, "w");
  if (hasStatistics) {
    print_statistics(file, get_steps());
  }
  if (binaryOutput) {
    serialize_configurations(file, results);
  } else {
    print_configurations(file, results);
  }
  fclose(file);
  return 0;
}
