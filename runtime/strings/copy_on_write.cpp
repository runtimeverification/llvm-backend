#include <runtime/header.h>

extern "C" bool enable_strict_bytes;

namespace {

SortBytes copy_bytes(SortBytes b) {
  auto new_len = len(b);
  auto *ret = static_cast<string *>(koreAllocToken(sizeof(string) + new_len));
  init_with_len(ret, new_len);
  memcpy(&(ret->data), &(b->data), new_len * sizeof(char));
  return ret;
}

} // namespace

void copy_if_needed(SortBytes &b) {
  if (enable_strict_bytes) {
    b = copy_bytes(b);
  }
}
