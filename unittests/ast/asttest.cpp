#include <boost/test/unit_test.hpp>

#include "kllvm/ast/AST.h"

using namespace kllvm;

BOOST_AUTO_TEST_SUITE(ASTTest)

BOOST_AUTO_TEST_CASE(substitute) {
  std::unordered_map<KOREObjectSortVariable, KOREObjectSort *, HashSort> subst;
  auto var = KOREObjectSortVariable::Create("foo");
  auto composite = KOREObjectCompositeSort::Create("bar");
  auto poly = KOREObjectCompositeSort::Create("baz");
  poly->addArgument(var);
  subst[*var] = composite;
  BOOST_CHECK_EQUAL(var->substitute(subst), composite);
  BOOST_CHECK_EQUAL(composite->substitute(subst), composite);
  auto expected = KOREObjectCompositeSort::Create("baz");
  expected->addArgument(composite);
  BOOST_CHECK_EQUAL(*poly->substitute(subst), *expected);
}

BOOST_AUTO_TEST_SUITE_END()
