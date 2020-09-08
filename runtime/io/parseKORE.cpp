#include <unistd.h>
#include <cstdio>

#include "runtime/header.h"

extern "C" {
  static block * dotK = leaf_block(getTagForSymbolName("dotk{}"));

  block * hook_KREFLECTION_parseKORE(SortString kore) {
    block * parsed = dotK;
    char filename[17] = "parseKORE_XXXXXX";

    int fd = mkstemp(filename);

    bool failed = write(fd, kore->data, len(kore)) == -1;

    close(fd);

    if (!failed) {
      parsed = parseConfiguration(filename);
    }

    remove(filename);

    return parsed;
  }
}
