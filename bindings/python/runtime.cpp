#include <pybind11/pybind11.h>

namespace py = pybind11;

void bind_runtime(py::module_ &m) {
  auto runtime = m.def_submodule("runtime", "K LLVM backend runtime");
}

PYBIND11_MODULE(kllvm_runtime, m) {
  bind_runtime(m);
}
