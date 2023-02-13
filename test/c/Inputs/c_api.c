#include <assert.h>
#include <dlfcn.h>

#include "c_api.h"

static void *dlsym_checked(void *lib, char const *sym_name) {
  void *symbol = dlsym(lib, sym_name);
  assert(symbol && sym_name);
  return symbol;
}

struct c_api load_c_api(char const *path) {
  void *lib = dlopen(path, RTLD_NOW);
  assert(lib && "Failed to load C API");

  struct c_api ret = {
      .init = dlsym_checked(lib, "kllvm_init"),
      .gc_free_all = dlsym_checked(lib, "kllvm_free_all_memory"),
      .composite_pattern_new = dlsym_checked(lib, "kore_composite_pattern_new"),
      .composite_sort_new = dlsym_checked(lib, "kore_composite_sort_new"),
      .simplify = dlsym_checked(lib, "kore_simplify"),
      .pattern_free = dlsym_checked(lib, "kore_pattern_free"),
      .sort_free = dlsym_checked(lib, "kore_sort_free"),
  };

  return ret;
}
