#include<boost/test/unit_test.hpp>
#include<gmp.h>
#include<mpfr.h>
#include<cstdint>
#include<cstring>

#define KCHAR char
extern "C" {
  struct blockheader {
    uint64_t len;
  };

  struct string {
    blockheader b;
    KCHAR data[0];
  };

  bool hook_STRING_gt(const string *, const string *);
  bool hook_STRING_ge(const string *, const string *);
  bool hook_STRING_lt(const string *, const string *);
  bool hook_STRING_le(const string *, const string *);
  bool hook_STRING_eq(const string *, const string *);
  bool hook_STRING_ne(const string *, const string *);
  const string * hook_STRING_concat(const string *, const string *);
  mpz_ptr hook_STRING_length(const string *);
  const string * hook_STRING_chr(const mpz_t);
  mpz_ptr hook_STRING_ord(const string *);
  string * hook_STRING_substr(string *, mpz_t, mpz_t);
  mpz_ptr hook_STRING_find(const string *, const string *, mpz_ptr);
  mpz_ptr hook_STRING_rfind(const string *, const string *, mpz_ptr);
  mpz_ptr hook_STRING_findChar(const string *, const string *, mpz_ptr);
  mpz_ptr hook_STRING_rfindChar(const string *, const string *, mpz_ptr);
  const mpz_ptr hook_STRING_string2int(const string *);
  const mpz_ptr hook_STRING_string2base(const string *, mpz_t);
  const string * hook_STRING_int2string(const mpz_t);
  const string * hook_STRING_replaceAll(const string *, const string *, const string *);
  const string * hook_STRING_replace(const string *, const string *, const string *, mpz_t);
  const string * hook_STRING_replaceFirst(const string *, const string *, const string *);
  mpz_ptr hook_STRING_countAllOccurrences(const string *, const string *);
  string * makeString(const KCHAR *);

  mpz_ptr move_int(mpz_t i) {
    mpz_ptr result = (mpz_ptr)malloc(sizeof(__mpz_struct));
    *result = *i;
    return result;
  }
}

BOOST_AUTO_TEST_SUITE(StringTest)

  BOOST_AUTO_TEST_CASE(gt) {
    auto a = makeString("hello");
    auto b = makeString("he");
    auto c = makeString("hf");
    auto d = makeString("");

    BOOST_CHECK_EQUAL(false, hook_STRING_gt(a,a));
    BOOST_CHECK_EQUAL(true,  hook_STRING_gt(a,b));
    BOOST_CHECK_EQUAL(false, hook_STRING_gt(a,c));
    BOOST_CHECK_EQUAL(true,  hook_STRING_gt(a,d));
    BOOST_CHECK_EQUAL(false, hook_STRING_gt(b,a));
    BOOST_CHECK_EQUAL(false, hook_STRING_gt(b,b));
    BOOST_CHECK_EQUAL(false, hook_STRING_gt(b,c));
    BOOST_CHECK_EQUAL(true,  hook_STRING_gt(b,d));
    BOOST_CHECK_EQUAL(true,  hook_STRING_gt(c,a));
    BOOST_CHECK_EQUAL(true,  hook_STRING_gt(c,b));
    BOOST_CHECK_EQUAL(false, hook_STRING_gt(c,c));
    BOOST_CHECK_EQUAL(true,  hook_STRING_gt(c,d));
    BOOST_CHECK_EQUAL(false, hook_STRING_gt(d,a));
    BOOST_CHECK_EQUAL(false, hook_STRING_gt(d,b));
    BOOST_CHECK_EQUAL(false, hook_STRING_gt(d,c));
    BOOST_CHECK_EQUAL(false, hook_STRING_gt(d,d));
  }

BOOST_AUTO_TEST_CASE(ge) {
  auto a = makeString("hello");
  auto b = makeString("he");
  auto c = makeString("hf");
  auto d = makeString("");

  BOOST_CHECK_EQUAL(true,  hook_STRING_ge(a,a));
  BOOST_CHECK_EQUAL(true,  hook_STRING_ge(a,b));
  BOOST_CHECK_EQUAL(false, hook_STRING_ge(a,c));
  BOOST_CHECK_EQUAL(true,  hook_STRING_ge(a,d));
  BOOST_CHECK_EQUAL(false, hook_STRING_ge(b,a));
  BOOST_CHECK_EQUAL(true,  hook_STRING_ge(b,b));
  BOOST_CHECK_EQUAL(false, hook_STRING_ge(b,c));
  BOOST_CHECK_EQUAL(true,  hook_STRING_ge(b,d));
  BOOST_CHECK_EQUAL(true,  hook_STRING_ge(c,a));
  BOOST_CHECK_EQUAL(true,  hook_STRING_ge(c,b));
  BOOST_CHECK_EQUAL(true,  hook_STRING_ge(c,c));
  BOOST_CHECK_EQUAL(true,  hook_STRING_ge(c,d));
  BOOST_CHECK_EQUAL(false, hook_STRING_ge(d,a));
  BOOST_CHECK_EQUAL(false, hook_STRING_ge(d,b));
  BOOST_CHECK_EQUAL(false, hook_STRING_ge(d,c));
  BOOST_CHECK_EQUAL(true,  hook_STRING_ge(d,d));
}

BOOST_AUTO_TEST_CASE(lt) {
  auto a = makeString("hello");
  auto b = makeString("he");
  auto c = makeString("hf");
  auto d = makeString("");

  BOOST_CHECK_EQUAL(false, hook_STRING_lt(a,a));
  BOOST_CHECK_EQUAL(false, hook_STRING_lt(a,b));
  BOOST_CHECK_EQUAL(true,  hook_STRING_lt(a,c));
  BOOST_CHECK_EQUAL(false, hook_STRING_lt(a,d));
  BOOST_CHECK_EQUAL(true,  hook_STRING_lt(b,a));
  BOOST_CHECK_EQUAL(false, hook_STRING_lt(b,b));
  BOOST_CHECK_EQUAL(true,  hook_STRING_lt(b,c));
  BOOST_CHECK_EQUAL(false, hook_STRING_lt(b,d));
  BOOST_CHECK_EQUAL(false, hook_STRING_lt(c,a));
  BOOST_CHECK_EQUAL(false, hook_STRING_lt(c,b));
  BOOST_CHECK_EQUAL(false, hook_STRING_lt(c,c));
  BOOST_CHECK_EQUAL(false, hook_STRING_lt(c,d));
  BOOST_CHECK_EQUAL(true,  hook_STRING_lt(d,a));
  BOOST_CHECK_EQUAL(true,  hook_STRING_lt(d,b));
  BOOST_CHECK_EQUAL(true,  hook_STRING_lt(d,c));
  BOOST_CHECK_EQUAL(false, hook_STRING_lt(d,d));
}

BOOST_AUTO_TEST_CASE(le) {
  auto a = makeString("hello");
  auto b = makeString("he");
  auto c = makeString("hf");
  auto d = makeString("");

  BOOST_CHECK_EQUAL(true,  hook_STRING_le(a,a));
  BOOST_CHECK_EQUAL(false, hook_STRING_le(a,b));
  BOOST_CHECK_EQUAL(true,  hook_STRING_le(a,c));
  BOOST_CHECK_EQUAL(false, hook_STRING_le(a,d));
  BOOST_CHECK_EQUAL(true,  hook_STRING_le(b,a));
  BOOST_CHECK_EQUAL(true,  hook_STRING_le(b,b));
  BOOST_CHECK_EQUAL(true,  hook_STRING_le(b,c));
  BOOST_CHECK_EQUAL(false, hook_STRING_le(b,d));
  BOOST_CHECK_EQUAL(false, hook_STRING_le(c,a));
  BOOST_CHECK_EQUAL(false, hook_STRING_le(c,b));
  BOOST_CHECK_EQUAL(true,  hook_STRING_le(c,c));
  BOOST_CHECK_EQUAL(false, hook_STRING_le(c,d));
  BOOST_CHECK_EQUAL(true,  hook_STRING_le(d,a));
  BOOST_CHECK_EQUAL(true,  hook_STRING_le(d,b));
  BOOST_CHECK_EQUAL(true,  hook_STRING_le(d,c));
  BOOST_CHECK_EQUAL(true,  hook_STRING_le(d,d));
}

BOOST_AUTO_TEST_CASE(eq) {
  auto a = makeString("hello");
  auto b = makeString("he");
  auto c = makeString("hf");
  auto d = makeString("");

  BOOST_CHECK_EQUAL(true,  hook_STRING_eq(a,a));
  BOOST_CHECK_EQUAL(false, hook_STRING_eq(a,b));
  BOOST_CHECK_EQUAL(false, hook_STRING_eq(a,c));
  BOOST_CHECK_EQUAL(false, hook_STRING_eq(a,d));
  BOOST_CHECK_EQUAL(false, hook_STRING_eq(b,a));
  BOOST_CHECK_EQUAL(true,  hook_STRING_eq(b,b));
  BOOST_CHECK_EQUAL(false, hook_STRING_eq(b,c));
  BOOST_CHECK_EQUAL(false, hook_STRING_eq(b,d));
  BOOST_CHECK_EQUAL(false, hook_STRING_eq(c,a));
  BOOST_CHECK_EQUAL(false, hook_STRING_eq(c,b));
  BOOST_CHECK_EQUAL(true,  hook_STRING_eq(c,c));
  BOOST_CHECK_EQUAL(false, hook_STRING_eq(c,d));
  BOOST_CHECK_EQUAL(false, hook_STRING_eq(d,a));
  BOOST_CHECK_EQUAL(false, hook_STRING_eq(d,b));
  BOOST_CHECK_EQUAL(false, hook_STRING_eq(d,c));
  BOOST_CHECK_EQUAL(true,  hook_STRING_eq(d,d));
}

BOOST_AUTO_TEST_CASE(ne) {
  auto a = makeString("hello");
  auto b = makeString("he");
  auto c = makeString("hf");
  auto d = makeString("");

  BOOST_CHECK_EQUAL(false, hook_STRING_ne(a,a));
  BOOST_CHECK_EQUAL(true,  hook_STRING_ne(a,b));
  BOOST_CHECK_EQUAL(true,  hook_STRING_ne(a,c));
  BOOST_CHECK_EQUAL(true,  hook_STRING_ne(a,d));
  BOOST_CHECK_EQUAL(true,  hook_STRING_ne(b,a));
  BOOST_CHECK_EQUAL(false, hook_STRING_ne(b,b));
  BOOST_CHECK_EQUAL(true,  hook_STRING_ne(b,c));
  BOOST_CHECK_EQUAL(true,  hook_STRING_ne(b,d));
  BOOST_CHECK_EQUAL(true,  hook_STRING_ne(c,a));
  BOOST_CHECK_EQUAL(true,  hook_STRING_ne(c,b));
  BOOST_CHECK_EQUAL(false, hook_STRING_ne(c,c));
  BOOST_CHECK_EQUAL(true,  hook_STRING_ne(c,d));
  BOOST_CHECK_EQUAL(true,  hook_STRING_ne(d,a));
  BOOST_CHECK_EQUAL(true,  hook_STRING_ne(d,b));
  BOOST_CHECK_EQUAL(true,  hook_STRING_ne(d,c));
  BOOST_CHECK_EQUAL(false, hook_STRING_ne(d,d));
}

BOOST_AUTO_TEST_CASE(concat) {
  auto a = makeString("hello");
  auto b = makeString("he");
  auto c = makeString("hf");
  auto d = makeString("");

  auto emptyCatR = hook_STRING_concat(a, d);
  BOOST_CHECK_EQUAL(0, memcmp(emptyCatR->data, a->data, emptyCatR->b.len));
  BOOST_CHECK_EQUAL(emptyCatR->b.len, a->b.len);

  auto emptyCatL = hook_STRING_concat(d, a);
  BOOST_CHECK_EQUAL(0, memcmp(emptyCatL->data, a->data, emptyCatL->b.len));
  BOOST_CHECK_EQUAL(emptyCatL->b.len, a->b.len);

  auto catAll = hook_STRING_concat(hook_STRING_concat(a,b), c);
  auto expected = makeString("hellohehf");
  BOOST_CHECK_EQUAL(0, memcmp(catAll->data, expected->data, catAll->b.len));
  BOOST_CHECK_EQUAL(catAll->b.len, expected->b.len);
}


BOOST_AUTO_TEST_CASE(chr) {
  mpz_t a, b;
  mpz_init_set_ui(a, 65);
  mpz_init_set_ui(b, 32);

  const string * A = hook_STRING_chr(a);
  const string * space = hook_STRING_chr(b);

  BOOST_CHECK_EQUAL(A->data[0], 'A');
  BOOST_CHECK_EQUAL(A->b.len, 1);
  BOOST_CHECK_EQUAL(space->data[0], ' ');
  BOOST_CHECK_EQUAL(space->b.len, 1);
}

BOOST_AUTO_TEST_CASE(length) {
  BOOST_CHECK_EQUAL(mpz_cmp_ui(hook_STRING_length(makeString("hello")), 5), 0);
  BOOST_CHECK_EQUAL(mpz_cmp_ui(hook_STRING_length(makeString("")), 0), 0);
  BOOST_CHECK_EQUAL(mpz_cmp_ui(hook_STRING_length(makeString("worl")), 4), 0);
}

BOOST_AUTO_TEST_CASE(ord) {
  mpz_ptr result = hook_STRING_ord(makeString("A"));
  BOOST_CHECK_EQUAL(mpz_cmp_ui(result, 65), 0);
  result = hook_STRING_ord(makeString(" "));
  BOOST_CHECK_EQUAL(mpz_cmp_ui(result, 32), 0);
  BOOST_CHECK_THROW(hook_STRING_ord(makeString("")), std::invalid_argument);
  BOOST_CHECK_THROW(hook_STRING_ord(makeString("AA")), std::invalid_argument);
}

BOOST_AUTO_TEST_CASE(substr) {
  auto catAll = makeString("hellohehf");

  mpz_t _2, _9, _6, _0, _4, _7, _40, _8, _10, _1024, _4096;
  mpz_init_set_si(_2, 2);
  mpz_init_set_si(_9, 9);
  mpz_init_set_si(_6, 6);
  mpz_init_set_si(_0, 0);
  mpz_init_set_si(_4, 4);
  mpz_init_set_si(_7, 7);
  mpz_init_set_si(_40, 40);
  mpz_init_set_si(_8, 8);
  mpz_init_set_si(_10, 10);
  mpz_init_set_si(_1024, 1024);
  mpz_init_set_si(_4096, 4096);
  BOOST_CHECK_EQUAL(memcmp(hook_STRING_substr(catAll, _2, _9)->data, "llohehf", 7), 0);
  BOOST_CHECK_EQUAL(memcmp(hook_STRING_substr(catAll, _2, _6)->data, "lloh", 4), 0);
  BOOST_CHECK_EQUAL(memcmp(hook_STRING_substr(catAll, _0, _4)->data, "hell", 4), 0);
  BOOST_CHECK_EQUAL(memcmp(hook_STRING_substr(catAll, _6, _9)->data, "ehf", 3), 0);
  BOOST_CHECK_THROW(hook_STRING_substr(catAll, _7, _40), std::invalid_argument);
  BOOST_CHECK_THROW(hook_STRING_substr(catAll, _8, _40), std::invalid_argument);
  BOOST_CHECK_EQUAL(memcmp(hook_STRING_substr(catAll, _8, _9)->data, "f", 1), 0);
  BOOST_CHECK_EQUAL(hook_STRING_substr(catAll, _9, _9)->b.len, 0);
  BOOST_CHECK_THROW(hook_STRING_substr(catAll, _8, _7), std::invalid_argument);
  BOOST_CHECK_THROW(hook_STRING_substr(catAll, _7, _10), std::invalid_argument);
  BOOST_CHECK_THROW(hook_STRING_substr(catAll, _1024, _4096), std::invalid_argument);
}

BOOST_AUTO_TEST_CASE(find) {
  auto haystack = makeString("hellollo");
  auto needle = makeString("llo");
  auto needle2 = makeString("hf");

  mpz_t a, b, c, d, e, f, g;
  mpz_init_set_si(a, 0);
  mpz_init_set_si(b, 1);
  mpz_init_set_si(c, 2);
  mpz_init_set_si(d, 3);
  mpz_init_set_si(e, 4);
  mpz_init_set_si(f, 5);
  mpz_init_set_si(g, 6);

  BOOST_CHECK_EQUAL(mpz_cmp_si(hook_STRING_find(haystack, needle, a), 2), 0);
  BOOST_CHECK_EQUAL(mpz_cmp_si(hook_STRING_find(haystack, needle, b), 2), 0);
  BOOST_CHECK_EQUAL(mpz_cmp_si(hook_STRING_find(haystack, needle, c), 2), 0);
  BOOST_CHECK_EQUAL(mpz_cmp_si(hook_STRING_find(haystack, needle, d), 5), 0);
  BOOST_CHECK_EQUAL(mpz_cmp_si(hook_STRING_find(haystack, needle, e), 5), 0);
  BOOST_CHECK_EQUAL(mpz_cmp_si(hook_STRING_find(haystack, needle, f), 5), 0);
  BOOST_CHECK_EQUAL(mpz_cmp_si(hook_STRING_find(haystack, needle, g), -1), 0);
  BOOST_CHECK_EQUAL(mpz_cmp_si(hook_STRING_find(haystack, needle2, d), -1), 0);
}

BOOST_AUTO_TEST_CASE(rfind) {
  auto haystack = makeString("hellollo");
  auto needle = makeString("llo");
  auto needle2 = makeString("hf");

  mpz_t a, b, c, d, e, f, g;
  mpz_init_set_si(a, 0);
  mpz_init_set_si(b, 1);
  mpz_init_set_si(c, 2);
  mpz_init_set_si(d, 3);
  mpz_init_set_si(e, 4);
  mpz_init_set_si(f, 5);
  mpz_init_set_si(g, 6);

  BOOST_CHECK_EQUAL(mpz_cmp_si(hook_STRING_rfind(haystack, needle, a), -1), 0);
  BOOST_CHECK_EQUAL(mpz_cmp_si(hook_STRING_rfind(haystack, needle, b), -1), 0);
  BOOST_CHECK_EQUAL(mpz_cmp_si(hook_STRING_rfind(haystack, needle, c), 2), 0);
  BOOST_CHECK_EQUAL(mpz_cmp_si(hook_STRING_rfind(haystack, needle, d), 2), 0);
  BOOST_CHECK_EQUAL(mpz_cmp_si(hook_STRING_rfind(haystack, needle, e), 2), 0);
  BOOST_CHECK_EQUAL(mpz_cmp_si(hook_STRING_rfind(haystack, needle, f), 5), 0);
  BOOST_CHECK_EQUAL(mpz_cmp_si(hook_STRING_rfind(haystack, needle, g), 5), 0);
  BOOST_CHECK_EQUAL(mpz_cmp_si(hook_STRING_rfind(haystack, needle2, d), -1), 0);
}

BOOST_AUTO_TEST_CASE(int2string) {
  mpz_t a;
  mpz_init_set_ui(a, 10);

  auto res = hook_STRING_int2string(a);
  BOOST_CHECK_EQUAL(2, res->b.len);
  BOOST_CHECK_EQUAL(0, memcmp(res->data, "10", 2));

  mpz_set_ui(a, 1234);
  res = hook_STRING_int2string(a);
  BOOST_CHECK_EQUAL(4, res->b.len);
  BOOST_CHECK_EQUAL(0, memcmp(res->data, "1234", 4));

  mpz_set_si(a, -1234);
  res = hook_STRING_int2string(a);
  BOOST_CHECK_EQUAL(5, res->b.len);
  BOOST_CHECK_EQUAL(0, memcmp(res->data, "-1234", 5));

  mpz_set_ui(a, 0);
  res = hook_STRING_int2string(a);
  BOOST_CHECK_EQUAL(1, res->b.len);
  BOOST_CHECK_EQUAL(0, memcmp(res->data, "0", 1));
}

BOOST_AUTO_TEST_CASE(string2int) {
  auto _0 = makeString("0");
  auto _10 = makeString("10");
  auto neg10 = makeString("-10");
  auto neg10000 = makeString("-10000");

  BOOST_CHECK_EQUAL(mpz_cmp_si(hook_STRING_string2int(_0), 0), 0);
  BOOST_CHECK_EQUAL(mpz_cmp_si(hook_STRING_string2int(_10), 10), 0);
  BOOST_CHECK_EQUAL(mpz_cmp_si(hook_STRING_string2int(neg10), -10), 0);
  BOOST_CHECK_EQUAL(mpz_cmp_si(hook_STRING_string2int(neg10000), -10000), 0);
}

BOOST_AUTO_TEST_CASE(string2base) {
  auto _0 = makeString("0");
  auto _10 = makeString("10");
  auto neg10 = makeString("-10");
  auto ff = makeString("ff");

  mpz_t int10;
  mpz_init_set_ui(int10, 10);
  mpz_t int16;
  mpz_init_set_ui(int16, 16);

  BOOST_CHECK_EQUAL(mpz_cmp_si(hook_STRING_string2base(_0, int10), 0), 0);
  BOOST_CHECK_EQUAL(mpz_cmp_si(hook_STRING_string2base(_10, int10), 10), 0);
  BOOST_CHECK_EQUAL(mpz_cmp_si(hook_STRING_string2base(neg10, int10), -10), 0);
  BOOST_CHECK_THROW(hook_STRING_string2base(ff, int10), std::invalid_argument);

  BOOST_CHECK_EQUAL(mpz_cmp_si(hook_STRING_string2base(_0, int16), 0), 0);
  BOOST_CHECK_EQUAL(mpz_cmp_si(hook_STRING_string2base(_10, int16), 16), 0);
  BOOST_CHECK_EQUAL(mpz_cmp_si(hook_STRING_string2base(neg10, int16), -16), 0);
  BOOST_CHECK_EQUAL(mpz_cmp_si(hook_STRING_string2base(ff, int16), 255), 0);
}

BOOST_AUTO_TEST_CASE(replace) {
  auto replacee = makeString("hello world hello world hello world he worl");
  auto matcher = makeString("hello");
  auto replacer = makeString("goodbye");

  mpz_t a, b, c, d, e;
  mpz_init_set_si(a, 0);
  mpz_init_set_si(b, 1);
  mpz_init_set_si(c, 2);
  mpz_init_set_si(d, 3);
  mpz_init_set_si(e, 4);

  BOOST_CHECK_EQUAL(true,
      hook_STRING_eq(hook_STRING_replaceAll(replacee, matcher, replacer),
        makeString("goodbye world goodbye world goodbye world he worl")));
  BOOST_CHECK_EQUAL(true,
      hook_STRING_eq(hook_STRING_replace(replacee, matcher, replacer, a),
        makeString("hello world hello world hello world he worl")));
  BOOST_CHECK_EQUAL(true,
      hook_STRING_eq(hook_STRING_replace(replacee, matcher, replacer, b),
        makeString("goodbye world hello world hello world he worl")));
  BOOST_CHECK_EQUAL(true,
      hook_STRING_eq(hook_STRING_replace(replacee, matcher, replacer, c),
        makeString("goodbye world goodbye world hello world he worl")));
  BOOST_CHECK_EQUAL(true,
      hook_STRING_eq(hook_STRING_replace(replacee, matcher, replacer, d),
        makeString("goodbye world goodbye world goodbye world he worl")));
  BOOST_CHECK_EQUAL(true,
      hook_STRING_eq(hook_STRING_replace(replacee, matcher, replacer, e),
        makeString("goodbye world goodbye world goodbye world he worl")));
  BOOST_CHECK_EQUAL(true,
      hook_STRING_eq(hook_STRING_replaceFirst(replacee, matcher, replacer),
        makeString("goodbye world hello world hello world he worl")));
}

BOOST_AUTO_TEST_CASE(countAllOccurrences) {
  auto replacee = makeString("hello world hello world hello world he worl");
  auto matcher = makeString("hello");
  BOOST_CHECK_EQUAL(mpz_cmp_ui(hook_STRING_countAllOccurrences(replacee, matcher), 3), 0);

  replacee = makeString("hel world hel world heo world he worl");
  matcher = makeString("hello");
  BOOST_CHECK_EQUAL(mpz_cmp_ui(hook_STRING_countAllOccurrences(replacee, matcher), 0), 0);

  replacee = makeString("hel world hel world hello world he worl");
  matcher = makeString("hello");
  BOOST_CHECK_EQUAL(mpz_cmp_ui(hook_STRING_countAllOccurrences(replacee, matcher), 1), 0);
}

BOOST_AUTO_TEST_SUITE_END()
