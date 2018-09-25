#include <gmp.h>
#include <cstdlib>
#include <cstring>
#include <cstdint>
#include <algorithm>
#include <stdexcept>

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

  string *hook_BYTES_empty() {
    static string empty;
    empty.b.len = 0;
    return &empty;
  }

  extern const uint64_t tag_big_endian;
  extern const uint64_t tag_unsigned;

  string *hook_STRING_concat(string *a, string *b);
  mpz_ptr hook_STRING_length(string *a);
  string *hook_STRING_substr(string *a, mpz_t start, mpz_t end);

  mpz_ptr hook_BYTES_bytes2int(string *b, uint64_t endianness, uint64_t signedness) {
    mpz_t result;
    mpz_init(result);
    int order = endianness == tag_big_endian ? 1 : -1;
    mpz_import(result, b->b.len, order, 1, 0, 0, b->data);
    if (signedness != tag_unsigned && b->b.len != 0) {
      bool msb;
      if (endianness == tag_big_endian) {
        msb = b->data[0] & 0x80;
      } else {
        msb = b->data[b->b.len - 1] & 0x80;
      }
      if (msb) {
        mpz_t max;
        mpz_init_set_ui(max, 1);
        mpz_mul_2exp(max, max, b->b.len*8);
        mpz_sub(result, result, max);
        mpz_clear(max);
      }
    }
    return move_int(result);
  }

  string *hook_BYTES_int2bytes(mpz_t len, mpz_t i, uint64_t endianness) {
    if (!mpz_fits_ulong_p(len)) {
      throw std::invalid_argument("Integer overflow on len");
    }
    unsigned long len_long = mpz_get_ui(len);
    if (len_long == 0) {
      return hook_BYTES_empty();
    }
    size_t sizeInBytes = (mpz_sizeinbase(i, 2) + 7) / 8;
    bool neg = mpz_cmp_si(i, 0) < 0;
    string *result = static_cast<string *>(malloc(sizeof(string) + len_long));
    result->b.len = len_long;
    memset(result->data, neg ? 0xff : 0x00, len_long);
    int order = endianness == tag_big_endian ? 1 : -1;
    void *start = result->data + (endianness == tag_big_endian ? len_long - sizeInBytes : 0);
    mpz_t twos;
    mpz_init_set(twos, i);
    if (mpz_sgn(i) < 0) {
      mpz_com(twos, twos);
      mpz_add_ui(twos, twos, 1);
    }
    mpz_export(start, nullptr, order, 1, 0, 0, twos);
    mpz_clear(twos);
    return result;
  }

  string *hook_BYTES_bytes2string(string *b) {
    size_t size = sizeof(string) + b->b.len;
    string *result = static_cast<string *>(malloc(size));
    memcpy(result, b, size);
    return result;
  }

  string *hook_BYTES_string2bytes(string *s) {
    return hook_BYTES_bytes2string(s);
  }

  string *hook_BYTES_substr(string *b, mpz_t start, mpz_t end) {
    return hook_STRING_substr(b, start, end);
  }

  string *hook_BYTES_replaceAt(string *b, mpz_t start, string *b2) {
    if (!mpz_fits_ulong_p(start)) {
      throw std::invalid_argument("Integer overflow on offset");
    }
    unsigned long start_long = mpz_get_ui(start);
    if (start_long + b2->b.len > b->b.len) {
      throw std::invalid_argument("Buffer overflow on replaceAt");
    }
    memcpy(b->data + start_long, b2->data, b2->b.len);
    return b;
  }

  mpz_ptr hook_BYTES_length(string *b) {
    return hook_STRING_length(b);
  }

  string *hook_BYTES_padRight(string *b, mpz_t len, mpz_t v) {
    if (!mpz_fits_ulong_p(len)) {
      throw std::invalid_argument("Integer overflow on len");
    }
    uint64_t ulen = mpz_get_ui(len);
    if (ulen <= b->b.len) {
      return b;
    }
    if (!mpz_fits_ulong_p(v)) {
      throw std::invalid_argument("Integer overflow on value");
    }
    uint64_t uv = mpz_get_ui(v);
    if (uv > 255) {
      throw std::invalid_argument("Integer overflow on value");
    }
    string *result = static_cast<string *>(malloc(sizeof(string) + ulen));
    result->b.len = ulen;
    memcpy(result->data, b->data, b->b.len);
    memset(result->data + b->b.len, uv, ulen - b->b.len);
    return result;
  }

  string *hook_BYTES_padLeft(string *b, mpz_t len, mpz_t v) {
    if (!mpz_fits_ulong_p(len)) {
      throw std::invalid_argument("Integer overflow on len");
    }
    uint64_t ulen = mpz_get_ui(len);
    if (ulen <= b->b.len) {
      return b;
    }
    if (!mpz_fits_ulong_p(v)) {
      throw std::invalid_argument("Integer overflow on value");
    }
    uint64_t uv = mpz_get_ui(v);
    if (uv > 255) {
      throw std::invalid_argument("Integer overflow on value");
    }
    string *result = static_cast<string *>(malloc(sizeof(string) + ulen));
    result->b.len = ulen;
    memset(result->data, uv, ulen - b->b.len);
    memcpy(result->data + ulen - b->b.len, b->data, b->b.len);
    return result;
  }

  string *hook_BYTES_reverse(string *b) {
    std::reverse(b->data, b->data + b->b.len);
    return b;
  }

  string *hook_BYTES_concat(string *b1, string *b2) {
    return hook_STRING_concat(b1, b2);
  }

}
