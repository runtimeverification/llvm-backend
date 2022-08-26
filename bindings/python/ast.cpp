#include <pybind11/pybind11.h>

namespace py = pybind11;

#include "runtime.h"

PYBIND11_MODULE(kllvm, m) {
#ifdef KLLVM_BUILD_RUNTIME
  bind_runtime(m);
#endif
}
