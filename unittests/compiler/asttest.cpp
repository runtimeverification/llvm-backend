#include <boost/test/unit_test.hpp>

#include "kllvm/ast/AST.h"

using namespace kllvm;

BOOST_AUTO_TEST_SUITE(ASTTest)

BOOST_AUTO_TEST_CASE(substitute) {
  kore_sort::substitution subst;
  auto var = kore_sort_variable::create("foo");
  auto composite = kore_composite_sort::create("bar");
  auto poly = kore_composite_sort::create("baz");
  poly->add_argument(var);
  subst[*var] = composite;
  BOOST_CHECK_EQUAL(var->substitute(subst), composite);
  BOOST_CHECK_EQUAL(composite->substitute(subst), composite);
  auto expected = kore_composite_sort::create("baz");
  expected->add_argument(composite);
  BOOST_CHECK_EQUAL(*poly->substitute(subst), *expected);
}

BOOST_AUTO_TEST_CASE(instantiate) {
  auto decl = kore_symbol_declaration::create("sym");
  auto var = kore_sort_variable::create("foo");
  decl->add_object_sort_variable(var);
  auto poly = kore_composite_sort::create("baz");
  auto composite = kore_composite_sort::create("bar");
  poly->add_argument(var);
  decl->get_symbol()->add_argument(poly);
  decl->get_symbol()->add_argument(composite);
  decl->get_symbol()->add_sort(poly);
  auto sym = kore_symbol::create("sym");
  sym->add_formal_argument(composite);
  sym->instantiate_symbol(decl.get());
  auto expected = kore_composite_sort::create("baz");
  expected->add_argument(composite);
  BOOST_CHECK_EQUAL(*sym->get_sort(), *expected);
  BOOST_CHECK_EQUAL(sym->get_arguments().size(), 2);
  BOOST_CHECK_EQUAL(*sym->get_arguments()[0], *expected);
  BOOST_CHECK_EQUAL(*sym->get_arguments()[1], *composite);
}

BOOST_AUTO_TEST_SUITE_END()
