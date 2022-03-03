#include <unordered_set>

#include "runtime/header.h"

extern "C" {
void initStaticObjects(void);
uint64_t get_steps(void);
}

std::unordered_set<block *, HashBlock, KEq>
take_search_steps(int64_t depth, block *subject);
void printConfigurations(
    const char *filename, std::unordered_set<block *, HashBlock, KEq> results);

static bool hasStatistics = false;

void parse_flags(int argc, char **argv) {
  for (int i = 4; i < argc; ++i) {
    if (str_eq(argv[i], "--statistics")) {
      hasStatistics = true;
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
      = take_search_steps(depth, input);
  if (hasStatistics) {
    printStatistics(output, get_steps());
  }
  printConfigurations(output, results);
  return 0;
}
