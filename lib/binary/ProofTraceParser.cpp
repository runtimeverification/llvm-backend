#include <kllvm/binary/ProofTraceParser.h>

#include <fmt/format.h>

namespace kllvm {

constexpr auto indent_size = 2U;

llvm_function_event::llvm_function_event(
    std::string name, std::string relative_position)
    : name_(std::move(name))
    , relative_position_(std::move(relative_position)) { }

std::vector<llvm_event> const &llvm_function_event::getArguments() const {
  return arguments_;
}

void llvm_function_event::addArgument(llvm_event const &argument) {
  arguments_.push_back(argument);
}

llvm_hook_event::llvm_hook_event(std::string name, std::string relative_position)
    : name_(std::move(name))
    , relative_position_(std::move(relative_position))
    , kore_pattern_(nullptr) { }

void llvm_hook_event::addArgument(llvm_event const &argument) {
  arguments_.push_back(argument);
}

void llvm_rewrite_event::printSubstitution(
    std::ostream &out, unsigned indent) const {
  std::string Indent(indent * indent_size, ' ');
  for (auto const &p : substitution_) {
    out << fmt::format("{}{} = kore[{}]\n", Indent, p.first, p.second.second);
  }
}

void llvm_rule_event::print(std::ostream &out, unsigned indent) const {
  std::string Indent(indent * indent_size, ' ');
  out << fmt::format(
      "{}rule: {} {}\n", Indent, getRuleOrdinal(), getSubstitution().size());
  printSubstitution(out, indent + 1U);
}

void llvm_side_condition_event::print(std::ostream &out, unsigned indent) const {
  std::string Indent(indent * indent_size, ' ');
  out << fmt::format(
      "{}side condition entry: {} {}\n", Indent, getRuleOrdinal(),
      getSubstitution().size());
  printSubstitution(out, indent + 1U);
}

void llvm_side_condition_end_event::print(
    std::ostream &out, unsigned indent) const {
  std::string Indent(indent * indent_size, ' ');
  out << fmt::format(
      "{}side condition exit: {} kore[{}]\n", Indent, rule_ordinal_,
      pattern_length_);
}

void llvm_function_event::print(std::ostream &out, unsigned indent) const {
  std::string Indent(indent * indent_size, ' ');
  out << fmt::format("{}function: {} ({})\n", Indent, name_, relative_position_);
  for (auto const &arg : arguments_) {
    arg.print(out, true, indent + 1U);
  }
}

void llvm_hook_event::print(std::ostream &out, unsigned indent) const {
  std::string Indent(indent * indent_size, ' ');
  out << fmt::format("{}hook: {} ({})\n", Indent, name_, relative_position_);
  for (auto const &arg : arguments_) {
    arg.print(out, true, indent + 1U);
  }
  out << fmt::format("{}hook result: kore[{}]\n", Indent, pattern_length_);
}

void llvm_event::print(std::ostream &out, bool is_arg, unsigned indent) const {
  if (is_step_event_) {
    step_event_->print(out, indent);
  } else {
    std::string Indent(indent * indent_size, ' ');
    out << fmt::format(
        "{}{}: kore[{}]\n", Indent, is_arg ? "arg" : "config", pattern_length_);
  }
}

void llvm_rewrite_trace::print(std::ostream &out, unsigned indent) const {
  std::string Indent(indent * indent_size, ' ');
  out << fmt::format("{}version: {}\n", Indent, version_);
  for (auto const &pre_trace_event : pre_trace_) {
    pre_trace_event.print(out, false, indent);
  }
  initial_config_.print(out, false, indent);
  for (auto const &trace_event : trace_) {
    trace_event.print(out, false, indent);
  }
}

proof_trace_parser::proof_trace_parser(bool verbose)
    : verbose_(verbose) { }

std::optional<llvm_rewrite_trace>
proof_trace_parser::parse_proof_trace(std::string const &data) {
  auto ptr = data.begin();
  llvm_rewrite_trace trace;
  bool result = parse_trace(ptr, data.end(), trace);

  if (!result || ptr != data.end()) {
    return std::nullopt;
  }

  if (verbose_) {
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
