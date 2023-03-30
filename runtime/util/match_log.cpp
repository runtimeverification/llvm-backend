#include "runtime/header.h"

#include "runtime/alloc.h"

#include <vector>

extern "C" void *getStderr(void) {
  return stderr;
}

static std::vector<MatchLog> matchLog;

void resetMatchReason(void) {
  matchLog.clear();
}

MatchLog *getMatchLog(void) {
  return &matchLog[0];
}

void **getMatchFnArgs(MatchLog *log) {
  return &log->args[0];
}

size_t getMatchLogSize(void) {
  return matchLog.size();
}

extern "C" {

void addMatchSuccess(void) {
  matchLog.push_back(
      {MatchLog::SUCCESS, NULL, NULL, NULL, {}, NULL, NULL, NULL});
}

void addMatchFailReason(void *subject, char *pattern, char *sort) {
  matchLog.push_back(
      {MatchLog::FAIL, NULL, NULL, NULL, {}, pattern, subject, sort});
}

void addMatchFunction(char *debugName, char *function, void *result, ...) {
  va_list ap;
  va_start(ap, result);

  std::vector<void *> args;
  while (true) {
    void *arg = va_arg(ap, void *);
    if (!arg)
      break;
    args.push_back(arg);
  }

  matchLog.push_back(
      {MatchLog::FUNCTION, function, debugName, result, args, NULL, NULL,
       NULL});

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
