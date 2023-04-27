#include <cstdio>
#include <unistd.h>

#include "kllvm/util/FileRAII.h"
#include "runtime/header.h"

extern "C" {
static block *dotK = leaf_block(getTagForSymbolName("dotk{}"));

block *hook_KREFLECTION_parseKORE(SortString kore) {
  block *parsed = dotK;
  char filename[17] = "parseKORE_XXXXXX";

  int fd = FileRAII(filename).getTempFd();

  bool failed = write(fd, kore->data, len(kore)) == -1;

  if (!failed) {
    parsed = parseConfiguration(filename);
  }

  return parsed;
}

SortString hook_KREFLECTION_printKORE(block *subject) {
  return printConfigurationToString(subject);
}
}
