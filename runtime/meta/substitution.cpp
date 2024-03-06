#include <cstring>
#include <vector>

#include "runtime/alloc.h"
#include "runtime/header.h"

static thread_local string *var;
static thread_local block *to_replace;
static thread_local block *replacement;
static thread_local block *replacement_inj;
static thread_local uint64_t idx;
static thread_local uint64_t idx2;

extern "C" {

map map_map(void *, block *(block *));
rangemap rangemap_map(void *, block *(block *));
list list_map(void *, block *(block *));
set set_map(void *, block *(block *));
}

template <class New>
void make_dirty(bool &dirty, uint64_t offset, New new_arg, block *&new_block) {
  if (!dirty) {
    dirty = true;
    auto *alloc = (block *)kore_alloc(size_hdr(new_block->h.hdr));
    alloc->h = new_block->h;
    reset_gc(alloc);
    memcpy(alloc->children, new_block->children, offset - 8);
    new_block = alloc;
  }
  New *new_ptr = (New *)(((char *)new_block) + offset);
  *new_ptr = new_arg;
}

block *debruijnize_internal(block *curr_block) {
  if (is_leaf_block(curr_block)) {
    return curr_block;
  }
  uint64_t const hdr = curr_block->h.hdr;
  uint16_t layout_int = layout_hdr(hdr);
  if (layout_int) {
    uint32_t tag = tag_hdr(hdr);
    bool is_binder = is_symbol_a_binder(tag);
    if (is_binder) {
      idx++;
    }
    layout *layout_data = get_layout_data(layout_int);
    bool dirty = false;
    block *new_block = curr_block;
    for (unsigned i = 0; i < layout_data->nargs; i++) {
      layoutitem *arg_data = layout_data->args + i;
      void *arg = ((char *)curr_block) + arg_data->offset;
      switch (arg_data->cat) {
      case MAP_LAYOUT: {
        map new_arg = map_map(arg, debruijnize_internal);
        make_dirty(dirty, arg_data->offset, new_arg, new_block);
        break;
      }
      case RANGEMAP_LAYOUT: {
        rangemap new_arg = rangemap_map(arg, debruijnize_internal);
        make_dirty(dirty, arg_data->offset, new_arg, new_block);
        break;
      }
      case LIST_LAYOUT: {
        list new_arg = list_map(arg, debruijnize_internal);
        make_dirty(dirty, arg_data->offset, new_arg, new_block);
        break;
      }
      case SET_LAYOUT: {
        set new_arg = set_map(arg, debruijnize_internal);
        make_dirty(dirty, arg_data->offset, new_arg, new_block);
        break;
      }
      case SYMBOL_LAYOUT: {
        block *old_arg = *(block **)arg;
        block *new_arg = debruijnize_internal(old_arg);
        if (old_arg != new_arg || dirty) {
          make_dirty(dirty, arg_data->offset, new_arg, new_block);
        }
        break;
      }
      case VARIABLE_LAYOUT: {
        if ((i != 0 || !is_binder) && hook_STRING_eq(var, *(string **)arg)) {
          block *new_arg = variable_block(idx);
          make_dirty(dirty, arg_data->offset, new_arg, new_block);
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
    if (is_binder) {
      idx--;
    }
    return new_block;
  }
  return curr_block;
}

block *replace_binder_internal(block *curr_block) {
  if (is_variable_block(curr_block)) {
    uint64_t var_idx = ((uintptr_t)curr_block) >> 32;
    if (idx == var_idx) {
      return (block *)var;
    }
    if (idx < var_idx) {
      var_idx--;
      return variable_block(var_idx);
    }
    return curr_block;
  }
  if (is_leaf_block(curr_block)) {
    return curr_block;
  }
  uint64_t const hdr = curr_block->h.hdr;
  uint16_t layout_int = layout_hdr(hdr);
  if (layout_int) {
    uint32_t tag = tag_hdr(hdr);
    bool is_binder = is_symbol_a_binder(tag);
    if (is_binder) {
      idx++;
    }
    layout *layout_data = get_layout_data(layout_int);
    bool dirty = false;
    block *new_block = curr_block;
    for (unsigned i = 0; i < layout_data->nargs; i++) {
      layoutitem *arg_data = layout_data->args + i;
      void *arg = ((char *)curr_block) + arg_data->offset;
      switch (arg_data->cat) {
      case MAP_LAYOUT: {
        map new_arg = map_map(arg, replace_binder_internal);
        make_dirty(dirty, arg_data->offset, new_arg, new_block);
        break;
      }
      case RANGEMAP_LAYOUT: {
        rangemap new_arg = rangemap_map(arg, replace_binder_internal);
        make_dirty(dirty, arg_data->offset, new_arg, new_block);
        break;
      }
      case LIST_LAYOUT: {
        list new_arg = list_map(arg, replace_binder_internal);
        make_dirty(dirty, arg_data->offset, new_arg, new_block);
        break;
      }
      case SET_LAYOUT: {
        set new_arg = set_map(arg, replace_binder_internal);
        make_dirty(dirty, arg_data->offset, new_arg, new_block);
        break;
      }
      case VARIABLE_LAYOUT:
      case SYMBOL_LAYOUT: {
        block *old_arg = *(block **)arg;
        block *new_arg = replace_binder_internal(old_arg);
        if (old_arg != new_arg || dirty) {
          make_dirty(dirty, arg_data->offset, new_arg, new_block);
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
    if (is_binder) {
      idx--;
    }
    return new_block;
  }
  return curr_block;
}

block *substitute_internal(block *curr_block) {
  if (is_leaf_block(curr_block)) {
    return curr_block;
  }
  uint64_t const hdr = curr_block->h.hdr;
  uint16_t layout_int = layout_hdr(hdr);
  if (hook_KEQUAL_eq(curr_block, to_replace)) {
    idx2 = 0;
    if (layout_int) {
      uint32_t tag = tag_hdr(hdr);
      uint32_t inj_tag = get_injection_for_sort_of_tag(tag);
      if (tag_hdr(replacement_inj->h.hdr) != inj_tag) {
        return increment_debruijn(replacement_inj);
      }
    }
    return increment_debruijn(replacement);
  }
  if (layout_int) {
    layout *layout_data = get_layout_data(layout_int);
    bool dirty = false;
    block *new_block = curr_block;
    uint32_t tag = tag_hdr(hdr);
    std::vector<void *> arguments;
    bool is_binder = is_symbol_a_binder(tag);
    if (is_binder) {
      idx++;
    }
    for (unsigned i = 0; i < layout_data->nargs; i++) {
      layoutitem *arg_data = layout_data->args + i;
      void *arg = ((char *)curr_block) + arg_data->offset;
      switch (arg_data->cat) {
      case MAP_LAYOUT: {
        map new_arg = map_map(arg, substitute_internal);
        make_dirty(dirty, arg_data->offset, new_arg, new_block);
        arguments.push_back(((char *)new_block) + arg_data->offset);
        break;
      }
      case RANGEMAP_LAYOUT: {
        rangemap new_arg = rangemap_map(arg, substitute_internal);
        make_dirty(dirty, arg_data->offset, new_arg, new_block);
        arguments.push_back(((char *)new_block) + arg_data->offset);
        break;
      }
      case LIST_LAYOUT: {
        list new_arg = list_map(arg, substitute_internal);
        make_dirty(dirty, arg_data->offset, new_arg, new_block);
        arguments.push_back(((char *)new_block) + arg_data->offset);
        break;
      }
      case SET_LAYOUT: {
        set new_arg = set_map(arg, substitute_internal);
        make_dirty(dirty, arg_data->offset, new_arg, new_block);
        arguments.push_back(((char *)new_block) + arg_data->offset);
        break;
      }
      case VARIABLE_LAYOUT:
      case SYMBOL_LAYOUT: {
        block *old_arg = *(block **)arg;
        block *new_arg = substitute_internal(old_arg);
        if (old_arg != new_arg || dirty) {
          make_dirty(dirty, arg_data->offset, new_arg, new_block);
        }
        arguments.push_back(new_arg);
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
    if (is_binder) {
      idx--;
    }
    if (is_symbol_a_function(tag)) {
      uint64_t idx_stack = idx;
      block *to_replace_stack = to_replace;
      block *replacement_stack = replacement;
      block *replacement_inj_stack = replacement_inj;
      auto *result = (block *)evaluate_function_symbol(tag, arguments.data());
      to_replace = to_replace_stack;
      replacement = replacement_stack;
      replacement_inj = replacement_inj_stack;
      idx = idx_stack;
      return result;
    }
    return new_block;
  }
  return curr_block;
}

extern "C" {

block *debruijnize(block *term) {
  auto *layout_data = get_layout_data(get_layout(term));
  auto layout_var = layout_data->args[0];
  auto layout_body = layout_data->args[layout_data->nargs - 1];
  var = *(string **)(((char *)term) + layout_var.offset);
  idx = 0;
  auto *body_ptr = *(block **)(((char *)term) + layout_body.offset);
  auto *new_body = debruijnize_internal(body_ptr);
  auto *new_block = term;
  if (new_body != body_ptr) {
    bool dirty = false;
    make_dirty(dirty, layout_body.offset, new_body, new_block);
  }
  auto *new_var = *(string **)(((char *)new_block) + layout_var.offset);
  new_var->h.hdr |= VARIABLE_BIT;
  return new_block;
}

block *increment_debruijn(block *curr_block) {
  if (is_variable_block(curr_block)) {
    uint64_t var_idx = ((uintptr_t)curr_block) >> 32;
    if (var_idx >= idx2) {
      var_idx += idx;
      return variable_block(var_idx);
    }
    return curr_block;
  }
  if (is_leaf_block(curr_block)) {
    return curr_block;
  }
  uint64_t const hdr = curr_block->h.hdr;
  uint16_t layout_int = layout_hdr(hdr);
  if (layout_int) {
    layout *layout_data = get_layout_data(layout_int);
    bool dirty = false;
    block *new_block = curr_block;
    uint32_t tag = tag_hdr(hdr);
    bool is_binder = is_symbol_a_binder(tag);
    if (is_binder) {
      idx2++;
    }
    for (unsigned i = 0; i < layout_data->nargs; i++) {
      layoutitem *arg_data = layout_data->args + i;
      void *arg = ((char *)curr_block) + arg_data->offset;
      switch (arg_data->cat) {
      case MAP_LAYOUT: {
        map new_arg = map_map(arg, increment_debruijn);
        make_dirty(dirty, arg_data->offset, new_arg, new_block);
        break;
      }
      case RANGEMAP_LAYOUT: {
        rangemap new_arg = rangemap_map(arg, increment_debruijn);
        make_dirty(dirty, arg_data->offset, new_arg, new_block);
        break;
      }
      case LIST_LAYOUT: {
        list new_arg = list_map(arg, increment_debruijn);
        make_dirty(dirty, arg_data->offset, new_arg, new_block);
        break;
      }
      case SET_LAYOUT: {
        set new_arg = set_map(arg, increment_debruijn);
        make_dirty(dirty, arg_data->offset, new_arg, new_block);
        break;
      }
      case VARIABLE_LAYOUT:
      case SYMBOL_LAYOUT: {
        block *old_arg = *(block **)arg;
        block *new_arg = nullptr;
        if (i == 0 && is_binder) {
          new_arg = alpha_rename(old_arg);
        } else {
          new_arg = increment_debruijn(old_arg);
        }
        if (old_arg != new_arg || dirty) {
          make_dirty(dirty, arg_data->offset, new_arg, new_block);
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
    if (is_binder) {
      idx2--;
    }
    return new_block;
  }
  return curr_block;
}

block *alpha_rename(block *term) {
  auto *var = (string *)term;
  size_t var_len = len(var);
  auto *new_token = (string *)kore_alloc_token(sizeof(string) + var_len);
  memcpy(new_token->data, var->data, var_len);
  init_with_len(new_token, var_len);
  new_token->h.hdr |= VARIABLE_BIT;
  return (block *)new_token;
}

block *replace_binder_index(block *term, block *variable) {
  idx = 0;
  var = (string *)variable;
  return replace_binder_internal(term);
}

block *
hook_SUBSTITUTION_substOne(block *body, SortKItem new_val, SortKItem var_inj) {
  bool is_same_sort = tag_hdr(new_val->h.hdr) == tag_hdr(var_inj->h.hdr);
  idx = 0;
  replacement = *(block **)(((char *)new_val) + sizeof(blockheader));
  if (is_same_sort) {
    to_replace = *(block **)(((char *)var_inj) + sizeof(blockheader));
    replacement_inj = replacement;
  } else {
    to_replace = var_inj;
    replacement_inj = new_val;
  }
  return substitute_internal(body);
}
}
