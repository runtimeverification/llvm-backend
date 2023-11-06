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
void *constructInitialConfiguration(const KOREPattern *);
void resetMatchReason();
MatchLog *getMatchLog();
size_t getMatchLogSize();
void printMatchResult(std::ostream &, MatchLog *, size_t, std::string const &);
void initStaticObjects();
}

void *constructInitialConfiguration(const KOREPattern *pattern, void *handle) {
  void *funcPtr = dlsym(handle, "constructInitialConfiguration");
  if (funcPtr == NULL) {
    return NULL;
  }
  auto f = reinterpret_cast<void *(*)(const KOREPattern *)>(funcPtr);
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

MatchLog *getMatchLog(void *handle) {
  void *funcPtr = dlsym(handle, "getMatchLog");
  if (funcPtr == NULL) {
    return NULL;
  }
  auto f = reinterpret_cast<MatchLog *(*)()>(funcPtr);
  return f();
}

size_t getMatchLogSize(void *handle) {
  void *funcPtr = dlsym(handle, "getMatchLogSize");
  if (funcPtr == NULL) {
    return -1;
  }
  auto f = reinterpret_cast<size_t (*)()>(funcPtr);
  return f();
}

void *printMatchResult(
    std::ostream &os, MatchLog *log, size_t logSize, std::string const &dir,
    void *handle) {
  void *funcPtr = dlsym(handle, "printMatchResult");
  if (funcPtr == NULL) {
    return NULL;
  }
  auto f = reinterpret_cast<void *(*)(std::ostream &, MatchLog *, size_t,
                                      std::string const &)>(funcPtr);
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