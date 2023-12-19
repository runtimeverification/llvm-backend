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

BOOST_AUTO_TEST_SUITE_END()
