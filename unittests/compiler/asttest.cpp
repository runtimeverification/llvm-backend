#include <boost/test/unit_test.hpp>

#include "kllvm/ast/AST.h"

using namespace kllvm;

BOOST_AUTO_TEST_SUITE(ASTTest)

BOOST_AUTO_TEST_CASE(substitute) {
  kore_sort::substitution subst;
  auto var = kore_sort_variable::Create("foo");
  auto composite = kore_composite_sort::Create("bar");
  auto poly = kore_composite_sort::Create("baz");
  poly->addArgument(var);
  subst[*var] = composite;
  BOOST_CHECK_EQUAL(var->substitute(subst), composite);
  BOOST_CHECK_EQUAL(composite->substitute(subst), composite);
  auto expected = kore_composite_sort::Create("baz");
  expected->addArgument(composite);
  BOOST_CHECK_EQUAL(*poly->substitute(subst), *expected);
}

BOOST_AUTO_TEST_CASE(instantiate) {
  auto decl = kore_symbol_declaration::Create("sym");
  auto var = kore_sort_variable::Create("foo");
  decl->addObjectSortVariable(var);
  auto poly = kore_composite_sort::Create("baz");
  auto composite = kore_composite_sort::Create("bar");
  poly->addArgument(var);
  decl->getSymbol()->addArgument(poly);
  decl->getSymbol()->addArgument(composite);
  decl->getSymbol()->addSort(poly);
  auto sym = kore_symbol::Create("sym");
  sym->addFormalArgument(composite);
  sym->instantiateSymbol(decl.get());
  auto expected = kore_composite_sort::Create("baz");
  expected->addArgument(composite);
  BOOST_CHECK_EQUAL(*sym->getSort(), *expected);
  BOOST_CHECK_EQUAL(sym->getArguments().size(), 2);
  BOOST_CHECK_EQUAL(*sym->getArguments()[0], *expected);
  BOOST_CHECK_EQUAL(*sym->getArguments()[1], *composite);
}

BOOST_AUTO_TEST_SUITE_END()
