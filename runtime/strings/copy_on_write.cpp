#include <runtime/header.h>

static std::unordered_set<SortBytes> copy_on_write_set = {};

extern "C" void make_copy_on_write(SortBytes b);

namespace {

void make_cow_rec_internal(
    writer *file, block *subject, char const *sort, bool isVar,
    void *state_ptr);

SortBytes copy_bytes(SortBytes b) {
  auto new_len = len(b);
  auto *ret = static_cast<string *>(koreAllocToken(sizeof(string) + new_len));
  init_with_len(ret, new_len);
  memcpy(&(ret->data), &(b->data), new_len * sizeof(char));
  return ret;
}

void make_map_cow(
    writer *file, map *map, char const *unit, char const *element,
    char const *concat, void *state) {
  for (auto iter = map->begin(); iter != map->end(); ++iter) {
    make_cow_rec_internal(file, iter->first, nullptr, false, state);
    make_cow_rec_internal(file, iter->second, nullptr, false, state);
  }
}

void make_list_cow(
    writer *file, list *list, char const *unit, char const *element,
    char const *concat, void *state) {
  for (auto iter = list->begin(); iter != list->end(); ++iter) {
    make_cow_rec_internal(file, *iter, nullptr, false, state);
  }
}

void make_set_cow(
    writer *file, set *set, char const *unit, char const *element,
    char const *concat, void *state) {
  for (auto iter = set->begin(); iter != set->end(); ++iter) {
    make_cow_rec_internal(file, *iter, nullptr, false, state);
  }
}

void make_rangemap_cow(
    writer *file, rangemap *map, char const *unit, char const *element,
    char const *concat, void *state) {
  for (auto iter = rng_map::ConstRangeMapIterator<KElem, KElem>(*map);
       iter.has_next(); ++iter) {
    make_cow_rec_internal(file, iter->first.start(), nullptr, false, state);
    make_cow_rec_internal(file, iter->first.end(), nullptr, false, state);
    make_cow_rec_internal(file, iter->second, nullptr, false, state);
  }
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
         make_map_cow,
         make_list_cow,
         make_set_cow,
         nothing,
         nothing,
         nothing,
         nothing,
         nothing,
         nothing,
         make_rangemap_cow};

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
