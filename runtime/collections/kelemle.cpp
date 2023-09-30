#include "runtime/header.h"

extern "C" {
bool hook_MAP_eq(SortMap, SortMap);
bool hook_RANGEMAP_eq(SortRangeMap, SortRangeMap);
bool hook_LIST_eq(SortList, SortList);
bool hook_SET_eq(SortSet, SortSet);
bool hook_INT_eq(SortInt, SortInt);
bool hook_FLOAT_trueeq(SortFloat, SortFloat);
bool hook_STRING_lt(SortString, SortString);

bool hook_KEQUAL_eq(block *arg1, block *arg2) {
  uint64_t arg1intptr = (uint64_t)arg1;
  uint64_t arg2intptr = (uint64_t)arg2;
  bool arg1lb = is_leaf_block(arg1);
  bool arg2lb = is_leaf_block(arg2);
  if (arg1lb == arg2lb) {
    if (arg1lb) {
      // Both arg1 and arg2 are constants.
      return arg1intptr == arg2intptr;
    } else {
      // Both arg1 and arg2 are blocks.
      uint64_t arg1hdrcanon = arg1->h.hdr & HDR_MASK;
      uint64_t arg2hdrcanon = arg2->h.hdr & HDR_MASK;
      if (arg1hdrcanon == arg2hdrcanon) {
        // Canonical headers of arg1 and arg2 are equal.
        // Both arg1 and arg2 are either strings or symbols.
        uint64_t arglayout = get_layout(arg1);
        if (arglayout == 0) {
          // Both arg1 and arg2 are strings.
          return hook_STRING_eq((string *)arg1, (string *)arg2);
        } else { // arglayout != 0
          // Both arg1 and arg2 are blocks.
          uint16_t arglayoutshort = (uint16_t)arglayout;
          layout *layoutPtr = getLayoutData(arglayoutshort);
          uint8_t length = layoutPtr->nargs;
          for (uint8_t i = 0; i < length; i++) {
            uint64_t offset = layoutPtr->args[i].offset;
            uint64_t child1intptr = arg1intptr + offset;
            uint64_t child2intptr = arg2intptr + offset;
            uint16_t cat = layoutPtr->args[i].cat;
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
              rangemap *rangemap1ptr = (rangemap *)(child1intptr);
              rangemap *rangemap2ptr = (rangemap *)(child2intptr);
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
              mpz_ptr *int1ptrptr = (mpz_ptr *)(child1intptr);
              mpz_ptr *int2ptrptr = (mpz_ptr *)(child2intptr);
              bool cmp = hook_INT_eq(*int1ptrptr, *int2ptrptr);
              if (!cmp) {
                return false;
              }
              break;
            }
            case FLOAT_LAYOUT: {
              floating **float1ptrptr = (floating **)(child1intptr);
              floating **float2ptrptr = (floating **)(child2intptr);
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
            case BYTES_LAYOUT:
            case SYMBOL_LAYOUT: {
              block **child1ptrptr = (block **)(child1intptr);
              block **child2ptrptr = (block **)(child2intptr);
              bool cmp = hook_KEQUAL_eq(*child1ptrptr, *child2ptrptr);
              if (!cmp) {
                return false;
              }
              break;
            }
            case VARIABLE_LAYOUT: {
              block **var1ptrptr = (block **)(child1intptr);
              block **var2ptrptr = (block **)(child2intptr);
              bool cmp = hook_STRING_eq(
                  (string *)*var1ptrptr, (string *)*var2ptrptr);
              if (!cmp) {
                return false;
              }
              break;
            }
            default: abort();
            }
          }
          return true;
        }
      } else { // arg1hdrcanon != arg2hdrcanon
        return false;
      }
    }
  } else { // arg1lb != arg2lb
    // Between arg1 and arg2, one is a constant and one is a block.
    return false;
  }
}

// We arbitrarily impose the following ordering betwene different types of
// blocks:
// - Constants < Blocks.
// - Symbols < Strings.
// - Symbols with different tags are ordered based on their tags.
// - Symbols with same tags are ordered be lexicographically comparing their
//   childen.
bool hook_KEQUAL_lt(block *arg1, block *arg2) {
  uint64_t arg1intptr = (uint64_t)arg1;
  uint64_t arg2intptr = (uint64_t)arg2;
  bool isconstant1 = is_leaf_block(arg1);
  bool isconstant2 = is_leaf_block(arg2);
  if (isconstant1 != isconstant2) {
    // Between arg1 and arg2, one is a constant and one is not.
    return isconstant1;
  } else if (isconstant1) {
    // Both arg1 and arg2 are constants.
    return arg1intptr < arg2intptr;
  } else {
    // Both arg1 and arg2 are blocks.
    uint16_t arg1layout = get_layout(arg1);
    uint16_t arg2layout = get_layout(arg2);
    if (arg1layout == 0 && arg2layout == 0) {
      // Both arg1 and arg2 are strings.
      return hook_STRING_lt((string *)arg1, (string *)arg2);
    } else if (arg1layout == 0 || arg2layout == 0) {
      // One of arg1, arg2 is a string, the other is a symbol.
      return arg2layout == 0;
    } else {
      // Both arg1 and arg2 are symbols.
      uint32_t arg1tag = tag(arg1);
      uint32_t arg2tag = tag(arg2);
      if (arg1tag != arg2tag) {
        return arg1tag < arg2tag;
      } else {
        assert(arg1layout == arg2layout);
        layout *layoutPtr = getLayoutData(arg1layout);
        uint8_t length = layoutPtr->nargs;
        for (uint8_t i = 0; i < length; i++) {
          uint64_t offset = layoutPtr->args[i].offset;
          uint16_t cat = layoutPtr->args[i].cat;
          switch (cat) {
          case MAP_LAYOUT: {
            abort(); // Implement when needed.
          }
          case RANGEMAP_LAYOUT: {
            abort(); // Implement when needed.
          }
          case LIST_LAYOUT: {
            abort(); // Implement when needed.
          }
          case SET_LAYOUT: {
            abort(); // Implement when needed.
          }
          case INT_LAYOUT: {
            mpz_ptr *int1ptrptr = (mpz_ptr *)(arg1intptr + offset);
            mpz_ptr *int2ptrptr = (mpz_ptr *)(arg2intptr + offset);
            int cmp = mpz_cmp(*int1ptrptr, *int2ptrptr);
            if (cmp != 0) {
              return cmp < 0;
            }
            break;
          }
          case FLOAT_LAYOUT: {
            abort(); // Implement when needed.
          }
          case STRINGBUFFER_LAYOUT: {
            abort(); // Implement when needed.
          }
          case BOOL_LAYOUT: {
            abort(); // Implement when needed.
          }
          case BYTES_LAYOUT: {
            abort(); // Implement when needed.
          }
          case SYMBOL_LAYOUT: {
            abort(); // Implement when needed.
          }
          case VARIABLE_LAYOUT: {
            abort(); // Implement when needed.
          }
          default: abort();
          }
        }
        return false;
      }
    }
  }
}

bool hook_KEQUAL_ne(block *arg1, block *arg2) {
  return !hook_KEQUAL_eq(arg1, arg2);
}

bool hook_BOOL_eq(bool arg1, bool arg2) {
  return arg1 == arg2;
}
}
