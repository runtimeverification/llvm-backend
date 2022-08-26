#include <kllvm/ast/AST.h>
#include <kllvm/parser/KOREParser.h>

#include <pybind11/pybind11.h>

#include <sstream>

#include "runtime.h"

namespace py = pybind11;

using namespace kllvm;
using namespace kllvm::parser;

void bind_ast(py::module_ &m) {
  auto ast = m.def_submodule("ast", "KLLVM AST submodule");

  /* Data Types */

  py::enum_<SortCategory>(ast, "SortCategory")
      .value("Uncomputed", SortCategory::Uncomputed)
      .value("Map", SortCategory::Map)
      .value("List", SortCategory::List)
      .value("Set", SortCategory::Set)
      .value("Int", SortCategory::Int)
      .value("Float", SortCategory::Float)
      .value("StringBuffer", SortCategory::StringBuffer)
      .value("Bool", SortCategory::Bool)
      .value("Symbol", SortCategory::Symbol)
      .value("Variable", SortCategory::Variable)
      .value("MInt", SortCategory::MInt);

  py::class_<ValueType>(ast, "ValueType")
      .def(py::init([](SortCategory cat) {
        return ValueType{cat, 0};
      }))
      .def(py::init([](SortCategory cat, uint64_t bits) {
        return ValueType{cat, bits};
      }));

  /* Sorts */

  auto sort_base
      = py::class_<KORESort, std::shared_ptr<KORESort>>(ast, "Sort")
            .def_property_readonly("is_concrete", &KORESort::isConcrete)
            .def("__repr__", [](KORESort &sort) {
              auto ss = std::stringstream{};
              sort.print(ss);
              return ss.str();
            });

  py::class_<KORESortVariable, std::shared_ptr<KORESortVariable>>(
      ast, "SortVariable", sort_base)
      .def(py::init(&KORESortVariable::Create));

  py::class_<KORECompositeSort, std::shared_ptr<KORECompositeSort>>(
      ast, "CompositeSort", sort_base)
      .def(
          py::init(&KORECompositeSort::Create), py::arg("name"),
          py::arg("cat") = ValueType{SortCategory::Uncomputed, 0})
      .def("add_argument", &KORECompositeSort::addArgument)
      .def_property_readonly("name", &KORECompositeSort::getName);
}

PYBIND11_MODULE(kllvm, m) {
#ifdef KLLVM_BUILD_RUNTIME
  bind_runtime(m);
#endif

  bind_ast(m);
}
