#include <unistd.h>
#include <cstdio>

#include "runtime/header.h"

extern "C" {
  static block * dotK = (block *)((((uint64_t)getTagForSymbolName("dotk{}")) << 32) | 1);

  block * hook_KREFLECTION_parseKORE(string *kore) {
    block * parsed = dotK;
    char initbuf[] = "[initial-configuration{}(";
    char endbuf[] = ")] module TMP endmodule []";
    char filename[17] = "parseKORE_XXXXXX";

    int fd = mkstemp(filename);

    int ret = write(fd, initbuf, sizeof(initbuf) - 1);
    ret -= write(fd, kore->data, len(kore));
    ret -= write(fd, endbuf, sizeof(endbuf) - 1);

    close(fd);

    /* If ret is negative, one of the writes returned an error so return .K */
    if (ret >= 0) {
      parsed = parseConfiguration(filename);
    }

    remove(filename);

    return parsed;
  }
}
