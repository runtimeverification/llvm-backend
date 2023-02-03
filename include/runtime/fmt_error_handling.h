#ifndef FMT_ERROR_HANDLING_H
#define FMT_ERROR_HANDLING_H

#include <fmt/format.h>

#define KLLVM_HOOK_INVALID_ARGUMENT(...)                                       \
  do {                                                                         \
    auto err_msg                                                               \
        = ::fmt::format("[{}]: {}", __func__, ::fmt::format(__VA_ARGS__));     \
    throw std::invalid_argument(err_msg);                                      \
  } while (false)

#endif // FMT_ERROR_HANDLING_H
