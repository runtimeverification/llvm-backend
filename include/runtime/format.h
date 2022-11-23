#ifndef RUNTIME_FORMAT_H
#define RUNTIME_FORMAT_H

#include <sstream>
#include <string>
#include <vector>

namespace kllvm::fmt {

namespace detail {

template <typename T>
std::string stringify(T &&arg) {
  auto ss = std::stringstream{};
  ss << std::forward<T>(arg);
  return ss.str();
}

template <typename... Args>
std::vector<std::string> stringify_all(Args &&...args) {
  auto ret = std::vector<std::string>{};
  (ret.push_back(stringify(std::forward<Args>(args))), ...);
  return ret;
}

} // namespace detail

/*
 * Implements a tiny fraction of the std::format API, but without requiring any
 * external dependencies for the K LLVM runtime library.
 *
 * Format strings use {} as their placeholder, and do not support _any_ escaping
 * of that string for simplicity. For example:
 *
 *   format("a: {}, b: {}", 1, "abc") == "a: 1, b: abc"
 *   format(X) == X for all strings not containing {}
 */
template <typename... Args>
std::string format(std::string const &format, Args &&...args) {
  auto string_args = detail::stringify_all(std::forward<Args>(args)...);
  auto arg_idx = 0;

  auto ss = std::stringstream{};
  for (auto i = 0u; i < format.size(); ++i) {
    if (i < format.size() - 1 && format[i] == '{' && format[i + 1] == '}') {
      if (arg_idx == string_args.size()) {
        throw std::invalid_argument("Too few arguments for format string");
      }

      ss << string_args[arg_idx++];
      ++i;
    } else {
      ss << format[i];
    }
  }

  if (arg_idx != string_args.size()) {
    throw std::invalid_argument("Too many arguments for format string");
  }

  return ss.str();
}

} // namespace kllvm::fmt

#endif
