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

extern "C" {

#define KCHAR char

  struct blockheader {
    uint64_t len;
  };

  struct string {
    blockheader b;
    KCHAR data[0];
  };

  struct stringbuffer {
    uint64_t capacity;
    string *contents;
  };

  mpz_ptr move_int(mpz_t);

  string *hook_BYTES_bytes2string(string *);
  string *hook_BYTES_concat(string *a, string *b);
  mpz_ptr hook_BYTES_length(string *a);
  string *hook_BYTES_substr(string *a, mpz_t start, mpz_t end);

  bool hook_STRING_gt(const string * a, const string * b) {
    auto res = memcmp(a->data, b->data, std::min(a->b.len, b->b.len));
    return res > 0 || (res == 0 && a->b.len > b->b.len);
  }

  bool hook_STRING_ge(const string * a, const string * b) {
    auto res = memcmp(a->data, b->data, std::min(a->b.len, b->b.len));
    return (res > 0 || (res == 0 && a->b.len >= b->b.len));
  }

  bool hook_STRING_lt(const string * a, const string * b) {
    auto res = memcmp(a->data, b->data, std::min(a->b.len, b->b.len));
    return res < 0 || (res == 0 && a->b.len < b->b.len);
  }

  bool hook_STRING_le(const string * a, const string * b) {
    auto res = memcmp(a->data, b->data, std::min(a->b.len, b->b.len));
    return (res < 0 || (res == 0 && a->b.len <= b->b.len));
  }

  bool hook_STRING_eq(const string * a, const string * b) {
    auto res = memcmp(a->data, b->data, std::min(a->b.len, b->b.len));
    return (res == 0 && a->b.len == b->b.len);
  }

  bool hook_STRING_ne(const string * a, const string * b) {
    auto res = memcmp(a->data, b->data, std::min(a->b.len, b->b.len));
    return (res != 0 || a->b.len != b->b.len);
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
    auto ret = static_cast<string *>(malloc(sizeof(string) + sizeof(KCHAR)));
    ret->b.len = 1;
    ret->data[0] = static_cast<KCHAR>(uord);
    return ret;
  }

  const mpz_ptr hook_STRING_ord(const string * input) {
    mpz_t result;
    if (input->b.len != 1) {
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
    if (upos >= haystack->b.len) {
      mpz_init_set_si(result, -1);
      return move_int(result);
    }
    auto out = std::search(haystack->data + upos * sizeof(KCHAR), haystack->data + haystack->b.len * sizeof(KCHAR),
        needle->data,   needle->data   + needle->b.len * sizeof(KCHAR));
    int64_t ret = (out - haystack->data) / sizeof(KCHAR);
    // search returns the end of the range if it is not found, but we want -1 in such a case.
    auto res = (ret < haystack->b.len)?ret:-1;
    mpz_init_set_si(result, res);
    return move_int(result);
  }

  mpz_ptr hook_STRING_rfind(const string * haystack, const string * needle, mpz_t pos) {
    // The semantics of rfind uposition are strange, it is the last position at which
    // the match can _start_, which means the end of the haystack needs to be upos + len(needle),
    // or the end of the haystack, if that's less.
    mpz_t result;
    uint64_t upos = gs(pos);
    upos += needle->b.len;
    auto end = (upos < haystack->b.len)?upos:haystack->b.len;
    auto out = std::find_end(&haystack->data[0], &haystack->data[end],
        &needle->data[0], &needle->data[needle->b.len]);
    auto ret = &*out - &haystack->data[0];
    auto res = (ret < end)?ret:-1;
    mpz_init_set_si(result, res);
    return move_int(result);
  }

  mpz_ptr hook_STRING_findChar(const string * haystack, const string * needle, mpz_t pos) {
    if (needle->b.len > 1) {
      mpz_t result;
      mpz_init_set_si(result, -1);
      return move_int(result);
    }
    return hook_STRING_find(haystack, needle, pos);
  }

  mpz_ptr hook_STRING_rfindChar(const string * haystack, const string * needle, mpz_ptr pos) {
    if (needle->b.len > 1) {
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
    auto ret = static_cast<string *>(malloc(sizeof(string) + len));
    memcpy(ret->data, input, len);
    ret->b.len = len;
    return ret;
  }

  const string * hook_STRING_int2string(const mpz_t input) {
    size_t len = mpz_sizeinbase(input, 10) + 2;
    // +1 for null terminator needed by mpz_get_str, +1 for minus sign
    auto result = static_cast<string *>(malloc(sizeof(string) + len));
    mpz_get_str(result->data, 10, input);
    result->b.len = strlen(result->data);
    return static_cast<string *>(realloc(result, sizeof(string) + result->b.len));
  }

  const mpz_ptr hook_STRING_string2base_long(const string *input, uint64_t base) {
    mpz_t result;
    auto copy = static_cast<char *>(malloc(input->b.len + 1));
    memcpy(copy, input->data, input->b.len);
    copy[input->b.len] = 0;
    if (mpz_init_set_str(result, copy, base)) {
      throw std::invalid_argument("Not a valid integer");
    }
    free(copy);
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
    auto end = &haystack->data[haystack->b.len];
    size_t matches[haystack->b.len];
    int i = 0;
    while (i < uoccurences) {
      pos = std::search(pos, end, &needle->data[0], &needle->data[needle->b.len]);
      if (pos == end) {
        break;
      }
      matches[i] = (pos - start);
      ++pos; ++i;
    }
    if ( i == 0 ) {
      return haystack;
    }
    auto diff = needle->b.len - replacer->b.len;
    size_t new_len = haystack->b.len - i * diff;
    auto ret = static_cast<string *>(malloc(sizeof(string) + new_len * sizeof(KCHAR)));
    ret->b.len = new_len;
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
        memcpy(&ret->data[r], replacer->data, replacer->b.len);
        r += replacer->b.len;
        h += needle->b.len;
      }
    }
    return ret;
  }

  const string * hook_STRING_replaceAll(const string * haystack, const string * needle, const string * replacer) {
    // It's guaranteed that there can be no more replacements than the length of the haystack, so this
    // gives us the functionality of replaceAll.
    mpz_t arg;
    mpz_init_set_si(arg, haystack->b.len);
    return hook_STRING_replace(haystack, needle, replacer, arg);
  }

  const string * hook_STRING_replaceFirst(const string * haystack, const string * needle, const string * replacer) {
    mpz_t arg;
    mpz_init_set_si(arg, 1);
    return hook_STRING_replace(haystack, needle, replacer, arg);
  }

  mpz_ptr hook_STRING_countAllOccurrences(const string * haystack, const string * needle) {
    auto pos = &haystack->data[0];
    auto end = &haystack->data[haystack->b.len];
    int i = 0;
    while (true) {
      pos = std::search(pos, end, &needle->data[0], &needle->data[needle->b.len]);
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
    auto result = static_cast<stringbuffer *>(malloc(sizeof(stringbuffer)));
    result->capacity = 16;
    auto str = static_cast<string *>(malloc(sizeof(string) + 16));
    str->b.len = 0;
    result->contents = str;
    return result;
  }

  stringbuffer *hook_BUFFER_concat(stringbuffer *buf, string *s) {
    uint64_t newCapacity = buf->capacity;
    uint64_t minCapacity = buf->contents->b.len + s->b.len;
    if (newCapacity < minCapacity) {
      newCapacity = buf->contents->b.len * 2 + 2;
      if (newCapacity < minCapacity) {
        newCapacity = minCapacity;
      }
      buf->capacity = newCapacity;
      buf->contents = static_cast<string *>(realloc(buf->contents, sizeof(string) + newCapacity));
    }
    memcpy(buf->contents->data + buf->contents->b.len, s->data, s->b.len);
    buf->contents->b.len += s->b.len;
    return buf;
  }

  string *hook_BUFFER_toString(stringbuffer *buf) {
    return hook_BYTES_bytes2string(buf->contents);
  }
}
