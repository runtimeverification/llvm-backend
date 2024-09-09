#include "config/macros.h"
#include "runtime/alloc.h"
#include "runtime/collections/rangemap.h"
#include "runtime/header.h"

#include <cstddef>
#include <fmt/printf.h>
#include <gmp.h>

#include <immer/flex_vector.hpp>
#include <immer/flex_vector_transient.hpp>
#include <immer/map.hpp>
#include <immer/map_transient.hpp>
#include <immer/set.hpp>
#include <immer/set_transient.hpp>

struct kore_alloc_forever_heap {

  template <typename... Tags>
  static void *allocate(size_t size, Tags...) {
    if (during_gc()) {
      return ::operator new(size);
    }
    bool enabled = gc_enabled;
    gc_enabled = false;
    auto *result
        = (string *)kore_alloc_token_forever(size + sizeof(blockheader));
    gc_enabled = enabled;
    init_with_len_forever(result, size);
    return result->data;
  }

  static void deallocate(size_t size, void *data) {
    // This should never be called
    abort();
  }
};

using list_forever = immer::flex_vector<
    k_elem, immer::memory_policy<
                immer::heap_policy<kore_alloc_forever_heap>,
                immer::no_refcount_policy, immer::no_lock_policy>>;

using map_forever = immer::map<
    k_elem, k_elem, hash_block, std::equal_to<>, list_forever::memory_policy>;
using set_forever = immer::set<
    k_elem, hash_block, std::equal_to<>, list_forever::memory_policy>;

list_forever list_map_forever(list *l, block *(process)(block *)) {
  auto tmp = list_forever().transient();

  for (auto iter = l->begin(); iter != l->end(); ++iter) {
    tmp.push_back(process(*iter));
  }

  return tmp.persistent();
}

map_forever map_map_forever(map *m, block *(process)(block *)) {
  auto tmp = map_forever().transient();

  for (auto iter = m->begin(); iter != m->end(); ++iter) {
    auto *key = process(iter->first);
    auto *value = process(iter->second);
    tmp.insert(std::make_pair(key, value));
  }

  return tmp.persistent();
}

set_forever set_map_forever(set *s, block *(process)(block *)) {
  auto tmp = set_forever().transient();

  for (auto iter = s->begin(); iter != s->end(); ++iter) {
    tmp.insert(process(*iter));
  }

  return tmp.persistent();
}

extern "C" {
rangemap rangemap_map(void *, block *(block *));

block *copy_block_to_eternal_lifetime(block *term) {
  if (is_leaf_block(term)) {
    return term;
  }

  // Get the layout data for the block and allocate a new block with the same size as the original block
  auto layout = get_layout(term);

  // If the block is string term we have reallocate it so it wouldn't be catched by the gc
  if (layout == 0) {
    auto *str = (string *)term;
    auto *new_str
        = (string *)kore_alloc_token_forever(sizeof(string) + len(str));
    memcpy(new_str, str, sizeof(string) + len(str));
    new_str->h.hdr |= NOT_YOUNG_OBJECT_BIT;
    return (block *)new_str;
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
      map_forever new_arg
          = map_map_forever((map *)arg, copy_block_to_eternal_lifetime);
      memcpy(new_block->children, term->children, arg_data->offset - 8);
      *(map_forever *)(((char *)new_block) + arg_data->offset) = new_arg;
      break;
    }
    case RANGEMAP_LAYOUT: {
      rangemap new_arg = rangemap_map(arg, copy_block_to_eternal_lifetime);
      memcpy(new_block->children, term->children, arg_data->offset - 8);
      *(rangemap *)(((char *)new_block) + arg_data->offset) = new_arg;
      break;
    }
    case LIST_LAYOUT: {
      list_forever new_arg
          = list_map_forever((list *)arg, copy_block_to_eternal_lifetime);
      memcpy(new_block->children, term->children, arg_data->offset - 8);
      *(list_forever *)(((char *)new_block) + arg_data->offset) = new_arg;
      break;
    }
    case SET_LAYOUT: {
      set_forever new_arg
          = set_map_forever((set *)arg, copy_block_to_eternal_lifetime);
      memcpy(new_block->children, term->children, arg_data->offset - 8);
      *(set_forever *)(((char *)new_block) + arg_data->offset) = new_arg;
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
      auto *new_buffer = (stringbuffer *)new_block;
      auto *buffer = (stringbuffer *)arg;

      uint64_t const cap = len(buffer->contents);
      auto *new_contents
          = (string *)kore_alloc_token_forever(sizeof(string) + cap);

      memcpy(new_contents, buffer->contents, sizeof(string) + buffer->strlen);
      memcpy(new_buffer, buffer, sizeof(stringbuffer));

      new_buffer->h.hdr |= NOT_YOUNG_OBJECT_BIT;
      new_buffer->contents = new_contents;
      *(stringbuffer **)(buffer->contents) = new_buffer;

      *(stringbuffer **)(((char *)new_block) + arg_data->offset)
          = *(stringbuffer **)(buffer->contents);
      break;
    }
    case INT_LAYOUT: {
      auto *ptr = static_cast<mpz_ptr *>((mpz_ptr *)arg);
      mpz_hdr *new_arg = STRUCT_BASE(mpz_hdr, i, *ptr);

      mpz_hdr *new_intgr = nullptr;
      string *new_limbs = nullptr;
      bool has_limbs = new_arg->i->_mp_alloc > 0;
      if (has_limbs) {
        string *limbs = STRUCT_BASE(string, data, new_arg->i->_mp_d);
        size_t len_limbs = new_arg->i->_mp_alloc * sizeof(mp_limb_t);

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

      *ptr = *(mpz_ptr *)(&new_arg->i->_mp_d);
      *(void **)(((char *)new_block) + arg_data->offset) = *(void **)ptr;
      break;
    }
    case FLOAT_LAYOUT: {
      auto **floating_ptr = static_cast<floating **>((floating **)arg);
      floating_hdr *flt = STRUCT_BASE(floating_hdr, f, *floating_ptr);

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

      *floating_ptr = *(floating **)(flt->f.f->_mpfr_d);
      *(void **)(((char *)new_block) + arg_data->offset)
          = *(void **)floating_ptr;
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