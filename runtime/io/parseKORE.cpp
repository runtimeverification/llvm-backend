#include <unistd.h>
#include <cstdio>

#include "runtime/header.h"

extern "C" {
  static block * dotK = (block *)((((uint64_t)getTagForSymbolName("dotk{}")) << 32) | 1);

  block * hook_KREFLECTION_parseKORE(string *kore) {
    char filename[17] = "parseKORE_XXXXXX";
    int fd = mkstemp(filename);
    char initbuf[] = "[initial-configuration{}(";
    write(fd, initbuf, sizeof(initbuf) - 1);
    write(fd, kore->data, len(kore));
    char endbuf[] = "] module TMP endmodule []";
    write(fd, endbuf, sizeof(endbuf) - 1);
    close(fd);

    block * parsed = parseConfiguration(filename);

    remove(filename);
    return parsed;
  }
}
