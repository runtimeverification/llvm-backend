#include "runtime/header.h"
extern "C" {
block *hook_IO_log(string *path, string *msg);

SortKItem hook_IO_logTerm(SortString path, SortKItem term) {
  string *msg = printConfigurationToString(term);
  hook_IO_log(path, msg);
  return term;
}
}
