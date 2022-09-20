#include <kllvm/ast/AST.h>
#include <kllvm/parser/KOREParser.h>

#include <pybind11/operators.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <sstream>

#include "runtime.h"

namespace py = pybind11;

using namespace kllvm;
using namespace kllvm::parser;

/**
 * Adapt an AST node's print method to return a string for use with Python's
 * __repr__ method.
 */
template <typename T>
std::string print_repr_adapter(T &node) {
  auto ss = std::stringstream{};
  node.print(ss);
  return ss.str();
}

void bind_ast(py::module_ &m) {
  auto ast = m.def_submodule("ast", "K LLVM backend KORE AST");

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
            .def("substitute", &KORESort::substitute)
            .def("__repr__", print_repr_adapter<KORESort>)
            .def(
                "__hash__",
                [](KORESort const &sort) { return HashSort{}(sort); })
            .def(py::self == py::self)
            .def(py::self != py::self);

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

  /* Symbols */

  py::class_<KORESymbol>(ast, "Symbol")
      .def(py::init(&KORESymbol::Create))
      .def("print", print_repr_adapter<KORESymbol>)
      .def("add_formal_argument", &KORESymbol::addFormalArgument);

  py::class_<KOREVariable>(ast, "Variable")
      .def(py::init(&KOREVariable::Create))
      .def_property_readonly("name", &KOREVariable::getName);
}

PYBIND11_MODULE(_kllvm, m) {
  bind_ast(m);
}
