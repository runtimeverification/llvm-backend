#include <kllvm/binary/ProofTraceParser.h>

#include <fmt/format.h>

namespace kllvm {

constexpr auto indent_size = 2U;

void LLVMRewriteEvent::printSubstitution(
    std::ostream &Out, unsigned indent) const {
  std::string Indent(indent * indent_size, ' ');
  for (auto const &p : substitution) {
    Out << fmt::format("{}{} = kore[{}]\n", Indent, p.first, p.second.second);
  }
}

void LLVMRuleEvent::print(std::ostream &Out, unsigned indent) const {
  std::string Indent(indent * indent_size, ' ');
  Out << fmt::format(
      "{}rule: {} {}\n", Indent, ruleOrdinal, substitution.size());
  printSubstitution(Out, indent + 1U);
}

void LLVMSideConditionEvent::print(std::ostream &Out, unsigned indent) const {
  std::string Indent(indent * indent_size, ' ');
  Out << fmt::format(
      "{}side condition: {} {}\n", Indent, ruleOrdinal, substitution.size());
  printSubstitution(Out, indent + 1U);
}

void LLVMFunctionEvent::print(std::ostream &Out, unsigned indent) const {
  std::string Indent(indent * indent_size, ' ');
  Out << fmt::format("{}function: {} ({})\n", Indent, name, relativePosition);
  for (auto const &arg : arguments) {
    arg.print(Out, true, indent + 1U);
  }
}

void LLVMHookEvent::print(std::ostream &Out, unsigned indent) const {
  std::string Indent(indent * indent_size, ' ');
  Out << fmt::format("{}hook: {} ({})\n", Indent, name, relativePosition);
  for (auto const &arg : arguments) {
    arg.print(Out, true, indent + 1U);
  }
  Out << fmt::format("{}hook result: kore[{}]\n", Indent, patternLength);
}

void LLVMEvent::print(std::ostream &Out, bool isArg, unsigned indent) const {
  if (isStepEvent) {
    stepEvent->print(Out, indent);
  } else {
    std::string Indent(indent * indent_size, ' ');
    Out << fmt::format(
        "{}{}: kore[{}]\n", Indent, isArg ? "arg" : "config", patternLength);
  }
}

void LLVMRewriteTrace::print(std::ostream &Out, unsigned indent) const {
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

ProofTraceParser::ProofTraceParser(bool _verbose)
    : verbose(_verbose) { }

std::optional<LLVMRewriteTrace>
ProofTraceParser::parse_proof_trace(std::string const &data) {
  auto ptr = data.begin();
  LLVMRewriteTrace trace;
  bool result = parse_trace(ptr, data.end(), trace);

  if (!result || ptr != data.end()) {
    return std::nullopt;
  }

  if (verbose) {
    trace.print(std::cout);
  }

  return trace;
}

std::optional<LLVMRewriteTrace>
ProofTraceParser::parse_proof_trace_from_file(std::string const &filename) {
  auto data = file_contents(filename);
  return parse_proof_trace(data);
}

} // namespace kllvm
