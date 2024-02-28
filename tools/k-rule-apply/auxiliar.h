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
void *constructInitialConfiguration(kore_pattern const *);
void resetMatchReason();
match_log *getmatch_log();
size_t getmatch_logSize();
void printMatchResult(std::ostream &, match_log *, size_t, std::string const &);
void initStaticObjects();
}

void *constructInitialConfiguration(kore_pattern const *pattern, void *handle) {
  void *funcPtr = dlsym(handle, "constructInitialConfiguration");
  if (funcPtr == NULL) {
    return NULL;
  }
  auto f = reinterpret_cast<void *(*)(kore_pattern const *)>(funcPtr);
  return f(pattern);
}

void *resetMatchReason(void *handle) {
  void *funcPtr = dlsym(handle, "resetMatchReason");
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

size_t getmatch_logSize(void *handle) {
  void *funcPtr = dlsym(handle, "getmatch_logSize");
  if (funcPtr == NULL) {
    return -1;
  }
  auto f = reinterpret_cast<size_t (*)()>(funcPtr);
  return f();
}

void *printMatchResult(
    std::ostream &os, match_log *log, size_t logSize, std::string const &dir,
    void *handle) {
  void *funcPtr = dlsym(handle, "printMatchResult");
  if (funcPtr == NULL) {
    return NULL;
  }
  auto f = reinterpret_cast<
      void *(*)(std::ostream &, match_log *, size_t, std::string const &)>(
      funcPtr);
  return f(os, log, logSize, dir);
}

void *initStaticObjects(void *handle) {
  void *funcPtr = dlsym(handle, "initStaticObjects");
  if (funcPtr == NULL) {
    return NULL;
  }
  auto f = reinterpret_cast<void *(*)()>(funcPtr);
  return f();
}