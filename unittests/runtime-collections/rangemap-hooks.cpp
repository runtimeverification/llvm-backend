#include <boost/test/unit_test.hpp>
#include <gmp.h>

#include "runtime/header.h"

extern "C" {
rangemap hook_RANGEMAP_element(block *keyrangestart, block *keyrangeend, block *value);
rangemap hook_RANGEMAP_concat(rangemap *m1, rangemap *m2);
rangemap hook_RANGEMAP_unit(void);
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
  auto result = hook_RANGEMAP_size_long(&map);
  BOOST_CHECK_EQUAL(result, 1);
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

BOOST_AUTO_TEST_SUITE_END()
