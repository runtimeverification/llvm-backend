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

#include "kllvm/ast/AST.h"
#include "runtime/alloc.h"
#include "runtime/header.h"

extern "C" {

#define KCHAR char

mpz_ptr move_int(mpz_t);
floating *move_float(floating *);

string *hook_BYTES_concat(string *a, string *b);
string *hook_BYTES_bytes2string(string *);
string *hook_BYTES_string2bytes(string *);
char *getTerminatedString(string *str);

bool hook_STRING_gt(SortString a, SortString b) {
  // UTF-8 ensures lexicographic ordering by bytes aligns
  // with lexicographic ordering by code points
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
  auto ret = hook_BYTES_concat(a, b);
  set_is_bytes(ret, false);
  return ret;
}

static uint64_t num_codepoints(SortString a) {
  uint64_t length = 0;
  for (size_t i = 0; i < len(a); ++i) {
    // every continuation byte is of the form 10xxxxxx,
    // and the number of codepoints is the number of non-continuation bytes
    if ((a->data[i] & 0xC0) != 0x80) {
      length += 1;
    }
  }
  return length;
}

SortInt hook_STRING_length(SortString a) {
  mpz_t result;
  mpz_init_set_ui(result, num_codepoints(a));
  return move_int(result);
}

SortString hook_STRING_chr(SortInt ord) {
  uint64_t uord = get_ui(ord);
  if (0xD800 <= uord && uord <= 0xDFFF) {
    KLLVM_HOOK_INVALID_ARGUMENT(
        "Ord cannot correspond to a surrogate code point in the range [U+D800, "
        "U+DFFF]: {}",
        uord);
  }
  if (uord > 0x10FFFF) {
    KLLVM_HOOK_INVALID_ARGUMENT(
        "Ord exceeds the largest code point U+10FFFF: {}", uord);
  }
  std::string std_ret = kllvm::codepointToUTF8(static_cast<uint32_t>(uord));

  auto ret
      = static_cast<string *>(koreAllocToken(sizeof(string) + std_ret.size()));
  init_with_len(ret, std_ret.size());
  memcpy(ret->data, std_ret.c_str(), std_ret.size());
  return ret;
}

SortInt hook_STRING_ord(SortString input) {
  if (len(input) > 0) {
    auto [codepoint, num_bytes] = kllvm::readCodepoint(input->data);
    if (num_bytes == len(input)) {
      mpz_t result;
      mpz_init_set_ui(result, codepoint);
      return move_int(result);
    }
  }
  KLLVM_HOOK_INVALID_ARGUMENT(
      "Input must be a string of length 1: {}", std::string(input->data));
}

// Convert from a byte-based index to a codepoint-based index.
// Provided idx must be in the range [0, length of str).
//
// Returns whatever codepoint that idx occurs in.
static inline uint64_t byte_idx_to_codepoint(const char *str, uint64_t idx) {
  uint64_t codepoint = 0;
  for (size_t i = 1; i <= idx; ++i) {
    if ((str[i] & 0xC0) != 0x80) {
      ++codepoint;
    }
  }
  return codepoint;
}

// Convert from a codepoint-based index to a byte-based index.
//
// If the codepoint is found, i.e., it is in the range [0, num_codepoints(str, length)),
// we return true and write the byte-based index of the start of the codepoint to *result.
//
// Otherwise, we return false and write num_codepoints(str, length) to *result.
static inline bool codepoint_idx_to_byte(
    const char *str, uint64_t length, uint64_t idx, uint64_t *result) {
  uint64_t codepoint = 0;
  for (uint64_t i = 0; i < length; ++i) {
    if ((str[i] & 0xC0) != 0x80) {
      // codepoint records number of codepoints so far, not the index of the current codepoint,
      // so we do this check before incrementing to account for the off by 1
      if (codepoint == idx) {
        *result = i;
        return true;
      }
      ++codepoint;
    }
  }
  // out of range, so return number of codepoints
  *result = codepoint;
  return false;
}

SortString hook_STRING_substr(SortString input, SortInt start, SortInt end) {
  uint64_t ustart = get_ui(start);
  uint64_t uend = get_ui(end);
  if (uend < ustart) {
    KLLVM_HOOK_INVALID_ARGUMENT(
        "Invalid string slice: Requested start index {} is greater than "
        "requested end index {}.",
        ustart, uend);
  }

  uint64_t start_bytes;
  uint64_t end_bytes;
  if (codepoint_idx_to_byte(input->data, len(input), ustart, &start_bytes)) {
  } else if (ustart == start_bytes) {
    // in the failure case, codepoint_to_idx writes num_codepoints to start_bytes,
    // so this branch is when ustart is one past the end of the string
    start_bytes = len(input);
  } else {
    KLLVM_HOOK_INVALID_ARGUMENT(
        "Invalid string slice for string: Requested start index {} is greater "
        "than string length {}",
        ustart, len(input));
  }

  if (codepoint_idx_to_byte(
          input->data + start_bytes, len(input) - start_bytes, uend - ustart,
          &end_bytes)) {
    end_bytes += start_bytes;
  } else if (uend == ustart + end_bytes) {
    // in the failure case, codepoint_to_idx writes num_codepoints to end_bytes,
    // so this branch is when uend is one past the end of the string
    end_bytes = len(input);
  } else {
    KLLVM_HOOK_INVALID_ARGUMENT(
        "Invalid string slice for string: Requested end index {} is greater "
        "than string length {}",
        uend, len(input));
  }

  uint64_t len_bytes = end_bytes - start_bytes;
  auto ret = static_cast<string *>(
      koreAllocToken(sizeof(string) + sizeof(KCHAR) * len_bytes));
  init_with_len(ret, len_bytes);
  memcpy(&(ret->data), &(input->data[start_bytes]), len_bytes * sizeof(KCHAR));
  return ret;
}

SortInt hook_STRING_find(SortString haystack, SortString needle, SortInt pos) {
  mpz_t result;
  uint64_t upos = get_ui(pos);
  uint64_t upos_bytes;
  if (!codepoint_idx_to_byte(
          haystack->data, len(haystack), upos, &upos_bytes)) {
    mpz_init_set_si(result, -1);
    return move_int(result);
  }

  auto out = std::search(
      haystack->data + upos_bytes * sizeof(KCHAR),
      haystack->data + len(haystack) * sizeof(KCHAR), needle->data,
      needle->data + len(needle) * sizeof(KCHAR));
  uint64_t ret = (out - haystack->data) / sizeof(KCHAR);

  // search returns the end of the range if it is not found, but we want -1 in
  // such a case.
  if (ret >= len(haystack)) {
    mpz_init_set_si(result, -1);
  } else {
    mpz_init_set_ui(
        result, upos
                    + byte_idx_to_codepoint(
                        haystack->data + upos_bytes, ret - upos_bytes));
  }
  return move_int(result);
}

SortInt hook_STRING_rfind(SortString haystack, SortString needle, SortInt pos) {
  // The semantics of rfind uposition are strange, it is the last position at
  // which the match can _start_, which means the end of the match needs to
  // be upos_bytes + len(needle), or the end of the haystack, if that's less.
  mpz_t result;
  uint64_t upos = get_ui(pos);
  uint64_t end_bytes;
  if (codepoint_idx_to_byte(haystack->data, len(haystack), upos, &end_bytes)) {
    end_bytes += len(needle);
    end_bytes = end_bytes < len(haystack) ? end_bytes : len(haystack);
  } else {
    end_bytes = len(haystack);
  }

  // TODO: find_end works with forward iterators and thus searches front to back,
  // so we could be a bit more efficient using std::find with reverse_iterators
  auto out = std::find_end(
      &haystack->data[0], &haystack->data[end_bytes], &needle->data[0],
      &needle->data[len(needle)]);
  uint64_t ret = &*out - &haystack->data[0];

  if (ret >= end_bytes) {
    mpz_init_set_si(result, -1);
  } else {
    mpz_init_set_ui(result, byte_idx_to_codepoint(haystack->data, ret));
  }
  return move_int(result);
}

static std::vector<uint64_t>
string_to_codepoints(const char *str, uint64_t length) {
  std::vector<uint64_t> res;
  res.reserve(length);
  for (uint64_t i = 0; i < length;) {
    auto [codepoint, num_bytes] = kllvm::readCodepoint(str + i);
    res.push_back(codepoint);
    i += num_bytes;
  }
  return res;
}

SortInt
hook_STRING_findChar(SortString haystack, SortString needle, SortInt pos) {
  mpz_t result;
  uint64_t upos = get_ui(pos);
  uint64_t upos_bytes;
  if (!codepoint_idx_to_byte(
          haystack->data, len(haystack), upos, &upos_bytes)) {
    mpz_init_set_si(result, -1);
    return move_int(result);
  }

  std::vector<uint64_t> needle_pts
      = string_to_codepoints(needle->data, len(needle));
  for (uint64_t h = upos_bytes, h_pt = upos; h < len(haystack); ++h_pt) {
    auto [codepoint, num_bytes] = kllvm::readCodepoint(haystack->data + h);
    for (uint64_t n : needle_pts) {
      if (codepoint == n) {
        mpz_init_set_ui(result, h_pt);
        return move_int(result);
      }
    }
    h += num_bytes;
  }

  mpz_init_set_si(result, -1);
  return move_int(result);
}

SortInt
hook_STRING_rfindChar(SortString haystack, SortString needle, SortInt pos) {
  mpz_t result;
  uint64_t upos = get_ui(pos);
  uint64_t end_bytes;
  if (codepoint_idx_to_byte(
          haystack->data, len(haystack), upos + 1, &end_bytes)) {
    // found the start of upos + 1, so subtract 1 byte to get the end of the upos codepoint
    --end_bytes;
  } else {
    // In the failure case, codepoint_idx_to_byte writes num_codepoints to end_bytes
    upos = end_bytes - 1;
    end_bytes = len(haystack) - 1;
  }

  std::vector<uint64_t> needle_pts
      = string_to_codepoints(needle->data, len(needle));
  for (uint64_t h = 0, h_pt = upos; h <= end_bytes; --h_pt) {
    auto [codepoint, num_bytes]
        = kllvm::readCodepointEndingAtIndex(haystack->data, end_bytes - h);
    for (uint64_t n : needle_pts) {
      if (codepoint == n) {
        mpz_init_set_ui(result, h_pt);
        return move_int(result);
      }
    }
    h += num_bytes;
  }

  mpz_init_set_si(result, -1);
  return move_int(result);
}

string *makeString(const KCHAR *input, ssize_t len = -1) {
  if (len == -1) {
    len = strlen(input);
  }
  auto ret = static_cast<string *>(koreAllocToken(sizeof(string) + len));
  init_with_len(ret, len);
  memcpy(ret->data, input, len);
  return ret;
}

char *getTerminatedString(string *str) {
  int length = len(str);
  string *buf
      = static_cast<string *>(koreAllocToken(sizeof(string) + (length + 1)));
  init_with_len(buf, length + 1);
  memcpy(buf->data, str->data, length);
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
  init_with_len(result, str.size());
  strncpy(result->data, str.c_str(), str_len);

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
  uint64_t ubase = get_ui(base);
  return hook_STRING_string2base_long(input, ubase);
}

SortString hook_STRING_base2string(SortInt input, SortInt base) {
  uint64_t ubase = get_ui(base);
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
  uint64_t uoccurences = get_ui(occurences);
  auto start = &haystack->data[0];
  auto pos = start;
  auto end = &haystack->data[len(haystack)];
  size_t matches[len(haystack)];
  uint64_t i = 0;
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
  init_with_len(ret, new_len);
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
    SortString inputStr, SortString inputCharset, SortString outputCharset) {
  iconv_t converter = iconv_open(
      getTerminatedString(outputCharset), getTerminatedString(inputCharset));
  SortBytes input = hook_BYTES_string2bytes(inputStr);
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
  return hook_BYTES_bytes2string(
      makeString(buf, len(input) * 4 - outbytesleft));
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
  init_with_len(result, sizeof(stringbuffer) - sizeof(blockheader));
  result->strlen = 0;
  auto str = static_cast<string *>(koreAllocToken(sizeof(string) + 16));
  init_with_len(str, 16);
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
  init_with_len(buf->contents, newCapacity);
  return buf;
}

SortString hook_BUFFER_toString(SortStringBuffer buf) {
  uint64_t buf_len = buf->strlen;
  string *result
      = static_cast<string *>(koreAllocToken(sizeof(string) + buf_len));
  init_with_len(result, buf_len);
  memcpy(result->data, buf->contents->data, buf_len);
  return result;
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
