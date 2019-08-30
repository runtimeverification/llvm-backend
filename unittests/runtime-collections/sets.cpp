#include<boost/test/unit_test.hpp>
#include<gmp.h>

#include "runtime/header.h"

extern "C" {
  set hook_SET_element(block *elem);
  set hook_SET_unit(void);
  bool hook_SET_in(block *elem, set *set);
  set hook_SET_concat(set *s1, set *s2);
  set hook_SET_difference(set *s1, set *s2);
  set hook_SET_remove(set *s, block *elem);
  bool hook_SET_inclusion(set *s1, set *s2);
  set hook_SET_intersection(set *s1, set *s2);
  block *hook_SET_choice(set *s);
  mpz_ptr hook_SET_size(set *s);
  list hook_SET_set2list(set *s);
  set hook_SET_list2set(list *l);
  bool hook_SET_eq(set *s1, set *s2);

  extern block *DUMMY0, *DUMMY1, *DUMMY2;
}

BOOST_AUTO_TEST_SUITE(SetTest)

  BOOST_AUTO_TEST_CASE(element) {
    auto set = hook_SET_element(DUMMY0);
    auto result = hook_SET_choice(&set);
    BOOST_CHECK_EQUAL(result, DUMMY0);
    auto contains = hook_SET_in(DUMMY0, &set);
    BOOST_CHECK(contains);
  }

  BOOST_AUTO_TEST_CASE(unit) {
    auto set = hook_SET_unit();
    auto result = hook_SET_size(&set);
    BOOST_CHECK_EQUAL(__gmpz_cmp_ui(result, 0), 0);
    free_int(result);
  }

  BOOST_AUTO_TEST_CASE(concat) {
    auto s1 = hook_SET_element(DUMMY0);
    auto s2 = hook_SET_element(DUMMY1);
    auto set = hook_SET_concat(&s1, &s2);
    auto result = hook_SET_size(&set);
    BOOST_CHECK_EQUAL(__gmpz_cmp_ui(result, 2), 0);
    free_int(result);
  }

  BOOST_AUTO_TEST_CASE(difference) {
    auto s1 = hook_SET_element(DUMMY0);
    auto s2 = hook_SET_element(DUMMY0);
    auto s3 = hook_SET_element(DUMMY1);
    auto set = hook_SET_difference(&s1, &s2);
    auto result = hook_SET_size(&set);
    BOOST_CHECK_EQUAL(__gmpz_cmp_ui(result, 0), 0);
    auto s1 = hook_SET_concat(&s1, &s3);
    auto set = hook_SET_difference(&s1, &s3);
    auto result = hook_SET_size(&set);
    BOOST_CHECK_EQUAL(__gmpz_cmp_ui(result, 1), 0);
    free_int(result);
  }

  BOOST_AUTO_TEST_CASE(inclusion) {
    auto s1 = hook_SET_element(DUMMY0);
    auto s2 = hook_SET_element(DUMMY1);
    auto result = hook_SET_inclusion(&s1, &s2);
    BOOST_CHECK(!result);
    auto s2 = hook_SET_concat(&s2, &s1);
    auto result = hook_SET_inclusion(&s1, &s2);
    BOOST_CHECK(result);
  }

  BOOST_AUTO_TEST_CASE(intersection) {
    auto s1 = hook_SET_element(DUMMY0);
    auto s2 = hook_SET_element(DUMMY1);
    auto s3 = hook_SET_element(DUMMY2);
    auto s3 = hook_SET_concat(&s3, &s1);
    auto set = hook_SET_intersection(&s1, &s2);
    auto result = hook_SET_size(&set);
    BOOST_CHECK_EQUAL(__gmpz_cmp_ui(result, 0), 0);
    auto s1 = hook_SET_concat(&s1, &s2);
    auto set = hook_SET_intersection(&s1, &s3);
    auto result = hook_SET_size(&set);
    BOOST_CHECK_EQUAL(__gmpz_cmp_ui(result, 1), 0);
    auto set = hook_SET_intersection(&s3, &s1);
    auto result = hook_SET_size(&set);
    BOOST_CHECK_EQUAL(__gmpz_cmp_ui(result, 1), 0);
    auto set = hook_SET_intersection(&s2, &s2);
    auto result = hook_SET_size(&set);
    BOOST_CHECK_EQUAL(__gmpz_cmp_ui(result, 1), 0);
    free_int(result);
  }

  BOOST_AUTO_TEST_CASE(set2list) {
    auto set = hook_SET_element(DUMMY0);
    auto set2 = hook_SET_element(DUMMY1);
    auto set = hook_SET_concat(&set, &set2);
    auto set2 = hook_SET_element(DUMMY2);
    auto set = hook_SET_concat(&set, &set2);
    auto list = hook_SET_set2list(&set);
    BOOST_CHECK_EQUAL((list).len(), 3);
  }

  BOOST_AUTO_TEST_CASE(list2set) {
    auto list = list();
    list.push_back(DUMMY0);
    list.push_back(DUMMY1);
    list.push_back(DUMMY2);
    auto set = hook_SET_list2set(&list);
    auto result = hook_SET_size(&set);
    BOOST_CHECK_EQUAL(__gmpz_cmp_ui(result, 3), 0);
    auto contains = hook_SET_in(DUMMY0, &set);
    BOOST_CHECK(contains);
    auto contains = hook_SET_in(DUMMY1, &set);
    BOOST_CHECK(contains);
    auto contains = hook_SET_in(DUMMY2, &set);
    BOOST_CHECK(contains);
  }

  BOOST_AUTO_TEST_CASE(eq) {
    auto set = hook_SET_element(DUMMY0);
    auto set2 = hook_SET_element(DUMMY1);
    auto result = hook_SET_eq(&set, &set2);
    BOOST_CHECK(!result);
    auto set2 = hook_SET_element(DUMMY0);
    auto result = hook_SET_eq(&set, &set2);
    BOOST_CHECK(result);
  }

BOOST_AUTO_TEST_SUITE_END()
