#include<gmp.h>
#include<mpfr.h>
#include<cstdlib>
#include<cstring>
#include<algorithm>
#include<cstdint>
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

  mpz_ptr move_int(mpz_t);

  bool hook_STRING_gt(const string * a, const string * b) {
    auto res = memcmp(a->data, b->data, std::min(a->b.len, b->b.len));
    return res > 0 || (res == 0 && a->b.len > b->b.len);
  }

  bool hook_STRING_gte(const string * a, const string * b) {
    auto res = memcmp(a->data, b->data, std::min(a->b.len, b->b.len));
    return (res > 0 || (res == 0 && a->b.len >= b->b.len));
  }

  bool hook_STRING_lt(const string * a, const string * b) {
    auto res = memcmp(a->data, b->data, std::min(a->b.len, b->b.len));
    return res < 0 || (res == 0 && a->b.len < b->b.len);
  }

  bool hook_STRING_lte(const string * a, const string * b) {
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
    auto len_a = a->b.len;
    auto len_b = b->b.len;
    auto newlen = len_a  + len_b;
    auto ret = static_cast<string *>(malloc(sizeof(string) + newlen));
    ret->b.len = newlen;
    memcpy(&(ret->data), &(a->data), a->b.len * sizeof(KCHAR));
    memcpy(&(ret->data[a->b.len]), &(b->data), b->b.len * sizeof(KCHAR));
    return ret;
  }

  mpz_ptr hook_STRING_length(string * a) {
    mpz_t result;
    mpz_init_set_ui(result, a->b.len);
    return move_int(result);
  }

  const string * hook_STRING_chr(mpz_t ord) {
    if (!mpz_fits_ulong_p(ord)) {
      throw std::invalid_argument("Ord too large for uint64_t");
    }
    uint64_t uord = mpz_get_ui(ord);
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
      throw std::invalid_argument("Input must not be an empty string");
    }
    mpz_init_set_ui(result, static_cast<uint64_t>(input->data[0]));
    return move_int(result);
  }

  static inline uint64_t gs(mpz_t i) {
    if (!mpz_fits_ulong_p(i)) {
      throw std::invalid_argument("Arg too large for int64_t");
    }
    return mpz_get_ui(i);
  }

  // -1 means take the entire rest of the string.
  string * hook_STRING_substr(string * input, mpz_t start, mpz_t end) {
    uint64_t ustart = gs(start);
    uint64_t uend = gs(end);
    if (uend < ustart) {
      throw std::invalid_argument("Invalid string slice");
    }
    if (uend > input->b.len) {
      throw std::invalid_argument("Invalid string slice");
    }
    uint64_t len = uend - ustart;
    auto ret = static_cast<string *>(malloc(sizeof(string) + sizeof(KCHAR) * len));
    ret->b.len = len;
    memcpy(&(ret->data), &(input->data[ustart]), len * sizeof(KCHAR));
    return ret;
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
    if (upos <= needle->b.len) {
      mpz_init_set_si(result, -1);
      return move_int(result);
    }
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

  string * makeString(const KCHAR * input) {
    auto len = strlen(input);
    auto ret = static_cast<string *>(malloc(sizeof(string) + len));
    for (unsigned i = 0; i < len; ++i) {
      ret->data[i] = input[i];
    }
    ret->b.len = len;
    return ret;
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
      if (r < matches[m] - (diff * m) || m >= i) {
        ret->data[r++] = haystack->data[h++];
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

  mpz_ptr hook_STRING_countAllOccurences(const string * haystack, const string * needle) {
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
}
