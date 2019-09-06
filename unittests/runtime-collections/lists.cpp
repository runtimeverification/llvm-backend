#include<boost/test/unit_test.hpp>
#include<gmp.h>

#include "runtime/header.h"

extern "C" {
  list hook_LIST_unit();
  list hook_LIST_element(block *);
  list hook_LIST_concat(list *, list *);
  list hook_LIST_range(list *, mpz_t, mpz_t);
  list hook_LIST_range_long(list *, size_t, size_t);
  list hook_LIST_make(mpz_t len, block * value);
  list hook_LIST_update(list * list, mpz_t index, block * value);
  list hook_LIST_updateAll(list * l1, mpz_t index, list * l2);
  list hook_LIST_fill(list * l, mpz_t index, mpz_t len, block * val);
  mpz_ptr hook_LIST_size(list *);
  block * hook_LIST_get(list *, mpz_t);
  bool hook_LIST_in(block *, list *);
  bool hook_LIST_eq(list *, list *);

  mpz_ptr move_int(mpz_t i) {
    mpz_ptr result = (mpz_ptr)malloc(sizeof(__mpz_struct));
    *result = *i;
    return result;
  }

  bool during_gc() {
    return false;
  }

  void *koreAllocToken(size_t requested) {
    return malloc(requested);
  }
  const size_t BLOCK_SIZE = -1;

  void printConfigurationInternal(writer *file, block *subject, const char *sort, bool) {}
  void sfprintf(writer *, const char *, ...) {}

  bool hook_KEQUAL_eq(block * b1, block * b2) {
    return b1->h.hdr == b2->h.hdr;
  }

  bool hash_enter(void) {
    return true;
  }

  void hash_exit(void) {}

  void k_hash(block *, void *) {}

  block D0 = {{0}};
  block * DUMMY0 = &D0;
  block D1 = {{1}};
  block * DUMMY1 = &D1;
}

BOOST_AUTO_TEST_SUITE(ListTest)

  BOOST_AUTO_TEST_CASE(element) {
    list list = hook_LIST_element(DUMMY0);
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
    list list = hook_LIST_unit();
    mpz_ptr result = hook_LIST_size(&list);
    BOOST_CHECK_EQUAL(0, mpz_cmp_ui(result, 0));
  }

  BOOST_AUTO_TEST_CASE(concat) {
    list l1 = hook_LIST_element(DUMMY0);
    list l2 = hook_LIST_element(DUMMY1);
    list list = hook_LIST_concat(&l1, &l2);

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
    list list = hook_LIST_element(DUMMY0);
    bool result = hook_LIST_in(DUMMY0, &list);
    BOOST_CHECK(result == true);
    result = hook_LIST_in(DUMMY1, &list);
    BOOST_CHECK(result == false);
  }

  BOOST_AUTO_TEST_CASE(get_negative) {
    mpz_t index;
    mpz_init_set_si(index, -2);
    list list = hook_LIST_element(DUMMY0);
    BOOST_CHECK_THROW(hook_LIST_get(&list, index), std::out_of_range);
  }

  BOOST_AUTO_TEST_CASE(get_out_of_range) {
    mpz_t index;
    mpz_init_set_si(index, 1);
    list list = hook_LIST_element(DUMMY0);

    BOOST_CHECK_THROW(hook_LIST_get(&list, index), std::out_of_range);
  }


  BOOST_AUTO_TEST_CASE(get_range_neg_idx) {
    mpz_t neg, zero;
    mpz_init_set_si(neg, -1);
    mpz_init_set_si(zero, 0);
    list list = hook_LIST_element(DUMMY0);

    BOOST_CHECK_THROW(hook_LIST_range(&list, neg, zero), std::invalid_argument);
  }

  BOOST_AUTO_TEST_CASE(range_neg_len) {
    mpz_t neg, zero;
    mpz_init_set_si(neg, -1);
    mpz_init_set_si(zero, 0);
    list list = hook_LIST_element(DUMMY0);

    BOOST_CHECK_THROW(hook_LIST_range(&list, zero, neg), std::invalid_argument);
  }

  BOOST_AUTO_TEST_CASE(range) {
    mpz_t zero, one;
    mpz_init_set_ui(zero, 0);
    mpz_init_set_ui(one, 1);

    list l = hook_LIST_element(DUMMY0);
    list result = hook_LIST_range(&l, zero, one);

    mpz_ptr size = hook_LIST_size(&result);
    BOOST_CHECK_EQUAL(0, mpz_cmp_ui(size, 0));

    l = hook_LIST_concat(&l, &l);
    result = hook_LIST_range(&l, one, zero);

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
    list l = hook_LIST_make(ten, DUMMY0);
    block * result = hook_LIST_get(&l, zero);

    BOOST_CHECK_EQUAL(true, hook_KEQUAL_eq(result, DUMMY0));
    mpz_ptr size = hook_LIST_size(&l);

    BOOST_CHECK_EQUAL(0, mpz_cmp_ui(size, 10));
  }

  BOOST_AUTO_TEST_CASE(update_neg) {
    list list = hook_LIST_element(DUMMY0);
    mpz_t neg;
    mpz_init_set_si(neg, -1);

    BOOST_CHECK_THROW(hook_LIST_update(&list, neg, DUMMY1), std::invalid_argument);
  }

  BOOST_AUTO_TEST_CASE(update_out_of_range) {
    list list = hook_LIST_element(DUMMY0);
    mpz_t one;
    mpz_init_set_ui(one, 1);

    BOOST_CHECK_THROW(hook_LIST_update(&list, one, DUMMY1), std::invalid_argument);
  }

  BOOST_AUTO_TEST_CASE(update) {
    list list = hook_LIST_element(DUMMY0);
    mpz_t index;
    mpz_init_set_ui(index, 0);
    list = hook_LIST_update(&list, index, DUMMY1);
    block * result = hook_LIST_get(&list, index);

    BOOST_CHECK_EQUAL(true, hook_KEQUAL_eq(result, DUMMY1));
  }

  BOOST_AUTO_TEST_CASE(update_all_neg) {
    mpz_t neg;
    mpz_init_set_si(neg, -1);
    list l1 = hook_LIST_element(DUMMY0);
    list l2 = hook_LIST_unit();

    BOOST_CHECK_THROW(hook_LIST_updateAll(&l1, neg, &l2), std::invalid_argument);
  }

  BOOST_AUTO_TEST_CASE(update_all) {
    mpz_t zero, one;
    mpz_init_set_ui(zero, 0);
    mpz_init_set_ui(one, 1);
    list l1 = hook_LIST_element(DUMMY0);
    list l2 = hook_LIST_unit();
    list list = hook_LIST_updateAll(&l1, one, &l2);
    block * result = hook_LIST_get(&list, zero);

    BOOST_CHECK_EQUAL(true, hook_KEQUAL_eq(result, DUMMY0));
    list = hook_LIST_updateAll(&l1, zero, &l2);
    result = hook_LIST_get(&list, zero);

    BOOST_CHECK_EQUAL(true, hook_KEQUAL_eq(result, DUMMY0));
    l2 = hook_LIST_element(DUMMY1);
    list = hook_LIST_updateAll(&l1, zero, &l2);
    result = hook_LIST_get(&list, zero);

    BOOST_CHECK_EQUAL(true, hook_KEQUAL_eq(result, DUMMY1));
  }

  BOOST_AUTO_TEST_CASE(update_all_out_of_range) {
    mpz_t one;
    mpz_init_set_ui(one, 1);
    list l1 = hook_LIST_element(DUMMY0);
    list l2 = hook_LIST_element(DUMMY1);

    BOOST_CHECK_THROW(hook_LIST_updateAll(&l1, one, &l2), std::invalid_argument);
  }

  BOOST_AUTO_TEST_CASE(fill_out_of_range) {
    list list = hook_LIST_unit();
    mpz_t index, len;
    mpz_init_set_ui(index, 1);
    mpz_init_set_ui(len, 2);

    BOOST_CHECK_THROW(hook_LIST_fill(&list, index, len, DUMMY0), std::out_of_range);
  }

  BOOST_AUTO_TEST_CASE(fill) {
    mpz_t zero, one, two, three, four;
    mpz_init_set_ui(zero, 0);
    mpz_init_set_ui(one, 1);
    mpz_init_set_ui(two, 2);
    mpz_init_set_ui(three, 3);
    mpz_init_set_ui(four, 4);

    list l1 = hook_LIST_make(four, DUMMY0);
    list l2 = hook_LIST_fill(&l1, one, two, DUMMY1);

    block * result = hook_LIST_get(&l2, zero);
    BOOST_CHECK_EQUAL(true, hook_KEQUAL_eq(result, DUMMY0));

    result = hook_LIST_get(&l2, one);
    BOOST_CHECK_EQUAL(true, hook_KEQUAL_eq(result, DUMMY1));

    result = hook_LIST_get(&l2, two);
    BOOST_CHECK_EQUAL(true, hook_KEQUAL_eq(result, DUMMY1));

    result = hook_LIST_get(&l2, three);
    BOOST_CHECK_EQUAL(true, hook_KEQUAL_eq(result, DUMMY0));
  }

  BOOST_AUTO_TEST_CASE(eq) {
    list l1 = hook_LIST_element(DUMMY0);
    list l2 = hook_LIST_unit();
    bool result = hook_LIST_eq(&l1, &l2);

    BOOST_CHECK(!result);

    l2 = hook_LIST_element(DUMMY0);
    result = hook_LIST_eq(&l1, &l2);
    BOOST_CHECK(result);
  }
BOOST_AUTO_TEST_SUITE_END()
