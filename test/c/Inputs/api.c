#include "api.h"

#include <assert.h>
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>

// Uses a GNU extension (fully supported by clang) to make the cast from dlsym
// nicer, but this is only ever compiled as test code so it's not an issue.
#define API_FUNCTION(name)                                                     \
  do {                                                                         \
    api.name = (__typeof__(name) *)dlsym(lib, #name);                          \
    assert(api.name && "Failed to load API function: " #name);                 \
  } while (false);

struct kllvm_c_api load_c_api(char const *path) {
  void *lib = dlopen(path, RTLD_NOW);
  if (!lib) {
    fprintf(stderr, "%s", dlerror());
    abort();
  }

  struct kllvm_c_api api;

  API_FUNCTION(kore_pattern_dump);
  API_FUNCTION(kore_pattern_pretty_print);
  API_FUNCTION(kore_pattern_serialize);
  API_FUNCTION(kore_pattern_free);
  API_FUNCTION(kore_pattern_parse);
  API_FUNCTION(kore_pattern_parse_file);
  API_FUNCTION(kore_pattern_new_token);
  API_FUNCTION(kore_pattern_new_token_with_len);
  API_FUNCTION(kore_pattern_new_injection);
  API_FUNCTION(kore_pattern_make_interpreter_input);
  API_FUNCTION(kore_composite_pattern_new);
  API_FUNCTION(kore_composite_pattern_from_symbol);
  API_FUNCTION(kore_composite_pattern_add_argument);
  API_FUNCTION(kore_string_pattern_new);
  API_FUNCTION(kore_string_pattern_new_with_len);
  API_FUNCTION(kore_pattern_construct);
  API_FUNCTION(kore_block_dump);
  API_FUNCTION(kore_pattern_from_block);
  API_FUNCTION(kore_block_get_bool);
  API_FUNCTION(kore_simplify_bool);
  API_FUNCTION(kore_simplify);
  API_FUNCTION(kore_simplify_binary);
  API_FUNCTION(take_steps);
  API_FUNCTION(kore_sort_dump);
  API_FUNCTION(kore_sort_free);
  API_FUNCTION(kore_sort_is_concrete);
  API_FUNCTION(kore_sort_is_kitem);
  API_FUNCTION(kore_sort_is_k);
  API_FUNCTION(kore_composite_sort_new);
  API_FUNCTION(kore_composite_sort_add_argument);
  API_FUNCTION(kore_symbol_new);
  API_FUNCTION(kore_symbol_free);
  API_FUNCTION(kore_symbol_add_formal_argument);
  API_FUNCTION(kllvm_init);
  API_FUNCTION(kllvm_free_all_memory);

  return api;
}
