#include "runtime/alloc.h"
#include "runtime/arena.h"
#include "runtime/header.h"

extern "C" {
map map_map(void *, block *(block *));
rangemap rangemap_map(void *, block *(block *));
list list_map(void *, block *(block *));
set set_map(void *, block *(block *));

block *copy_block_to_eternal_lifetime(block *term) {
  if (is_leaf_block(term)) {
    return term;
  }

  // Get the layout data for the block and allocate a new block with the same size as the original block
  auto layout = get_layout(term);

  // If the block is string term we can just return it as we do for leaf blocks
  if (layout == 0) {
    return term;
  }

  auto *layout_data = get_layout_data(layout);

  auto *new_block = (block *)kore_alloc_forever(size_hdr(term->h.hdr));

  // Copy the block header and set the it as not young object, so gc does not collect it
  new_block->h = term->h;
  new_block->h.hdr |= NOT_YOUNG_OBJECT_BIT;

  for (unsigned i = 0; i < layout_data->nargs; i++) {

    // Get the layout item for the argument and the argument itself
    auto *arg_data = layout_data->args + i;
    auto *arg = ((char *)term) + arg_data->offset;

    // Copy the argument based on the category of the argument
    switch (arg_data->cat) {
    case MAP_LAYOUT: {
      map new_arg = map_map(arg, copy_block_to_eternal_lifetime);
      memcpy(new_block->children, term->children, arg_data->offset - 8);
      map *new_ptr = (map *)(((char *)term) + arg_data->offset);
      *new_ptr = new_arg;
      break;
    }
    case RANGEMAP_LAYOUT: {
      rangemap new_arg = rangemap_map(arg, copy_block_to_eternal_lifetime);
      memcpy(new_block->children, term->children, arg_data->offset - 8);
      rangemap *new_ptr = (rangemap *)(((char *)term) + arg_data->offset);
      *new_ptr = new_arg;
      break;
    }
    case LIST_LAYOUT: {
      list new_arg = list_map(arg, copy_block_to_eternal_lifetime);
      memcpy(new_block->children, term->children, arg_data->offset - 8);
      list *new_ptr = (list *)(((char *)term) + arg_data->offset);
      *new_ptr = new_arg;
      break;
    }
    case SET_LAYOUT: {
      set new_arg = set_map(arg, copy_block_to_eternal_lifetime);
      memcpy(new_block->children, term->children, arg_data->offset - 8);
      set *new_ptr = (set *)(((char *)term) + arg_data->offset);
      *new_ptr = new_arg;
      break;
    }
    case VARIABLE_LAYOUT: {
      auto *new_arg = copy_block_to_eternal_lifetime(*(block **)arg);
      *(block **)(((char *)new_block) + arg_data->offset) = new_arg;
      break;
    }
    case SYMBOL_LAYOUT: {
      auto *new_arg = copy_block_to_eternal_lifetime(*(block **)arg);
      *(block **)(((char *)new_block) + arg_data->offset) = new_arg;
      break;
    }
    case STRINGBUFFER_LAYOUT: {
      auto new_buffer = (stringbuffer *)new_block;
      auto buffer = (stringbuffer *)arg;

      uint64_t const cap = len(buffer->contents);
      string *new_contents
          = (string *)kore_alloc_token_forever(sizeof(string) + cap);

      memcpy(new_contents, buffer->contents, sizeof(string) + buffer->strlen);
      memcpy(new_buffer, buffer, sizeof(stringbuffer));

      new_buffer->contents = new_contents;
      *(stringbuffer **)(buffer->contents) = new_buffer;
      break;
    }
    case INT_LAYOUT: {
      *(void **)(((char *)new_block) + arg_data->offset) = *(void **)arg;
      break;
    }
    case FLOAT_LAYOUT: {
      *(void **)(((char *)new_block) + arg_data->offset) = *(void **)arg;
      break;
    }
    case BOOL_LAYOUT: {
      *(void **)(((char *)new_block) + arg_data->offset) = *(void **)arg;
      break;
    }
    default: // mint
      *(void **)(((char *)new_block) + arg_data->offset) = *(void **)arg;
      break;
    }
  }
  return new_block;
}

block *hook_KREFLECTION_freeze(block *term) {
  return copy_block_to_eternal_lifetime(term);
}
}