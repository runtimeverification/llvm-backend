#include <kllvm/binary/serializer.h>

#include "runtime/header.h"

#include <cassert>
#include <cstdio>
#include <set>

using namespace kllvm;

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
    writer *file, block *subject, const char *sort, bool isVar) {
  /* uint8_t isConstant = ((uintptr_t)subject) & 3; */
  /* if (isConstant) { */
  /*   uint32_t tag = ((uintptr_t)subject) >> 32; */
  /*   if (isConstant == 3) { */
  /*     // bound variable */
  /*     printConfigurationInternal( */
  /*         file, boundVariables[boundVariables.size() - 1 - tag], sort, true); */
  /*     return; */
  /*   } */
  /*   const char *symbol = getSymbolNameForTag(tag); */
  /*   sfprintf(file, "%s()", symbol); */
  /*   return; */
  /* } */
  /* uint16_t layout = layout(subject); */
  /* if (!layout) { */
  /*   string *str = (string *)subject; */
  /*   size_t len = len(subject); */
  /*   sfprintf(file, "\\dv{%s}(\"", sort); */
  /*   for (size_t i = 0; i < len; ++i) { */
  /*     char c = str->data[i]; */
  /*     switch (c) { */
  /*     case '\\': sfprintf(file, "\\\\"); break; */
  /*     case '"': sfprintf(file, "\\\""); break; */
  /*     case '\n': sfprintf(file, "\\n"); break; */
  /*     case '\t': sfprintf(file, "\\t"); break; */
  /*     case '\r': sfprintf(file, "\\r"); break; */
  /*     case '\f': sfprintf(file, "\\f"); break; */
  /*     default: */
  /*       if ((unsigned char)c >= 32 && (unsigned char)c < 127) { */
  /*         sfprintf(file, "%c", c); */
  /*       } else { */
  /*         sfprintf(file, "\\x%02x", (unsigned char)c); */
  /*       } */
  /*       break; */
  /*     } */
  /*   } */
  /*   if (isVar && !varNames.count(str)) { */
  /*     std::string stdStr = std::string(str->data, len(str)); */
  /*     std::string suffix = ""; */
  /*     while (usedVarNames.count(stdStr + suffix)) { */
  /*       suffix = std::to_string(varCounter++); */
  /*     } */
  /*     stdStr = stdStr + suffix; */
  /*     sfprintf(file, "%s", suffix.c_str()); */
  /*     usedVarNames.insert(stdStr); */
  /*     varNames[str] = suffix; */
  /*   } else if (isVar) { */
  /*     sfprintf(file, "%s", varNames[str].c_str()); */
  /*   } */
  /*   sfprintf(file, "\")"); */
  /*   return; */
  /* } */
  /* uint32_t tag = tag_hdr(subject->h.hdr); */
  /* bool isBinder = isSymbolABinder(tag); */
  /* if (isBinder) { */
  /*   boundVariables.push_back( */
  /*       *(block **)(((char *)subject) + sizeof(blockheader))); */
  /* } */
  /* const char *symbol = getSymbolNameForTag(tag); */
  /* std::string symbolStr(symbol); */
  /* if (symbolStr.rfind("inj{", 0) == 0) { */
  /*   std::string prefix = symbolStr.substr(0, symbolStr.find_first_of(',')); */
  /*   sfprintf(file, "%s, %s}(", prefix.c_str(), sort); */
  /* } else { */
  /*   sfprintf(file, "%s(", symbol); */
  /* } */

  /* visitor callbacks */
  /*     = {printConfigurationInternal, */
  /*        printMap, */
  /*        printList, */
  /*        printSet, */
  /*        printInt, */
  /*        printFloat, */
  /*        printBool, */
  /*        printStringBuffer, */
  /*        printMInt, */
  /*        printComma}; */

  /* visitChildren(subject, file, &callbacks); */

  /* if (isBinder) { */
  /*   boundVariables.pop_back(); */
  /* } */
  /* sfprintf(file, ")"); */
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
