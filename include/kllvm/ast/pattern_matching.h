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

/**
 * This file contains an implementation of a template metaprogramming-based
 * library for concise, readable pattern-matching over KORE AST patterns. Rather
 * than writing a long chain of nested if-else statements to decompose a deeply
 * nested pattern structure, this library offers a declarative mini-DSL to zoom
 * in on the relevant parts of a pattern and extract sub-patterns easily.
 *
 * For example, consider the abstract pattern-matching problem below:
 *
 *   a(_, _, b(_, c(X))) => X
 *
 * Previously, doing this would require a deep chain of casts, constructor
 * comparisons and arity checks to make sure that the correct information was
 * being extracted. See [1] for an example of what analogous code looks like in
 * practice.
 *
 * Using the DSL in this file, this matching problem can be written as:
 *
 *   "a"_p(any, any, "b"_p(any, "c"_p(subject(any)))).match(kore_pattern)
 *
 * While noisier syntactically than the abstract specification, the _structure_
 * of this code precisely models the specification's intent.
 *
 * The design of this library follows typical template metaprogramming patterns:
 * we build instances of types modelling the abstract syntax of the problem at
 * hand, then decompose that instance's structure at the same time as the object
 * term's structure.
 *
 * In the rest of this library's documentation, we refer generically to the
 * templated objects constructed to decompose a KORE pattern as "lenses". For
 * example, a _pattern lens_ matches KORE composite patterns with a specific
 * constructor and arity.
 *
 * [1]: https://github.com/runtimeverification/llvm-backend/blob/83a5b3015ac8816076573d3c8ac266b448b78130/lib/ast/AST.cpp#L1503-L1544
 */
namespace kllvm::pattern_matching {

// Forward declarations

template <typename... Ts>
class pattern;

template <typename Inner>
class subject;

class any_;

template <typename Pattern, typename Result>
class map;

namespace detail {

/**
 * Internal implementation detail for `enumerate` that unfolds invocations of `f`
 * over an explicitly-provided `index_sequence`.
 */
template <typename Tuple, typename Func, std::size_t... Idxs>
void enumerate_impl(Tuple &&t, Func &&f, std::index_sequence<Idxs...> idxs) {
  (std::forward<Func>(f)(Idxs, std::get<Idxs>(std::forward<Tuple>(t))), ...);
}

/**
 * Given a tuple-like object `t` with members of type <T0, ..., TN>, `enumerate`
 * statically generates the following sequence of calls to emulate an iteration
 * over the members of `t`:
 *
 *   f(0, std::get<0>(t));
 *   ...
 *   f(N, std::get<N>(t));
 *
 * The parameter `f` must be invokable with two arguments of type:
 *
 *   (std::size_t, Ti)
 *
 * for all member types `Ti` of `t`.
 */
template <typename Tuple, typename Func>
void enumerate(Tuple &&t, Func &&f) {
  constexpr auto size = std::tuple_size_v<std::decay_t<Tuple>>;
  enumerate_impl(
      std::forward<Tuple>(t), std::forward<Func>(f),
      std::make_index_sequence<size>{});
}

/**
 * Helper template to count the number of subject lenses contained in a
 * particular pattern; we ensure statically that constructed patterns don't try
 * to bind incorrect numbers of subject terms.
 */
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

/**
 * Helper type to enable the literal syntax:
 *
 *   "ctor"_p(...)
 *
 * By having "ctor"_p return an instance of this type with a constructor stored,
 * we can overload operator() to forward sub-pattern arguments on when creating
 * actual pattern lenses.
 */
struct pattern_forwarder {
  std::string constructor;

  template <typename... Args>
  pattern<Args...> operator()(Args... args) const {
    return pattern(constructor, args...);
  }
};

} // namespace detail

/**
 * Matching a lens against a pattern produces two useful pieces of information:
 *
 *   - Does the lens model the pattern's structure?
 *   - Was a subject subterm extracted during the matching process?
 *
 * For example, writing `lens :: pattern` for the matching check:
 * 
 *   b(_)          :: a(b()) -> { false, nullptr }
 *   a(_)          :: a(b()) -> { true,  nullptr }
 *   a(subject(_)) :: a(b()) -> { true,  b()     }
 *   subject(a(_)) :: a(b()) -> { true,  a(b())  }
 *
 * All valid lens types must supply an implementation of:
 *
 *   match_result match(std::shared_ptr<KOREPattern> const&) const
 */
struct match_result {
  bool matches;
  std::shared_ptr<KOREPattern> subject;
};

/**
 * Models _any_ KORE pattern.
 */
class any_ {
public:
  match_result match(std::shared_ptr<KOREPattern> const &term) const {
    return {true, nullptr};
  }
};

// Helper syntax to avoid having to write any() every time a wildcard lens is
// needed.
constexpr inline any_ any;

/**
 * Models precisely the same set of KORE patterns as its argument lens of type
 * `Inner`. If the child lens matches against a term, then that term will be
 * propagated upwards as the subject result.
 */
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

/**
 * Models a KORE composite pattern with a known, fixed constructor and arity.
 * Additionally, each child lens must model the corresponding argument of a
 * pattern for the top-level lens to match.
 */
template <typename... Ts>
class pattern {
public:
  pattern(std::string ctor, Ts... children)
      : constructor_(ctor)
      , children_(children...) {
    static_assert(
        detail::subject_count_v<std::decay_t<decltype(*this)>> <= 1,
        "Cannot construct a pattern with multiple nested subject terms");
  }

  /**
   * This lens matches a pattern iff:
   *
   *   - The lens and pattern constructors agree
   *   - The lens and pattern arities agree
   *   - Each child lens matches the corresponding child argument of the pattern
   *
   * The set of lens constructors enforce statically that there can be at most
   * one subject lens contained in a top-level lens. It is therefore safe to
   * assume that the first child lens to match with a returned subject pattern
   * is the only such lens.
   */
  match_result match(std::shared_ptr<KOREPattern> const &term) const {
    if (auto composite = std::dynamic_pointer_cast<KORECompositePattern>(term);
        composite && composite->getArguments().size() == arity()
        && composite->getConstructor()->getName() == constructor_) {
      auto results = std::vector<match_result>{};

      detail::enumerate(children_, [&](auto idx, auto const &subpattern) {
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

/**
 * A `map` transforms the result of matching a lens against a term into an
 * instance of some other type by applying a function. This is useful when
 * testing a sequence of such `map`s in turn to produce a result.
 *
 * The supplied callable should have a signature compatible with:
 *
 *   Result (std::shared_ptr<KOREPattern> const&)
 *
 * where `Result` is a specialisation of `std::optional`. This is required to
 * allow the supplied callable to _not return a mapped value_ for a subset of
 * subject terms, and permits greater composition in downstream code.
 */
template <typename Pattern, typename Result>
class map {
public:
  using result_t = Result;

  template <typename Func>
  map(Pattern p, Func f)
      : pattern_(p)
      , func_(f) { }

  map(Pattern p)
      : map(p, [](auto const &p) { return p; }) { }

  /**
   * Analogous to `match` on a lens type, but with the subject result being a
   * transformed value of type Result rather than a pattern.
   */
  std::pair<bool, Result>
  match(std::shared_ptr<KOREPattern> const &term) const {
    auto [match, subject] = pattern_.match(term);

    if (!match) {
      return {false, std::nullopt};
    }

    return {true, func_(subject)};
  }

private:
  Pattern pattern_;
  std::function<Result(std::shared_ptr<KOREPattern> const &)> func_;
};

template <typename Pattern, typename Func>
map(Pattern, Func) -> map<
    Pattern, std::invoke_result_t<Func, std::shared_ptr<KOREPattern> const &>>;

template <typename Pattern>
map(Pattern) -> map<Pattern, std::optional<std::shared_ptr<KOREPattern>>>;

/**
 * Helper function to allow the precisely deduced type of a `map` constructed
 * from a lens of type Lens to be known.
 */
template <typename Lens>
constexpr auto make_map(Lens arg) {
  return map(arg);
}

template <typename Lens>
using make_map_t = std::invoke_result_t<decltype(make_map<Lens>), Lens>;

/**
 * Compose a non-empty, ordered sequence of lenses or maps into an object that,
 * when matched against a pattern, returns the result of the first successful
 * matching lens or map in that sequence.
 *
 * The first argument passed when constructing a `match_first` determines the
 * signature for the remaining arguments in the sequence.
 *
 * If _any_ child lens matches the structure of the pattern being examined, the
 * "matched" return from a `match_first` is true. The transformed "subject"
 * return is the first one returned by any map in the sequence.
 */
template <typename Lens, typename... Lenses>
class match_first {
public:
  using result_t = typename make_map_t<Lens>::result_t;

  match_first(Lens first, Lenses... rest)
      : maps_(make_map(first), make_map(rest)...) { }

  std::pair<bool, result_t>
  match(std::shared_ptr<KOREPattern> const &term) const {
    result_t result = std::nullopt;
    bool any = false;

    detail::enumerate(maps_, [&](auto idx, auto const &map) {
      if (!result) {
        auto [matched, next_res] = map.match(term);
        if (matched && next_res) {
          result = next_res;
        }

        any = any || matched;
      }
    });

    return {any, result};
  }

private:
  std::tuple<make_map_t<Lens>, make_map_t<Lenses>...> maps_;
};

namespace literals {

/**
 * Convenience literal to allow lenses of the form:
 *
 *   "ctor"_p(args, ...)
 *
 * to be written via the `pattern_forwarder` type. Additionally, if additional
 * conciseness is desired for a set of known constructors:
 *
 *   auto ctor = "ctor"_p;
 *
 * enables the syntax:
 *
 *   ctor(args, ...)
 *
 * to create `pattern` lens objects.
 */
inline detail::pattern_forwarder
operator""_p(const char *str, std::size_t data) {
  return {std::string(str, data)};
}

} // namespace literals

} // namespace kllvm::pattern_matching

#endif
