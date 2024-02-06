#include "runtime/header.h"

#include "runtime/alloc.h"

#include <cstdarg>
#include <vector>

extern "C" void *getStderr(void) {
  return stderr;
}

static std::vector<MatchLog> matchLog;

void **getMatchFnArgs(MatchLog *log) {
  return log->args.data();
}

extern "C" {
void resetMatchReason(void) {
  matchLog.clear();
}

MatchLog *getMatchLog(void) {
  return matchLog.data();
}

size_t getMatchLogSize(void) {
  return matchLog.size();
}

void addMatchSuccess(void) {
  matchLog.push_back(
      {MatchLog::SUCCESS,
       nullptr,
       nullptr,
       nullptr,
       {},
       nullptr,
       nullptr,
       nullptr});
}

void addMatchFailReason(void *subject, char const *pattern, char const *sort) {
  matchLog.push_back(
      {MatchLog::FAIL, nullptr, nullptr, nullptr, {}, pattern, subject, sort});
}

void addMatchFunction(
    char const *debugName, char const *function, void *result, ...) {
  va_list ap;
  va_start(ap, result);

  std::vector<void *> args;
  while (true) {
    void *arg = va_arg(ap, void *);
    if (!arg) {
      break;
    }
    args.push_back(arg);
  }

  matchLog.push_back(
      {MatchLog::FUNCTION, function, debugName, result, args, nullptr, nullptr,
       nullptr});

  va_end(ap);
}
}

#define DEFINE_GDB_PY_SCRIPT(script_path, script_name)                         \
  asm("\
.pushsection \".debug_gdb_scripts\", \"MS\",@progbits,1\n\
.byte 4 /* Python inlined */\n\
.ascii \"" script_name "\\n\"\n\
.incbin \"" script_path "/" script_name "\"\n\
.byte 0 \n\
.popsection \n\
");

#ifndef __APPLE__
DEFINE_GDB_PY_SCRIPT(GDB_SCRIPT_PATH, GDB_SCRIPT_NAME)
#endif
