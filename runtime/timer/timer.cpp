#include "runtime/header.h"
#include "runtime/timer.h"

#include <fstream>

REGISTER_TIMER(hook, nanoseconds);

extern "C" {

block *hook_TIMER_timerStart(void) {
  hook_timer_start();

  return dot_k();
}

block *hook_TIMER_timerStop(void) {
  if (!hook_timer_started()) {
    return dot_k();
  }

  hook_timer_stop();

  std::ofstream times_file;
  times_file.open("hook_times.txt", std::ios_base::app);
  times_file << hook_timer_measurement() << std::endl;

  return dot_k();
}

block *hook_TIMER_timer(mpz_ptr i) {
  std::ofstream times_file;
  times_file.open("hook_times.txt", std::ios_base::app);

  if (!hook_timer_started()) {
    hook_timer_start();
    times_file << int_to_string(i) << " ";

    return dot_k();
  }

  // Finish the previous timer and write the time to the file.
  hook_timer_stop();  
  times_file << hook_timer_measurement() << std::endl;

  // Start a new timer.
  hook_timer_start();
  times_file << int_to_string(i) << " ";

  return dot_k();
}

}
