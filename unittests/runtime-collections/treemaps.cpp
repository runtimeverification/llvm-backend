#include <boost/test/unit_test.hpp>

#include "../../runtime/collections/RBTree.h"

#include <iostream>
#include <cstdlib>
#include <ctime>

void hugeTest(std::vector<int> &v)
{
    std::vector<RBTree<int, int> > trees;
    trees.push_back(RBTree<int, int>());
    for (int i: v)
    {
        trees.push_back(trees.back().inserted(i, i+1));
        trees.push_back(trees.back().inserted(i, i+2));
    }
    for (int i: v)
    {
        trees.push_back(trees.back().deleted(i));
    }
    for (int i = 0; i < v.size(); i++ )
    {
        int idxI1 = 1 + 2*i;
        auto result1 = trees[idxI1].member(v[i]);
        BOOST_CHECK_EQUAL(result1, true);
        auto result2 = trees[idxI1].lookup(v[i]);
        BOOST_CHECK_EQUAL(result2, v[i]+1);
        trees[idxI1].assert1();
        trees[idxI1].countB();
        trees[idxI1].assertBST();
        int idxI2 = 1 + 2*i+1;
        result1 = trees[idxI2].member(v[i]);
        BOOST_CHECK_EQUAL(result1, true);
        result2 = trees[idxI2].lookup(v[i]);
        BOOST_CHECK_EQUAL(result2, v[i]+2);
        trees[idxI2].assert1();
        trees[idxI2].countB();
        trees[idxI2].assertBST();
        int idxD = 1 + 2*v.size() + i;
        result1 = trees[idxD].member(v[i]);
        BOOST_CHECK_EQUAL(result1, false);
        trees[idxD].assert1();
        trees[idxD].countB();
        trees[idxD].assertBST();
    }
    auto result = trees[0].isEmpty();
    BOOST_CHECK_EQUAL(result, true);
    trees[0].assert1();
    trees[0].countB();
    trees[0].assertBST();
}

void makeTestVectors(int n, std::vector<int> &asc, std::vector<int> &desc, std::vector<int> &rnd)
{
    asc.clear(); desc.clear(); rnd.clear();
    srand(time(NULL));
    for (int i = 0; i < n; i++)
    {
        asc.push_back(i);
        desc.push_back(n-i-1);
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

BOOST_AUTO_TEST_CASE(element) {
  auto map = (RBTree<int, int>()).inserted(0, 0);
  auto result = map.lookup(0);
  BOOST_CHECK_EQUAL(result, 0);
}

BOOST_AUTO_TEST_CASE(size) {
  auto map = RBTree<int, int>();
  auto result = map.size();
  BOOST_CHECK_EQUAL(result, 0);
  auto map2 = map.inserted(0, 1);
  result = map.size();
  BOOST_CHECK_EQUAL(result, 0);
  result = map2.size();
  BOOST_CHECK_EQUAL(result, 1);
}

BOOST_AUTO_TEST_CASE(concat_success) {
  auto m1 = (RBTree<int, int>()).inserted(0, 1);
  auto m2 = (RBTree<int, int>()).inserted(1, 2);
  auto map = mapConcat(m1, m2);
  auto result = map.size();
  BOOST_CHECK_EQUAL(result, 2);
  result = map.member(0);
  BOOST_CHECK_EQUAL(result, true);
  result = map.member(1);
  BOOST_CHECK_EQUAL(result, true);
  result = map.lookup(0);
  BOOST_CHECK_EQUAL(result, 1);
  result = map.lookup(1);
  BOOST_CHECK_EQUAL(result, 2);
}

BOOST_AUTO_TEST_CASE(concat_failure) {
  auto m1 = (RBTree<int, int>()).inserted(0, 1);
  auto m2 = (RBTree<int, int>()).inserted(0, 2);
  BOOST_CHECK_THROW(mapConcat(m1, m2), std::invalid_argument);
}

BOOST_AUTO_TEST_CASE(lookup) {
  auto map = (RBTree<int, int>()).inserted(0, 1);
  auto result = map.lookup(0);
  BOOST_CHECK_EQUAL(result, 1);
  BOOST_CHECK_THROW(map.lookup(2), std::invalid_argument);
}

BOOST_AUTO_TEST_CASE(member) {
  auto map = (RBTree<int, int>()).inserted(0, 1);
  auto result = map.member(0);
  BOOST_CHECK_EQUAL(result, true);
  result = map.member(1);
  BOOST_CHECK_EQUAL(result, false);
}

BOOST_AUTO_TEST_CASE(update) {
  auto map = (RBTree<int, int>()).inserted(0, 0);
  auto result = map.lookup(0);
  BOOST_CHECK_EQUAL(result, 0);
  auto map2 = map.inserted(0, 1);
  result = map.lookup(0);
  BOOST_CHECK_EQUAL(result, 0);
  result = map2.lookup(0);
  BOOST_CHECK_EQUAL(result, 1);
}

BOOST_AUTO_TEST_CASE(remove) {
  auto map = (RBTree<int, int>()).inserted(0, 0);
  auto map2 = map.deleted(0);
  auto result = map.size();
  BOOST_CHECK_EQUAL(result, 1);
  result = map2.size();
  BOOST_CHECK_EQUAL(result, 0);
}

BOOST_AUTO_TEST_SUITE_END()
