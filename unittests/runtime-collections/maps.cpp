#include<boost/test/unit_test.hpp>
#include<gmp.h>

#include "runtime/header.h"

extern "C" {
  map hook_MAP_element(block *key, block *value);
  map hook_MAP_concat(map *m1, map *m2);
  map hook_MAP_unit(void);
  block *hook_MAP_lookup(map *m, block *key);
  block *hook_MAP_lookupOrDefault(map *m, block *key, block *_default);
  map hook_MAP_update(map *m, block *key, block *value);
  map hook_MAP_remove(map *m, block *key);
  map hook_MAP_difference(map *m1, map *m2);
  set hook_MAP_keys(map *m);
  list hook_MAP_keys_list(map *m);
  bool hook_MAP_in_keys(block *key, map *m);
  list hook_MAP_values(map *m);
  block *hook_MAP_choice(map *m);
  size_t hook_MAP_size_long(map *m);
  mpz_ptr hook_MAP_size(map *m);
  bool hook_MAP_inclusion(map *m1, map *m2);
  map hook_MAP_updateAll(map *m1, map *m2);
  map hook_MAP_removeAll(map *map, set *set);
  bool hook_MAP_eq(map *m1, map *m2);

  bool hook_SET_in(block *, set *);
  set hook_SET_element(block *);
  block *hook_LIST_get_long(list *, size_t);
  extern block *DUMMY0;
  extern block *DUMMY1;
  block D2 = {{1}};
  block * DUMMY2 = &D2;

  size_t hash_k(block * kitem) {
    return (size_t) kitem;
  }
}

BOOST_AUTO_TEST_SUITE(MapTest)

  BOOST_AUTO_TEST_CASE(element) {
    auto map = hook_MAP_element(DUMMY0, DUMMY0);
    auto result = hook_MAP_choice(&map);
    BOOST_CHECK_EQUAL(result, DUMMY0);
    result = hook_MAP_lookup(&map, DUMMY0);
    BOOST_CHECK_EQUAL(result, DUMMY0);
  }

  BOOST_AUTO_TEST_CASE(unit) {
    auto map = hook_MAP_unit();
    auto result = hook_MAP_size_long(&map);
    BOOST_CHECK_EQUAL(result, 0);
  }

  BOOST_AUTO_TEST_CASE(concat_success) {
    auto m1 = hook_MAP_element(DUMMY0, DUMMY1);
    auto m2 = hook_MAP_element(DUMMY1, DUMMY2);
    auto map = hook_MAP_concat(&m1, &m2);
    auto result = hook_MAP_size_long(&map);
    BOOST_CHECK_EQUAL(result, 2);
  }

  BOOST_AUTO_TEST_CASE(concat_failure) {
    auto m1 = hook_MAP_element(DUMMY0, DUMMY1);
    auto m2 = hook_MAP_element(DUMMY0, DUMMY2);
    BOOST_CHECK_THROW(hook_MAP_concat(&m1, &m2), std::invalid_argument);
  }

  BOOST_AUTO_TEST_CASE(lookup_or_default) {
    auto map = hook_MAP_element(DUMMY0, DUMMY0);
    auto result = hook_MAP_lookupOrDefault(&map, DUMMY0, DUMMY1);
    BOOST_CHECK_EQUAL(result, DUMMY0);
    result = hook_MAP_lookupOrDefault(&map, DUMMY1, DUMMY1);
    BOOST_CHECK_EQUAL(result, DUMMY1);
  }

  BOOST_AUTO_TEST_CASE(update) {
    auto map = hook_MAP_element(DUMMY0, DUMMY0);
    auto result = hook_MAP_lookup(&map, DUMMY0);
    BOOST_CHECK_EQUAL(result, DUMMY0);
    auto map2 = hook_MAP_update(&map, DUMMY0, DUMMY1);
    result = hook_MAP_lookup(&map, DUMMY0);
    BOOST_CHECK_EQUAL(result, DUMMY0);
    result = hook_MAP_lookup(&map2, DUMMY0);
    BOOST_CHECK_EQUAL(result, DUMMY1);
  }

  BOOST_AUTO_TEST_CASE(remove) {
    auto map = hook_MAP_element(DUMMY0, DUMMY0);
    auto map2 = hook_MAP_remove(&map, DUMMY0);
    auto result = hook_MAP_size(&map);
    BOOST_CHECK_EQUAL(mpz_cmp_ui(result, 1), 0);
    result = hook_MAP_size(&map2);
    BOOST_CHECK_EQUAL(mpz_cmp_ui(result, 0), 0);
  }

  BOOST_AUTO_TEST_CASE(difference) {
    auto m1 = hook_MAP_element(DUMMY0, DUMMY0);
    auto m2 = hook_MAP_element(DUMMY0, DUMMY0);
    auto map = hook_MAP_difference(&m1, &m2);
    auto result = hook_MAP_size(&map);
    BOOST_CHECK_EQUAL(mpz_cmp_ui(result, 0), 0);
    m2 = hook_MAP_update(&m2, DUMMY0, DUMMY1);
    map = hook_MAP_difference(&m1, &m2);
    result = hook_MAP_size(&map);
    BOOST_CHECK_EQUAL(mpz_cmp_ui(result, 1), 0);
  }

  BOOST_AUTO_TEST_CASE(keys) {
    auto map = hook_MAP_element(DUMMY0, DUMMY0);
    auto set = hook_MAP_keys(&map);
    BOOST_CHECK(hook_SET_in(DUMMY0, &set));
  }

  BOOST_AUTO_TEST_CASE(keys_list) {
    auto map = hook_MAP_element(DUMMY0, DUMMY0);
    auto list = hook_MAP_keys_list(&map);
    BOOST_CHECK_EQUAL(hook_LIST_get_long(&list, 0), DUMMY0);
  }

  BOOST_AUTO_TEST_CASE(in_keys) {
    auto map = hook_MAP_element(DUMMY0, DUMMY0);
    auto result = hook_MAP_in_keys(DUMMY0, &map);
    BOOST_CHECK(result);
    result = hook_MAP_in_keys(DUMMY1, &map);
    BOOST_CHECK(!result);
  }

  BOOST_AUTO_TEST_CASE(values) {
    auto map = hook_MAP_element(DUMMY0, DUMMY0);
    auto list = hook_MAP_values(&map);
    BOOST_CHECK_EQUAL(hook_LIST_get_long(&list, 0), DUMMY0);
  }

  BOOST_AUTO_TEST_CASE(inclusion) {
    auto m1 = hook_MAP_element(DUMMY0, DUMMY0);
    auto m2 = hook_MAP_element(DUMMY1, DUMMY1);
    auto result = hook_MAP_inclusion(&m1, &m2);
    BOOST_CHECK(!result);
    m2 = hook_MAP_update(&m2, DUMMY0, DUMMY0);
    result = hook_MAP_inclusion(&m1, &m2);
    BOOST_CHECK(result);
    m2 = hook_MAP_element(DUMMY0, DUMMY1);
    result = hook_MAP_inclusion(&m1, &m2);
    BOOST_CHECK(!result);
  }

  BOOST_AUTO_TEST_CASE(update_all) {
    auto m1 = hook_MAP_element(DUMMY0, DUMMY0);
    auto m2 = hook_MAP_element(DUMMY1, DUMMY1);
    m2 = hook_MAP_update(&m2, DUMMY0, DUMMY1);
    auto map = hook_MAP_updateAll(&m1, &m2);
    auto result = hook_MAP_lookup(&map, DUMMY0);
    BOOST_CHECK_EQUAL(result, DUMMY1);
    result = hook_MAP_lookup(&map, DUMMY1);
    BOOST_CHECK_EQUAL(result, DUMMY1);
  }

  BOOST_AUTO_TEST_CASE(remove_all) {
    auto set = hook_SET_element(DUMMY0);
    auto m1 = hook_MAP_element(DUMMY0, DUMMY0);
    auto m2 = hook_MAP_removeAll(&m1, &set);
    auto result = hook_MAP_size(&m2);
    BOOST_CHECK_EQUAL(mpz_cmp_ui(result, 0), 0);
  }

  BOOST_AUTO_TEST_CASE(eq) {
    auto m1 = hook_MAP_element(DUMMY0, DUMMY0);
    auto m2 = hook_MAP_element(DUMMY1, DUMMY1);
    auto result = hook_MAP_eq(&m1, &m2);
    BOOST_CHECK(!result);
    m2 = hook_MAP_element(DUMMY0, DUMMY0);
    result = hook_MAP_eq(&m1, &m2);
    BOOST_CHECK(result);
  }

BOOST_AUTO_TEST_SUITE_END()
