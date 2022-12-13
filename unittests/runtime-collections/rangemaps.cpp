#include <boost/test/unit_test.hpp>

#include "../../runtime/collections/rangemap.h"

BOOST_AUTO_TEST_SUITE(TreeMapTest)

BOOST_AUTO_TEST_CASE(rangemap_test_getkv) {
  auto map = RangeMap<int, int>();
  auto result = map.size();
  BOOST_CHECK_EQUAL(result, 0);
  auto result2 = map.getKeyValue(0);
  BOOST_CHECK_EQUAL(result2.has_value(), false);
}

BOOST_AUTO_TEST_CASE(rangemap_test_ins) {
  auto map = RangeMap<int, int>();
  auto result = map.size();
  BOOST_CHECK_EQUAL(result, 0);
  auto map2 = map.inserted(std::make_pair(0,1), 1);
  map2.print();
  result = map2.size();
  BOOST_CHECK_EQUAL(result, 1);
}

BOOST_AUTO_TEST_CASE(rangemap_test_del) {
  auto map = RangeMap<int, int>();
  auto result = map.size();
  BOOST_CHECK_EQUAL(result, 0);
  auto map2 = map.deleted(std::make_pair(0,1));
  result = map2.size();
  BOOST_CHECK_EQUAL(result, 0);
}


BOOST_AUTO_TEST_SUITE_END()
