#include <kllvm/ast/pattern_matching.h>

#include <boost/test/unit_test.hpp>

using namespace kllvm;
using namespace kllvm::pattern_matching;

template <typename... Args>
std::shared_ptr<KORECompositePattern>
term(std::string const &s, Args &&...args) {
  std::shared_ptr<KORECompositePattern> ret = KORECompositePattern::Create(s);
  (ret->addArgument(std::forward<Args>(args)), ...);
  return ret;
}

BOOST_AUTO_TEST_SUITE(PatternMatching)

BOOST_AUTO_TEST_CASE(empty) {
  auto foo = term("foo");
  auto [foo_m, foo_sub] = pattern("foo").match(foo);
  auto [bar_m, bar_sub] = pattern("bar").match(foo);

  BOOST_CHECK(foo_m);
  BOOST_CHECK(!bar_m);
}

BOOST_AUTO_TEST_CASE(any_match) {
  auto foo = term("foo");
  auto bar = term("bar");
  auto big = term("baz", term("a", term("a1"), term("a2")), term("b"));

  for (auto const &t : {foo, bar, big}) {
    auto [match, sub] = any.match(t);
    BOOST_CHECK(match);
  }
}

BOOST_AUTO_TEST_CASE(extract_subject_simple) {
  auto foo = term("foo");
  auto [match, sub] = subject(pattern("foo")).match(foo);
  BOOST_CHECK(match);
  BOOST_CHECK_EQUAL(ast_to_string(*foo), ast_to_string(*sub));
}

BOOST_AUTO_TEST_CASE(extract_subject_deeper) {
  auto bar = term("bar");
  auto baz = term("baz");

  // foo(a1(), a2(bar()), a3(b1(baz())))
  auto foo
      = term("foo", term("a1"), term("a2", bar), term("a3", term("b1", baz)));

  // foo(_, a2(X), _)
  auto [bar_m, bar_sub]
      = pattern("foo", any, pattern("a2", subject(any)), any).match(foo);
  BOOST_CHECK(bar_m);
  BOOST_CHECK_EQUAL(ast_to_string(*bar), ast_to_string(*bar_sub));

  // foo(_, _, a3(b1(X)))
  auto [baz_m, baz_sub]
      = pattern("foo", any, any, pattern("a3", pattern("b1", subject(any))))
            .match(foo);
  BOOST_CHECK(baz_m);
  BOOST_CHECK_EQUAL(ast_to_string(*baz), ast_to_string(*baz_sub));
}

BOOST_AUTO_TEST_CASE(literals) {
  using namespace kllvm::pattern_matching::literals;

  auto bar = term("bar");
  auto baz = term("baz");

  // foo(a1(), a2(bar()), a3(b1(baz())))
  auto foo
      = term("foo", term("a1"), term("a2", bar), term("a3", term("b1", baz)));

  // foo(_, a2(X), _)
  auto [bar_m, bar_sub] = "foo"_p(any, "a2"_p(subject(any)), any).match(foo);
  BOOST_CHECK(bar_m);
  BOOST_CHECK_EQUAL(ast_to_string(*bar), ast_to_string(*bar_sub));

  // foo(_, _, a3(b1(X)))
  auto [baz_m, baz_sub]
      = "foo"_p(any, any, "a3"_p("b1"_p(subject(any)))).match(foo);
  BOOST_CHECK(baz_m);
  BOOST_CHECK_EQUAL(ast_to_string(*baz), ast_to_string(*baz_sub));
}

BOOST_AUTO_TEST_SUITE_END()
