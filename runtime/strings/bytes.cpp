#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <gmp.h>
#include <stdexcept>

#include "kllvm/ast/AST.h"
#include "runtime/alloc.h"
#include "runtime/header.h"

extern "C" {

#define KCHAR char

mpz_ptr move_int(mpz_t);

SortBytes hook_BYTES_empty() {
  static string empty;
  empty.h.hdr = NOT_YOUNG_OBJECT_BIT | IS_BYTES_BIT;
  return &empty;
}

uint32_t getTagForSymbolName(const char *);

// bytes2int and int2bytes expect constructors of sort Endianness, which become
// a uint64_t constant value in the K term representation
uint64_t tag_big_endian() {
  static uint64_t tag = (uint64_t)-1;
  if (tag == -1) {
    tag = (uint64_t)leaf_block(getTagForSymbolName("LblbigEndianBytes{}"));
  }
  return tag;
}

// bytes2int expects constructors of sort Signedness, which become a uint64_t
// constant value in the K term representation
uint64_t tag_unsigned() {
  static uint64_t tag = (uint64_t)-1;
  if (tag == -1) {
    tag = (uint64_t)leaf_block(getTagForSymbolName("LblunsignedBytes{}"));
  }
  return tag;
}

// syntax Int ::= Bytes2Int(Bytes, Endianness, Signedness)
SortInt hook_BYTES_bytes2int(
    SortBytes b, SortEndianness endianness_ptr, SortSignedness signedness_ptr) {
  uint64_t endianness = (uint64_t)endianness_ptr;
  uint64_t signedness = (uint64_t)signedness_ptr;
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
      mpz_mul_2exp(max, max, len(b) * 8);
      mpz_sub(result, result, max);
      mpz_clear(max);
    }
  }
  return move_int(result);
}

void extract(mpz_t, mpz_t, size_t, size_t);

// syntax Bytes ::= Int2Bytes(Int, Int, Endianness)
SortBytes
hook_BYTES_int2bytes(SortInt len, SortInt i, SortEndianness endianness_ptr) {
  uint64_t endianness = (uint64_t)endianness_ptr;
  unsigned long len_long = mpz_get_ui(len);
  if (len_long == 0) {
    return hook_BYTES_empty();
  }
  bool neg = mpz_sgn(i) < 0;
  string *result
      = static_cast<string *>(koreAllocToken(sizeof(string) + len_long));
  init_with_len(result, len_long);
  set_is_bytes(result, true);
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

SortString hook_BYTES_bytes2string(SortBytes b) {
  uint64_t b_len = len(b);
  uint64_t new_len = 0;
  for (uint64_t i = 0; i < b_len; ++i) {
    new_len += (static_cast<unsigned char>(b->data[i]) <= 0x7F) ? 1 : 2;
  }

  string *result
      = static_cast<string *>(koreAllocToken(sizeof(string) + new_len));
  init_with_len(result, new_len);
  set_is_bytes(result, false);
  for (uint64_t b_i = 0, res_i = 0; b_i < b_len; ++b_i) {
    unsigned char hh = static_cast<unsigned char>(b->data[b_i]);
    if (hh <= 0x7F) {
      result->data[res_i] = hh;
      ++res_i;
    } else {
      result->data[res_i] = 0xC0 | (hh >> 6);
      result->data[res_i + 1] = 0x80 | (hh & 0x3F);
      res_i += 2;
    }
  }
  return result;
}

SortBytes hook_BYTES_string2bytes(SortString s) {
  std::string std_result;
  char *s_iter = s->data;
  char *s_end = s->data + len(s);
  int index = 0;
  while (s_iter != s_end) {
    auto [codepoint, num_bytes] = kllvm::readCodepoint(s_iter);
    if (codepoint > 0xFF) {
      KLLVM_HOOK_INVALID_ARGUMENT(
          "Invalid codepoint U+{:04X} at index {} in input. Only codepoints up "
          "to U+00FF are permitted.",
          codepoint, index);
    }
    std_result += static_cast<char>(codepoint);
    s_iter += num_bytes;
    ++index;
  }

  string *result = static_cast<string *>(
      koreAllocToken(sizeof(string) + std_result.size()));
  init_with_len(result, std_result.size());
  set_is_bytes(result, true);
  memcpy(result->data, std_result.c_str(), std_result.size());
  return result;
}

SortBytes hook_BYTES_substr(SortBytes input, SortInt start, SortInt end) {
  uint64_t ustart = get_ui(start);
  uint64_t uend = get_ui(end);
  if (uend < ustart) {
    KLLVM_HOOK_INVALID_ARGUMENT(
        "Invalid bytes slice: Requested start index {} is greater than "
        "requested end index {}.",
        ustart, uend);
  }
  uint64_t input_len = len(input);
  if (uend > input_len) {
    KLLVM_HOOK_INVALID_ARGUMENT(
        "Invalid bytes slice for bytes: Requested end index {} is greater "
        "than bytes length {}",
        uend, input_len);
  }
  uint64_t len = uend - ustart;
  auto ret = static_cast<string *>(
      koreAllocToken(sizeof(string) + sizeof(KCHAR) * len));
  init_with_len(ret, len);
  set_is_bytes(ret, true);
  memcpy(&(ret->data), &(input->data[ustart]), len * sizeof(KCHAR));
  return ret;
}

SortInt hook_BYTES_get(SortBytes b, SortInt off) {
  unsigned long off_long = get_ui(off);
  if (off_long >= len(b)) {
    KLLVM_HOOK_INVALID_ARGUMENT(
        "Buffer overflow on get: off={}, len={}", off_long, len(b));
  }
  mpz_t result;
  mpz_init_set_ui(result, (unsigned char)b->data[off_long]);
  return move_int(result);
}

SortBytes hook_BYTES_update(SortBytes b, SortInt off, SortInt val) {
  unsigned long off_long = get_ui(off);
  if (off_long >= len(b)) {
    KLLVM_HOOK_INVALID_ARGUMENT(
        "Buffer overflow on update: off={}, len={}", off_long, len(b));
  }
  unsigned long val_long = get_ui(val);
  if (val_long >= 256) {
    KLLVM_HOOK_INVALID_ARGUMENT(
        "Not a valid value for a byte in update: {}", val_long);
  }
  b->data[off_long] = (unsigned char)val_long;
  return b;
}

SortBytes hook_BYTES_replaceAt(SortBytes b, SortInt start, SortBytes b2) {
  unsigned long start_long = get_ui(start);
  if (start_long + len(b2) > len(b)) {
    KLLVM_HOOK_INVALID_ARGUMENT(
        "Buffer overflow on replaceAt: start={}, dest_len={}, src_len={}",
        start_long, len(b), len(b2));
  }
  memcpy(b->data + start_long, b2->data, len(b2));
  return b;
}

SortInt hook_BYTES_length(SortBytes a) {
  mpz_t result;
  mpz_init_set_ui(result, len(a));
  return move_int(result);
}

SortBytes hook_BYTES_padRight(SortBytes b, SortInt length, SortInt v) {
  unsigned long ulen = get_ui(length);
  if (ulen <= len(b)) {
    return b;
  }
  unsigned long uv = get_ui(v);
  if (uv > 255) {
    KLLVM_HOOK_INVALID_ARGUMENT("Integer overflow on value: {}", uv);
  }
  string *result = static_cast<string *>(koreAllocToken(sizeof(string) + ulen));
  init_with_len(result, ulen);
  set_is_bytes(result, true);
  memcpy(result->data, b->data, len(b));
  memset(result->data + len(b), uv, ulen - len(b));
  return result;
}

SortBytes hook_BYTES_padLeft(SortBytes b, SortInt length, SortInt v) {
  unsigned long ulen = get_ui(length);
  if (ulen <= len(b)) {
    return b;
  }
  unsigned long uv = get_ui(v);
  if (uv > 255) {
    KLLVM_HOOK_INVALID_ARGUMENT("Integer overflow on value: {}", uv);
  }
  string *result = static_cast<string *>(koreAllocToken(sizeof(string) + ulen));
  init_with_len(result, ulen);
  set_is_bytes(result, true);
  memset(result->data, uv, ulen - len(b));
  memcpy(result->data + ulen - len(b), b->data, len(b));
  return result;
}

SortBytes hook_BYTES_reverse(SortBytes b) {
  std::reverse(b->data, b->data + len(b));
  return b;
}

SortBytes hook_BYTES_concat(SortBytes a, SortBytes b) {
  auto len_a = len(a);
  auto len_b = len(b);
  auto newlen = len_a + len_b;
  auto ret = static_cast<string *>(koreAllocToken(sizeof(string) + newlen));
  init_with_len(ret, newlen);
  set_is_bytes(ret, true);
  memcpy(&(ret->data), &(a->data), len(a) * sizeof(KCHAR));
  memcpy(&(ret->data[len(a)]), &(b->data), len(b) * sizeof(KCHAR));
  return ret;
}
}
