#include <gmp.h>
#include <cstdlib>
#include <cstring>
#include <cstdint>
#include <algorithm>
#include <stdexcept>

#include "runtime/alloc.h"
#include "runtime/header.h"

extern "C" {

#define KCHAR char

  mpz_ptr move_int(mpz_t);

  string *hook_BYTES_empty() {
    static string empty;
    empty.h.hdr = NOT_YOUNG_OBJECT_BIT;
    return &empty;
  }

  uint32_t getTagForSymbolName(const char *);

  // bytes2int and int2bytes expect constructors of sort Endianness, which become a uint64_t
  // constant value in the K term representation
  uint64_t tag_big_endian() {
    static uint64_t tag = (uint64_t)-1;
    if (tag == -1) {
      tag = (((uint64_t)getTagForSymbolName("LblbigEndianBytes{}")) << 32) | 1;
    }
    return tag;
  }

  // bytes2int expects constructors of sort Signedness, which become a uint64_t
  // constant value in the K term representation
  uint64_t tag_unsigned() {
    static uint64_t tag = (uint64_t)-1;
    if (tag == -1) {
      tag = (((uint64_t)getTagForSymbolName("LblunsignedBytes{}")) << 32) | 1;
    }
    return tag;
  }

  // syntax Int ::= Bytes2Int(Bytes, Endianness, Signedness)
  mpz_ptr hook_BYTES_bytes2int(string *b, uint64_t endianness, uint64_t signedness) {
    mpz_t result;
    mpz_init(result);
    int order = endianness == tag_big_endian() ? 1 : -1;
    mpz_import(result, len(b), order, 1, 0, 0, b->data);
    if (signedness != tag_unsigned() && len(b) != 0) {
      bool msb;
      if (endianness == tag_big_endian()) {
        msb = b->data[0] & 0x80;
      } else {
        msb = b->data[len(b) - 1] & 0x80;
      }
      if (msb) {
        mpz_t max;
        mpz_init_set_ui(max, 1);
        mpz_mul_2exp(max, max, len(b)*8);
        mpz_sub(result, result, max);
        mpz_clear(max);
      }
    }
    return move_int(result);
  }

  unsigned long get_ui(mpz_t i) {
    if (!mpz_fits_ulong_p(i)) {
      throw std::invalid_argument("Integer overflow");
    }
    return mpz_get_ui(i);
  }

  void extract(mpz_t, mpz_t, size_t, size_t);

  // syntax Bytes ::= Int2Bytes(Int, Int, Endianness)
  string *hook_BYTES_int2bytes(mpz_t len, mpz_t i, uint64_t endianness) {
    unsigned long len_long = mpz_get_ui(len);
    if (len_long == 0) {
      return hook_BYTES_empty();
    }
    bool neg = mpz_sgn(i) < 0;
    string *result = static_cast<string *>(koreAllocToken(sizeof(string) + len_long));
    set_len(result, len_long);
    memset(result->data, neg ? 0xff : 0x00, len_long);
    int order = endianness == tag_big_endian() ? 1 : -1;
    mpz_t twos;
    mpz_init(twos);
    extract(twos, i, 0, len_long*8);
    size_t sizeInBytes = (mpz_sizeinbase(twos, 2) + 7) / 8;
    void *start = result->data + (endianness == tag_big_endian() ? len_long - sizeInBytes : 0);
    mpz_export(start, nullptr, order, 1, 0, 0, twos);
    mpz_clear(twos);
    return result;
  }

  string *bytes2string(string *b, size_t len) {
    string *result = static_cast<string *>(koreAllocToken(sizeof(string) + len));
    memcpy(result->data, b->data, len);
    set_len(result, len);
    return result;
  }

  string *hook_BYTES_bytes2string(string *b) {
    return bytes2string(b, len(b));
  }

  string *hook_BYTES_string2bytes(string *s) {
    return hook_BYTES_bytes2string(s);
  }

  string *hook_BYTES_substr(string *input, mpz_t start, mpz_t end) {
    uint64_t ustart = get_ui(start);
    uint64_t uend = get_ui(end);
    if (uend < ustart) {
      throw std::invalid_argument("Invalid string slice");
    }
    if (uend > len(input)) {
      throw std::invalid_argument("Invalid string slice");
    }
    uint64_t len = uend - ustart;
    auto ret = static_cast<string *>(koreAllocToken(sizeof(string) + sizeof(KCHAR) * len));
    set_len(ret, len);
    memcpy(&(ret->data), &(input->data[ustart]), len * sizeof(KCHAR));
    return ret;
  }

  mpz_ptr hook_BYTES_get(string *b, mpz_t off) {
    unsigned long off_long = get_ui(off);
    if (off_long >= len(b)) {
      throw std::invalid_argument("Buffer overflow on get");
    }
    mpz_t result;
    mpz_init_set_ui(result, (unsigned char)b->data[off_long]);
    return move_int(result);
  }

  string *hook_BYTES_update(string *b, mpz_t off, mpz_t val) {
    unsigned long off_long = get_ui(off);
    if (off_long >= len(b)) {
      throw std::invalid_argument("Buffer overflow on update");
    }
    unsigned long val_long = get_ui(val);
    if (val_long >= 256) {
      throw std::invalid_argument("Not a valid value for a byte in update");
    }
    b->data[off_long] = (unsigned char)val_long;
    return b;
  }

  string *hook_BYTES_replaceAt(string *b, mpz_t start, string *b2) {
    unsigned long start_long = get_ui(start);
    if (start_long + len(b2) > len(b)) {
      throw std::invalid_argument("Buffer overflow on replaceAt");
    }
    memcpy(b->data + start_long, b2->data, len(b2));
    return b;
  }

  mpz_ptr hook_BYTES_length(string *a) {
    mpz_t result;
    mpz_init_set_ui(result, len(a));
    return move_int(result);
  }

  string *hook_BYTES_padRight(string *b, mpz_t len, mpz_t v) {
    unsigned long ulen = get_ui(len);
    if (ulen <= len(b)) {
      return b;
    }
    unsigned long uv = get_ui(v);
    if (uv > 255) {
      throw std::invalid_argument("Integer overflow on value");
    }
    string *result = static_cast<string *>(koreAllocToken(sizeof(string) + ulen));
    set_len(result, ulen);
    memcpy(result->data, b->data, len(b));
    memset(result->data + len(b), uv, ulen - len(b));
    return result;
  }

  string *hook_BYTES_padLeft(string *b, mpz_t len, mpz_t v) {
    unsigned long ulen = get_ui(len);
    if (ulen <= len(b)) {
      return b;
    }
    unsigned long uv = get_ui(v);
    if (uv > 255) {
      throw std::invalid_argument("Integer overflow on value");
    }
    string *result = static_cast<string *>(koreAllocToken(sizeof(string) + ulen));
    set_len(result, ulen);
    memset(result->data, uv, ulen - len(b));
    memcpy(result->data + ulen - len(b), b->data, len(b));
    return result;
  }

  string *hook_BYTES_reverse(string *b) {
    std::reverse(b->data, b->data + len(b));
    return b;
  }

  string *hook_BYTES_concat(string *a, string *b) {
    auto len_a = len(a);
    auto len_b = len(b);
    auto newlen = len_a  + len_b;
    auto ret = static_cast<string *>(koreAllocToken(sizeof(string) + newlen));
    set_len(ret, newlen);
    memcpy(&(ret->data), &(a->data), len(a) * sizeof(KCHAR));
    memcpy(&(ret->data[len(a)]), &(b->data), len(b) * sizeof(KCHAR));
    return ret;
  }

}
