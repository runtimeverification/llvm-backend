#include <cstdint>
#include <ctime>
#include <iostream>

static struct timespec start { };

extern "C" {
void start_clock() {
  clock_gettime(CLOCK_MONOTONIC, &start);
}

void stop_clock(uint64_t ordinal) {
  struct timespec stop { };
  clock_gettime(CLOCK_MONOTONIC, &stop);
  int64_t diff_s = stop.tv_sec - start.tv_sec;
  int64_t diff_ns = stop.tv_nsec - start.tv_nsec;

  uint64_t diff_ns_total = diff_s * 1000000000 + diff_ns;

  std::cerr << ordinal << " " << diff_ns_total << std::endl;
}
}
