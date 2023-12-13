#include <kllvm/binary/ProofTraceParser.h>

namespace kllvm {

constexpr auto indent_size = 2u;

void LLVMRewriteEvent::printSubstitution(
    std::ostream &Out, unsigned indent) const {
  std::string Indent(indent * indent_size, ' ');
  for (const auto &p : substitution) {
    Out << Indent << p.first << " = kore[" << p.second.second << "]"
        << std::endl;
  }
}

void LLVMRuleEvent::print(std::ostream &Out, unsigned indent) const {
  std::string Indent(indent * indent_size, ' ');
  Out << Indent << "rule: " << ruleOrdinal << " " << substitution.size()
      << std::endl;
  printSubstitution(Out, indent + 1u);
}

void LLVMSideConditionEvent::print(std::ostream &Out, unsigned indent) const {
  std::string Indent(indent * indent_size, ' ');
  Out << Indent << "side condition: " << ruleOrdinal << " "
      << substitution.size() << std::endl;
  printSubstitution(Out, indent + 1u);
}

void LLVMFunctionEvent::print(std::ostream &Out, unsigned indent) const {
  std::string Indent(indent * indent_size, ' ');
  Out << Indent << "function: " << name << " (" << relativePosition << ")"
      << std::endl;
  for (const auto &arg : arguments) {
    arg.print(Out, true, indent + 1u);
  }
}

void LLVMHookEvent::print(std::ostream &Out, unsigned indent) const {
  std::string Indent(indent * indent_size, ' ');
  Out << Indent << "hook: " << name << " (" << relativePosition << ")"
      << std::endl;
  for (const auto &arg : arguments) {
    arg.print(Out, true, indent + 1u);
  }
  Out << Indent << "hook result: kore[" << patternLength << "]" << std::endl;
}

void LLVMEvent::print(std::ostream &Out, bool isArg, unsigned indent) const {
  if (isStepEvent) {
    stepEvent->print(Out, indent);
  } else {
    std::string Indent(indent * indent_size, ' ');
    if (isArg) {
      Out << Indent << "arg: kore[";
    } else {
      Out << Indent << "config: kore[";
    }
    Out << patternLength << "]" << std::endl;
  }
}

void LLVMRewriteTrace::print(std::ostream &Out, unsigned indent) const {
  std::string Indent(indent * indent_size, ' ');
  Out << Indent << "version: " << version << std::endl;
  for (const auto &pre_trace_event : preTrace) {
    pre_trace_event.print(Out, false, indent);
  }
  initialConfig.print(Out, false, indent);
  for (const auto &trace_event : trace) {
    trace_event.print(Out, false, indent);
  }
}

ProofTraceParser::ProofTraceParser(bool _verbose, uint32_t _expectedVersion)
    : verbose(_verbose)
    , expectedVersion(_expectedVersion) { }

std::optional<LLVMRewriteTrace>
ProofTraceParser::parse_proof_trace(std::string const &filename) {
  auto data = file_contents(filename);

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

} // namespace kllvm
