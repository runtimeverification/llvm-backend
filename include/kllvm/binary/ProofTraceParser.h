#ifndef PROOF_TRACE_VALIDATOR_H
#define PROOF_TRACE_VALIDATOR_H

#include <kllvm/ast/AST.h>
#include <kllvm/binary/deserializer.h>

#include <fmt/format.h>

#include <iostream>
#include <string>

namespace kllvm {

namespace detail {

constexpr uint64_t word(uint8_t b) {
  uint64_t w = uint64_t(b);
  for (auto i = 0; i < 7; ++i) {
    w = (w << 8u) | w;
  }
  return w;
}

} // namespace detail

constexpr uint64_t config_sentinel = detail::word(0xFF);
constexpr uint64_t kore_end_sentinel = detail::word(0xCC);
constexpr uint64_t function_event_sentinel = detail::word(0xDD);
constexpr uint64_t function_end_sentinel = detail::word(0x11);
constexpr uint64_t hook_event_sentinel = detail::word(0xAA);
constexpr uint64_t hook_result_sentinel = detail::word(0xBB);
constexpr uint64_t rule_event_sentinel = detail::word(0x22);
constexpr uint64_t side_condition_event_sentinel = detail::word(0xEE);

class LLVMStepEvent : public std::enable_shared_from_this<LLVMStepEvent> {
public:
  virtual void print(std::ostream &Out, unsigned indent = 0u) const = 0;
  virtual ~LLVMStepEvent() = default;
};

class LLVMRewriteEvent : public LLVMStepEvent {
public:
  using substitution_t
      = std::map<std::string, std::pair<sptr<KOREPattern>, uint64_t>>;

protected:
  uint64_t ruleOrdinal;
  substitution_t substitution;

  void printSubstitution(std::ostream &Out, unsigned indent = 0u) const;

public:
  LLVMRewriteEvent(uint64_t _ruleOrdinal)
      : ruleOrdinal(_ruleOrdinal)
      , substitution() { }

  uint64_t getRuleOrdinal() const { return ruleOrdinal; }
  substitution_t const &getSubstitution() const { return substitution; }

  void addSubstitution(
      std::string const &name, sptr<KOREPattern> term, uint64_t pattern_len) {
    substitution.insert(
        std::make_pair(name, std::make_pair(term, pattern_len)));
  }

  virtual ~LLVMRewriteEvent() = default;
};

class LLVMRuleEvent : public LLVMRewriteEvent {
private:
  LLVMRuleEvent(uint64_t _ruleOrdinal)
      : LLVMRewriteEvent(_ruleOrdinal) { }

public:
  static sptr<LLVMRuleEvent> Create(uint64_t _ruleOrdinal) {
    return sptr<LLVMRuleEvent>(new LLVMRuleEvent(_ruleOrdinal));
  }

  virtual void print(std::ostream &Out, unsigned indent = 0u) const override;
};

class LLVMSideConditionEvent : public LLVMRewriteEvent {
private:
  LLVMSideConditionEvent(uint64_t _ruleOrdinal)
      : LLVMRewriteEvent(_ruleOrdinal) { }

public:
  static sptr<LLVMSideConditionEvent> Create(uint64_t _ruleOrdinal) {
    return sptr<LLVMSideConditionEvent>(
        new LLVMSideConditionEvent(_ruleOrdinal));
  }

  virtual void print(std::ostream &Out, unsigned indent = 0u) const override;
};

class LLVMEvent;

class LLVMFunctionEvent : public LLVMStepEvent {
private:
  std::string name;
  std::string relativePosition;
  std::vector<LLVMEvent> arguments;

  LLVMFunctionEvent(
      std::string const &_name, std::string const &_relativePosition)
      : name(_name)
      , relativePosition(_relativePosition)
      , arguments() { }

public:
  static sptr<LLVMFunctionEvent>
  Create(std::string const &_name, std::string const &_relativePosition) {
    return sptr<LLVMFunctionEvent>(
        new LLVMFunctionEvent(_name, _relativePosition));
  }

  std::string const &getName() const { return name; }
  std::string const &getRelativePosition() const { return relativePosition; }
  std::vector<LLVMEvent> const &getArguemnts() const { return arguments; }

  void addArgument(LLVMEvent const &argument) { arguments.push_back(argument); }

  virtual void print(std::ostream &Out, unsigned indent = 0u) const override;
};

class LLVMHookEvent : public LLVMStepEvent {
private:
  std::string name;
  std::string relativePosition;
  std::vector<LLVMEvent> arguments;
  sptr<KOREPattern> korePattern;
  uint64_t patternLength;

  LLVMHookEvent(std::string const &_name, std::string const &_relativePosition)
      : name(_name)
      , relativePosition(_relativePosition)
      , arguments()
      , korePattern(nullptr)
      , patternLength(0u) { }

public:
  static sptr<LLVMHookEvent>
  Create(std::string const &_name, std::string const &_relativePosition) {
    return sptr<LLVMHookEvent>(new LLVMHookEvent(_name, _relativePosition));
  }

  std::string const &getName() const { return name; }
  std::string const &getRelativePosition() const { return relativePosition; }
  std::vector<LLVMEvent> const &getArguemnts() const { return arguments; }
  sptr<KOREPattern> getKOREPattern() const { return korePattern; }
  uint64_t getPatternLength() const { return patternLength; }
  void setKOREPattern(sptr<KOREPattern> _korePattern, uint64_t _patternLength) {
    korePattern = _korePattern;
    patternLength = _patternLength;
  }

  void addArgument(LLVMEvent const &argument) { arguments.push_back(argument); }

  virtual void print(std::ostream &Out, unsigned indent = 0u) const override;
};

class LLVMEvent {
private:
  bool isStepEvent;
  sptr<LLVMStepEvent> stepEvent;
  sptr<KOREPattern> korePattern;
  uint64_t patternLength;

public:
  bool isStep() const { return isStepEvent; }
  bool isPattern() const { return !isStep(); }
  sptr<LLVMStepEvent> getStepEvent() const { return stepEvent; }
  sptr<KOREPattern> getKOREPattern() const { return korePattern; }
  uint64_t getPatternLength() const { return patternLength; }
  void setStepEvent(sptr<LLVMStepEvent> _stepEvent) {
    isStepEvent = true;
    stepEvent = _stepEvent;
  }
  void setKOREPattern(sptr<KOREPattern> _korePattern, uint64_t _patternLength) {
    isStepEvent = false;
    korePattern = _korePattern;
    patternLength = _patternLength;
  }
  void print(std::ostream &Out, bool isArg, unsigned indent = 0u) const;
};

struct LLVMRewriteTrace {
  uint32_t version;
  std::vector<LLVMEvent> preTrace;
  LLVMEvent initialConfig;
  std::vector<LLVMEvent> trace;

  void print(std::ostream &Out, unsigned indent = 0u) const;
};

class ProofTraceParser {

private:
  bool verbose;
  uint32_t expectedVersion;

  // Caller needs to check that there are at least 8 bytes remaining in the
  // stream before peeking
  template <typename It>
  uint64_t peek_word(It const &it) {
    return detail::from_bytes<uint64_t>(
        reinterpret_cast<std::byte const *>(&*it));
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
  sptr<KOREPattern> parse_kore_term(It &ptr, It end, uint64_t &pattern_len) {
    if (std::distance(ptr, end) < 11u) {
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
    } else if (pattern_len > 0 && std::distance(ptr, end) > pattern_len) {
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
    if (std::distance(ptr, end) < 4u) {
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

    if (version != expectedVersion) {
      return false;
    }

    return true;
  }

  template <typename It>
  bool parse_variable(It &ptr, It end, sptr<LLVMRewriteEvent> event) {
    std::string name;
    if (!parse_name(ptr, end, name)) {
      return false;
    }

    uint64_t pattern_len;
    auto kore_term = parse_kore_term(ptr, end, pattern_len);
    if (!kore_term) {
      return false;
    }

    event->addSubstitution(name, kore_term, pattern_len);

    return check_word(ptr, end, kore_end_sentinel);
  }

  template <typename It>
  sptr<LLVMHookEvent> parse_hook(It &ptr, It end) {
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

    auto event = LLVMHookEvent::Create(name, location);

    while (std::distance(ptr, end) < 8u
           || peek_word(ptr) != hook_result_sentinel) {
      LLVMEvent argument;
      if (!parse_argument(ptr, end, argument)) {
        return nullptr;
      }
      event->addArgument(argument);
    }

    if (!check_word(ptr, end, hook_result_sentinel)) {
      return nullptr;
    }

    uint64_t pattern_len;
    auto kore_term = parse_kore_term(ptr, end, pattern_len);
    if (!kore_term) {
      return nullptr;
    }
    event->setKOREPattern(kore_term, pattern_len);

    return event;
  }

  template <typename It>
  sptr<LLVMFunctionEvent> parse_function(It &ptr, It end) {
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

    auto event = LLVMFunctionEvent::Create(name, location);

    while (std::distance(ptr, end) < 8u
           || peek_word(ptr) != function_end_sentinel) {
      LLVMEvent argument;
      if (!parse_argument(ptr, end, argument)) {
        return nullptr;
      }
      event->addArgument(argument);
    }

    if (!check_word(ptr, end, function_end_sentinel)) {
      return nullptr;
    }

    return event;
  }

  template <typename It>
  sptr<KOREPattern> parse_config(It &ptr, It end, uint64_t &pattern_len) {
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
  sptr<LLVMRuleEvent> parse_rule(It &ptr, It end) {
    if (!check_word(ptr, end, rule_event_sentinel)) {
      return nullptr;
    }

    uint64_t ordinal;
    if (!parse_ordinal(ptr, end, ordinal)) {
      return nullptr;
    }

    uint64_t arity;
    if (!parse_arity(ptr, end, arity)) {
      return nullptr;
    }

    auto event = LLVMRuleEvent::Create(ordinal);

    for (auto i = 0; i < arity; i++) {
      if (!parse_variable(ptr, end, event)) {
        return nullptr;
      }
    }

    return event;
  }

  template <typename It>
  sptr<LLVMSideConditionEvent> parse_side_condition(It &ptr, It end) {
    if (!check_word(ptr, end, side_condition_event_sentinel)) {
      return nullptr;
    }

    uint64_t ordinal;
    if (!parse_ordinal(ptr, end, ordinal)) {
      return nullptr;
    }

    uint64_t arity;
    if (!parse_arity(ptr, end, arity)) {
      return nullptr;
    }

    auto event = LLVMSideConditionEvent::Create(ordinal);

    for (auto i = 0; i < arity; i++) {
      if (!parse_variable(ptr, end, event)) {
        return nullptr;
      }
    }

    return event;
  }

  template <typename It>
  bool parse_argument(It &ptr, It end, LLVMEvent &event) {
    if (std::distance(ptr, end) >= 1u
        && detail::peek(ptr) == std::byte('\x7F')) {
      uint64_t pattern_len;
      auto kore_term = parse_kore_term(ptr, end, pattern_len);
      if (!kore_term) {
        return false;
      }
      event.setKOREPattern(kore_term, pattern_len);

      return true;
    }

    if (std::distance(ptr, end) < 8u) {
      return false;
    }

    switch (peek_word(ptr)) {

    case hook_event_sentinel: {
      auto hook_event = parse_hook(ptr, end);
      if (!hook_event) {
        return false;
      }
      event.setStepEvent(hook_event);
      return true;
    }

    case function_event_sentinel: {
      auto function_event = parse_function(ptr, end);
      if (!function_event) {
        return false;
      }
      event.setStepEvent(function_event);
      return true;
    }

    case rule_event_sentinel: {
      auto rule_event = parse_rule(ptr, end);
      if (!rule_event) {
        return false;
      }
      event.setStepEvent(rule_event);
      return true;
    }

    default: return false;
    }
  }

  template <typename It>
  sptr<LLVMStepEvent> parse_step_event(It &ptr, It end) {
    if (std::distance(ptr, end) < 8u) {
      return nullptr;
    }

    switch (peek_word(ptr)) {

    case hook_event_sentinel: return parse_hook(ptr, end);

    case function_event_sentinel: return parse_function(ptr, end);

    case rule_event_sentinel: return parse_rule(ptr, end);

    case side_condition_event_sentinel: return parse_side_condition(ptr, end);

    default: return nullptr;
    }
  }

  template <typename It>
  bool parse_event(It &ptr, It end, LLVMEvent &event) {
    if (std::distance(ptr, end) < 8u) {
      return false;
    }

    if (peek_word(ptr) == config_sentinel) {
      uint64_t pattern_len;
      auto config = parse_config(ptr, end, pattern_len);
      if (!config) {
        return false;
      }
      event.setKOREPattern(config, pattern_len);
    } else {
      auto step_event = parse_step_event(ptr, end);
      if (!step_event) {
        return false;
      }
      event.setStepEvent(step_event);
    }

    return true;
  }

  template <typename It>
  bool parse_trace(It &ptr, It end, LLVMRewriteTrace &trace) {
    uint32_t version;
    if (!parse_header(ptr, end, version)) {
      return false;
    }
    trace.version = version;

    while (std::distance(ptr, end) >= 8u && peek_word(ptr) != config_sentinel) {
      LLVMEvent event;
      if (!parse_event(ptr, end, event)) {
        return false;
      }
      trace.preTrace.push_back(event);
    }

    uint64_t pattern_len;
    auto config = parse_config(ptr, end, pattern_len);
    if (!config) {
      return false;
    }
    LLVMEvent config_event;
    config_event.setKOREPattern(config, pattern_len);
    trace.initialConfig = config_event;

    while (ptr != end) {
      LLVMEvent event;
      if (!parse_event(ptr, end, event)) {
        return false;
      }
      trace.trace.push_back(event);
    }

    return true;
  }

public:
  ProofTraceParser(bool _verbose, uint32_t _expectedVersion);

  bool parse_proof_trace(std::string const &filename, LLVMRewriteTrace &trace);
};

} // namespace kllvm

#endif
