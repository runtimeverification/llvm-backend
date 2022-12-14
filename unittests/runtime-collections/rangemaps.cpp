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

BOOST_AUTO_TEST_CASE(rangemap_test_ins_1) {
  auto map = (RangeMap<int, int>()).inserted(std::make_pair(0,5), 1);
  auto result = map.size();
  BOOST_CHECK_EQUAL(result, 1);
//  map.print();
  auto map2 = map.inserted(std::make_pair(2,4), 2);
  result = map.size();
  BOOST_CHECK_EQUAL(result, 1);
//  map.print();
  result = map2.size();
  BOOST_CHECK_EQUAL(result, 3);
//  map2.print();
}

BOOST_AUTO_TEST_CASE(rangemap_test_ins_2) {
  auto map = (RangeMap<int, int>()).inserted(std::make_pair(2,5), 1);
  auto result = map.size();
  BOOST_CHECK_EQUAL(result, 1);
  map.print();
  auto map2 = map.inserted(std::make_pair(4,7), 2);
  result = map.size();
  BOOST_CHECK_EQUAL(result, 1);
  map.print();
  result = map2.size();
  BOOST_CHECK_EQUAL(result, 2);
  map2.print();
  auto map3 = map.inserted(std::make_pair(5,7), 2);
  result = map.size();
  BOOST_CHECK_EQUAL(result, 1);
  map.print();
  result = map3.size();
  BOOST_CHECK_EQUAL(result, 2);
  map3.print();
  auto map4 = map.inserted(std::make_pair(4,7), 1);
  result = map.size();
  BOOST_CHECK_EQUAL(result, 1);
  map.print();
  result = map4.size();
  BOOST_CHECK_EQUAL(result, 1);
  map4.print();
  auto map5 = map.inserted(std::make_pair(5,7), 1);
  result = map.size();
  BOOST_CHECK_EQUAL(result, 1);
  map.print();
  result = map5.size();
  BOOST_CHECK_EQUAL(result, 1);
  map5.print();
  auto map6 = map.inserted(std::make_pair(0,3), 2);
  result = map.size();
  BOOST_CHECK_EQUAL(result, 1);
  map.print();
  result = map6.size();
  BOOST_CHECK_EQUAL(result, 2);
  map6.print();
  auto map7 = map.inserted(std::make_pair(0,2), 2);
  result = map.size();
  BOOST_CHECK_EQUAL(result, 1);
  map.print();
  result = map7.size();
  BOOST_CHECK_EQUAL(result, 2);
  map7.print();
  auto map8 = map.inserted(std::make_pair(0,3), 1);
  result = map.size();
  BOOST_CHECK_EQUAL(result, 1);
  map.print();
  result = map8.size();
  BOOST_CHECK_EQUAL(result, 1);
  map8.print();
  auto map9 = map.inserted(std::make_pair(0,2), 1);
  result = map.size();
  BOOST_CHECK_EQUAL(result, 1);
  map.print();
  result = map9.size();
  BOOST_CHECK_EQUAL(result, 1);
  map9.print();
}

BOOST_AUTO_TEST_CASE(rangemap_test_ins_3) {
  auto map = (RangeMap<int, int>()).inserted(std::make_pair(0,5), 1);
  auto result = map.size();
  BOOST_CHECK_EQUAL(result, 1);
  auto map2 = map.inserted(std::make_pair(10,15), 1);
  result = map2.size();
  BOOST_CHECK_EQUAL(result, 2);
  map2.print();
  auto map3 = map2.inserted(std::make_pair(5,10), 1);
  result = map3.size();
  BOOST_CHECK_EQUAL(result, 1);
  map3.print();
  auto map4 = map2.inserted(std::make_pair(4,10), 1);
  result = map4.size();
  BOOST_CHECK_EQUAL(result, 1);
  map4.print();
  auto map5 = map2.inserted(std::make_pair(5,11), 1);
  result = map5.size();
  BOOST_CHECK_EQUAL(result, 1);
  map5.print();
  auto map6 = map2.inserted(std::make_pair(4,11), 1);
  result = map6.size();
  BOOST_CHECK_EQUAL(result, 1);
  map6.print();
  auto map7 = map2.inserted(std::make_pair(5,10), 2);
  result = map7.size();
  BOOST_CHECK_EQUAL(result, 3);
  map7.print();
  auto map8 = map2.inserted(std::make_pair(4,10), 2);
  result = map8.size();
  BOOST_CHECK_EQUAL(result, 3);
  map8.print();
  auto map9 = map2.inserted(std::make_pair(5,11), 2);
  result = map9.size();
  BOOST_CHECK_EQUAL(result, 3);
  map9.print();
  auto map10 = map2.inserted(std::make_pair(4,11), 2);
  result = map10.size();
  BOOST_CHECK_EQUAL(result, 3);
  map10.print();
}

BOOST_AUTO_TEST_CASE(rangemap_test_del) {
  auto map = RangeMap<int, int>();
  auto result = map.size();
  BOOST_CHECK_EQUAL(result, 0);
  auto map2 = map.inserted(std::make_pair(0,1), 1);
  result = map2.size();
  BOOST_CHECK_EQUAL(result, 1);
  auto map3 = map2.deleted(std::make_pair(0,1));
  result = map3.size();
  BOOST_CHECK_EQUAL(result, 0);
}


BOOST_AUTO_TEST_SUITE_END()
