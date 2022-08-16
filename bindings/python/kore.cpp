#include <kllvm/ast/AST.h>
#include <kllvm/parser/KOREParser.h>

#include <pybind11/pybind11.h>

#include <iostream>

namespace py = pybind11;

using namespace kllvm;
using namespace kllvm::parser;

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

block *parseConfiguration(const char *filename);

block *take_steps(int64_t, block *);
void finish_rewriting(block *, bool);
void printConfiguration(const char *filename, block *subject);
void initStaticObjects();
}

void *constructInitialConfiguration(const KOREPattern *initial);
static block *construct_stub(const KOREPattern *initial) {
  return static_cast<block *>(constructInitialConfiguration(initial));
}

void bind_runtime(py::module_ &mod) {
  py::class_<block, raw_ptr<block>>(mod, "Block");

  mod.def("parse_configuration", &parseConfiguration);
  mod.def("take_steps", &take_steps);
  mod.def("print_configuration", &printConfiguration);
}

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

  py::class_<KORESymbol>(ast, "Symbol")
      .def(py::init(&KORESymbol::Create))
      .def(
          "print",
          [](KORESymbol const &sym) {
            sym.print(std::cout);
            std::cout << '\n';
          })
      .def("add_formal_argument", &KORESymbol::addFormalArgument);

  py::class_<KOREVariable>(ast, "Variable")
      .def(py::init(&KOREVariable::Create))
      .def_property_readonly("name", &KOREVariable::getName);

  /* Patterns */

  auto pattern_base
      = py::class_<KOREPattern, std::shared_ptr<KOREPattern>>(ast, "Pattern")
            .def(py::init(&KOREPattern::load))
            .def(
                "print",
                [](KOREPattern const &p) {
                  p.print(std::cout);
                  std::cout << '\n';
                })
            .def_property_readonly("sort", &KOREPattern::getSort)
            .def("construct", [](KOREPattern const &pat) {
              return construct_stub(&pat);
            });

  py::class_<KOREVariablePattern, std::shared_ptr<KOREVariablePattern>>(
      ast, "VariablePattern", pattern_base)
      .def(py::init(&KOREVariablePattern::Create))
      .def("dump", [](KOREVariablePattern const &p) {
        p.print(std::cout);
        std::cout << '\n';
      });

  py::class_<KORECompositePattern, std::shared_ptr<KORECompositePattern>>(
      ast, "CompositePattern", pattern_base)
      .def(py::init(py::overload_cast<std::string const &>(
          &KORECompositePattern::Create)))
      .def(py::init(
          py::overload_cast<KORESymbol *>(&KORECompositePattern::Create)))
      .def("add_argument", &KORECompositePattern::addArgument);

  py::class_<KOREStringPattern, std::shared_ptr<KOREStringPattern>>(
      ast, "StringPattern", pattern_base)
      .def(py::init(&KOREStringPattern::Create));

  /* Top-level */

  py::class_<KOREModule>(ast, "Module");

  py::class_<KOREDefinition>(ast, "Definition")
      .def(py::init(&KOREDefinition::Create))
      .def("print", [](KOREDefinition const &def) { def.print(std::cout); });
}

void bind_parser(py::module_ &mod) {
  auto parser = mod.def_submodule("parser", "KORE Parser submodule");

  py::class_<KOREParser>(parser, "Parser")
      .def(py::init<std::string>())
      .def(
          "pattern",
          [](KOREParser &parser) { return std::shared_ptr(parser.pattern()); })
      .def("definition", &KOREParser::definition)
      .def_static("from_string", &KOREParser::from_string);

  /* .def("declarations", &KOREParser::declarations) */
  /* .def("symbol_sort_list", &KOREParser::symbol_sort_list) */
}

PYBIND11_MODULE(kore, mod) {
  initStaticObjects();

  bind_ast(mod);
  bind_parser(mod);
  bind_runtime(mod);
}
