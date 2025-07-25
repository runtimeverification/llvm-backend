#include <cstdlib>
#include <cstring>
#include <gmp.h>
#include <stdexcept>

#include "runtime/header.h"

extern "C" {

void add_hash64(void *, uint64_t);

SortInt hook_INT_tmod(SortInt a, SortInt b) {
  mpz_t result;
  if (mpz_sgn(b) == 0) {
    KLLVM_HOOK_INVALID_ARGUMENT(
        "Modulus by zero: {} % {}", int_to_string(a), int_to_string(b));
  }
  mpz_init(result);
  mpz_tdiv_r(result, a, b);
  return move_int(result);
}

SortInt hook_INT_emod(SortInt a, SortInt b) {
  mpz_t result;
  if (mpz_sgn(b) == 0) {
    KLLVM_HOOK_INVALID_ARGUMENT(
        "Modulus by zero: {} % {}", int_to_string(a), int_to_string(b));
  }
  mpz_init(result);
  mpz_tdiv_r(result, a, b);
  if (mpz_sgn(result) >= 0) {
    return move_int(result);
  }
  mpz_t absb;
  mpz_init(absb);
  mpz_abs(absb, b);
  mpz_add(result, result, absb);
  mpz_clear(absb);
  return move_int(result);
}

SortInt hook_INT_add(SortInt a, SortInt b) {
  mpz_t result;
  mpz_init(result);
  mpz_add(result, a, b);
  return move_int(result);
}

bool hook_INT_le(SortInt a, SortInt b) {
  return mpz_cmp(a, b) <= 0;
}

bool hook_INT_eq(SortInt a, SortInt b) {
  return mpz_cmp(a, b) == 0;
}

bool hook_INT_ne(SortInt a, SortInt b) {
  return mpz_cmp(a, b) != 0;
}

SortInt hook_INT_and(SortInt a, SortInt b) {
  mpz_t result;
  mpz_init(result);
  mpz_and(result, a, b);
  return move_int(result);
}

SortInt hook_INT_mul(SortInt a, SortInt b) {
  mpz_t result;
  mpz_init(result);
  mpz_mul(result, a, b);
  return move_int(result);
}

SortInt hook_INT_sub(SortInt a, SortInt b) {
  mpz_t result;
  mpz_init(result);
  mpz_sub(result, a, b);
  return move_int(result);
}

SortInt hook_INT_neg(SortInt a) {
  mpz_t result;
  mpz_init(result);
  mpz_neg(result, a);
  return move_int(result);
}

SortInt hook_INT_tdiv(SortInt a, SortInt b) {
  mpz_t result;
  if (mpz_sgn(b) == 0) {
    KLLVM_HOOK_INVALID_ARGUMENT(
        "Division by zero: {} / {}", int_to_string(a), int_to_string(b));
  }
  mpz_init(result);
  mpz_tdiv_q(result, a, b);
  return move_int(result);
}

SortInt hook_INT_ediv(SortInt a, SortInt b) {
  mpz_t result;
  if (mpz_sgn(b) == 0) {
    KLLVM_HOOK_INVALID_ARGUMENT(
        "Division by zero: {} / {}", int_to_string(a), int_to_string(b));
  }
  mpz_init(result);
  if (mpz_sgn(b) >= 0) {
    mpz_fdiv_q(result, a, b);
  } else {
    mpz_cdiv_q(result, a, b);
  }
  return move_int(result);
}

SortInt hook_INT_shl(SortInt a, SortInt b) {
  mpz_t result;
  if (!mpz_fits_ulong_p(b)) {
    KLLVM_HOOK_INVALID_ARGUMENT(
        "Left shift amount out of range: {} << {}", int_to_string(a),
        int_to_string(b));
  }
  mpz_init(result);
  unsigned long blong = mpz_get_ui(b);
  mpz_mul_2exp(result, a, blong);
  return move_int(result);
}

bool hook_INT_lt(SortInt a, SortInt b) {
  return mpz_cmp(a, b) < 0;
}

bool hook_INT_ge(SortInt a, SortInt b) {
  return mpz_cmp(a, b) >= 0;
}

SortInt hook_INT_shr(SortInt a, SortInt b) {
  mpz_t result;
  mpz_init(result);
  if (!mpz_fits_ulong_p(b)) {
    if (mpz_sgn(b) < 0) {
      KLLVM_HOOK_INVALID_ARGUMENT(
          "Negative right shift amount: {} >> {}", int_to_string(a),
          int_to_string(b));
    }
    if (mpz_sgn(a) < 0) {
      mpz_set_si(result, -1);
    }
    return move_int(result);
  }
  unsigned long blong = mpz_get_ui(b);
  mpz_fdiv_q_2exp(result, a, blong);
  return move_int(result);
}

bool hook_INT_gt(SortInt a, SortInt b) {
  return mpz_cmp(a, b) > 0;
}

SortInt hook_INT_pow(SortInt a, SortInt b) {
  mpz_t result;
  if (!mpz_fits_ulong_p(b)) {
    KLLVM_HOOK_INVALID_ARGUMENT(
        "Exponent out of range: {} ^ {}", int_to_string(a), int_to_string(b));
  }
  mpz_init(result);
  unsigned long blong = mpz_get_ui(b);
  mpz_pow_ui(result, a, blong);
  return move_int(result);
}

SortInt hook_INT_powmod(SortInt a, SortInt b, SortInt mod) {
  mpz_t result;
  mpz_init(result);
  if (mpz_sgn(b) < 0) {
    mpz_gcd(result, a, mod);
    if (mpz_cmp_ui(result, 1) != 0) {
      mpz_clear(result);
      KLLVM_HOOK_INVALID_ARGUMENT(
          "Modular inverse not defined: {} ^ {} % {}", int_to_string(a),
          int_to_string(b), int_to_string(mod));
    }
  }
  mpz_powm(result, a, b, mod);
  return move_int(result);
}

SortInt hook_INT_xor(SortInt a, SortInt b) {
  mpz_t result;
  mpz_init(result);
  mpz_xor(result, a, b);
  return move_int(result);
}

SortInt hook_INT_or(SortInt a, SortInt b) {
  mpz_t result;
  mpz_init(result);
  mpz_ior(result, a, b);
  return move_int(result);
}

SortInt hook_INT_not(SortInt a) {
  mpz_t result;
  mpz_init(result);
  mpz_com(result, a);
  return move_int(result);
}

SortInt hook_INT_abs(SortInt a) {
  mpz_t result;
  mpz_init(result);
  mpz_abs(result, a);
  return move_int(result);
}

SortInt hook_INT_max(SortInt a, SortInt b) {
  mpz_t result;
  mpz_init(result);
  if (mpz_cmp(a, b) >= 0) {
    mpz_set(result, a);
  } else {
    mpz_set(result, b);
  }
  return move_int(result);
}

SortInt hook_INT_min(SortInt a, SortInt b) {
  mpz_t result;
  mpz_init(result);
  if (mpz_cmp(a, b) <= 0) {
    mpz_set(result, a);
  } else {
    mpz_set(result, b);
  }
  return move_int(result);
}

SortInt hook_INT_log2(SortInt a) {
  mpz_t result;
  if (mpz_sgn(a) <= 0) {
    KLLVM_HOOK_INVALID_ARGUMENT(
        "Logarithm of nonpositive integer: log2({})", int_to_string(a));
  }
  mpz_init(result);
  size_t log = mpz_sizeinbase(a, 2) - 1;
  mpz_set_ui(result, log);
  return move_int(result);
}

#define LIMB_BITS (sizeof(mp_limb_t) * 8)

void extract(mpz_t result, mpz_t i, size_t off, size_t len) {
  ssize_t size = (len + LIMB_BITS - 1) / LIMB_BITS;
  mpz_init2(result, len + LIMB_BITS);
  memset(result->_mp_d, 0, result->_mp_alloc * sizeof(mp_limb_t));
  size_t off_words = off / LIMB_BITS;
  size_t off_bits = off % LIMB_BITS;
  size_t num_limbs = mpz_size(i);
  ssize_t copy_size = num_limbs - off_words;
  if (copy_size > size + 1) {
    copy_size = size + 1;
  }
  mp_limb_t carry = 0;
  if (copy_size > 0) {
    if (off_bits) {
      carry = mpn_rshift(
          result->_mp_d, i->_mp_d + off_words, copy_size, off_bits);
    } else {
      mpn_copyi(result->_mp_d, i->_mp_d + off_words, copy_size);
    }
  }
  if (mpz_sgn(i) < 0) {
    mpn_com(result->_mp_d, result->_mp_d, size);
    for (int j = 0; !carry && j < off_words && j < num_limbs; i++) {
      carry = i->_mp_d[j];
    }
    if (!carry) {
      mpn_add_1(result->_mp_d, result->_mp_d, size, 1);
    }
  }
  len %= LIMB_BITS;
  if (len) {
    result->_mp_d[size - 1] &= ((mp_limb_t)-1) >> (LIMB_BITS - len);
  }
  while (size > 0 && result->_mp_d[size - 1] == 0) {
    size--;
  }
  result->_mp_size = size;
}

SortInt hook_INT_bitRange(SortInt i, SortInt off, SortInt len) {
  mpz_t result;
  if (mpz_sgn(len) == 0) {
    mpz_init(result);
    return move_int(result);
  }
  if (!mpz_fits_ulong_p(len)) {
    KLLVM_HOOK_INVALID_ARGUMENT("Length out of range: {}", int_to_string(len));
  }
  unsigned long lenlong = mpz_get_ui(len);
  if (!mpz_fits_ulong_p(off)) {
    if (mpz_sgn(off) < 0) {
      KLLVM_HOOK_INVALID_ARGUMENT("Negative offset: {}", int_to_string(off));
    }
    mpz_init(result);
    if (mpz_sgn(i) < 0) {
      mpz_set_ui(result, 1);
      mpz_mul_2exp(result, result, lenlong);
      mpz_sub_ui(result, result, 1);
    }
    return move_int(result);
  }
  unsigned long offlong = mpz_get_ui(off);
  extract(result, i, offlong, lenlong);
  return move_int(result);
}

void signed_extract(mpz_t result, mpz_t i, size_t off, size_t len) {
  mpz_init(result);
  if (len == 0) {
    return;
  }
  if (mpz_tstbit(i, off + len - 1)) {
    mpz_t max;
    mpz_t tmp;
    mpz_init(max);
    mpz_init(tmp);
    mpz_set_ui(max, 1);
    mpz_mul_2exp(max, max, len - 1);
    extract(result, i, off, len);
    mpz_add(result, result, max);
    extract(tmp, result, 0, len);
    mpz_sub(result, tmp, max);
    mpz_clear(max);
    mpz_clear(tmp);
  } else {
    extract(result, i, off, len);
  }
}

SortInt hook_INT_signExtendBitRange(SortInt i, SortInt off, SortInt len) {
  mpz_t result;
  if (!mpz_fits_ulong_p(off)) {
    if (mpz_sgn(off) < 0) {
      KLLVM_HOOK_INVALID_ARGUMENT("Negative offset: {}", int_to_string(off));
    }
    mpz_init(result);
    if (mpz_sgn(i) < 0) {
      mpz_set_si(result, -1);
    }
    return move_int(result);
  }
  if (!mpz_fits_ulong_p(len)) {
    KLLVM_HOOK_INVALID_ARGUMENT("Length out of range: {}", int_to_string(len));
  }
  unsigned long offlong = mpz_get_ui(off);
  unsigned long lenlong = mpz_get_ui(len);
  signed_extract(result, i, offlong, lenlong);
  return move_int(result);
}

int32_t hook_INT_size_int(SortInt t) {
  return (int32_t)t->_mp_size;
}

uint64_t hook_INT_limb(SortInt t, uint64_t i) {
  return (uint64_t)t->_mp_d[i];
}

void int_hash(mpz_t i, void *hasher) {
  int nlimbs = mpz_size(i);
  for (int j = 0; j < nlimbs; j++) {
    add_hash64(hasher, i[0]._mp_d[j]);
  }
}

thread_local gmp_randstate_t kllvm_rand_state;
thread_local constinit bool kllvm_rand_state_initialized = false;

SortK hook_INT_srand(SortInt seed) {
  if (!kllvm_rand_state_initialized) {
    gmp_randinit_default(kllvm_rand_state);
  }
  gmp_randseed(kllvm_rand_state, seed);
  kllvm_rand_state_initialized = true;
  return dot_k();
}

SortInt hook_INT_rand(SortInt upper_bound) {
  mpz_t result;
  mpz_init(result);
  if (!kllvm_rand_state_initialized) {
    gmp_randinit_default(kllvm_rand_state);
    mpz_set_si(result, time(nullptr));
    gmp_randseed(kllvm_rand_state, result);
    kllvm_rand_state_initialized = true;
  }
  mpz_urandomm(result, kllvm_rand_state, upper_bound);
  return move_int(result);
}

uint64_t hook_MINT_pow64(uint64_t base, uint64_t exp) {
  if (exp == 0) {
    return 1;
  }
  if (base == 0) {
    return 0;
  }
  uint64_t result = 1;
  uint64_t current_base = base;
  uint64_t current_exp = exp;
  while (current_exp > 0) {
    if (current_exp & 1) {
      result *= current_base;
    }
    current_base *= current_base;
    current_exp >>= 1;
  }
  return result;
}

size_t *hook_MINT_export(mpz_t in, uint64_t bits) {
  uint64_t nwords = (bits + 63) / 64;
  mpz_t twos;
  mpz_init(twos);
  extract(twos, in, 0, nwords * 64);
  if (nwords == 0) {
    return nullptr;
  }
  uint64_t numb = 8 * sizeof(size_t);
  uint64_t count = (mpz_sizeinbase(twos, 2) + numb - 1) / numb;
  if (mpz_sgn(twos) == 0) {
    count = 0;
  }
  uint64_t alloccount = nwords > count ? nwords : count;
  size_t allocsize = alloccount * sizeof(size_t);
  auto *allocptr = (size_t *)kore_alloc_always_gc(allocsize);
  memset(allocptr, 0, allocsize);
  size_t *exportptr = nwords > count ? allocptr + nwords - count : allocptr;
  size_t actualcount = 0;
  mpz_export(exportptr, &actualcount, 1, sizeof(size_t), 0, 0, twos);
  assert(count == actualcount);
  if (count == 0) {
    return allocptr;
  }
  size_t *resultptr = nwords > count ? allocptr : allocptr + count - nwords;
  return resultptr;
}

mpz_ptr hook_MINT_import(size_t *i, uint64_t bits, bool is_signed) {
  mpz_t result;
  mpz_t twos;
  mpz_init(twos);
  mpz_init(result);

  uint64_t nwords = (bits + 63) / 64;
  mpz_import(twos, nwords, -1, sizeof(size_t), 0, 0, i);

  if (is_signed) {
    signed_extract(result, twos, 0, bits);
    return move_int(result);
  }

  return move_int(twos);
}
}
