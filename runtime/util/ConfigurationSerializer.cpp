#include <kllvm/ast/AST.h>
#include <kllvm/binary/serializer.h>

#include "runtime/header.h"

#include <cassert>
#include <cstdio>
#include <set>

using namespace kllvm;

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

/**
 * Emit a constant symbol of the form ctor{}().
 */
static void emitConstantSymbol(char const *name) {
  instance.emit(header_byte<KORESymbol>);
  instance.emit(int16_t{0});
  instance.emit_string(drop_back(name, 2));

  instance.emit(header_byte<KORECompositePattern>);
  instance.emit(int16_t{0});
}

/**
 * Emit a symbol of the form \dv{Sort}("string")
 */
static void emitToken(char const *sort, char const *string) {
  instance.emit(header_byte<KOREStringPattern>);
  instance.emit_string(string);

  instance.emit(header_byte<KORESortVariable>);
  instance.emit_string(drop_back(sort, 2));

  instance.emit(header_byte<KORESymbol>);
  instance.emit(int16_t{1});
  instance.emit_string("\\dv");

  instance.emit(header_byte<KORECompositePattern>);
  instance.emit(int16_t{1});
}

void serializeMap(
    writer *file, map *map, const char *unit, const char *element,
    const char *concat) {
  /* size_t size = map->size(); */
  /* if (size == 0) { */
  /*   sfprintf(file, "%s()", unit); */
  /*   return; */
  /* } */

  /* sfprintf(file, "\\left-assoc{}(%s(", concat); */

  /* bool once = true; */
  /* for (auto iter = map->begin(); iter != map->end(); ++iter) { */
  /*   if (once) { */
  /*     once = false; */
  /*   } else { */
  /*     sfprintf(file, ","); */
  /*   } */

  /*   sfprintf(file, "%s(", element); */
  /*   auto entry = *iter; */
  /*   printConfigurationInternal(file, entry.first, "SortKItem{}", false); */
  /*   sfprintf(file, ","); */
  /*   printConfigurationInternal(file, entry.second, "SortKItem{}", false); */
  /*   sfprintf(file, ")"); */
  /* } */
  /* sfprintf(file, "))"); */
}

void serializeList(
    writer *file, list *list, const char *unit, const char *element,
    const char *concat) {
  /* size_t size = list->size(); */
  /* if (size == 0) { */
  /*   sfprintf(file, "%s()", unit); */
  /*   return; */
  /* } */

  /* sfprintf(file, "\\left-assoc{}(%s(", concat); */

  /* bool once = true; */
  /* for (auto iter = list->begin(); iter != list->end(); ++iter) { */
  /*   if (once) { */
  /*     once = false; */
  /*   } else { */
  /*     sfprintf(file, ","); */
  /*   } */
  /*   sfprintf(file, "%s(", element); */
  /*   printConfigurationInternal(file, *iter, "SortKItem{}", false); */
  /*   sfprintf(file, ")"); */
  /* } */
  /* sfprintf(file, "))"); */
}

void serializeSet(
    writer *file, set *set, const char *unit, const char *element,
    const char *concat) {
  /* size_t size = set->size(); */
  /* if (size == 0) { */
  /*   sfprintf(file, "%s()", unit); */
  /*   return; */
  /* } */

  /* sfprintf(file, "\\left-assoc{}(%s(", concat); */

  /* bool once = true; */
  /* for (auto iter = set->begin(); iter != set->end(); ++iter) { */
  /*   if (once) { */
  /*     once = false; */
  /*   } else { */
  /*     sfprintf(file, ","); */
  /*   } */

  /*   sfprintf(file, "%s(", element); */
  /*   printConfigurationInternal(file, *iter, "SortKItem{}", false); */
  /*   sfprintf(file, ")"); */
  /* } */
  /* sfprintf(file, "))"); */
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
  /* sfprintf(file, "\\dv{%s}(\"%s\")", sort, str.c_str()); */
}

void serializeMInt(writer *file, size_t *i, size_t bits, const char *sort) {
  /* if (i == nullptr) { */
  /*   sfprintf(file, "\\dv{%s}(\"0p%zd\")", sort, bits); */
  /* } else { */
  /*   mpz_ptr z = hook_MINT_import(i, bits, false); */
  /*   char *str = mpz_get_str(NULL, 10, z); */
  /*   sfprintf(file, "\\dv{%s}(\"%sp%zd\")", sort, str, bits); */
  /* } */
}

void serializeComma(writer *file) { }

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
    const char *symbol = getSymbolNameForTag(tag);
    emitConstantSymbol(symbol);
    return;
  }

  uint16_t layout = layout(subject);
  if (!layout) {
    string *str = (string *)subject;
    size_t len = len(subject);

    if (len > 0) {
      emitToken(sort, str->data);
    } else if (isVar && !varNames.count(str)) {
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

  const char *symbol = getSymbolNameForTag(tag);
  std::string symbolStr(symbol);

  if (symbolStr.rfind("inj{", 0) == 0) {
    std::string prefix = symbolStr.substr(4, symbolStr.find_first_of(',') - 4);

    instance.emit(header_byte<KORESortVariable>);
    instance.emit_string(drop_back(prefix, 2));

    instance.emit(header_byte<KORESortVariable>);
    instance.emit_string(drop_back(sort, 2));

    instance.emit(header_byte<KORESymbol>);
    instance.emit(int16_t{2});
    instance.emit_string("inj");
  } else {
    instance.emit(header_byte<KORESymbol>);
    instance.emit(int16_t{0});
    instance.emit_string(symbolStr.substr(0, symbolStr.size() - 2));
  }

  instance.emit(header_byte<KORECompositePattern>);
  instance.emit(static_cast<int16_t>(getSymbolArity(tag)));

  if (isBinder) {
    boundVariables.pop_back();
  }
}

void serializeConfiguration(const char *filename, block *subject) {
  FILE *file = fopen(filename, "w");
  instance.reset();
  boundVariables.clear();
  varCounter = 0;

  writer w = {nullptr, nullptr};
  serializeConfigurationInternal(&w, subject, nullptr, false);

  varNames.clear();
  usedVarNames.clear();
  fwrite(instance.data().data(), 1, instance.data().size(), file);
  fclose(file);
}
