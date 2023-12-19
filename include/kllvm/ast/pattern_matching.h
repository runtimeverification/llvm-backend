#ifndef AST_PATTERN_MATCHING_H
#define AST_PATTERN_MATCHING_H

#include <kllvm/ast/AST.h>

#include <cassert>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

namespace kllvm::pattern_matching {

template <typename... Ts>
class pattern;

template <typename Inner>
class subject;

class any_;

namespace detail {

template <typename Tuple, typename Func, std::size_t... Idxs>
void for_each_impl(Tuple &&t, Func &&f, std::index_sequence<Idxs...> idxs) {
  (std::forward<Func>(f)(Idxs, std::get<Idxs>(std::forward<Tuple>(t))), ...);
}

template <typename Tuple, typename Func>
void for_each(Tuple &&t, Func &&f) {
  constexpr auto size = std::tuple_size_v<std::decay_t<Tuple>>;
  for_each_impl(
      std::forward<Tuple>(t), std::forward<Func>(f),
      std::make_index_sequence<size>{});
}

template <typename T>
struct subject_count;

template <>
struct subject_count<any_> {
  constexpr static size_t value = 0;
};

template <typename Inner>
struct subject_count<subject<Inner>> {
  constexpr static size_t value = 1 + subject_count<Inner>::value;
};

template <typename... Ts>
struct subject_count<pattern<Ts...>> {
  constexpr static size_t value = (subject_count<Ts>::value + ... + 0);
};

template <typename T>
constexpr inline size_t subject_count_v = subject_count<T>::value;

struct pattern_forwarder {
  std::string constructor;

  template <typename... Args>
  pattern<Args...> operator()(Args... args) const {
    return pattern(constructor, args...);
  }
};

} // namespace detail

struct match_result {
  bool matches;
  std::shared_ptr<KOREPattern> subject;
};

class any_ {
public:
  match_result match(std::shared_ptr<KOREPattern> const &term) const {
    return {true, nullptr};
  }
};

constexpr inline any_ any;

template <typename Inner>
class subject {
public:
  subject(Inner inner)
      : inner_(inner) {
    static_assert(
        detail::subject_count_v<Inner> == 0,
        "Cannot construct a subject expression with nested subject terms");
  }

  match_result match(std::shared_ptr<KOREPattern> const &term) const {
    auto inner_result = inner_.match(term);

    if (inner_result.matches) {
      return {true, term};
    } else {
      return {false, nullptr};
    }
  }

private:
  Inner inner_;
};

template <typename... Ts>
class pattern {
public:
  pattern(std::string ctor, Ts... children)
      : constructor_(ctor)
      , children_(children...) {
    static_assert(
        detail::subject_count_v<pattern<Ts...>> <= 1,
        "Cannot construct a pattern with multiple nested subject terms");
  }

  match_result match(std::shared_ptr<KOREPattern> const &term) const {
    if (auto composite = std::dynamic_pointer_cast<KORECompositePattern>(term);
        composite->getArguments().size() == arity()
        && composite->getConstructor()->getName() == constructor_) {
      auto results = std::vector<match_result>{};

      detail::for_each(children_, [&](auto idx, auto const &subpattern) {
        results.push_back(subpattern.match(composite->getArguments()[idx]));
      });

      auto all_match
          = std::all_of(results.begin(), results.end(), [](auto const &res) {
              return res.matches;
            });

      if (all_match) {
        auto subject_result
            = std::find_if(results.begin(), results.end(), [](auto const &res) {
                return res.subject;
              });

        return {
            true, subject_result != results.end() ? subject_result->subject
                                                  : nullptr};
      }
    }

    return {false, nullptr};
  }

private:
  constexpr size_t arity() const { return sizeof...(Ts); }

  std::string constructor_;
  std::tuple<Ts...> children_;
};

template <typename Pattern, typename Result>
class matcher {
public:
  template <typename Func>
  matcher(Pattern p, Func f)
      : pattern_(p)
      , func_(f) { }

  matcher(Pattern p)
      : matcher(p, [](auto const &p) { return p; }) { }

  Result match(std::shared_ptr<KOREPattern> const &term) {
    auto [match, subject] = pattern_.match(term);

    if (!match || !subject) {
      return std::nullopt;
    }

    return func_(subject);
  }

private:
  Pattern pattern_;
  std::function<Result(std::shared_ptr<KOREPattern> const &)> func_;
};

template <typename Pattern, typename Func>
matcher(Pattern, Func) -> matcher<
    Pattern, std::invoke_result_t<Func, std::shared_ptr<KOREPattern> const &>>;

template <typename Pattern>
matcher(Pattern) -> matcher<Pattern, std::shared_ptr<KOREPattern> const &>;

namespace literals {

detail::pattern_forwarder operator""_p(const char *str, std::size_t data) {
  return {std::string(str, data)};
}

} // namespace literals

} // namespace kllvm::pattern_matching

#endif
