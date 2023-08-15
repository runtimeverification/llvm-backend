#include <algorithm>
#include <cassert>
#include <cinttypes>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <gmp.h>
#include <iconv.h>
#include <iomanip>
#include <mpfr.h>
#include <sstream>
#include <stdexcept>
#include <string>

#include "runtime/alloc.h"
#include "runtime/header.h"

extern "C" {

#define KCHAR char

mpz_ptr move_int(mpz_t);
floating *move_float(floating *);

string *allocStringCopy(string *, size_t);
string *hook_BYTES_concat(string *a, string *b);
mpz_ptr hook_BYTES_length(string *a);
string *hook_BYTES_substr(string *a, mpz_t start, mpz_t end);
char *getTerminatedString(string *str);

bool hook_STRING_gt(SortString a, SortString b) {
  auto res = memcmp(a->data, b->data, std::min(len(a), len(b)));
  return res > 0 || (res == 0 && len(a) > len(b));
}

bool hook_STRING_ge(SortString a, SortString b) {
  auto res = memcmp(a->data, b->data, std::min(len(a), len(b)));
  return (res > 0 || (res == 0 && len(a) >= len(b)));
}

bool hook_STRING_lt(SortString a, SortString b) {
  auto res = memcmp(a->data, b->data, std::min(len(a), len(b)));
  return res < 0 || (res == 0 && len(a) < len(b));
}

bool hook_STRING_le(SortString a, SortString b) {
  auto res = memcmp(a->data, b->data, std::min(len(a), len(b)));
  return (res < 0 || (res == 0 && len(a) <= len(b)));
}

bool hook_STRING_eq(SortString a, SortString b) {
  if (is_variable_block(a) || is_variable_block(b)) {
    return a == b;
  }
  if (a->h.hdr & VARIABLE_BIT || b->h.hdr & VARIABLE_BIT) {
    return a == b;
  }
  auto res = memcmp(a->data, b->data, std::min(len(a), len(b)));
  return (res == 0 && len(a) == len(b));
}

bool hook_STRING_ne(SortString a, SortString b) {
  auto res = memcmp(a->data, b->data, std::min(len(a), len(b)));
  return (res != 0 || len(a) != len(b));
}

SortString hook_STRING_concat(SortString a, SortString b) {
  return hook_BYTES_concat(a, b);
}

SortInt hook_STRING_length(SortString a) {
  return hook_BYTES_length(a);
}

static inline uint64_t gs(mpz_t i) {
  if (!mpz_fits_ulong_p(i)) {
    KLLVM_HOOK_INVALID_ARGUMENT("Arg too large for int64_t");
  }
  return mpz_get_ui(i);
}

SortString hook_STRING_chr(SortInt ord) {
  uint64_t uord = gs(ord);
  if (uord > 255) {
    KLLVM_HOOK_INVALID_ARGUMENT("Ord must be <= 255: {}", uord);
  }
  auto ret
      = static_cast<string *>(koreAllocToken(sizeof(string) + sizeof(KCHAR)));
  set_len(ret, 1);
  ret->data[0] = static_cast<KCHAR>(uord);
  return ret;
}

SortInt hook_STRING_ord(SortString input) {
  mpz_t result;
  if (len(input) != 1) {
    KLLVM_HOOK_INVALID_ARGUMENT(
        "Input must be a string of length 1: {}", std::string(input->data));
  }
  mpz_init_set_ui(result, static_cast<unsigned char>(input->data[0]));
  return move_int(result);
}

SortString hook_STRING_substr(SortString input, SortInt start, SortInt end) {
  return hook_BYTES_substr(input, start, end);
}

SortInt hook_STRING_find(SortString haystack, SortString needle, SortInt pos) {
  mpz_t result;
  uint64_t upos = gs(pos);
  if (upos >= len(haystack)) {
    mpz_init_set_si(result, -1);
    return move_int(result);
  }
  auto out = std::search(
      haystack->data + upos * sizeof(KCHAR),
      haystack->data + len(haystack) * sizeof(KCHAR), needle->data,
      needle->data + len(needle) * sizeof(KCHAR));
  int64_t ret = (out - haystack->data) / sizeof(KCHAR);
  // search returns the end of the range if it is not found, but we want -1 in
  // such a case.
  auto res = (ret < len(haystack)) ? ret : -1;
  mpz_init_set_si(result, res);
  return move_int(result);
}

SortInt hook_STRING_rfind(SortString haystack, SortString needle, SortInt pos) {
  // The semantics of rfind uposition are strange, it is the last position at
  // which the match can _start_, which means the end of the haystack needs to
  // be upos + len(needle), or the end of the haystack, if that's less.
  mpz_t result;
  uint64_t upos = gs(pos);
  upos += len(needle);
  auto end = (upos < len(haystack)) ? upos : len(haystack);
  auto out = std::find_end(
      &haystack->data[0], &haystack->data[end], &needle->data[0],
      &needle->data[len(needle)]);
  auto ret = &*out - &haystack->data[0];
  auto res = (ret < end) ? ret : -1;
  mpz_init_set_si(result, res);
  return move_int(result);
}

SortInt
hook_STRING_findChar(SortString haystack, SortString needle, SortInt pos) {
  mpz_t result;
  uint64_t upos = gs(pos);
  if (upos >= len(haystack)) {
    mpz_init_set_si(result, -1);
    return move_int(result);
  }
  auto out = std::find_first_of(
      haystack->data + upos * sizeof(KCHAR),
      haystack->data + len(haystack) * sizeof(KCHAR), needle->data,
      needle->data + len(needle) * sizeof(KCHAR));
  int64_t ret = (out - haystack->data) / sizeof(KCHAR);
  // search returns the end of the range if it is not found, but we want -1 in
  // such a case.
  auto res = (ret < len(haystack)) ? ret : -1;
  mpz_init_set_si(result, res);
  return move_int(result);
}

SortInt
hook_STRING_rfindChar(SortString haystack, SortString needle, SortInt pos) {
  // The semantics of rfind uposition are strange, it is the last position at
  // which the match can _start_, which means the end of the haystack needs to
  // be upos + len(needle), or the end of the haystack, if that's less.
  mpz_t result;
  uint64_t upos = gs(pos);
  upos += 1;
  auto end = (upos < len(haystack)) ? upos : len(haystack);
  auto out = std::find_first_of(
      std::reverse_iterator<const char *>(&haystack->data[end]),
      std::reverse_iterator<const char *>(&haystack->data[0]), &needle->data[0],
      &needle->data[len(needle)]);
  auto ret = &*out - &haystack->data[0];
  auto res = (ret < end) ? ret : -1;
  mpz_init_set_si(result, res);
  return move_int(result);
}

string *makeString(const KCHAR *input, ssize_t len = -1) {
  if (len == -1) {
    len = strlen(input);
  }
  auto ret = static_cast<string *>(koreAllocToken(sizeof(string) + len));
  memcpy(ret->data, input, len);
  set_len(ret, len);
  return ret;
}

char *getTerminatedString(string *str) {
  int length = len(str);
  string *buf
      = static_cast<string *>(koreAllocToken(sizeof(string) + (length + 1)));
  memcpy(buf->data, str->data, length);
  set_len(buf, length + 1);
  buf->data[length] = '\0';
  return buf->data;
}

SortString hook_STRING_base2string_long(SortInt input, uint64_t base) {
  auto str = intToStringInBase(input, base);

  // Include the null terminator in size calculations relating to allocation,
  // but not when setting the length of the string object itself. Any minus
  // signs will have been accounted for already by the intToString call.
  auto str_len = str.size() + 1;
  auto result = static_cast<string *>(koreAllocToken(sizeof(string) + str_len));
  strncpy(result->data, str.c_str(), str_len);
  set_len(result, str.size());

  return static_cast<string *>(koreResizeLastAlloc(
      result, sizeof(string) + len(result), sizeof(string) + str_len));
}

SortInt hook_STRING_string2base_long(SortString input, uint64_t base) {
  mpz_t result;
  size_t length;
  const char *dataStart;

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
    KLLVM_HOOK_INVALID_ARGUMENT(
        "Not a valid integer: {}", std::string(input->data));
  }
  return move_int(result);
}

SortString hook_STRING_int2string(SortInt input) {
  return hook_STRING_base2string_long(input, 10);
}

SortInt hook_STRING_string2int(SortString input) {
  return hook_STRING_string2base_long(input, 10);
}

SortInt hook_STRING_string2base(SortString input, SortInt base) {
  uint64_t ubase = gs(base);
  return hook_STRING_string2base_long(input, ubase);
}

SortString hook_STRING_base2string(SortInt input, SortInt base) {
  uint64_t ubase = gs(base);
  return hook_STRING_base2string_long(input, ubase);
}

SortString hook_STRING_float2string(SortFloat input) {
  std::string result = floatToString(input);
  return makeString(result.c_str());
}

SortFloat hook_STRING_string2float(SortString input) {
  floating result[1];
  init_float2(result, std::string(input->data, len(input)));
  return move_float(result);
}

string *hook_STRING_string2token(SortString input) {
  return input;
}

SortString hook_STRING_token2string(string *input) {
  auto in_block = (block *)input;
  if (is_injection(in_block)) {
    input = (string *)strip_injection(in_block);
  }

  if (get_layout(input) != 0) {
    KLLVM_HOOK_INVALID_ARGUMENT(
        "token2string: input is not a string token: {}",
        std::string(input->data));
  }
  return input;
}

inline SortString hook_STRING_replace(
    SortString haystack, SortString needle, SortString replacer,
    SortInt occurences) {
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
    ++pos;
    ++i;
  }
  if (i == 0) {
    return haystack;
  }
  auto diff = len(needle) - len(replacer);
  size_t new_len = len(haystack) - i * diff;
  auto ret = static_cast<string *>(
      koreAllocToken(sizeof(string) + new_len * sizeof(KCHAR)));
  set_len(ret, new_len);
  int m = 0;
  for (size_t r = 0, h = 0; r < new_len;) {
    if (m >= i) {
      memcpy(ret->data + r, haystack->data + h, new_len - r);
      break;
    } else if (r < matches[m] - diff * m) {
      auto size = matches[m] - diff * m - r;
      memcpy(ret->data + r, haystack->data + h, size);
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

SortString hook_STRING_replaceAll(
    SortString haystack, SortString needle, SortString replacer) {
  // It's guaranteed that there can be no more replacements than the length of
  // the haystack, so this gives us the functionality of replaceAll.
  mpz_t arg;
  mpz_init_set_si(arg, len(haystack));
  return hook_STRING_replace(haystack, needle, replacer, arg);
}

SortString hook_STRING_replaceFirst(
    SortString haystack, SortString needle, SortString replacer) {
  mpz_t arg;
  mpz_init_set_si(arg, 1);
  return hook_STRING_replace(haystack, needle, replacer, arg);
}

SortInt
hook_STRING_countAllOccurrences(SortString haystack, SortString needle) {
  auto pos = &haystack->data[0];
  auto end = &haystack->data[len(haystack)];
  int i = 0;
  while (true) {
    pos = std::search(pos, end, &needle->data[0], &needle->data[len(needle)]);
    if (pos == end) {
      break;
    }
    ++pos;
    ++i;
  }
  mpz_t result;
  mpz_init_set_ui(result, i);
  return move_int(result);
}

SortString hook_STRING_transcode(
    SortString input, SortString inputCharset, SortString outputCharset) {
  iconv_t converter = iconv_open(
      getTerminatedString(outputCharset), getTerminatedString(inputCharset));
  char *inbuf = input->data;
  size_t inbytesleft = len(input);
  size_t outbytesleft = inbytesleft * 4;
  char *buf = (char *)malloc(outbytesleft);
  char *outbuf = buf;
  size_t result
      = iconv(converter, &inbuf, &inbytesleft, &outbuf, &outbytesleft);
  if (result < 0) {
    KLLVM_HOOK_INVALID_ARGUMENT("transcoding failed: STRING.transcode");
  }
  *outbuf = 0;
  return makeString(buf, len(input) * 4 - outbytesleft);
}

string *hook_STRING_uuid() {
  KLLVM_HOOK_INVALID_ARGUMENT("not implemented: STRING.uuid");
}

string *hook_STRING_category(string *str) {
  KLLVM_HOOK_INVALID_ARGUMENT("not implemented: STRING.category");
}

string *hook_STRING_directionality(string *str) {
  KLLVM_HOOK_INVALID_ARGUMENT("not implemented: STRING.directionality");
}

string *hook_STRING_floatFormat(string *str, string *fmt) {
  KLLVM_HOOK_INVALID_ARGUMENT("not implemented: STRING.floatFormat");
}

SortStringBuffer hook_BUFFER_empty() {
  auto result = static_cast<stringbuffer *>(koreAlloc(sizeof(stringbuffer)));
  set_len(result, sizeof(stringbuffer) - sizeof(blockheader));
  result->strlen = 0;
  auto str = static_cast<string *>(koreAllocToken(sizeof(string) + 16));
  set_len(str, 16);
  result->contents = str;
  return result;
}

SortStringBuffer hook_BUFFER_concat(SortStringBuffer buf, SortString s) {
  return hook_BUFFER_concat_raw(buf, s->data, len(s));
}

stringbuffer *
hook_BUFFER_concat_raw(stringbuffer *buf, char const *data, uint64_t n) {
  uint64_t newCapacity = len(buf->contents);
  uint64_t minCapacity = buf->strlen + n;
  uint64_t notYoungObjectBit = buf->h.hdr & NOT_YOUNG_OBJECT_BIT;
  if (newCapacity < minCapacity) {
    newCapacity = len(buf->contents) * 2 + 2;
    if (newCapacity < minCapacity) {
      newCapacity = minCapacity;
    }
    string *new_contents;
    if (notYoungObjectBit) {
      assert(buf->h.hdr & AGE_MASK);
      new_contents = static_cast<string *>(
          koreAllocTokenOld(sizeof(string) + newCapacity));
    } else {
      new_contents
          = static_cast<string *>(koreAllocToken(sizeof(string) + newCapacity));
    }
    memcpy(new_contents->data, buf->contents->data, buf->strlen);
    buf->contents = new_contents;
  }
  memcpy(buf->contents->data + buf->strlen, data, n);
  buf->strlen += n;
  set_len(buf->contents, newCapacity);
  return buf;
}

SortString hook_BUFFER_toString(SortStringBuffer buf) {
  return allocStringCopy(buf->contents, buf->strlen);
}
}

void init_float2(floating *result, std::string contents) {
  size_t prec, exp;
  const char last = contents.back();
  if (last == 'f' || last == 'F') {
    prec = 24;
    exp = 8;
  } else {
    size_t has_prec = contents.find_first_of("pP");
    if (has_prec == std::string::npos) {
      prec = 53;
      exp = 11;
    } else {
      size_t exp_idx = contents.find_first_of("xX");
      std::string prec_str = contents.substr(has_prec + 1, exp_idx - has_prec);
      std::string exp_str = contents.substr(exp_idx + 1);
      prec = atoll(prec_str.c_str());
      exp = atoll(exp_str.c_str());
    }
  }
  result->exp = exp;
  mpfr_init2(result->f, prec);
  int retValue;
  if (contents == "+Infinity" || contents == "-Infinity"
      || contents == "Infinity") {
    retValue = mpfr_set_str(result->f, contents.c_str(), 10, MPFR_RNDN);
  } else {
    size_t last = contents.find_last_of("fFdDpP");
    std::string str_value = contents.substr(0, last);
    retValue = mpfr_set_str(result->f, str_value.c_str(), 10, MPFR_RNDN);
  }
  if (retValue != 0) {
    KLLVM_HOOK_INVALID_ARGUMENT("Can't convert to float: {}", contents);
  }
}
