#include <boost/test/unit_test.hpp>

#include <runtime/collections/rangemap.h>

BOOST_AUTO_TEST_SUITE(RangeMapTest)

BOOST_AUTO_TEST_CASE(rangemap_test_empty) {
  auto map = rng_map::RangeMap<int, int>();
  auto result = map.size();
  BOOST_CHECK_EQUAL(result, 0);
  auto result2 = map.contains(0);
  BOOST_CHECK_EQUAL(result2, false);
}

BOOST_AUTO_TEST_CASE(rangemap_test_concat_success) {
  auto m
      = (rng_map::RangeMap<int, int>()).inserted(rng_map::Range<int>(2, 4), 1);
  auto map1 = m.inserted(rng_map::Range<int>(6, 7), 1);
  auto m1
      = (rng_map::RangeMap<int, int>()).inserted(rng_map::Range<int>(0, 2), 2);
  auto m2 = m1.inserted(rng_map::Range<int>(4, 6), 3);
  auto map2 = m2.inserted(rng_map::Range<int>(8, 9), 4);
  auto map3 = map1.concat(map2);
  auto result = map3.size();
  BOOST_CHECK_EQUAL(result, 5);
  auto map4
      = (rng_map::RangeMap<int, int>()).inserted(rng_map::Range<int>(4, 6), 1);
  auto map5 = map1.concat(map4);
  result = map5.size();
  BOOST_CHECK_EQUAL(result, 1);
}

BOOST_AUTO_TEST_CASE(rangemap_test_concat_failure) {
  auto m
      = (rng_map::RangeMap<int, int>()).inserted(rng_map::Range<int>(2, 4), 1);
  auto map1 = m.inserted(rng_map::Range<int>(6, 7), 2);
  auto m1
      = (rng_map::RangeMap<int, int>()).inserted(rng_map::Range<int>(0, 3), 2);
  auto map2 = m1.inserted(rng_map::Range<int>(8, 9), 4);
  BOOST_CHECK_THROW(map1.concat(map2), std::invalid_argument);
  auto map3
      = (rng_map::RangeMap<int, int>()).inserted(rng_map::Range<int>(3, 6), 2);
  BOOST_CHECK_THROW(map1.concat(map3), std::invalid_argument);
  auto map4
      = (rng_map::RangeMap<int, int>()).inserted(rng_map::Range<int>(5, 8), 2);
  BOOST_CHECK_THROW(map1.concat(map4), std::invalid_argument);
}

BOOST_AUTO_TEST_CASE(rangemap_test_contains_key) {
  auto m1
      = (rng_map::RangeMap<int, int>()).inserted(rng_map::Range<int>(0, 5), 1);
  auto m2 = m1.inserted(rng_map::Range<int>(2, 4), 2);
  auto map = m2.deleted(rng_map::Range<int>(3, 4));
  auto result = map.size();
  BOOST_CHECK_EQUAL(result, 3);
  auto result2 = map.contains(0);
  BOOST_CHECK_EQUAL(result2, true);
  result2 = map.contains(2);
  BOOST_CHECK_EQUAL(result2, true);
  result2 = map.contains(3);
  BOOST_CHECK_EQUAL(result2, false);
  result2 = map.contains(4);
  BOOST_CHECK_EQUAL(result2, true);
  result2 = map.contains(5);
  BOOST_CHECK_EQUAL(result2, false);
  result2 = map.contains(6);
  BOOST_CHECK_EQUAL(result2, false);
}

BOOST_AUTO_TEST_CASE(rangemap_test_vlookup) {
  auto m1
      = (rng_map::RangeMap<int, int>()).inserted(rng_map::Range<int>(0, 5), 1);
  auto m2 = m1.inserted(rng_map::Range<int>(2, 4), 2);
  auto map = m2.deleted(rng_map::Range<int>(3, 4));
  auto result = map.size();
  BOOST_CHECK_EQUAL(result, 3);
  auto result2 = map.get_value(0);
  BOOST_CHECK_EQUAL(result2.has_value(), true);
  BOOST_CHECK_EQUAL(result2.value(), 1);
  result2 = map.get_value(2);
  BOOST_CHECK_EQUAL(result2.has_value(), true);
  BOOST_CHECK_EQUAL(result2.value(), 2);
  result2 = map.get_value(3);
  BOOST_CHECK_EQUAL(result2.has_value(), false);
  result2 = map.get_value(4);
  BOOST_CHECK_EQUAL(result2.has_value(), true);
  BOOST_CHECK_EQUAL(result2.value(), 1);
  result2 = map.get_value(5);
  BOOST_CHECK_EQUAL(result2.has_value(), false);
  result2 = map.get_value(6);
  BOOST_CHECK_EQUAL(result2.has_value(), false);
}

BOOST_AUTO_TEST_CASE(rangemap_test_kvlookup) {
  auto m1
      = (rng_map::RangeMap<int, int>()).inserted(rng_map::Range<int>(0, 5), 1);
  auto m2 = m1.inserted(rng_map::Range<int>(2, 4), 2);
  auto map = m2.deleted(rng_map::Range<int>(3, 4));
  auto result = map.size();
  BOOST_CHECK_EQUAL(result, 3);
  auto result2 = map.get_key_value(0);
  BOOST_CHECK_EQUAL(result2.has_value(), true);
  BOOST_CHECK_EQUAL(result2.value().second, 1);
  BOOST_CHECK_EQUAL(result2.value().first.start(), 0);
  BOOST_CHECK_EQUAL(result2.value().first.end(), 2);
  result2 = map.get_key_value(2);
  BOOST_CHECK_EQUAL(result2.has_value(), true);
  BOOST_CHECK_EQUAL(result2.value().second, 2);
  BOOST_CHECK_EQUAL(result2.value().first.start(), 2);
  BOOST_CHECK_EQUAL(result2.value().first.end(), 3);
  result2 = map.get_key_value(3);
  BOOST_CHECK_EQUAL(result2.has_value(), false);
  result2 = map.get_key_value(4);
  BOOST_CHECK_EQUAL(result2.has_value(), true);
  BOOST_CHECK_EQUAL(result2.value().second, 1);
  BOOST_CHECK_EQUAL(result2.value().first.start(), 4);
  BOOST_CHECK_EQUAL(result2.value().first.end(), 5);
  result2 = map.get_key_value(5);
  BOOST_CHECK_EQUAL(result2.has_value(), false);
  result2 = map.get_key_value(6);
  BOOST_CHECK_EQUAL(result2.has_value(), false);
}

BOOST_AUTO_TEST_CASE(rangemap_test_ins_1) {
  auto map
      = (rng_map::RangeMap<int, int>()).inserted(rng_map::Range<int>(0, 5), 1);
  auto result = map.size();
  BOOST_CHECK_EQUAL(result, 1);
  auto map2 = map.inserted(rng_map::Range<int>(2, 4), 2);
  result = map.size();
  BOOST_CHECK_EQUAL(result, 1);
  result = map2.size();
  BOOST_CHECK_EQUAL(result, 3);
  auto result2 = map.get_key_value(2);
  BOOST_CHECK_EQUAL(result2.has_value(), true);
  BOOST_CHECK_EQUAL(result2.value().second, 1);
  BOOST_CHECK_EQUAL(result2.value().first.start(), 0);
  BOOST_CHECK_EQUAL(result2.value().first.end(), 5);
  result2 = map2.get_key_value(0);
  BOOST_CHECK_EQUAL(result2.has_value(), true);
  BOOST_CHECK_EQUAL(result2.value().second, 1);
  BOOST_CHECK_EQUAL(result2.value().first.start(), 0);
  BOOST_CHECK_EQUAL(result2.value().first.end(), 2);
  result2 = map2.get_key_value(1);
  BOOST_CHECK_EQUAL(result2.has_value(), true);
  BOOST_CHECK_EQUAL(result2.value().second, 1);
  BOOST_CHECK_EQUAL(result2.value().first.start(), 0);
  BOOST_CHECK_EQUAL(result2.value().first.end(), 2);
  result2 = map2.get_key_value(2);
  BOOST_CHECK_EQUAL(result2.has_value(), true);
  BOOST_CHECK_EQUAL(result2.value().second, 2);
  BOOST_CHECK_EQUAL(result2.value().first.start(), 2);
  BOOST_CHECK_EQUAL(result2.value().first.end(), 4);
  result2 = map2.get_key_value(3);
  BOOST_CHECK_EQUAL(result2.has_value(), true);
  BOOST_CHECK_EQUAL(result2.value().second, 2);
  BOOST_CHECK_EQUAL(result2.value().first.start(), 2);
  BOOST_CHECK_EQUAL(result2.value().first.end(), 4);
  result2 = map2.get_key_value(4);
  BOOST_CHECK_EQUAL(result2.has_value(), true);
  BOOST_CHECK_EQUAL(result2.value().second, 1);
  BOOST_CHECK_EQUAL(result2.value().first.start(), 4);
  BOOST_CHECK_EQUAL(result2.value().first.end(), 5);
  result2 = map2.get_key_value(5);
  BOOST_CHECK_EQUAL(result2.has_value(), false);
}

BOOST_AUTO_TEST_CASE(rangemap_test_ins_2) {
  auto map
      = (rng_map::RangeMap<int, int>()).inserted(rng_map::Range<int>(2, 5), 1);
  auto result = map.size();
  BOOST_CHECK_EQUAL(result, 1);
  auto map2 = map.inserted(rng_map::Range<int>(4, 7), 2);
  result = map.size();
  BOOST_CHECK_EQUAL(result, 1);
  result = map2.size();
  BOOST_CHECK_EQUAL(result, 2);
  auto result2 = map2.get_key_value(4);
  BOOST_CHECK_EQUAL(result2.has_value(), true);
  BOOST_CHECK_EQUAL(result2.value().second, 2);
  BOOST_CHECK_EQUAL(result2.value().first.start(), 4);
  BOOST_CHECK_EQUAL(result2.value().first.end(), 7);
  result2 = map2.get_key_value(5);
  BOOST_CHECK_EQUAL(result2.has_value(), true);
  BOOST_CHECK_EQUAL(result2.value().second, 2);
  BOOST_CHECK_EQUAL(result2.value().first.start(), 4);
  BOOST_CHECK_EQUAL(result2.value().first.end(), 7);
  auto map3 = map.inserted(rng_map::Range<int>(5, 7), 2);
  result = map.size();
  BOOST_CHECK_EQUAL(result, 1);
  result = map3.size();
  BOOST_CHECK_EQUAL(result, 2);
  result2 = map3.get_key_value(4);
  BOOST_CHECK_EQUAL(result2.has_value(), true);
  BOOST_CHECK_EQUAL(result2.value().second, 1);
  BOOST_CHECK_EQUAL(result2.value().first.start(), 2);
  BOOST_CHECK_EQUAL(result2.value().first.end(), 5);
  result2 = map3.get_key_value(5);
  BOOST_CHECK_EQUAL(result2.has_value(), true);
  BOOST_CHECK_EQUAL(result2.value().second, 2);
  BOOST_CHECK_EQUAL(result2.value().first.start(), 5);
  BOOST_CHECK_EQUAL(result2.value().first.end(), 7);
  auto map4 = map.inserted(rng_map::Range<int>(4, 7), 1);
  result = map.size();
  BOOST_CHECK_EQUAL(result, 1);
  result = map4.size();
  BOOST_CHECK_EQUAL(result, 1);
  result2 = map4.get_key_value(4);
  BOOST_CHECK_EQUAL(result2.has_value(), true);
  BOOST_CHECK_EQUAL(result2.value().second, 1);
  BOOST_CHECK_EQUAL(result2.value().first.start(), 2);
  BOOST_CHECK_EQUAL(result2.value().first.end(), 7);
  result2 = map4.get_key_value(5);
  BOOST_CHECK_EQUAL(result2.has_value(), true);
  BOOST_CHECK_EQUAL(result2.value().second, 1);
  BOOST_CHECK_EQUAL(result2.value().first.start(), 2);
  BOOST_CHECK_EQUAL(result2.value().first.end(), 7);
  auto map5 = map.inserted(rng_map::Range<int>(5, 7), 1);
  result = map.size();
  BOOST_CHECK_EQUAL(result, 1);
  result = map5.size();
  BOOST_CHECK_EQUAL(result, 1);
  result2 = map5.get_key_value(4);
  BOOST_CHECK_EQUAL(result2.has_value(), true);
  BOOST_CHECK_EQUAL(result2.value().second, 1);
  BOOST_CHECK_EQUAL(result2.value().first.start(), 2);
  BOOST_CHECK_EQUAL(result2.value().first.end(), 7);
  result2 = map5.get_key_value(5);
  BOOST_CHECK_EQUAL(result2.has_value(), true);
  BOOST_CHECK_EQUAL(result2.value().second, 1);
  BOOST_CHECK_EQUAL(result2.value().first.start(), 2);
  BOOST_CHECK_EQUAL(result2.value().first.end(), 7);
  auto map6 = map.inserted(rng_map::Range<int>(0, 3), 2);
  result = map.size();
  BOOST_CHECK_EQUAL(result, 1);
  result = map6.size();
  BOOST_CHECK_EQUAL(result, 2);
  result2 = map6.get_key_value(1);
  BOOST_CHECK_EQUAL(result2.has_value(), true);
  BOOST_CHECK_EQUAL(result2.value().second, 2);
  BOOST_CHECK_EQUAL(result2.value().first.start(), 0);
  BOOST_CHECK_EQUAL(result2.value().first.end(), 3);
  result2 = map6.get_key_value(2);
  BOOST_CHECK_EQUAL(result2.has_value(), true);
  BOOST_CHECK_EQUAL(result2.value().second, 2);
  BOOST_CHECK_EQUAL(result2.value().first.start(), 0);
  BOOST_CHECK_EQUAL(result2.value().first.end(), 3);
  auto map7 = map.inserted(rng_map::Range<int>(0, 2), 2);
  result = map.size();
  BOOST_CHECK_EQUAL(result, 1);
  result = map7.size();
  BOOST_CHECK_EQUAL(result, 2);
  result2 = map7.get_key_value(1);
  BOOST_CHECK_EQUAL(result2.has_value(), true);
  BOOST_CHECK_EQUAL(result2.value().second, 2);
  BOOST_CHECK_EQUAL(result2.value().first.start(), 0);
  BOOST_CHECK_EQUAL(result2.value().first.end(), 2);
  result2 = map7.get_key_value(2);
  BOOST_CHECK_EQUAL(result2.has_value(), true);
  BOOST_CHECK_EQUAL(result2.value().second, 1);
  BOOST_CHECK_EQUAL(result2.value().first.start(), 2);
  BOOST_CHECK_EQUAL(result2.value().first.end(), 5);
  auto map8 = map.inserted(rng_map::Range<int>(0, 3), 1);
  result = map.size();
  BOOST_CHECK_EQUAL(result, 1);
  result = map8.size();
  BOOST_CHECK_EQUAL(result, 1);
  result2 = map8.get_key_value(1);
  BOOST_CHECK_EQUAL(result2.has_value(), true);
  BOOST_CHECK_EQUAL(result2.value().second, 1);
  BOOST_CHECK_EQUAL(result2.value().first.start(), 0);
  BOOST_CHECK_EQUAL(result2.value().first.end(), 5);
  result2 = map8.get_key_value(2);
  BOOST_CHECK_EQUAL(result2.has_value(), true);
  BOOST_CHECK_EQUAL(result2.value().second, 1);
  BOOST_CHECK_EQUAL(result2.value().first.start(), 0);
  BOOST_CHECK_EQUAL(result2.value().first.end(), 5);
  auto map9 = map.inserted(rng_map::Range<int>(0, 2), 1);
  result = map.size();
  BOOST_CHECK_EQUAL(result, 1);
  result = map9.size();
  BOOST_CHECK_EQUAL(result, 1);
  result2 = map9.get_key_value(1);
  BOOST_CHECK_EQUAL(result2.has_value(), true);
  BOOST_CHECK_EQUAL(result2.value().second, 1);
  BOOST_CHECK_EQUAL(result2.value().first.start(), 0);
  BOOST_CHECK_EQUAL(result2.value().first.end(), 5);
  result2 = map9.get_key_value(2);
  BOOST_CHECK_EQUAL(result2.has_value(), true);
  BOOST_CHECK_EQUAL(result2.value().second, 1);
  BOOST_CHECK_EQUAL(result2.value().first.start(), 0);
  BOOST_CHECK_EQUAL(result2.value().first.end(), 5);
  auto map10 = map.inserted(rng_map::Range<int>(1, 7), 2);
  result = map.size();
  BOOST_CHECK_EQUAL(result, 1);
  result = map10.size();
  BOOST_CHECK_EQUAL(result, 1);
  result2 = map10.get_key_value(2);
  BOOST_CHECK_EQUAL(result2.has_value(), true);
  BOOST_CHECK_EQUAL(result2.value().second, 2);
  BOOST_CHECK_EQUAL(result2.value().first.start(), 1);
  BOOST_CHECK_EQUAL(result2.value().first.end(), 7);
  result2 = map10.get_key_value(5);
  BOOST_CHECK_EQUAL(result2.has_value(), true);
  BOOST_CHECK_EQUAL(result2.value().second, 2);
  BOOST_CHECK_EQUAL(result2.value().first.start(), 1);
  BOOST_CHECK_EQUAL(result2.value().first.end(), 7);
  auto map11 = map.inserted(rng_map::Range<int>(1, 7), 1);
  result = map.size();
  BOOST_CHECK_EQUAL(result, 1);
  result = map11.size();
  BOOST_CHECK_EQUAL(result, 1);
  result2 = map11.get_key_value(2);
  BOOST_CHECK_EQUAL(result2.has_value(), true);
  BOOST_CHECK_EQUAL(result2.value().second, 1);
  BOOST_CHECK_EQUAL(result2.value().first.start(), 1);
  BOOST_CHECK_EQUAL(result2.value().first.end(), 7);
  result2 = map11.get_key_value(5);
  BOOST_CHECK_EQUAL(result2.has_value(), true);
  BOOST_CHECK_EQUAL(result2.value().second, 1);
  BOOST_CHECK_EQUAL(result2.value().first.start(), 1);
  BOOST_CHECK_EQUAL(result2.value().first.end(), 7);
}

BOOST_AUTO_TEST_CASE(rangemap_test_ins_3) {
  auto m1
      = (rng_map::RangeMap<int, int>()).inserted(rng_map::Range<int>(0, 5), 1);
  auto result = m1.size();
  BOOST_CHECK_EQUAL(result, 1);
  auto map = m1.inserted(rng_map::Range<int>(10, 15), 1);
  result = map.size();
  BOOST_CHECK_EQUAL(result, 2);
  auto map2 = map.inserted(rng_map::Range<int>(5, 10), 1);
  result = map2.size();
  BOOST_CHECK_EQUAL(result, 1);
  auto result2 = map2.get_key_value(4);
  BOOST_CHECK_EQUAL(result2.has_value(), true);
  BOOST_CHECK_EQUAL(result2.value().second, 1);
  BOOST_CHECK_EQUAL(result2.value().first.start(), 0);
  BOOST_CHECK_EQUAL(result2.value().first.end(), 15);
  auto result3 = map2.get_value(5);
  BOOST_CHECK_EQUAL(result3.has_value(), true);
  BOOST_CHECK_EQUAL(result3.value(), 1);
  result3 = map2.get_value(10);
  BOOST_CHECK_EQUAL(result3.has_value(), true);
  BOOST_CHECK_EQUAL(result3.value(), 1);
  result3 = map2.get_value(11);
  BOOST_CHECK_EQUAL(result3.has_value(), true);
  BOOST_CHECK_EQUAL(result3.value(), 1);
  auto map3 = map.inserted(rng_map::Range<int>(4, 10), 1);
  result = map3.size();
  BOOST_CHECK_EQUAL(result, 1);
  result2 = map3.get_key_value(4);
  BOOST_CHECK_EQUAL(result2.has_value(), true);
  BOOST_CHECK_EQUAL(result2.value().second, 1);
  BOOST_CHECK_EQUAL(result2.value().first.start(), 0);
  BOOST_CHECK_EQUAL(result2.value().first.end(), 15);
  result3 = map3.get_value(5);
  BOOST_CHECK_EQUAL(result3.has_value(), true);
  BOOST_CHECK_EQUAL(result3.value(), 1);
  result3 = map3.get_value(10);
  BOOST_CHECK_EQUAL(result3.has_value(), true);
  BOOST_CHECK_EQUAL(result3.value(), 1);
  result3 = map3.get_value(11);
  BOOST_CHECK_EQUAL(result3.has_value(), true);
  BOOST_CHECK_EQUAL(result3.value(), 1);
  auto map4 = map.inserted(rng_map::Range<int>(5, 11), 1);
  result = map4.size();
  BOOST_CHECK_EQUAL(result, 1);
  result2 = map4.get_key_value(4);
  BOOST_CHECK_EQUAL(result2.has_value(), true);
  BOOST_CHECK_EQUAL(result2.value().second, 1);
  BOOST_CHECK_EQUAL(result2.value().first.start(), 0);
  BOOST_CHECK_EQUAL(result2.value().first.end(), 15);
  result3 = map4.get_value(5);
  BOOST_CHECK_EQUAL(result3.has_value(), true);
  BOOST_CHECK_EQUAL(result3.value(), 1);
  result3 = map4.get_value(10);
  BOOST_CHECK_EQUAL(result3.has_value(), true);
  BOOST_CHECK_EQUAL(result3.value(), 1);
  result3 = map4.get_value(11);
  BOOST_CHECK_EQUAL(result3.has_value(), true);
  BOOST_CHECK_EQUAL(result3.value(), 1);
  auto map5 = map.inserted(rng_map::Range<int>(4, 11), 1);
  result = map5.size();
  BOOST_CHECK_EQUAL(result, 1);
  result2 = map5.get_key_value(4);
  BOOST_CHECK_EQUAL(result2.has_value(), true);
  BOOST_CHECK_EQUAL(result2.value().second, 1);
  BOOST_CHECK_EQUAL(result2.value().first.start(), 0);
  BOOST_CHECK_EQUAL(result2.value().first.end(), 15);
  result3 = map5.get_value(5);
  BOOST_CHECK_EQUAL(result3.has_value(), true);
  BOOST_CHECK_EQUAL(result3.value(), 1);
  result3 = map5.get_value(10);
  BOOST_CHECK_EQUAL(result3.has_value(), true);
  BOOST_CHECK_EQUAL(result3.value(), 1);
  result3 = map5.get_value(11);
  BOOST_CHECK_EQUAL(result3.has_value(), true);
  BOOST_CHECK_EQUAL(result3.value(), 1);
  auto map6 = map.inserted(rng_map::Range<int>(5, 10), 2);
  result = map6.size();
  BOOST_CHECK_EQUAL(result, 3);
  result2 = map6.get_key_value(4);
  BOOST_CHECK_EQUAL(result2.has_value(), true);
  BOOST_CHECK_EQUAL(result2.value().second, 1);
  BOOST_CHECK_EQUAL(result2.value().first.start(), 0);
  BOOST_CHECK_EQUAL(result2.value().first.end(), 5);
  result2 = map6.get_key_value(5);
  BOOST_CHECK_EQUAL(result2.has_value(), true);
  BOOST_CHECK_EQUAL(result2.value().second, 2);
  BOOST_CHECK_EQUAL(result2.value().first.start(), 5);
  BOOST_CHECK_EQUAL(result2.value().first.end(), 10);
  result2 = map6.get_key_value(10);
  BOOST_CHECK_EQUAL(result2.has_value(), true);
  BOOST_CHECK_EQUAL(result2.value().second, 1);
  BOOST_CHECK_EQUAL(result2.value().first.start(), 10);
  BOOST_CHECK_EQUAL(result2.value().first.end(), 15);
  result3 = map6.get_value(11);
  BOOST_CHECK_EQUAL(result3.has_value(), true);
  BOOST_CHECK_EQUAL(result3.value(), 1);
  auto map7 = map.inserted(rng_map::Range<int>(4, 10), 2);
  result = map7.size();
  BOOST_CHECK_EQUAL(result, 3);
  result2 = map7.get_key_value(4);
  BOOST_CHECK_EQUAL(result2.has_value(), true);
  BOOST_CHECK_EQUAL(result2.value().second, 2);
  BOOST_CHECK_EQUAL(result2.value().first.start(), 4);
  BOOST_CHECK_EQUAL(result2.value().first.end(), 10);
  result3 = map7.get_value(5);
  BOOST_CHECK_EQUAL(result3.has_value(), true);
  BOOST_CHECK_EQUAL(result3.value(), 2);
  result2 = map7.get_key_value(10);
  BOOST_CHECK_EQUAL(result2.has_value(), true);
  BOOST_CHECK_EQUAL(result2.value().second, 1);
  BOOST_CHECK_EQUAL(result2.value().first.start(), 10);
  BOOST_CHECK_EQUAL(result2.value().first.end(), 15);
  result3 = map7.get_value(11);
  BOOST_CHECK_EQUAL(result3.has_value(), true);
  BOOST_CHECK_EQUAL(result3.value(), 1);
  auto map8 = map.inserted(rng_map::Range<int>(5, 11), 2);
  result = map8.size();
  BOOST_CHECK_EQUAL(result, 3);
  result2 = map8.get_key_value(4);
  BOOST_CHECK_EQUAL(result2.has_value(), true);
  BOOST_CHECK_EQUAL(result2.value().second, 1);
  BOOST_CHECK_EQUAL(result2.value().first.start(), 0);
  BOOST_CHECK_EQUAL(result2.value().first.end(), 5);
  result2 = map8.get_key_value(5);
  BOOST_CHECK_EQUAL(result2.has_value(), true);
  BOOST_CHECK_EQUAL(result2.value().second, 2);
  BOOST_CHECK_EQUAL(result2.value().first.start(), 5);
  BOOST_CHECK_EQUAL(result2.value().first.end(), 11);
  result3 = map8.get_value(10);
  BOOST_CHECK_EQUAL(result3.has_value(), true);
  BOOST_CHECK_EQUAL(result3.value(), 2);
  result2 = map8.get_key_value(11);
  BOOST_CHECK_EQUAL(result2.has_value(), true);
  BOOST_CHECK_EQUAL(result2.value().second, 1);
  BOOST_CHECK_EQUAL(result2.value().first.start(), 11);
  BOOST_CHECK_EQUAL(result2.value().first.end(), 15);
  auto map9 = map.inserted(rng_map::Range<int>(4, 11), 2);
  result = map9.size();
  BOOST_CHECK_EQUAL(result, 3);
  result2 = map9.get_key_value(4);
  BOOST_CHECK_EQUAL(result2.has_value(), true);
  BOOST_CHECK_EQUAL(result2.value().second, 2);
  BOOST_CHECK_EQUAL(result2.value().first.start(), 4);
  BOOST_CHECK_EQUAL(result2.value().first.end(), 11);
  result3 = map9.get_value(5);
  BOOST_CHECK_EQUAL(result3.has_value(), true);
  BOOST_CHECK_EQUAL(result3.value(), 2);
  result3 = map9.get_value(10);
  BOOST_CHECK_EQUAL(result3.has_value(), true);
  BOOST_CHECK_EQUAL(result3.value(), 2);
  result2 = map9.get_key_value(11);
  BOOST_CHECK_EQUAL(result2.has_value(), true);
  BOOST_CHECK_EQUAL(result2.value().second, 1);
  BOOST_CHECK_EQUAL(result2.value().first.start(), 11);
  BOOST_CHECK_EQUAL(result2.value().first.end(), 15);
}

BOOST_AUTO_TEST_CASE(rangemap_test_del) {
  auto m1
      = (rng_map::RangeMap<int, int>()).inserted(rng_map::Range<int>(0, 10), 1);
  auto result = m1.size();
  BOOST_CHECK_EQUAL(result, 1);
  auto map = m1.inserted(rng_map::Range<int>(5, 10), 2);
  result = map.size();
  BOOST_CHECK_EQUAL(result, 2);
  auto map2 = map.deleted(rng_map::Range<int>(4, 6));
  result = map2.size();
  BOOST_CHECK_EQUAL(result, 2);
  auto result2 = map2.get_key_value(3);
  BOOST_CHECK_EQUAL(result2.has_value(), true);
  BOOST_CHECK_EQUAL(result2.value().second, 1);
  BOOST_CHECK_EQUAL(result2.value().first.start(), 0);
  BOOST_CHECK_EQUAL(result2.value().first.end(), 4);
  auto result3 = map2.contains(4);
  BOOST_CHECK_EQUAL(result3, false);
  result3 = map2.contains(5);
  BOOST_CHECK_EQUAL(result3, false);
  result2 = map2.get_key_value(6);
  BOOST_CHECK_EQUAL(result2.has_value(), true);
  BOOST_CHECK_EQUAL(result2.value().second, 2);
  BOOST_CHECK_EQUAL(result2.value().first.start(), 6);
  BOOST_CHECK_EQUAL(result2.value().first.end(), 10);
  auto map3 = map2.deleted(rng_map::Range<int>(8, 9));
  result = map2.size();
  BOOST_CHECK_EQUAL(result, 2);
  result = map3.size();
  BOOST_CHECK_EQUAL(result, 3);
  result2 = map3.get_key_value(7);
  BOOST_CHECK_EQUAL(result2.has_value(), true);
  BOOST_CHECK_EQUAL(result2.value().second, 2);
  BOOST_CHECK_EQUAL(result2.value().first.start(), 6);
  BOOST_CHECK_EQUAL(result2.value().first.end(), 8);
  result3 = map3.contains(8);
  BOOST_CHECK_EQUAL(result3, false);
  result2 = map3.get_key_value(9);
  BOOST_CHECK_EQUAL(result2.has_value(), true);
  BOOST_CHECK_EQUAL(result2.value().second, 2);
  BOOST_CHECK_EQUAL(result2.value().first.start(), 9);
  BOOST_CHECK_EQUAL(result2.value().first.end(), 10);
  auto map4 = map3.deleted(rng_map::Range<int>(3, 10));
  result = map4.size();
  BOOST_CHECK_EQUAL(result, 1);
  result2 = map4.get_key_value(2);
  BOOST_CHECK_EQUAL(result2.has_value(), true);
  BOOST_CHECK_EQUAL(result2.value().second, 1);
  BOOST_CHECK_EQUAL(result2.value().first.start(), 0);
  BOOST_CHECK_EQUAL(result2.value().first.end(), 3);
  result3 = map4.contains(3);
  BOOST_CHECK_EQUAL(result3, false);
  auto map5 = map4.deleted(rng_map::Range<int>(5, 7));
  result = map5.size();
  BOOST_CHECK_EQUAL(result, 1);
  result2 = map5.get_key_value(2);
  BOOST_CHECK_EQUAL(result2.has_value(), true);
  BOOST_CHECK_EQUAL(result2.value().second, 1);
  BOOST_CHECK_EQUAL(result2.value().first.start(), 0);
  BOOST_CHECK_EQUAL(result2.value().first.end(), 3);
  auto map6 = map2.deleted(rng_map::Range<int>(3, 10));
  result = map6.size();
  BOOST_CHECK_EQUAL(result, 1);
  result2 = map6.get_key_value(2);
  BOOST_CHECK_EQUAL(result2.has_value(), true);
  BOOST_CHECK_EQUAL(result2.value().second, 1);
  BOOST_CHECK_EQUAL(result2.value().first.start(), 0);
  BOOST_CHECK_EQUAL(result2.value().first.end(), 3);
  result3 = map6.contains(3);
  BOOST_CHECK_EQUAL(result3, false);
  auto map7 = map2.deleted(rng_map::Range<int>(5, 7));
  result = map7.size();
  BOOST_CHECK_EQUAL(result, 2);
  result2 = map7.get_key_value(3);
  BOOST_CHECK_EQUAL(result2.has_value(), true);
  BOOST_CHECK_EQUAL(result2.value().second, 1);
  BOOST_CHECK_EQUAL(result2.value().first.start(), 0);
  BOOST_CHECK_EQUAL(result2.value().first.end(), 4);
  result3 = map7.contains(6);
  BOOST_CHECK_EQUAL(result3, false);
  result2 = map7.get_key_value(7);
  BOOST_CHECK_EQUAL(result2.has_value(), true);
  BOOST_CHECK_EQUAL(result2.value().second, 2);
  BOOST_CHECK_EQUAL(result2.value().first.start(), 7);
  BOOST_CHECK_EQUAL(result2.value().first.end(), 10);
  auto map8 = map3.deleted(rng_map::Range<int>(4, 9));
  result = map8.size();
  BOOST_CHECK_EQUAL(result, 2);
  result2 = map8.get_key_value(3);
  BOOST_CHECK_EQUAL(result2.has_value(), true);
  BOOST_CHECK_EQUAL(result2.value().second, 1);
  BOOST_CHECK_EQUAL(result2.value().first.start(), 0);
  BOOST_CHECK_EQUAL(result2.value().first.end(), 4);
  result3 = map8.contains(6);
  BOOST_CHECK_EQUAL(result3, false);
  result3 = map8.contains(7);
  BOOST_CHECK_EQUAL(result3, false);
  result2 = map8.get_key_value(9);
  BOOST_CHECK_EQUAL(result2.has_value(), true);
  BOOST_CHECK_EQUAL(result2.value().second, 2);
  BOOST_CHECK_EQUAL(result2.value().first.start(), 9);
  BOOST_CHECK_EQUAL(result2.value().first.end(), 10);
}

BOOST_AUTO_TEST_CASE(rangemap_test_constr_mult) {
  auto v
      = {std::pair<rng_map::Range<int>, int>(rng_map::Range<int>(0, 2), 1),
         std::pair<rng_map::Range<int>, int>(rng_map::Range<int>(1, 3), 2)};
  auto map = rng_map::RangeMap<int, int>(v.begin(), v.end());
  auto result = map.size();
  BOOST_CHECK_EQUAL(result, 2);
  auto result2 = map.get_value(1);
  BOOST_CHECK_EQUAL(result2.has_value(), true);
  BOOST_CHECK_EQUAL(result2.value(), 2);
}

BOOST_AUTO_TEST_CASE(rangemap_test_ins_mult) {
  auto v
      = {std::pair<rng_map::Range<int>, int>(rng_map::Range<int>(0, 2), 1),
         std::pair<rng_map::Range<int>, int>(rng_map::Range<int>(1, 3), 2)};
  auto m1 = rng_map::RangeMap<int, int>();
  auto map = rng_map::inserted(m1, v.begin(), v.end());
  auto result = map.size();
  BOOST_CHECK_EQUAL(result, 2);
  auto result2 = map.get_value(1);
  BOOST_CHECK_EQUAL(result2.has_value(), true);
  BOOST_CHECK_EQUAL(result2.value(), 2);
}

BOOST_AUTO_TEST_SUITE_END()
