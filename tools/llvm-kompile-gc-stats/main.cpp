#include <gmp.h>

#include <array>
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>

// NOLINTNEXTLINE(*-cognitive-complexity)
int main(int argc, char **argv) {
  const char *usage = "usage: %s [dump|analyze|generation|count] <file>"
                      " [<lower_bound> <upper_bound>]\n";
  if (argc < 3) {
    fprintf(stderr, usage, argv[0]);
    return 1;
  }
  FILE *f = fopen(argv[2], "rb");
  auto frame = std::array<size_t, 2049>{};
  auto total = std::array<mpz_t, 2048>{};
  size_t step = 0;
  // `llvm-kompile-gc-stats dump` dumps the raw log to stdout. useful for
  // debugging, less useful as information
  bool dump = strcmp(argv[1], "dump") == 0;
  // `llvm-kompile-gc-stats analyze` emits the total number of bytes over the
  // life of the program that survived exactly N collections for N in [0, 2046]
  // as well as the total number of bytes that survived >2046 collections
  bool analyze = strcmp(argv[1], "analyze") == 0;
  // `llvm-kompile-gc-stats generation` emits the size in bytes of a generation
  // with a user-specified size at each step of the trace. This can then be
  // analyzed further to determine max heap size and average heap size over
  // the life of a program.
  bool generation = strcmp(argv[1], "generation") == 0;
  // `llvm-kompile-gc-stats count` emits the number of collections in the log.
  bool count = strcmp(argv[1], "count") == 0;
  // `llvm-kompile-gc-stats alloc` emits the number of bytes allocated
  // after each collection. This can be used to graph the allocation behavior
  // over time.
  bool alloc = strcmp(argv[1], "alloc") == 0;
  if (analyze) {
    for (auto &i : total) {
      mpz_init(i);
    }
  }
  int lowerBound;
  int upperBound;
  mpz_t size;
  if (generation) {
    lowerBound = atoi(argv[3]);
    upperBound = atoi(argv[4]);
    mpz_init(size);
  }
  while (true) {
    int ret = fread(frame.data(), sizeof(size_t), 2049, f);
    // the frame contains 2049 integers:
    //
    // frame[0] contains the total number of bytes allocated since the last
    // collection cycle
    //
    // frame[i] for i in [1..2047] contains the total number of bytes that
    // survived exactly i collection cycles that are alive at that point in
    // time.
    //
    // frame[2048] contains the total number of bytes that survived
    // at least 2048 collection cycles that are alive at that point in time.
    if (ret < 2049) {
      break;
    }
    if (dump) {
      printf("Collection %zd\n", step);
      for (int i = 0; i < 2048; i++) {
        printf("%d: %zd\n", i, frame[i]);
      }
      printf("saturated: %zd\n", frame[2048]);
    } else if (analyze) {
      for (int i = 0; i < 2048; i++) {
        mpz_add_ui(total[i], total[i], frame[i]);
        if (i > 0) {
          assert(mpz_cmp_ui(total[i - 1], frame[i]) >= 0);
          mpz_sub_ui(total[i - 1], total[i - 1], frame[i]);
        }
      }
    } else if (generation) {
      mpz_set_ui(size, 0);
      for (int i = lowerBound; i <= upperBound; i++) {
        if (i == 0) {
          mpz_add_ui(size, size, frame[0]);
          mpz_sub_ui(size, size, frame[1]);
        } else {
          mpz_add_ui(size, size, frame[i]);
        }
      }
      gmp_printf("%zd: %Zd\n", step, size);
    } else if (alloc) {
      printf("%zd: %zd\n", step, frame[0]);
    } else if (!count) {
      fprintf(stderr, usage, argv[0]);
      return 1;
    }
    step++;
  }
  if (analyze) {
    for (int i = 0; i < 2047; i++) {
      gmp_printf("%d: %Zd\n", i, total[i]);
    }
    gmp_printf("saturated: %Zd\n", total[2047]);
  } else if (count) {
    printf("%zd collections\n", step);
  }
  return 0;
}
