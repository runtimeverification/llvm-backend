#include<gmp.h>
#include<mpfr.h>
#include<algorithm>
#include<cstdlib>
#include<cstdint>
#include<cstring>
#include<iomanip>
#include<string>
#include<sstream>
#include<stdexcept>

#include<iostream>

extern "C" {

#include "runtime/alloc.h"
#include "runtime/header.h"
#include<stdexcept>

#define KCHAR char

  mpz_ptr move_int(mpz_t);

  string *hook_BYTES_bytes2string(string *);
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
    auto res = memcmp(a->data, b->data, std::min(len(a), len(b)));
    return (res == 0 && len(a) == len(b));
  }

  bool hook_STRING_ne(const string * a, const string * b) {
    auto res = memcmp(a->data, b->data, std::min(len(a), len(b)));
    return (res != 0 || len(a) != len(b));
  }

  int64_t hook_STRING_cmp(const string * a, const string * b) {
    auto len_a = len(a);
    auto len_b = len(b);
    int64_t diff = len_a - len_b;
    if (diff != 0) {
        return diff;
    }
    return memcmp(a->data, b->data, len_a);
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

  const string * hook_STRING_int2string(const mpz_t input) {
    size_t len = mpz_sizeinbase(input, 10) + 2;
    // +1 for null terminator needed by mpz_get_str, +1 for minus sign
    auto result = static_cast<string *>(koreAllocToken(sizeof(string) + len));
    mpz_get_str(result->data, 10, input);
    set_len(result, strlen(result->data));
    return static_cast<string *>(koreResizeLastAlloc(result, sizeof(string) + len(result), sizeof(string) + len));
  }

  const mpz_ptr hook_STRING_string2base_long(const string *input, uint64_t base) {
    mpz_t result;
    auto copy = static_cast<char *>(koreAllocToken(len(input) + 1));
    memcpy(copy, input->data, len(input));
    copy[len(input)] = 0;
    if (mpz_init_set_str(result, copy, base)) {
      throw std::invalid_argument("Not a valid integer");
    }
    return move_int(result);
  }

  const mpz_ptr hook_STRING_string2int(const string *input) {
    return hook_STRING_string2base_long(input, 10);
  }

  const mpz_ptr hook_STRING_string2base(const string *input, mpz_t base) {
    uint64_t ubase = gs(base);
    return hook_STRING_string2base_long(input, ubase);
  }

  const string * hook_STRING_float2string(const mpfr_t input) {
    throw std::logic_error("TODO: floats");
  }

  mpfr_ptr hook_STRING_string2float(const string * input) {
    throw std::logic_error("TODO: floats");
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

  stringbuffer *hook_BUFFER_empty() {
    auto result = static_cast<stringbuffer *>(koreAllocToken(sizeof(stringbuffer)));
    result->capacity = 16;
    auto str = static_cast<string *>(koreAllocToken(sizeof(string) + 16));
    set_len(str, 0);
    result->contents = str;
    return result;
  }

  int64_t hook_BUFFER_cmp(const stringbuffer * a, const stringbuffer * b) {
    auto len_a = a->capacity;
    auto len_b = b->capacity;
    int64_t diff = len_a - len_b;
    if (diff != 0) {
        return diff;
    }
    return memcmp(a->contents, b->contents, len_a);
  }

  stringbuffer *hook_BUFFER_concat(stringbuffer *buf, string *s) {
    uint64_t newCapacity = buf->capacity;
    uint64_t minCapacity = len(buf->contents) + len(s);
    if (newCapacity < minCapacity) {
      newCapacity = len(buf->contents) * 2 + 2;
      if (newCapacity < minCapacity) {
        newCapacity = minCapacity;
      }
      buf->capacity = newCapacity;
      string* new_contents;
      if (buf->contents->h.hdr & NOT_YOUNG_OBJECT_BIT) {
        new_contents = static_cast<string *>(koreAllocTokenOld(sizeof(string) + newCapacity));
      } else {
        new_contents = static_cast<string *>(koreAllocToken(sizeof(string) + newCapacity));
      }
      memcpy(new_contents, buf->contents, sizeof(string) + len(buf->contents));
      // TODO: free/decref old contents.
      buf->contents = new_contents;
    }
    memcpy(buf->contents->data + len(buf->contents), s->data, len(s));
    set_len(buf->contents, len(buf->contents) + len(s));
    return buf;
  }

  string *hook_BUFFER_toString(stringbuffer *buf) {
    return hook_BYTES_bytes2string(buf->contents);
  }
}
