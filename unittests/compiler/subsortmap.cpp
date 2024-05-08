#include <boost/test/unit_test.hpp>
#include <kllvm/ast/AST.h>

using namespace kllvm;

BOOST_AUTO_TEST_SUITE(SubsortMapTest)

BOOST_AUTO_TEST_CASE(insert_contains) {
  SortSet sortSet;
  SymbolSet symbolSet;
  auto subsortMap = SubsortMap{};

  auto sort = kore_sort_variable::create("Sort");
  auto subsort = kore_sort_variable::create("Subsort 1");
  auto subsort2 = kore_sort_variable::create("Subsort 2");

  subsortMap[sort.get()].insert(subsort.get());
  subsortMap[sort.get()].insert(subsort2.get());


  BOOST_CHECK(subsortMap[sort.get()].contains(subsort.get()));
  BOOST_CHECK(subsortMap[sort.get()].contains(subsort2.get()));
  BOOST_CHECK(subsortMap[sort.get()].size() == 2);
  BOOST_CHECK(subsortMap.size() == 1);
  BOOST_CHECK(!subsortMap[sort.get()].contains(sort.get()));
  BOOST_CHECK(!subsortMap[sort.get()].empty());
}

BOOST_AUTO_TEST_CASE(subsorts_supersorts) {
  auto subsort1 = kore_sort_variable::create("bar");
  auto subsort2 = kore_sort_variable::create("baz");

  auto pat = sptr<kore_composite_pattern>(kore_composite_pattern::create("subsort"));
  pat->get_constructor()->add_formal_argument(subsort1);
  pat->get_constructor()->add_formal_argument(subsort2);

  sptr<kore_axiom_declaration> decl = kore_axiom_declaration::create();
  decl->attributes().add(pat);

  auto mod = sptr<kore_module>(kore_module::create("FooModule"));
  mod->add_declaration(decl);

  auto def = kore_definition::create();
  def->add_module(mod);

  auto subsorts_ = def->get_subsorts();
  BOOST_CHECK(subsorts_[subsort1.get()].contains(subsort2.get()));
}

BOOST_AUTO_TEST_SUITE_END()