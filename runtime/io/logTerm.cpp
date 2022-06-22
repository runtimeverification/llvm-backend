#include "runtime/header.h"

#include <kllvm/printer/printer.h>

#include <cstdio>
#include <iostream>
#include <unistd.h>

static block *dotK = leaf_block(getTagForSymbolName("dotk{}"));

extern "C" {

extern char kompiled_directory;

block *hook_IO_log(string *path, string *msg);
int getTag(block *term);

SortKItem hook_IO_logTerm(SortString path, SortKItem term) {
  string *msg = printConfigurationToString(term);
  hook_IO_log(path, msg);
  return term;
}

SortK hook_IO_traceTerm(block *term) {
  char filename[17] = "traceKORE_XXXXXX";
  int fd = mkstemp(filename);

  FILE *fp = fdopen(fd, "w");

  // Ensure that the term is injected into KItem correctly; if we don't do this
  // then the unparsed KORE ends up with a (null) in it which breaks the
  // printing below.
  printSortedConfigurationToFile(fp, term, "SortKItem{}");
  fflush(fp);

  kllvm::printKORE(std::cerr, &kompiled_directory, filename, false, true);

  close(fd);
  remove(filename);

  return dotK;
}
}
