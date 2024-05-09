#include <boost/test/unit_test.hpp>
#include <kllvm/ast/AST.h>

using namespace kllvm;

BOOST_AUTO_TEST_SUITE(SubsortMapTest)

BOOST_AUTO_TEST_CASE(insert_contains) {
  SortSet sortSet;
  SymbolSet symbolSet;
  auto subsortMap = SubsortMap{};

  auto sort = kore_composite_sort::create("Sort");
  auto subsort1 = kore_composite_sort::create("Subsort");
  auto subsort2 = kore_composite_sort::create("Subsort");

  subsortMap[sort.get()].insert(subsort1.get());
  subsortMap[sort.get()].insert(subsort2.get());

  BOOST_CHECK(subsortMap[sort.get()].contains(subsort1.get()));
  BOOST_CHECK(subsortMap[sort.get()].contains(subsort2.get()));
  BOOST_CHECK(subsortMap[sort.get()].size() == 1); // subsort1 == subsort2
  BOOST_CHECK(subsortMap.size() == 1);
  BOOST_CHECK(!subsortMap[sort.get()].contains(sort.get()));
  BOOST_CHECK(!subsortMap[sort.get()].empty());
}

BOOST_AUTO_TEST_CASE(subsorts_supersorts) {
  auto subsort1 = kore_composite_sort::create("bar");
  auto subsort2 = kore_composite_sort::create("baz");

  auto pat
      = sptr<kore_composite_pattern>(kore_composite_pattern::create("subsort"));
  pat->get_constructor()->add_formal_argument(subsort1);
  pat->get_constructor()->add_formal_argument(subsort2);

  sptr<kore_axiom_declaration> decl = kore_axiom_declaration::create();
  decl->attributes().add(pat);

  auto mod = sptr<kore_module>(kore_module::create("FooModule"));
  mod->add_declaration(decl);

  auto def = kore_definition::create();
  def->add_module(mod);

  auto subsorts_ = def->get_subsorts();
  auto supersorts_ = def->get_supersorts();

  // Check bar <: baz
  // Check bar !<: bar
  // Check baz !<: baz
  // Check baz !<: bar
  BOOST_CHECK(subsorts_[subsort1.get()].contains(subsort2.get()));
  BOOST_CHECK(!subsorts_[subsort1.get()].contains(subsort1.get()));
  BOOST_CHECK(!subsorts_[subsort2.get()].contains(subsort2.get()));
  BOOST_CHECK(!subsorts_[subsort2.get()].contains(subsort1.get()));

  // Check baz :> bar
  // Check bar !:> baz
  // Check bar !:> bar
  // Check baz !:> baz
  BOOST_CHECK(supersorts_[subsort2.get()].contains(subsort1.get()));
  BOOST_CHECK(!supersorts_[subsort1.get()].contains(subsort2.get()));
  BOOST_CHECK(!supersorts_[subsort1.get()].contains(subsort1.get()));
  BOOST_CHECK(!supersorts_[subsort2.get()].contains(subsort2.get()));
}

BOOST_AUTO_TEST_CASE(transitive_subsorts_supersorts) {
  auto subsort1 = kore_composite_sort::create("foo");
  auto subsort2 = kore_composite_sort::create("bar");
  auto subsort3 = kore_composite_sort::create("baz");

  auto pat1
      = sptr<kore_composite_pattern>(kore_composite_pattern::create("subsort"));
  pat1->get_constructor()->add_formal_argument(subsort1);
  pat1->get_constructor()->add_formal_argument(subsort2);

  auto pat2
      = sptr<kore_composite_pattern>(kore_composite_pattern::create("subsort"));
  pat2->get_constructor()->add_formal_argument(subsort2);
  pat2->get_constructor()->add_formal_argument(subsort3);

  sptr<kore_axiom_declaration> decl1 = kore_axiom_declaration::create();
  decl1->attributes().add(pat1);

  sptr<kore_axiom_declaration> decl2 = kore_axiom_declaration::create();
  decl2->attributes().add(pat2);

  auto mod = sptr<kore_module>(kore_module::create("FooModule"));
  mod->add_declaration(decl1);
  mod->add_declaration(decl2);

  auto def = kore_definition::create();
  def->add_module(mod);

  auto subsorts_ = def->get_subsorts();
  auto supersorts_ = def->get_supersorts();

  // Check foo <: bar
  // Check bar <: baz
  // Check foo <: baz
  BOOST_CHECK(subsorts_[subsort1.get()].contains(subsort2.get()));
  BOOST_CHECK(subsorts_[subsort2.get()].contains(subsort3.get()));
  BOOST_CHECK(subsorts_[subsort1.get()].contains(subsort3.get()));

  // Check baz :> bar
  // Check bar :> foo
  // Check baz :> foo
  BOOST_CHECK(supersorts_[subsort2.get()].contains(subsort1.get()));
  BOOST_CHECK(supersorts_[subsort3.get()].contains(subsort2.get()));
  BOOST_CHECK(supersorts_[subsort3.get()].contains(subsort1.get()));
}

BOOST_AUTO_TEST_SUITE_END()