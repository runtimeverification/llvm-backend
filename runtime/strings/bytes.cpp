#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <gmp.h>
#include <stdexcept>
#include <unordered_set>

#include "runtime/alloc.h"
#include "runtime/header.h"

void copy_if_needed(SortBytes &b);

extern "C" {

#undef GET_UI
#define GET_UI(x) get_ui_named(x, __func__)
#define KCHAR char

SortBytes hook_BYTES_empty() {
  static string empty;
  empty.h.hdr = NOT_YOUNG_OBJECT_BIT;
  return &empty;
}

// bytes2int and int2bytes expect constructors of sort Endianness, which become
// a uint64_t constant value in the K term representation
uint64_t tag_big_endian() {
  static auto tag = (uint64_t)-1;
  if (tag == -1) {
    tag = (uint64_t)leaf_block(getTagForSymbolName("LblbigEndianBytes{}"));
  }
  return tag;
}

// bytes2int expects constructors of sort Signedness, which become a uint64_t
// constant value in the K term representation
uint64_t tag_unsigned() {
  static auto tag = (uint64_t)-1;
  if (tag == -1) {
    tag = (uint64_t)leaf_block(getTagForSymbolName("LblunsignedBytes{}"));
  }
  return tag;
}

// syntax Int ::= Bytes2Int(Bytes, Endianness, Signedness)
SortInt hook_BYTES_bytes2int(
    SortBytes b, SortEndianness endianness_ptr, SortSignedness signedness_ptr) {
  auto endianness = (uint64_t)endianness_ptr;
  auto signedness = (uint64_t)signedness_ptr;
  mpz_t result;
  mpz_init(result);
  int order = endianness == tag_big_endian() ? 1 : -1;
  mpz_import(result, len(b), order, 1, 0, 0, b->data);
  if (signedness != tag_unsigned() && len(b) != 0) {
    bool msb = false;
    if (endianness == tag_big_endian()) {
      msb = b->data[0] & 0x80;
    } else {
      msb = b->data[len(b) - 1] & 0x80;
    }
    if (msb) {
      mpz_t max;
      mpz_init_set_ui(max, 1);
      mpz_mul_2exp(max, max, len(b) * 8);
      mpz_sub(result, result, max);
      mpz_clear(max);
    }
  }
  return move_int(result);
}

unsigned long get_ui_named(mpz_t i, std::string const &caller) {
  if (!mpz_fits_ulong_p(i)) {
    KLLVM_HOOK_INVALID_ARGUMENT(
        "Integer overflow from {}: {}", caller, intToString(i));
  }
  return mpz_get_ui(i);
}

void extract(mpz_t, mpz_t, size_t, size_t);

// syntax Bytes ::= Int2Bytes(Int, Int, Endianness)
SortBytes
hook_BYTES_int2bytes(SortInt len, SortInt i, SortEndianness endianness_ptr) {
  auto endianness = (uint64_t)endianness_ptr;
  unsigned long len_long = mpz_get_ui(len);
  if (len_long == 0) {
    return hook_BYTES_empty();
  }
  bool neg = mpz_sgn(i) < 0;
  auto *result
      = static_cast<string *>(koreAllocToken(sizeof(string) + len_long));
  init_with_len(result, len_long);
  memset(result->data, neg ? 0xff : 0x00, len_long);
  int order = endianness == tag_big_endian() ? 1 : -1;
  mpz_t twos;
  mpz_init(twos);
  extract(twos, i, 0, len_long * 8);
  size_t sizeInBytes = (mpz_sizeinbase(twos, 2) + 7) / 8;
  void *start = result->data
                + (endianness == tag_big_endian() ? len_long - sizeInBytes : 0);
  mpz_export(start, nullptr, order, 1, 0, 0, twos);
  mpz_clear(twos);
  return result;
}

string *bytes2string(string *b, size_t len) {
  auto *result = static_cast<string *>(koreAllocToken(sizeof(string) + len));
  memcpy(result->data, b->data, len);
  init_with_len(result, len);
  return result;
}

SortString hook_BYTES_bytes2string(SortBytes b) {
  return bytes2string(b, len(b));
}

SortBytes hook_BYTES_string2bytes(SortString s) {
  return hook_BYTES_bytes2string(s);
}

SortBytes hook_BYTES_substr(SortBytes input, SortInt start, SortInt end) {
  uint64_t ustart = GET_UI(start);
  uint64_t uend = GET_UI(end);
  if (uend < ustart) {
    KLLVM_HOOK_INVALID_ARGUMENT(
        "Invalid string slice: Requested start index {} is greater than "
        "requested end index {}.",
        ustart, uend);
  }
  uint64_t input_len = len(input);
  if (uend > input_len) {
    KLLVM_HOOK_INVALID_ARGUMENT(
        "Invalid string slice for string: Requested end index {} is greater "
        "than string length {}",
        uend, input_len);
  }
  uint64_t len = uend - ustart;
  auto *ret = static_cast<string *>(
      koreAllocToken(sizeof(string) + sizeof(KCHAR) * len));
  init_with_len(ret, len);
  memcpy(&(ret->data), &(input->data[ustart]), len * sizeof(KCHAR));
  return ret;
}

SortInt hook_BYTES_get(SortBytes b, SortInt off) {
  unsigned long off_long = GET_UI(off);
  if (off_long >= len(b)) {
    KLLVM_HOOK_INVALID_ARGUMENT(
        "Buffer overflow on get: off={}, len={}", off_long, len(b));
  }
  mpz_t result;
  mpz_init_set_ui(result, (unsigned char)b->data[off_long]);
  return move_int(result);
}

SortBytes hook_BYTES_update(SortBytes b, SortInt off, SortInt val) {
  copy_if_needed(b);

  unsigned long off_long = GET_UI(off);
  if (off_long >= len(b)) {
    KLLVM_HOOK_INVALID_ARGUMENT(
        "Buffer overflow on update: off={}, len={}", off_long, len(b));
  }
  unsigned long val_long = GET_UI(val);
  if (val_long >= 256) {
    KLLVM_HOOK_INVALID_ARGUMENT(
        "Not a valid value for a byte in update: {}", val_long);
  }
  b->data[off_long] = (unsigned char)val_long;
  return b;
}

SortBytes hook_BYTES_replaceAt(SortBytes b, SortInt start, SortBytes b2) {
  copy_if_needed(b);

  unsigned long start_long = GET_UI(start);
  if (start_long + len(b2) > len(b)) {
    KLLVM_HOOK_INVALID_ARGUMENT(
        "Buffer overflow on replaceAt: start={}, dest_len={}, src_len={}",
        start_long, len(b), len(b2));
  }
  memcpy(b->data + start_long, b2->data, len(b2));
  return b;
}

SortBytes
hook_BYTES_memset(SortBytes b, SortInt start, SortInt count, SortInt value) {
  copy_if_needed(b);

  uint64_t ustart = GET_UI(start);
  uint64_t ucount = GET_UI(count);
  uint64_t uend = ustart + ucount;
  if ((uend < ustart) || (uend < ucount)) {
    KLLVM_HOOK_INVALID_ARGUMENT(
        "Exception on memset: unsigned addition start {} plus count {} wraps "
        "around: uend= {}",
        ustart, ucount, uend);
  }
  uint64_t input_len = len(b);
  if (uend > input_len) {
    KLLVM_HOOK_INVALID_ARGUMENT(
        "Buffer overflow on memset: start {} plus count {} is greater "
        "than buffer length {}",
        ustart, ucount, input_len);
  }
  int v = mpz_get_si(value);
  if ((v < -128) || (v > 127)) {
    KLLVM_HOOK_INVALID_ARGUMENT(
        "Not a valid value for a byte in memset: v={}", v);
  }
  memset(b->data + ustart, v, ucount);
  return b;
}

SortInt hook_BYTES_length(SortBytes a) {
  mpz_t result;
  mpz_init_set_ui(result, len(a));
  return move_int(result);
}

SortBytes hook_BYTES_padRight(SortBytes b, SortInt length, SortInt v) {
  unsigned long ulen = GET_UI(length);
  if (ulen <= len(b)) {
    return b;
  }
  unsigned long uv = GET_UI(v);
  if (uv > 255) {
    KLLVM_HOOK_INVALID_ARGUMENT("Integer overflow on value: {}", uv);
  }
  auto *result = static_cast<string *>(koreAllocToken(sizeof(string) + ulen));
  init_with_len(result, ulen);
  memcpy(result->data, b->data, len(b));
  memset(result->data + len(b), uv, ulen - len(b));
  return result;
}

SortBytes hook_BYTES_padLeft(SortBytes b, SortInt length, SortInt v) {
  unsigned long ulen = GET_UI(length);
  if (ulen <= len(b)) {
    return b;
  }
  unsigned long uv = GET_UI(v);
  if (uv > 255) {
    KLLVM_HOOK_INVALID_ARGUMENT("Integer overflow on value: {}", uv);
  }
  auto *result = static_cast<string *>(koreAllocToken(sizeof(string) + ulen));
  init_with_len(result, ulen);
  memset(result->data, uv, ulen - len(b));
  memcpy(result->data + ulen - len(b), b->data, len(b));
  return result;
}

SortBytes hook_BYTES_reverse(SortBytes b) {
  copy_if_needed(b);
  std::reverse(b->data, b->data + len(b));
  return b;
}

SortBytes hook_BYTES_concat(SortBytes a, SortBytes b) {
  auto len_a = len(a);
  auto len_b = len(b);
  auto newlen = len_a + len_b;
  auto *ret = static_cast<string *>(koreAllocToken(sizeof(string) + newlen));
  init_with_len(ret, newlen);
  memcpy(&(ret->data), &(a->data), len(a) * sizeof(KCHAR));
  memcpy(&(ret->data[len(a)]), &(b->data), len(b) * sizeof(KCHAR));
  return ret;
}
}
