#include <kllvm/ast/AST.h>
#include <kllvm/parser/KOREParser.h>

#include <pybind11/pybind11.h>

#include <iostream>

namespace py = pybind11;

using namespace kllvm;
using namespace kllvm::parser;

void bind_ast(py::module_ &mod) {
  auto ast = mod.def_submodule("ast", "KORE AST submodule");

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
            .def("print", [](KORESort &sort) {
              sort.print(std::cout);
              std::cout << '\n';
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

  /* Symbols */

  py::class_<KOREVariable>(ast, "Variable")
      .def(py::init(&KOREVariable::Create))
      .def_property_readonly("name", &KOREVariable::getName);

  py::class_<KOREModule>(ast, "Module");

  py::class_<KOREDefinition>(ast, "Definition")
      .def(py::init(&KOREDefinition::Create))
      .def("print", [](KOREDefinition const &def) { def.print(std::cout); });
}

void bind_parser(py::module_ &mod) {
  auto parser = mod.def_submodule("parser", "KORE Parser submodule");

  py::class_<KOREParser>(parser, "Parser")
      .def(py::init<std::string>())
      .def("definition", &KOREParser::definition);
  /* .def_static("from_string", &KOREParser::from_string) */
  /* .def("pattern", &KOREParser::pattern) */
  /* .def("declarations", &KOREParser::declarations) */
  /* .def("symbol_sort_list", &KOREParser::symbol_sort_list) */
}

PYBIND11_MODULE(kore, mod) {
  bind_ast(mod);
  bind_parser(mod);
}
