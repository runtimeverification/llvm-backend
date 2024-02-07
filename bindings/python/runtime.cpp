#include <kllvm/ast/AST.h>
#include <kllvm/bindings/core/core.h>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

// This header needs to be included last because it pollutes a number of macro
// definitions into the global namespace.
#include <runtime/header.h>

namespace py = pybind11;

using namespace kllvm;

/*
 * We can't use the pybind default holders because they'll try to take ownership
 * of the runtime's objects. This is the minimum viable holder that does _not_
 * express ownership of the underlying object.
 */
template <typename T>
class raw_ptr {
public:
  explicit raw_ptr(T *ptr)
      : ptr_(ptr) { }

  raw_ptr()
      : raw_ptr(nullptr) { }

  T *get() { return ptr_; }

private:
  T *ptr_;
};

PYBIND11_DECLARE_HOLDER_TYPE(T, raw_ptr<T>, true);

/*
 * These declarations are internal to the backend and aren't exposed explicitly
 * through any header files, so we pull them in manually here.
 */

extern "C" {
void initStaticObjects(void);
void freeAllKoreMem(void);
block *take_steps(int64_t, block *);
void *constructInitialConfiguration(KOREPattern const *initial);
}

void bind_runtime(py::module_ &m) {
  auto runtime = m.def_submodule("runtime", "K LLVM backend runtime");

  // These simplifications should really be member functions on the Python
  // Pattern class, but they depend on the runtime library and so need to be
  // bound as free functions in the kllvm.runtime module.
  m.def("simplify_pattern", bindings::simplify);
  m.def("simplify_bool_pattern", bindings::simplify_to_bool);

  m.def("return_sort_for_label", bindings::return_sort_for_label);

  m.def("evaluate_function", bindings::evaluate_function);

  m.def("init_static_objects", initStaticObjects);
  m.def("free_all_gc_memory", freeAllKoreMem);

  // This class can't be used directly from Python; the mutability semantics
  // that we get from the Pybind wrappers make it really easy to break things.
  // We therefore have to wrap it up in some external Python code; see
  // package/kllvm/__init__.py for the details of the external class.
  py::class_<block, raw_ptr<block>>(m, "InternalTerm", py::module_local())
      .def(py::init([](KOREPattern const *init) {
        return static_cast<block *>(constructInitialConfiguration(init));
      }))
      .def(
          "__str__",
          [](block *term) {
            auto *k_str = printConfigurationToString(term);
            return std::string(k_str->data, len(k_str));
          })
      .def("step", [](block *term, int64_t n) { return take_steps(n, term); })
      .def("to_pattern", [](block *term) { return termToKorePattern(term); })
      .def(
          "serialize",
          [](block *term, bool emit_size) {
            char *data = nullptr;
            size_t size = 0;
            serializeConfiguration(term, nullptr, &data, &size, emit_size);
            return py::bytes(std::string(data, data + size));
          },
          py::kw_only(), py::arg("emit_size") = false)
      .def(
          "deserialize",
          [](py::bytes const &bytes) {
            auto str = std::string(bytes);
            return deserializeConfiguration(str.data(), str.size());
          })
      .def(
          "_serialize_raw", [](block *term, std::string const &filename,
                               std::string const &sort) {
            serializeRawTermToFile(filename.c_str(), term, sort.c_str());
          });
}

PYBIND11_MODULE(_kllvm_runtime, m) {
  bind_runtime(m);
}
