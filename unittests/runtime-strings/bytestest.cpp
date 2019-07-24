#include<boost/test/unit_test.hpp>
#include<gmp.h>
#include<cstdint>
#include<cstring>

#include "runtime/header.h"
#include "runtime/alloc.h"

#define KCHAR char
extern "C" {
  uint32_t getTagForSymbolName(const char *s) {
    return 0;
  }

  uint64_t tag_big_endian();
  uint64_t tag_unsigned();

  mpz_ptr hook_BYTES_bytes2int(string *b, uint64_t endianness, uint64_t signedness);
  string *hook_BYTES_int2bytes(mpz_t len, mpz_t i, uint64_t endianness);
  string *hook_BYTES_bytes2string(string *b);
  string *hook_BYTES_string2bytes(string *s);
  string *hook_BYTES_substr(string *b, mpz_t start, mpz_t end);
  string *hook_BYTES_replaceAt(string *b, mpz_t start, string *b2);
  string *hook_BYTES_update(string *b, mpz_t off, mpz_t val);
  mpz_ptr hook_BYTES_get(string *b, mpz_t off);
  mpz_ptr hook_BYTES_length(string *b);
  string *hook_BYTES_padRight(string *b, mpz_t len, mpz_t v);
  string *hook_BYTES_padLeft(string *b, mpz_t len, mpz_t v);
  string *hook_BYTES_reverse(string *b);
  string *hook_BYTES_concat(string *b1, string *);
  string * makeString(const KCHAR *, int64_t len = -1);
}

BOOST_AUTO_TEST_SUITE(BytesTest)

BOOST_AUTO_TEST_CASE(bytes2int) {
  auto empty = makeString("");
  BOOST_CHECK_EQUAL(0, mpz_cmp_si(hook_BYTES_bytes2int(empty, tag_big_endian(), tag_unsigned()), 0));
  BOOST_CHECK_EQUAL(0, mpz_cmp_si(hook_BYTES_bytes2int(empty, 2, tag_unsigned()), 0));
  BOOST_CHECK_EQUAL(0, mpz_cmp_si(hook_BYTES_bytes2int(empty, tag_big_endian(), 2), 0));
  BOOST_CHECK_EQUAL(0, mpz_cmp_si(hook_BYTES_bytes2int(empty, 2, 2), 0));

  auto ff = makeString("\xff");
  BOOST_CHECK_EQUAL(0, mpz_cmp_si(hook_BYTES_bytes2int(ff, tag_big_endian(), tag_unsigned()), 255));
  BOOST_CHECK_EQUAL(0, mpz_cmp_si(hook_BYTES_bytes2int(ff, tag_big_endian(), 2), -1));
  BOOST_CHECK_EQUAL(0, mpz_cmp_si(hook_BYTES_bytes2int(ff, 2, tag_unsigned()), 255));
  BOOST_CHECK_EQUAL(0, mpz_cmp_si(hook_BYTES_bytes2int(ff, 2, 2), -1));

  auto _00ff = makeString("\x00\xff", 2);
  BOOST_CHECK_EQUAL(0, mpz_cmp_si(hook_BYTES_bytes2int(_00ff, tag_big_endian(), tag_unsigned()), 255));
  BOOST_CHECK_EQUAL(0, mpz_cmp_si(hook_BYTES_bytes2int(_00ff, tag_big_endian(), 2), 255));

  auto ff00 = makeString("\xff\x00", 2);
  BOOST_CHECK_EQUAL(0, mpz_cmp_si(hook_BYTES_bytes2int(ff00, 2, 2), 255));
  BOOST_CHECK_EQUAL(0, mpz_cmp_si(hook_BYTES_bytes2int(ff00, 2, tag_unsigned()), 255));
}

BOOST_AUTO_TEST_CASE(int2bytes) {
  mpz_t _0;
  mpz_init_set_ui(_0, 0);

  auto res = hook_BYTES_int2bytes(_0, _0, tag_big_endian());
  BOOST_CHECK_EQUAL(0, len(res));
  res = hook_BYTES_int2bytes(_0, _0, 2);
  BOOST_CHECK_EQUAL(0, len(res));

  mpz_t _4;
  mpz_init_set_ui(_4, 4);

  res = hook_BYTES_int2bytes(_4, _0, tag_big_endian());
  BOOST_CHECK_EQUAL(4, len(res));
  BOOST_CHECK_EQUAL(0, memcmp(res->data, "\x00\x00\x00\x00", 4));
  res = hook_BYTES_int2bytes(_4, _0, 2);
  BOOST_CHECK_EQUAL(4, len(res));
  BOOST_CHECK_EQUAL(0, memcmp(res->data, "\x00\x00\x00\x00", 4));

  mpz_t _1, neg128, neg2;
  mpz_init_set_ui(_1, 1);
  mpz_init_set_si(neg128, -128);
  mpz_init_set_si(neg2, -2);

  res = hook_BYTES_int2bytes(_1, neg128, tag_big_endian());
  BOOST_CHECK_EQUAL(1, len(res));
  BOOST_CHECK_EQUAL(0, memcmp(res->data, "\x80", 1));
  res = hook_BYTES_int2bytes(_1, neg128, 2);
  BOOST_CHECK_EQUAL(1, len(res));
  BOOST_CHECK_EQUAL(0, memcmp(res->data, "\x80", 1));

  res = hook_BYTES_int2bytes(_1, neg2, tag_big_endian());
  BOOST_CHECK_EQUAL(1, len(res));
  BOOST_CHECK_EQUAL(0, memcmp(res->data, "\xfe", 1));
  res = hook_BYTES_int2bytes(_1, neg2, 2);
  BOOST_CHECK_EQUAL(1, len(res));
  BOOST_CHECK_EQUAL(0, memcmp(res->data, "\xfe", 1));

  mpz_t _128;
  mpz_init_set_ui(_128, 128);

  res = hook_BYTES_int2bytes(_1, _128, tag_big_endian());
  BOOST_CHECK_EQUAL(1, len(res));
  BOOST_CHECK_EQUAL(0, memcmp(res->data, "\x80", 1));
  res = hook_BYTES_int2bytes(_1, _128, 2);
  BOOST_CHECK_EQUAL(1, len(res));
  BOOST_CHECK_EQUAL(0, memcmp(res->data, "\x80", 1));

  mpz_t _2;
  mpz_init_set_ui(_2, 2);

  res = hook_BYTES_int2bytes(_2, _128, tag_big_endian());
  BOOST_CHECK_EQUAL(2, len(res));
  BOOST_CHECK_EQUAL(0, memcmp(res->data, "\x00\x80", 2));
  res = hook_BYTES_int2bytes(_2, _128, 2);
  BOOST_CHECK_EQUAL(2, len(res));
  BOOST_CHECK_EQUAL(0, memcmp(res->data, "\x80\x00", 2));

  res = hook_BYTES_int2bytes(_2, neg128, tag_big_endian());
  BOOST_CHECK_EQUAL(2, len(res));
  BOOST_CHECK_EQUAL(0, memcmp(res->data, "\xff\x80", 2));
  res = hook_BYTES_int2bytes(_2, neg128, 2);
  BOOST_CHECK_EQUAL(2, len(res));
  BOOST_CHECK_EQUAL(0, memcmp(res->data, "\x80\xff", 2));

  res = hook_BYTES_int2bytes(_2, neg2, tag_big_endian());
  BOOST_CHECK_EQUAL(2, len(res));
  BOOST_CHECK_EQUAL(0, memcmp(res->data, "\xff\xfe", 2));
  res = hook_BYTES_int2bytes(_2, neg2, 2);
  BOOST_CHECK_EQUAL(2, len(res));
  BOOST_CHECK_EQUAL(0, memcmp(res->data, "\xfe\xff", 2));

  mpz_t num;
  mpz_init_set_str(num, "-340240827991474256463460986840063997951", 10);
  mpz_t _17;
  mpz_init_set_ui(_17, 17);

  res = hook_BYTES_int2bytes(_17, num, tag_big_endian());
  BOOST_CHECK_EQUAL(17, len(res));
  BOOST_CHECK_EQUAL(0, memcmp(res->data, "\xff\x00\x08\x00\x06\xFF\xFE\xFF\xF0\x00\x08\xFF\xFF\xFF\xFF\x08\x01", 17));
}

BOOST_AUTO_TEST_CASE(bytes2string) {
  auto empty = makeString("");
  auto res = hook_BYTES_bytes2string(empty);
  BOOST_CHECK(res != empty);
  BOOST_CHECK_EQUAL(len(empty), 0);

  auto _1234 = makeString("1234");
  res = hook_BYTES_bytes2string(_1234);
  BOOST_CHECK(res != _1234);
  BOOST_CHECK_EQUAL(len(_1234), 4);
  BOOST_CHECK_EQUAL(0, memcmp(_1234->data, "1234", 4));
}

BOOST_AUTO_TEST_CASE(string2bytes) {
  auto empty = makeString("");
  auto res = hook_BYTES_string2bytes(empty);
  BOOST_CHECK(res != empty);
  BOOST_CHECK_EQUAL(len(empty), 0);

  auto _1234 = makeString("1234");
  res = hook_BYTES_string2bytes(_1234);
  BOOST_CHECK(res != _1234);
  BOOST_CHECK_EQUAL(len(_1234), 4);
  BOOST_CHECK_EQUAL(0, memcmp(_1234->data, "1234", 4));
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
  BOOST_CHECK_EQUAL(memcmp(hook_BYTES_substr(catAll, _2, _9)->data, "llohehf", 7), 0);
  BOOST_CHECK_EQUAL(memcmp(hook_BYTES_substr(catAll, _2, _6)->data, "lloh", 4), 0);
  BOOST_CHECK_EQUAL(memcmp(hook_BYTES_substr(catAll, _0, _4)->data, "hell", 4), 0);
  BOOST_CHECK_EQUAL(memcmp(hook_BYTES_substr(catAll, _6, _9)->data, "ehf", 3), 0);
  BOOST_CHECK_THROW(hook_BYTES_substr(catAll, _7, _40), std::invalid_argument);
  BOOST_CHECK_THROW(hook_BYTES_substr(catAll, _8, _40), std::invalid_argument);
  BOOST_CHECK_EQUAL(memcmp(hook_BYTES_substr(catAll, _8, _9)->data, "f", 1), 0);
  BOOST_CHECK_EQUAL(len(hook_BYTES_substr(catAll, _9, _9)), 0);
  BOOST_CHECK_THROW(hook_BYTES_substr(catAll, _8, _7), std::invalid_argument);
  BOOST_CHECK_THROW(hook_BYTES_substr(catAll, _7, _10), std::invalid_argument);
  BOOST_CHECK_THROW(hook_BYTES_substr(catAll, _1024, _4096), std::invalid_argument);
}

BOOST_AUTO_TEST_CASE(update) {
  auto _1234 = makeString("1234");
  mpz_t _0, _2;
  mpz_init_set_ui(_0, '0');
  mpz_init_set_ui(_2, 2);

  auto res = hook_BYTES_update(_1234, _2, _0);
  BOOST_CHECK_EQUAL(_1234, res);
  BOOST_CHECK_EQUAL(4, len(res));
  BOOST_CHECK_EQUAL(0, memcmp(res->data, "1204", 4));
}

BOOST_AUTO_TEST_CASE(get) {
  auto _1234 = makeString("1234");
  mpz_t _0;
  mpz_init_set_ui(_0, 0);

  auto res = hook_BYTES_get(_1234, _0);
  BOOST_CHECK_EQUAL(0, mpz_cmp_ui(res, '1'));
}



BOOST_AUTO_TEST_CASE(replaceAt) {
  auto _1234 = makeString("1234");
  auto _2 = makeString("2");
  mpz_t _0;
  mpz_init_set_ui(_0, 0);

  auto res = hook_BYTES_replaceAt(_1234, _0, _2);
  BOOST_CHECK_EQUAL(_1234, res);
  BOOST_CHECK_EQUAL(4, len(res));
  BOOST_CHECK_EQUAL(0, memcmp(res->data, "2234", 4));

  auto _23 = makeString("23");
  _1234 = makeString("1234");

  res = hook_BYTES_replaceAt(_1234, _0, _23);
  BOOST_CHECK_EQUAL(_1234, res);
  BOOST_CHECK_EQUAL(4, len(res));
  BOOST_CHECK_EQUAL(0, memcmp(res->data, "2334", 4));

  auto empty = makeString("");
  _1234 = makeString("1234");

  res = hook_BYTES_replaceAt(_1234, _0, empty);
  BOOST_CHECK_EQUAL(_1234, res);
  BOOST_CHECK_EQUAL(4, len(res));
  BOOST_CHECK_EQUAL(0, memcmp(res->data, "1234", 4));

  mpz_t _1;
  mpz_init_set_ui(_1, 1);
  auto _12 = makeString("12");

  res = hook_BYTES_replaceAt(_1234, _1, _12);
  BOOST_CHECK_EQUAL(_1234, res);
  BOOST_CHECK_EQUAL(4, len(res));
  BOOST_CHECK_EQUAL(0, memcmp(res->data, "1124", 4));
}

BOOST_AUTO_TEST_CASE(length) {
  auto empty = makeString("");
  BOOST_CHECK_EQUAL(0, mpz_get_ui(hook_BYTES_length(empty)));

  auto _1234 = makeString("1234");
  BOOST_CHECK_EQUAL(4, mpz_get_ui(hook_BYTES_length(_1234)));
}

BOOST_AUTO_TEST_CASE(padRight) {
  auto empty = makeString("");
  mpz_t _0;
  mpz_init_set_ui(_0, 0);

  auto res = hook_BYTES_padRight(empty, _0, _0);
  BOOST_CHECK_EQUAL(empty, res);
  BOOST_CHECK_EQUAL(0, len(empty));

  mpz_t _4, _97;
  mpz_init_set_ui(_4, 4);
  mpz_init_set_ui(_97, 97);

  res = hook_BYTES_padRight(empty, _4, _97);
  BOOST_CHECK_EQUAL(4, len(res));
  BOOST_CHECK_EQUAL(0, memcmp(res->data, "aaaa", 4));

  auto _1234 = makeString("1234");
  mpz_t _8;
  mpz_init_set_ui(_8, 8);

  res = hook_BYTES_padRight(_1234, _8, _97);
  BOOST_CHECK_EQUAL(8, len(res));
  BOOST_CHECK_EQUAL(0, memcmp(res->data, "1234aaaa", 8));
}

BOOST_AUTO_TEST_CASE(padLeft) {
  auto empty = makeString("");
  mpz_t _0;
  mpz_init_set_ui(_0, 0);

  auto res = hook_BYTES_padLeft(empty, _0, _0);
  BOOST_CHECK_EQUAL(empty, res);
  BOOST_CHECK_EQUAL(0, len(empty));

  mpz_t _4, _97;
  mpz_init_set_ui(_4, 4);
  mpz_init_set_ui(_97, 97);

  res = hook_BYTES_padLeft(empty, _4, _97);
  BOOST_CHECK_EQUAL(4, len(res));
  BOOST_CHECK_EQUAL(0, memcmp(res->data, "aaaa", 4));

  auto _1234 = makeString("1234");
  mpz_t _8;
  mpz_init_set_ui(_8, 8);

  res = hook_BYTES_padLeft(_1234, _8, _97);
  BOOST_CHECK_EQUAL(8, len(res));
  BOOST_CHECK_EQUAL(0, memcmp(res->data, "aaaa1234", 8));
}

BOOST_AUTO_TEST_CASE(reverse) {
  auto empty = makeString("");
  auto res = hook_BYTES_reverse(empty);
  BOOST_CHECK_EQUAL(empty, res);
  BOOST_CHECK_EQUAL(len(empty), 0);

  auto _1234 = makeString("1234");
  res = hook_BYTES_reverse(_1234);
  BOOST_CHECK_EQUAL(_1234, res);
  BOOST_CHECK_EQUAL(len(_1234), 4);
  BOOST_CHECK_EQUAL(0, memcmp(_1234->data, "4321", 4));
}

BOOST_AUTO_TEST_CASE(concat) {
  auto a = makeString("hello");
  auto b = makeString("he");
  auto c = makeString("hf");
  auto d = makeString("");

  auto emptyCatR = hook_BYTES_concat(a, d);
  BOOST_CHECK_EQUAL(0, memcmp(emptyCatR->data, a->data, len(emptyCatR)));
  BOOST_CHECK_EQUAL(len(emptyCatR), len(a));

  auto emptyCatL = hook_BYTES_concat(d, a);
  BOOST_CHECK_EQUAL(0, memcmp(emptyCatL->data, a->data, len(emptyCatL)));
  BOOST_CHECK_EQUAL(len(emptyCatL), len(a));

  auto catAll = hook_BYTES_concat(hook_BYTES_concat(a,b), c);
  auto expected = makeString("hellohehf");
  BOOST_CHECK_EQUAL(0, memcmp(catAll->data, expected->data, len(catAll)));
  BOOST_CHECK_EQUAL(len(catAll), len(expected));
}

BOOST_AUTO_TEST_SUITE_END()
