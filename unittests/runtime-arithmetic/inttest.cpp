#include <boost/test/unit_test.hpp>
#include <gmp.h>

#include "runtime/header.h"

extern "C" {
  mpz_ptr hook_INT_tmod(mpz_t, mpz_t);
  mpz_ptr hook_INT_emod(mpz_t, mpz_t);
  mpz_ptr hook_INT_add(mpz_t, mpz_t);
  mpz_ptr hook_INT_and(mpz_t, mpz_t);
  mpz_ptr hook_INT_mul(mpz_t, mpz_t);
  mpz_ptr hook_INT_sub(mpz_t, mpz_t);
  mpz_ptr hook_INT_tdiv(mpz_t, mpz_t);
  mpz_ptr hook_INT_ediv(mpz_t, mpz_t);
  mpz_ptr hook_INT_shl(mpz_t, mpz_t);
  mpz_ptr hook_INT_shr(mpz_t, mpz_t);
  mpz_ptr hook_INT_pow(mpz_t, mpz_t);
  mpz_ptr hook_INT_xor(mpz_t, mpz_t);
  mpz_ptr hook_INT_or(mpz_t, mpz_t);
  mpz_ptr hook_INT_max(mpz_t, mpz_t);
  mpz_ptr hook_INT_min(mpz_t, mpz_t);
  mpz_ptr hook_INT_powmod(mpz_t, mpz_t, mpz_t);
  mpz_ptr hook_INT_bitRange(mpz_t, mpz_t, mpz_t);
  mpz_ptr hook_INT_signExtendBitRange(mpz_t, mpz_t, mpz_t);
  mpz_ptr hook_INT_not(mpz_t);
  mpz_ptr hook_INT_abs(mpz_t);
  mpz_ptr hook_INT_log2(mpz_t);
  mpz_ptr hook_INT_rand(void);
  bool hook_INT_le(mpz_t, mpz_t);
  bool hook_INT_lt(mpz_t, mpz_t);
  bool hook_INT_eq(mpz_t, mpz_t);
  bool hook_INT_ne(mpz_t, mpz_t);
  bool hook_INT_ge(mpz_t, mpz_t);
  bool hook_INT_gt(mpz_t, mpz_t);
  block *hook_INT_srand(mpz_t);

  mpz_ptr move_int(mpz_t i) {
    mpz_ptr result = (mpz_ptr)malloc(sizeof(__mpz_struct));
    *result = *i;
    return result;
  }
  
  void add_hash64(void*, uint64_t) {}

  uint32_t getTagForSymbolName(const char *) {
    return 0;
  }
}

BOOST_AUTO_TEST_SUITE(IntTest)

BOOST_AUTO_TEST_CASE(tmod) {
  mpz_t a, b;
  mpz_ptr result;
  mpz_init_set_ui(a, 7);
  mpz_init_set_ui(b, 3);
  result = hook_INT_tmod(a, b);
  BOOST_CHECK_EQUAL(mpz_cmp_ui(result, 1), 0);
  mpz_set_si(a, -7);
  mpz_clear(result);
  free(result);
  result = hook_INT_tmod(a, b);
  BOOST_CHECK_EQUAL(mpz_cmp_si(result, -1), 0);
  mpz_clear(result);
  free(result);
  mpz_set_si(b, 0);
  BOOST_CHECK_THROW(hook_INT_tmod(a, b), std::invalid_argument);
  mpz_clear(a);
  mpz_clear(b);
}

BOOST_AUTO_TEST_CASE(emod) {
  mpz_t a, b;
  mpz_ptr result;
  mpz_init_set_ui(a, 7);
  mpz_init_set_ui(b, 3);
  result = hook_INT_emod(a, b);
  BOOST_CHECK_EQUAL(mpz_cmp_ui(result, 1), 0);
  mpz_set_si(a, -7);
  mpz_clear(result);
  free(result);
  result = hook_INT_emod(a, b);
  BOOST_CHECK_EQUAL(mpz_cmp_si(result, 2), 0);
  mpz_clear(result);
  free(result);
  mpz_set_si(b, 0);
  BOOST_CHECK_THROW(hook_INT_emod(a, b), std::invalid_argument);
  mpz_clear(a);
  mpz_clear(b);
}

BOOST_AUTO_TEST_CASE(add) {
  mpz_t a, b;
  mpz_ptr result;
  mpz_init_set_ui(a, 7);
  mpz_init_set_ui(b, 3);
  result = hook_INT_add(a, b);
  BOOST_CHECK_EQUAL(mpz_cmp_ui(result, 10), 0);
  mpz_clear(result);
  free(result);
  mpz_clear(a);
  mpz_clear(b);
}

BOOST_AUTO_TEST_CASE(le) {
  mpz_t a, b;
  bool result;
  mpz_init_set_ui(a, 2);
  mpz_init_set_ui(b, 3);
  result = hook_INT_le(a, b);
  BOOST_CHECK(result);
  result = false;
  mpz_set_si(a, 3);
  result = hook_INT_le(a, b);
  BOOST_CHECK(result);
  mpz_clear(a);
  mpz_clear(b);
}

BOOST_AUTO_TEST_CASE(eq) {
  mpz_t a, b;
  bool result;
  mpz_init_set_ui(a, 2);
  mpz_init_set_ui(b, 3);
  result = hook_INT_eq(a, b);
  BOOST_CHECK(!result);
  result = false;
  mpz_set_si(a, 3);
  result = hook_INT_eq(a, b);
  BOOST_CHECK(result);
  mpz_clear(a);
  mpz_clear(b);
}

BOOST_AUTO_TEST_CASE(ne) {
  mpz_t a, b;
  bool result;
  mpz_init_set_ui(a, 2);
  mpz_init_set_ui(b, 3);
  result = hook_INT_ne(a, b);
  BOOST_CHECK(result);
  result = true;
  mpz_set_si(a, 3);
  result = hook_INT_ne(a, b);
  BOOST_CHECK(!result);
  mpz_clear(a);
  mpz_clear(b);
}

BOOST_AUTO_TEST_CASE(bitwise_and) {
  mpz_t a, b;
  mpz_ptr result;
  mpz_init_set_ui(a, 13);
  mpz_init_set_ui(b, 3);
  result = hook_INT_and(a, b);
  BOOST_CHECK_EQUAL(mpz_cmp_ui(result, 1), 0);
  mpz_clear(result);
  free(result);
  mpz_clear(a);
  mpz_clear(b);
}

BOOST_AUTO_TEST_CASE(mul) {
  mpz_t a, b;
  mpz_ptr result;
  mpz_init_set_ui(a, 7);
  mpz_init_set_ui(b, 3);
  result = hook_INT_mul(a, b);
  BOOST_CHECK_EQUAL(mpz_cmp_ui(result, 21), 0);
  mpz_clear(result);
  free(result);
  mpz_clear(a);
  mpz_clear(b);
}

BOOST_AUTO_TEST_CASE(sub) {
  mpz_t a, b;
  mpz_ptr result;
  mpz_init_set_ui(a, 7);
  mpz_init_set_ui(b, 3);
  result = hook_INT_sub(a, b);
  BOOST_CHECK_EQUAL(mpz_cmp_ui(result, 4), 0);
  mpz_clear(result);
  free(result);
  mpz_clear(a);
  mpz_clear(b);
}

BOOST_AUTO_TEST_CASE(tdiv) {
  mpz_t a, b;
  mpz_ptr result;
  mpz_init_set_ui(a, 7);
  mpz_init_set_ui(b, 3);
  result = hook_INT_tdiv(a, b);
  BOOST_CHECK_EQUAL(mpz_cmp_ui(result, 2), 0);
  mpz_set_si(b, -3);
  mpz_clear(result);
  free(result);
  result = hook_INT_tdiv(a, b);
  BOOST_CHECK_EQUAL(mpz_cmp_si(result, -2), 0);
  mpz_clear(result);
  free(result);
  mpz_set_si(b, 0);
  BOOST_CHECK_THROW(hook_INT_tdiv(a, b), std::invalid_argument);
  mpz_clear(a);
  mpz_clear(b);
}

BOOST_AUTO_TEST_CASE(ediv) {
  mpz_t a, b;
  mpz_ptr result;
  mpz_init_set_ui(a, 7);
  mpz_init_set_ui(b, 3);
  result = hook_INT_ediv(a, b);
  BOOST_CHECK_EQUAL(mpz_cmp_ui(result, 2), 0);
  mpz_set_si(a, -7);
  mpz_set_si(b, -3);
  mpz_clear(result);
  free(result);
  result = hook_INT_ediv(a, b);
  BOOST_CHECK_EQUAL(mpz_cmp_si(result, 3), 0);
  mpz_clear(result);
  free(result);
  mpz_set_si(b, 0);
  BOOST_CHECK_THROW(hook_INT_ediv(a, b), std::invalid_argument);
  mpz_clear(a);
  mpz_clear(b);
}

BOOST_AUTO_TEST_CASE(shl) {
  mpz_t a, b;
  mpz_ptr result;
  mpz_init_set_ui(a, 7);
  mpz_init_set_ui(b, 3);
  result = hook_INT_shl(a, b);
  BOOST_CHECK_EQUAL(mpz_cmp_ui(result, 56), 0);
  mpz_set_si(b, -3);
  mpz_clear(result);
  free(result);
  BOOST_CHECK_THROW(hook_INT_shl(a, b), std::invalid_argument);
  mpz_clear(a);
  mpz_clear(b);
}

BOOST_AUTO_TEST_CASE(lt) {
  mpz_t a, b;
  bool result;
  mpz_init_set_ui(a, 2);
  mpz_init_set_ui(b, 3);
  result = hook_INT_lt(a, b);
  BOOST_CHECK(result);
  result = true;
  mpz_set_si(a, 3);
  result = hook_INT_lt(a, b);
  BOOST_CHECK(!result);
  mpz_clear(a);
  mpz_clear(b);
}

BOOST_AUTO_TEST_CASE(ge) {
  mpz_t a, b;
  bool result;
  mpz_init_set_ui(a, 4);
  mpz_init_set_ui(b, 3);
  result = hook_INT_ge(a, b);
  BOOST_CHECK(result);
  result = false;
  mpz_set_si(a, 3);
  result = hook_INT_ge(a, b);
  BOOST_CHECK(result);
  mpz_clear(a);
  mpz_clear(b);
}

BOOST_AUTO_TEST_CASE(shr) {
  mpz_t a, b;
  mpz_ptr result;
  mpz_init_set_ui(a, 21);
  mpz_init_set_ui(b, 3);
  result = hook_INT_shr(a, b);
  BOOST_CHECK_EQUAL(mpz_cmp_ui(result, 2), 0);
  mpz_set_si(b, -3);
  mpz_clear(result);
  free(result);
  BOOST_CHECK_THROW(hook_INT_shr(a, b), std::invalid_argument);
  mpz_set_ui(b, 1);
  mpz_mul_2exp(b, b, 64);
  result = hook_INT_shr(a, b);
  BOOST_CHECK_EQUAL(mpz_cmp_ui(result, 0), 0);
  mpz_clear(result);
  free(result);
  mpz_set_si(a, -21);
  result = hook_INT_shr(a, b);
  BOOST_CHECK_EQUAL(mpz_cmp_si(result, -1), 0);
  mpz_clear(result);
  free(result);
  mpz_clear(a);
  mpz_clear(b);
}

BOOST_AUTO_TEST_CASE(gt) {
  mpz_t a, b;
  bool result;
  mpz_init_set_ui(a, 4);
  mpz_init_set_ui(b, 3);
  result = hook_INT_gt(a, b);
  BOOST_CHECK(result);
  result = true;
  mpz_set_si(a, 3);
  result = hook_INT_gt(a, b);
  BOOST_CHECK(!result);
  mpz_clear(a);
  mpz_clear(b);
}

BOOST_AUTO_TEST_CASE(pow) {
  mpz_t a, b;
  mpz_ptr result;
  mpz_init_set_ui(a, 7);
  mpz_init_set_ui(b, 3);
  result = hook_INT_pow(a, b);
  BOOST_CHECK_EQUAL(mpz_cmp_ui(result, 343), 0);
  mpz_set_si(b, -3);
  mpz_clear(result);
  free(result);
  BOOST_CHECK_THROW(hook_INT_pow(a, b), std::invalid_argument);
  mpz_clear(a);
  mpz_clear(b);
}

BOOST_AUTO_TEST_CASE(powmod) {
  mpz_t a, b, mod;
  mpz_ptr result;
  mpz_init_set_ui(a, 5);
  mpz_init_set_ui(b, 2);
  mpz_init_set_ui(mod, 3);
  result = hook_INT_powmod(a, b, mod);
  BOOST_CHECK_EQUAL(mpz_cmp_ui(result, 1), 0);
  mpz_set_si(b, -3);
  mpz_clear(result);
  free(result);
  result = hook_INT_powmod(a, b, mod);
  BOOST_CHECK_EQUAL(mpz_cmp_ui(result, 2), 0);
  mpz_clear(result);
  free(result);
  mpz_set_si(mod, 5);
  BOOST_CHECK_THROW(hook_INT_powmod(a, b, mod), std::invalid_argument);
  mpz_clear(a);
  mpz_clear(b);
}

BOOST_AUTO_TEST_CASE(bitwise_xor) {
  mpz_t a, b;
  mpz_ptr result;
  mpz_init_set_ui(a, 13);
  mpz_init_set_ui(b, 3);
  result = hook_INT_xor(a, b);
  BOOST_CHECK_EQUAL(mpz_cmp_ui(result, 14), 0);
  mpz_clear(result);
  free(result);
  mpz_clear(a);
  mpz_clear(b);
}

BOOST_AUTO_TEST_CASE(bitwise_or) {
  mpz_t a, b;
  mpz_ptr result;
  mpz_init_set_ui(a, 13);
  mpz_init_set_ui(b, 3);
  result = hook_INT_or(a, b);
  BOOST_CHECK_EQUAL(mpz_cmp_ui(result, 15), 0);
  mpz_clear(result);
  free(result);
  mpz_clear(a);
  mpz_clear(b);
}

BOOST_AUTO_TEST_CASE(bitwise_not) {
  mpz_t a;
  mpz_ptr result;
  mpz_init_set_ui(a, 7);
  result = hook_INT_not(a);
  BOOST_CHECK_EQUAL(mpz_cmp_si(result, -8), 0);
  mpz_clear(result);
  free(result);
  mpz_clear(a);
}

BOOST_AUTO_TEST_CASE(abs) {
  mpz_t a;
  mpz_ptr result;
  mpz_init_set_si(a, -7);
  result = hook_INT_abs(a);
  BOOST_CHECK_EQUAL(mpz_cmp_si(result, 7), 0);
  mpz_clear(result);
  free(result);
  mpz_clear(a);
}

BOOST_AUTO_TEST_CASE(max) {
  mpz_t a, b;
  mpz_ptr result;
  mpz_init_set_ui(a, 2);
  mpz_init_set_ui(b, 3);
  result = hook_INT_max(a, b);
  BOOST_CHECK_EQUAL(mpz_cmp_ui(result, 3), 0);
  mpz_clear(result);
  free(result);
  mpz_set_si(b, 2);
  result = hook_INT_max(a, b);
  BOOST_CHECK_EQUAL(mpz_cmp_ui(result, 2), 0);
  mpz_clear(result);
  free(result);
  mpz_clear(a);
  mpz_clear(b);
}

BOOST_AUTO_TEST_CASE(min) {
  mpz_t a, b;
  mpz_ptr result;
  mpz_init_set_ui(a, 2);
  mpz_init_set_ui(b, 3);
  result = hook_INT_min(a, b);
  BOOST_CHECK_EQUAL(mpz_cmp_ui(result, 2), 0);
  mpz_clear(result);
  free(result);
  mpz_set_si(a, 3);
  result = hook_INT_min(a, b);
  BOOST_CHECK_EQUAL(mpz_cmp_ui(result, 3), 0);
  mpz_clear(result);
  free(result);
  mpz_clear(a);
  mpz_clear(b);
}

BOOST_AUTO_TEST_CASE(log2) {
  mpz_t a;
  mpz_ptr result;
  mpz_init_set_ui(a, 0);
  BOOST_CHECK_THROW(hook_INT_log2(a), std::invalid_argument);
  mpz_set_ui(a, 256);
  result = hook_INT_log2(a);
  BOOST_CHECK_EQUAL(mpz_cmp_ui(result, 8), 0);
  mpz_set_ui(a, 255);
  mpz_clear(result);
  free(result);
  result = hook_INT_log2(a);
  BOOST_CHECK_EQUAL(mpz_cmp_ui(result, 7), 0);
  mpz_clear(result);
  free(result);
  mpz_clear(a);
}

BOOST_AUTO_TEST_CASE(bitRange) {
  mpz_t i, off, len;
  mpz_ptr result;
  mpz_init_set_ui(i, 127);
  mpz_init_set_ui(off, 0);
  mpz_init_set_ui(len, 8);
  result = hook_INT_bitRange(i, off, len);
  BOOST_CHECK_EQUAL(mpz_cmp_ui(result, 127), 0);
  mpz_clear(result);
  free(result);
  mpz_set_ui(i, 255);
  result = hook_INT_bitRange(i, off, len);
  BOOST_CHECK_EQUAL(mpz_cmp_ui(result, 255), 0);
  mpz_clear(result);
  free(result);
  mpz_set_ui(i, 128);
  mpz_set_ui(off, 1);
  mpz_set_ui(len, 7);
  result = hook_INT_bitRange(i, off, len);
  BOOST_CHECK_EQUAL(mpz_cmp_ui(result, 64), 0);
  mpz_clear(result);
  free(result);
  mpz_set_ui(i, 129);
  mpz_set_ui(len, 5);
  result = hook_INT_bitRange(i, off, len);
  BOOST_CHECK_EQUAL(mpz_cmp_ui(result, 0), 0);
  mpz_clear(result);
  free(result);
  mpz_set_ui(i, 1);
  mpz_mul_2exp(i, i, 256);
  mpz_sub_ui(i, i, 1);
  mpz_set_ui(off, 1);
  mpz_mul_2exp(off, off, 64);
  mpz_set_ui(len, 1);
  result = hook_INT_bitRange(i, off, len);
  BOOST_CHECK_EQUAL(mpz_cmp_ui(result, 0), 0);
  mpz_clear(result);
  free(result);
  mpz_set_ui(i, 128);
  mpz_set_ui(off, 0);
  mpz_set_ui(len, 0);
  result = hook_INT_bitRange(i, off, len);
  BOOST_CHECK_EQUAL(mpz_cmp_ui(result, 0), 0);
  mpz_clear(result);
  free(result);
  mpz_set_si(len, -1);
  BOOST_CHECK_THROW(hook_INT_bitRange(i, off, len), std::invalid_argument);
  mpz_set_ui(len, 8);
  mpz_set_si(off, -1);
  BOOST_CHECK_THROW(hook_INT_bitRange(i, off, len), std::invalid_argument);
  mpz_set_ui(off, 1);
  mpz_mul_2exp(off, off, 64);
  result = hook_INT_bitRange(i, off, len);
  BOOST_CHECK_EQUAL(mpz_cmp_ui(result, 0), 0);
  mpz_clear(result);
  free(result);
  mpz_set_si(i, -128);
  result = hook_INT_bitRange(i, off, len);
  BOOST_CHECK_EQUAL(mpz_cmp_ui(result, 255), 0);
  mpz_clear(result);
  free(result);
  mpz_set_ui(i, 0x8040201008040201);
  mpz_set_ui(off, 256);
  mpz_set_ui(len, 8);
  result = hook_INT_bitRange(i, off, len);
  BOOST_CHECK_EQUAL(mpz_cmp_ui(result, 0), 0);
  mpz_clear(result);
  free(result);
  mpz_set_str(i, "-710567042938717889665411037832208781722350888143921263584927239275773573551204588944105336352942349727184887589413944684473529682801526123805453895275517072855048781056", 10);
  mpz_set_ui(off, 32);
  mpz_set_ui(len, 8);
  result = hook_INT_bitRange(i, off, len);
  BOOST_CHECK_EQUAL(mpz_cmp_ui(result, 12), 0);
  mpz_clear(result);
  free(result);
  mpz_set_str(i, "697754608693466068295273213726275558775348389513141500672185545754018175722916164768735179047222610843044264325669307777729891642448846794142000", 10);
  mpz_set_ui(off, 64);
  mpz_set_ui(len, 8);
  result = hook_INT_bitRange(i, off, len);
  BOOST_CHECK_EQUAL(mpz_cmp_ui(result, 56), 0);
  mpz_clear(result);
  free(result);
  mpz_clear(i);
  mpz_clear(off);
  mpz_clear(len);
}

BOOST_AUTO_TEST_CASE(signExtendBitRange) {
  mpz_t i, off, len;
  mpz_ptr result;
  mpz_init_set_ui(i, 255);
  mpz_init_set_ui(off, 0);
  mpz_init_set_ui(len, 8);
  result = hook_INT_signExtendBitRange(i, off, len);
  BOOST_CHECK_EQUAL(mpz_cmp_si(result, -1), 0);
  mpz_clear(result);
  free(result);
  mpz_set_ui(i, 127);
  result = hook_INT_signExtendBitRange(i, off, len);
  BOOST_CHECK_EQUAL(mpz_cmp_ui(result, 127), 0);
  mpz_clear(result);
  free(result);
  mpz_set_ui(i, 128);
  mpz_set_ui(off, 1);
  mpz_set_ui(len, 7);
  result = hook_INT_signExtendBitRange(i, off, len);
  BOOST_CHECK_EQUAL(mpz_cmp_si(result, -64), 0);
  mpz_clear(result);
  free(result);
  mpz_set_ui(i, 129);
  mpz_set_ui(len, 5);
  result = hook_INT_signExtendBitRange(i, off, len);
  BOOST_CHECK_EQUAL(mpz_cmp_ui(result, 0), 0);
  mpz_clear(result);
  free(result);
  mpz_set_ui(i, 1);
  mpz_mul_2exp(i, i, 256);
  mpz_sub_ui(i, i, 1);
  mpz_set_ui(off, 1);
  mpz_mul_2exp(off, off, 64);
  mpz_set_ui(len, 1);
  result = hook_INT_signExtendBitRange(i, off, len);
  BOOST_CHECK_EQUAL(mpz_cmp_ui(result, -0), 0);
  mpz_clear(result);
  free(result);
  mpz_set_ui(i, 128);
  mpz_set_ui(off, 0);
  mpz_set_ui(len, 0);
  result = hook_INT_signExtendBitRange(i, off, len);
  BOOST_CHECK_EQUAL(mpz_cmp_ui(result, 0), 0);
  mpz_clear(result);
  free(result);
  mpz_set_si(len, -1);
  BOOST_CHECK_THROW(hook_INT_signExtendBitRange(i, off, len), std::invalid_argument);
  mpz_set_ui(len, 8);
  mpz_set_si(off, -1);
  BOOST_CHECK_THROW(hook_INT_signExtendBitRange(i, off, len), std::invalid_argument);
  mpz_set_ui(off, 1);
  mpz_mul_2exp(off, off, 64);
  result = hook_INT_signExtendBitRange(i, off, len);
  BOOST_CHECK_EQUAL(mpz_cmp_ui(result, 0), 0);
  mpz_clear(result);
  free(result);
  mpz_set_si(i, -128);
  result = hook_INT_signExtendBitRange(i, off, len);
  BOOST_CHECK_EQUAL(mpz_cmp_si(result, -1), 0);
  mpz_clear(result);
  free(result);
  mpz_clear(i);
  mpz_clear(off);
  mpz_clear(len);
}

BOOST_AUTO_TEST_CASE(rand) {
  mpz_t seed;
  mpz_init_set_ui(seed, 0);
  BOOST_CHECK_EQUAL((uintptr_t)hook_INT_srand(seed), 1);
  mpz_ptr result = hook_INT_rand();
  BOOST_CHECK_EQUAL(mpz_cmp_ui(result, 1804289383), 0);
}

BOOST_AUTO_TEST_SUITE_END()
