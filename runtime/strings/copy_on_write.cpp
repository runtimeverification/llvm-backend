#include <runtime/header.h>

static std::unordered_set<SortBytes> copy_on_write_set = {};

extern "C" void make_copy_on_write(SortBytes b);

namespace {

SortBytes copy_bytes(SortBytes b) {
  auto new_len = len(b);
  auto *ret = static_cast<string *>(koreAllocToken(sizeof(string) + new_len));
  init_with_len(ret, new_len);
  memcpy(&(ret->data), &(b->data), new_len * sizeof(char));
  return ret;
}

void make_cow_rec_internal(
    writer *file, block *subject, char const *sort, bool isVar,
    void *state_ptr) {
  uint8_t isConstant = ((uintptr_t)subject) & 3;
  if (isConstant) {
    return;
  }

  uint16_t layout = get_layout(subject);
  if (!layout) {
    auto *str = (string *)subject;
    make_copy_on_write(str);
    return;
  }

  auto nothing = [](auto...) {};
  visitor callbacks
      = {make_cow_rec_internal,
         nothing, // map
         nothing, // list
         nothing, // set
         nothing, // int
         nothing, // float
         nothing, // bool
         nothing, // stringb
         nothing, // mint
         nothing, // comma
         nothing}; // rangenm

  visitChildren(subject, file, &callbacks, state_ptr);
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

void make_copy_on_write_rec(block *b) {
  make_cow_rec_internal(nullptr, b, nullptr, false, nullptr);
}
}
