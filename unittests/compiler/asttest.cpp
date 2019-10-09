#include <boost/test/unit_test.hpp>

#include "kllvm/ast/AST.h"

using namespace kllvm;

BOOST_AUTO_TEST_SUITE(ASTTest)

BOOST_AUTO_TEST_CASE(substitute) {
  KORESort::substitution subst;
  auto var = KORESortVariable::Create("foo");
  auto composite = KORECompositeSort::Create("bar");
  auto poly = KORECompositeSort::Create("baz");
  poly->addArgument(var);
  subst[*var] = composite;
  BOOST_CHECK_EQUAL(var->substitute(subst), composite);
  BOOST_CHECK_EQUAL(composite->substitute(subst), composite);
  auto expected = KORECompositeSort::Create("baz");
  expected->addArgument(composite);
  BOOST_CHECK_EQUAL(*poly->substitute(subst), *expected);
}

BOOST_AUTO_TEST_CASE(instantiate) {
  auto decl = KORESymbolDeclaration::Create("sym");
  auto var = KORESortVariable::Create("foo");
  decl->addObjectSortVariable(var);
  auto poly = KORECompositeSort::Create("baz");
  auto composite = KORECompositeSort::Create("bar");
  poly->addArgument(var);
  decl->getSymbol()->addArgument(poly);
  decl->getSymbol()->addArgument(composite);
  decl->getSymbol()->addSort(poly);
  auto sym = KORESymbol::Create("sym");
  sym->addFormalArgument(composite);
  sym->instantiateSymbol(decl.get());
  auto expected = KORECompositeSort::Create("baz");
  expected->addArgument(composite);
  BOOST_CHECK_EQUAL(*sym->getSort(), *expected);
  BOOST_CHECK_EQUAL(sym->getArguments().size(), 2);
  BOOST_CHECK_EQUAL(*sym->getArguments()[0], *expected);
  BOOST_CHECK_EQUAL(*sym->getArguments()[1], *composite);
}

BOOST_AUTO_TEST_SUITE_END()
