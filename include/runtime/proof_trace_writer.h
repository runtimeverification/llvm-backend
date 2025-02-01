#ifndef RUNTIME_PROOF_TRACE_WRITER_H
#define RUNTIME_PROOF_TRACE_WRITER_H

#include <kllvm/binary/ProofTraceParser.h>
#include <runtime/header.h>

#include <cstdio>

class proof_trace_writer {
public:
  virtual ~proof_trace_writer() = default;

  virtual void proof_trace_header(uint32_t version) = 0;
  virtual void hook_event_pre(
      char const *name, char const *pattern, char const *location_stack)
      = 0;
  virtual void
  hook_event_post(void *hook_result, uint64_t block_header, uint64_t bits)
      = 0;
  virtual void argument(void *arg, uint64_t block_header, uint64_t bits) = 0;
  virtual void rewrite_event_pre(uint64_t ordinal, uint64_t arity) = 0;
  virtual void
  variable(char const *name, void *var, uint64_t block_header, uint64_t bits)
      = 0;
  virtual void
  rewrite_event_post(void *config, uint64_t block_header, uint64_t bits)
      = 0;
  virtual void function_event_pre(char const *name, char const *location_stack)
      = 0;
  virtual void function_event_post() = 0;
  virtual void side_condition_event_pre(uint64_t ordinal, uint64_t arity) = 0;
  virtual void
  side_condition_event_post(uint64_t ordinal, bool side_cond_result)
      = 0;
  virtual void pattern_matching_failure(char const *function_name) = 0;
  virtual void function_exit(uint64_t ordinal, bool is_tail) = 0;
  virtual void configuration(block *config, bool is_initial) = 0;
  virtual void start_new_chunk() = 0;
  virtual void end_of_trace() = 0;
};

extern "C" {
extern FILE *output_file;
}

class proof_trace_file_writer : public proof_trace_writer {
private:
  char const *filename_base_;
  size_t chunk_size_;
  size_t file_number_;
  uint32_t version_;
  FILE *file_;

  void write_bytes(void const *ptr, size_t len) { fwrite(ptr, len, 1, file_); }

  void write_string(char const *str, size_t len) { fwrite(str, 1, len, file_); }

  // Note: This method will not write a 0 at the end of string.
  // The passed string should be 0 terminated.
  void write_string(char const *str) { fputs(str, file_); }

  // Note: this method will write a 0 at the end of the string.
  // The passed string should be 0 terminated.
  void write_null_terminated_string(char const *str) {
    write_string(str);
    char n = '\0';
    write_bytes(&n, 1);
  }

  void write_bool(bool b) { write_bytes(&b, sizeof(bool)); }
  void write_uint32(uint32_t i) { write_bytes(&i, sizeof(uint32_t)); }
  void write_uint64(uint64_t i) { write_bytes(&i, sizeof(uint64_t)); }

public:
  proof_trace_file_writer(char const *filename_base, size_t chunk_size)
      : filename_base_(filename_base)
      , chunk_size_(chunk_size)
      , file_number_(0)
      , version_(0) {
    if (chunk_size_ > 0) {
      std::string filename = std::string(filename_base_) + ".pre_trace";
      file_ = std::fopen(filename.c_str(), "w");
    } else {
      file_ = std::fopen(filename_base_, "w");
    }
    output_file = file_;
  }

  void proof_trace_header(uint32_t version) override {
    if (chunk_size_ == 0) {
      write_string("HINT");
      write_uint32(version);
    } else {
      write_string("PTRC");
      write_uint32(version);
      version_ = version;
    }
  }

  void hook_event_pre(
      char const *name, char const *pattern,
      char const *location_stack) override {
    write_uint64(kllvm::hook_event_sentinel);
    write_null_terminated_string(name);
    write_null_terminated_string(pattern);
    write_null_terminated_string(location_stack);
  }

  void hook_event_post(
      void *hook_result, uint64_t block_header, uint64_t bits) override {
    write_uint64(kllvm::hook_result_sentinel);
    serialize_term_to_proof_trace(file_, hook_result, block_header, bits);
  }

  void argument(void *arg, uint64_t block_header, uint64_t bits) override {
    serialize_term_to_proof_trace(file_, arg, block_header, bits);
  }

  void rewrite_event_pre(uint64_t ordinal, uint64_t arity) override {
    write_uint64(kllvm::rule_event_sentinel);
    write_uint64(ordinal);
    write_uint64(arity);
  }

  void variable(
      char const *name, void *var, uint64_t block_header,
      uint64_t bits) override {
    write_null_terminated_string(name);
    serialize_term_to_proof_trace(file_, var, block_header, bits);
  }

  void rewrite_event_post(
      void *config, uint64_t block_header, uint64_t bits) override {
    write_uint64(kllvm::config_sentinel);
    serialize_term_to_proof_trace(file_, config, block_header, bits);
  }

  void
  function_event_pre(char const *name, char const *location_stack) override {
    write_uint64(kllvm::function_event_sentinel);
    write_null_terminated_string(name);
    write_null_terminated_string(location_stack);
  }

  void function_event_post() override {
    write_uint64(kllvm::function_end_sentinel);
  }

  void side_condition_event_pre(uint64_t ordinal, uint64_t arity) override {
    write_uint64(kllvm::side_condition_event_sentinel);
    write_uint64(ordinal);
    write_uint64(arity);
  }

  void
  side_condition_event_post(uint64_t ordinal, bool side_cond_result) override {
    write_uint64(kllvm::side_condition_end_sentinel);
    write_uint64(ordinal);
    write_bool(side_cond_result);
  }

  void pattern_matching_failure(char const *function_name) override {
    write_uint64(kllvm::pattern_matching_failure_sentinel);
    write_null_terminated_string(function_name);
  }

  void function_exit(uint64_t ordinal, bool is_tail) override {
    write_uint64(kllvm::function_exit_sentinel);
    write_uint64(ordinal);
    write_bool(is_tail);
  }

  void configuration(block *config, bool is_initial) override {
    write_uint64(kllvm::config_sentinel);
    serialize_configuration_to_proof_trace(file_, config, 0);

    if (chunk_size_ > 0 && is_initial) {
      start_new_chunk();
    }
  }

  void start_new_chunk() override {
    std::fclose(file_);
    std::string filename
        = std::string(filename_base_) + "." + std::to_string(file_number_);
    file_number_++;
    file_ = std::fopen(filename.c_str(), "w");
    output_file = file_;
    write_string("CHNK");
    write_uint32(version_);
  }

  void end_of_trace() override { }
};

class proof_trace_callback_writer : public proof_trace_writer {
protected:
  struct kore_term_construction {
    void *subject;
    uint64_t block_header;
    uint64_t bits;

    kore_term_construction()
        : subject(nullptr)
        , block_header(0)
        , bits(0) { }

    kore_term_construction(void *subject, uint64_t block_header, uint64_t bits)
        : subject(subject)
        , block_header(block_header)
        , bits(bits) { }
  };

  struct kore_configuration_construction {
    block *subject;

    kore_configuration_construction(block *subject)
        : subject(subject) { }
  };

  struct pattern_matching_failure_construction {
    char const *function_name;

    pattern_matching_failure_construction(char const *function_name)
        : function_name(function_name) { }
  };

  struct side_condition_result_construction {
    uint64_t ordinal;
    bool result;

    side_condition_result_construction(uint64_t ordinal, bool result)
        : ordinal(ordinal)
        , result(result) { }
  };

  struct function_exit_construction {
    uint64_t ordinal;
    bool is_tail;

    function_exit_construction(uint64_t ordinal, bool is_tail)
        : ordinal(ordinal)
        , is_tail(is_tail) { }
  };

  struct call_event_construction {
    char const *hook_name;
    char const *symbol_name;
    char const *location;
    std::vector<kore_term_construction> arguments;
    std::optional<kore_term_construction> result;

    void initialize(char const *h_name, char const *s_name, char const *loc) {
      hook_name = h_name;
      symbol_name = s_name;
      location = loc;
      arguments.clear();
      result.reset();
    }

    /*
    call_event_construction(
        char const *hook_name, char const *symbol_name, char const *location)
        : hook_name(hook_name)
        , symbol_name(symbol_name)
        , location(location) { }

    call_event_construction(char const *symbol_name, char const *location)
        : hook_name(nullptr)
        , symbol_name(symbol_name)
        , location(location) { }
    */
  };

  struct rewrite_event_construction {
    using subst_t
        = std::vector<std::pair<char const *, kore_term_construction>>;

    uint64_t ordinal;
    uint64_t arity;
    size_t pos;
    subst_t substitution;

    void initialize(uint64_t ord, uint64_t arty) {
      ordinal = ord;
      arity = arty;
      pos = 0;
      substitution.resize(arity);
    }

    /*
    rewrite_event_construction(uint64_t ordinal, uint64_t arity)
        : ordinal(ordinal)
        , arity(arity)
        , pos(0) {
      substitution.resize(arity);
    }
    */
  };

private:
  //std::optional<call_event_construction> current_call_event_;
  call_event_construction current_call_event_;

  //std::optional<rewrite_event_construction> current_rewrite_event_{
  //    std::nullopt};
  rewrite_event_construction current_rewrite_event_;

  bool rewrite_callback_pending_;

  virtual void proof_trace_header_callback(uint32_t version) { }
  virtual void hook_event_callback(call_event_construction const &event) { }
  virtual void rewrite_event_callback(rewrite_event_construction const &event) {
  }
  virtual void
  configuration_term_event_callback(kore_term_construction const &config) { }
  virtual void function_event_callback(call_event_construction const &event) { }
  virtual void
  side_condition_event_callback(rewrite_event_construction const &event) { }
  virtual void side_condition_result_callback(
      side_condition_result_construction const &event) { }
  virtual void pattern_matching_failure_callback(
      pattern_matching_failure_construction const &event) { }
  virtual void function_exit_callback(function_exit_construction const &event) {
  }
  virtual void configuration_event_callback(
      kore_configuration_construction const &config, bool is_initial) { }

public:
  proof_trace_callback_writer() { }

  void proof_trace_header(uint32_t version) override {
    proof_trace_header_callback(version);
  }

  void hook_event_pre(
      char const *name, char const *pattern,
      char const *location_stack) override {
    //current_call_event_.reset();
    //current_call_event_.emplace(name, pattern, location_stack);
    current_call_event_.initialize(name, pattern, location_stack);
  }

  void hook_event_post(
      void *hook_result, uint64_t block_header, uint64_t bits) override {
    //current_call_event_->result.emplace(hook_result, block_header, bits);
    current_call_event_.result.emplace(hook_result, block_header, bits);
    //hook_event_callback(current_call_event_.value());
    hook_event_callback(current_call_event_);
  }

  void argument(void *arg, uint64_t block_header, uint64_t bits) override {
    //current_call_event_->arguments.emplace_back(arg, block_header, bits);
    current_call_event_.arguments.emplace_back(arg, block_header, bits);
  }

  void rewrite_event_pre(uint64_t ordinal, uint64_t arity) override {
    //current_rewrite_event_.reset();
    //current_rewrite_event_.emplace(ordinal, arity);
    current_rewrite_event_.initialize(ordinal, arity);
    if (arity == 0) {
      //rewrite_event_callback(current_rewrite_event_.value());
      rewrite_event_callback(current_rewrite_event_);
    } else {
      rewrite_callback_pending_ = true;
    }
  }

  void variable(
      char const *name, void *var, uint64_t block_header,
      uint64_t bits) override {
    //auto &p = current_rewrite_event_->substitution[current_rewrite_event_->pos];
    auto &p = current_rewrite_event_.substitution[current_rewrite_event_.pos];
    p.first = name;
    p.second.subject = var;
    p.second.block_header = block_header;
    p.second.bits = bits;
    //size_t new_pos = ++current_rewrite_event_->pos;
    size_t new_pos = ++current_rewrite_event_.pos;
    //if (new_pos == current_rewrite_event_->arity) {
    if (new_pos == current_rewrite_event_.arity) {
      if (rewrite_callback_pending_) {
        //rewrite_event_callback(current_rewrite_event_.value());
        rewrite_event_callback(current_rewrite_event_);
        rewrite_callback_pending_ = false;
      } else {
        //side_condition_event_callback(current_rewrite_event_.value());
        side_condition_event_callback(current_rewrite_event_);
      }
    }
  }

  void rewrite_event_post(
      void *config, uint64_t block_header, uint64_t bits) override {
    kore_term_construction configuration(config, block_header, bits);
    configuration_term_event_callback(configuration);
  }

  void
  function_event_pre(char const *name, char const *location_stack) override {
    //current_call_event_.reset();
    //current_call_event_.emplace(name, location_stack);
    current_call_event_.initialize(nullptr, name, location_stack);
  }

  void function_event_post() override {
    //function_event_callback(current_call_event_.value());
    function_event_callback(current_call_event_);
  }

  void side_condition_event_pre(uint64_t ordinal, uint64_t arity) override {
    //current_rewrite_event_.reset();
    //current_rewrite_event_.emplace(ordinal, arity);
    current_rewrite_event_.initialize(ordinal, arity);
    if (arity == 0) {
      //side_condition_event_callback(current_rewrite_event_.value());
      side_condition_event_callback(current_rewrite_event_);
    }
  }

  void
  side_condition_event_post(uint64_t ordinal, bool side_cond_result) override {
    side_condition_result_construction side_condition_result(
        ordinal, side_cond_result);
    side_condition_result_callback(side_condition_result);
  }

  void pattern_matching_failure(char const *function_name) override {
    pattern_matching_failure_construction pm_failure(function_name);
    pattern_matching_failure_callback(pm_failure);
  }

  void function_exit(uint64_t ordinal, bool is_tail) override {
    function_exit_construction function_exit(ordinal, is_tail);
    function_exit_callback(function_exit);
  }

  void configuration(block *config, bool is_initial) override {
    kore_configuration_construction configuration(config);
    configuration_event_callback(configuration, is_initial);
  }

  void start_new_chunk() override { }

  void end_of_trace() override { }
};

#endif // RUNTIME_PROOF_TRACE_WRITER_H
