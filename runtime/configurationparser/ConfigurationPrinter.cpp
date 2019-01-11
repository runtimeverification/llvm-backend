#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cinttypes>

#include "runtime/header.h"
#include "runtime/alloc.h"

void printInt(FILE *file, mpz_t i, const char *sort) {
  char *str = mpz_get_str(NULL, 10, i);
  fprintf(file, "\\dv{%s}(\"%s\")", sort, str);
}

void printFloat(FILE *file, floating *f, const char *sort) {
  uint64_t prec = mpfr_get_prec(f->f);
  uint64_t exp = f->exp;
  char suffix[41]; // 19 chars per long + p and x and null byte
  if (prec == 53 && exp == 11) {
    suffix[0] = 0;
  } else if (prec == 24 && exp == 8) {
    suffix[0] = 'f';
    suffix[1] = 0;
  } else {
    sprintf(suffix, "p%" PRIu64 "x%" PRIu64, prec, exp);
  }
  if (mpfr_nan_p(f->f)) {
    fprintf(file, "\\dv{%s}(\"NaN%s\")", sort, suffix);
  } else if (mpfr_inf_p(f->f)) {
    if (mpfr_signbit(f->f)) {
      fprintf(file, "\\dv{%s}(\"-Infinity%s\")", sort, suffix);
    } else {
      fprintf(file, "\\dv{%s}(\"Infinity%s\")", sort, suffix);
    }
  } else {
    mpfr_exp_t printed_exp;
    char *str = mpfr_get_str(NULL, &printed_exp, 10, 0, f->f, MPFR_RNDN);
    size_t len = strlen(str);
    char *newstr = (char *)koreAllocOld(len+2);
    size_t idx = 0;
    if (str[0] == '-') {
      newstr[0] = '-';
      idx = 1;
    }
    newstr[idx] = '0';
    newstr[idx+1] = '.';
    strcpy(newstr+idx+2,str+idx);
    fprintf(file, "\\dv{%s}(\"%se%ld%s\")", sort, newstr, printed_exp, suffix);
  }
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

