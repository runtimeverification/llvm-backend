#ifndef TIMER_H
#define TIMER_H

#include <chrono>

// Macro to register a new timer.
// Timers are implemented using the std::chrono::high_resolution_clock.
// The unit should be one of the duration types provided in std::chrono,
// e.g. seconds, microseconds, etc.
#define REGISTER_TIMER(name, unit) \
  static std::chrono::high_resolution_clock::time_point name ## _clock_start; \
  static std::chrono::high_resolution_clock::time_point name ## _clock_stop; \
  void name ## _timer_start() { \
    name ## _clock_start = std::chrono::high_resolution_clock::now(); \
  } \
  void name ## _timer_stop() { \
    name ## _clock_stop = std::chrono::high_resolution_clock::now(); \
  } \
  uint64_t name ## _timer_measurement() { \
    return std::chrono::duration_cast<std::chrono::unit>( \
       name ## _clock_stop -  name ## _clock_start).count(); \
  }

#endif // TIMER_H
