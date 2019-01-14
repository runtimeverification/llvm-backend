#include<gmp.h>
#include<mpfr.h>
#include<stdexcept>

#include "runtime/header.h"

#if MPFR_VERSION_MAJOR < 4
  #define mpfr_rootn_ui mpfr_root
#endif

static mpfr_exp_t emin(mpfr_exp_t e, mpfr_prec_t p) {
  return (- (1 << (e-1))) + 2;
}

static mpfr_exp_t emax(mpfr_exp_t e) {
  return (1 << (e-1)) - 1;
}

static mpfr_exp_t default_emax, default_emin;

/* Each floating point number contains a number of exponent bits (here e) and
 * a precision (p). Here we initialize the result of a floating point computation
 * with that exponent range and precision and then prepare mpfr to perform the calculation
 * by transferring ourselves to that exponent range. An overload also exists to
 * get the value from a floating * if one already exists in the arguments to the function. */
static void mpfr_enter(mpfr_prec_t p, mpfr_exp_t e, floating *result) {
  mpfr_init2(result->f, p);
  result->exp = e;
  default_emax = mpfr_get_emax();
  default_emin = mpfr_get_emin();
  mpfr_set_emin(emin(e, p)-p+2);
  mpfr_set_emax(emax(e)+1);
}

static void mpfr_enter(floating *arg, floating *result) {
  mpfr_prec_t p = mpfr_get_prec(arg->f);
  mpfr_enter(p, arg->exp, result);
}

/* Here we finalize the computation by ensuring that the value is correctly rounded into
 * The result exponent range, including subnormal arithmetic, and then restore the previous
 * values for emin and emax within mpfr. */
static void mpfr_leave(int t, floating *result) {
  t = mpfr_check_range(result->f, t, MPFR_RNDN);
  mpfr_subnormalize(result->f, t, MPFR_RNDN);
  mpfr_set_emin(default_emin);
  mpfr_set_emax(default_emax);
}

extern "C" {

floating *move_float(floating *);
mpz_ptr move_int(mpz_t);
void add_hash64(void *, uint64_t);
void *move_mint(mpz_t, uint64_t) {
  throw std::invalid_argument("not yet implemented");
}

floating *hook_FLOAT_ceil(floating *a) {
  floating result[1];
  mpfr_enter(a, result);
  int t = mpfr_ceil(result->f, a->f);
  mpfr_leave(t, result);
  return move_float(result); 
}

floating *hook_FLOAT_floor(floating *a) {
  floating result[1];
  mpfr_enter(a, result);
  int t = mpfr_floor(result->f, a->f);
  mpfr_leave(t, result);
  return move_float(result);
}

floating *hook_FLOAT_round(floating *a, mpz_t prec, mpz_t exp) {
  if (!mpz_fits_ulong_p(prec)) {
    throw std::invalid_argument("Precision out of range");
  }
  unsigned long uprec = mpz_get_ui(prec);
  if (!mpz_fits_ulong_p(exp)) {
    throw std::invalid_argument("Exponent out of range");
  }
  unsigned long uexp = mpz_get_ui(exp);
  floating result[1];
  mpfr_enter(uprec, uexp, result);
  int t = mpfr_set(result->f, a->f, MPFR_RNDN);
  mpfr_leave(t, result);
  return move_float(result);
}

mpz_ptr hook_FLOAT_float2int(floating *a) {
  if (!mpfr_number_p(a->f)) {
    throw std::invalid_argument("Not a finite number");
  }
  mpz_t result;
  mpz_init(result);
  mpfr_get_z(result, a->f, MPFR_RNDN);
  return move_int(result);
}

floating *hook_FLOAT_int2float(mpz_t a, mpz_t prec, mpz_t exp) {
  if (!mpz_fits_ulong_p(prec)) {
    throw std::invalid_argument("Precision out of range");
  }
  unsigned long uprec = mpz_get_ui(prec);
  if (!mpz_fits_ulong_p(exp)) {
    throw std::invalid_argument("Exponent out of range");
  }
  unsigned long uexp = mpz_get_ui(exp);
  floating result[1];
  mpfr_enter(uprec, uexp, result);
  int t = mpfr_set_z(result->f, a, MPFR_RNDN);
  mpfr_leave(t, result);
  return move_float(result);
}

floating *hook_FLOAT_sin(floating *a) {
  floating result[1];
  mpfr_enter(a, result);
  int t = mpfr_sin(result->f, a->f, MPFR_RNDN);
  mpfr_leave(t, result);
  return move_float(result);
}

floating *hook_FLOAT_cos(floating *a) {
  floating result[1];
  mpfr_enter(a, result);
  int t = mpfr_cos(result->f, a->f, MPFR_RNDN);
  mpfr_leave(t, result);
  return move_float(result);
}

floating *hook_FLOAT_tan(floating *a) {
  floating result[1];
  mpfr_enter(a, result);
  int t = mpfr_tan(result->f, a->f, MPFR_RNDN);
  mpfr_leave(t, result);
  return move_float(result);
}

floating *hook_FLOAT_sec(floating *a) {
  floating result[1];
  mpfr_enter(a, result);
  int t = mpfr_sec(result->f, a->f, MPFR_RNDN);
  mpfr_leave(t, result);
  return move_float(result);
}

floating *hook_FLOAT_csc(floating *a) {
  floating result[1];
  mpfr_enter(a, result);
  int t = mpfr_csc(result->f, a->f, MPFR_RNDN);
  mpfr_leave(t, result);
  return move_float(result);
}

floating *hook_FLOAT_cot(floating *a) {
  floating result[1];
  mpfr_enter(a, result);
  int t = mpfr_cot(result->f, a->f, MPFR_RNDN);
  mpfr_leave(t, result);
  return move_float(result);
}

floating *hook_FLOAT_asin(floating *a) {
  floating result[1];
  mpfr_enter(a, result);
  int t = mpfr_asin(result->f, a->f, MPFR_RNDN);
  mpfr_leave(t, result);
  return move_float(result);
}

floating *hook_FLOAT_acos(floating *a) {
  floating result[1];
  mpfr_enter(a, result);
  int t = mpfr_acos(result->f, a->f, MPFR_RNDN);
  mpfr_leave(t, result);
  return move_float(result);
}

floating *hook_FLOAT_atan(floating *a) {
  floating result[1];
  mpfr_enter(a, result);
  int t = mpfr_atan(result->f, a->f, MPFR_RNDN);
  mpfr_leave(t, result);
  return move_float(result);
}

floating *hook_FLOAT_atan2(floating *a, floating *b) {
  floating result[1];
  mpfr_enter(a, result);
  int t = mpfr_atan2(result->f, a->f, b->f, MPFR_RNDN);
  mpfr_leave(t, result);
  return move_float(result);
}

mpz_ptr hook_FLOAT_precision(floating *a) {
  mpz_t result;
  mpz_init_set_ui(result, mpfr_get_prec(a->f));
  return move_int(result);
}

mpz_ptr hook_FLOAT_exponentBits(floating *a) {
  mpz_t result;
  mpz_init_set_ui(result, a->exp);
  return move_int(result);
}

mpz_ptr hook_FLOAT_exponent(floating *a) {
  mpz_t result;
  mpz_init(result);
  mpfr_exp_t min = emin(a->exp, mpfr_get_prec(a->f));
  if (mpfr_regular_p(a->f)) {
    if (mpfr_get_exp(a->f) - 1 < min) {
      //subnormal
      mpz_set_si(result, min-1);
    } else {
      mpz_set_si(result, mpfr_get_exp(a->f)-1);
    }
  } else if (mpfr_zero_p(a->f)) {
    mpz_set_si(result, min-1);
  } else { // nan or infinity
    mpz_set_si(result, emax(a->exp)+1);
  }
  return move_int(result);
}

void *hook_FLOAT_significand(floating *a) {
  if (mpfr_nan_p(a->f)) {
    throw std::invalid_argument("NaN payload is undefined");
  }
  mpfr_prec_t prec = mpfr_get_prec(a->f);
  uint64_t len = (prec + 7) / 8;
  mpz_t z;
  mpz_init(z);
  if (mpfr_inf_p(a->f) || mpfr_zero_p(a->f)) {
    return move_mint(z, len);
  }
  mpfr_get_z_2exp(z, a->f);
  mpz_abs(z, z);
  mpfr_exp_t exp = mpfr_get_exp(a->f);
  mpfr_exp_t min = emin(a->exp, prec);
  if (exp - 1 < min) {
    //subnormal
    mpz_fdiv_q_2exp(z, z, min - (exp - 1));
  }
  return move_mint(z, len);
}

bool hook_FLOAT_isNaN(floating *a) {
  return mpfr_nan_p(a->f);
}

floating *hook_FLOAT_maxValue(mpz_t prec, mpz_t exp) {
  if (!mpz_fits_ulong_p(prec)) {
    throw std::invalid_argument("Precision out of range");
  }
  unsigned long uprec = mpz_get_ui(prec);
  if (!mpz_fits_ulong_p(exp)) {
    throw std::invalid_argument("Exponent out of range");
  }
  unsigned long uexp = mpz_get_ui(exp);
  floating result[1];
  mpfr_enter(uprec, uexp, result);
  mpfr_set_inf(result->f, 1);
  mpfr_nextbelow(result->f);
  mpfr_leave(0, result);
  return move_float(result);
}

floating *hook_FLOAT_minValue(mpz_t prec, mpz_t exp) {
  if (!mpz_fits_ulong_p(prec)) {
    throw std::invalid_argument("Precision out of range");
  }
  unsigned long uprec = mpz_get_ui(prec);
  if (!mpz_fits_ulong_p(exp)) {
    throw std::invalid_argument("Exponent out of range");
  }
  unsigned long uexp = mpz_get_ui(exp);
  floating result[1];
  mpfr_enter(uprec, uexp, result);
  mpfr_set_zero(result->f, 1);
  mpfr_nextabove(result->f);
  mpfr_leave(0, result);
  return move_float(result);
}

bool hook_FLOAT_gt(floating *a, floating *b) {
  return mpfr_greater_p(a->f, b->f);
}

bool hook_FLOAT_ge(floating *a, floating *b) {
  return mpfr_greaterequal_p(a->f, b->f);
}

bool hook_FLOAT_lt(floating *a, floating *b) {
  return mpfr_less_p(a->f, b->f);
}

bool hook_FLOAT_le(floating *a, floating *b) {
  return mpfr_lessequal_p(a->f, b->f);
}

bool hook_FLOAT_eq(floating *a, floating *b) {
  return mpfr_equal_p(a->f, b->f);
}

bool hook_FLOAT_ne(floating *a, floating *b) {
  return !mpfr_equal_p(a->f, b->f);
}

bool hook_FLOAT_trueeq(floating *a, floating *b) {
  if (a->exp != b->exp) {
    return false;
  }
  if (mpfr_get_prec(a->f) != mpfr_get_prec(b->f)) {
    return false;
  }
  if (mpfr_regular_p(a->f) && mpfr_regular_p(b->f)) {
    return mpfr_equal_p(a->f, b->f);
  }
  if (mpfr_nan_p(a->f) && mpfr_nan_p(b->f)) {
    return true;
  }
  if (mpfr_inf_p(a->f) && mpfr_inf_p(b->f)) {
    return mpfr_signbit(a->f) == mpfr_signbit(b->f);
  }
  if (mpfr_zero_p(a->f) && mpfr_zero_p(b->f)) {
    return mpfr_signbit(a->f) == mpfr_signbit(b->f);
  }
  return false;
}

floating *hook_FLOAT_abs(floating *a) {
  floating result[1];
  mpfr_enter(a, result);
  int t = mpfr_abs(result->f, a->f, MPFR_RNDN);
  mpfr_leave(t, result);
  return move_float(result); 
}

floating *hook_FLOAT_neg(floating *a) {
  floating result[1];
  mpfr_enter(a, result);
  int t = mpfr_neg(result->f, a->f, MPFR_RNDN);
  mpfr_leave(t, result);
  return move_float(result); 
}

floating *hook_FLOAT_min(floating *a, floating *b) {
  floating result[1];
  mpfr_enter(a, result);
  int t = mpfr_min(result->f, a->f, b->f, MPFR_RNDN);
  mpfr_leave(t, result);
  return move_float(result);
}

floating *hook_FLOAT_max(floating *a, floating *b) {
  floating result[1];
  mpfr_enter(a, result);
  int t = mpfr_max(result->f, a->f, b->f, MPFR_RNDN);
  mpfr_leave(t, result);
  return move_float(result);
}

floating *hook_FLOAT_add(floating *a, floating *b) {
  floating result[1];
  mpfr_enter(a, result);
  int t = mpfr_add(result->f, a->f, b->f, MPFR_RNDN);
  mpfr_leave(t, result);
  return move_float(result);
}

floating *hook_FLOAT_sub(floating *a, floating *b) {
  floating result[1];
  mpfr_enter(a, result);
  int t = mpfr_sub(result->f, a->f, b->f, MPFR_RNDN);
  mpfr_leave(t, result);
  return move_float(result);
}

floating *hook_FLOAT_mul(floating *a, floating *b) {
  floating result[1];
  mpfr_enter(a, result);
  int t = mpfr_mul(result->f, a->f, b->f, MPFR_RNDN);
  mpfr_leave(t, result);
  return move_float(result);
}

floating *hook_FLOAT_div(floating *a, floating *b) {
  floating result[1];
  mpfr_enter(a, result);
  int t = mpfr_div(result->f, a->f, b->f, MPFR_RNDN);
  mpfr_leave(t, result);
  return move_float(result);
}

floating *hook_FLOAT_rem(floating *a, floating *b) {
  floating result[1];
  mpfr_enter(a, result);
  int t = mpfr_remainder(result->f, a->f, b->f, MPFR_RNDN);
  mpfr_leave(t, result);
  return move_float(result);
}

floating *hook_FLOAT_pow(floating *a, floating *b) {
  floating result[1];
  mpfr_enter(a, result);
  int t = mpfr_pow(result->f, a->f, b->f, MPFR_RNDN);
  mpfr_leave(t, result);
  return move_float(result);
}

floating *hook_FLOAT_root(floating *a, mpz_t b) {
  if (!mpz_fits_ulong_p(b)) {
    throw std::invalid_argument("Root out of range");
  }
  unsigned long root = mpz_get_ui(b);
  floating result[1];
  mpfr_enter(a, result);
  int t = mpfr_rootn_ui(result->f, a->f, root, MPFR_RNDN);
  mpfr_leave(t, result);
  return move_float(result); 
}

floating *hook_FLOAT_log(floating *a) {
  floating result[1];
  mpfr_enter(a, result);
  int t = mpfr_log(result->f, a->f, MPFR_RNDN);
  mpfr_leave(t, result);
  return move_float(result); 
}

floating *hook_FLOAT_exp(floating *a) {
  floating result[1];
  mpfr_enter(a, result);
  int t = mpfr_exp(result->f, a->f, MPFR_RNDN);
  mpfr_leave(t, result);
  return move_float(result); 
}

bool hook_FLOAT_sign(floating *a) {
  return mpfr_signbit(a->f);
}

void float_hash(floating *f, void *hasher) {
  int nlimbs = (mpfr_get_prec(f->f) + 63) / 64;
  for (int i = 0; i < nlimbs; i++) {
    add_hash64(hasher, f->f[0]._mpfr_d[i]);
  }
}

}
