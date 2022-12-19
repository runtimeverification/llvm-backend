#include <kllvm/ast/AST.h>
#include <kllvm/binary/serializer.h>
#include <kllvm/parser/KOREParser.h>

#include "runtime/header.h"

#include <cassert>
#include <cstdio>
#include <limits>
#include <set>

using namespace kllvm;
using namespace kllvm::parser;

static std::string drop_back(std::string s, int n) {
  return s.substr(0, s.size() - n);
}

static thread_local serializer instance = serializer();

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

static thread_local std::vector<block *> boundVariables;
static thread_local std::unordered_map<
    string *, std::string, StringHash, StringEq>
    varNames;
static thread_local std::set<std::string> usedVarNames;
static thread_local uint64_t varCounter = 0;

void serializeConfigurationInternal(
    writer *file, block *subject, const char *sort, bool isVar);

/**
 * Emit a symbol of the form ctor{}(...); this should be preceded by the
 * appropriate pattern arguments in the buffer.
 */
static void emitSymbol(char const *name, uint64_t arity = 0) {
  instance.emit(header_byte<KORESymbol>);
  instance.emit_length(0);
  instance.emit_string(drop_back(name, 2));

  instance.emit(header_byte<KORECompositePattern>);
  instance.emit_length(arity);
}

/**
 * Emit a 0-argument sort of the form Sort{}
 */
static void emitConstantSort(char const *name) {
  instance.emit(header_byte<KORECompositeSort>);
  instance.emit_length(0);
  instance.emit_string(name);
}

/**
 * Emit a symbol of the form \dv{Sort}("string")
 */
static void emitToken(char const *sort, char const *string, int len = 0) {
  instance.emit(header_byte<KOREStringPattern>);

  // Allow the length of the token to be passed in explicitly to handle the
  // Bytes sort, which can include null characters in the middle of a string.
  // Otherwise, assume that the string is null-terminated and that its length
  // can be worked out implicitly.
  if (len == 0) {
    instance.emit_string(string);
  } else {
    instance.emit_string(std::string(string, len));
  }

  emitConstantSort(drop_back(sort, 2).c_str());

  instance.emit(header_byte<KORESymbol>);
  instance.emit_length(1);
  instance.emit_string("\\dv");

  instance.emit(header_byte<KORECompositePattern>);
  instance.emit_length(1);
}

void serializeMap(
    writer *file, map *map, const char *unit, const char *element,
    const char *concat) {
  size_t size = map->size();
  if (size == 0) {
    emitSymbol(unit);
    return;
  }

  for (auto iter = map->begin(); iter != map->end(); ++iter) {
    serializeConfigurationInternal(file, iter->first, "SortKItem{}", false);
    serializeConfigurationInternal(file, iter->second, "SortKItem{}", false);
    emitSymbol(element, 2);

    if (iter != map->begin()) {
      emitSymbol(concat, 2);
    }
  }
}

void serializeList(
    writer *file, list *list, const char *unit, const char *element,
    const char *concat) {
  size_t size = list->size();
  if (size == 0) {
    emitSymbol(unit);
    return;
  }

  for (auto iter = list->begin(); iter != list->end(); ++iter) {
    serializeConfigurationInternal(file, *iter, "SortKItem{}", false);
    emitSymbol(element, 1);

    if (iter != list->begin()) {
      emitSymbol(concat, 2);
    }
  }
}

void serializeSet(
    writer *file, set *set, const char *unit, const char *element,
    const char *concat) {
  size_t size = set->size();
  if (size == 0) {
    emitSymbol(unit);
    return;
  }

  for (auto iter = set->begin(); iter != set->end(); ++iter) {
    serializeConfigurationInternal(file, *iter, "SortKItem{}", false);
    emitSymbol(element, 1);

    if (iter != set->begin()) {
      emitSymbol(concat, 2);
    }
  }
}

void serializeInt(writer *file, mpz_t i, const char *sort) {
  char *str = mpz_get_str(NULL, 10, i);
  emitToken(sort, str);
}

void serializeFloat(writer *file, floating *f, const char *sort) {
  std::string str = floatToString(f);
  emitToken(sort, str.c_str());
}

void serializeBool(writer *file, bool b, const char *sort) {
  const char *str = b ? "true" : "false";
  emitToken(sort, str);
}

void serializeStringBuffer(writer *file, stringbuffer *b, const char *sort) {
  std::string str(b->contents->data, b->strlen);
  emitToken(sort, str.c_str());
}

void serializeMInt(writer *file, size_t *i, size_t bits, const char *sort) {
  auto fmt = "%sp%zd";
  char const *str = nullptr;

  if (i == nullptr) {
    str = "0";
  } else {
    mpz_ptr z = hook_MINT_import(i, bits, false);
    str = mpz_get_str(NULL, 10, z);
  }

  auto buf_len = snprintf(NULL, 0, fmt, str, bits);
  auto buffer = std::make_unique<char[]>(buf_len + 1);

  snprintf(buffer.get(), buf_len + 1, fmt, str, bits);
  emitToken(sort, buffer.get());
}

void serializeComma(writer *file) { }

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
    writer *file, block *subject, const char *sort, bool isVar) {
  uint8_t isConstant = ((uintptr_t)subject) & 3;

  if (isConstant) {
    uint32_t tag = ((uintptr_t)subject) >> 32;

    if (isConstant == 3) {
      // bound variable
      printConfigurationInternal(
          file, boundVariables[boundVariables.size() - 1 - tag], sort, true);
      return;
    }

    emitSymbol(getSymbolNameForTag(tag));
    return;
  }

  uint16_t layout = layout(subject);
  if (!layout) {
    string *str = (string *)subject;
    size_t len = len(subject);

    if (isVar && !varNames.count(str)) {
      std::string stdStr = std::string(str->data, len(str));
      std::string suffix = "";
      while (usedVarNames.count(stdStr + suffix)) {
        suffix = std::to_string(varCounter++);
      }
      stdStr = stdStr + suffix;
      emitToken(sort, suffix.c_str());
      usedVarNames.insert(stdStr);
      varNames[str] = suffix;
    } else if (isVar) {
      emitToken(sort, varNames[str].c_str());
    } else {
      emitToken(sort, str->data, len);
    }

    return;
  }

  uint32_t tag = tag_hdr(subject->h.hdr);
  bool isBinder = isSymbolABinder(tag);
  if (isBinder) {
    boundVariables.push_back(
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
         serializeComma};

  visitChildren(subject, file, &callbacks);

  auto symbol = getSymbolNameForTag(tag);
  auto symbolStr = std::string(symbol);

  auto [name, sorts] = cached_symbol_sort_list(symbolStr);

  if (name == "inj") {
    if (sorts.size() != 2) {
      abort();
    }

    sorts[0]->serialize_to(instance);
    emitConstantSort(drop_back(sort, 2).c_str());
  } else {
    for (auto const &s : sorts) {
      s->serialize_to(instance);
    }
  }

  instance.emit(header_byte<KORESymbol>);
  instance.emit_length(sorts.size());
  instance.emit_string(name);

  instance.emit(header_byte<KORECompositePattern>);
  instance.emit_length(getSymbolArity(tag));

  if (isBinder) {
    boundVariables.pop_back();
  }
}

void serializeConfigurationToFile(const char *filename, block *subject) {
  char *data;
  size_t size;
  serializeConfiguration(subject, nullptr, &data, &size);

  FILE *file = fopen(filename, "w");
  fwrite(data, 1, size, file);
  fclose(file);
}

void serializeConfiguration(
    block *subject, char const *sort, char **data_out, size_t *size_out) {
  instance.reset();
  boundVariables.clear();
  varCounter = 0;

  writer w = {nullptr, nullptr};
  serializeConfigurationInternal(&w, subject, sort, false);

  varNames.clear();
  usedVarNames.clear();

  *data_out = (char *)instance.data().data();
  *size_out = instance.data().size();
}
