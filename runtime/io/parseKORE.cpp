#include <cstdio>
#include <unistd.h>

#include "kllvm/util/temporary_file.h"
#include "runtime/header.h"

extern "C" {
static block *dotK = leaf_block(getTagForSymbolName("dotk{}"));

block *hook_KREFLECTION_parseKORE(SortString kore) {
  block *parsed = dotK;
  auto temp_file = temporary_file("parseKORE_XXXXXX");

  int fd = temp_file.getTempFd();

  bool failed = write(fd, kore->data, len(kore)) == -1;

  if (!failed) {
    parsed = parseConfiguration(temp_file.getFilename().c_str());
  }

  return parsed;
}

SortString hook_KREFLECTION_printKORE(block *subject) {
  return printConfigurationToString(subject);
}
}
