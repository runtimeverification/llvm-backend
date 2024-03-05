#include <kllvm/ast/AST.h>
#include <kllvm/parser/KOREParser.h>

#include <llvm/Support/CommandLine.h>

#include <cstdlib>
#include <dlfcn.h>
#include <fstream>
#include <iostream>
#include <optional>

#include "runtime/header.h"

using namespace kllvm;

extern "C" {
void *construct_initial_configuration(kore_pattern const *);
void reset_match_reason();
match_log *getmatch_log();
size_t getmatch_log_size();
void print_match_result(
    std::ostream &, match_log *, size_t, std::string const &);
void init_static_objects();
}

void *
construct_initial_configuration(kore_pattern const *pattern, void *handle) {
  void *funcPtr = dlsym(handle, "construct_initial_configuration");
  if (funcPtr == NULL) {
    return NULL;
  }
  auto f = reinterpret_cast<void *(*)(kore_pattern const *)>(funcPtr);
  return f(pattern);
}

void *reset_match_reason(void *handle) {
  void *funcPtr = dlsym(handle, "reset_match_reason");
  if (funcPtr == NULL) {
    return NULL;
  }
  auto f = reinterpret_cast<void *(*)()>(funcPtr);
  return f();
}

match_log *getmatch_log(void *handle) {
  void *funcPtr = dlsym(handle, "getmatch_log");
  if (funcPtr == NULL) {
    return NULL;
  }
  auto f = reinterpret_cast<match_log *(*)()>(funcPtr);
  return f();
}

size_t getmatch_log_size(void *handle) {
  void *funcPtr = dlsym(handle, "getmatch_log_size");
  if (funcPtr == NULL) {
    return -1;
  }
  auto f = reinterpret_cast<size_t (*)()>(funcPtr);
  return f();
}

void *print_match_result(
    std::ostream &os, match_log *log, size_t logSize, std::string const &dir,
    void *handle) {
  void *funcPtr = dlsym(handle, "print_match_result");
  if (funcPtr == NULL) {
    return NULL;
  }
  auto f = reinterpret_cast<
      void *(*)(std::ostream &, match_log *, size_t, std::string const &)>(
      funcPtr);
  return f(os, log, logSize, dir);
}

void *init_static_objects(void *handle) {
  void *funcPtr = dlsym(handle, "init_static_objects");
  if (funcPtr == NULL) {
    return NULL;
  }
  auto f = reinterpret_cast<void *(*)()>(funcPtr);
  return f();
}