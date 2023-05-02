#include <kllvm/printer/printer.h>
#include <unistd.h>

#include <cstdio>
#include <iostream>

#include "kllvm/util/temporary_file.h"
#include "runtime/header.h"

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
  auto temp_file = temporary_file("traceKORE_XXXXXX");
  auto fp = temp_file.file_pointer("w");

  // Ensure that the term is injected into KItem correctly; if we don't do this
  // then the unparsed KORE ends up with a (null) in it which breaks the
  // printing below.
  printSortedConfigurationToFile(fp, term, "SortKItem{}");
  fflush(fp);

  kllvm::printKORE(
      std::cerr, &kompiled_directory, temp_file.filename(), false, true);

  return dotK;
}
}
