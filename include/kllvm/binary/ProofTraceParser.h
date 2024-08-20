#ifndef PROOF_TRACE_PARSER_H
#define PROOF_TRACE_PARSER_H

#include <kllvm/ast/AST.h>
#include <kllvm/ast/util.h>
#include <kllvm/binary/deserializer.h>

#include <iostream>
#include <string>
#include <utility>

namespace kllvm {

namespace detail {

constexpr uint64_t word(uint8_t byte) {
  auto ret = uint64_t{0};
  for (auto i = 0U; i < sizeof(ret); ++i) {
    ret <<= 8;
    ret |= byte;
  }
  return ret;
}

static_assert(word(0xAA) == 0xAAAAAAAAAAAAAAAA);

} // namespace detail

constexpr uint64_t config_sentinel = detail::word(0xFF);
constexpr uint64_t function_event_sentinel = detail::word(0xDD);
constexpr uint64_t function_end_sentinel = detail::word(0x11);
constexpr uint64_t hook_event_sentinel = detail::word(0xAA);
constexpr uint64_t hook_result_sentinel = detail::word(0xBB);
constexpr uint64_t rule_event_sentinel = detail::word(0x22);
constexpr uint64_t side_condition_event_sentinel = detail::word(0xEE);
constexpr uint64_t side_condition_end_sentinel = detail::word(0x33);
constexpr uint64_t pattern_matching_failure_sentinel = detail::word(0x44);

class llvm_step_event : public std::enable_shared_from_this<llvm_step_event> {
public:
  virtual void
  print(std::ostream &out, bool expand_terms, unsigned indent = 0U) const
      = 0;
  virtual ~llvm_step_event() = default;
};

class llvm_rewrite_event : public llvm_step_event {
public:
  using substitution_t
      = std::map<std::string, std::pair<sptr<kore_pattern>, uint64_t>>;

private:
  uint64_t rule_ordinal_;
  substitution_t substitution_{};
  std::string label_;
  std::string location_;
  bool debug_{};

protected:
  void print_substitution(
      std::ostream &out, bool expand_terms, unsigned indent = 0U) const;

public:
  llvm_rewrite_event(uint64_t rule_ordinal)
      : rule_ordinal_(rule_ordinal) { }
  llvm_rewrite_event(
      uint64_t rule_ordinal, std::string label, std::string location)
      : rule_ordinal_(rule_ordinal)
      , label_(std::move(label))
      , location_(std::move(location))
      , debug_(true) { }

  [[nodiscard]] std::string const &get_label() const { return label_; }
  [[nodiscard]] std::string const &get_location() const { return location_; }
  [[nodiscard]] uint64_t get_rule_ordinal() const { return rule_ordinal_; }
  [[nodiscard]] substitution_t const &get_substitution() const {
    return substitution_;
  }
  [[nodiscard]] bool print_debug_info() const { return debug_; }

  void add_substitution(
      std::string const &name, sptr<kore_pattern> const &term,
      uint64_t pattern_len) {
    substitution_.insert(
        std::make_pair(name, std::make_pair(term, pattern_len)));
  }

  ~llvm_rewrite_event() override = default;
};

class llvm_rule_event : public llvm_rewrite_event {
private:
  llvm_rule_event(uint64_t rule_ordinal)
      : llvm_rewrite_event(rule_ordinal) { }
  llvm_rule_event(
      uint64_t rule_ordinal, std::string label, std::string location)
      : llvm_rewrite_event(
          rule_ordinal, std::move(label), std::move(location)) { }

public:
  static sptr<llvm_rule_event> create(uint64_t rule_ordinal) {
    return sptr<llvm_rule_event>(new llvm_rule_event(rule_ordinal));
  }

  static sptr<llvm_rule_event>
  create(uint64_t rule_ordinal, std::string label, std::string location) {
    return sptr<llvm_rule_event>(new llvm_rule_event(
        rule_ordinal, std::move(label), std::move(location)));
  }

  void print(std::ostream &out, bool expand_terms, unsigned indent = 0U)
      const override;
};

class llvm_side_condition_event : public llvm_rewrite_event {
private:
  llvm_side_condition_event(uint64_t rule_ordinal)
      : llvm_rewrite_event(rule_ordinal) { }

public:
  static sptr<llvm_side_condition_event> create(uint64_t rule_ordinal) {
    return sptr<llvm_side_condition_event>(
        new llvm_side_condition_event(rule_ordinal));
  }

  void print(std::ostream &out, bool expand_terms, unsigned indent = 0U)
      const override;
};

class llvm_side_condition_end_event : public llvm_step_event {
private:
  uint64_t rule_ordinal_;
  bool result_;

  llvm_side_condition_end_event(uint64_t rule_ordinal, bool result)
      : rule_ordinal_(rule_ordinal)
      , result_(result) { }

public:
  static sptr<llvm_side_condition_end_event>
  create(uint64_t rule_ordinal, bool result) {
    return sptr<llvm_side_condition_end_event>(
        new llvm_side_condition_end_event(rule_ordinal, result));
  }

  [[nodiscard]] uint64_t get_rule_ordinal() const { return rule_ordinal_; }
  [[nodiscard]] bool get_result() const { return result_; }

  void print(std::ostream &out, bool expand_terms, unsigned indent = 0U)
      const override;
};

class llvm_pattern_matching_failure_event : public llvm_step_event {
private:
  std::string function_name_;

  llvm_pattern_matching_failure_event(std::string function_name)
      : function_name_(std::move(function_name)) { }

public:
  static sptr<llvm_pattern_matching_failure_event>
  create(std::string function_name) {
    return sptr<llvm_pattern_matching_failure_event>(
        new llvm_pattern_matching_failure_event(std::move(function_name)));
  }

  [[nodiscard]] std::string const &get_function_name() const {
    return function_name_;
  }

  void print(std::ostream &out, bool expand_terms, unsigned indent = 0U)
      const override;
};

class llvm_event;

class llvm_function_event : public llvm_step_event {
private:
  std::string name_;
  std::string relative_position_;
  std::vector<llvm_event> arguments_;

  llvm_function_event(std::string name, std::string relative_position);

public:
  static sptr<llvm_function_event>
  create(std::string const &name, std::string const &relative_position) {
    return sptr<llvm_function_event>(
        new llvm_function_event(name, relative_position));
  }

  [[nodiscard]] std::string const &get_name() const { return name_; }
  [[nodiscard]] std::string const &get_relative_position() const {
    return relative_position_;
  }
  [[nodiscard]] std::vector<llvm_event> const &get_arguments() const;

  void add_argument(llvm_event const &argument);

  void print(std::ostream &out, bool expand_terms, unsigned indent = 0U)
      const override;
};

class llvm_hook_event : public llvm_step_event {
private:
  std::string name_;
  std::string symbol_name_;
  std::string relative_position_;
  std::vector<llvm_event> arguments_;
  sptr<kore_pattern> kore_pattern_;
  uint64_t pattern_length_{0U};

  llvm_hook_event(
      std::string name, std::string symbol_name, std::string relative_position);

public:
  static sptr<llvm_hook_event> create(
      std::string const &name, std::string const &symbol_name,
      std::string const &relative_position) {
    return sptr<llvm_hook_event>(
        new llvm_hook_event(name, symbol_name, relative_position));
  }

  [[nodiscard]] std::string const &get_name() const { return name_; }
  [[nodiscard]] std::string const &get_symbol_name() const {
    return symbol_name_;
  }
  [[nodiscard]] std::string const &get_relative_position() const {
    return relative_position_;
  }
  [[nodiscard]] std::vector<llvm_event> const &get_arguments() const {
    return arguments_;
  }
  [[nodiscard]] sptr<kore_pattern> getkore_pattern() const {
    return kore_pattern_;
  }
  [[nodiscard]] uint64_t get_pattern_length() const { return pattern_length_; }
  void
  setkore_pattern(sptr<kore_pattern> kore_pattern, uint64_t pattern_length) {
    kore_pattern_ = std::move(kore_pattern);
    pattern_length_ = pattern_length;
  }

  void add_argument(llvm_event const &argument);

  void print(std::ostream &out, bool expand_terms, unsigned indent = 0U)
      const override;
};

class llvm_event {
private:
  bool is_step_event_{};
  sptr<llvm_step_event> step_event_{};
  sptr<kore_pattern> kore_pattern_{};
  uint64_t pattern_length_{};

public:
  [[nodiscard]] bool is_step() const { return is_step_event_; }
  [[nodiscard]] bool is_pattern() const { return !is_step(); }
  [[nodiscard]] sptr<llvm_step_event> get_step_event() const {
    return step_event_;
  }
  [[nodiscard]] sptr<kore_pattern> getkore_pattern() const {
    return kore_pattern_;
  }
  [[nodiscard]] uint64_t get_pattern_length() const { return pattern_length_; }
  void set_step_event(sptr<llvm_step_event> step_event) {
    is_step_event_ = true;
    step_event_ = std::move(step_event);
  }
  void
  setkore_pattern(sptr<kore_pattern> kore_pattern, uint64_t pattern_length) {
    is_step_event_ = false;
    kore_pattern_ = std::move(kore_pattern);
    pattern_length_ = pattern_length;
  }
  void print(
      std::ostream &out, bool expand_terms, bool is_arg,
      unsigned indent = 0U) const;
};

enum class llvm_event_type { PreTrace, InitialConfig, Trace };

struct annotated_llvm_event {
  llvm_event_type type{};
  llvm_event event;
};

class llvm_rewrite_trace {
private:
  uint32_t version_{};
  std::vector<llvm_event> pre_trace_{};
  llvm_event initial_config_;
  std::vector<llvm_event> trace_{};

public:
  [[nodiscard]] uint32_t get_version() const { return version_; }
  [[nodiscard]] std::vector<llvm_event> const &get_pre_trace() const {
    return pre_trace_;
  }
  [[nodiscard]] llvm_event get_initial_config() const {
    return initial_config_;
  }
  [[nodiscard]] std::vector<llvm_event> const &get_trace() const {
    return trace_;
  }
  void set_version(uint32_t v) { version_ = v; }
  void set_initial_config(llvm_event initial_config) {
    initial_config_ = std::move(initial_config);
  }

  void add_pre_trace_event(llvm_event const &event) {
    pre_trace_.push_back(event);
  }
  void add_trace_event(llvm_event const &event) { trace_.push_back(event); }

  void print(std::ostream &out, bool expand_terms, unsigned indent = 0U) const;
};

class proof_trace_parser {
public:
  static constexpr uint32_t expected_version = 12U;

private:
  bool verbose_;
  bool expand_terms_;
  [[maybe_unused]] kore_header const &header_;
  [[maybe_unused]] std::optional<kore_definition> kore_definition_
      = std::nullopt;

  sptr<kore_pattern>
  parse_kore_term(proof_trace_buffer &buffer, uint64_t &pattern_len) {
    std::array<char, 4> magic{};
    if (!buffer.read(magic.data(), sizeof(magic))) {
      return nullptr;
    }
    if (magic[0] != '\x7F' || magic[1] != 'K' || magic[2] != 'R'
        || magic[3] != '2') {
      return nullptr;
    }
    auto result = detail::read_v2(buffer, header_, pattern_len);
    pattern_len += 4;
    return result;
  }

  static bool parse_header(proof_trace_buffer &buffer, uint32_t &version) {
    std::array<char, 4> magic{};
    if (!buffer.read(magic.data(), sizeof(magic))) {
      return false;
    }
    if (magic[0] != 'H' || magic[1] != 'I' || magic[2] != 'N'
        || magic[3] != 'T') {
      return false;
    }

    if (!buffer.read_uint32(version)) {
      return false;
    }

    if (version != expected_version) {
      return false;
    }

    return true;
  }

  bool parse_variable(
      proof_trace_buffer &buffer, sptr<llvm_rewrite_event> const &event) {
    std::string name;
    if (!buffer.read_string(name)) {
      return false;
    }

    uint64_t pattern_len = 0;
    auto kore_term = parse_kore_term(buffer, pattern_len);
    if (!kore_term) {
      return false;
    }

    event->add_substitution(name, kore_term, pattern_len);

    return true;
  }

  sptr<llvm_hook_event> parse_hook(proof_trace_buffer &buffer) {
    if (!buffer.check_word(hook_event_sentinel)) {
      return nullptr;
    }

    std::string name;
    if (!buffer.read_string(name)) {
      return nullptr;
    }

    std::string symbol_name;
    if (!buffer.read_string(symbol_name)) {
      return nullptr;
    }

    std::string location;
    if (!buffer.read_string(location)) {
      return nullptr;
    }

    auto event = llvm_hook_event::create(name, symbol_name, location);

    while (!buffer.has_word() || buffer.peek_word() != hook_result_sentinel) {
      llvm_event argument;
      if (!parse_argument(buffer, argument)) {
        return nullptr;
      }
      event->add_argument(argument);
    }

    if (!buffer.check_word(hook_result_sentinel)) {
      return nullptr;
    }

    uint64_t pattern_len = 0;
    auto kore_term = parse_kore_term(buffer, pattern_len);
    if (!kore_term) {
      return nullptr;
    }
    event->setkore_pattern(kore_term, pattern_len);

    return event;
  }

  sptr<llvm_function_event> parse_function(proof_trace_buffer &buffer) {
    if (!buffer.check_word(function_event_sentinel)) {
      return nullptr;
    }

    std::string name;
    if (!buffer.read_string(name)) {
      return nullptr;
    }

    std::string location;
    if (!buffer.read_string(location)) {
      return nullptr;
    }

    auto event = llvm_function_event::create(name, location);

    while (!buffer.has_word() || buffer.peek_word() != function_end_sentinel) {
      llvm_event argument;
      if (!parse_argument(buffer, argument)) {
        return nullptr;
      }
      event->add_argument(argument);
    }

    if (!buffer.check_word(function_end_sentinel)) {
      return nullptr;
    }

    return event;
  }

  sptr<kore_pattern>
  parse_config(proof_trace_buffer &buffer, uint64_t &pattern_len) {
    if (!buffer.check_word(config_sentinel)) {
      return nullptr;
    }

    auto kore_term = parse_kore_term(buffer, pattern_len);
    if (!kore_term) {
      return nullptr;
    }

    return kore_term;
  }

  sptr<llvm_rule_event> parse_rule(proof_trace_buffer &buffer) {
    if (!buffer.check_word(rule_event_sentinel)) {
      return nullptr;
    }

    uint64_t ordinal = 0;
    if (!buffer.read_uint64(ordinal)) {
      return nullptr;
    }

    uint64_t arity = 0;
    if (!buffer.read_uint64(arity)) {
      return nullptr;
    }

    kllvm::sptr<llvm_rule_event> event;

    if (kore_definition_) {
      auto axiom = kore_definition_->get_axiom_by_ordinal(ordinal);
      auto axiom_att = axiom.attributes();

      std::string label;
      if (axiom_att.contains(kllvm::attribute_set::key::Label)) {
        label = axiom_att.get_string(kllvm::attribute_set::key::Label);
      }

      std::string location;
      auto loc = kllvm::get_start_line_location(axiom);
      if (loc.has_value()) {
        location = loc.value().first + ":" + std::to_string(loc.value().second);
      }

      event = llvm_rule_event::create(ordinal, label, location);

    } else {
      event = llvm_rule_event::create(ordinal);
    }
    for (auto i = 0; i < arity; i++) {
      if (!parse_variable(buffer, event)) {
        return nullptr;
      }
    }

    return event;
  }

  sptr<llvm_side_condition_event>
  parse_side_condition(proof_trace_buffer &buffer) {
    if (!buffer.check_word(side_condition_event_sentinel)) {
      return nullptr;
    }

    uint64_t ordinal = 0;
    if (!buffer.read_uint64(ordinal)) {
      return nullptr;
    }

    uint64_t arity = 0;
    if (!buffer.read_uint64(arity)) {
      return nullptr;
    }

    auto event = llvm_side_condition_event::create(ordinal);

    for (auto i = 0; i < arity; i++) {
      if (!parse_variable(buffer, event)) {
        return nullptr;
      }
    }

    return event;
  }

  sptr<llvm_side_condition_end_event> static parse_side_condition_end(
      proof_trace_buffer &buffer) {
    if (!buffer.check_word(side_condition_end_sentinel)) {
      return nullptr;
    }

    uint64_t ordinal = 0;
    if (!buffer.read_uint64(ordinal)) {
      return nullptr;
    }

    bool side_condition_result = false;
    auto result = buffer.read_bool(side_condition_result);
    if (!result) {
      return nullptr;
    }

    auto event
        = llvm_side_condition_end_event::create(ordinal, side_condition_result);

    return event;
  }

  sptr<llvm_pattern_matching_failure_event> static parse_pattern_matching_failure(
      proof_trace_buffer &buffer) {
    if (!buffer.check_word(pattern_matching_failure_sentinel)) {
      return nullptr;
    }

    std::string function_name;
    if (!buffer.read_string(function_name)) {
      return nullptr;
    }

    auto event = llvm_pattern_matching_failure_event::create(function_name);

    return event;
  }

  bool parse_argument(proof_trace_buffer &buffer, llvm_event &event) {
    if (!buffer.eof() && buffer.peek() == '\x7F') {
      uint64_t pattern_len = 0;
      auto kore_term = parse_kore_term(buffer, pattern_len);
      if (!kore_term) {
        return false;
      }
      event.setkore_pattern(kore_term, pattern_len);

      return true;
    }

    if (!buffer.has_word()) {
      return false;
    }

    switch (buffer.peek_word()) {

    case hook_event_sentinel: {
      auto hook_event = parse_hook(buffer);
      if (!hook_event) {
        return false;
      }
      event.set_step_event(hook_event);
      return true;
    }

    case function_event_sentinel: {
      auto function_event = parse_function(buffer);
      if (!function_event) {
        return false;
      }
      event.set_step_event(function_event);
      return true;
    }

    case rule_event_sentinel: {
      auto rule_event = parse_rule(buffer);
      if (!rule_event) {
        return false;
      }
      event.set_step_event(rule_event);
      return true;
    }

    case side_condition_event_sentinel: {
      auto side_condition_event = parse_side_condition(buffer);
      if (!side_condition_event) {
        return false;
      }
      event.set_step_event(side_condition_event);
      return true;
    }

    case side_condition_end_sentinel: {
      auto side_condition_end_event = parse_side_condition_end(buffer);
      if (!side_condition_end_event) {
        return false;
      }
      event.set_step_event(side_condition_end_event);
      return true;
    }

    case pattern_matching_failure_sentinel: {
      auto pattern_matching_failure_event
          = parse_pattern_matching_failure(buffer);
      if (!pattern_matching_failure_event) {
        return false;
      }
      event.set_step_event(pattern_matching_failure_event);
      return true;
    }

    default: return false;
    }
  }

  sptr<llvm_step_event> parse_step_event(proof_trace_buffer &buffer) {
    if (!buffer.has_word()) {
      return nullptr;
    }

    switch (buffer.peek_word()) {

    case hook_event_sentinel: return parse_hook(buffer);

    case function_event_sentinel: return parse_function(buffer);

    case rule_event_sentinel: return parse_rule(buffer);

    case side_condition_event_sentinel: return parse_side_condition(buffer);

    case side_condition_end_sentinel: return parse_side_condition_end(buffer);

    case pattern_matching_failure_sentinel:
      return parse_pattern_matching_failure(buffer);

    default: return nullptr;
    }
  }

  bool parse_event(proof_trace_buffer &buffer, llvm_event &event) {
    if (!buffer.has_word()) {
      return false;
    }

    if (buffer.peek_word() == config_sentinel) {
      uint64_t pattern_len = 0;
      auto config = parse_config(buffer, pattern_len);
      if (!config) {
        return false;
      }
      event.setkore_pattern(config, pattern_len);
    } else {
      auto step_event = parse_step_event(buffer);
      if (!step_event) {
        return false;
      }
      event.set_step_event(step_event);
    }

    return true;
  }

  bool parse_trace(proof_trace_buffer &buffer, llvm_rewrite_trace &trace) {
    uint32_t version = 0;
    if (!parse_header(buffer, version)) {
      return false;
    }
    trace.set_version(version);

    while (buffer.has_word() && buffer.peek_word() != config_sentinel) {
      llvm_event event;
      if (!parse_event(buffer, event)) {
        return false;
      }
      trace.add_pre_trace_event(event);
    }

    uint64_t pattern_len = 0;
    auto config = parse_config(buffer, pattern_len);
    if (!config) {
      return false;
    }
    llvm_event config_event;
    config_event.setkore_pattern(config, pattern_len);
    trace.set_initial_config(config_event);

    while (!buffer.eof()) {
      llvm_event event;
      if (!parse_event(buffer, event)) {
        return false;
      }
      trace.add_trace_event(event);
    }

    return true;
  }

public:
  proof_trace_parser(
      bool verbose, bool expand_terms, kore_header const &header,
      std::optional<kore_definition> kore_definition = std::nullopt);

  std::optional<llvm_rewrite_trace>
  parse_proof_trace_from_file(std::string const &filename);
  std::optional<llvm_rewrite_trace> parse_proof_trace(std::string const &data);

  friend class llvm_rewrite_trace_iterator;
};

class llvm_rewrite_trace_iterator {
private:
  uint32_t version_{};
  std::unique_ptr<proof_trace_buffer> buffer_;
  llvm_event_type type_ = llvm_event_type::PreTrace;
  proof_trace_parser parser_;

public:
  llvm_rewrite_trace_iterator(
      std::unique_ptr<proof_trace_buffer> buffer, kore_header const &header);
  [[nodiscard]] uint32_t get_version() const { return version_; }
  std::optional<annotated_llvm_event> get_next_event();
  void print(std::ostream &out, bool expand_terms, unsigned indent = 0U);
};

} // namespace kllvm

#endif
