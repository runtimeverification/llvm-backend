#include <boost/test/unit_test.hpp>
#include <gmp.h>

#include "runtime/header.h"

extern "C" {
rangemap
hook_RANGEMAP_element(block *keyrangestart, block *keyrangeend, block *value);
rangemap hook_RANGEMAP_concat(rangemap *m1, rangemap *m2);
rangemap hook_RANGEMAP_unit(void);
//block *hook_RANGEMAP_lookup_null(rangemap *m, block *key);
block *hook_RANGEMAP_lookup(rangemap *m, block *key);
block *hook_RANGEMAP_lookupOrDefault(rangemap *m, block *key, block *_default);
rangemap hook_RANGEMAP_update(
    rangemap *m, block *keyRangeStart, block *keyRangeEnd, block *value);
rangemap
hook_RANGEMAP_remove(rangemap *m, block *keyRangeStart, block *keyRangeEnd);
rangemap hook_RANGEMAP_difference(rangemap *m1, rangemap *m2);
bool hook_RANGEMAP_in_keys(block *key, rangemap *m);
bool hook_RANGEMAP_inclusion(rangemap *m1, rangemap *m2);
size_t hook_RANGEMAP_size_long(rangemap *m);
mpz_ptr hook_RANGEMAP_size(rangemap *m);

block RD0 = {{0}};
block *RDUMMY0 = &RD0;
block RD1 = {{1}};
block *RDUMMY1 = &RD1;
block RD2 = {{2}};
block *RDUMMY2 = &RD2;

bool hook_KEQUAL_lt(block *b1, block *b2) {
  return b1->h.hdr < b2->h.hdr;
}
}

BOOST_AUTO_TEST_SUITE(RangeMapHookTest)

BOOST_AUTO_TEST_CASE(rangemap_hook_element) {
  auto map = hook_RANGEMAP_element(RDUMMY0, RDUMMY1, RDUMMY0);
  auto result1 = hook_RANGEMAP_size_long(&map);
  BOOST_CHECK_EQUAL(result1, 1);
  auto result2 = hook_RANGEMAP_lookup(&map, RDUMMY0);
  BOOST_CHECK_EQUAL(result2, RDUMMY0);
}

BOOST_AUTO_TEST_CASE(rangemap_hook_unit) {
  auto map = hook_RANGEMAP_unit();
  auto result = hook_RANGEMAP_size_long(&map);
  BOOST_CHECK_EQUAL(result, 0);
}

BOOST_AUTO_TEST_CASE(rangemap_hook_concat_success) {
  auto map1 = hook_RANGEMAP_element(RDUMMY0, RDUMMY1, RDUMMY0);
  auto map2 = hook_RANGEMAP_element(RDUMMY1, RDUMMY2, RDUMMY1);
  auto map = hook_RANGEMAP_concat(&map1, &map2);
  auto result = hook_RANGEMAP_size_long(&map);
  BOOST_CHECK_EQUAL(result, 2);
}

BOOST_AUTO_TEST_CASE(rangemap_hook_concat_failure) {
  auto map1 = hook_RANGEMAP_element(RDUMMY0, RDUMMY1, RDUMMY0);
  auto map2 = hook_RANGEMAP_element(RDUMMY0, RDUMMY2, RDUMMY1);
  BOOST_CHECK_THROW(hook_RANGEMAP_concat(&map1, &map2), std::invalid_argument);
}

BOOST_AUTO_TEST_CASE(rangemap_hook_lookup_or_default) {
  auto map = hook_RANGEMAP_element(RDUMMY0, RDUMMY1, RDUMMY0);
  auto result = hook_RANGEMAP_lookupOrDefault(&map, RDUMMY0, RDUMMY1);
  BOOST_CHECK_EQUAL(result, RDUMMY0);
  result = hook_RANGEMAP_lookupOrDefault(&map, RDUMMY1, RDUMMY1);
  BOOST_CHECK_EQUAL(result, RDUMMY1);
}

BOOST_AUTO_TEST_CASE(rangemap_hook_update) {
  auto map = hook_RANGEMAP_element(RDUMMY0, RDUMMY1, RDUMMY0);
  auto result = hook_RANGEMAP_lookup(&map, RDUMMY0);
  BOOST_CHECK_EQUAL(result, RDUMMY0);
  auto map2 = hook_RANGEMAP_update(&map, RDUMMY0, RDUMMY1, RDUMMY1);
  result = hook_RANGEMAP_lookup(&map, RDUMMY0);
  BOOST_CHECK_EQUAL(result, RDUMMY0);
  result = hook_RANGEMAP_lookup(&map2, RDUMMY0);
  BOOST_CHECK_EQUAL(result, RDUMMY1);
}

BOOST_AUTO_TEST_CASE(rangemap_hook_remove) {
  auto map = hook_RANGEMAP_element(RDUMMY0, RDUMMY1, RDUMMY0);
  auto map2 = hook_RANGEMAP_remove(&map, RDUMMY0, RDUMMY1);
  auto result = hook_RANGEMAP_size(&map);
  BOOST_CHECK_EQUAL(mpz_cmp_ui(result, 1), 0);
  result = hook_RANGEMAP_size(&map2);
  BOOST_CHECK_EQUAL(mpz_cmp_ui(result, 0), 0);
}

BOOST_AUTO_TEST_CASE(rangemap_hook_difference) {
  auto map1 = hook_RANGEMAP_element(RDUMMY0, RDUMMY2, RDUMMY0);
  auto map2 = hook_RANGEMAP_element(RDUMMY0, RDUMMY1, RDUMMY0);
  auto map3 = hook_RANGEMAP_difference(&map1, &map2);
  auto result = hook_RANGEMAP_size_long(&map3);
  BOOST_CHECK_EQUAL(result, 1);
  auto map4 = hook_RANGEMAP_difference(&map2, &map1);
  result = hook_RANGEMAP_size_long(&map4);
  BOOST_CHECK_EQUAL(result, 0);
}

BOOST_AUTO_TEST_CASE(rangemap_hook_in_keys) {
  auto map = hook_RANGEMAP_element(RDUMMY0, RDUMMY1, RDUMMY0);
  auto result = hook_RANGEMAP_in_keys(RDUMMY0, &map);
  BOOST_CHECK(result);
  result = hook_RANGEMAP_in_keys(RDUMMY1, &map);
  BOOST_CHECK(!result);
}

BOOST_AUTO_TEST_CASE(rangemap_hook_inclusion) {
  auto map1 = hook_RANGEMAP_element(RDUMMY0, RDUMMY2, RDUMMY0);
  auto map2 = hook_RANGEMAP_element(RDUMMY0, RDUMMY1, RDUMMY0);
  auto result = hook_RANGEMAP_inclusion(&map1, &map2);
  BOOST_CHECK(!result);
  result = hook_RANGEMAP_inclusion(&map2, &map1);
  BOOST_CHECK(result);
  auto map3 = hook_RANGEMAP_update(&map2, RDUMMY0, RDUMMY1, RDUMMY1);
  result = hook_RANGEMAP_inclusion(&map3, &map1);
  BOOST_CHECK(!result);
}

BOOST_AUTO_TEST_SUITE_END()
