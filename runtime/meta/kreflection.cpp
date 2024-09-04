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

  for (unsigned j = 0; j < layout_data->nargs; j++) {

    // Get the layout item for the argument and the argument itself
    auto *arg_data = layout_data->args + j;
    auto *arg = ((char *)term) + arg_data->offset;

    // Copy the argument based on the category of the argument
    switch (arg_data->cat) {
    case MAP_LAYOUT: {
      map new_arg = map_map(arg, copy_block_to_eternal_lifetime);
      memcpy(new_block->children, term->children, arg_data->offset - 8);
      *(map *)(((char *)new_block) + arg_data->offset) = new_arg;
      break;
    }
    case RANGEMAP_LAYOUT: {
      rangemap new_arg = rangemap_map(arg, copy_block_to_eternal_lifetime);
      memcpy(new_block->children, term->children, arg_data->offset - 8);
      *(rangemap *)(((char *)new_block) + arg_data->offset) = new_arg;
      break;
    }
    case LIST_LAYOUT: {
      list new_arg = list_map(arg, copy_block_to_eternal_lifetime);
      memcpy(new_block->children, term->children, arg_data->offset - 8);
      *(list *)(((char *)new_block) + arg_data->offset) = new_arg;
      break;
    }
    case SET_LAYOUT: {
      set new_arg = set_map(arg, copy_block_to_eternal_lifetime);
      memcpy(new_block->children, term->children, arg_data->offset - 8);
      *(set *)(((char *)new_block) + arg_data->offset) = new_arg;
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
      uint64_t const hdr = buffer->h.hdr;
      bool hasForwardingAddress = hdr & FWD_PTR_BIT;

      if (!hasForwardingAddress) {
        uint64_t const cap = len(buffer->contents);
        string *new_contents
            = (string *)kore_alloc_token_forever(sizeof(string) + cap);

        memcpy(new_contents, buffer->contents, sizeof(string) + buffer->strlen);
        memcpy(new_buffer, buffer, sizeof(stringbuffer));

        new_buffer->h.hdr |= NOT_YOUNG_OBJECT_BIT;
        new_buffer->contents = new_contents;
        *(stringbuffer **)(buffer->contents) = new_buffer;
        buffer->h.hdr |= FWD_PTR_BIT;
      }
      *(stringbuffer **)(((char *)new_block) + arg_data->offset)
          = *(stringbuffer **)(buffer->contents);
      break;
    }
    case INT_LAYOUT: {
      mpz_ptr *ptr = static_cast<mpz_ptr *>((mpz_ptr *)arg);
      mpz_hdr *new_arg = STRUCT_BASE(mpz_hdr, i, *ptr);
      uint64_t const hdr = new_arg->h.hdr;
      bool hasForwardingAddress = hdr & FWD_PTR_BIT;

      if (!hasForwardingAddress) {
        mpz_hdr *new_intgr = nullptr;
        string *new_limbs = nullptr;
        bool has_limbs = new_arg->i->_mp_alloc > 0;
        if (has_limbs) {
          string *limbs = STRUCT_BASE(string, data, new_arg->i->_mp_d);
          size_t len_limbs = len(limbs);

          assert(new_arg->i->_mp_alloc * sizeof(mp_limb_t) == len_limbs);

          new_intgr = STRUCT_BASE(mpz_hdr, i, kore_alloc_integer_forever(0));
          new_limbs
              = (string *)kore_alloc_token_forever(sizeof(string) + len_limbs);

          memcpy(new_limbs, limbs, sizeof(string) + len_limbs);
        } else {
          new_intgr = STRUCT_BASE(mpz_hdr, i, kore_alloc_integer_forever(0));
        }
        memcpy(new_intgr, new_arg, sizeof(mpz_hdr));
        new_intgr->h.hdr |= NOT_YOUNG_OBJECT_BIT;
        if (has_limbs) {
          new_intgr->i->_mp_d = (mp_limb_t *)new_limbs->data;
        }
        *(mpz_ptr *)(&new_arg->i->_mp_d) = new_intgr->i;
        new_arg->h.hdr |= FWD_PTR_BIT;
      }
      *ptr = *(mpz_ptr *)(&new_arg->i->_mp_d);
      *(void **)(((char *)new_block) + arg_data->offset) = *(void **)ptr;
      break;
    }
    case FLOAT_LAYOUT: {
      floating **floating_ptr = static_cast<floating **>((floating **)arg);
      floating_hdr *flt = STRUCT_BASE(floating_hdr, f, *floating_ptr);
      uint64_t const hdr = flt->h.hdr;
      bool hasForwardingAddress = hdr & FWD_PTR_BIT;

      if (!hasForwardingAddress) {
        floating_hdr *new_flt = nullptr;
        string *new_limbs = nullptr;
        string *limbs = STRUCT_BASE(string, data, flt->f.f->_mpfr_d - 1);
        size_t len_limbs = len(limbs);

        assert(
            ((flt->f.f->_mpfr_prec + mp_bits_per_limb - 1) / mp_bits_per_limb)
                * sizeof(mp_limb_t)
            <= len_limbs);

        new_flt = STRUCT_BASE(floating_hdr, f, kore_alloc_floating_forever(0));
        new_limbs
            = (string *)kore_alloc_token_forever(sizeof(string) + len_limbs);

        memcpy(new_limbs, limbs, sizeof(string) + len_limbs);
        memcpy(new_flt, flt, sizeof(floating_hdr));

        new_flt->h.hdr |= NOT_YOUNG_OBJECT_BIT;
        new_flt->f.f->_mpfr_d = (mp_limb_t *)new_limbs->data + 1;

        *(floating **)(flt->f.f->_mpfr_d) = &new_flt->f;
        flt->h.hdr |= FWD_PTR_BIT;
      }

      *floating_ptr = *(floating **)(flt->f.f->_mpfr_d);
      *(void **)(((char *)new_block) + arg_data->offset) = *(void **)floating_ptr;
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