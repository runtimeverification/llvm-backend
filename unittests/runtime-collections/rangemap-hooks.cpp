#include <boost/test/unit_test.hpp>
#include <gmp.h>

#include "runtime/header.h"

extern "C" {
rangemap hook_RANGEMAP_element(block *start, block *end, block *value);
rangemap hook_RANGEMAP_elementRng(block *rng, block *value);
rangemap hook_RANGEMAP_concat(rangemap *m1, rangemap *m2);
rangemap hook_RANGEMAP_unit(void);
block *hook_RANGEMAP_lookup(rangemap *m, block *key);
block *hook_RANGEMAP_lookupOrDefault(rangemap *m, block *key, block *_default);
block *hook_RANGEMAP_find_range(rangemap *m, block *key);
rangemap
hook_RANGEMAP_update(rangemap *m, block *start, block *end, block *value);
rangemap hook_RANGEMAP_updateRng(rangemap *m, block *rng, block *value);
rangemap hook_RANGEMAP_remove(rangemap *m, block *start, block *end);
rangemap hook_RANGEMAP_removeRng(rangemap *map, block *rng);
rangemap hook_RANGEMAP_difference(rangemap *m1, rangemap *m2);
set hook_RANGEMAP_keys(rangemap *m);
list hook_RANGEMAP_keys_list(rangemap *m);
bool hook_RANGEMAP_in_keys(block *key, rangemap *m);
list hook_RANGEMAP_values(rangemap *m);
block *hook_RANGEMAP_choice(rangemap *m);
block *hook_RANGEMAP_choiceRng(rangemap *m);
size_t hook_RANGEMAP_size_long(rangemap *m);
mpz_ptr hook_RANGEMAP_size(rangemap *m);
bool hook_RANGEMAP_inclusion(rangemap *m1, rangemap *m2);
rangemap hook_RANGEMAP_updateAll(rangemap *m1, rangemap *m2);
rangemap hook_RANGEMAP_removeAll(rangemap *map, set *set);
bool hook_RANGEMAP_eq(rangemap *m1, rangemap *m2);

set hook_SET_element(block *);
mpz_ptr hook_SET_size(set *s);
set hook_SET_unit(void);
mpz_ptr hook_LIST_size(list *);

block *hook_LIST_get_long(list *, size_t);
block RD0 = {{0}};
block *RDUMMY0 = &RD0;
block RD1 = {{1}};
block *RDUMMY1 = &RD1;
block RD2 = {{2}};
block *RDUMMY2 = &RD2;

bool hook_KEQUAL_lt(block *b1, block *b2) {
  return b1->h.hdr < b2->h.hdr;
}

void *koreAlloc(size_t requested) {
  return malloc(requested);
}

uint32_t getTagForSymbolName(const char *symbolName) {
  return 0;
}

struct blockheader getBlockHeaderForSymbol(uint32_t tag) {
  return {(uint64_t)tag};
}

struct range {
  blockheader h;
  block *start;
  block *end;
};
static struct blockheader range_header() {
  static struct blockheader hdr = {(uint64_t)-1};
  if (hdr.hdr == -1) {
    hdr = getBlockHeaderForSymbol(
        (uint64_t)getTagForSymbolName("range{SortKItem{}, SortKItem{}}"));
  }
  return hdr;
}
struct inj_range2kitem {
  blockheader h;
  range *child;
};
static struct blockheader inj_range2kitem_header() {
  static struct blockheader hdr = {(uint64_t)-1};
  if (hdr.hdr == -1) {
    hdr = getBlockHeaderForSymbol(
        (uint64_t)getTagForSymbolName("inj{SortRange{}, SortKItem{}}"));
  }
  return hdr;
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

BOOST_AUTO_TEST_CASE(rangemap_hook_element_rng) {
  range *ptr = (range *)koreAlloc(sizeof(range));
  ptr->h = range_header();
  ptr->start = RDUMMY0;
  ptr->end = RDUMMY1;
  auto map = hook_RANGEMAP_elementRng((SortRange)ptr, RDUMMY0);
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

BOOST_AUTO_TEST_CASE(rangemap_hook_find_range) {
  auto map = hook_RANGEMAP_element(RDUMMY0, RDUMMY1, RDUMMY0);
  auto result = hook_RANGEMAP_find_range(&map, RDUMMY0);
  BOOST_CHECK_EQUAL((block *)result->children[0], RDUMMY0);
  BOOST_CHECK_EQUAL((block *)result->children[1], RDUMMY1);
  BOOST_CHECK_THROW(
      hook_RANGEMAP_find_range(&map, RDUMMY1), std::invalid_argument);
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

BOOST_AUTO_TEST_CASE(rangemap_hook_update_rng) {
  auto map1 = hook_RANGEMAP_element(RDUMMY0, RDUMMY2, RDUMMY0);
  auto result = hook_RANGEMAP_lookup(&map1, RDUMMY0);
  BOOST_CHECK_EQUAL(result, RDUMMY0);
  auto result2 = hook_RANGEMAP_size_long(&map1);
  BOOST_CHECK_EQUAL(result2, 1);
  range *ptr = (range *)koreAlloc(sizeof(range));
  ptr->h = range_header();
  ptr->start = RDUMMY0;
  ptr->end = RDUMMY1;
  auto map2 = hook_RANGEMAP_updateRng(&map1, (SortRange)ptr, RDUMMY1);
  result = hook_RANGEMAP_lookup(&map2, RDUMMY0);
  BOOST_CHECK_EQUAL(result, RDUMMY1);
  result2 = hook_RANGEMAP_size_long(&map2);
  BOOST_CHECK_EQUAL(result2, 2);
}

BOOST_AUTO_TEST_CASE(rangemap_hook_remove) {
  auto map = hook_RANGEMAP_element(RDUMMY0, RDUMMY1, RDUMMY0);
  auto map2 = hook_RANGEMAP_remove(&map, RDUMMY0, RDUMMY1);
  auto result = hook_RANGEMAP_size(&map);
  BOOST_CHECK_EQUAL(mpz_cmp_ui(result, 1), 0);
  result = hook_RANGEMAP_size(&map2);
  BOOST_CHECK_EQUAL(mpz_cmp_ui(result, 0), 0);
}

BOOST_AUTO_TEST_CASE(rangemap_hook_remove_rng) {
  auto m1 = hook_RANGEMAP_element(RDUMMY0, RDUMMY2, RDUMMY0);
  auto map1 = hook_RANGEMAP_update(&m1, RDUMMY0, RDUMMY1, RDUMMY1);
  auto result = hook_RANGEMAP_size_long(&map1);
  BOOST_CHECK_EQUAL(result, 2);
  range *ptr = (range *)koreAlloc(sizeof(range));
  ptr->h = range_header();
  ptr->start = RDUMMY0;
  ptr->end = RDUMMY1;
  auto map2 = hook_RANGEMAP_removeRng(&map1, (SortRange)ptr);
  result = hook_RANGEMAP_size_long(&map2);
  BOOST_CHECK_EQUAL(result, 1);
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

BOOST_AUTO_TEST_CASE(rangemap_hook_keys) {
  auto map = hook_RANGEMAP_element(RDUMMY0, RDUMMY1, RDUMMY0);
  auto set = hook_RANGEMAP_keys(&map);
  auto result = hook_SET_size(&set);
  BOOST_CHECK_EQUAL(mpz_cmp_ui(result, 1), 0);
  for (auto iter = set.begin(); iter != set.end(); ++iter) {
    block *b_ptr = (block *)iter->elem;
    range *r_ptr = (range *)(b_ptr->children[0]);
    BOOST_CHECK_EQUAL(r_ptr->start, RDUMMY0);
    BOOST_CHECK_EQUAL(r_ptr->end, RDUMMY1);
  }
}

BOOST_AUTO_TEST_CASE(rangemap_hook_keys_list) {
  auto map = hook_RANGEMAP_element(RDUMMY0, RDUMMY1, RDUMMY0);
  auto list = hook_RANGEMAP_keys_list(&map);
  auto result = hook_LIST_size(&list);
  BOOST_CHECK_EQUAL(mpz_cmp_ui(result, 1), 0);
  for (auto iter = list.begin(); iter != list.end(); ++iter) {
    block *b_ptr = (block *)iter->elem;
    range *r_ptr = (range *)(b_ptr->children[0]);
    BOOST_CHECK_EQUAL(r_ptr->start, RDUMMY0);
    BOOST_CHECK_EQUAL(r_ptr->end, RDUMMY1);
  }
}

BOOST_AUTO_TEST_CASE(rangemap_hook_in_keys) {
  auto map = hook_RANGEMAP_element(RDUMMY0, RDUMMY1, RDUMMY0);
  auto result = hook_RANGEMAP_in_keys(RDUMMY0, &map);
  BOOST_CHECK(result);
  result = hook_RANGEMAP_in_keys(RDUMMY1, &map);
  BOOST_CHECK(!result);
}

BOOST_AUTO_TEST_CASE(rangemap_hook_values) {
  auto map = hook_RANGEMAP_element(RDUMMY0, RDUMMY1, RDUMMY0);
  auto list = hook_RANGEMAP_values(&map);
  BOOST_CHECK_EQUAL(hook_LIST_get_long(&list, 0), RDUMMY0);
}

BOOST_AUTO_TEST_CASE(rangemap_hook_choice) {
  auto map = hook_RANGEMAP_element(RDUMMY0, RDUMMY1, RDUMMY0);
  auto result = hook_RANGEMAP_choice(&map);
  BOOST_CHECK_EQUAL(result, RDUMMY0);
}

BOOST_AUTO_TEST_CASE(rangemap_hook_choice_rng) {
  auto map = hook_RANGEMAP_element(RDUMMY0, RDUMMY1, RDUMMY0);
  auto result = hook_RANGEMAP_choiceRng(&map);
  BOOST_CHECK_EQUAL((block *)result->children[0], RDUMMY0);
  BOOST_CHECK_EQUAL((block *)result->children[1], RDUMMY1);
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

BOOST_AUTO_TEST_CASE(rangemap_hook_update_all) {
  auto map1 = hook_RANGEMAP_element(RDUMMY0, RDUMMY1, RDUMMY0);
  auto m1 = hook_RANGEMAP_element(RDUMMY0, RDUMMY2, RDUMMY0);
  auto map2 = hook_RANGEMAP_update(&m1, RDUMMY0, RDUMMY1, RDUMMY1);
  auto map = hook_RANGEMAP_updateAll(&map2, &map1);
  auto result = hook_RANGEMAP_lookup(&map, RDUMMY0);
  BOOST_CHECK_EQUAL(result, RDUMMY0);
  result = hook_RANGEMAP_lookup(&map, RDUMMY1);
  BOOST_CHECK_EQUAL(result, RDUMMY0);
  auto result2 = hook_RANGEMAP_find_range(&map, RDUMMY0);
  BOOST_CHECK_EQUAL((block *)result2->children[0], RDUMMY0);
  BOOST_CHECK_EQUAL((block *)result2->children[1], RDUMMY2);
}

BOOST_AUTO_TEST_CASE(rangemap_hook_remove_all) {
  auto map1 = hook_RANGEMAP_element(RDUMMY0, RDUMMY1, RDUMMY0);
  auto set = hook_SET_unit();
  auto map2 = hook_RANGEMAP_removeAll(&map1, &set);
  auto result = hook_RANGEMAP_size_long(&map2);
  BOOST_CHECK_EQUAL(result, 1);
  range *ptr = (range *)koreAlloc(sizeof(range));
  ptr->h = range_header();
  ptr->start = RDUMMY0;
  ptr->end = RDUMMY1;
  inj_range2kitem *inj_ptr
      = (inj_range2kitem *)koreAlloc(sizeof(inj_range2kitem));
  inj_ptr->h = inj_range2kitem_header();
  inj_ptr->child = ptr;
  auto elem = hook_SET_element((SortKItem)inj_ptr);
  map2 = hook_RANGEMAP_removeAll(&map1, &elem);
  result = hook_RANGEMAP_size_long(&map2);
  BOOST_CHECK_EQUAL(result, 0);
}

BOOST_AUTO_TEST_CASE(rangemap_hook_eq) {
  auto map1 = hook_RANGEMAP_element(RDUMMY0, RDUMMY1, RDUMMY0);
  auto map2 = hook_RANGEMAP_element(RDUMMY0, RDUMMY2, RDUMMY0);
  auto map3 = hook_RANGEMAP_element(RDUMMY1, RDUMMY2, RDUMMY1);
  auto map4 = hook_RANGEMAP_element(RDUMMY0, RDUMMY1, RDUMMY1);
  auto result = hook_RANGEMAP_eq(&map1, &map2);
  BOOST_CHECK(!result);
  result = hook_RANGEMAP_eq(&map1, &map3);
  BOOST_CHECK(!result);
  result = hook_RANGEMAP_eq(&map1, &map4);
  BOOST_CHECK(!result);
  map2 = hook_RANGEMAP_element(RDUMMY0, RDUMMY1, RDUMMY0);
  result = hook_RANGEMAP_eq(&map1, &map2);
  BOOST_CHECK(result);
}

BOOST_AUTO_TEST_SUITE_END()
