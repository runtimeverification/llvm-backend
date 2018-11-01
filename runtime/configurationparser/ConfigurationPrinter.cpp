#include <stdio.h>
#include <stdlib.h>

#include "runtime/header.h"

void printInt(FILE *file, mpz_t i, const char *sort) {
  char *str = mpz_get_str(NULL, 10, i);
  fprintf(file, "\\dv{%s}(\"%s\")", sort, str);
}

void printFloat(FILE *file, mpfr_t f, const char *sort) {
  //TODO: print float
  abort();
}

void printBool(FILE *file, bool b, const char *sort) {
  const char *str = b ? "true" : "false";
  fprintf(file, "\\dv{%s}(\"%s\")", sort, str);
}

void printMInt(FILE *file, void *i, const char *sort) {
  //TODO: print mint
  abort();
}

void printComma(FILE *file) {
  fprintf(file, ",");
}

void printConfigurationInternal(FILE *file, block *subject, const char *sort) {
  bool isConstant = ((uintptr_t)subject) & 1;
  if (isConstant) {
    uint32_t tag = ((uintptr_t)subject) >> 32;
    const char *symbol = getSymbolNameForTag(tag);
    fprintf(file, "%s()", symbol);
    return;
  }
  uint16_t layout = subject->h.hdr >> 48;
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
          fprintf(file, "\\%03o", c);
        }
        break;
      }
    }
    fprintf(file, "\")");
    return;
  }
  uint32_t tag = subject->h.hdr & 0xffffffffLL;
  const char *symbol = getSymbolNameForTag(tag);
  fprintf(file, "%s(", symbol);
  visitChildren(subject, file, printConfigurationInternal, printMap, printList, printSet, printInt, printFloat,
      printBool, printMInt, printComma);
  fprintf(file, ")");
}

void printConfiguration(const char *filename, block *subject) {
  FILE *file = fopen(filename, "w");
  printConfigurationInternal(file, subject, nullptr);
  fclose(file);
}

