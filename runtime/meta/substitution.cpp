#include <cstring>
#include <vector>

#include "runtime/alloc.h"
#include "runtime/header.h"

static thread_local string *var;
static thread_local block *to_replace;
static thread_local block *replacement;
static thread_local block *replacementInj;
static thread_local uint64_t idx;
static thread_local uint64_t idx2;

extern "C" {
bool hook_KEQUAL_eq(block *, block *);
map map_map(void *, block *(block *));
rangemap rangemap_map(void *, block *(block *));
list list_map(void *, block *(block *));
set set_map(void *, block *(block *));
}

template <class New>
void makeDirty(bool &dirty, uint64_t offset, New newArg, block *&newBlock) {
  if (!dirty) {
    dirty = true;
    block *alloc = (block *)koreAlloc(size_hdr(newBlock->h.hdr));
    alloc->h = newBlock->h;
    reset_gc(alloc);
    memcpy(alloc->children, newBlock->children, offset - 8);
    newBlock = alloc;
  }
  New *newPtr = (New *)(((char *)newBlock) + offset);
  *newPtr = newArg;
}

block *debruijnizeInternal(block *currBlock) {
  if (is_leaf_block(currBlock)) {
    return currBlock;
  }
  const uint64_t hdr = currBlock->h.hdr;
  uint16_t layoutInt = layout_hdr(hdr);
  if (layoutInt) {
    uint32_t tag = tag_hdr(hdr);
    bool isBinder = isSymbolABinder(tag);
    if (isBinder) {
      idx++;
    }
    layout *layoutData = getLayoutData(layoutInt);
    bool dirty = false;
    block *newBlock = currBlock;
    for (unsigned i = 0; i < layoutData->nargs; i++) {
      layoutitem *argData = layoutData->args + i;
      void *arg = ((char *)currBlock) + argData->offset;
      switch (argData->cat) {
      case MAP_LAYOUT: {
        map newArg = map_map(arg, debruijnizeInternal);
        makeDirty(dirty, argData->offset, newArg, newBlock);
        break;
      }
      case RANGEMAP_LAYOUT: {
        rangemap newArg = rangemap_map(arg, debruijnizeInternal);
        makeDirty(dirty, argData->offset, newArg, newBlock);
        break;
      }
      case LIST_LAYOUT: {
        list newArg = list_map(arg, debruijnizeInternal);
        makeDirty(dirty, argData->offset, newArg, newBlock);
        break;
      }
      case SET_LAYOUT: {
        set newArg = set_map(arg, debruijnizeInternal);
        makeDirty(dirty, argData->offset, newArg, newBlock);
        break;
      }
      case SYMBOL_LAYOUT: {
        block *oldArg = *(block **)arg;
        block *newArg = debruijnizeInternal(oldArg);
        if (oldArg != newArg || dirty) {
          makeDirty(dirty, argData->offset, newArg, newBlock);
        }
        break;
      }
      case VARIABLE_LAYOUT: {
        if (!(i == 0 && isBinder) && hook_STRING_eq(var, *(string **)arg)) {
          block *newArg = variable_block(idx);
          makeDirty(dirty, argData->offset, newArg, newBlock);
        }
        break;
      }
      case STRINGBUFFER_LAYOUT:
      case INT_LAYOUT:
      case FLOAT_LAYOUT:
      case BOOL_LAYOUT:
      default: // mint
        break;
      }
    }
    if (isBinder) {
      idx--;
    }
    return newBlock;
  } else {
    return currBlock;
  }
}

block *replaceBinderInternal(block *currBlock) {
  uintptr_t ptr = (uintptr_t)currBlock;
  if (is_variable_block(ptr)) {
    uint64_t varIdx = ptr >> 32;
    if (idx == varIdx) {
      return (block *)var;
    } else if (idx < varIdx) {
      varIdx--;
      return variable_block(varIdx);
    } else {
      return currBlock;
    }
  } else if (is_leaf_block(ptr)) {
    return currBlock;
  }
  const uint64_t hdr = currBlock->h.hdr;
  uint16_t layoutInt = layout_hdr(hdr);
  if (layoutInt) {
    uint32_t tag = tag_hdr(hdr);
    bool isBinder = isSymbolABinder(tag);
    if (isBinder) {
      idx++;
    }
    layout *layoutData = getLayoutData(layoutInt);
    bool dirty = false;
    block *newBlock = currBlock;
    for (unsigned i = 0; i < layoutData->nargs; i++) {
      layoutitem *argData = layoutData->args + i;
      void *arg = ((char *)currBlock) + argData->offset;
      switch (argData->cat) {
      case MAP_LAYOUT: {
        map newArg = map_map(arg, replaceBinderInternal);
        makeDirty(dirty, argData->offset, newArg, newBlock);
        break;
      }
      case RANGEMAP_LAYOUT: {
        rangemap newArg = rangemap_map(arg, replaceBinderInternal);
        makeDirty(dirty, argData->offset, newArg, newBlock);
        break;
      }
      case LIST_LAYOUT: {
        list newArg = list_map(arg, replaceBinderInternal);
        makeDirty(dirty, argData->offset, newArg, newBlock);
        break;
      }
      case SET_LAYOUT: {
        set newArg = set_map(arg, replaceBinderInternal);
        makeDirty(dirty, argData->offset, newArg, newBlock);
        break;
      }
      case VARIABLE_LAYOUT:
      case SYMBOL_LAYOUT: {
        block *oldArg = *(block **)arg;
        block *newArg = replaceBinderInternal(oldArg);
        if (oldArg != newArg || dirty) {
          makeDirty(dirty, argData->offset, newArg, newBlock);
        }
        break;
      }
      case STRINGBUFFER_LAYOUT:
      case INT_LAYOUT:
      case FLOAT_LAYOUT:
      case BOOL_LAYOUT:
      default: // mint
        break;
      }
    }
    if (isBinder) {
      idx--;
    }
    return newBlock;
  } else {
    return currBlock;
  }
}

block *substituteInternal(block *currBlock) {
  if (is_leaf_block(currBlock)) {
    return currBlock;
  }
  const uint64_t hdr = currBlock->h.hdr;
  uint16_t layoutInt = layout_hdr(hdr);
  if (hook_KEQUAL_eq(currBlock, to_replace)) {
    idx2 = 0;
    if (layoutInt) {
      uint32_t tag = tag_hdr(hdr);
      uint32_t injTag = getInjectionForSortOfTag(tag);
      if (tag_hdr(replacementInj->h.hdr) != injTag) {
        return incrementDebruijn(replacementInj);
      }
    }
    return incrementDebruijn(replacement);
  }
  if (layoutInt) {
    layout *layoutData = getLayoutData(layoutInt);
    bool dirty = false;
    block *newBlock = currBlock;
    uint32_t tag = tag_hdr(hdr);
    std::vector<void *> arguments;
    bool isBinder = isSymbolABinder(tag);
    if (isBinder) {
      idx++;
    }
    for (unsigned i = 0; i < layoutData->nargs; i++) {
      layoutitem *argData = layoutData->args + i;
      void *arg = ((char *)currBlock) + argData->offset;
      switch (argData->cat) {
      case MAP_LAYOUT: {
        map newArg = map_map(arg, substituteInternal);
        makeDirty(dirty, argData->offset, newArg, newBlock);
        arguments.push_back(((char *)newBlock) + argData->offset);
        break;
      }
      case RANGEMAP_LAYOUT: {
        rangemap newArg = rangemap_map(arg, substituteInternal);
        makeDirty(dirty, argData->offset, newArg, newBlock);
        arguments.push_back(((char *)newBlock) + argData->offset);
        break;
      }
      case LIST_LAYOUT: {
        list newArg = list_map(arg, substituteInternal);
        makeDirty(dirty, argData->offset, newArg, newBlock);
        arguments.push_back(((char *)newBlock) + argData->offset);
        break;
      }
      case SET_LAYOUT: {
        set newArg = set_map(arg, substituteInternal);
        makeDirty(dirty, argData->offset, newArg, newBlock);
        arguments.push_back(((char *)newBlock) + argData->offset);
        break;
      }
      case VARIABLE_LAYOUT:
      case SYMBOL_LAYOUT: {
        block *oldArg = *(block **)arg;
        block *newArg = substituteInternal(oldArg);
        if (oldArg != newArg || dirty) {
          makeDirty(dirty, argData->offset, newArg, newBlock);
        }
        arguments.push_back(newArg);
        break;
      }
      case STRINGBUFFER_LAYOUT:
      case INT_LAYOUT:
      case FLOAT_LAYOUT: arguments.push_back(*(void **)arg); break;
      case BOOL_LAYOUT:
      default: // mint
        arguments.push_back(arg);
        break;
      }
    }
    if (isBinder) {
      idx--;
    }
    if (isSymbolAFunction(tag)) {
      uint64_t idx_stack = idx;
      block *to_replace_stack = to_replace;
      block *replacement_stack = replacement;
      block *replacementInj_stack = replacementInj;
      block *result = (block *)evaluateFunctionSymbol(tag, &arguments[0]);
      to_replace = to_replace_stack;
      replacement = replacement_stack;
      replacementInj = replacementInj_stack;
      idx = idx_stack;
      return result;
    }
    return newBlock;
  } else {
    return currBlock;
  }
}

extern "C" {

block *debruijnize(block *term) {
  auto layoutData = getLayoutData(layout(term));
  auto layoutVar = layoutData->args[0];
  auto layoutBody = layoutData->args[layoutData->nargs - 1];
  var = *(string **)(((char *)term) + layoutVar.offset);
  idx = 0;
  auto bodyPtr = *(block **)(((char *)term) + layoutBody.offset);
  auto newBody = debruijnizeInternal(bodyPtr);
  auto newBlock = term;
  if (newBody != bodyPtr) {
    bool dirty = false;
    makeDirty(dirty, layoutBody.offset, newBody, newBlock);
  }
  auto newVar = *(string **)(((char *)newBlock) + layoutVar.offset);
  newVar->h.hdr |= VARIABLE_BIT;
  return newBlock;
}

block *incrementDebruijn(block *currBlock) {
  uintptr_t ptr = (uintptr_t)currBlock;
  if (is_variable_block(ptr)) {
    uint64_t varIdx = ptr >> 32;
    if (varIdx >= idx2) {
      varIdx += idx;
      return variable_block(varIdx);
    } else {
      return currBlock;
    }
  } else if (is_leaf_block(ptr)) {
    return currBlock;
  }
  const uint64_t hdr = currBlock->h.hdr;
  uint16_t layoutInt = layout_hdr(hdr);
  if (layoutInt) {
    layout *layoutData = getLayoutData(layoutInt);
    bool dirty = false;
    block *newBlock = currBlock;
    uint32_t tag = tag_hdr(hdr);
    bool isBinder = isSymbolABinder(tag);
    if (isBinder) {
      idx2++;
    }
    for (unsigned i = 0; i < layoutData->nargs; i++) {
      layoutitem *argData = layoutData->args + i;
      void *arg = ((char *)currBlock) + argData->offset;
      switch (argData->cat) {
      case MAP_LAYOUT: {
        map newArg = map_map(arg, incrementDebruijn);
        makeDirty(dirty, argData->offset, newArg, newBlock);
        break;
      }
      case RANGEMAP_LAYOUT: {
        rangemap newArg = rangemap_map(arg, incrementDebruijn);
        makeDirty(dirty, argData->offset, newArg, newBlock);
        break;
      }
      case LIST_LAYOUT: {
        list newArg = list_map(arg, incrementDebruijn);
        makeDirty(dirty, argData->offset, newArg, newBlock);
        break;
      }
      case SET_LAYOUT: {
        set newArg = set_map(arg, incrementDebruijn);
        makeDirty(dirty, argData->offset, newArg, newBlock);
        break;
      }
      case VARIABLE_LAYOUT:
      case SYMBOL_LAYOUT: {
        block *oldArg = *(block **)arg;
        block *newArg;
        if (i == 0 && isBinder) {
          newArg = alphaRename(oldArg);
        } else {
          newArg = incrementDebruijn(oldArg);
        }
        if (oldArg != newArg || dirty) {
          makeDirty(dirty, argData->offset, newArg, newBlock);
        }
        break;
      }
      case STRINGBUFFER_LAYOUT:
      case INT_LAYOUT:
      case FLOAT_LAYOUT:
      case BOOL_LAYOUT:
      default: // mint
        break;
      }
    }
    if (isBinder) {
      idx2--;
    }
    return newBlock;
  } else {
    return currBlock;
  }
}

block *alphaRename(block *term) {
  string *var = (string *)term;
  size_t len = len(var);
  auto newToken = (string *)koreAllocToken(sizeof(string) + len);
  memcpy(newToken->data, var->data, len);
  set_len(newToken, len);
  newToken->h.hdr |= VARIABLE_BIT;
  return (block *)newToken;
}

block *replaceBinderIndex(block *term, block *variable) {
  idx = 0;
  var = (string *)variable;
  return replaceBinderInternal(term);
}

block *
hook_SUBSTITUTION_substOne(block *body, SortKItem newVal, SortKItem varInj) {
  bool isSameSort = tag_hdr(newVal->h.hdr) == tag_hdr(varInj->h.hdr);
  idx = 0;
  replacement = *(block **)(((char *)newVal) + sizeof(blockheader));
  if (isSameSort) {
    to_replace = *(block **)(((char *)varInj) + sizeof(blockheader));
    replacementInj = replacement;
  } else {
    to_replace = varInj;
    replacementInj = newVal;
  }
  return substituteInternal(body);
}
}
