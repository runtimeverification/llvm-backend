#include "runtime/header.h"

#include "runtime/alloc.h"

#include <cstdarg>
#include <vector>

extern "C" void *get_stderr(void) {
  return stderr;
}

static std::vector<match_log> match_logs;

void **get_match_fn_args(match_log *log) {
  return log->args.data();
}

extern "C" {
void reset_match_reason(void) {
  match_logs.clear();
}

match_log *getmatch_log(void) {
  return match_logs.data();
}

size_t getmatch_log_size(void) {
  return match_logs.size();
}

void add_match_success(void) {
  match_logs.push_back(
      {match_log::Success,
       nullptr,
       nullptr,
       nullptr,
       {},
       nullptr,
       nullptr,
       nullptr});
}

void add_match_fail_reason(
    void *subject, char const *pattern, char const *sort) {
  match_logs.push_back(
      {match_log::Fail, nullptr, nullptr, nullptr, {}, pattern, subject, sort});
}

void add_match_function(
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
      {match_log::Function, function, debug_name, result, args, nullptr,
       nullptr, nullptr});

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
