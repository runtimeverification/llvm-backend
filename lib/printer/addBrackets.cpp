#include "kllvm/ast/AST.h"

#include <cassert>
#include <cstdlib>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

namespace kllvm {

enum fixity { EMPTY = 0, BareLeft = 1, BareRight = 2, BareBoth = 3 };

/**
 * Get the index of the Nth nonterminal within the list of all terminals and
 * nonterminals in a production.
 *
 * terminals: the `terminals` attribute of a symbol, representing the pattern
 * of which production items are terminals and which are nonterminals as a
 * pattern of 1s and 0s.
 *
 * position: the index of the nonterminal to look up within the list of all
 * nonterminals of the symbol
 *
 * returns: the index of the nonterminal within the list of all nonterminals
 * and terminals of the production the symbol corresponds to.
 */
int get_nt_position_in_prod(std::string terminals, int position) {
  int terminal_pos = 0;
  int nt_idx = 0;
  for (; terminal_pos < terminals.size(); terminal_pos++) {
    if (terminals[terminal_pos] == '0') {
      if (nt_idx == position) {
        break;
      }
      nt_idx++;
    }
  }
  return terminal_pos;
}

/**
 * Get whether a production has a terminal at a particular index.
 *
 * Out of bounds errors do not occur with this function: if the index is out of
 * range, `false` is returned, indicating there is not a terminal at that index
 */
bool has_terminal_at_idx(std::string terminals, int position) {
  if (position < 0 || position >= terminals.length()) {
    return false;
  }
  return terminals[position] == '1';
}

/**
 * Compute whether there are terminals immediately to the left and right of a
 * nonterminal in a production.
 *
 * position: the index of the nonterminal to look up within the list of all
 * nonterminals in a symbol
 *
 * sym: the symbol to examine
 *
 * data: pretty printing metadata
 *
 * returns: EMPTY if there are terminals on both sides of this nonterminal
 *          BARE_LEFT if there is a terminal only on the right of this
 * nonterminal BARE_RIGHT if there is a terminal only on the left of this
 * nonterminal BARE_BOTH if there are not terminals on either side of this
 * nonterminal
 */
fixity
get_fixity(int position, kore_symbol *sym, pretty_print_data const &data) {
  int result = EMPTY;
  std::string terminals = data.terminals.at(sym->get_name());
  int terminal_pos = get_nt_position_in_prod(terminals, position);
  if (!has_terminal_at_idx(terminals, terminal_pos + 1)) {
    result |= BareRight;
  }
  if (!has_terminal_at_idx(terminals, terminal_pos - 1)) {
    result |= BareLeft;
  }
  return (fixity)result;
}

/**
 * Compute whether there are terminals on the far left and far right of a
 * production.
 *
 * sym: the symbol to examine
 *
 * data: pretty printing metadata
 *
 * returns: EMPTY if there are terminals on both edges of this production.
 *          BARE_LEFT if there is a terminal only on the right edge of this
 * production BARE_RIGHT if there is a terminal only on the left edge of this
 * production BARE_BOTH if there are not terminals on either edge of this
 * production
 */
fixity get_fixity(kore_symbol *sym, pretty_print_data const &data) {
  auto const &name = sym->get_name();
  int result = EMPTY;
  if (data.terminals.at(name)[0] == '0') {
    result = result | BareLeft;
  }
  if (data.terminals.at(name)[data.terminals.at(name).size() - 1] == '0') {
    result = result | BareRight;
  }
  return (fixity)result;
}

/**
 * Compute the left capture of a pattern.
 *
 * If you were to unparse the AST of a term without any parentheses as a
 * sequence of terminals, start from the first terminal that is part of a
 * particular pattern (either directly or nested) and proceed left until you
 * reach the first terminal that is part of a pattern (directly) that is a
 * parent of that pattern, the pattern whose terminal you reach would be the
 * left capture of that pattern. Not all terms have a left capture; for
 * example, the left capture of the top term in an AST does not exist.
 *
 * previousLeftCapture: the left capture of `outer`
 *
 * outer: the parent of the symbol you wish to compute the left capture of
 *
 * position: the position within the nonterminals of `outer` of the pattern
 * you wish to compute the left capture of.
 *
 * data: pretty printing metadata
 *
 * returns: the left capture of the term, or NULL if no such term exists
 */
kore_composite_pattern *get_left_capture(
    kore_composite_pattern *previous_left_capture,
    kore_composite_pattern *outer, int position,
    pretty_print_data const &data) {
  fixity fixity = get_fixity(outer->get_constructor(), data);
  if (position == 0 && (fixity & BareLeft)) {
    return previous_left_capture;
  }

  return outer;
}

/**
 * Compute the right capture of a pattern.
 *
 * If you were to unparse the AST of a term without any parentheses as a
 * sequence of terminals, start from the last terminal that is part of a
 * particular pattern (either directly or nested) and proceed right until you
 * reach the first terminal that is part of a pattern (directly) that is a
 * parent of that pattern, the pattern whose terminal you reach would be the
 * right capture of that pattern. Not all terms have a right capture; for
 * example, the right capture of the top term in an AST does not exist.
 *
 * previousRightCapture: the right capture of `outer`
 *
 * outer: the parent of the symbol you wish to compute the right capture of
 *
 * position: the position within the nonterminals of `outer` of the pattern
 * you wish to compute the right capture of.
 *
 * data: pretty printing metadata
 *
 * returns: the right capture of the term, or NULL if no such term exists
 */

kore_composite_pattern *get_right_capture(
    kore_composite_pattern *previous_right_capture,
    kore_composite_pattern *outer, int position,
    pretty_print_data const &data) {
  fixity fixity = get_fixity(outer->get_constructor(), data);
  if (position == outer->get_arguments().size() - 1 && (fixity & BareRight)) {
    return previous_right_capture;
  }

  return outer;
}

/**
 * Computes whether one sort is less than or equal to the other according to
 * the subsort relation of the definition.
 *
 * data: pretty printing metadata
 *
 * s1: first sort
 *
 * s2: second sort
 *
 * returns: s1 <= s2
 */
bool less_than_eq(pretty_print_data const &data, kore_sort *s1, kore_sort *s2) {
  return *s1 == *s2
         || (data.subsorts.contains(s1) && data.subsorts.at(s1).contains(s2));
}

sptr<kore_sort> get_arg_sort(
    kore_symbol *symbol, int position, sptr<kore_sort> first_arg_sort) {
  if (!symbol->is_builtin()) {
    return symbol->get_arguments()[position];
  }

  if (symbol->get_name() == "\\and" || symbol->get_name() == "\\not"
      || symbol->get_name() == "\\or" || symbol->get_name() == "\\implies"
      || symbol->get_name() == "\\iff" || symbol->get_name() == "\\ceil"
      || symbol->get_name() == "\\floor" || symbol->get_name() == "\\equals"
      || symbol->get_name() == "\\in" || symbol->get_name() == "\\next"
      || symbol->get_name() == "\\rewrites"
      || symbol->get_name() == "weakAlwaysFinally"
      || symbol->get_name() == "weakExistsFinally"
      || symbol->get_name() == "allPathGlobally") {
    return symbol->get_formal_arguments()[0];
  }

  if (symbol->get_name() == "\\forall" || symbol->get_name() == "\\exists") {
    if (position == 0) {
      assert(first_arg_sort != nullptr);
      return first_arg_sort;
    }

    return symbol->get_formal_arguments()[0];
  }

  if (symbol->get_name() == "\\mu" || symbol->get_name() == "\\nu") {
    assert(first_arg_sort != nullptr);
    return first_arg_sort;
  }

  abort();
}

sptr<kore_sort> get_return_sort(kore_pattern *pat) {
  if (auto *composite = dynamic_cast<kore_composite_pattern *>(pat)) {
    auto *symbol = composite->get_constructor();
    if (!symbol->is_builtin()) {
      return pat->get_sort();
    }
    if (symbol->get_name() == "\\top" || symbol->get_name() == "\\bottom"
        || symbol->get_name() == "\\and" || symbol->get_name() == "\\not"
        || symbol->get_name() == "\\or" || symbol->get_name() == "\\implies"
        || symbol->get_name() == "\\iff" || symbol->get_name() == "\\exists"
        || symbol->get_name() == "\\forall" || symbol->get_name() == "\\next"
        || symbol->get_name() == "\\rewrites"
        || symbol->get_name() == "weakAlwaysFinally"
        || symbol->get_name() == "weakExistsFinally"
        || symbol->get_name() == "allPathGlobally") {
      return symbol->get_formal_arguments()[0];
    }
    if (symbol->get_name() == "\\ceil" || symbol->get_name() == "\\floor"
        || symbol->get_name() == "\\equals" || symbol->get_name() == "\\in") {
      return symbol->get_formal_arguments()[1];
    }
    if (symbol->get_name() == "\\mu" || symbol->get_name() == "\\nu") {
      return composite->get_arguments()[0]->get_sort();
    }
    abort();

  } else {
    return pat->get_sort();
  }
}

/**
 * Computes whether `inner` can appear at position `position` inside `outer`
 * according to the rules of subsorting, priority, and associativity. The
 * pattern does not actually have to exist at that position; we ask only
 * whether it `can` appear there.
 *
 * outer: the outer pattern
 *
 * inner: the inner pattern
 *
 * position: a position within `outer` you wish to test
 *
 * data: pretty printing metadata
 *
 * returns: true if priority forbids `inner` to appear inside `outer` at that
 * position.
 */
bool is_priority_wrong(
    kore_composite_pattern *outer, kore_composite_pattern *inner, int position,
    pretty_print_data const &data) {
  std::string outer_name = outer->get_constructor()->get_name();
  std::string inner_name = inner->get_constructor()->get_name();
  kore_sort *inner_sort = get_return_sort(inner).get();
  kore_sort *outer_sort = get_arg_sort(
                              outer->get_constructor(), position,
                              outer->get_arguments()[0]->get_sort())
                              .get();
  if (!less_than_eq(data, inner_sort, outer_sort)) {
    return true;
  }
  if (data.priorities.contains(outer_name)
      && data.priorities.at(outer_name).contains(inner_name)) {
    return true;
  }
  std::string terminals = data.terminals.at(outer_name);
  int terminal_pos = get_nt_position_in_prod(terminals, position);
  if (data.left_assoc.contains(outer_name)
      && data.left_assoc.at(outer_name).contains(inner_name)
      && terminal_pos == terminals.size() - 1) {
    return true;
  }
  if (data.right_assoc.contains(outer_name)
      && data.right_assoc.at(outer_name).contains(inner_name)
      && terminal_pos == 0) {
    return true;
  }
  return false;
}

/**
 * Computes whether a bracket needs to appear around `inner`.
 *
 * outer: the parent of `inner`
 *
 * leftCapture: the left capture of `inner`
 *
 * rightCapture: the right capture of `inner`
 *
 * inner: the pattern to test
 *
 * position: the position of `inner` within the nonterminals of `outer`
 *
 * data: pretty printing metadata
 *
 * returns: true if a bracket is required according to this algorithm. Roughly
 * speaking, parentheses are not needed around associative symbols, constants,
 * or patterns that appear between two terminals in their parent. Otherwise,
 * parentheses are needed if priority or associativity forbids the term inside
 * its parent, or if the term begins or ends with a nonterminal and priority
 * and associativity do not forbid the term from appearing inside its left
 * or right capture, respectively, and its left or right capture does not
 * end or begin, respectively, with a terminal.
 *
 * Warning: this algorithm is *unsound*. There is no guarantee that after
 * it completes, the resulting term can be unparsed into a string that will
 * parse unambiguously as the original AST. Indeed, the problem of inserting
 * brackets into an AST in order to ensure this property is NP-hard, and
 * grammars can be constructed such that solving the unparsing problem is
 * equivalent to boolean satisfiability. This algorithm is merely a linear-time
 * approximation of the general problem which performs very well in most
 * real-world grammars, especially the common expression-grammar category of
 * grammar where parentheses are most often required in practice.
 *
 * As an example, consider the following simple grammar:
 *
 * syntax Exp ::= Exp "++" | "++" Exp | Exp Exp | "(" Exp ")" [bracket]
 *
 * The terms `(1 ++) 1` and `1 (++ 1)` will not have brackets inserted
 * into them by this algorithm, even though they are required.
 */
// NOLINTNEXTLINE(*-cognitive-complexity)
bool requires_bracket_with_simple_algorithm(
    kore_composite_pattern *outer, kore_composite_pattern *left_capture,
    kore_composite_pattern *right_capture, kore_pattern *inner, int position,
    pretty_print_data const &data) {
  if (auto *composite = dynamic_cast<kore_composite_pattern *>(inner)) {
    std::string inner_name = composite->get_constructor()->get_name();
    if (inner_name == outer->get_constructor()->get_name()) {
      if (data.assoc.contains(inner_name)) {
        return false;
      }
    }
    if (inner_name == "\\dv") {
      return false;
    }
    auto fixity = get_fixity(position, outer->get_constructor(), data);
    if (fixity == EMPTY) {
      return false;
    }
    bool priority = is_priority_wrong(outer, composite, position, data);
    if (priority) {
      return true;
    }
    if (data.terminals.at(inner_name) == "0") {
      return false;
    }

    auto inner_fixity = get_fixity(composite->get_constructor(), data);

    if ((inner_fixity & BareRight) && right_capture != nullptr) {
      bool inverse_priority = is_priority_wrong(
          composite, right_capture, composite->get_arguments().size() - 1,
          data);
      auto right_capture_fixity
          = get_fixity(right_capture->get_constructor(), data);
      if (!inverse_priority && (right_capture_fixity & BareLeft)) {
        return true;
      }
    }

    if ((inner_fixity & BareLeft) && left_capture != nullptr) {
      bool inverse_priority
          = is_priority_wrong(composite, left_capture, 0, data);
      auto left_capture_fixity
          = get_fixity(left_capture->get_constructor(), data);
      if (!inverse_priority && (left_capture_fixity & BareRight)) {
        return true;
      }
    }

    return false;
  }
  return false;
}

sptr<kore_pattern> add_brackets(
    sptr<kore_pattern> inner, kore_composite_pattern *outer,
    kore_composite_pattern *left_capture, kore_composite_pattern *right_capture,
    int position, pretty_print_data const &data) {
  if (auto *inner_composite
      = dynamic_cast<kore_composite_pattern *>(inner.get())) {
    if (inner_composite->get_constructor()->get_name() == "inj") {
      return add_brackets(
          inner_composite->get_arguments()[0], outer, left_capture,
          right_capture, position, data);
    }
  }
  if (requires_bracket_with_simple_algorithm(
          outer, left_capture, right_capture, inner.get(), position, data)) {
    sptr<kore_sort> outer_sort = get_arg_sort(
        outer->get_constructor(), position,
        outer->get_arguments()[0]->get_sort());
    sptr<kore_sort> inner_sort = get_return_sort(inner.get());
    for (auto const &entry : data.brackets) {
      bool is_correct_outer_sort
          = less_than_eq(data, entry.first, outer_sort.get());
      if (is_correct_outer_sort) {
        for (kore_symbol *s : entry.second) {
          bool is_correct_inner_sort = less_than_eq(
              data, inner_sort.get(), get_arg_sort(s, 0, nullptr).get());
          if (is_correct_inner_sort) {
            sptr<kore_composite_pattern> result
                = kore_composite_pattern::create(s);
            result->add_argument(inner);
            return result;
          }
        }
      }
    }
    sptr<kore_composite_pattern> result
        = kore_composite_pattern::create("bracket");
    result->add_argument(inner);
    result->get_constructor()->add_sort(inner_sort);
    return result;
  }
  return inner;
}

sptr<kore_pattern> add_brackets(
    sptr<kore_pattern> t, kore_composite_pattern *previous_left_capture,
    kore_composite_pattern *previous_right_capture,
    pretty_print_data const &data) {
  if (auto *outer = dynamic_cast<kore_composite_pattern *>(t.get())) {
    if (outer->get_constructor()->get_name() == "\\dv") {
      return t;
    }
    std::vector<sptr<kore_pattern>> new_items;

    sptr<kore_composite_pattern> result
        = kore_composite_pattern::create(outer->get_constructor());
    int position = 0;
    for (auto const &inner : outer->get_arguments()) {
      kore_composite_pattern *left_capture
          = get_left_capture(previous_left_capture, outer, position, data);
      kore_composite_pattern *right_capture
          = get_right_capture(previous_right_capture, outer, position, data);
      sptr<kore_pattern> new_inner = add_brackets(
          inner, outer, left_capture, right_capture, position, data);
      new_inner = add_brackets(new_inner, left_capture, right_capture, data);
      result->add_argument(new_inner);
      position++;
    }
    return result;
  }
  return t;
}

sptr<kore_pattern>
add_brackets(sptr<kore_pattern> const &t, pretty_print_data const &data) {
  return add_brackets(t, nullptr, nullptr, data);
}

} // namespace kllvm
