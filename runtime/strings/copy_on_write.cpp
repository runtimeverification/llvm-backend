#include <runtime/header.h>

extern "C" bool enable_mutable_bytes;

namespace {

SortBytes copy_bytes(SortBytes b) {
  auto new_len = len(b);
  auto *ret = static_cast<string *>(kore_alloc_token(sizeof(string) + new_len));
  init_with_len(ret, new_len);
  memcpy(&(ret->data), &(b->data), new_len * sizeof(char));
  return ret;
}

} // namespace

extern "C" bool hook_BYTES_mutableBytesEnabled() {
  return enable_mutable_bytes;
}

void copy_if_needed(SortBytes &b) {
  if (!hook_BYTES_mutableBytesEnabled()) {
    b = copy_bytes(b);
  }
}
