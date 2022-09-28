#include <kllvm/ast/AST.h>
#include <kllvm/parser/KOREParser.h>

#include <pybind11/operators.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <sstream>
#include <tuple>

namespace py = pybind11;

using namespace kllvm;
using namespace kllvm::parser;

// Metaprogramming support for the adapter function between AST print() methods
// and Python's __repr__.
namespace detail {

template <typename T>
struct type_identity {
  using type = T;
};

template <typename T, typename... Args>
struct print_repr_adapter_st {
  print_repr_adapter_st(type_identity<T>, Args &&...args)
      : args_(std::forward<Args>(args)...) { }

  std::string operator()(T &node) {
    auto ss = std::stringstream{};

    std::apply(
        [&](auto &&...args) { return node.print(args...); },
        std::tuple_cat(
            std::tuple{std::ref(ss)}, std::forward<decltype(args_)>(args_)));

    return ss.str();
  }

private:
  std::tuple<Args...> args_;
};

template <typename T, typename... Args>
print_repr_adapter_st(type_identity<T>, Args &&...)
    -> print_repr_adapter_st<T, Args...>;

} // namespace detail

/**
 * Adapt an AST node's print method to return a string for use with Python's
 * __repr__ method.
 */
template <typename T, typename... Args>
auto print_repr_adapter(Args &&...args) {
  return ::detail::print_repr_adapter_st(
      ::detail::type_identity<T>{}, std::forward<Args>(args)...);
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
            .def("__repr__", print_repr_adapter<KORESort>())
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
      .def("__repr__", print_repr_adapter<KORESymbol>())
      .def("add_formal_argument", &KORESymbol::addFormalArgument)
      .def(py::self == py::self)
      .def(py::self != py::self);

  py::class_<KOREVariable>(ast, "Variable")
      .def(py::init(&KOREVariable::Create))
      .def("__repr__", print_repr_adapter<KOREVariable>())
      .def_property_readonly("name", &KOREVariable::getName);

  /* Patterns */

  auto pattern_base
      = py::class_<KOREPattern, std::shared_ptr<KOREPattern>>(ast, "Pattern")
            .def(py::init(&KOREPattern::load))
            .def("__repr__", print_repr_adapter<KOREPattern>())
            .def("substitute", &KOREPattern::substitute);

  py::class_<KORECompositePattern, std::shared_ptr<KORECompositePattern>>(
      ast, "CompositePattern", pattern_base)
      .def(py::init(py::overload_cast<std::string const &>(
          &KORECompositePattern::Create)))
      .def(py::init(
          py::overload_cast<KORESymbol *>(&KORECompositePattern::Create)))
      .def("add_argument", &KORECompositePattern::addArgument);

  py::class_<KOREVariablePattern, std::shared_ptr<KOREVariablePattern>>(
      ast, "VariablePattern", pattern_base)
      .def(py::init(&KOREVariablePattern::Create))
      .def_property_readonly("name", &KOREVariablePattern::getName)
      .def_property_readonly("sort", &KOREVariablePattern::getSort);

  py::class_<KOREStringPattern, std::shared_ptr<KOREStringPattern>>(
      ast, "StringPattern", pattern_base)
      .def(py::init(&KOREStringPattern::Create))
      .def_property_readonly("contents", &KOREStringPattern::getContents);
}

void bind_parser(py::module_ &mod) {
  auto parser = mod.def_submodule("parser", "KORE Parser");

  py::class_<KOREParser, std::unique_ptr<KOREParser>>(parser, "Parser")
      .def(py::init<std::string>())
      .def_static("from_string", &KOREParser::from_string)
      .def("pattern", [](KOREParser &parser) {
        return std::shared_ptr(parser.pattern());
      });
}

PYBIND11_MODULE(_kllvm, m) {
  bind_ast(m);
  bind_parser(m);
}
