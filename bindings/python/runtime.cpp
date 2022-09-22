#include <kllvm/ast/AST.h>

#include <pybind11/pybind11.h>

namespace py = pybind11;

using namespace kllvm;

/*
 * We can't use the pybind default holders because they'll try to take ownership
 * of the runtime's objects. This is the minimum viable holder that does _not_
 * express ownership of the underlying object.
 */
template <typename T>
struct raw_ptr {
  T *ptr_;
  T *get() { return ptr_; }
  raw_ptr(T *ptr)
      : ptr_(ptr) { }
};
PYBIND11_DECLARE_HOLDER_TYPE(T, raw_ptr<T>, true);

// TODO:  fix name clashes with macros in header.h so that we don't need to slice
//        these ones out.
extern "C" {
typedef struct blockheader {
  uint64_t hdr;
} blockheader;

typedef struct block {
  blockheader h;
  uint64_t *children[];
} block;

typedef struct string {
  blockheader h;
  char data[];
} string;

/* block *parseConfiguration(const char *filename); */

/* block *take_steps(int64_t, block *); */
/* void finish_rewriting(block *, bool); */
/* void printConfiguration(const char *filename, block *subject); */
/* void printConfigurationToFile(FILE *file, block *subject); */
void initStaticObjects();
string *printConfigurationToString(block *subject);
}

void *constructInitialConfiguration(const KOREPattern *initial);
static block *construct_stub(const KOREPattern *initial) {
  return static_cast<block *>(constructInitialConfiguration(initial));
}

void bind_runtime(py::module_ &m) {
  auto runtime = m.def_submodule("runtime", "K LLVM backend runtime");

  py::class_<block, raw_ptr<block>>(m, "Term")
      .def(py::init(
          [](KOREPattern const *init) { return construct_stub(init); }))
      .def("__str__", [](block *term) {
        return printConfigurationToString(term)->data;
      });
}

PYBIND11_MODULE(_kllvm_runtime, m) {
  initStaticObjects();

  bind_runtime(m);
}
