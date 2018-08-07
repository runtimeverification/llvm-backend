#include<gmp.h>
#include<stdbool.h>

bool hook_INT_tmod(mpz_t result, mpz_t a, mpz_t b) {
  if (mpz_sgn(b) == 0) {
    return false;
  }
  mpz_init(result);
  mpz_tdiv_r(result, a, b);
  return true;
}

bool hook_INT_emod(mpz_t result, mpz_t a, mpz_t b) {
  if (mpz_sgn(b) == 0) {
    return false;
  }
  mpz_init(result);
  mpz_tdiv_r(result, a, b);
  if (mpz_sgn(result) >= 0) {
    return true;
  }
  mpz_t absb;
  mpz_init(absb);
  mpz_abs(absb, b);
  mpz_add(result, result, absb);
  mpz_clear(absb);
  return true;
}

bool hook_INT_add(mpz_t result, mpz_t a, mpz_t b) {
  mpz_init(result);
  mpz_add(result, a, b);
  return true;
}

bool hook_INT_le(bool *result, mpz_t a, mpz_t b) {
  *result = mpz_cmp(a, b) <= 0;
  return true;
}

bool hook_INT_eq(bool *result, mpz_t a, mpz_t b) {
  *result = mpz_cmp(a, b) == 0;
  return true;
}

bool hook_INT_ne(bool *result, mpz_t a, mpz_t b) {
  *result = mpz_cmp(a, b) != 0;
  return true;
}

bool hook_INT_and(mpz_t result, mpz_t a, mpz_t b) {
  mpz_init(result);
  mpz_and(result, a, b);
  return true;
}

bool hook_INT_mul(mpz_t result, mpz_t a, mpz_t b) {
  mpz_init(result);
  mpz_mul(result, a, b);
  return true;
}

bool hook_INT_sub(mpz_t result, mpz_t a, mpz_t b) {
  mpz_init(result);
  mpz_sub(result, a, b);
  return true;
}

bool hook_INT_tdiv(mpz_t result, mpz_t a, mpz_t b) {
  if (mpz_sgn(b) == 0) {
    return false;
  }
  mpz_init(result);
  mpz_tdiv_q(result, a, b);
  return true;
}

bool hook_INT_ediv(mpz_t result, mpz_t a, mpz_t b) {
  if (mpz_sgn(b) == 0) {
    return false;
  }
  mpz_init(result);
  if (mpz_sgn(b) >= 0) {
    mpz_fdiv_q(result, a, b);
  } else {
    mpz_cdiv_q(result, a, b);
  }
  return true;
}

bool hook_INT_shl(mpz_t result, mpz_t a, mpz_t b) {
  if (!mpz_fits_ulong_p(b)) {
    return false;
  }
  mpz_init(result);
  unsigned long blong = mpz_get_ui(b);
  mpz_mul_2exp(result, a, blong);
  return true;
}

bool hook_INT_lt(bool *result, mpz_t a, mpz_t b) {
  *result = mpz_cmp(a, b) < 0;
  return true;
}

bool hook_INT_ge(bool *result, mpz_t a, mpz_t b) {
  *result = mpz_cmp(a, b) >= 0;
  return true;
}

bool hook_INT_shr(mpz_t result, mpz_t a, mpz_t b) {
  mpz_init(result);
  if (!mpz_fits_ulong_p(b)) {
    if (mpz_sgn(b) < 0) {
      return false;
    }
    if (mpz_sgn(a) < 0) {
      mpz_set_si(result, -1);
    }
    return true;
  }
  unsigned long blong = mpz_get_ui(b);
  mpz_fdiv_q_2exp(result, a, blong);
  return true;
}

bool hook_INT_gt(bool *result, mpz_t a, mpz_t b) {
  *result = mpz_cmp(a, b) > 0;
  return true;
}

bool hook_INT_pow(mpz_t result, mpz_t a, mpz_t b) {
  if (!mpz_fits_ulong_p(b)) {
    return false;
  }
  mpz_init(result);
  unsigned long blong = mpz_get_ui(b);
  mpz_pow_ui(result, a, blong);
  return true;
}

bool hook_INT_powmod(mpz_t result, mpz_t a, mpz_t b, mpz_t mod) {
  mpz_init(result);
  if (mpz_sgn(b) < 0) {
    mpz_gcd(result, a, mod);
    if (mpz_cmp_ui(result, 1) != 0) {
      mpz_clear(result);
      return false;
    }
  }
  mpz_powm(result, a, b, mod);
  return true;
}

bool hook_INT_xor(mpz_t result, mpz_t a, mpz_t b) {
  mpz_init(result);
  mpz_xor(result, a, b);
  return true;
}

bool hook_INT_or(mpz_t result, mpz_t a, mpz_t b) {
  mpz_init(result);
  mpz_ior(result, a, b);
  return true;
}

bool hook_INT_not(mpz_t result, mpz_t a) {
  mpz_init(result);
  mpz_com(result, a);
  return true;
}

bool hook_INT_abs(mpz_t result, mpz_t a) {
  mpz_init(result);
  mpz_abs(result, a);
  return true;
}

bool hook_INT_max(mpz_t result, mpz_t a, mpz_t b) {
  mpz_init(result);
  if (mpz_cmp(a, b) >= 0) {
    mpz_set(result, a);
  } else {
    mpz_set(result, b);
  }
  return true;
}

bool hook_INT_min(mpz_t result, mpz_t a, mpz_t b) {
  mpz_init(result);
  if (mpz_cmp(a, b) <= 0) {
    mpz_set(result, a);
  } else {
    mpz_set(result, b);
  }
  return true;
}

bool hook_INT_log(mpz_t result, mpz_t a) {
  if (mpz_sgn(a) <= 0) {
    return false;
  }
  mpz_init(result);
  size_t log = mpz_sizeinbase(a, 2) - 1;
  mpz_set_ui(result, log);
  return true;
}

#define LIMB_BITS (sizeof(mp_limb_t) * 8)

void extract(mpz_t result, mpz_t i, size_t off, size_t len) {
  size_t size = (len + LIMB_BITS - 1) / LIMB_BITS;
  mpz_init2(result, len+LIMB_BITS);
  size_t off_words = off / LIMB_BITS;
  size_t off_bits = off % LIMB_BITS;
  size_t num_limbs = mpz_size(i);
  size_t copy_size = num_limbs - off_words;
  if (copy_size > size + 1) {
    copy_size = size + 1;
  }
  if (copy_size > 0) {
    if (off_bits) {
      mpn_rshift(result->_mp_d, i->_mp_d + off_words, copy_size, off_bits);
    }
    else {
      mpn_copyi(result->_mp_d, i->_mp_d, copy_size);
    }
  } else {
    copy_size = 0;
  }
  if (mpz_sgn(i) < 0) {
    mpn_com(result->_mp_d, result->_mp_d, size);
    mpn_add_1(result->_mp_d, result->_mp_d, size, 1);
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

bool hook_INT_bitRange(mpz_t result, mpz_t i, mpz_t off, mpz_t len) {
  if (mpz_sgn(len) == 0) {
    mpz_init(result);
    return true;
  }
  if (!mpz_fits_ulong_p(len)) {
    return false;
  }
  unsigned long lenlong = mpz_get_ui(len);
  if (!mpz_fits_ulong_p(off)) {
    if (mpz_sgn(off) < 0) {
      return false;
    }
    mpz_init(result);
    if (mpz_sgn(i) < 0) {
      mpz_set_ui(result, 1);
      mpz_mul_2exp(result, result, lenlong);
      mpz_sub_ui(result, result, 1);
    }
    return true;
  }
  unsigned long offlong = mpz_get_ui(off);
  extract(result, i, offlong, lenlong);
  return true;
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

bool hook_INT_signExtendBitRange(mpz_t result, mpz_t i, mpz_t off, mpz_t len) {
  if (!mpz_fits_ulong_p(off)) {
    if (mpz_sgn(off) < 0) {
      return false;
    }
    mpz_init(result);
    if (mpz_sgn(i) < 0) {
      mpz_set_si(result, -1);
    }
    return true;
  }
  if (!mpz_fits_ulong_p(len)) {
    return false;
  }
  unsigned long offlong = mpz_get_ui(off);
  unsigned long lenlong = mpz_get_ui(len);
  signed_extract(result, i, offlong, lenlong);
  return true;
}
