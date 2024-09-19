#include <kllvm/binary/ProofTraceUtils.h>

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
  if (!print_debug_info()) {
    out << fmt::format(
        "{}rule: {} {}\n", indent, get_rule_ordinal(),
        get_substitution().size());
  } else {
    out << fmt::format(
        "{}rule: {} {} [{}] [{}]\n", indent, get_rule_ordinal(),
        get_substitution().size(), get_label(), get_location());
  }
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
  out << fmt::format(
      "{}side condition exit: {} {}\n", indent, rule_ordinal_,
      (result_ ? "true" : "false"));
}

void llvm_pattern_matching_failure_event::print(
    std::ostream &out, bool expand_terms, unsigned ind) const {
  std::string indent(ind * indent_size, ' ');
  out << fmt::format(
      "{}pattern matching failure: {}\n", indent, function_name_);
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
  } else if (kore_pattern_) {
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
  if (!proof_trace_parser::parse_header(*buffer_, kind_, version_)) {
    throw std::runtime_error("invalid header");
  }
  if (kind_ != proof_trace_parser::trace_kind::Hint) {
    throw std::runtime_error("invalid hint file: streaming parser does not "
                             "work with partial traces");
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
    std::ostream &out, bool expand_terms, unsigned ind,
    bool intermediate_configs) {
  std::string indent(ind * indent_size, ' ');
  out << fmt::format("{}version: {}\n", indent, version_);
  while (auto event = get_next_event()) {
    event.value().event.print(out, expand_terms, false, ind);
    if (intermediate_configs) {
      if (event.value().type == llvm_event_type::InitialConfig) {
        current_config_ = std::dynamic_pointer_cast<kore_composite_pattern>(
            event.value().event.getkore_pattern());
      } else if (event.value().type == llvm_event_type::Trace) {
        if (event.value().event.is_pattern()) {
          current_config_ = std::dynamic_pointer_cast<kore_composite_pattern>(
              event.value().event.getkore_pattern());
        } else {
          if (auto function_event
              = std::dynamic_pointer_cast<llvm_function_event>(
                  event.value().event.get_step_event())) {
            auto *new_config_event = build_post_function_event(
                current_config_, function_event, expand_terms);
            if (new_config_event) {
              current_config_
                  = std::dynamic_pointer_cast<kore_composite_pattern>(
                      new_config_event->getkore_pattern());
              new_config_event->print(out, expand_terms, false, ind);
            }
          }
        }
      }
    }
  }
}

void llvm_rewrite_trace::print(
    std::ostream &out, bool expand_terms, unsigned ind,
    bool intermediate_configs) const {
  std::string indent(ind * indent_size, ' ');
  out << fmt::format("{}version: {}\n", indent, version_);
  for (auto const &pre_trace_event : pre_trace_) {
    pre_trace_event.print(out, expand_terms, false, ind);
  }
  initial_config_.print(out, expand_terms, false, ind);
  if (intermediate_configs) {
    auto current_config = std::dynamic_pointer_cast<kore_composite_pattern>(
        initial_config_.getkore_pattern());
    for (auto const &trace_event : trace_) {
      trace_event.print(out, expand_terms, false, ind);
      if (trace_event.is_pattern()) {
        current_config = std::dynamic_pointer_cast<kore_composite_pattern>(
            trace_event.getkore_pattern());
      } else {
        if (auto function_event
            = std::dynamic_pointer_cast<llvm_function_event>(
                trace_event.get_step_event())) {
          auto *new_config_event = build_post_function_event(
              current_config, function_event, expand_terms);
          if (new_config_event) {
            current_config = std::dynamic_pointer_cast<kore_composite_pattern>(
                new_config_event->getkore_pattern());
            new_config_event->print(out, expand_terms, false, ind);
          }
        }
      }
    }
  } else {
    for (auto const &trace_event : trace_) {
      trace_event.print(out, expand_terms, false, ind);
    }
  }
}

proof_trace_parser::proof_trace_parser(
    bool verbose, bool expand_terms, kore_header const &header,
    std::optional<kore_definition> kore_definition)
    : verbose_(verbose)
    , expand_terms_(expand_terms)
    , header_(header)
    , kore_definition_(std::move(kore_definition)) { }

std::optional<llvm_rewrite_trace> proof_trace_parser::parse_proof_trace(
    std::string const &data, bool intermediate_configs) {
  proof_trace_memory_buffer buffer(data.data(), data.data() + data.length());
  llvm_rewrite_trace trace;
  bool result = parse_trace(buffer, trace);

  if (!result || !buffer.eof()) {
    return std::nullopt;
  }

  if (verbose_) {
    trace.print(std::cout, expand_terms_, 0U, intermediate_configs);
  }

  return trace;
}

std::optional<llvm_rewrite_trace>
proof_trace_parser::parse_proof_trace_from_file(
    std::string const &filename, bool intermediate_configs) {
  std::ifstream file(filename, std::ios_base::binary);
  proof_trace_file_buffer buffer(std::move(file));
  llvm_rewrite_trace trace;
  bool result = parse_trace(buffer, trace);

  if (!result || !buffer.eof()) {
    return std::nullopt;
  }

  if (verbose_) {
    trace.print(std::cout, expand_terms_, 0U, intermediate_configs);
  }

  return trace;
}

} // namespace kllvm
