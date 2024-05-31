#include <kllvm/binary/ProofTraceParser.h>

#include <fmt/format.h>
#include <fstream>

namespace kllvm {

constexpr auto indent_size = 2U;

llvm_function_event::llvm_function_event(
    std::string name, std::string relative_position)
    : name_(std::move(name))
    , relative_position_(std::move(relative_position)) { }

std::vector<llvm_event> const &llvm_function_event::get_arguments() const {
  return arguments_;
}

void llvm_function_event::add_argument(llvm_event const &argument) {
  arguments_.push_back(argument);
}

llvm_hook_event::llvm_hook_event(
    std::string name, std::string symbol_name, std::string relative_position)
    : name_(std::move(name))
    , symbol_name_(std::move(symbol_name))
    , relative_position_(std::move(relative_position))
    , kore_pattern_(nullptr) { }

void llvm_hook_event::add_argument(llvm_event const &argument) {
  arguments_.push_back(argument);
}

void llvm_rewrite_event::print_substitution(
    std::ostream &out, bool expand_terms, unsigned ind) const {
  std::string indent(ind * indent_size, ' ');
  for (auto const &p : substitution_) {
    if (expand_terms) {
      out << fmt::format("{}{} = kore[", indent, p.first);
      p.second.first->strip_injections()->print(out);
      out << fmt::format("]\n");
    } else {
      out << fmt::format("{}{} = kore[{}]\n", indent, p.first, p.second.second);
    }
  }
}

void llvm_rule_event::print(
    std::ostream &out, bool expand_terms, unsigned ind) const {
  std::string indent(ind * indent_size, ' ');
  out << fmt::format(
      "{}rule: {} {}\n", indent, get_rule_ordinal(), get_substitution().size());
  print_substitution(out, expand_terms, ind + 1U);
}

void llvm_side_condition_event::print(
    std::ostream &out, bool expand_terms, unsigned ind) const {
  std::string indent(ind * indent_size, ' ');
  out << fmt::format(
      "{}side condition entry: {} {}\n", indent, get_rule_ordinal(),
      get_substitution().size());
  print_substitution(out, expand_terms, ind + 1U);
}

void llvm_side_condition_end_event::print(
    std::ostream &out, bool expand_terms, unsigned ind) const {
  std::string indent(ind * indent_size, ' ');
  out << fmt::format("{}side condition exit: {} ", indent, rule_ordinal_);
  out << (result_ ? "true" : "false");
  out << fmt::format("\n");
}

void llvm_function_event::print(
    std::ostream &out, bool expand_terms, unsigned ind) const {
  std::string indent(ind * indent_size, ' ');
  out << fmt::format(
      "{}function: {} ({})\n", indent, name_, relative_position_);
  for (auto const &arg : arguments_) {
    arg.print(out, expand_terms, true, ind + 1U);
  }
}

void llvm_hook_event::print(
    std::ostream &out, bool expand_terms, unsigned ind) const {
  std::string indent(ind * indent_size, ' ');
  out << fmt::format(
      "{}hook: {} {} ({})\n", indent, name_, symbol_name_, relative_position_);
  for (auto const &arg : arguments_) {
    arg.print(out, expand_terms, true, ind + 1U);
  }
  if (expand_terms) {
    out << fmt::format("{}hook result: kore[", indent);
    kore_pattern_->strip_injections()->print(out);
    out << fmt::format("]\n");
  } else {
    out << fmt::format("{}hook result: kore[{}]\n", indent, pattern_length_);
  }
}

void llvm_event::print(
    std::ostream &out, bool expand_terms, bool is_arg, unsigned ind) const {
  if (is_step_event_) {
    step_event_->print(out, expand_terms, ind);
  } else {
    std::string indent(ind * indent_size, ' ');
    if (expand_terms) {
      out << fmt::format("{}{}: kore[", indent, is_arg ? "arg" : "config");
      kore_pattern_->strip_injections()->print(out);
      out << fmt::format("]\n");
    } else {
      out << fmt::format(
          "{}{}: kore[{}]\n", indent, is_arg ? "arg" : "config",
          pattern_length_);
    }
  }
}

llvm_rewrite_trace_iterator::llvm_rewrite_trace_iterator(
    std::unique_ptr<proof_trace_buffer> buffer, kore_header const &header)
    : buffer_(std::move(buffer))
    , parser_(false, false, header) {
  if (!proof_trace_parser::parse_header(*buffer_, version_)) {
    throw std::runtime_error("invalid header");
  }
}

std::optional<annotated_llvm_event>
llvm_rewrite_trace_iterator::get_next_event() {
  if (buffer_->eof()) {
    return std::nullopt;
  }
  switch (type_) {
  case llvm_event_type::PreTrace: {
    if (buffer_->has_word() && buffer_->peek_word() != config_sentinel) {
      llvm_event event;
      if (!parser_.parse_event(*buffer_, event)) {
        throw std::runtime_error("could not parse pre-trace event");
      }
      return {{type_, event}};
    }
    uint64_t pattern_len = 0;
    auto config = parser_.parse_config(*buffer_, pattern_len);
    if (!config) {
      throw std::runtime_error("could not parse config event");
    }
    llvm_event config_event;
    config_event.setkore_pattern(config, pattern_len);
    type_ = llvm_event_type::Trace;
    return {{llvm_event_type::InitialConfig, config_event}};
  }
  case llvm_event_type::Trace: {
    llvm_event event;
    if (!parser_.parse_event(*buffer_, event)) {
      throw std::runtime_error("could not parse trace event");
    }
    return {{type_, event}};
  }
  default: throw std::runtime_error("should be unreachable");
  }
}

void llvm_rewrite_trace_iterator::print(
    std::ostream &out, bool expand_terms, unsigned ind) {
  std::string indent(ind * indent_size, ' ');
  out << fmt::format("{}version: {}\n", indent, version_);
  while (auto event = get_next_event()) {
    event.value().event.print(out, expand_terms, false, ind);
  }
}

void llvm_rewrite_trace::print(
    std::ostream &out, bool expand_terms, unsigned ind) const {
  std::string indent(ind * indent_size, ' ');
  out << fmt::format("{}version: {}\n", indent, version_);
  for (auto const &pre_trace_event : pre_trace_) {
    pre_trace_event.print(out, expand_terms, false, ind);
  }
  initial_config_.print(out, expand_terms, false, ind);
  for (auto const &trace_event : trace_) {
    trace_event.print(out, expand_terms, false, ind);
  }
}

proof_trace_parser::proof_trace_parser(
    bool verbose, bool expand_terms, kore_header const &header)
    : verbose_(verbose)
    , expand_terms_(expand_terms)
    , header_(header) { }

std::optional<llvm_rewrite_trace>
proof_trace_parser::parse_proof_trace(std::string const &data) {
  proof_trace_memory_buffer buffer(data.data(), data.data() + data.length());
  llvm_rewrite_trace trace;
  bool result = parse_trace(buffer, trace);

  if (!result || !buffer.eof()) {
    return std::nullopt;
  }

  if (verbose_) {
    trace.print(std::cout, expand_terms_);
  }

  return trace;
}

std::optional<llvm_rewrite_trace>
proof_trace_parser::parse_proof_trace_from_file(std::string const &filename) {
  std::ifstream file(filename, std::ios_base::binary);
  proof_trace_file_buffer buffer(std::move(file));
  llvm_rewrite_trace trace;
  bool result = parse_trace(buffer, trace);

  if (!result || !buffer.eof()) {
    return std::nullopt;
  }

  if (verbose_) {
    trace.print(std::cout, expand_terms_);
  }

  return trace;
}

} // namespace kllvm
