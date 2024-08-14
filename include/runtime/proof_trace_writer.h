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
  hook_event_post(void *hook_result, uint64_t block_header, bool indirect)
      = 0;
  virtual void argument(void *arg, uint64_t block_header, bool indirect) = 0;
  virtual void rewrite_event_pre(uint64_t ordinal, uint64_t arity) = 0;
  virtual void
  variable(char const *name, void *var, uint64_t block_header, bool indirect)
      = 0;
  virtual void
  rewrite_event_post(void *config, uint64_t block_header, bool indirect)
      = 0;
  virtual void function_event_pre(char const *name, char const *location_stack)
      = 0;
  virtual void function_event_post() = 0;
  virtual void side_condition_event_pre(uint64_t ordinal, uint64_t arity) = 0;
  virtual void
  side_condition_event_post(uint64_t ordinal, bool side_cond_result)
      = 0;
  virtual void configuration(block *config) = 0;
  virtual void end_of_trace() = 0;
};

class proof_trace_file_writer : public proof_trace_writer {
private:
  FILE *file_;

  void write(void const *ptr, size_t len) { fwrite(ptr, len, 1, file_); }

  void write_string(char const *str, size_t len) { fwrite(str, 1, len, file_); }

  // Note: This method will not write a 0 at the end of string.
  // The passed string should be 0 terminated.
  void write_string(char const *str) { fputs(str, file_); }

  // Note: this method will write a 0 at the end of the string.
  // The passed string should be 0 terminated.
  void write_null_terminated_string(char const *str) {
    write_string(str);
    char n = 0;
    write(&n, 1);
  }

  void write_bool(bool b) { write(&b, sizeof(bool)); }
  void write_uint32(uint32_t i) { write(&i, sizeof(uint32_t)); }
  void write_uint64(uint64_t i) { write(&i, sizeof(uint64_t)); }

public:
  proof_trace_file_writer(FILE *file)
      : file_(file) { }

  void proof_trace_header(uint32_t version) override {
    write_string("HINT");
    write_uint32(version);
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
      void *hook_result, uint64_t block_header, bool indirect) override {
    write_uint64(kllvm::hook_result_sentinel);
    serialize_term_to_proof_trace(file_, hook_result, block_header, indirect);
  }

  void argument(void *arg, uint64_t block_header, bool indirect) override {
    serialize_term_to_proof_trace(file_, arg, block_header, indirect);
  }

  void rewrite_event_pre(uint64_t ordinal, uint64_t arity) override {
    write_uint64(kllvm::rule_event_sentinel);
    write_uint64(ordinal);
    write_uint64(arity);
  }

  void variable(
      char const *name, void *var, uint64_t block_header,
      bool indirect) override {
    write_null_terminated_string(name);
    serialize_term_to_proof_trace(file_, var, block_header, indirect);
  }

  void rewrite_event_post(
      void *config, uint64_t block_header, bool indirect) override {
    write_uint64(kllvm::config_sentinel);
    serialize_term_to_proof_trace(file_, config, block_header, indirect);
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

  void configuration(block *config) override {
    write_uint64(kllvm::config_sentinel);
    serialize_configuration_to_proof_trace(file_, config, 0);
  }

  void end_of_trace() override { }
};

#endif // RUNTIME_PROOF_TRACE_WRITER_H
