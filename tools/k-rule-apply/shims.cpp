#include "shims.h"

#include <dlfcn.h>
#include <iostream>

#include "runtime/header.h"

namespace kllvm {

block *parse_initial_configuration(std::string const &filename, void *handle) {
  auto *parse_file = reinterpret_cast<kore_pattern *(*)(char const *)>(
      dlsym(handle, "kore_pattern_parse_file"));
  if (!parse_file) {
    return nullptr;
  }

  auto *construct = reinterpret_cast<block *(*)(kore_pattern *)>(
      dlsym(handle, "kore_pattern_construct"));
  if (!construct) {
    return nullptr;
  }

  return construct(parse_file(filename.c_str()));
}

char *get_match_function_name(
    std::string const &definition, std::string const &label, void *handle) {
  auto *get_name = reinterpret_cast<char *(*)(char const *, char const *)>(
      dlsym(handle, "kore_match_function_name"));
  if (!get_name) {
    return nullptr;
  }

  return get_name(definition.c_str(), label.c_str());
}

void *reset_match_reason(void *handle) {
  void *func_ptr = dlsym(handle, "reset_match_reason");
  if (func_ptr == nullptr) {
    return nullptr;
  }
  auto f = reinterpret_cast<void *(*)()>(func_ptr);
  return f();
}

match_log *getmatch_log(void *handle) {
  void *func_ptr = dlsym(handle, "getmatch_log");
  if (func_ptr == nullptr) {
    return nullptr;
  }
  auto f = reinterpret_cast<match_log *(*)()>(func_ptr);
  return f();
}

size_t getmatch_log_size(void *handle) {
  void *func_ptr = dlsym(handle, "getmatch_log_size");
  if (func_ptr == nullptr) {
    return -1;
  }
  auto f = reinterpret_cast<size_t (*)()>(func_ptr);
  return f();
}

void *print_match_result(
    std::ostream &os, match_log *log, size_t log_size, std::string const &dir,
    void *handle) {
  void *func_ptr = dlsym(handle, "print_match_result");
  if (func_ptr == nullptr) {
    return nullptr;
  }
  auto f = reinterpret_cast<
      void *(*)(std::ostream &, match_log *, size_t, std::string const &)>(
      func_ptr);
  return f(os, log, log_size, dir);
}

void *init_static_objects(void *handle) {
  void *func_ptr = dlsym(handle, "init_static_objects");
  if (func_ptr == nullptr) {
    return nullptr;
  }
  auto f = reinterpret_cast<void *(*)()>(func_ptr);
  return f();
}

} // namespace kllvm
