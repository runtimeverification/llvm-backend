#include <cstring>
#include <unordered_set>

#include "runtime/header.h"

extern "C" {
void initStaticObjects(void);
uint64_t get_steps(void);
}

std::unordered_set<block *, HashBlock, KEq>
take_search_steps(int64_t depth, int64_t bound, block *subject);
void printConfigurations(
    const char *filename, std::unordered_set<block *, HashBlock, KEq> results);

void serializeConfigurations(
    const char *filename, std::unordered_set<block *, HashBlock, KEq> results);

static bool hasStatistics = false;
static bool binaryOutput = false;
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
  }
}

int main(int argc, char **argv) {
  char *filename = argv[1];
  int64_t depth = atol(argv[2]);
  char *output = argv[3];

  parse_flags(argc, argv);

  initStaticObjects();

  block *input = parseConfiguration(filename);
  std::unordered_set<block *, HashBlock, KEq> results
      = take_search_steps(depth, bound, input);
  if (hasStatistics) {
    printStatistics(output, get_steps());
  }
  if (binaryOutput) {
    serializeConfigurations(output, results);
  } else {
    printConfigurations(output, results);
  }
  return 0;
}
