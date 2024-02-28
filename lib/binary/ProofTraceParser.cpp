#include <kllvm/binary/ProofTraceParser.h>

#include <fmt/format.h>

namespace kllvm {

constexpr auto indent_size = 2U;

llvm_function_event::llvm_function_event(
    std::string _name, std::string _relativePosition)
    : name(std::move(_name))
    , relativePosition(std::move(_relativePosition)) { }

std::vector<llvm_event> const &llvm_function_event::getArguments() const {
  return arguments;
}

void llvm_function_event::addArgument(llvm_event const &argument) {
  arguments.push_back(argument);
}

llvm_hook_event::llvm_hook_event(std::string _name, std::string _relativePosition)
    : name(std::move(_name))
    , relativePosition(std::move(_relativePosition))
    , korePattern(nullptr) { }

void llvm_hook_event::addArgument(llvm_event const &argument) {
  arguments.push_back(argument);
}

void llvm_rewrite_event::printSubstitution(
    std::ostream &Out, unsigned indent) const {
  std::string Indent(indent * indent_size, ' ');
  for (auto const &p : substitution) {
    Out << fmt::format("{}{} = kore[{}]\n", Indent, p.first, p.second.second);
  }
}

void llvm_rule_event::print(std::ostream &Out, unsigned indent) const {
  std::string Indent(indent * indent_size, ' ');
  Out << fmt::format(
      "{}rule: {} {}\n", Indent, getRuleOrdinal(), getSubstitution().size());
  printSubstitution(Out, indent + 1U);
}

void llvm_side_condition_event::print(std::ostream &Out, unsigned indent) const {
  std::string Indent(indent * indent_size, ' ');
  Out << fmt::format(
      "{}side condition entry: {} {}\n", Indent, getRuleOrdinal(),
      getSubstitution().size());
  printSubstitution(Out, indent + 1U);
}

void llvm_side_condition_end_event::print(
    std::ostream &Out, unsigned indent) const {
  std::string Indent(indent * indent_size, ' ');
  Out << fmt::format(
      "{}side condition exit: {} kore[{}]\n", Indent, ruleOrdinal,
      patternLength);
}

void llvm_function_event::print(std::ostream &Out, unsigned indent) const {
  std::string Indent(indent * indent_size, ' ');
  Out << fmt::format("{}function: {} ({})\n", Indent, name, relativePosition);
  for (auto const &arg : arguments) {
    arg.print(Out, true, indent + 1U);
  }
}

void llvm_hook_event::print(std::ostream &Out, unsigned indent) const {
  std::string Indent(indent * indent_size, ' ');
  Out << fmt::format("{}hook: {} ({})\n", Indent, name, relativePosition);
  for (auto const &arg : arguments) {
    arg.print(Out, true, indent + 1U);
  }
  Out << fmt::format("{}hook result: kore[{}]\n", Indent, patternLength);
}

void llvm_event::print(std::ostream &Out, bool isArg, unsigned indent) const {
  if (isStepEvent) {
    stepEvent->print(Out, indent);
  } else {
    std::string Indent(indent * indent_size, ' ');
    Out << fmt::format(
        "{}{}: kore[{}]\n", Indent, isArg ? "arg" : "config", patternLength);
  }
}

void llvm_rewrite_trace::print(std::ostream &Out, unsigned indent) const {
  std::string Indent(indent * indent_size, ' ');
  Out << fmt::format("{}version: {}\n", Indent, version);
  for (auto const &pre_trace_event : preTrace) {
    pre_trace_event.print(Out, false, indent);
  }
  initialConfig.print(Out, false, indent);
  for (auto const &trace_event : trace) {
    trace_event.print(Out, false, indent);
  }
}

proof_trace_parser::proof_trace_parser(bool _verbose)
    : verbose(_verbose) { }

std::optional<llvm_rewrite_trace>
proof_trace_parser::parse_proof_trace(std::string const &data) {
  auto ptr = data.begin();
  llvm_rewrite_trace trace;
  bool result = parse_trace(ptr, data.end(), trace);

  if (!result || ptr != data.end()) {
    return std::nullopt;
  }

  if (verbose) {
    trace.print(std::cout);
  }

  return trace;
}

std::optional<llvm_rewrite_trace>
proof_trace_parser::parse_proof_trace_from_file(std::string const &filename) {
  auto data = file_contents(filename);
  return parse_proof_trace(data);
}

} // namespace kllvm
