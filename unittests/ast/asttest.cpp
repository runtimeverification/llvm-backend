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

BOOST_AUTO_TEST_CASE(instantiate) {
  auto decl = KOREObjectSymbolDeclaration::Create("sym");
  auto var = KOREObjectSortVariable::Create("foo");
  decl->addObjectSortVariable(var);
  auto poly = KOREObjectCompositeSort::Create("baz");
  auto composite = KOREObjectCompositeSort::Create("bar");
  poly->addArgument(var);
  decl->getSymbol()->addArgument(poly);
  decl->getSymbol()->addArgument(composite);
  decl->getSymbol()->addSort(poly);
  auto sym = KOREObjectSymbol::Create("sym");
  sym->addArgument(composite);
  sym->instantiateSymbol(decl);
  auto expected = KOREObjectCompositeSort::Create("baz");
  expected->addArgument(composite);
  BOOST_CHECK_EQUAL(*sym->getSort(), *expected);
  BOOST_CHECK_EQUAL(sym->getArguments().size(), 2);
  BOOST_CHECK_EQUAL(*sym->getArguments()[0], *expected);
  BOOST_CHECK_EQUAL(*sym->getArguments()[1], *composite);
}

BOOST_AUTO_TEST_SUITE_END()
