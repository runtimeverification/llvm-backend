#include <array>
#include <cinttypes>
#include <cstdio>
#include <cstring>
#include <string>

#include "runtime/alloc.h"
#include "runtime/header.h"

std::string float_to_string(floating const *f, char const *suffix) {
  if (mpfr_nan_p(f->f)) {
    return "NaN" + std::string(suffix);
  }
  if (mpfr_inf_p(f->f)) {
    if (mpfr_signbit(f->f)) {
      return "-Infinity" + std::string(suffix);
    }
    return "Infinity" + std::string(suffix);
  }
  mpfr_exp_t printed_exp = 0;
  char *str = mpfr_get_str(nullptr, &printed_exp, 10, 0, f->f, MPFR_RNDN);
  size_t len = strlen(str);
  auto *newstr = (string *)kore_alloc_token(sizeof(string) + len + 2);
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
  return std::string(newstr->data) + "e" + std::to_string(printed_exp) + suffix;
}

std::string float_to_string(floating const *f) {
  uint64_t prec = mpfr_get_prec(f->f);
  uint64_t exp = f->exp;

  auto suffix = [&]() -> std::string {
    if (prec == 53 && exp == 11) {
      return "";
    }

    if (prec == 24 && exp == 8) {
      return "f";
    }

    return fmt::sprintf("p%" PRIu64 "x%" PRIu64, prec, exp);
  }();

  return float_to_string(f, suffix.c_str());
}

std::string int_to_string_in_base(mpz_t i, uint64_t base) {
  char *tmp = mpz_get_str(nullptr, base, i);
  auto ret = std::string(tmp);

  void (*mpz_free)(void *, size_t) = nullptr;
  mp_get_memory_functions(nullptr, nullptr, &mpz_free);
  mpz_free(tmp, strlen(tmp) + 1);

  return ret;
}

std::string int_to_string(mpz_t i) {
  return int_to_string_in_base(i, 10);
}
