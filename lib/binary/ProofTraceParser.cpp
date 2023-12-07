#include <kllvm/binary/ProofTraceParser.h>

namespace kllvm {

#define INDENT_SIZE 2u

void LLVMRewriteEvent::printSubstitution(
    std::ostream &Out, unsigned indent) const {
  std::string Indent(indent * INDENT_SIZE, ' ');
  for (const auto &p : substitution) {
    Out << Indent << p.first << " = kore[" << p.second.second << "]"
        << std::endl;
  }
}

void LLVMRuleEvent::print(std::ostream &Out, unsigned indent) const {
  std::string Indent(indent * INDENT_SIZE, ' ');
  Out << Indent << "rule: " << ruleOrdinal << " " << substitution.size()
      << std::endl;
  printSubstitution(Out, indent + 1u);
}

void LLVMSideConditionEvent::print(std::ostream &Out, unsigned indent) const {
  std::string Indent(indent * INDENT_SIZE, ' ');
  Out << Indent << "side condition: " << ruleOrdinal << " "
      << substitution.size() << std::endl;
  printSubstitution(Out, indent + 1u);
}

void LLVMFunctionEvent::print(std::ostream &Out, unsigned indent) const {
  std::string Indent(indent * INDENT_SIZE, ' ');
  Out << Indent << "function: " << name << " (" << relativePosition << ")"
      << std::endl;
  for (const auto &arg : arguments) {
    arg.print(Out, true, indent + 1u);
  }
}

void LLVMHookEvent::print(std::ostream &Out, unsigned indent) const {
  std::string Indent(indent * INDENT_SIZE, ' ');
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
    std::string Indent(indent * INDENT_SIZE, ' ');
    if (isArg) {
      Out << Indent << "arg: kore[";
    } else {
      Out << Indent << "config: kore[";
    }
    Out << patternLength << "]" << std::endl;
  }
}

void LLVMRewriteTrace::print(std::ostream &Out, unsigned indent) const {
  std::string Indent(indent * INDENT_SIZE, ' ');
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

bool ProofTraceParser::parse_proof_trace(
    std::string const &filename, LLVMRewriteTrace &trace) {
  auto data = file_contents(filename);

  auto ptr = data.begin();
  bool result = parse_trace(ptr, data.end(), trace);

  if (ptr != data.end()) {
    return false;
  }

  if (result && verbose) {
    trace.print(std::cout);
  }

  return result;
}

} // namespace kllvm
