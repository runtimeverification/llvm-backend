#include "runtime/header.h"

extern "C" {
bool hook_MAP_eq(SortMap, SortMap);
bool hook_RANGEMAP_eq(SortRangeMap, SortRangeMap);
bool hook_LIST_eq(SortList, SortList);
bool hook_SET_eq(SortSet, SortSet);
bool hook_INT_eq(SortInt, SortInt);
bool hook_FLOAT_trueeq(SortFloat, SortFloat);
bool hook_STRING_lt(SortString, SortString);

// NOLINTNEXTLINE(*-cognitive-complexity)
bool hook_KEQUAL_eq(block *arg1, block *arg2) {
  auto arg1intptr = (uint64_t)arg1;
  auto arg2intptr = (uint64_t)arg2;
  bool arg1lb = is_leaf_block(arg1);
  bool arg2lb = is_leaf_block(arg2);
  if (arg1lb == arg2lb) {
    if (arg1lb) {
      // Both arg1 and arg2 are constants.
      return arg1intptr == arg2intptr;
    } // Both arg1 and arg2 are blocks.
    uint64_t arg1hdrcanon = arg1->h.hdr & HDR_MASK;
    uint64_t arg2hdrcanon = arg2->h.hdr & HDR_MASK;
    if (arg1hdrcanon == arg2hdrcanon) {
      // Canonical headers of arg1 and arg2 are equal.
      // Both arg1 and arg2 are either strings or symbols.
      uint64_t arglayout = get_layout(arg1);
      if (arglayout == 0) {
        // Both arg1 and arg2 are strings.
        return hook_STRING_eq((string *)arg1, (string *)arg2);
      } // arglayout != 0
      // Both arg1 and arg2 are blocks.
      auto arglayoutshort = (uint16_t)arglayout;
      layout *layout_ptr = get_layout_data(arglayoutshort);
      uint8_t length = layout_ptr->nargs;
      for (uint8_t i = 0; i < length; i++) {
        uint64_t offset = layout_ptr->args[i].offset;
        uint64_t child1intptr = arg1intptr + offset;
        uint64_t child2intptr = arg2intptr + offset;
        uint16_t cat = layout_ptr->args[i].cat;
        switch (cat) {
        case MAP_LAYOUT: {
          map *map1ptr = (map *)(child1intptr);
          map *map2ptr = (map *)(child2intptr);
          bool cmp = hook_MAP_eq(map1ptr, map2ptr);
          if (!cmp) {
            return false;
          }
          break;
        }
        case RANGEMAP_LAYOUT: {
          auto *rangemap1ptr = (rangemap *)(child1intptr);
          auto *rangemap2ptr = (rangemap *)(child2intptr);
          bool cmp = hook_RANGEMAP_eq(rangemap1ptr, rangemap2ptr);
          if (!cmp) {
            return false;
          }
          break;
        }
        case LIST_LAYOUT: {
          list *list1ptr = (list *)(child1intptr);
          list *list2ptr = (list *)(child2intptr);
          bool cmp = hook_LIST_eq(list1ptr, list2ptr);
          if (!cmp) {
            return false;
          }
          break;
        }
        case SET_LAYOUT: {
          set *set1ptr = (set *)(child1intptr);
          set *set2ptr = (set *)(child2intptr);
          bool cmp = hook_SET_eq(set1ptr, set2ptr);
          if (!cmp) {
            return false;
          }
          break;
        }
        case INT_LAYOUT: {
          auto *int1ptrptr = (mpz_ptr *)(child1intptr);
          auto *int2ptrptr = (mpz_ptr *)(child2intptr);
          bool cmp = hook_INT_eq(*int1ptrptr, *int2ptrptr);
          if (!cmp) {
            return false;
          }
          break;
        }
        case FLOAT_LAYOUT: {
          auto **float1ptrptr = (floating **)(child1intptr);
          auto **float2ptrptr = (floating **)(child2intptr);
          bool cmp = hook_FLOAT_trueeq(*float1ptrptr, *float2ptrptr);
          if (!cmp) {
            return false;
          }
          break;
        }
        case STRINGBUFFER_LAYOUT: {
          abort();
        }
        case BOOL_LAYOUT: {
          bool *bool1ptr = (bool *)(child1intptr);
          bool *bool2ptr = (bool *)(child2intptr);
          bool cmp = *bool1ptr == *bool2ptr;
          if (!cmp) {
            return false;
          }
          break;
        }
        case SYMBOL_LAYOUT: {
          auto **child1ptrptr = (block **)(child1intptr);
          auto **child2ptrptr = (block **)(child2intptr);
          bool cmp = hook_KEQUAL_eq(*child1ptrptr, *child2ptrptr);
          if (!cmp) {
            return false;
          }
          break;
        }
        case VARIABLE_LAYOUT: {
          auto **var1ptrptr = (block **)(child1intptr);
          auto **var2ptrptr = (block **)(child2intptr);
          bool cmp
              = hook_STRING_eq((string *)*var1ptrptr, (string *)*var2ptrptr);
          if (!cmp) {
            return false;
          }
          break;
        }
        case MINT_LAYOUT + 32: {
          auto *child1ptr = (int32_t *)(child1intptr);
          auto *child2ptr = (int32_t *)(child2intptr);
          bool cmp = *child1ptr == *child2ptr;
          if (!cmp) {
            return false;
          }
          break;
        }
        case MINT_LAYOUT + 64: {
          auto *child1ptr = (int64_t *)(child1intptr);
          auto *child2ptr = (int64_t *)(child2intptr);
          bool cmp = *child1ptr == *child2ptr;
          if (!cmp) {
            return false;
          }
          break;
        }
        case MINT_LAYOUT + 160: {
          auto *child1ptr = (int64_t *)(child1intptr);
          auto *child2ptr = (int64_t *)(child2intptr);
          bool cmp
              = child1ptr[0] == child2ptr[0] && child1ptr[1] == child2ptr[1]
                && (child1ptr[2] & UINT32_MAX) == (child2ptr[2] & UINT32_MAX);
          if (!cmp) {
            return false;
          }
          break;
        }
        case MINT_LAYOUT + 256: {
          auto *child1ptr = (int64_t *)(child1intptr);
          auto *child2ptr = (int64_t *)(child2intptr);
          bool cmp
              = child1ptr[0] == child2ptr[0] && child1ptr[1] == child2ptr[1]
                && child1ptr[2] == child2ptr[2] && child1ptr[3] == child2ptr[3];
          if (!cmp) {
            return false;
          }
          break;
        }

        default: abort();
        }
      }
      return true;

    } // arg1hdrcanon != arg2hdrcanon
    return false;

  } // arg1lb != arg2lb
  // Between arg1 and arg2, one is a constant and one is a block.
  return false;
}

// We arbitrarily impose the following ordering betwene different types of
// blocks:
// - Constants < Blocks.
// - Symbols < Strings.
// - Symbols with different tags are ordered based on their tags.
// - Symbols with same tags are ordered be lexicographically comparing their
//   childen.
bool hook_KEQUAL_lt(block *arg1, block *arg2) {
  auto arg1intptr = (uint64_t)arg1;
  auto arg2intptr = (uint64_t)arg2;
  bool isconstant1 = is_leaf_block(arg1);
  bool isconstant2 = is_leaf_block(arg2);
  if (isconstant1 != isconstant2) {
    // Between arg1 and arg2, one is a constant and one is not.
    return isconstant1;
  }
  if (isconstant1) {
    // Both arg1 and arg2 are constants.
    return arg1intptr < arg2intptr;
  } // Both arg1 and arg2 are blocks.
  uint16_t arg1layout = get_layout(arg1);
  uint16_t arg2layout = get_layout(arg2);
  if (arg1layout == 0 && arg2layout == 0) {
    // Both arg1 and arg2 are strings.
    return hook_STRING_lt((string *)arg1, (string *)arg2);
  }
  if (arg1layout == 0 || arg2layout == 0) {
    // One of arg1, arg2 is a string, the other is a symbol.
    return arg2layout == 0;
  } // Both arg1 and arg2 are symbols.
  uint32_t arg1tag = tag(arg1);
  uint32_t arg2tag = tag(arg2);
  if (arg1tag != arg2tag) {
    return arg1tag < arg2tag;
  }
  assert(arg1layout == arg2layout);
  layout *layout_ptr = get_layout_data(arg1layout);
  uint8_t length = layout_ptr->nargs;
  for (uint8_t i = 0; i < length; i++) {
    uint64_t offset = layout_ptr->args[i].offset;
    uint16_t cat = layout_ptr->args[i].cat;
    switch (cat) {
    case INT_LAYOUT: {
      auto *int1ptrptr = (mpz_ptr *)(arg1intptr + offset);
      auto *int2ptrptr = (mpz_ptr *)(arg2intptr + offset);
      int cmp = mpz_cmp(*int1ptrptr, *int2ptrptr);
      if (cmp != 0) {
        return cmp < 0;
      }
      break;
    }
    case MAP_LAYOUT:
    case RANGEMAP_LAYOUT:
    case LIST_LAYOUT:
    case SET_LAYOUT:
    case FLOAT_LAYOUT:
    case STRINGBUFFER_LAYOUT:
    case BOOL_LAYOUT:
    case SYMBOL_LAYOUT:
    case VARIABLE_LAYOUT: {
      abort(); // Implement when needed.
    }
    default: abort();
    }
  }
  return false;
}

bool hook_KEQUAL_ne(block *arg1, block *arg2) {
  return !hook_KEQUAL_eq(arg1, arg2);
}

bool hook_BOOL_eq(bool arg1, bool arg2) {
  return arg1 == arg2;
}
}
