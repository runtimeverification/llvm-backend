#include <unistd.h>
#include <cstdio>

#include "runtime/header.h"

extern "C" {
  static block * dotK = (block *)((((uint64_t)getTagForSymbolName("dotk{}")) << 32) | 1);

  block * hook_KREFLECTION_parseKORE(string *kore) {
    char filename[17] = "parseKORE_XXXXXX";
    int fd = mkstemp(filename);
    int ret = write(fd, kore->data, len(kore));
    close(fd);

    block * parsed = dotK;

    if (ret != -1) {
      parsed = parseConfiguration(filename);
    }

    remove(filename);
    return parsed;
  }
}
