#include <runtime/header.h>

static std::unordered_set<SortBytes> copy_on_write_set = {};

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
  if (copy_on_write_set.find(b) != copy_on_write_set.end()) {
    b = copy_bytes(b);
  }
}

extern "C" {

void make_copy_on_write(SortBytes b) {
  copy_on_write_set.insert(b);
}
}
