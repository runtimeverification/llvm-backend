#include <boost/test/unit_test.hpp>
#include <gmp.h>

extern "C" {
  bool hook_INT_tmod(mpz_t, mpz_t, mpz_t);
  bool hook_INT_emod(mpz_t, mpz_t, mpz_t);
  bool hook_INT_add(mpz_t, mpz_t, mpz_t);
  bool hook_INT_and(mpz_t, mpz_t, mpz_t);
  bool hook_INT_mul(mpz_t, mpz_t, mpz_t);
  bool hook_INT_sub(mpz_t, mpz_t, mpz_t);
  bool hook_INT_tdiv(mpz_t, mpz_t, mpz_t);
  bool hook_INT_ediv(mpz_t, mpz_t, mpz_t);
  bool hook_INT_shl(mpz_t, mpz_t, mpz_t);
  bool hook_INT_shr(mpz_t, mpz_t, mpz_t);
  bool hook_INT_pow(mpz_t, mpz_t, mpz_t);
  bool hook_INT_xor(mpz_t, mpz_t, mpz_t);
  bool hook_INT_or(mpz_t, mpz_t, mpz_t);
  bool hook_INT_max(mpz_t, mpz_t, mpz_t);
  bool hook_INT_min(mpz_t, mpz_t, mpz_t);
  bool hook_INT_powmod(mpz_t, mpz_t, mpz_t, mpz_t);
  bool hook_INT_bitRange(mpz_t, mpz_t, mpz_t, mpz_t);
  bool hook_INT_signExtendBitRange(mpz_t, mpz_t, mpz_t, mpz_t);
  bool hook_INT_not(mpz_t, mpz_t);
  bool hook_INT_abs(mpz_t, mpz_t);
  bool hook_INT_log(mpz_t, mpz_t);
  bool hook_INT_le(bool *, mpz_t, mpz_t);
  bool hook_INT_lt(bool *, mpz_t, mpz_t);
  bool hook_INT_eq(bool *, mpz_t, mpz_t);
  bool hook_INT_ne(bool *, mpz_t, mpz_t);
  bool hook_INT_ge(bool *, mpz_t, mpz_t);
  bool hook_INT_gt(bool *, mpz_t, mpz_t);
}

BOOST_AUTO_TEST_SUITE(IntTest)

BOOST_AUTO_TEST_CASE(tmod) {
  mpz_t a, b, result;
  mpz_init_set_ui(a, 7);
  mpz_init_set_ui(b, 3);
  BOOST_CHECK(hook_INT_tmod(result, a, b));
  BOOST_CHECK_EQUAL(mpz_cmp_ui(result, 1), 0);
  mpz_set_si(a, -7);
  mpz_clear(result);
  BOOST_CHECK(hook_INT_tmod(result, a, b));
  BOOST_CHECK_EQUAL(mpz_cmp_si(result, -1), 0);
  mpz_clear(result);
  mpz_set_si(b, 0);
  BOOST_CHECK(!hook_INT_tmod(result, a, b));
  mpz_clear(a);
  mpz_clear(b);
}

BOOST_AUTO_TEST_CASE(emod) {
  mpz_t a, b, result;
  mpz_init_set_ui(a, 7);
  mpz_init_set_ui(b, 3);
  BOOST_CHECK(hook_INT_emod(result, a, b));
  BOOST_CHECK_EQUAL(mpz_cmp_ui(result, 1), 0);
  mpz_set_si(a, -7);
  mpz_clear(result);
  BOOST_CHECK(hook_INT_emod(result, a, b));
  BOOST_CHECK_EQUAL(mpz_cmp_si(result, 2), 0);
  mpz_clear(result);
  mpz_set_si(b, 0);
  BOOST_CHECK(!hook_INT_emod(result, a, b));
  mpz_clear(a);
  mpz_clear(b);
}

BOOST_AUTO_TEST_CASE(add) {
  mpz_t a, b, result;
  mpz_init_set_ui(a, 7);
  mpz_init_set_ui(b, 3);
  BOOST_CHECK(hook_INT_add(result, a, b));
  BOOST_CHECK_EQUAL(mpz_cmp_ui(result, 10), 0);
  mpz_clear(result);
  mpz_clear(a);
  mpz_clear(b);
}

BOOST_AUTO_TEST_CASE(le) {
  mpz_t a, b;
  bool result = false;
  mpz_init_set_ui(a, 2);
  mpz_init_set_ui(b, 3);
  BOOST_CHECK(hook_INT_le(&result, a, b));
  BOOST_CHECK(result);
  result = false;
  mpz_set_si(a, 3);
  BOOST_CHECK(hook_INT_le(&result, a, b));
  BOOST_CHECK(result);
  mpz_clear(a);
  mpz_clear(b);
}

BOOST_AUTO_TEST_CASE(eq) {
  mpz_t a, b;
  bool result = true;
  mpz_init_set_ui(a, 2);
  mpz_init_set_ui(b, 3);
  BOOST_CHECK(hook_INT_eq(&result, a, b));
  BOOST_CHECK(!result);
  result = false;
  mpz_set_si(a, 3);
  BOOST_CHECK(hook_INT_eq(&result, a, b));
  BOOST_CHECK(result);
  mpz_clear(a);
  mpz_clear(b);
}

BOOST_AUTO_TEST_CASE(ne) {
  mpz_t a, b;
  bool result = false;
  mpz_init_set_ui(a, 2);
  mpz_init_set_ui(b, 3);
  BOOST_CHECK(hook_INT_ne(&result, a, b));
  BOOST_CHECK(result);
  result = true;
  mpz_set_si(a, 3);
  BOOST_CHECK(hook_INT_ne(&result, a, b));
  BOOST_CHECK(!result);
  mpz_clear(a);
  mpz_clear(b);
}

BOOST_AUTO_TEST_CASE(bitwise_and) {
  mpz_t a, b, result;
  mpz_init_set_ui(a, 13);
  mpz_init_set_ui(b, 3);
  BOOST_CHECK(hook_INT_and(result, a, b));
  BOOST_CHECK_EQUAL(mpz_cmp_ui(result, 1), 0);
  mpz_clear(result);
  mpz_clear(a);
  mpz_clear(b);
}

BOOST_AUTO_TEST_CASE(mul) {
  mpz_t a, b, result;
  mpz_init_set_ui(a, 7);
  mpz_init_set_ui(b, 3);
  BOOST_CHECK(hook_INT_mul(result, a, b));
  BOOST_CHECK_EQUAL(mpz_cmp_ui(result, 21), 0);
  mpz_clear(result);
  mpz_clear(a);
  mpz_clear(b);
}

BOOST_AUTO_TEST_CASE(sub) {
  mpz_t a, b, result;
  mpz_init_set_ui(a, 7);
  mpz_init_set_ui(b, 3);
  BOOST_CHECK(hook_INT_sub(result, a, b));
  BOOST_CHECK_EQUAL(mpz_cmp_ui(result, 4), 0);
  mpz_clear(result);
  mpz_clear(a);
  mpz_clear(b);
}

BOOST_AUTO_TEST_CASE(tdiv) {
  mpz_t a, b, result;
  mpz_init_set_ui(a, 7);
  mpz_init_set_ui(b, 3);
  BOOST_CHECK(hook_INT_tdiv(result, a, b));
  BOOST_CHECK_EQUAL(mpz_cmp_ui(result, 2), 0);
  mpz_set_si(b, -3);
  mpz_clear(result);
  BOOST_CHECK(hook_INT_tdiv(result, a, b));
  BOOST_CHECK_EQUAL(mpz_cmp_si(result, -2), 0);
  mpz_clear(result);
  mpz_set_si(b, 0);
  BOOST_CHECK(!hook_INT_tdiv(result, a, b));
  mpz_clear(a);
  mpz_clear(b);
}

BOOST_AUTO_TEST_CASE(ediv) {
  mpz_t a, b, result;
  mpz_init_set_ui(a, 7);
  mpz_init_set_ui(b, 3);
  BOOST_CHECK(hook_INT_ediv(result, a, b));
  BOOST_CHECK_EQUAL(mpz_cmp_ui(result, 2), 0);
  mpz_set_si(a, -7);
  mpz_set_si(b, -3);
  mpz_clear(result);
  BOOST_CHECK(hook_INT_ediv(result, a, b));
  BOOST_CHECK_EQUAL(mpz_cmp_si(result, 3), 0);
  mpz_clear(result);
  mpz_set_si(b, 0);
  BOOST_CHECK(!hook_INT_ediv(result, a, b));
  mpz_clear(a);
  mpz_clear(b);
}

BOOST_AUTO_TEST_CASE(shl) {
  mpz_t a, b, result;
  mpz_init_set_ui(a, 7);
  mpz_init_set_ui(b, 3);
  BOOST_CHECK(hook_INT_shl(result, a, b));
  BOOST_CHECK_EQUAL(mpz_cmp_ui(result, 56), 0);
  mpz_set_si(b, -3);
  mpz_clear(result);
  BOOST_CHECK(!hook_INT_shl(result, a, b));
  mpz_clear(a);
  mpz_clear(b);
}

BOOST_AUTO_TEST_CASE(lt) {
  mpz_t a, b;
  bool result = false;
  mpz_init_set_ui(a, 2);
  mpz_init_set_ui(b, 3);
  BOOST_CHECK(hook_INT_lt(&result, a, b));
  BOOST_CHECK(result);
  result = true;
  mpz_set_si(a, 3);
  BOOST_CHECK(hook_INT_lt(&result, a, b));
  BOOST_CHECK(!result);
  mpz_clear(a);
  mpz_clear(b);
}

BOOST_AUTO_TEST_CASE(ge) {
  mpz_t a, b;
  bool result = false;
  mpz_init_set_ui(a, 4);
  mpz_init_set_ui(b, 3);
  BOOST_CHECK(hook_INT_ge(&result, a, b));
  BOOST_CHECK(result);
  result = false;
  mpz_set_si(a, 3);
  BOOST_CHECK(hook_INT_ge(&result, a, b));
  BOOST_CHECK(result);
  mpz_clear(a);
  mpz_clear(b);
}

BOOST_AUTO_TEST_CASE(shr) {
  mpz_t a, b, result;
  mpz_init_set_ui(a, 21);
  mpz_init_set_ui(b, 3);
  BOOST_CHECK(hook_INT_shr(result, a, b));
  BOOST_CHECK_EQUAL(mpz_cmp_ui(result, 2), 0);
  mpz_set_si(b, -3);
  mpz_clear(result);
  BOOST_CHECK(!hook_INT_shr(result, a, b));
  mpz_set_ui(b, 1);
  mpz_mul_2exp(b, b, 64);
  BOOST_CHECK(hook_INT_shr(result, a, b));
  BOOST_CHECK_EQUAL(mpz_cmp_ui(result, 0), 0);
  mpz_clear(result);
  mpz_set_si(a, -21);
  BOOST_CHECK(hook_INT_shr(result, a, b));
  BOOST_CHECK_EQUAL(mpz_cmp_si(result, -1), 0);
  mpz_clear(result);
  mpz_clear(a);
  mpz_clear(b);
}

BOOST_AUTO_TEST_CASE(gt) {
  mpz_t a, b;
  bool result = false;
  mpz_init_set_ui(a, 4);
  mpz_init_set_ui(b, 3);
  BOOST_CHECK(hook_INT_gt(&result, a, b));
  BOOST_CHECK(result);
  result = true;
  mpz_set_si(a, 3);
  BOOST_CHECK(hook_INT_gt(&result, a, b));
  BOOST_CHECK(!result);
  mpz_clear(a);
  mpz_clear(b);
}

BOOST_AUTO_TEST_CASE(pow) {
  mpz_t a, b, result;
  mpz_init_set_ui(a, 7);
  mpz_init_set_ui(b, 3);
  BOOST_CHECK(hook_INT_pow(result, a, b));
  BOOST_CHECK_EQUAL(mpz_cmp_ui(result, 343), 0);
  mpz_set_si(b, -3);
  mpz_clear(result);
  BOOST_CHECK(!hook_INT_pow(result, a, b));
  mpz_clear(a);
  mpz_clear(b);
}

BOOST_AUTO_TEST_CASE(powmod) {
  mpz_t a, b, mod, result;
  mpz_init_set_ui(a, 5);
  mpz_init_set_ui(b, 2);
  mpz_init_set_ui(mod, 3);
  BOOST_CHECK(hook_INT_powmod(result, a, b, mod));
  BOOST_CHECK_EQUAL(mpz_cmp_ui(result, 1), 0);
  mpz_set_si(b, -3);
  mpz_clear(result);
  BOOST_CHECK(hook_INT_powmod(result, a, b, mod));
  BOOST_CHECK_EQUAL(mpz_cmp_ui(result, 2), 0);
  mpz_clear(result);
  mpz_set_si(mod, 5);
  BOOST_CHECK(!hook_INT_powmod(result, a, b, mod));
  mpz_clear(a);
  mpz_clear(b);
}

BOOST_AUTO_TEST_CASE(bitwise_xor) {
  mpz_t a, b, result;
  mpz_init_set_ui(a, 13);
  mpz_init_set_ui(b, 3);
  BOOST_CHECK(hook_INT_xor(result, a, b));
  BOOST_CHECK_EQUAL(mpz_cmp_ui(result, 14), 0);
  mpz_clear(result);
  mpz_clear(a);
  mpz_clear(b);
}

BOOST_AUTO_TEST_CASE(bitwise_or) {
  mpz_t a, b, result;
  mpz_init_set_ui(a, 13);
  mpz_init_set_ui(b, 3);
  BOOST_CHECK(hook_INT_or(result, a, b));
  BOOST_CHECK_EQUAL(mpz_cmp_ui(result, 15), 0);
  mpz_clear(result);
  mpz_clear(a);
  mpz_clear(b);
}

BOOST_AUTO_TEST_CASE(bitwise_not) {
  mpz_t a, result;
  mpz_init_set_ui(a, 7);
  BOOST_CHECK(hook_INT_not(result, a));
  BOOST_CHECK_EQUAL(mpz_cmp_si(result, -8), 0);
  mpz_clear(result);
  mpz_clear(a);
}

BOOST_AUTO_TEST_CASE(abs) {
  mpz_t a, result;
  mpz_init_set_si(a, -7);
  BOOST_CHECK(hook_INT_abs(result, a));
  BOOST_CHECK_EQUAL(mpz_cmp_si(result, 7), 0);
  mpz_clear(result);
  mpz_clear(a);
}

BOOST_AUTO_TEST_CASE(max) {
  mpz_t a, b, result;
  mpz_init_set_ui(a, 2);
  mpz_init_set_ui(b, 3);
  BOOST_CHECK(hook_INT_max(result, a, b));
  BOOST_CHECK_EQUAL(mpz_cmp_ui(result, 3), 0);
  mpz_clear(result);
  mpz_set_si(b, 2);
  BOOST_CHECK(hook_INT_max(result, a, b));
  BOOST_CHECK_EQUAL(mpz_cmp_ui(result, 2), 0);
  mpz_clear(result);
  mpz_clear(a);
  mpz_clear(b);
}

BOOST_AUTO_TEST_CASE(min) {
  mpz_t a, b, result;
  mpz_init_set_ui(a, 2);
  mpz_init_set_ui(b, 3);
  BOOST_CHECK(hook_INT_min(result, a, b));
  BOOST_CHECK_EQUAL(mpz_cmp_ui(result, 2), 0);
  mpz_clear(result);
  mpz_set_si(a, 3);
  BOOST_CHECK(hook_INT_min(result, a, b));
  BOOST_CHECK_EQUAL(mpz_cmp_ui(result, 3), 0);
  mpz_clear(result);
  mpz_clear(a);
  mpz_clear(b);
}

BOOST_AUTO_TEST_CASE(log) {
  mpz_t a, result;
  mpz_init_set_ui(a, 0);
  BOOST_CHECK(!hook_INT_log(result, a));
  mpz_set_ui(a, 256);
  BOOST_CHECK(hook_INT_log(result, a));
  BOOST_CHECK_EQUAL(mpz_cmp_ui(result, 8), 0);
  mpz_set_ui(a, 255);
  mpz_clear(result);
  BOOST_CHECK(hook_INT_log(result, a));
  BOOST_CHECK_EQUAL(mpz_cmp_ui(result, 7), 0);
  mpz_clear(result);
  mpz_clear(a);
}

BOOST_AUTO_TEST_CASE(bitRange) {
  mpz_t i, off, len, result;
  mpz_init_set_ui(i, 127);
  mpz_init_set_ui(off, 0);
  mpz_init_set_ui(len, 8);
  BOOST_CHECK(hook_INT_bitRange(result, i, off, len));
  BOOST_CHECK_EQUAL(mpz_cmp_ui(result, 127), 0);
  mpz_clear(result);
  mpz_set_ui(i, 255);
  BOOST_CHECK(hook_INT_bitRange(result, i, off, len));
  BOOST_CHECK_EQUAL(mpz_cmp_ui(result, 255), 0);
  mpz_clear(result);
  mpz_set_ui(i, 128);
  mpz_set_ui(off, 1);
  mpz_set_ui(len, 7);
  BOOST_CHECK(hook_INT_bitRange(result, i, off, len));
  BOOST_CHECK_EQUAL(mpz_cmp_ui(result, 64), 0);
  mpz_clear(result);
  mpz_set_ui(i, 129);
  mpz_set_ui(len, 5);
  BOOST_CHECK(hook_INT_bitRange(result, i, off, len));
  BOOST_CHECK_EQUAL(mpz_cmp_ui(result, 0), 0);
  mpz_clear(result);
  mpz_set_ui(i, 1);
  mpz_mul_2exp(i, i, 256);
  mpz_sub_ui(i, i, 1);
  mpz_set_ui(off, 1);
  mpz_mul_2exp(off, off, 64);
  mpz_set_ui(len, 1);
  BOOST_CHECK(hook_INT_bitRange(result, i, off, len));
  BOOST_CHECK_EQUAL(mpz_cmp_ui(result, 0), 0);
  mpz_clear(result);
  mpz_set_ui(i, 128);
  mpz_set_ui(off, 0);
  mpz_set_ui(len, 0);
  BOOST_CHECK(hook_INT_bitRange(result, i, off, len));
  BOOST_CHECK_EQUAL(mpz_cmp_ui(result, 0), 0);
  mpz_clear(result);
  mpz_set_si(len, -1);
  BOOST_CHECK(!hook_INT_bitRange(result, i, off, len));
  mpz_set_ui(len, 8);
  mpz_set_si(off, -1);
  BOOST_CHECK(!hook_INT_bitRange(result, i, off, len));
  mpz_set_ui(off, 1);
  mpz_mul_2exp(off, off, 64);
  BOOST_CHECK(hook_INT_bitRange(result, i, off, len));
  BOOST_CHECK_EQUAL(mpz_cmp_ui(result, 0), 0);
  mpz_clear(result);
  mpz_set_si(i, -128);
  BOOST_CHECK(hook_INT_bitRange(result, i, off, len));
  BOOST_CHECK_EQUAL(mpz_cmp_ui(result, 255), 0);
  mpz_clear(result);
  mpz_clear(i);
  mpz_clear(off);
  mpz_clear(len);
}

BOOST_AUTO_TEST_CASE(signExtendBitRange) {
  mpz_t i, off, len, result;
  mpz_init_set_ui(i, 255);
  mpz_init_set_ui(off, 0);
  mpz_init_set_ui(len, 8);
  BOOST_CHECK(hook_INT_signExtendBitRange(result, i, off, len));
  BOOST_CHECK_EQUAL(mpz_cmp_si(result, -1), 0);
  mpz_clear(result);
  mpz_set_ui(i, 127);
  BOOST_CHECK(hook_INT_signExtendBitRange(result, i, off, len));
  BOOST_CHECK_EQUAL(mpz_cmp_ui(result, 127), 0);
  mpz_clear(result);
  mpz_set_ui(i, 128);
  mpz_set_ui(off, 1);
  mpz_set_ui(len, 7);
  BOOST_CHECK(hook_INT_signExtendBitRange(result, i, off, len));
  BOOST_CHECK_EQUAL(mpz_cmp_si(result, -64), 0);
  mpz_clear(result);
  mpz_set_ui(i, 129);
  mpz_set_ui(len, 5);
  BOOST_CHECK(hook_INT_signExtendBitRange(result, i, off, len));
  BOOST_CHECK_EQUAL(mpz_cmp_ui(result, 0), 0);
  mpz_clear(result);
  mpz_set_ui(i, 1);
  mpz_mul_2exp(i, i, 256);
  mpz_sub_ui(i, i, 1);
  mpz_set_ui(off, 1);
  mpz_mul_2exp(off, off, 64);
  mpz_set_ui(len, 1);
  BOOST_CHECK(hook_INT_signExtendBitRange(result, i, off, len));
  BOOST_CHECK_EQUAL(mpz_cmp_ui(result, -0), 0);
  mpz_clear(result);
  mpz_set_ui(i, 128);
  mpz_set_ui(off, 0);
  mpz_set_ui(len, 0);
  BOOST_CHECK(hook_INT_signExtendBitRange(result, i, off, len));
  BOOST_CHECK_EQUAL(mpz_cmp_ui(result, 0), 0);
  mpz_clear(result);
  mpz_set_si(len, -1);
  BOOST_CHECK(!hook_INT_signExtendBitRange(result, i, off, len));
  mpz_set_ui(len, 8);
  mpz_set_si(off, -1);
  BOOST_CHECK(!hook_INT_signExtendBitRange(result, i, off, len));
  mpz_set_ui(off, 1);
  mpz_mul_2exp(off, off, 64);
  BOOST_CHECK(hook_INT_signExtendBitRange(result, i, off, len));
  BOOST_CHECK_EQUAL(mpz_cmp_ui(result, 0), 0);
  mpz_clear(result);
  mpz_set_si(i, -128);
  BOOST_CHECK(hook_INT_signExtendBitRange(result, i, off, len));
  BOOST_CHECK_EQUAL(mpz_cmp_si(result, -1), 0);
  mpz_clear(result);
  mpz_clear(i);
  mpz_clear(off);
  mpz_clear(len);
}

BOOST_AUTO_TEST_SUITE_END()
