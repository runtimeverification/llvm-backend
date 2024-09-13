#include <kllvm/ast/AST.h>
#include <kllvm/binary/deserializer.h>
#include <kllvm/binary/serializer.h>
#include <kllvm/parser/KOREParser.h>

#include "runtime/header.h"
#include "runtime/proof_trace_writer.h"

#include <fmt/printf.h>

#include <cassert>
#include <cstdio>
#include <iostream>
#include <limits>
#include <set>

using namespace kllvm;
using namespace kllvm::parser;

struct string_hash {
  size_t operator()(string *const &k) const {
    return std::hash<std::string>{}(std::string(k->data, len(k)));
  }
};

struct string_eq {
  bool operator()(string *const &lhs, string *const &rhs) const {
    return hook_STRING_eq(lhs, rhs);
  }
};

struct serialization_state {
  serialization_state() = default;
  serialization_state(serializer::flags flags)
      : instance(flags) { }

  // We never want to copy the state; it should only ever get passed around by
  // reference.
  serialization_state(serialization_state const &) = delete;
  serialization_state &operator=(serialization_state const &) = delete;

  serialization_state(serialization_state &&) = default;
  serialization_state &operator=(serialization_state &&) = default;

  ~serialization_state() = default;

  serializer instance;
  std::vector<block *> bound_variables;
  std::unordered_map<string *, std::string, string_hash, string_eq> var_names;
  std::set<std::string> used_var_names;
  uint64_t var_counter{0};
};

static std::string drop_back(std::string const &s, int n) {
  return s.substr(0, s.size() - n);
}

void serialize_configuration_internal(
    writer *file, block *subject, char const *sort, bool is_var, void *state);
void serialize_configuration_to_proof_trace_internal(
    FILE *file, block *subject, uint32_t sort, bool is_var);

/**
 * Emit a symbol of the form ctor{...}(...); this should be preceded by the
 * appropriate pattern arguments in the buffer.
 */
static void emit_symbol(
    serializer &instance, char const *name, uint64_t arity = 0,
    uint64_t symbol_arity = 0) {
  instance.emit(header_byte<kore_symbol>);
  instance.emit_length(symbol_arity);
  instance.emit_string(drop_back(name, 2));

  instance.emit(header_byte<kore_composite_pattern>);
  instance.emit_length(arity);
}

const uint8_t COMPOSITE = 0x01;
const uint8_t STRING = 0x00;
const uint8_t NULL_BYTE = 0x00;

static void emit_symbol_to_proof_trace(FILE *file, int32_t tag) {
  fwrite(&COMPOSITE, sizeof(COMPOSITE), 1, file);
  fwrite(&tag, sizeof(tag), 1, file);
}

/**
 * Emit a 0-argument sort of the form Sort{}
 */
static void emit_constant_sort(serializer &instance, char const *name) {
  instance.emit(header_byte<kore_composite_sort>);
  instance.emit_length(0);
  instance.emit_string(name);
}

/**
 * Emit a symbol of the form \dv{Sort}("string")
 */
static void emit_token(
    serializer &instance, char const *sort, char const *string, int len = -1) {
  instance.emit(header_byte<kore_string_pattern>);

  // Allow the length of the token to be passed in explicitly to handle the
  // Bytes sort, which can include null characters in the middle of a string.
  // Otherwise, assume that the string is null-terminated and that its length
  // can be worked out implicitly.
  if (len < 0) {
    instance.emit_string(string);
  } else {
    instance.emit_string(std::string(string, len));
  }

  emit_constant_sort(instance, drop_back(sort, 2).c_str());

  instance.emit(header_byte<kore_symbol>);
  instance.emit_length(1);
  instance.emit_string("\\dv");

  instance.emit(header_byte<kore_composite_pattern>);
  instance.emit_length(1);
}

static void emit_token_to_proof_trace(
    FILE *file, uint32_t sort, char const *str, size_t len) {
  emit_symbol_to_proof_trace(file, sort);

  fwrite(&STRING, sizeof(STRING), 1, file);
  fwrite(&len, sizeof(len), 1, file);
  fwrite(str, 1, len, file);
  fwrite(&NULL_BYTE, sizeof(NULL_BYTE), 1, file);
}

void serialize_map(
    writer *file, map *map, char const *unit, char const *element,
    char const *concat, void *state) {
  auto &instance = static_cast<serialization_state *>(state)->instance;

  size_t size = map->size();
  if (size == 0) {
    emit_symbol(instance, unit);
    return;
  }

  auto tag = get_tag_for_symbol_name(element);
  auto *arg_sorts = get_argument_sorts_for_tag(tag);

  for (auto iter = map->begin(); iter != map->end(); ++iter) {
    serialize_configuration_internal(
        file, iter->first, arg_sorts[0], false, state);
    serialize_configuration_internal(
        file, iter->second, arg_sorts[1], false, state);
    emit_symbol(instance, element, 2);

    if (iter != map->begin()) {
      emit_symbol(instance, concat, 2);
    }
  }
}

void serialize_map_to_proof_trace(
    FILE *file, map *map, uint32_t unit, uint32_t element, uint32_t concat) {
  size_t size = map->size();
  if (size == 0) {
    emit_symbol_to_proof_trace(file, unit);
    return;
  }

  auto *arg_sorts
      = get_argument_sorts_for_tag_with_proof_trace_serialization(element);

  for (size_t i = 0; i < size - 1; ++i) {
    emit_symbol_to_proof_trace(file, concat);
  }

  for (auto iter = map->begin(); iter != map->end(); ++iter) {
    emit_symbol_to_proof_trace(file, element);
    serialize_configuration_to_proof_trace_internal(
        file, iter->first, arg_sorts[0], false);
    serialize_configuration_to_proof_trace_internal(
        file, iter->second, arg_sorts[1], false);
  }
}

void serialize_range_map(
    writer *file, rangemap *map, char const *unit, char const *element,
    char const *concat, void *state) {
  auto &instance = static_cast<serialization_state *>(state)->instance;

  size_t size = map->size();
  if (size == 0) {
    emit_symbol(instance, unit);
    return;
  }

  auto tag = get_tag_for_symbol_name(element);
  auto *arg_sorts = get_argument_sorts_for_tag(tag);

  bool once = true;
  for (auto iter = rng_map::ConstRangeMapIterator<k_elem, k_elem>(*map);
       iter.has_next(); ++iter) {
    serialize_configuration_internal(
        file, iter->first.start(), "SortKItem{}", false, state);
    serialize_configuration_internal(
        file, iter->first.end(), "SortKItem{}", false, state);
    emit_symbol(instance, "LblRangeMap'Coln'Range{}", 2);
    serialize_configuration_internal(
        file, iter->second, arg_sorts[1], false, state);
    emit_symbol(instance, element, 2);

    if (once) {
      once = false;
    } else {
      emit_symbol(instance, concat, 2);
    }
  }
}

void serialize_range_map_to_proof_trace(
    FILE *file, rangemap *map, uint32_t unit, uint32_t element,
    uint32_t concat) {
  size_t size = map->size();
  if (size == 0) {
    emit_symbol_to_proof_trace(file, unit);
    return;
  }

  auto *arg_sorts
      = get_argument_sorts_for_tag_with_proof_trace_serialization(element);
  const static auto range_tag
      = get_tag_for_symbol_name("LblRangeMap'Coln'Range{}");
  const static auto *range_sorts
      = get_argument_sorts_for_tag_with_proof_trace_serialization(range_tag);

  for (size_t i = 0; i < size - 1; ++i) {
    emit_symbol_to_proof_trace(file, concat);
  }

  for (auto iter = rng_map::ConstRangeMapIterator<k_elem, k_elem>(*map);
       iter.has_next(); ++iter) {
    emit_symbol_to_proof_trace(file, element);
    emit_symbol_to_proof_trace(file, range_tag);
    serialize_configuration_to_proof_trace_internal(
        file, iter->first.start(), range_sorts[0], false);
    serialize_configuration_to_proof_trace_internal(
        file, iter->first.end(), range_sorts[1], false);
    serialize_configuration_to_proof_trace_internal(
        file, iter->second, arg_sorts[1], false);
  }
}

void serialize_list(
    writer *file, list *list, char const *unit, char const *element,
    char const *concat, void *state) {
  auto &instance = static_cast<serialization_state *>(state)->instance;

  size_t size = list->size();
  if (size == 0) {
    emit_symbol(instance, unit);
    return;
  }

  auto tag = get_tag_for_symbol_name(element);
  auto *arg_sorts = get_argument_sorts_for_tag(tag);

  for (auto iter = list->begin(); iter != list->end(); ++iter) {
    serialize_configuration_internal(file, *iter, arg_sorts[0], false, state);
    emit_symbol(instance, element, 1);

    if (iter != list->begin()) {
      emit_symbol(instance, concat, 2);
    }
  }
}

void serialize_list_to_proof_trace(
    FILE *file, list *list, uint32_t unit, uint32_t element, uint32_t concat) {
  size_t size = list->size();
  if (size == 0) {
    emit_symbol_to_proof_trace(file, unit);
    return;
  }

  auto *arg_sorts
      = get_argument_sorts_for_tag_with_proof_trace_serialization(element);

  for (size_t i = 0; i < size - 1; ++i) {
    emit_symbol_to_proof_trace(file, concat);
  }

  for (auto iter = list->begin(); iter != list->end(); ++iter) {
    emit_symbol_to_proof_trace(file, element);
    serialize_configuration_to_proof_trace_internal(
        file, *iter, arg_sorts[0], false);
  }
}

void serialize_set(
    writer *file, set *set, char const *unit, char const *element,
    char const *concat, void *state) {
  auto &instance = static_cast<serialization_state *>(state)->instance;

  size_t size = set->size();
  if (size == 0) {
    emit_symbol(instance, unit);
    return;
  }

  auto tag = get_tag_for_symbol_name(element);
  auto *arg_sorts = get_argument_sorts_for_tag(tag);

  for (auto iter = set->begin(); iter != set->end(); ++iter) {
    serialize_configuration_internal(file, *iter, arg_sorts[0], false, state);
    emit_symbol(instance, element, 1);

    if (iter != set->begin()) {
      emit_symbol(instance, concat, 2);
    }
  }
}

void serialize_set_to_proof_trace(
    FILE *file, set *set, uint32_t unit, uint32_t element, uint32_t concat) {
  size_t size = set->size();
  if (size == 0) {
    emit_symbol_to_proof_trace(file, unit);
    return;
  }

  auto *arg_sorts
      = get_argument_sorts_for_tag_with_proof_trace_serialization(element);

  for (size_t i = 0; i < size - 1; ++i) {
    emit_symbol_to_proof_trace(file, concat);
  }

  for (auto iter = set->begin(); iter != set->end(); ++iter) {
    emit_symbol_to_proof_trace(file, element);
    serialize_configuration_to_proof_trace_internal(
        file, *iter, arg_sorts[0], false);
  }
}

void serialize_int(writer *file, mpz_t i, char const *sort, void *state) {
  auto &instance = static_cast<serialization_state *>(state)->instance;

  auto str = int_to_string(i);
  emit_token(instance, sort, str.c_str());
}

void serialize_int_to_proof_trace(FILE *file, mpz_t i, uint32_t sort) {
  auto str = int_to_string(i);
  emit_token_to_proof_trace(file, sort, str.data(), str.length());
}

void serialize_float(writer *file, floating *f, char const *sort, void *state) {
  auto &instance = static_cast<serialization_state *>(state)->instance;

  std::string str = float_to_string(f);
  emit_token(instance, sort, str.c_str());
}

void serialize_float_to_proof_trace(FILE *file, floating *f, uint32_t sort) {
  auto str = float_to_string(f);
  emit_token_to_proof_trace(file, sort, str.data(), str.length());
}

void serialize_bool(writer *file, bool b, char const *sort, void *state) {
  auto &instance = static_cast<serialization_state *>(state)->instance;

  char const *str = b ? "true" : "false";
  emit_token(instance, sort, str);
}

void serialize_bool_to_proof_trace(FILE *file, bool b, uint32_t sort) {
  std::string str = b ? "true" : "false";
  emit_token_to_proof_trace(file, sort, str.data(), str.length());
}

void serialize_string_buffer(
    writer *file, stringbuffer *b, char const *sort, void *state) {
  auto &instance = static_cast<serialization_state *>(state)->instance;

  emit_token(instance, sort, b->contents->data, b->strlen);
}

void serialize_string_buffer_to_proof_trace(
    FILE *file, stringbuffer *b, uint32_t sort) {
  emit_token_to_proof_trace(file, sort, b->contents->data, b->strlen);
}

void serialize_m_int(
    writer *file, size_t *i, size_t bits, char const *sort, void *state) {
  auto &instance = static_cast<serialization_state *>(state)->instance;

  auto str = (i == nullptr) ? std::string("0")
                            : int_to_string(hook_MINT_import(i, bits, false));

  auto buffer = fmt::format("{}p{}", str, bits);
  emit_token(instance, sort, buffer.c_str());
}

void serialize_m_int_to_proof_trace(
    FILE *file, size_t *i, size_t bits, uint32_t sort) {
  auto str = (i == nullptr) ? std::string("0")
                            : int_to_string(hook_MINT_import(i, bits, false));

  auto buffer = fmt::format("{}p{}", str, bits);
  emit_token_to_proof_trace(file, sort, buffer.data(), buffer.length());
}

void serialize_comma(writer *file, void *state) { }

static std::pair<std::string, std::vector<sptr<kore_sort>>>
cached_symbol_sort_list(std::string const &symbol) {
  static auto cache = std::unordered_map<
      std::string, std::pair<std::string, std::vector<sptr<kore_sort>>>>{};

  if (cache.find(symbol) == cache.end()) {
    auto [id, sorts] = kore_parser::from_string(symbol)->symbol_sort_list();

    // The parser returns the actual name of the symbol separately to its formal
    // sort parameters. However, the interface of emitSymbol is compatible with
    // the values stored in the interpreter's table of symbol names, which
    // include the fully-instantiated symbol. For constant symbols, we know to
    // drop the last two characters when emitting them. We therefore tack on an
    // extra "{}" at the end of what comes back from the parser to keep it
    // compatible with the same interface.
    cache[symbol] = {fmt::format("{}{{}}", id), sorts};
  }

  return cache.at(symbol);
}

void serialize_configuration_internal(
    writer *file, block *subject, char const *sort, bool is_var,
    void *state_ptr) {
  auto &state = *static_cast<serialization_state *>(state_ptr);

  uint8_t is_constant = ((uintptr_t)subject) & 3;

  if (is_constant) {
    uint32_t tag = ((uintptr_t)subject) >> 32;

    if (is_constant == 3) {
      // bound variable
      serialize_configuration_internal(
          file, state.bound_variables[state.bound_variables.size() - 1 - tag],
          sort, true, state_ptr);
      return;
    }

    emit_symbol(state.instance, get_symbol_name_for_tag(tag));
    return;
  }

  uint16_t layout = get_layout(subject);
  if (!layout) {
    auto *str = (string *)subject;
    size_t subject_len = len(subject);

    if (is_var && !state.var_names.contains(str)) {
      std::string std_str = std::string(str->data, len(str));
      std::string suffix;
      while (state.used_var_names.contains(std_str + suffix)) {
        suffix = std::to_string(state.var_counter++);
      }
      std_str = std_str + suffix;
      emit_token(state.instance, sort, suffix.c_str());
      state.used_var_names.insert(std_str);
      state.var_names[str] = suffix;
    } else if (is_var) {
      emit_token(state.instance, sort, state.var_names[str].c_str());
    } else {
      emit_token(state.instance, sort, str->data, subject_len);
    }

    return;
  }

  uint32_t tag = tag_hdr(subject->h.hdr);
  bool is_binder = is_symbol_a_binder(tag);
  if (is_binder) {
    state.bound_variables.push_back(
        *(block **)(((char *)subject) + sizeof(blockheader)));
  }

  visitor callbacks
      = {serialize_configuration_internal,
         serialize_map,
         serialize_list,
         serialize_set,
         serialize_int,
         serialize_float,
         serialize_bool,
         serialize_string_buffer,
         serialize_m_int,
         serialize_comma,
         serialize_range_map};

  visit_children(subject, file, &callbacks, state_ptr);

  auto const *symbol = get_symbol_name_for_tag(tag);

  if (symbol_is_instantiation(tag)) {
    auto [name, sorts] = cached_symbol_sort_list(symbol);

    if (name == "inj{}") {
      // Injections need to be special cased; the sort being injected _into_
      // will be different at runtime to what's in the definition. We therefore
      // use the contextual sort from the serialization process instead of the
      // second formal argument.
      assert(sorts.size() == 2 && "Malformed injection when serializing");

      sorts[0]->serialize_to(state.instance);
      emit_constant_sort(state.instance, drop_back(sort, 2).c_str());
    } else {
      for (auto const &s : sorts) {
        s->serialize_to(state.instance);
      }
    }

    emit_symbol(
        state.instance, name.c_str(), get_symbol_arity(tag), sorts.size());
  } else {
    emit_symbol(state.instance, symbol, get_symbol_arity(tag));
  }

  if (is_binder) {
    state.bound_variables.pop_back();
  }
}

void serialize_configuration_to_proof_trace_internal(
    FILE *file, block *subject, uint32_t sort, bool is_var) {
  if (is_var) {
    throw std::invalid_argument("does not support bound variables yet");
  }
  uint8_t is_constant = ((uintptr_t)subject) & 3;

  if (is_constant) {
    uint32_t tag = ((uintptr_t)subject) >> 32;
    if (is_constant == 3) {
      throw std::invalid_argument("does not support bound variables yet");
    }

    emit_symbol_to_proof_trace(file, tag);
    return;
  }

  uint16_t layout = get_layout(subject);
  if (!layout) {
    auto *str = (string *)subject;
    emit_token_to_proof_trace(file, sort, str->data, len(subject));
    return;
  }

  uint32_t tag = tag_hdr(subject->h.hdr);
  emit_symbol_to_proof_trace(file, tag);

  serialize_to_proof_trace_visitor callbacks
      = {serialize_configuration_to_proof_trace_internal,
         serialize_map_to_proof_trace,
         serialize_list_to_proof_trace,
         serialize_set_to_proof_trace,
         serialize_int_to_proof_trace,
         serialize_float_to_proof_trace,
         serialize_bool_to_proof_trace,
         serialize_string_buffer_to_proof_trace,
         serialize_m_int_to_proof_trace,
         serialize_range_map_to_proof_trace};

  visit_children_for_serialize_to_proof_trace(subject, file, &callbacks);
}

void serialize_configurations(
    FILE *file, std::unordered_set<block *, hash_block, k_eq> results) {
  auto state = serialization_state();

  auto w = writer{file, nullptr};
  auto size = results.size();
  if (size == 0) {
    emit_constant_sort(state.instance, "SortGeneratedTopCell");
    emit_symbol(state.instance, "\\bottom{}", size, 1);
  } else if (size == 1) {
    auto *result = *results.begin();
    serialize_configuration_internal(&w, result, nullptr, false, &state);
  } else {
    for (auto const &subject : results) {
      serialize_configuration_internal(&w, subject, nullptr, false, &state);
    }

    emit_constant_sort(state.instance, "SortGeneratedTopCell");
    emit_symbol(state.instance, "\\or{}", size, 1);
  }

  auto buf_size = state.instance.data().size();
  auto *buf = static_cast<char *>(malloc(buf_size));
  std::copy_n(state.instance.data().begin(), buf_size, buf);
  fwrite(buf, 1, buf_size, file);

  free(buf);
}

void serialize_configuration_to_file(
    FILE *file, block *subject, bool emit_size, bool use_intern) {
  char *data = nullptr;
  size_t size = 0;
  serialize_configuration(
      subject, nullptr, &data, &size, emit_size, use_intern);

  fwrite(data, 1, size, file);

  free(data);
}

void serialize_configuration(
    block *subject, char const *sort, char **data_out, size_t *size_out,
    bool emit_size, bool use_intern) {
  auto state = serialization_state(
      use_intern ? serializer::flags::NONE : serializer::flags::NoIntern);

  writer w = {nullptr, nullptr};
  serialize_configuration_internal(&w, subject, sort, false, &state);

  if (emit_size) {
    state.instance.correct_emitted_size();
  }

  auto size = state.instance.data().size();
  auto *buf = static_cast<char *>(malloc(size));
  std::copy_n(state.instance.data().begin(), size, buf);

  *data_out = buf;
  *size_out = size;
}

void serialize_configuration_to_proof_trace(
    FILE *file, block *subject, uint32_t sort) {
  fputs("\x7FKR2", file);
  serialize_configuration_to_proof_trace_internal(file, subject, sort, false);
}

void write_hook_event_pre_to_proof_trace(
    void *proof_writer, char const *name, char const *pattern,
    char const *location_stack) {
  static_cast<proof_trace_writer *>(proof_writer)
      ->hook_event_pre(name, pattern, location_stack);
}

void write_hook_event_post_to_proof_trace(
    void *proof_writer, void *hook_result, uint64_t block_header,
    bool indirect) {
  static_cast<proof_trace_writer *>(proof_writer)
      ->hook_event_post(hook_result, block_header, indirect);
}

void write_argument_to_proof_trace(
    void *proof_writer, void *arg, uint64_t block_header, bool indirect) {
  static_cast<proof_trace_writer *>(proof_writer)
      ->argument(arg, block_header, indirect);
}

void write_rewrite_event_pre_to_proof_trace(
    void *proof_writer, uint64_t ordinal, uint64_t arity) {
  static_cast<proof_trace_writer *>(proof_writer)
      ->rewrite_event_pre(ordinal, arity);
}

void write_variable_to_proof_trace(
    void *proof_writer, char const *name, void *var, uint64_t block_header,
    bool indirect) {
  static_cast<proof_trace_writer *>(proof_writer)
      ->variable(name, var, block_header, indirect);
}

void write_rewrite_event_post_to_proof_trace(
    void *proof_writer, void *config, uint64_t block_header, bool indirect) {
  static_cast<proof_trace_writer *>(proof_writer)
      ->rewrite_event_post(config, block_header, indirect);
}

void write_function_event_pre_to_proof_trace(
    void *proof_writer, char const *name, char const *location_stack) {
  static_cast<proof_trace_writer *>(proof_writer)
      ->function_event_pre(name, location_stack);
}

void write_function_event_post_to_proof_trace(void *proof_writer) {
  static_cast<proof_trace_writer *>(proof_writer)->function_event_post();
}

void write_side_condition_event_pre_to_proof_trace(
    void *proof_writer, uint64_t ordinal, uint64_t arity) {
  static_cast<proof_trace_writer *>(proof_writer)
      ->side_condition_event_pre(ordinal, arity);
}

void write_side_condition_event_post_to_proof_trace(
    void *proof_writer, uint64_t ordinal, bool side_cond_result) {
  static_cast<proof_trace_writer *>(proof_writer)
      ->side_condition_event_post(ordinal, side_cond_result);
}

void write_pattern_matching_failure_to_proof_trace(
    void *proof_writer, char const *function_name) {
  static_cast<proof_trace_writer *>(proof_writer)
      ->pattern_matching_failure(function_name);
}

void write_configuration_to_proof_trace(void *proof_writer, block *config, bool is_initial) {
  static_cast<proof_trace_writer *>(proof_writer)->configuration(config, is_initial);
}

void start_new_chunk_in_proof_trace(void *proof_writer) {
  static_cast<proof_trace_writer *>(proof_writer)->start_new_chunk();
}

void serialize_term_to_file(
    FILE *file, void *subject, char const *sort, bool use_intern,
    bool k_item_inj) {
  block *term = k_item_inj ? construct_k_item_inj(subject, sort, true)
                           : (block *)subject;
  sort = k_item_inj ? "SortKItem{}" : sort;

  char *data = nullptr;
  size_t size = 0;
  serialize_configuration(term, sort, &data, &size, true, use_intern);

  fwrite(data, 1, size, file);

  free(data);
}

void serialize_term_to_proof_trace(
    FILE *file, void *subject, uint64_t block_header, bool indirect) {
  void *arg = indirect ? (void *)&subject : subject;
  struct blockheader header_val {
    block_header
  };
  auto *term = (block *)kore_alloc(size_hdr(block_header));
  term->h = header_val;
  store_symbol_children(term, &arg);
  fputs("\x7FKR2", file);

  serialize_to_proof_trace_visitor callbacks
      = {serialize_configuration_to_proof_trace_internal,
         serialize_map_to_proof_trace,
         serialize_list_to_proof_trace,
         serialize_set_to_proof_trace,
         serialize_int_to_proof_trace,
         serialize_float_to_proof_trace,
         serialize_bool_to_proof_trace,
         serialize_string_buffer_to_proof_trace,
         serialize_m_int_to_proof_trace,
         serialize_range_map_to_proof_trace};

  visit_children_for_serialize_to_proof_trace(term, file, &callbacks);
}

void serialize_raw_term_to_file(
    FILE *file, void *subject, char const *sort, bool use_intern) {
  block *term = construct_raw_term(subject, sort, true);

  char *data = nullptr;
  size_t size = 0;
  serialize_configuration(term, "SortKItem{}", &data, &size, true, use_intern);

  fwrite(data, 1, size, file);

  free(data);
}

std::shared_ptr<kllvm::kore_pattern>
sorted_term_to_kore_pattern(block *subject, char const *sort) {
  auto is_kitem = (std::string(sort) == "SortKItem{}");
  block *term = is_kitem ? subject : construct_raw_term(subject, sort, false);

  char *data_out = nullptr;
  size_t size_out = 0;

  serialize_configuration(
      term, "SortKItem{}", &data_out, &size_out, true, true);
  auto result = deserialize_pattern(data_out, data_out + size_out);

  free(data_out);
  return result;
}

std::shared_ptr<kllvm::kore_pattern> term_to_kore_pattern(block *subject) {
  return sorted_term_to_kore_pattern(subject, "SortKItem{}");
}
