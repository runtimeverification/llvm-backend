#include<gmp.h>
#include<cstdlib>
#include<cstring>
#include<stdexcept>

#include "runtime/header.h"

extern "C" {

mpz_ptr move_int(mpz_t);
void add_hash64(void *, uint64_t);

mpz_ptr hook_INT_tmod(mpz_t a, mpz_t b) {
  mpz_t result;
  if (mpz_sgn(b) == 0) {
    throw std::invalid_argument("Modulus by zero");
  }
  mpz_init(result);
  mpz_tdiv_r(result, a, b);
  return move_int(result);
}

mpz_ptr hook_INT_emod(mpz_t a, mpz_t b) {
  mpz_t result;
  if (mpz_sgn(b) == 0) {
    throw std::invalid_argument("Modulus by zero");
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

mpz_ptr hook_INT_add(mpz_t a, mpz_t b) {
  mpz_t result;
  mpz_init(result);
  mpz_add(result, a, b);
  return move_int(result);
}

bool hook_INT_le(mpz_t a, mpz_t b) {
  return mpz_cmp(a, b) <= 0;
}

bool hook_INT_eq(mpz_t a, mpz_t b) {
  return mpz_cmp(a, b) == 0;
}

bool hook_INT_ne(mpz_t a, mpz_t b) {
  return mpz_cmp(a, b) != 0;
}

mpz_ptr hook_INT_and(mpz_t a, mpz_t b) {
  mpz_t result;
  mpz_init(result);
  mpz_and(result, a, b);
  return move_int(result);
}

mpz_ptr hook_INT_mul(mpz_t a, mpz_t b) {
  mpz_t result;
  mpz_init(result);
  mpz_mul(result, a, b);
  return move_int(result);
}

mpz_ptr hook_INT_sub(mpz_t a, mpz_t b) {
  mpz_t result;
  mpz_init(result);
  mpz_sub(result, a, b);
  return move_int(result);
}

mpz_ptr hook_INT_tdiv(mpz_t a, mpz_t b) {
  mpz_t result;
  if (mpz_sgn(b) == 0) {
    throw std::invalid_argument("Division by zero");
  }
  mpz_init(result);
  mpz_tdiv_q(result, a, b);
  return move_int(result);
}

mpz_ptr hook_INT_ediv(mpz_t a, mpz_t b) {
  mpz_t result;
  if (mpz_sgn(b) == 0) {
    throw std::invalid_argument("Division by zero");
  }
  mpz_init(result);
  if (mpz_sgn(b) >= 0) {
    mpz_fdiv_q(result, a, b);
  } else {
    mpz_cdiv_q(result, a, b);
  }
  return move_int(result);
}

mpz_ptr hook_INT_shl(mpz_t a, mpz_t b) {
  mpz_t result;
  if (!mpz_fits_ulong_p(b)) {
    throw std::invalid_argument("Shift amount out of range");
  }
  mpz_init(result);
  unsigned long blong = mpz_get_ui(b);
  mpz_mul_2exp(result, a, blong);
  return move_int(result);
}

bool hook_INT_lt(mpz_t a, mpz_t b) {
  return mpz_cmp(a, b) < 0;
}

bool hook_INT_ge(mpz_t a, mpz_t b) {
  return mpz_cmp(a, b) >= 0;
}

mpz_ptr hook_INT_shr(mpz_t a, mpz_t b) {
  mpz_t result;
  mpz_init(result);
  if (!mpz_fits_ulong_p(b)) {
    if (mpz_sgn(b) < 0) {
      throw std::invalid_argument("Negative shift amount");
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

bool hook_INT_gt(mpz_t a, mpz_t b) {
  return mpz_cmp(a, b) > 0;
}

mpz_ptr hook_INT_pow(mpz_t a, mpz_t b) {
  mpz_t result;
  if (!mpz_fits_ulong_p(b)) {
    throw std::invalid_argument("Exponent out of range");
  }
  mpz_init(result);
  unsigned long blong = mpz_get_ui(b);
  mpz_pow_ui(result, a, blong);
  return move_int(result);
}

mpz_ptr hook_INT_powmod(mpz_t a, mpz_t b, mpz_t mod) {
  mpz_t result;
  mpz_init(result);
  if (mpz_sgn(b) < 0) {
    mpz_gcd(result, a, mod);
    if (mpz_cmp_ui(result, 1) != 0) {
      mpz_clear(result);
      throw std::invalid_argument("Modular inverse not defined");
    }
  }
  mpz_powm(result, a, b, mod);
  return move_int(result);
}

mpz_ptr hook_INT_xor(mpz_t a, mpz_t b) {
  mpz_t result;
  mpz_init(result);
  mpz_xor(result, a, b);
  return move_int(result);
}

mpz_ptr hook_INT_or(mpz_t a, mpz_t b) {
  mpz_t result;
  mpz_init(result);
  mpz_ior(result, a, b);
  return move_int(result);
}

mpz_ptr hook_INT_not(mpz_t a) {
  mpz_t result;
  mpz_init(result);
  mpz_com(result, a);
  return move_int(result);
}

mpz_ptr hook_INT_abs(mpz_t a) {
  mpz_t result;
  mpz_init(result);
  mpz_abs(result, a);
  return move_int(result);
}

mpz_ptr hook_INT_max(mpz_t a, mpz_t b) {
  mpz_t result;
  mpz_init(result);
  if (mpz_cmp(a, b) >= 0) {
    mpz_set(result, a);
  } else {
    mpz_set(result, b);
  }
  return move_int(result);
}

mpz_ptr hook_INT_min(mpz_t a, mpz_t b) {
  mpz_t result;
  mpz_init(result);
  if (mpz_cmp(a, b) <= 0) {
    mpz_set(result, a);
  } else {
    mpz_set(result, b);
  }
  return move_int(result);
}

mpz_ptr hook_INT_log2(mpz_t a) {
  mpz_t result;
  if (mpz_sgn(a) <= 0) {
    throw std::invalid_argument("Logarithm of nonpositive integer");
  }
  mpz_init(result);
  size_t log = mpz_sizeinbase(a, 2) - 1;
  mpz_set_ui(result, log);
  return move_int(result);
}

#define LIMB_BITS (sizeof(mp_limb_t) * 8)

void extract(mpz_t result, mpz_t i, size_t off, size_t len) {
  ssize_t size = (len + LIMB_BITS - 1) / LIMB_BITS;
  mpz_init2(result, len+LIMB_BITS);
  memset(result->_mp_d, 0, result->_mp_alloc*sizeof(mp_limb_t));
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
      carry = mpn_rshift(result->_mp_d, i->_mp_d + off_words, copy_size, off_bits);
    }
    else {
      mpn_copyi(result->_mp_d, i->_mp_d + off_words, copy_size);
    }
  } else {
    copy_size = 0;
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
    result->_mp_d[size-1] &= ((mp_limb_t)-1) >> (LIMB_BITS - len);
  }
  while (size > 0 && result->_mp_d[size-1] == 0) {
    size--;
  }
  result->_mp_size = size;
}

mpz_ptr hook_INT_bitRange(mpz_t i, mpz_t off, mpz_t len) {
  mpz_t result;
  if (mpz_sgn(len) == 0) {
    mpz_init(result);
    return move_int(result);
  }
  if (!mpz_fits_ulong_p(len)) {
    throw std::invalid_argument("Length out of range");
  }
  unsigned long lenlong = mpz_get_ui(len);
  if (!mpz_fits_ulong_p(off)) {
    if (mpz_sgn(off) < 0) {
      throw std::invalid_argument("Negative offset");
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
    mpz_t max, tmp;
    mpz_init(max);
    mpz_init(tmp);
    mpz_set_ui(max, 1);
    mpz_mul_2exp(max, max, len-1);
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

mpz_ptr hook_INT_signExtendBitRange(mpz_t i, mpz_t off, mpz_t len) {
  mpz_t result;
  if (!mpz_fits_ulong_p(off)) {
    if (mpz_sgn(off) < 0) {
      throw std::invalid_argument("Negative offset");
    }
    mpz_init(result);
    if (mpz_sgn(i) < 0) {
      mpz_set_si(result, -1);
    }
    return move_int(result);
  }
  if (!mpz_fits_ulong_p(len)) {
    throw std::invalid_argument("Length out of range");
  }
  unsigned long offlong = mpz_get_ui(off);
  unsigned long lenlong = mpz_get_ui(len);
  signed_extract(result, i, offlong, lenlong);
  return move_int(result);
}

int32_t hook_INT_size_int(mpz_t t) {
  return (int32_t)t->_mp_size;
}

uint64_t hook_INT_limb(mpz_t t, uint64_t i) {
  return (uint64_t)t->_mp_d[i];
}

void int_hash(mpz_t i, void *hasher) {
  int nlimbs = mpz_size(i);
  for (int j = 0; j < nlimbs; j++) {
    add_hash64(hasher, i[0]._mp_d[j]);
  }
}

static block * dotK = (block *)((((uint64_t)getTagForSymbolName("dotk{}")) << 32) | 1);

block *hook_INT_srand(mpz_t seed) {
  if (!mpz_fits_uint_p(seed)) {
    throw std::invalid_argument("Invalid seed");
  }
  srand(mpz_get_ui(seed));
  return dotK;
}

mpz_ptr hook_INT_rand(void) {
  mpz_t result;
  mpz_init_set_si(result, rand());
  return move_int(result);
}

}
