#include "runtime/header.h"
extern "C" {
  block * hook_IO_log(string * path, string * msg);

  block *hook_IO_logTerm(string *path, block *term) {
    string *msg = printConfigurationToString(term);
    hook_IO_log(path, msg);
    return term;
  }
}
