#include "runtime/header.h"

#include "runtime/alloc.h"

#include <cstdarg>
#include <vector>

extern "C" void *getStderr(void) {
  return stderr;
}

static std::vector<match_log> match_logs;

void **getMatchFnArgs(match_log *log) {
  return log->args.data();
}

extern "C" {
void resetMatchReason(void) {
  match_logs.clear();
}

match_log *getmatch_log(void) {
  return match_logs.data();
}

size_t getmatch_logSize(void) {
  return match_logs.size();
}

void addMatchSuccess(void) {
  match_logs.push_back(
      {match_log::SUCCESS,
       nullptr,
       nullptr,
       nullptr,
       {},
       nullptr,
       nullptr,
       nullptr});
}

void addMatchFailReason(void *subject, char const *pattern, char const *sort) {
  match_logs.push_back(
      {match_log::FAIL, nullptr, nullptr, nullptr, {}, pattern, subject, sort});
}

void addMatchFunction(
    char const *debug_name, char const *function, void *result, ...) {
  // This function needs to use C variadic arguments because it's called from
  // generated LLVM IR.
  // NOLINTBEGIN(*-vararg)
  va_list ap; // NOLINT(*-init-variables)
  va_start(ap, result);

  std::vector<void *> args;
  while (true) {
    void *arg = va_arg(ap, void *);
    if (!arg) {
      break;
    }
    args.push_back(arg);
  }

  match_logs.push_back(
      {match_log::FUNCTION, function, debug_name, result, args, nullptr, nullptr,
       nullptr});

  va_end(ap);
  // NOLINTEND(*-vararg)
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
