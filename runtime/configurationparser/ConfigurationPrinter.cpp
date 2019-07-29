#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>
#include <unordered_map>
#include <set>
#include <functional>

#include "runtime/header.h"
#include "runtime/alloc.h"

void printInt(FILE *file, mpz_t i, const char *sort) {
  char *str = mpz_get_str(NULL, 10, i);
  fprintf(file, "\\dv{%s}(\"%s\")", sort, str);
}

void printFloat(FILE *file, floating *f, const char *sort) {
  std::string str = floatToString(f);
  fprintf(file, "\\dv{%s}(\"%s\")", sort, str.c_str());
}

void printBool(FILE *file, bool b, const char *sort) {
  const char *str = b ? "true" : "false";
  fprintf(file, "\\dv{%s}(\"%s\")", sort, str);
}

void printStringBuffer(FILE *file, stringbuffer *b, const char *sort) {
  std::string str(b->contents->data, b->strlen);
  fprintf(file, "\\dv{%s}(\"%s\")", sort, str.c_str());
}

void printMInt(FILE *file, void *i, const char *sort) {
  //TODO: print mint
  abort();
}

void printComma(FILE *file) {
  fprintf(file, ",");
}

struct StringHash {
  size_t operator() (string * const& k) const {
    return std::hash<std::string>{}(std::string(k->data, len(k)));
  }
};

struct StringEq {
  bool operator() (string * const& lhs, string * const& rhs) const {
    return hook_STRING_eq(lhs, rhs);
  }
};

static thread_local std::vector<block *> boundVariables;
static thread_local std::unordered_map<string *, std::string, StringHash, StringEq> varNames;
static thread_local std::set<std::string> usedVarNames;
static thread_local uint64_t varCounter = 0;

void printConfigurationInternal(FILE *file, block *subject, const char *sort, bool isVar) {
  uint8_t isConstant = ((uintptr_t)subject) & 3;
  if (isConstant) {
    uint32_t tag = ((uintptr_t)subject) >> 32;
    if (isConstant == 3) {
      // bound variable
      printConfigurationInternal(file, boundVariables[boundVariables.size()-1-tag], sort, true);
      return;
    }
    const char *symbol = getSymbolNameForTag(tag);
    fprintf(file, "%s()", symbol);
    return;
  }
  uint16_t layout = layout(subject);
  if (!layout) {
    string *str = (string *)subject;
    size_t len = len(subject);
    fprintf(file, "\\dv{%s}(\"", sort);
    for (size_t i = 0; i < len; ++i) {
      char c = str->data[i];
      switch(c) {
      case '\\':
        fprintf(file, "\\\\");
        break;
      case '"':
        fprintf(file, "\\\"");
        break;
      case '\n':
        fprintf(file, "\\\n");
        break;
      case '\t':
        fprintf(file, "\\\t");
        break;
      case '\r':
        fprintf(file, "\\\r");
        break;
      case '\f':
        fprintf(file, "\\f");
        break;
      default:
        if ((unsigned char)c >= 32 && (unsigned char)c < 127) {
          fprintf(file, "%c", c);
        } else {
          fprintf(file, "\\%02x", (unsigned char)c);
        }
        break;
      }
    }
    if (isVar && !varNames.count(str)) {
      std::string stdStr = std::string(str->data, len(str));
      std::string suffix = "";
      while (usedVarNames.count(stdStr + suffix)) {
        suffix = std::to_string(varCounter++);
      }
      stdStr = stdStr + suffix;
      fprintf(file, "%s", suffix.c_str());
      usedVarNames.insert(stdStr);
      varNames[str] = suffix;
    } else if (isVar) {
      fprintf(file, "%s", varNames[str].c_str());
    }
    fprintf(file, "\")");
    return;
  }
  uint32_t tag = tag_hdr(subject->h.hdr);
  bool isBinder = isSymbolABinder(tag);
  if (isBinder) {
    boundVariables.push_back(*(block **)(((char *)subject) + sizeof(blockheader)));
  }
  const char *symbol = getSymbolNameForTag(tag);
  fprintf(file, "%s(", symbol);
  visitChildren(subject, file, printConfigurationInternal, printMap, printList, printSet, printInt, printFloat,
      printBool, printStringBuffer, printMInt, printComma);
  if (isBinder) {
    boundVariables.pop_back();
  }
  fprintf(file, ")");
}

void printConfiguration(const char *filename, block *subject) {
  FILE *file = fopen(filename, "w");
  boundVariables.clear();
  varCounter = 0;
  printConfigurationInternal(file, subject, nullptr, false);
  varNames.clear();
  usedVarNames.clear();
  fclose(file);
}

