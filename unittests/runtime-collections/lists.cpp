#include<boost/test/unit_test.hpp>
#include<gmp.h>

#include "runtime/header.h"

#include "immer/vector.hpp"

using List = immer::vector<block *>;

extern "C" {
  List hook_LIST_unit();
  List hook_LIST_element(block *);
  List hook_LIST_concat(List *, List *);
  List hook_LIST_range(List *, mpz_t, mpz_t);
  List hook_LIST_range_long(List *, size_t, size_t);
  List hook_LIST_make(mpz_t len, block * value);
  List hook_LIST_update(List * list, mpz_t index, block * value);
  List hook_LIST_updateAll(List * l1, mpz_t index, List * l2);
  mpz_ptr hook_LIST_size(List *);
  block * hook_LIST_get(List *, mpz_t);
  bool hook_LIST_in(block *, List *);

  mpz_ptr move_int(mpz_t i) {
    mpz_ptr result = (mpz_ptr)malloc(sizeof(__mpz_struct));
    *result = *i;
    return result;
  }

  void printConfigurationInternal(FILE *file, block *subject, const char *sort, bool) {}

  bool hook_KEQUAL_eq(block * b1, block * b2) {
    return b1->h.hdr == b2->h.hdr;
  }

  block D0 = {{0}, {}};
  block * DUMMY0 = &D0;
  block D1 = {{1}, {}};
  block * DUMMY1 = &D1;
  block D2 = {{2}, {}};
  block * DUMMY2 = &D2;
  block D3 = {{3}, {}};
  block * DUMMY3 = &D3;
}

BOOST_AUTO_TEST_SUITE(CollectionsTest)

  BOOST_AUTO_TEST_CASE(element) {
    List list = hook_LIST_element(DUMMY0);
    mpz_t index;
    mpz_init_set_ui(index, 0);
    block * result = hook_LIST_get(&list, index);
    BOOST_CHECK_EQUAL(true, hook_KEQUAL_eq(result, DUMMY0));

    mpz_t index2;
    mpz_init_set_si(index2, -1);
    result = hook_LIST_get(&list, index2);
    BOOST_CHECK_EQUAL(true, hook_KEQUAL_eq(result, DUMMY0));
  }

  BOOST_AUTO_TEST_CASE(unit) {
    List list = hook_LIST_unit();
    mpz_ptr result = hook_LIST_size(&list);
    BOOST_CHECK_EQUAL(0, mpz_cmp_ui(result, 0));
  }

  BOOST_AUTO_TEST_CASE(concat) {
    List l1 = hook_LIST_element(DUMMY0);
    List l2 = hook_LIST_element(DUMMY1);
    List list = hook_LIST_concat(&l1, &l2);

    mpz_t index;
    mpz_init_set_ui(index, 0);
    block * result = hook_LIST_get(&list, index);

    BOOST_CHECK_EQUAL(true, hook_KEQUAL_eq(result, DUMMY0));

    mpz_set_ui(index, 1);
    result = hook_LIST_get(&list, index);

    BOOST_CHECK_EQUAL(true, hook_KEQUAL_eq(result, DUMMY1));
    mpz_ptr size = hook_LIST_size(&list);

    BOOST_CHECK_EQUAL(0, mpz_cmp_ui(size, 2));
  }

  BOOST_AUTO_TEST_CASE(in) {
    List list = hook_LIST_element(DUMMY0);
    bool result = hook_LIST_in(DUMMY0, &list);
    BOOST_CHECK(result == true);
    result = hook_LIST_in(DUMMY1, &list);
    BOOST_CHECK(result == false);
  }

  BOOST_AUTO_TEST_CASE(get_negative) {
    mpz_t index;
    mpz_init_set_si(index, -2);
    List list = hook_LIST_element(DUMMY0);
    BOOST_CHECK_THROW(hook_LIST_get(&list, index), std::out_of_range);
  }

  BOOST_AUTO_TEST_CASE(get_out_of_range) {
    mpz_t index;
    mpz_init_set_si(index, 1);
    List list = hook_LIST_element(DUMMY0);

    BOOST_CHECK_THROW(hook_LIST_get(&list, index), std::out_of_range);
  }


  BOOST_AUTO_TEST_CASE(get_range_neg_idx) {
    mpz_t neg, zero;
    mpz_init_set_si(neg, -1);
    mpz_init_set_si(zero, 0);
    List list = hook_LIST_element(DUMMY0);

    BOOST_CHECK_THROW(hook_LIST_range(&list, neg, zero), std::invalid_argument);
  }

  BOOST_AUTO_TEST_CASE(range_neg_len) {
    mpz_t neg, zero;
    mpz_init_set_si(neg, -1);
    mpz_init_set_si(zero, 0);
    List list = hook_LIST_element(DUMMY0);

    BOOST_CHECK_THROW(hook_LIST_range(&list, zero, neg), std::invalid_argument);
  }

  BOOST_AUTO_TEST_CASE(range) {
    mpz_t zero, one;
    mpz_init_set_ui(zero, 0);
    mpz_init_set_ui(one, 1);

    List list = hook_LIST_element(DUMMY0);
    List result = hook_LIST_range(&list, zero, one);

    mpz_ptr size = hook_LIST_size(&result);
    BOOST_CHECK_EQUAL(0, mpz_cmp_ui(size, 0));

    list = hook_LIST_concat(&list, &list);
    result = hook_LIST_range(&list, one, zero);

    size = hook_LIST_size(&result);
    BOOST_CHECK_EQUAL(0, mpz_cmp_ui(size, 1));
  }

  BOOST_AUTO_TEST_CASE(make_out_of_range) {
    mpz_t neg;
    mpz_init_set_si(neg, -1);
    BOOST_CHECK_THROW(hook_LIST_make(neg, DUMMY0), std::invalid_argument);
  }

  BOOST_AUTO_TEST_CASE(make) {
    mpz_t zero, ten;
    mpz_init_set_ui(zero, 0);
    mpz_init_set_ui(ten, 10);
    List list = hook_LIST_make(ten, DUMMY0);
    block * result = hook_LIST_get(&list, zero);

    BOOST_CHECK_EQUAL(true, hook_KEQUAL_eq(result, DUMMY0));
    mpz_ptr size = hook_LIST_size(&list);

    BOOST_CHECK_EQUAL(0, mpz_cmp_ui(size, 10));
  }

  BOOST_AUTO_TEST_CASE(update_neg) {
    List list = hook_LIST_element(DUMMY0);
    mpz_t neg;
    mpz_init_set_si(neg, -1);

    BOOST_CHECK_THROW(hook_LIST_update(&list, neg, DUMMY1), std::invalid_argument);
  }
BOOST_AUTO_TEST_SUITE_END()
