#include "runtime/alloc.h"
#include "runtime/arena.h"
#include "runtime/header.h"

extern "C" {
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
      auto *new_arg = copy_block_to_eternal_lifetime(*(block **)arg);
      *(block **)(((char *)new_block) + arg_data->offset) = new_arg;
      break;
    }
    case RANGEMAP_LAYOUT: {
      auto *new_arg = copy_block_to_eternal_lifetime(*(block **)arg);
      *(block **)(((char *)new_block) + arg_data->offset) = new_arg;
      break;
    }
    case LIST_LAYOUT: {
      auto *new_arg = copy_block_to_eternal_lifetime(*(block **)arg);
      *(block **)(((char *)new_block) + arg_data->offset) = new_arg;
      break;
    }
    case SET_LAYOUT: {
      auto *new_arg = copy_block_to_eternal_lifetime(*(block **)arg);
      *(block **)(((char *)new_block) + arg_data->offset) = new_arg;
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
      *(void **)(((char *)new_block) + arg_data->offset) = *(void **)arg;
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