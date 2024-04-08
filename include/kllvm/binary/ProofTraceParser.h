#ifndef PROOF_TRACE_PARSER_H
#define PROOF_TRACE_PARSER_H

#include <kllvm/ast/AST.h>
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
constexpr uint64_t kore_end_sentinel = detail::word(0xCC);
constexpr uint64_t function_event_sentinel = detail::word(0xDD);
constexpr uint64_t function_end_sentinel = detail::word(0x11);
constexpr uint64_t hook_event_sentinel = detail::word(0xAA);
constexpr uint64_t hook_result_sentinel = detail::word(0xBB);
constexpr uint64_t rule_event_sentinel = detail::word(0x22);
constexpr uint64_t side_condition_event_sentinel = detail::word(0xEE);
constexpr uint64_t side_condition_end_sentinel = detail::word(0x33);

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

protected:
  void print_substitution(
      std::ostream &out, bool expand_terms, unsigned indent = 0U) const;

public:
  llvm_rewrite_event(uint64_t rule_ordinal)
      : rule_ordinal_(rule_ordinal) { }

  [[nodiscard]] uint64_t get_rule_ordinal() const { return rule_ordinal_; }
  [[nodiscard]] substitution_t const &get_substitution() const {
    return substitution_;
  }

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

public:
  static sptr<llvm_rule_event> create(uint64_t rule_ordinal) {
    return sptr<llvm_rule_event>(new llvm_rule_event(rule_ordinal));
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
  sptr<kore_pattern> kore_pattern_{};
  bool result_{false};
  uint64_t pattern_length_{0U};

  llvm_side_condition_end_event(uint64_t rule_ordinal)
      : rule_ordinal_(rule_ordinal)
      , kore_pattern_(nullptr) { }

public:
  static sptr<llvm_side_condition_end_event> create(uint64_t rule_ordinal) {
    return sptr<llvm_side_condition_end_event>(
        new llvm_side_condition_end_event(rule_ordinal));
  }

  [[nodiscard]] uint64_t get_rule_ordinal() const { return rule_ordinal_; }
  [[nodiscard]] sptr<kore_pattern> getkore_pattern() const {
    return kore_pattern_;
  }
  [[nodiscard]] uint64_t get_pattern_length() const { return pattern_length_; }
  void
  setkore_pattern(sptr<kore_pattern> kore_pattern, uint64_t pattern_length) {
    kore_pattern_ = std::move(kore_pattern);
    pattern_length_ = pattern_length;
  }

  [[nodiscard]] bool get_result() const { return result_; }
  void set_result(bool result) { result_ = result; }

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
  std::string relative_position_;
  std::vector<llvm_event> arguments_;
  sptr<kore_pattern> kore_pattern_;
  uint64_t pattern_length_{0U};

  llvm_hook_event(std::string name, std::string relative_position);

public:
  static sptr<llvm_hook_event>
  create(std::string const &name, std::string const &relative_position) {
    return sptr<llvm_hook_event>(new llvm_hook_event(name, relative_position));
  }

  [[nodiscard]] std::string const &get_name() const { return name_; }
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
  static constexpr uint32_t expected_version = 7U;

private:
  bool verbose_;
  bool expand_terms_;

  // Caller needs to check that there are at least 8 bytes remaining in the
  // stream before peeking
  template <typename It>
  uint64_t peek_word(It const &it) {
    return detail::from_bytes<uint64_t>(reinterpret_cast<char const *>(&*it));
  }

  template <typename It>
  bool check_word(It &ptr, It end, uint64_t w) {
    if (std::distance(ptr, end) < sizeof(uint64_t)) {
      return false;
    }

    return detail::read<uint64_t>(ptr, end) == w;
  }

  template <typename It>
  bool read_uint32(It &ptr, It end, uint32_t &i) {
    if (std::distance(ptr, end) < sizeof(uint32_t)) {
      return false;
    }

    i = detail::read<uint32_t>(ptr, end);
    return true;
  }

  template <typename It>
  bool read_uint64(It &ptr, It end, uint64_t &i) {
    if (std::distance(ptr, end) < sizeof(uint64_t)) {
      return false;
    }

    i = detail::read<uint64_t>(ptr, end);
    return true;
  }

  template <typename It>
  bool read_string(It &ptr, It end, std::string &str) {
    while (ptr != end) {
      char c = detail::read<char>(ptr, end);
      if (c == '\0') {
        return true;
      }
      str.push_back(c);
    }

    return false;
  }

  template <typename It>
  bool parse_ordinal(It &ptr, It end, uint64_t &ordinal) {
    return read_uint64(ptr, end, ordinal);
  }

  template <typename It>
  bool parse_arity(It &ptr, It end, uint64_t &arity) {
    return read_uint64(ptr, end, arity);
  }

  template <typename It>
  bool parse_bool(It &ptr, It end, bool &b) {
    if (std::distance(ptr, end) < 1U) {
      return false;
    }

    b = detail::read<bool>(ptr, end);
    return true;
  }

  template <typename It>
  sptr<kore_pattern> parse_kore_term(It &ptr, It end, uint64_t &pattern_len) {
    if (std::distance(ptr, end) < 11U) {
      return nullptr;
    }
    if (detail::read<char>(ptr, end) != '\x7F'
        || detail::read<char>(ptr, end) != 'K'
        || detail::read<char>(ptr, end) != 'O'
        || detail::read<char>(ptr, end) != 'R'
        || detail::read<char>(ptr, end) != 'E') {
      return nullptr;
    }
    auto version = detail::read_version(ptr, end);

    if (!read_uint64(ptr, end, pattern_len)) {
      return nullptr;
    }

    if (std::distance(ptr, end) < pattern_len) {
      return nullptr;
    }
    if (pattern_len > 0 && std::distance(ptr, end) > pattern_len) {
      end = std::next(ptr, pattern_len);
    }

    return detail::read(ptr, end, version);
  }

  template <typename It>
  bool parse_name(It &ptr, It end, std::string &name) {
    return read_string(ptr, end, name);
  }

  template <typename It>
  bool parse_location(It &ptr, It end, std::string &location) {
    return read_string(ptr, end, location);
  }

  template <typename It>
  bool parse_header(It &ptr, It end, uint32_t &version) {
    if (std::distance(ptr, end) < 4U) {
      return false;
    }
    if (detail::read<char>(ptr, end) != 'H'
        || detail::read<char>(ptr, end) != 'I'
        || detail::read<char>(ptr, end) != 'N'
        || detail::read<char>(ptr, end) != 'T') {
      return false;
    }

    if (!read_uint32(ptr, end, version)) {
      return false;
    }

    if (version != expected_version) {
      return false;
    }

    return true;
  }

  template <typename It>
  bool parse_variable(It &ptr, It end, sptr<llvm_rewrite_event> const &event) {
    std::string name;
    if (!parse_name(ptr, end, name)) {
      return false;
    }

    uint64_t pattern_len = 0;
    auto kore_term = parse_kore_term(ptr, end, pattern_len);
    if (!kore_term) {
      return false;
    }

    event->add_substitution(name, kore_term, pattern_len);

    return check_word(ptr, end, kore_end_sentinel);
  }

  template <typename It>
  sptr<llvm_hook_event> parse_hook(It &ptr, It end) {
    if (!check_word(ptr, end, hook_event_sentinel)) {
      return nullptr;
    }

    std::string name;
    if (!parse_name(ptr, end, name)) {
      return nullptr;
    }

    std::string location;
    if (!parse_location(ptr, end, location)) {
      return nullptr;
    }

    auto event = llvm_hook_event::create(name, location);

    while (std::distance(ptr, end) < 8U
           || peek_word(ptr) != hook_result_sentinel) {
      llvm_event argument;
      if (!parse_argument(ptr, end, argument)) {
        return nullptr;
      }
      event->add_argument(argument);
    }

    if (!check_word(ptr, end, hook_result_sentinel)) {
      return nullptr;
    }

    uint64_t pattern_len = 0;
    auto kore_term = parse_kore_term(ptr, end, pattern_len);
    if (!kore_term) {
      return nullptr;
    }
    event->setkore_pattern(kore_term, pattern_len);

    return event;
  }

  template <typename It>
  sptr<llvm_function_event> parse_function(It &ptr, It end) {
    if (!check_word(ptr, end, function_event_sentinel)) {
      return nullptr;
    }

    std::string name;
    if (!parse_name(ptr, end, name)) {
      return nullptr;
    }

    std::string location;
    if (!parse_location(ptr, end, location)) {
      return nullptr;
    }

    auto event = llvm_function_event::create(name, location);

    while (std::distance(ptr, end) < 8U
           || peek_word(ptr) != function_end_sentinel) {
      llvm_event argument;
      if (!parse_argument(ptr, end, argument)) {
        return nullptr;
      }
      event->add_argument(argument);
    }

    if (!check_word(ptr, end, function_end_sentinel)) {
      return nullptr;
    }

    return event;
  }

  template <typename It>
  sptr<kore_pattern> parse_config(It &ptr, It end, uint64_t &pattern_len) {
    if (!check_word(ptr, end, config_sentinel)) {
      return nullptr;
    }

    auto kore_term = parse_kore_term(ptr, end, pattern_len);
    if (!kore_term) {
      return nullptr;
    }

    if (!check_word(ptr, end, kore_end_sentinel)) {
      return nullptr;
    }

    return kore_term;
  }

  template <typename It>
  sptr<llvm_rule_event> parse_rule(It &ptr, It end) {
    if (!check_word(ptr, end, rule_event_sentinel)) {
      return nullptr;
    }

    uint64_t ordinal = 0;
    if (!parse_ordinal(ptr, end, ordinal)) {
      return nullptr;
    }

    uint64_t arity = 0;
    if (!parse_arity(ptr, end, arity)) {
      return nullptr;
    }

    auto event = llvm_rule_event::create(ordinal);

    for (auto i = 0; i < arity; i++) {
      if (!parse_variable(ptr, end, event)) {
        return nullptr;
      }
    }

    return event;
  }

  template <typename It>
  sptr<llvm_side_condition_event> parse_side_condition(It &ptr, It end) {
    if (!check_word(ptr, end, side_condition_event_sentinel)) {
      return nullptr;
    }

    uint64_t ordinal = 0;
    if (!parse_ordinal(ptr, end, ordinal)) {
      return nullptr;
    }

    uint64_t arity = 0;
    if (!parse_arity(ptr, end, arity)) {
      return nullptr;
    }

    auto event = llvm_side_condition_event::create(ordinal);

    for (auto i = 0; i < arity; i++) {
      if (!parse_variable(ptr, end, event)) {
        return nullptr;
      }
    }

    return event;
  }

  template <typename It>
  sptr<llvm_side_condition_end_event>
  parse_side_condition_end(It &ptr, It end) {
    if (!check_word(ptr, end, side_condition_end_sentinel)) {
      return nullptr;
    }

    uint64_t ordinal = 0;
    if (!parse_ordinal(ptr, end, ordinal)) {
      return nullptr;
    }

    auto event = llvm_side_condition_end_event::create(ordinal);

    bool side_condition_result;
    auto result = parse_bool(ptr, end, side_condition_result);
    if (!result) {
      return nullptr;
    }

    event->set_result(side_condition_result);

    return event;
  }

  template <typename It>
  bool parse_argument(It &ptr, It end, llvm_event &event) {
    if (std::distance(ptr, end) >= 1U && detail::peek(ptr) == '\x7F') {
      uint64_t pattern_len = 0;
      auto kore_term = parse_kore_term(ptr, end, pattern_len);
      if (!kore_term) {
        return false;
      }
      event.setkore_pattern(kore_term, pattern_len);

      return true;
    }

    if (std::distance(ptr, end) < 8U) {
      return false;
    }

    switch (peek_word(ptr)) {

    case hook_event_sentinel: {
      auto hook_event = parse_hook(ptr, end);
      if (!hook_event) {
        return false;
      }
      event.set_step_event(hook_event);
      return true;
    }

    case function_event_sentinel: {
      auto function_event = parse_function(ptr, end);
      if (!function_event) {
        return false;
      }
      event.set_step_event(function_event);
      return true;
    }

    case rule_event_sentinel: {
      auto rule_event = parse_rule(ptr, end);
      if (!rule_event) {
        return false;
      }
      event.set_step_event(rule_event);
      return true;
    }

    default: return false;
    }
  }

  template <typename It>
  sptr<llvm_step_event> parse_step_event(It &ptr, It end) {
    if (std::distance(ptr, end) < 8U) {
      return nullptr;
    }

    switch (peek_word(ptr)) {

    case hook_event_sentinel: return parse_hook(ptr, end);

    case function_event_sentinel: return parse_function(ptr, end);

    case rule_event_sentinel: return parse_rule(ptr, end);

    case side_condition_event_sentinel: return parse_side_condition(ptr, end);

    case side_condition_end_sentinel: return parse_side_condition_end(ptr, end);

    default: return nullptr;
    }
  }

  template <typename It>
  bool parse_event(It &ptr, It end, llvm_event &event) {
    if (std::distance(ptr, end) < 8U) {
      return false;
    }

    if (peek_word(ptr) == config_sentinel) {
      uint64_t pattern_len = 0;
      auto config = parse_config(ptr, end, pattern_len);
      if (!config) {
        return false;
      }
      event.setkore_pattern(config, pattern_len);
    } else {
      auto step_event = parse_step_event(ptr, end);
      if (!step_event) {
        return false;
      }
      event.set_step_event(step_event);
    }

    return true;
  }

  template <typename It>
  bool parse_trace(It &ptr, It end, llvm_rewrite_trace &trace) {
    uint32_t version = 0;
    if (!parse_header(ptr, end, version)) {
      return false;
    }
    trace.set_version(version);

    while (std::distance(ptr, end) >= 8U && peek_word(ptr) != config_sentinel) {
      llvm_event event;
      if (!parse_event(ptr, end, event)) {
        return false;
      }
      trace.add_pre_trace_event(event);
    }

    uint64_t pattern_len = 0;
    auto config = parse_config(ptr, end, pattern_len);
    if (!config) {
      return false;
    }
    llvm_event config_event;
    config_event.setkore_pattern(config, pattern_len);
    trace.set_initial_config(config_event);

    while (ptr != end) {
      llvm_event event;
      if (!parse_event(ptr, end, event)) {
        return false;
      }
      trace.add_trace_event(event);
    }

    return true;
  }

public:
  proof_trace_parser(bool verbose, bool expand_terms);

  std::optional<llvm_rewrite_trace>
  parse_proof_trace_from_file(std::string const &filename);
  std::optional<llvm_rewrite_trace> parse_proof_trace(std::string const &data);
};

} // namespace kllvm

#endif
