#include <kllvm/ast/AST.h>
#include <kllvm/binary/deserializer.h>
#include <kllvm/binary/serializer.h>
#include <kllvm/parser/KOREParser.h>

#include "runtime/header.h"

#include <cassert>
#include <cstdio>
#include <iostream>
#include <limits>
#include <set>

using namespace kllvm;
using namespace kllvm::parser;

struct StringHash {
  size_t operator()(string *const &k) const {
    return std::hash<std::string>{}(std::string(k->data, len(k)));
  }
};

struct StringEq {
  bool operator()(string *const &lhs, string *const &rhs) const {
    return hook_STRING_eq(lhs, rhs);
  }
};

struct serialization_state {
  serialization_state()
      : instance()
      , boundVariables{}
      , varNames{}
      , usedVarNames{}
      , varCounter(0) { }

  // We never want to copy the state; it should only ever get passed around by
  // reference.
  serialization_state(serialization_state const &) = delete;

  serializer instance;
  std::vector<block *> boundVariables;
  std::unordered_map<string *, std::string, StringHash, StringEq> varNames;
  std::set<std::string> usedVarNames;
  uint64_t varCounter;
};

static std::string drop_back(std::string s, int n) {
  return s.substr(0, s.size() - n);
}

void serializeConfigurationInternal(
    writer *file, block *subject, const char *sort, bool isVar, void *state);

/**
 * Emit a symbol of the form ctor{...}(...); this should be preceded by the
 * appropriate pattern arguments in the buffer.
 */
static void emitSymbol(
    serializer &instance, char const *name, uint64_t arity = 0,
    uint64_t symbol_arity = 0) {
  instance.emit(header_byte<KORESymbol>);
  instance.emit_length(symbol_arity);
  instance.emit_string(drop_back(name, 2));

  instance.emit(header_byte<KORECompositePattern>);
  instance.emit_length(arity);
}

/**
 * Emit a 0-argument sort of the form Sort{}
 */
static void emitConstantSort(serializer &instance, char const *name) {
  instance.emit(header_byte<KORECompositeSort>);
  instance.emit_length(0);
  instance.emit_string(name);
}

/**
 * Emit a symbol of the form \dv{Sort}("string")
 */
static void emitToken(
    serializer &instance, char const *sort, char const *string, int len = -1) {
  instance.emit(header_byte<KOREStringPattern>);

  // Allow the length of the token to be passed in explicitly to handle the
  // Bytes sort, which can include null characters in the middle of a string.
  // Otherwise, assume that the string is null-terminated and that its length
  // can be worked out implicitly.
  if (len < 0) {
    instance.emit_string(string);
  } else {
    instance.emit_string(std::string(string, len));
  }

  emitConstantSort(instance, drop_back(sort, 2).c_str());

  instance.emit(header_byte<KORESymbol>);
  instance.emit_length(1);
  instance.emit_string("\\dv");

  instance.emit(header_byte<KORECompositePattern>);
  instance.emit_length(1);
}

void serializeMap(
    writer *file, map *map, const char *unit, const char *element,
    const char *concat, void *state) {
  auto &instance = static_cast<serialization_state *>(state)->instance;

  size_t size = map->size();
  if (size == 0) {
    emitSymbol(instance, unit);
    return;
  }

  auto tag = getTagForSymbolName(element);
  auto arg_sorts = getArgumentSortsForTag(tag);

  for (auto iter = map->begin(); iter != map->end(); ++iter) {
    serializeConfigurationInternal(
        file, iter->first, arg_sorts[0], false, state);
    serializeConfigurationInternal(
        file, iter->second, arg_sorts[1], false, state);
    emitSymbol(instance, element, 2);

    if (iter != map->begin()) {
      emitSymbol(instance, concat, 2);
    }
  }
}

void serializeRangeMap(
    writer *file, rangemap *map, const char *unit, const char *element,
    const char *concat, void *state) {
  auto &instance = static_cast<serialization_state *>(state)->instance;

  size_t size = map->size();
  if (size == 0) {
    emitSymbol(instance, unit);
    return;
  }

  auto tag = getTagForSymbolName(element);
  auto arg_sorts = getArgumentSortsForTag(tag);

  bool once = true;
  for (auto iter = rng_map::ConstRangeMapIterator<KElem, KElem>(*map);
       iter.has_next(); ++iter) {
    serializeConfigurationInternal(
        file, iter->first.start(), "SortKItem{}", false, state);
    serializeConfigurationInternal(
        file, iter->first.end(), "SortKItem{}", false, state);
    emitSymbol(instance, "LblRangemap'Coln'Range{}", 2);
    serializeConfigurationInternal(
        file, iter->second, arg_sorts[1], false, state);
    emitSymbol(instance, element, 2);

    if (once) {
      once = false;
    } else {
      emitSymbol(instance, concat, 2);
    }
  }
}

void serializeList(
    writer *file, list *list, const char *unit, const char *element,
    const char *concat, void *state) {
  auto &instance = static_cast<serialization_state *>(state)->instance;

  size_t size = list->size();
  if (size == 0) {
    emitSymbol(instance, unit);
    return;
  }

  auto tag = getTagForSymbolName(element);
  auto arg_sorts = getArgumentSortsForTag(tag);

  for (auto iter = list->begin(); iter != list->end(); ++iter) {
    serializeConfigurationInternal(file, *iter, arg_sorts[0], false, state);
    emitSymbol(instance, element, 1);

    if (iter != list->begin()) {
      emitSymbol(instance, concat, 2);
    }
  }
}

void serializeSet(
    writer *file, set *set, const char *unit, const char *element,
    const char *concat, void *state) {
  auto &instance = static_cast<serialization_state *>(state)->instance;

  size_t size = set->size();
  if (size == 0) {
    emitSymbol(instance, unit);
    return;
  }

  auto tag = getTagForSymbolName(element);
  auto arg_sorts = getArgumentSortsForTag(tag);

  for (auto iter = set->begin(); iter != set->end(); ++iter) {
    serializeConfigurationInternal(file, *iter, arg_sorts[0], false, state);
    emitSymbol(instance, element, 1);

    if (iter != set->begin()) {
      emitSymbol(instance, concat, 2);
    }
  }
}

void serializeInt(writer *file, mpz_t i, const char *sort, void *state) {
  auto &instance = static_cast<serialization_state *>(state)->instance;

  auto str = intToString(i);
  emitToken(instance, sort, str.c_str());
}

void serializeFloat(writer *file, floating *f, const char *sort, void *state) {
  auto &instance = static_cast<serialization_state *>(state)->instance;

  std::string str = floatToString(f);
  emitToken(instance, sort, str.c_str());
}

void serializeBool(writer *file, bool b, const char *sort, void *state) {
  auto &instance = static_cast<serialization_state *>(state)->instance;

  const char *str = b ? "true" : "false";
  emitToken(instance, sort, str);
}

void serializeStringBuffer(
    writer *file, stringbuffer *b, const char *sort, void *state) {
  auto &instance = static_cast<serialization_state *>(state)->instance;

  emitToken(instance, sort, b->contents->data, b->strlen);
}

void serializeMInt(
    writer *file, size_t *i, size_t bits, const char *sort, void *state) {
  auto &instance = static_cast<serialization_state *>(state)->instance;

  auto fmt = "%sp%zd";
  auto str = std::string{};

  if (i == nullptr) {
    str = "0";
  } else {
    mpz_ptr z = hook_MINT_import(i, bits, false);
    str = intToString(z);
  }

  auto buf_len = snprintf(NULL, 0, fmt, str.c_str(), bits);
  auto buffer = std::make_unique<char[]>(buf_len + 1);

  snprintf(buffer.get(), buf_len + 1, fmt, str.c_str(), bits);
  emitToken(instance, sort, buffer.get());
}

void serializeComma(writer *file, void *state) { }

static std::pair<std::string, std::vector<sptr<KORESort>>>
cached_symbol_sort_list(std::string const &symbol) {
  static auto cache = std::unordered_map<
      std::string, std::pair<std::string, std::vector<sptr<KORESort>>>>{};

  if (cache.find(symbol) == cache.end()) {
    cache[symbol] = KOREParser::from_string(symbol)->symbol_sort_list();
  }

  return cache.at(symbol);
}

void serializeConfigurationInternal(
    writer *file, block *subject, const char *sort, bool isVar,
    void *state_ptr) {
  auto &state = *static_cast<serialization_state *>(state_ptr);

  uint8_t isConstant = ((uintptr_t)subject) & 3;

  if (isConstant) {
    uint32_t tag = ((uintptr_t)subject) >> 32;

    if (isConstant == 3) {
      // bound variable
      serializeConfigurationInternal(
          file, state.boundVariables[state.boundVariables.size() - 1 - tag],
          sort, true, state_ptr);
      return;
    }

    emitSymbol(state.instance, getSymbolNameForTag(tag));
    return;
  }

  uint16_t layout = get_layout(subject);
  if (!layout) {
    string *str = (string *)subject;
    size_t subject_len = len(subject);

    if (isVar && !state.varNames.count(str)) {
      std::string stdStr = std::string(str->data, len(str));
      std::string suffix = "";
      while (state.usedVarNames.count(stdStr + suffix)) {
        suffix = std::to_string(state.varCounter++);
      }
      stdStr = stdStr + suffix;
      emitToken(state.instance, sort, suffix.c_str());
      state.usedVarNames.insert(stdStr);
      state.varNames[str] = suffix;
    } else if (isVar) {
      emitToken(state.instance, sort, state.varNames[str].c_str());
    } else {
      emitToken(state.instance, sort, str->data, subject_len);
    }

    return;
  }

  uint32_t tag = tag_hdr(subject->h.hdr);
  bool isBinder = isSymbolABinder(tag);
  if (isBinder) {
    state.boundVariables.push_back(
        *(block **)(((char *)subject) + sizeof(blockheader)));
  }

  visitor callbacks
      = {serializeConfigurationInternal,
         serializeMap,
         serializeList,
         serializeSet,
         serializeInt,
         serializeFloat,
         serializeBool,
         serializeStringBuffer,
         serializeMInt,
         serializeComma,
         serializeRangeMap};

  visitChildren(subject, file, &callbacks, state_ptr);

  auto symbol = getSymbolNameForTag(tag);
  auto symbolStr = std::string(symbol);

  auto [name, sorts] = cached_symbol_sort_list(symbolStr);

  if (name == "inj") {
    if (sorts.size() != 2) {
      abort();
    }

    sorts[0]->serialize_to(state.instance);
    emitConstantSort(state.instance, drop_back(sort, 2).c_str());
  } else {
    for (auto const &s : sorts) {
      s->serialize_to(state.instance);
    }
  }

  state.instance.emit(header_byte<KORESymbol>);
  state.instance.emit_length(sorts.size());
  state.instance.emit_string(name);

  state.instance.emit(header_byte<KORECompositePattern>);
  state.instance.emit_length(getSymbolArity(tag));

  if (isBinder) {
    state.boundVariables.pop_back();
  }
}

void serializeConfigurations(
    const char *filename, std::unordered_set<block *, HashBlock, KEq> results) {
  FILE *file = fopen(filename, "w");
  auto state = serialization_state();

  auto w = writer{file, nullptr};
  auto size = results.size();
  if (size == 0) {
    emitConstantSort(state.instance, "SortGeneratedTopCell");
    emitSymbol(state.instance, "\\bottom{}", size, 1);
  } else if (size == 1) {
    auto result = *results.begin();
    serializeConfigurationInternal(&w, result, nullptr, false, &state);
  } else {
    for (auto const &subject : results) {
      serializeConfigurationInternal(&w, subject, nullptr, false, &state);
    }

    emitConstantSort(state.instance, "SortGeneratedTopCell");
    emitSymbol(state.instance, "\\or{}", size, 1);
  }

  auto buf_size = state.instance.data().size();
  auto buf = static_cast<char *>(malloc(buf_size));
  std::memcpy(buf, state.instance.data().data(), buf_size);
  fwrite(buf, 1, buf_size, file);

  fclose(file);
}

void serializeConfigurationToFile(
    const char *filename, block *subject, bool emit_size) {
  char *data;
  size_t size;
  serializeConfiguration(subject, nullptr, &data, &size, emit_size);

  FILE *file = fopen(filename, "a");
  fwrite(data, 1, size, file);
  fclose(file);
}

void serializeConfiguration(
    block *subject, char const *sort, char **data_out, size_t *size_out,
    bool emit_size) {
  auto state = serialization_state();

  writer w = {nullptr, nullptr};
  serializeConfigurationInternal(&w, subject, sort, false, &state);

  if (emit_size) {
    state.instance.correct_emitted_size();
  }

  auto size = state.instance.data().size();
  auto buf = static_cast<char *>(malloc(size));
  std::memcpy(buf, state.instance.data().data(), size);

  *data_out = buf;
  *size_out = size;
}

void writeUInt64ToFile(const char *filename, uint64_t i) {
  FILE *file = fopen(filename, "a");
  fwrite(&i, 8, 1, file);
  fclose(file);
}

void serializeTermToFile(
    const char *filename, block *subject, const char *sort) {
  char *data;
  size_t size;
  serializeConfiguration(subject, sort, &data, &size, true);

  FILE *file = fopen(filename, "a");
  fwrite(data, 1, size, file);
  fclose(file);
}

void serializeRawTermToFile(
    const char *filename, void *subject, const char *sort) {
  block *term = constructRawTerm(subject, sort, true);

  char *data;
  size_t size;
  serializeConfiguration(term, "SortKItem{}", &data, &size, true);

  FILE *file = fopen(filename, "a");
  fwrite(data, 1, size, file);
  fclose(file);
}

std::shared_ptr<kllvm::KOREPattern>
sortedTermToKorePattern(block *subject, const char *sort) {
  auto is_kitem = (std::string(sort) == "SortKItem{}");
  block *term = is_kitem ? subject : constructRawTerm(subject, sort, false);

  char *data_out;
  size_t size_out;

  serializeConfiguration(term, "SortKItem{}", &data_out, &size_out, true);
  return deserialize_pattern(data_out, data_out + size_out, true);
}

std::shared_ptr<kllvm::KOREPattern> termToKorePattern(block *subject) {
  return sortedTermToKorePattern(subject, "SortKItem{}");
}
