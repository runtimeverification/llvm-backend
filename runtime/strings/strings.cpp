#include<gmp.h>
#include<mpfr.h>
#include<algorithm>
#include<cassert>
#include<cinttypes>
#include<cstdlib>
#include<cstdint>
#include<cstring>
#include<iomanip>
#include<string>
#include<sstream>
#include<stdexcept>

#include "runtime/alloc.h"
#include "runtime/header.h"

extern "C" {

#define KCHAR char

  mpz_ptr move_int(mpz_t);
  floating *move_float(floating *);

  string *bytes2string(string *, size_t);
  string *hook_BYTES_concat(string *a, string *b);
  mpz_ptr hook_BYTES_length(string *a);
  string *hook_BYTES_substr(string *a, mpz_t start, mpz_t end);

  bool hook_STRING_gt(const string * a, const string * b) {
    auto res = memcmp(a->data, b->data, std::min(len(a), len(b)));
    return res > 0 || (res == 0 && len(a) > len(b));
  }

  bool hook_STRING_ge(const string * a, const string * b) {
    auto res = memcmp(a->data, b->data, std::min(len(a), len(b)));
    return (res > 0 || (res == 0 && len(a) >= len(b)));
  }

  bool hook_STRING_lt(const string * a, const string * b) {
    auto res = memcmp(a->data, b->data, std::min(len(a), len(b)));
    return res < 0 || (res == 0 && len(a) < len(b));
  }

  bool hook_STRING_le(const string * a, const string * b) {
    auto res = memcmp(a->data, b->data, std::min(len(a), len(b)));
    return (res < 0 || (res == 0 && len(a) <= len(b)));
  }

  bool hook_STRING_eq(const string * a, const string * b) {
    uintptr_t aint = (uintptr_t) a;
    uintptr_t bint = (uintptr_t) b;
    if ((aint & 3) == 3 || (bint & 3) == 3) {
      return a == b;
    }
    if (a->h.hdr & VARIABLE_BIT || b->h.hdr & VARIABLE_BIT) {
      return a == b;
    }
    auto res = memcmp(a->data, b->data, std::min(len(a), len(b)));
    return (res == 0 && len(a) == len(b));
  }

  bool hook_STRING_ne(const string * a, const string * b) {
    auto res = memcmp(a->data, b->data, std::min(len(a), len(b)));
    return (res != 0 || len(a) != len(b));
  }

  string * hook_STRING_concat(string * a, string * b) {
    return hook_BYTES_concat(a, b);
  }

  mpz_ptr hook_STRING_length(string * a) {
    return hook_BYTES_length(a);
  }

  static inline uint64_t gs(mpz_t i) {
    if (!mpz_fits_ulong_p(i)) {
      throw std::invalid_argument("Arg too large for int64_t");
    }
    return mpz_get_ui(i);
  }

  const string * hook_STRING_chr(mpz_t ord) {
    uint64_t uord = gs(ord);
    if (uord > 255) {
      throw std::invalid_argument("Ord must be <= 255");
    }
    auto ret = static_cast<string *>(koreAllocToken(sizeof(string) + sizeof(KCHAR)));
    set_len(ret, 1);
    ret->data[0] = static_cast<KCHAR>(uord);
    return ret;
  }

  const mpz_ptr hook_STRING_ord(const string * input) {
    mpz_t result;
    if (len(input) != 1) {
      throw std::invalid_argument("Input must a string of length 1");
    }
    mpz_init_set_ui(result, static_cast<unsigned char>(input->data[0]));
    return move_int(result);
  }

  string * hook_STRING_substr(string * input, mpz_t start, mpz_t end) {
    return hook_BYTES_substr(input, start, end);
  }

  mpz_ptr hook_STRING_find(const string * haystack, const string * needle, mpz_t pos) {
    mpz_t result;
    uint64_t upos = gs(pos);
    if (upos >= len(haystack)) {
      mpz_init_set_si(result, -1);
      return move_int(result);
    }
    auto out = std::search(haystack->data + upos * sizeof(KCHAR), haystack->data + len(haystack) * sizeof(KCHAR),
        needle->data,   needle->data   + len(needle) * sizeof(KCHAR));
    int64_t ret = (out - haystack->data) / sizeof(KCHAR);
    // search returns the end of the range if it is not found, but we want -1 in such a case.
    auto res = (ret < len(haystack))?ret:-1;
    mpz_init_set_si(result, res);
    return move_int(result);
  }

  mpz_ptr hook_STRING_rfind(const string * haystack, const string * needle, mpz_t pos) {
    // The semantics of rfind uposition are strange, it is the last position at which
    // the match can _start_, which means the end of the haystack needs to be upos + len(needle),
    // or the end of the haystack, if that's less.
    mpz_t result;
    uint64_t upos = gs(pos);
    upos += len(needle);
    auto end = (upos < len(haystack))?upos:len(haystack);
    auto out = std::find_end(&haystack->data[0], &haystack->data[end],
        &needle->data[0], &needle->data[len(needle)]);
    auto ret = &*out - &haystack->data[0];
    auto res = (ret < end)?ret:-1;
    mpz_init_set_si(result, res);
    return move_int(result);
  }

  mpz_ptr hook_STRING_findChar(const string * haystack, const string * needle, mpz_t pos) {
    if (len(needle) > 1) {
      mpz_t result;
      mpz_init_set_si(result, -1);
      return move_int(result);
    }
    return hook_STRING_find(haystack, needle, pos);
  }

  mpz_ptr hook_STRING_rfindChar(const string * haystack, const string * needle, mpz_ptr pos) {
    if (len(needle) > 1) {
      mpz_t result;
      mpz_init_set_si(result, -1);
      return move_int(result);
    }
    return hook_STRING_rfind(haystack, needle, pos);
  }

  string * makeString(const KCHAR * input, ssize_t len = -1) {
    if (len == -1) {
      len = strlen(input);
    }
    auto ret = static_cast<string *>(koreAllocToken(sizeof(string) + len));
    memcpy(ret->data, input, len);
    set_len(ret, len);
    return ret;
  }

  const string *hook_STRING_base2string_long(const mpz_t input, uint64_t base) {
    size_t len = mpz_sizeinbase(input, base) + 2;
    // +1 for null terminator needed by mpz_get_str, +1 for minus sign
    auto result = static_cast<string *>(koreAllocToken(sizeof(string) + len));
    mpz_get_str(result->data, base, input);
    set_len(result, strlen(result->data));
    return static_cast<string *>(koreResizeLastAlloc(result, sizeof(string) + len(result), sizeof(string) + len));
  }

  const mpz_ptr hook_STRING_string2base_long(const string *input, uint64_t base) {
    mpz_t result;
    size_t length;
    const char * dataStart;

    if (*(input->data) == '+') {
      length = len(input) - 1;
      dataStart = input->data + 1;
    } else {
      length = len(input);
      dataStart = input->data;
    }

    auto copy = static_cast<char *>(koreAllocToken(length + 1));
    memcpy(copy, dataStart, length);
    copy[length] = 0;
    if (mpz_init_set_str(result, copy, base)) {
      throw std::invalid_argument("Not a valid integer");
    }
    return move_int(result);
  }

  const string * hook_STRING_int2string(const mpz_t input) {
    return hook_STRING_base2string_long(input, 10);
  }

  const mpz_ptr hook_STRING_string2int(const string *input) {
    return hook_STRING_string2base_long(input, 10);
  }

  const mpz_ptr hook_STRING_string2base(const string *input, mpz_t base) {
    uint64_t ubase = gs(base);
    return hook_STRING_string2base_long(input, ubase);
  }

  const string *hook_STRING_base2string(mpz_t input, mpz_t base) {
    uint64_t ubase = gs(base);
    return hook_STRING_base2string_long(input, ubase);
  }

  const string * hook_STRING_float2string(const floating *input) {
    std::string result = floatToString(input);
    return makeString(result.c_str());
  }

  floating *hook_STRING_string2float(const string * input) {
    floating result[1];
    init_float2(result, std::string(input->data, len(input)));
    return move_float(result);
  }

  const string * hook_STRING_string2token(const string * input) {
    return input;
  }

  const string * hook_STRING_token2string(const string * input) {
    return input;
  }

  inline const string * hook_STRING_replace(const string * haystack, const string * needle, const string * replacer, mpz_t occurences) {
    uint64_t uoccurences = gs(occurences);
    auto start = &haystack->data[0];
    auto pos = start;
    auto end = &haystack->data[len(haystack)];
    size_t matches[len(haystack)];
    int i = 0;
    while (i < uoccurences) {
      pos = std::search(pos, end, &needle->data[0], &needle->data[len(needle)]);
      if (pos == end) {
        break;
      }
      matches[i] = (pos - start);
      ++pos; ++i;
    }
    if ( i == 0 ) {
      return haystack;
    }
    auto diff = len(needle) - len(replacer);
    size_t new_len = len(haystack) - i * diff;
    auto ret = static_cast<string *>(koreAllocToken(sizeof(string) + new_len * sizeof(KCHAR)));
    set_len(ret, new_len);
    int m = 0;
    for (size_t r = 0, h = 0; r < new_len;) {
      if (m >= i) {
        memcpy(ret->data+r, haystack->data+h, new_len - r);
        break;
      } else if (r < matches[m] - diff * m) {
        auto size = matches[m] - diff * m - r;
        memcpy(ret->data+r, haystack->data+h, size);
        r += size;
        h += size;
      } else {
        ++m;
        memcpy(&ret->data[r], replacer->data, len(replacer));
        r += len(replacer);
        h += len(needle);
      }
    }
    return ret;
  }

  const string * hook_STRING_replaceAll(const string * haystack, const string * needle, const string * replacer) {
    // It's guaranteed that there can be no more replacements than the length of the haystack, so this
    // gives us the functionality of replaceAll.
    mpz_t arg;
    mpz_init_set_si(arg, len(haystack));
    return hook_STRING_replace(haystack, needle, replacer, arg);
  }

  const string * hook_STRING_replaceFirst(const string * haystack, const string * needle, const string * replacer) {
    mpz_t arg;
    mpz_init_set_si(arg, 1);
    return hook_STRING_replace(haystack, needle, replacer, arg);
  }

  mpz_ptr hook_STRING_countAllOccurrences(const string * haystack, const string * needle) {
    auto pos = &haystack->data[0];
    auto end = &haystack->data[len(haystack)];
    int i = 0;
    while (true) {
      pos = std::search(pos, end, &needle->data[0], &needle->data[len(needle)]);
      if (pos == end) {
        break;
      }
      ++pos; ++i;
    }
    mpz_t result;
    mpz_init_set_ui(result, i);
    return move_int(result);
  }

  string *hook_STRING_uuid() {
    throw std::invalid_argument("not implemented: STRING.uuid");
  }

  string *hook_STRING_category(string * str) {
    throw std::invalid_argument("not implemented: STRING.category");
  }

  string *hook_STRING_directionality(string * str) {
    throw std::invalid_argument("not implemented: STRING.directionality");
  }

  string *hook_STRING_floatFormat(string * str, string * fmt) {
    throw std::invalid_argument("not implemented: STRING.floatFormat");
  }

  stringbuffer *hook_BUFFER_empty() {
    auto result = static_cast<stringbuffer *>(koreAlloc(sizeof(stringbuffer)));
    set_len(result, sizeof(stringbuffer) - sizeof(blockheader));
    result->strlen = 0;
    auto str = static_cast<string *>(koreAllocToken(sizeof(string) + 16));
    set_len(str, 16);
    result->contents = str;
    return result;
  }

  stringbuffer *hook_BUFFER_concat(stringbuffer *buf, string *s) {
    uint64_t newCapacity = len(buf->contents);
    uint64_t minCapacity = buf->strlen + len(s);
    uint64_t notYoungObjectBit = buf->h.hdr & NOT_YOUNG_OBJECT_BIT;
    if (newCapacity < minCapacity) {
      newCapacity = len(buf->contents) * 2 + 2;
      if (newCapacity < minCapacity) {
        newCapacity = minCapacity;
      }
      string* new_contents;
      if (notYoungObjectBit) {
        assert(buf->h.hdr & YOUNG_AGE_BIT);
        new_contents = static_cast<string *>(koreAllocTokenOld(sizeof(string) + newCapacity));
      } else {
        new_contents = static_cast<string *>(koreAllocToken(sizeof(string) + newCapacity));
      }
      memcpy(new_contents->data, buf->contents->data, buf->strlen);
      buf->contents = new_contents;
    }
    memcpy(buf->contents->data + buf->strlen, s->data, len(s));
    buf->strlen += len(s);
    set_len(buf->contents, newCapacity);
    return buf;
  }

  string *hook_BUFFER_toString(stringbuffer *buf) {
    return bytes2string(buf->contents, buf->strlen);
  }
}

void init_float2(floating *result, std::string contents) {
  size_t prec, exp;
  const char last = contents.back();
  if (last == 'f' || last == 'F' || last == 'y') {
    prec = 24;
    exp = 8;
  } else {
    size_t has_prec = contents.find_first_of("pP");
    if (has_prec == std::string::npos) {
       prec = 53;
       exp = 11;
    } else {
      size_t exp_idx = contents.find_first_of("xX");
      std::string prec_str = contents.substr(has_prec+1, exp_idx-has_prec);
      std::string exp_str = contents.substr(exp_idx+1);
      prec = atoll(prec_str.c_str());
      exp = atoll(exp_str.c_str());
    }
  }
  result->exp = exp;
  mpfr_init2(result->f, prec);
  int retValue;
  if (contents == "+Infinity" || contents == "-Infinity") {
    retValue = mpfr_set_str(result->f, contents.c_str(), 10, MPFR_RNDN);
  } else {
    size_t last = contents.find_last_of("fFdDpP");
    std::string str_value = contents.substr(0, last);
    retValue = mpfr_set_str(result->f, str_value.c_str(), 10, MPFR_RNDN);
  }
  if (retValue != 0) {
    throw std::invalid_argument("Can't convert to float");
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
    sprintf(suffix, "p%" PRIu64 "x%" PRIu64, prec, exp);
  }
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
    char *str = mpfr_get_str(NULL, &printed_exp, 10, 0, f->f, MPFR_RNDN);
    size_t len = strlen(str);
    string *newstr = (string *)koreAllocToken(sizeof(string) + len + 2);
    set_len(newstr, len + 2);
    size_t idx = 0;
    if (str[0] == '-') {
      newstr->data[0] = '-';
      idx = 1;
    }
    newstr->data[idx] = '0';
    newstr->data[idx+1] = '.';
    strcpy(newstr->data + idx + 2, str + idx);
    return std::string(newstr->data) + "e" + std::to_string(printed_exp) + suffix;
  }
}
