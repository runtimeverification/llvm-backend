#include <kllvm/ast/AST.h>
#include <kllvm/binary/ProofTraceParser.h>
#include <kllvm/binary/deserializer.h>
#include <kllvm/binary/serializer.h>
#include <kllvm/parser/KOREParser.h>

#include <pybind11/operators.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <cstddef>
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

/**
 * Rather than requiring the entire contents of the binary file to be read into
 * memory ahead of time, this binding uses the optional size field present in
 * version 1.2.0 of the binary format to read only the required portion of the
 * file. If the size is zero, or the input pattern uses an older version, an
 * exception will be thrown. The file pointer will be left at the end of the
 * pattern's bytes after calling this function.
 */
std::shared_ptr<kore_pattern> read_pattern_from_file(py::object &file_like) {
  if (!py::hasattr(file_like, "read")) {
    throw py::type_error("Argument to read_from is not a file-like object");
  }

  auto read_attr = file_like.attr("read");
  auto read = [&read_attr](auto len) -> std::string {
    return py::bytes(read_attr(len));
  };

  auto header = read(5);
  auto ref_header = serializer::magic_header;
  if (!std::equal(header.begin(), header.end(), ref_header.begin())) {
    throw std::invalid_argument(
        "Data does not begin with the binary KORE header bytes");
  }

  auto version_bytes = read(6);
  auto version_begin = version_bytes.begin();
  auto version
      = kllvm::detail::read_version(version_begin, version_bytes.end());

  if (version < binary_version(1, 2, 0)) {
    throw std::invalid_argument(
        "Pattern read from a file-like object must use version 1.2.0 or newer");
  }

  auto size_bytes = read(8);
  auto size_begin = size_bytes.begin();
  auto size = kllvm::detail::read_pattern_size_unchecked(
      size_begin, size_bytes.end());

  if (size == 0) {
    throw std::invalid_argument("Pattern size must be set explicitly when "
                                "reading from a file-like object");
  }

  auto pattern_bytes = read(size);
  auto pattern_begin = pattern_bytes.begin();
  return kllvm::detail::read(pattern_begin, pattern_bytes.end(), version);
}

void bind_ast(py::module_ &m) {
  auto ast = m.def_submodule("ast", "K LLVM backend KORE AST");

  /* Declarations */
  auto decl_base
      = py::class_<kore_declaration, std::shared_ptr<kore_declaration>>(
            ast, "Declaration")
            .def("__repr__", print_repr_adapter<kore_declaration>())
            .def(
                "add_object_sort_variable",
                &kore_declaration::add_object_sort_variable)
            .def_property_readonly(
                "object_sort_variables",
                &kore_declaration::get_object_sort_variables)
            .def(
                "add_attribute",
                [](kore_declaration &decl,
                   std::shared_ptr<kore_composite_pattern> const &arg) {
                  decl.attributes().add(arg);
                })
            .def_property_readonly("attributes", [](kore_declaration &decl) {
              return decl.attributes().underlying();
            });

  py::class_<
      kore_composite_sort_declaration,
      std::shared_ptr<kore_composite_sort_declaration>>(
      ast, "CompositeSortDeclaration", decl_base)
      .def(
          py::init(&kore_composite_sort_declaration::create), py::arg("name"),
          py::arg("is_hooked") = false)
      .def_property_readonly(
          "is_hooked", &kore_composite_sort_declaration::is_hooked)
      .def_property_readonly(
          "name", &kore_composite_sort_declaration::get_name);

  auto symbol_alias_decl_base
      = py::class_<
            kore_symbol_alias_declaration,
            std::shared_ptr<kore_symbol_alias_declaration>>(
            ast, "SymbolAliasDeclaration", decl_base)
            .def_property_readonly(
                "symbol", &kore_symbol_alias_declaration::get_symbol);

  py::class_<kore_symbol_declaration, std::shared_ptr<kore_symbol_declaration>>(
      ast, "SymbolDeclaration", symbol_alias_decl_base)
      .def(
          py::init(&kore_symbol_declaration::create), py::arg("name"),
          py::arg("is_hooked") = false)
      .def_property_readonly("is_hooked", &kore_symbol_declaration::is_hooked);

  py::class_<kore_alias_declaration, std::shared_ptr<kore_alias_declaration>>(
      ast, "AliasDeclaration", symbol_alias_decl_base)
      .def(py::init(&kore_alias_declaration::create))
      .def("add_variables", &kore_alias_declaration::add_variables)
      .def_property_readonly(
          "variables", &kore_alias_declaration::get_bound_variables)
      .def("add_pattern", &kore_alias_declaration::add_pattern)
      .def_property_readonly("pattern", &kore_alias_declaration::get_pattern);

  py::class_<kore_axiom_declaration, std::shared_ptr<kore_axiom_declaration>>(
      ast, "AxiomDeclaration", decl_base)
      .def(
          py::init(&kore_axiom_declaration::create),
          py::arg("is_claim") = false)
      .def_property_readonly("is_claim", &kore_axiom_declaration::is_claim)
      .def_property_readonly(
          "is_required", &kore_axiom_declaration::is_required)
      .def("add_pattern", &kore_axiom_declaration::add_pattern)
      .def_property_readonly("pattern", &kore_axiom_declaration::get_pattern)
      .def_property_readonly("requires", &kore_axiom_declaration::get_requires);

  py::class_<
      kore_module_import_declaration,
      std::shared_ptr<kore_module_import_declaration>>(
      ast, "ModuleImportDeclaration", decl_base)
      .def(py::init(&kore_module_import_declaration::create))
      .def_property_readonly(
          "module_name", &kore_module_import_declaration::get_module_name);

  py::class_<kore_module, std::shared_ptr<kore_module>>(ast, "Module")
      .def(py::init(&kore_module::create))
      .def("__repr__", print_repr_adapter<kore_module>())
      .def_property_readonly("name", &kore_module::get_name)
      .def("add_declaration", &kore_module::add_declaration)
      .def_property_readonly("declarations", &kore_module::get_declarations)
      .def(
          "add_attribute",
          [](kore_module &decl,
             std::shared_ptr<kore_composite_pattern> const &arg) {
            decl.attributes().add(arg);
          })
      .def_property_readonly("attributes", [](kore_module &decl) {
        return decl.attributes().underlying();
      });

  py::class_<kore_definition, std::shared_ptr<kore_definition>>(
      ast, "Definition")
      .def(py::init(&kore_definition::create))
      .def("__repr__", print_repr_adapter<kore_definition>())
      .def("add_module", &kore_definition::add_module)
      .def_property_readonly("modules", &kore_definition::get_modules)
      .def(
          "add_attribute",
          [](kore_definition &decl,
             std::shared_ptr<kore_composite_pattern> const &arg) {
            decl.attributes().add(arg);
          })
      .def("preprocess", &kore_definition::preprocess)
      .def("get_axiom_by_ordinal", &kore_definition::get_axiom_by_ordinal)
      .def_property_readonly("attributes", [](kore_definition &decl) {
        return decl.attributes().underlying();
      });

  /* Data Types */

  py::enum_<sort_category>(ast, "SortCategory")
      .value("Uncomputed", sort_category::Uncomputed)
      .value("Map", sort_category::Map)
      .value("MapIter", sort_category::MapIter)
      .value("RangeMap", sort_category::RangeMap)
      .value("List", sort_category::List)
      .value("Set", sort_category::Set)
      .value("SetIter", sort_category::SetIter)
      .value("Int", sort_category::Int)
      .value("Float", sort_category::Float)
      .value("StringBuffer", sort_category::StringBuffer)
      .value("Bool", sort_category::Bool)
      .value("Symbol", sort_category::Symbol)
      .value("Variable", sort_category::Variable)
      .value("MInt", sort_category::MInt);

  py::class_<value_type>(ast, "value_type")
      .def(py::init([](sort_category cat) {
        return value_type{cat, 0};
      }))
      .def(py::init([](sort_category cat, uint64_t bits) {
        return value_type{cat, bits};
      }));

  /* Sorts */

  // The "redundant" expressions here are used by Pybind's metaprogramming to
  // generate equality functions over the bound classes.
  // NOLINTBEGIN(misc-redundant-expression)

  auto sort_base
      = py::class_<kore_sort, std::shared_ptr<kore_sort>>(ast, "Sort")
            .def_property_readonly("is_concrete", &kore_sort::is_concrete)
            .def("substitute", &kore_sort::substitute)
            .def("__repr__", print_repr_adapter<kore_sort>())
            .def(
                "__hash__",
                [](kore_sort const &sort) { return hash_sort{}(sort); })
            .def(py::self == py::self)
            .def(py::self != py::self);

  py::class_<kore_sort_variable, std::shared_ptr<kore_sort_variable>>(
      ast, "SortVariable", sort_base)
      .def(py::init(&kore_sort_variable::create))
      .def_property_readonly("name", &kore_sort_variable::get_name);

  py::class_<kore_composite_sort, std::shared_ptr<kore_composite_sort>>(
      ast, "CompositeSort", sort_base)
      .def(
          py::init(&kore_composite_sort::create), py::arg("name"),
          py::arg("cat") = value_type{sort_category::Uncomputed, 0})
      .def_property_readonly("name", &kore_composite_sort::get_name)
      .def("add_argument", &kore_composite_sort::add_argument)
      .def_property_readonly("arguments", &kore_composite_sort::get_arguments);

  /* Symbols */

  py::class_<kore_symbol>(ast, "Symbol")
      .def(py::init(&kore_symbol::create))
      .def("__repr__", print_repr_adapter<kore_symbol>())
      .def("add_argument", &kore_symbol::add_argument)
      .def_property_readonly("arguments", &kore_symbol::get_arguments)
      .def("add_formal_argument", &kore_symbol::add_formal_argument)
      .def_property_readonly(
          "formal_arguments", &kore_symbol::get_formal_arguments)
      .def("add_sort", &kore_symbol::add_sort)
      .def_property_readonly(
          "sort", py::overload_cast<>(&kore_symbol::get_sort, py::const_))
      .def_property_readonly("name", &kore_symbol::get_name)
      .def_property_readonly("is_concrete", &kore_symbol::is_concrete)
      .def_property_readonly("is_builtin", &kore_symbol::is_builtin)
      .def(py::self == py::self)
      .def(py::self != py::self);

  py::class_<kore_variable>(ast, "Variable")
      .def(py::init(&kore_variable::create))
      .def("__repr__", print_repr_adapter<kore_variable>())
      .def_property_readonly("name", &kore_variable::get_name);

  // NOLINTEND(misc-redundant-expression)

  /* Patterns */

  auto pattern_base
      = py::class_<kore_pattern, std::shared_ptr<kore_pattern>>(ast, "Pattern")
            .def(py::init(&kore_pattern::load))
            .def("__repr__", print_repr_adapter<kore_pattern>())
            .def_property_readonly("sort", &kore_pattern::get_sort)
            .def("substitute", &kore_pattern::substitute)
            .def(
                "serialize",
                [](kore_pattern const &pattern, bool emit_size) {
                  auto out = serializer{};
                  pattern.serialize_to(out);

                  if (emit_size) {
                    out.correct_emitted_size();
                  }

                  return py::bytes(out.byte_string());
                },
                py::kw_only(), py::arg("emit_size") = false)
            .def_static(
                "deserialize",
                [](py::bytes const &bytes, bool strip_raw_term) {
                  auto str = std::string(bytes);
                  return deserialize_pattern(
                      str.begin(), str.end(), strip_raw_term);
                },
                py::arg("bytes"), py::kw_only(),
                py::arg("strip_raw_term") = true)
            .def_static("read_from", &read_pattern_from_file);

  py::class_<kore_composite_pattern, std::shared_ptr<kore_composite_pattern>>(
      ast, "CompositePattern", pattern_base)
      .def(py::init(py::overload_cast<std::string const &>(
          &kore_composite_pattern::create)))
      .def(py::init(
          py::overload_cast<kore_symbol *>(&kore_composite_pattern::create)))
      .def_property_readonly(
          "constructor", &kore_composite_pattern::get_constructor)
      .def("desugar_associative", &kore_composite_pattern::desugar_associative)
      .def("add_argument", &kore_composite_pattern::add_argument)
      .def_property_readonly(
          "arguments", &kore_composite_pattern::get_arguments);

  py::class_<kore_variable_pattern, std::shared_ptr<kore_variable_pattern>>(
      ast, "VariablePattern", pattern_base)
      .def(py::init(&kore_variable_pattern::create))
      .def_property_readonly("name", &kore_variable_pattern::get_name);

  py::class_<kore_string_pattern, std::shared_ptr<kore_string_pattern>>(
      ast, "StringPattern", pattern_base)
      .def(py::init(&kore_string_pattern::create))
      .def_property_readonly("contents", [](kore_string_pattern &pattern) {
        return py::bytes(pattern.get_contents());
      });
}

void bind_parser(py::module_ &mod) {
  auto parser = mod.def_submodule("parser", "KORE Parser");

  py::class_<kore_parser, std::unique_ptr<kore_parser>>(parser, "Parser")
      .def(py::init<std::string>())
      .def_static("from_string", &kore_parser::from_string)
      .def(
          "pattern",
          [](kore_parser &parser) { return std::shared_ptr(parser.pattern()); })
      .def("sort", [](kore_parser &parser) { return parser.sort(); })
      .def(
          "definition",
          [](kore_parser &parser) {
            return std::shared_ptr(parser.definition());
          })
      .def("symbol", &kore_parser::symbol);
}

void bind_proof_trace(py::module_ &m) {
  auto proof_trace = m.def_submodule("prooftrace", "K LLVM backend KORE AST");

  auto step_event
      = py::class_<llvm_step_event, std::shared_ptr<llvm_step_event>>(
            proof_trace, "llvm_step_event")
            .def("__repr__", print_repr_adapter<llvm_step_event>(true));

  auto rewrite_event
      = py::class_<llvm_rewrite_event, std::shared_ptr<llvm_rewrite_event>>(
            proof_trace, "llvm_rewrite_event", step_event)
            .def_property_readonly(
                "rule_ordinal", &llvm_rewrite_event::get_rule_ordinal)
            .def_property_readonly(
                "substitution", &llvm_rewrite_event::get_substitution);

  [[maybe_unused]] auto rule_event
      = py::class_<llvm_rule_event, std::shared_ptr<llvm_rule_event>>(
          proof_trace, "llvm_rule_event", rewrite_event);

  [[maybe_unused]] auto side_condition_event = py::class_<
      llvm_side_condition_event, std::shared_ptr<llvm_side_condition_event>>(
      proof_trace, "llvm_side_condition_event", rewrite_event);

  py::class_<
      llvm_side_condition_end_event,
      std::shared_ptr<llvm_side_condition_end_event>>(
      proof_trace, "llvm_side_condition_end_event", step_event)
      .def_property_readonly(
          "rule_ordinal", &llvm_side_condition_end_event::get_rule_ordinal)
      .def_property_readonly(
          "check_result", &llvm_side_condition_end_event::get_result);

  py::class_<
      llvm_pattern_matching_failure_event,
      std::shared_ptr<llvm_pattern_matching_failure_event>>(
      proof_trace, "llvm_pattern_matching_failure_event", step_event)
      .def_property_readonly(
          "function_name",
          &llvm_pattern_matching_failure_event::get_function_name);

  py::class_<
      llvm_tail_call_info_event,
      std::shared_ptr<llvm_tail_call_info_event>>(
      proof_trace, "llvm_tail_call_info_event", step_event)
      .def_property_readonly(
          "callern_name",
          &llvm_tail_call_info_event::get_caller_name)
      .def_property_readonly(
          "is_tail", &llvm_tail_call_info_event::is_tail);

  py::class_<llvm_function_event, std::shared_ptr<llvm_function_event>>(
      proof_trace, "llvm_function_event", step_event)
      .def_property_readonly("name", &llvm_function_event::get_name)
      .def_property_readonly(
          "relative_position", &llvm_function_event::get_relative_position)
      .def_property_readonly("args", &llvm_function_event::get_arguments);

  py::class_<llvm_hook_event, std::shared_ptr<llvm_hook_event>>(
      proof_trace, "llvm_hook_event", step_event)
      .def_property_readonly("name", &llvm_hook_event::get_name)
      .def_property_readonly("symbol_name", &llvm_hook_event::get_symbol_name)
      .def_property_readonly(
          "relative_position", &llvm_hook_event::get_relative_position)
      .def_property_readonly("args", &llvm_hook_event::get_arguments)
      .def_property_readonly("result", &llvm_hook_event::getkore_pattern);

  py::class_<llvm_event, std::shared_ptr<llvm_event>>(proof_trace, "Argument")
      .def("__repr__", print_repr_adapter<llvm_event>(true, true))
      .def_property_readonly("step_event", &llvm_event::get_step_event)
      .def_property_readonly("kore_pattern", &llvm_event::getkore_pattern)
      .def("is_step_event", &llvm_event::is_step)
      .def("is_kore_pattern", &llvm_event::is_pattern);

  py::class_<llvm_rewrite_trace, std::shared_ptr<llvm_rewrite_trace>>(
      proof_trace, "llvm_rewrite_trace")
      .def("__repr__", print_repr_adapter<llvm_rewrite_trace>(true))
      .def_property_readonly("version", &llvm_rewrite_trace::get_version)
      .def_property_readonly("pre_trace", &llvm_rewrite_trace::get_pre_trace)
      .def_property_readonly(
          "initial_config", &llvm_rewrite_trace::get_initial_config)
      .def_property_readonly("trace", &llvm_rewrite_trace::get_trace)
      .def_static(
          "parse",
          [](py::bytes const &bytes, std::shared_ptr<kore_header> &header) {
            proof_trace_parser parser(false, false, header);
            auto str = std::string(bytes);
            return parser.parse_proof_trace(str, false);
          },
          py::arg("bytes"), py::arg("header"));

  py::class_<kore_header, std::shared_ptr<kore_header>>(
      proof_trace, "kore_header")
      .def(py::init(&kore_header::create), py::arg("path"));

  py::enum_<llvm_event_type>(proof_trace, "EventType")
      .value("PreTrace", llvm_event_type::PreTrace)
      .value("InitialConfig", llvm_event_type::InitialConfig)
      .value("Trace", llvm_event_type::Trace);

  py::class_<annotated_llvm_event>(proof_trace, "annotated_llvm_event")
      .def_readonly("type", &annotated_llvm_event::type)
      .def_readonly("event", &annotated_llvm_event::event);

  py::class_<
      llvm_rewrite_trace_iterator,
      std::shared_ptr<llvm_rewrite_trace_iterator>>(
      proof_trace, "llvm_rewrite_trace_iterator")
      .def("__repr__", print_repr_adapter<llvm_rewrite_trace_iterator>(true))
      .def_static(
          "from_file",
          [](std::string const &filename,
             std::shared_ptr<kore_header> const &header) {
            std::ifstream file(filename, std::ios_base::binary);
            return llvm_rewrite_trace_iterator(
                std::make_unique<proof_trace_file_buffer>(std::move(file)),
                header);
          },
          py::arg("filename"), py::arg("header"))
      .def_property_readonly(
          "version", &llvm_rewrite_trace_iterator::get_version)
      .def("get_next_event", &llvm_rewrite_trace_iterator::get_next_event);
}

PYBIND11_MODULE(_kllvm, m) {
  bind_ast(m);
  bind_parser(m);
  bind_proof_trace(m);
}
