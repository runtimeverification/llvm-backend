#include <fstream>

#include "runtime/header.h"
#include "runtime/timer.h"

REGISTER_TIMER(hook, nanoseconds);

extern "C" {

block *hook_TIMER_timerStart(void) {
  hook_timer_start();

  return dot_k();
}

block *hook_TIMER_timerStop(void) {
  hook_timer_stop();

  std::ofstream times_file;
  times_file.open("hook_times.txt", std::ios_base::app);
  times_file << hook_timer_measurement() << std::endl;

  return dot_k();
}
}
