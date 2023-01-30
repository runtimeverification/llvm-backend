#include <boost/test/unit_test.hpp>

#include <runtime/collections/RBTree.h>

#include <cstdlib>
#include <ctime>
#include <iostream>

void hugeTest(std::vector<int> &v) {
  std::vector<RBTree<int, int>> trees;
  trees.push_back(RBTree<int, int>());
  for (int i : v) {
    trees.push_back(trees.back().inserted(i, i + 1));
    trees.push_back(trees.back().inserted(i, i + 2));
  }
  for (int i : v) {
    trees.push_back(trees.back().deleted(i));
  }
  for (int i = 0; i < v.size(); i++) {
    int idxI1 = 1 + 2 * i;
    auto result1 = trees[idxI1].contains(v[i]);
    BOOST_CHECK_EQUAL(result1, true);
    auto result2 = trees[idxI1].lookup(v[i]);
    BOOST_CHECK_EQUAL(result2, v[i] + 1);
    trees[idxI1].assert_red_invariant();
    trees[idxI1].assert_black_invariant();
    trees[idxI1].assert_BST_invariant();
    int idxI2 = 1 + 2 * i + 1;
    result1 = trees[idxI2].contains(v[i]);
    BOOST_CHECK_EQUAL(result1, true);
    result2 = trees[idxI2].lookup(v[i]);
    BOOST_CHECK_EQUAL(result2, v[i] + 2);
    trees[idxI2].assert_red_invariant();
    trees[idxI2].assert_black_invariant();
    trees[idxI2].assert_BST_invariant();
    int idxD = 1 + 2 * v.size() + i;
    result1 = trees[idxD].contains(v[i]);
    BOOST_CHECK_EQUAL(result1, false);
    trees[idxD].assert_red_invariant();
    trees[idxD].assert_black_invariant();
    trees[idxD].assert_BST_invariant();
  }
  auto result = trees[0].empty();
  BOOST_CHECK_EQUAL(result, true);
  trees[0].assert_red_invariant();
  trees[0].assert_black_invariant();
  trees[0].assert_BST_invariant();
}

void makeTestVectors(
    int n, std::vector<int> &asc, std::vector<int> &desc,
    std::vector<int> &rnd) {
  asc.clear();
  desc.clear();
  rnd.clear();
  srand(time(NULL));
  for (int i = 0; i < n; i++) {
    asc.push_back(i);
    desc.push_back(n - i - 1);
    rnd.push_back(rand());
  }
}

BOOST_AUTO_TEST_SUITE(TreeMapTest)

BOOST_AUTO_TEST_CASE(hugetest) {
  std::vector<int> a, d, r;
  makeTestVectors(500, a, d, r);
  hugeTest(a);
  hugeTest(d);
  hugeTest(r);
}

BOOST_AUTO_TEST_CASE(treemap_test_element) {
  auto map = (RBTree<int, int>()).inserted(0, 0);
  auto result = map.lookup(0);
  BOOST_CHECK_EQUAL(result, 0);
}

BOOST_AUTO_TEST_CASE(treemap_test_size) {
  auto map = RBTree<int, int>();
  auto result = map.size();
  BOOST_CHECK_EQUAL(result, 0);
  auto map2 = map.inserted(0, 1);
  result = map.size();
  BOOST_CHECK_EQUAL(result, 0);
  result = map2.size();
  BOOST_CHECK_EQUAL(result, 1);
}

BOOST_AUTO_TEST_CASE(treemap_test_concat_success) {
  auto m1 = (RBTree<int, int>()).inserted(0, 1);
  auto m2 = (RBTree<int, int>()).inserted(1, 2);
  auto map = m1.concat(m2);
  auto result = map.size();
  BOOST_CHECK_EQUAL(result, 2);
  result = map.contains(0);
  BOOST_CHECK_EQUAL(result, true);
  result = map.contains(1);
  BOOST_CHECK_EQUAL(result, true);
  result = map.lookup(0);
  BOOST_CHECK_EQUAL(result, 1);
  result = map.lookup(1);
  BOOST_CHECK_EQUAL(result, 2);
}

BOOST_AUTO_TEST_CASE(treemap_test_concat_failure) {
  auto m1 = (RBTree<int, int>()).inserted(0, 1);
  auto m2 = (RBTree<int, int>()).inserted(0, 2);
  BOOST_CHECK_THROW(m1.concat(m2), std::invalid_argument);
}

BOOST_AUTO_TEST_CASE(treemap_test_lookup) {
  auto map = (RBTree<int, int>()).inserted(0, 1);
  auto result = map.lookup(0);
  BOOST_CHECK_EQUAL(result, 1);
  BOOST_CHECK_THROW(map.lookup(2), std::invalid_argument);
}

BOOST_AUTO_TEST_CASE(treemap_test_contains) {
  auto map = (RBTree<int, int>()).inserted(0, 1);
  auto result = map.contains(0);
  BOOST_CHECK_EQUAL(result, true);
  result = map.contains(1);
  BOOST_CHECK_EQUAL(result, false);
}

BOOST_AUTO_TEST_CASE(treemap_test_inserted) {
  auto map = (RBTree<int, int>());
  auto result = map.size();
  BOOST_CHECK_EQUAL(result, 0);
  auto map2 = map.inserted(0, 0);
  result = map.size();
  BOOST_CHECK_EQUAL(result, 0);
  result = map2.size();
  BOOST_CHECK_EQUAL(result, 1);
}

BOOST_AUTO_TEST_CASE(treemap_test_update) {
  auto map = (RBTree<int, int>()).inserted(0, 0);
  auto result = map.lookup(0);
  BOOST_CHECK_EQUAL(result, 0);
  auto map2 = map.inserted(0, 1);
  result = map.lookup(0);
  BOOST_CHECK_EQUAL(result, 0);
  result = map2.lookup(0);
  BOOST_CHECK_EQUAL(result, 1);
}

BOOST_AUTO_TEST_CASE(treemap_test_remove) {
  auto map = (RBTree<int, int>()).inserted(0, 0);
  auto map2 = map.deleted(0);
  auto result = map.size();
  BOOST_CHECK_EQUAL(result, 1);
  result = map2.size();
  BOOST_CHECK_EQUAL(result, 0);
}

BOOST_AUTO_TEST_CASE(treemap_test_inserted_multiple) {
  auto map = (RBTree<int, int>()).inserted(0, 0);
  auto results = map.size();
  BOOST_CHECK_EQUAL(results, 1);
  auto resultv = map.lookup(0);
  BOOST_CHECK_EQUAL(resultv, 0);
  auto v = {std::pair<int, int>(1, 1), std::pair<int, int>(2, 2)};
  auto map2 = inserted(map, v.begin(), v.end());
  results = map.size();
  BOOST_CHECK_EQUAL(results, 1);
  results = map2.size();
  BOOST_CHECK_EQUAL(results, 3);
  auto u = {std::pair<int, int>(1, 1), std::pair<int, int>(1, 2)};
  auto map3 = inserted(map, u.begin(), u.end());
  results = map3.size();
  BOOST_CHECK_EQUAL(results, 2);
  resultv = map3.lookup(1);
  BOOST_CHECK_EQUAL(resultv, 2);
  auto w = {std::pair<int, int>(0, 1)};
  auto map4 = inserted(map, w.begin(), w.end());
  results = map4.size();
  BOOST_CHECK_EQUAL(results, 1);
  resultv = map4.lookup(0);
  BOOST_CHECK_EQUAL(resultv, 1);
}

BOOST_AUTO_TEST_SUITE_END()
