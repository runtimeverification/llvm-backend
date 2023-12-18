#include <cinttypes>
#include <cstdio>
#include <cstring>
#include <string>

#include "runtime/alloc.h"
#include "runtime/header.h"

std::string floatToString(const floating *f, const char *suffix) {
  if (mpfr_nan_p(f->f)) {
    return "NaN" + std::string(suffix);
  } else if (mpfr_inf_p(f->f)) {
    if (mpfr_signbit(f->f)) {
      return "-Infinity" + std::string(suffix);
    } else {
      return "Infinity" + std::string(suffix);
    }
  } else {
    mpfr_exp_t printed_exp;
    char *str = mpfr_get_str(nullptr, &printed_exp, 10, 0, f->f, MPFR_RNDN);
    size_t len = strlen(str);
    string *newstr = (string *)koreAllocToken(sizeof(string) + len + 2);
    init_with_len(newstr, len + 2);
    size_t idx = 0;
    if (str[0] == '-') {
      newstr->data[0] = '-';
      idx = 1;
    }
    newstr->data[idx] = '0';
    newstr->data[idx + 1] = '.';
    strncpy(newstr->data + idx + 2, str + idx, len - idx + 1);
    newstr->data[len + 2] = '\0';
    return std::string(newstr->data) + "e" + std::to_string(printed_exp)
           + suffix;
  }
}

std::string floatToString(const floating *f) {
  uint64_t prec = mpfr_get_prec(f->f);
  uint64_t exp = f->exp;
  char suffix[41]; // 19 chars per long + p and x and null byte
  if (prec == 53 && exp == 11) {
    suffix[0] = 0;
  } else if (prec == 24 && exp == 8) {
    suffix[0] = 'f';
    suffix[1] = 0;
  } else {
    snprintf(suffix, sizeof(suffix), "p%" PRIu64 "x%" PRIu64, prec, exp);
  }
  return floatToString(f, suffix);
}

std::string intToStringInBase(mpz_t i, uint64_t base) {
  char *tmp = mpz_get_str(nullptr, base, i);
  auto ret = std::string(tmp);

  void (*mpz_free)(void *, size_t);
  mp_get_memory_functions(nullptr, nullptr, &mpz_free);
  mpz_free(tmp, strlen(tmp) + 1);

  return ret;
}

std::string intToString(mpz_t i) {
  return intToStringInBase(i, 10);
}
