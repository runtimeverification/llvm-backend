#include <cstdio>
#include <unistd.h>

#include "kllvm/util/temporary_file.h"
#include "runtime/header.h"

extern "C" {

SortString hook_KREFLECTION_printKORE(block *subject) {
  return printConfigurationToString(subject);
}
}
