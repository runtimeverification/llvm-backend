#ifndef RANGEMAP_HEADER_H
#define RANGEMAP_HEADER_H

#include "RBTree.h"

#include "runtime/fmt_error_handling.h"
#include <cassert>
#include <iostream>
#include <optional>
#include <stack>
#include <stdexcept>
#include <vector>

namespace rng_map {

// Range bounded inclusively below and exlusively above: [start, end).
// - T : class of values within the range
template <class T>
class Range {
private:
  T start_;
  T end_;

public:
  // Create a new range [start, end).
  Range(T start, T end)
      : start_(start)
      , end_(end) { }

  // Create a new empty range.
  Range()
      : start_()
      , end_() { }

  // Getter for the start of this range.
  T start() const { return start_; }

  // Getter for the end of this range.
  T end() const { return end_; }

  // The following methods define the ordering for objects of class Range.
  // Operator < is used to subsequently define >, ==, <=, >=, and !=.
  bool operator<(Range const &other) const {
    return this->start_ == other.start_ ? this->end_ < other.end_
                                        : this->start_ < other.start_;
  }

  bool operator>(Range const &other) const { return other < *this; }

  bool operator==(Range const &other) const {
    return !(*this < other || other < *this);
  }

  bool operator<=(Range const &other) const { return !(other < *this); }

  bool operator>=(Range const &other) const { return !(*this < other); }

  bool operator!=(Range const &other) const {
    return *this < other || other < *this;
  }

  // Returns true if this range contains k.
  bool contains(T const &k) const { return k >= start_ && k < end_; }

  // Returns true if this range is empty.
  bool empty() const { return start_ >= end_; }

  // Returns true if this range overlaps with range r.
  bool overlaps(Range const &r) const {
    if (r.end_ <= start_) {
      return false;
    } else if (end_ <= r.start_) {
      return false;
    }
    return true;
  }

  // Returns true if this range and range r overlap or are adjacent, i.e.,
  // share a bound, either inclusive or exclusive.
  bool is_relevant(Range const &r) const {
    if (r.end_ < start_) {
      return false;
    } else if (end_ < r.start_) {
      return false;
    }
    return true;
  }
};

// Map whose keys are stored as ranges.
// - T : class of map keys
// - V : class of map values
template <class T, class V>
class RangeMap {

private:
  // Ordered map based on red-black tree.
  rb_tree::RBTree<Range<T>, V> treemap_;

  // Create a rangemap on top of a red-black tree that uses ranges as keys.
  // The red black tree should already be a well-formed rangemap.
  RangeMap(rb_tree::RBTree<Range<T>, V> t)
      : treemap_(t) { }

  std::optional<std::pair<Range<T>, V>>
  get_key_value(rb_tree::RBTree<Range<T>, V> const &t, T const &k) const {
    if (t.empty()) {
      return std::nullopt;
    }
    Range<T> r = t.root_key();
    if (r.contains(k)) {
      return std::make_pair(r, t.root_val());
    }
    T start = r.start();
    if (k < start) {
      return get_key_value(t.left(), k);
    }
    assert(k >= r.end());
    return get_key_value(t.right(), k);
  }

  // Return true if range r partially or completely overlaps with any range
  // stored in the ordered map t that is passed as an argument.
  bool
  overlaps(rb_tree::RBTree<Range<T>, V> const &t, Range<T> const &r) const {
    if (t.empty()) {
      return false;
    }
    T start = r.start();
    T end = r.end();
    T rstart = t.root_key().start();
    T rend = t.root_key().end();
    if (rend <= start) {
      // The root is to the left of range r, possibly adjacent but not
      // overlapping. Continue looking for overlapping ranges to the right of
      // root.
      return overlaps(t.right(), r);
    } else if (end <= rstart) {
      // The root is to the right of range r, possibly adjacent but not
      // overlapping. Continue looking for overlapping ranges to the left of
      // root.
      return overlaps(t.left(), r);
    } else {
      // In any other case, range r somehow overlaps with root, either partially
      // or completely.
      return true;
    }
  }

  // Gather all <Range<T>, V> pairs in t that are overlapping or directly
  // adjacent (share a boundary) with range r, in v.
  void get_overlapping_or_adjacent_ranges(
      rb_tree::RBTree<Range<T>, V> const &t, Range<T> const &r,
      std::vector<std::pair<Range<T>, V>> &v) const {
    if (t.empty()) {
      return;
    }
    T start = r.start();
    T end = r.end();
    T rstart = t.root_key().start();
    T rend = t.root_key().end();
    if (rend < start) {
      // The candidate range is to the left of our target range, and does
      // not share a boundary. It is not relevant. Continue looking for
      // relevant ranges to the right of the candidate range.
      get_overlapping_or_adjacent_ranges(t.right(), r, v);
    } else if (end < rstart) {
      // The candidate range is to the right of our target range, and
      // does not share a boundary. It is not relevant. Continue looking
      // for relevant ranges to the left of the candidate range.
      get_overlapping_or_adjacent_ranges(t.left(), r, v);
    } else if (start <= rstart && rend <= end) {
      // Our target range contains the candidate range, and may share its
      // boundaries (i.e. equal or larger). The candidate range is
      // relevant, and there may be relevant ranges in both left and
      // right directions.
      get_overlapping_or_adjacent_ranges(t.left(), r, v);
      v.emplace_back(std::make_pair(t.root_key(), t.root_val()));
      get_overlapping_or_adjacent_ranges(t.right(), r, v);
    } else if (rstart < start && end < rend) {
      // The candidate range fully contains the target range, i.e. is
      // larger. It is the only relevant range, no need to look for more.
      v.emplace_back(std::make_pair(t.root_key(), t.root_val()));
    } else if (rstart < start) {
      // The candidate range overlapps with the target range on the left
      // side, but does not fully contain it. It is relevant, and we need
      // to continue looking for relevant ranges to the right of the
      // candidate range.
      v.emplace_back(std::make_pair(t.root_key(), t.root_val()));
      get_overlapping_or_adjacent_ranges(t.right(), r, v);
    } else if (end < rend) {
      // The candidate range overlapps with the target range on the right
      // side, but does not fully contain it. It is relevant, and we need
      // to continue looking for relevant ranges to the left of the
      // candidate range.
      get_overlapping_or_adjacent_ranges(t.left(), r, v);
      v.emplace_back(std::make_pair(t.root_key(), t.root_val()));
    }
  }

  // Gather all <Range, V> pairs in t that are overlapping with range r, in v.
  void get_overlapping_ranges(
      rb_tree::RBTree<Range<T>, V> const &t, Range<T> const &r,
      std::vector<std::pair<Range<T>, V>> &v) const {
    if (t.empty()) {
      return;
    }
    T start = r.start();
    T end = r.end();
    T rstart = t.root_key().start();
    T rend = t.root_key().end();
    if (rend <= start) {
      // The candidate range is to the left of our target range, and may
      // share a boundary. It is not relevant. Continue looking for
      // relevant ranges to the right of the candidate range.
      get_overlapping_ranges(t.right(), r, v);
    } else if (end <= rstart) {
      // The candidate range is to the right of our target range, and
      // may share a boundary. It is not relevant. Continue looking for
      // relevant ranges to the left of the candidate range.
      get_overlapping_ranges(t.left(), r, v);
    } else if (start < rstart && rend < end) {
      // Our target range fully contains the candidate range, i.e. is
      // larger. The candidate range is relevant, and there may be
      // relevant ranges in both left and right directions.
      get_overlapping_ranges(t.left(), r, v);
      v.emplace_back(std::make_pair(t.root_key(), t.root_val()));
      get_overlapping_ranges(t.right(), r, v);
    } else if (rstart <= start && end <= rend) {
      // The candidate range contains the target range, i.e. is equal or
      // larger. It is the only relevant range, no need to look for more.
      v.emplace_back(std::make_pair(t.root_key(), t.root_val()));
    } else if (rstart <= start) {
      // The candidate range overlapps with the target range on the left
      // side, but does not fully contain it. It is relevant, and we need
      // to continue looking for relevant ranges to the right of the
      // candidate range.
      v.emplace_back(std::make_pair(t.root_key(), t.root_val()));
      get_overlapping_ranges(t.right(), r, v);
    } else if (end <= rend) {
      // The candidate range overlapps with the target range on the right
      // side, but does not fully contain it. It is relevant, and we need
      // to continue looking for relevant ranges to the left of the
      // candidate range.
      get_overlapping_ranges(t.left(), r, v);
      v.emplace_back(std::make_pair(t.root_key(), t.root_val()));
    }
  }

  // Gather the intersection ranges of this rangemap and rangemap m, i.e. all
  // ranges in this rangemap that overlap (fully or partially) with ranges in m
  // and are mapped to the same value in both rangemaps, in res.
  void
  get_intersection_ranges(RangeMap const &m, std::vector<Range<T>> &res) const {
    std::vector<std::pair<Range<T>, V>> r1;
    for_each(treemap_, [&r1](Range<T> const &x, V const &v) {
      r1.emplace_back(std::make_pair(x, v));
    });
    std::vector<std::pair<Range<T>, V>> r2;
    for_each(m.treemap_, [&r2](Range<T> const &x, V const &v) {
      r2.emplace_back(std::make_pair(x, v));
    });
    // Compute the intersection of this rangemap and m.
    int i = 0;
    int j = 0;
    // Repeat while there are more ranges in both rangemaps.
    while (i < r1.size() && j < r2.size()) {
      // Consider ranges r1[i] and r2[j] for intersection.
      T s = r1[i].first.start() > r2[j].first.start() ? r1[i].first.start()
                                                      : r2[j].first.start();
      T e = r1[i].first.end() < r2[j].first.end() ? r1[i].first.end()
                                                  : r2[j].first.end();
      if (s < e && r1[i].second == r2[j].second) {
        res.emplace_back(Range(s, e));
      }
      // The range with the smallest end has been processed - remove it.
      if (r1[i].first.end() < r2[j].first.end()) {
        i++;
      } else {
        j++;
      }
    }
  }

public:
  // Create an empty rangemap.
  RangeMap()
      : treemap_(rb_tree::RBTree<Range<T>, V>()) { }

  // Create a rangemap with elements from the container designated by the
  // beginning and end iterator arguments. The container should contain elements
  // of type std::pair<Range<T>,V>.
  template <class I>
  RangeMap(I b, I e) {
    RangeMap m = RangeMap();
    for_each(b, e, [&m](std::pair<Range<T>, V> const &p) {
      m = m.inserted(p.first, p.second);
    });
    treemap_ = m.treemap_;
  }

  // Getter for the rb-tree underlying this rangemap.
  rb_tree::RBTree<Range<T>, V> treemap() const { return treemap_; }

  // Return the number of key ranges in the map.
  size_t size() const { return treemap_.size(); }

  // Return true if a range in this map contains the key k.
  bool contains(T const &k) const { return get_key_value(k).has_value(); }

  // If the key k is contained in any range in this map, return the value
  // associated with k.
  std::optional<V> get_value(T const &k) const {
    auto opt = get_key_value(k);
    if (opt.has_value()) {
      return opt.value().second;
    }
    return std::nullopt;
  }

  // If the key k is contained in any range in this map, return the key range-
  // value pair associated with k.
  std::optional<std::pair<Range<T>, V>> get_key_value(T const &k) const {
    return get_key_value(treemap_, k);
  }

  /* Return a map resulting from inserting the given key range and value into
     * this map.
     * If the inserted range partially or completely overlaps any existing
     * range in the map, then the existing range (or ranges) will be partially
     * or completely replaced by the inserted range in the resulting map.
     * If the inserted range either overlaps or is immediately adjacent to any
     * existing range mapping to the same value, then the ranges will be
     * coalesced into a single contiguous range in the resulting map.
     */
  RangeMap inserted(Range<T> const &r, V const &v) const {
    // Empty ranges do not make sense here.
    if (r.empty()) {
      KLLVM_HOOK_INVALID_ARGUMENT("Insert empty range in range map");
    }

    std::vector<std::pair<Range<T>, V>> ranges;
    get_overlapping_or_adjacent_ranges(treemap_, r, ranges);
    // Each relevant range may lead to changes to the existing underlying
    // treemap data structure, as well as the bounds of the target inserted
    // range. We iterate over the collected relevant ranges to collect
    // these changes.
    T is = r.start();
    T ie = r.end();
    rb_tree::RBTree<Range<T>, V> tmpmap = treemap_;
    for (auto &p : ranges) {
      Range<T> rr = p.first;
      V rv = p.second;
      assert(r.is_relevant(rr));
      T rrs = rr.start();
      T rre = rr.end();

      if (v == rv) {
        // The inserted value is the same as the value stored in
        // the relevant range.
        // Adjust the bounds of the inserted range as needed, and delete
        // the relevant range.
        is = rrs < is ? rrs : is;
        ie = rre > ie ? rre : ie;
        tmpmap = tmpmap.deleted(rr);
      } else {
        // The inserted value is different from the value stored
        // in the relevant range.
        if (r.overlaps(rr)) {
          // The ranges overlap.
          // Delete the relevant range, and insert up to two ranges to
          // the left/right of the inserted range, as needed to
          // account for the bounds of the relevant range.
          tmpmap = tmpmap.deleted(rr);
          if (rrs < is) {
            tmpmap = tmpmap.inserted(Range<T>(rrs, is), rv);
          }
          if (rre > ie) {
            tmpmap = tmpmap.inserted(Range<T>(ie, rre), rv);
          }
        } else {
          // The ranges do not overlap. NOOP.
        }
      }
    }

    // Finally, insert a range with the properly adjusted bounds and the
    // new value. Create a new range map.
    tmpmap = tmpmap.inserted(Range<T>(is, ie), v);
    return RangeMap(tmpmap);
  }

  /* Given a range to be removed and this map, return a map that does not
     * contain the removed range or any part of it.
     * If the range to be removed partially overlaps with any ranges in the
     * map, then the boundaries of these ranges are adjusted in the resulting
     * map so that they do not overlap with the removed range.
     */
  RangeMap deleted(Range<T> const &r) const {
    // Empty ranges do not make sense here.
    if (r.empty()) {
      KLLVM_HOOK_INVALID_ARGUMENT("Delete empty range from range map");
    }

    std::vector<std::pair<Range<T>, V>> ranges;
    get_overlapping_ranges(treemap_, r, ranges);
    // Each relevant range may lead to changes to the existing underlying
    // treemap data structure.
    // We iterate over the collected relevant ranges to collect and apply
    // these changes.
    T ds = r.start();
    T de = r.end();
    rb_tree::RBTree<Range<T>, V> tmpmap = treemap_;
    for (auto &p : ranges) {
      Range<T> rr = p.first;
      V rv = p.second;
      assert(r.overlaps(rr));
      T rrs = rr.start();
      T rre = rr.end();
      tmpmap = tmpmap.deleted(rr);
      if (rrs < ds) {
        tmpmap = tmpmap.inserted(Range<T>(rrs, ds), rv);
      }
      if (rre > de) {
        tmpmap = tmpmap.inserted(Range<T>(de, rre), rv);
      }
    }
    return RangeMap(tmpmap);
  }

  // Return true if range r partially or completely overlaps with any key range
  // stored in this rangemap.
  bool overlaps(Range<T> const &r) const { return overlaps(treemap_, r); }

  // Print this rangemap to output stream os.
  void print(std::ostream &os) const {
    os << "--------------------------" << std::endl;
    for_each(treemap_, [&os](Range<T> x, V v) {
      os << "[ " << x.start() << ".." << x.end() << " ) -> " << v << std::endl;
    });
    os << "--------------------------" << std::endl;
  }

  // Return a rangemap that is the concatenation of this rangemap and rangemap
  // m. Throw an exception if any of the key ranges in this rangemap overlaps
  // with any of the key ranges in rangemap m.
  RangeMap concat(RangeMap const &m) const {
    RangeMap res = *this;
    for_each(m.treemap_, [&res, this](Range<T> const &x, V const &v) {
      if (!overlaps(x)) {
        res = res.inserted(x, v);
      } else {
        KLLVM_HOOK_INVALID_ARGUMENT(
            "Overlapping key ranges in map concatenation");
      }
    });
    return res;
  }

  // Return a rangemap that is the difference of this rangemap and rangemap m.
  // Given rangemaps A and B, we compute the intersection of A and B, as all
  // key range-value pairs whose keys are contained in ranges in both A and B
  // and are mapped to the same value. Then, we perform A - A^B, and return the
  // map resulting from deleting these ranges from A.
  RangeMap difference(RangeMap const &m) const {
    // Compute the intersection of this rangemap and m.
    std::vector<Range<T>> intersect;
    get_intersection_ranges(m, intersect);
    // Delete all collected intersection ranges from this rangemap.
    RangeMap tmpmap = *this;
    for (auto &r : intersect) {
      tmpmap = tmpmap.deleted(r);
    }
    return tmpmap;
  }

  // Return true if this rangemap is included in rangemap m, i.e. all key
  // range-value pairs contained in this rangemap whose keys are also contained
  // in key ranges in m and are mapped to the same value.
  bool inclusion(RangeMap const &m) const {
    // Compute the intersection of this rangemap and m.
    std::vector<Range<T>> intersect;
    get_intersection_ranges(m, intersect);
    // Compare the intersection ranges with this rangemap's ranges.
    // If they differ, return false.
    auto it = intersect.begin();
    bool equals = true;
    for_each(
        treemap_, [&intersect, &it, &equals](Range<T> const &x, V const &v) {
          if (it == intersect.end()) {
            equals = false;
          } else {
            if (x != *it) {
              equals = false;
            }
            ++it;
          }
        });
    return (it == intersect.end()) && equals;
  }
};

// Iterator over objects of class RangeMap.
// We do not need the full flexibility offered by the std interface.
// Instead we only need to iterate. Therefore, this iterator provides
// prefix increment operator, dereference operator, and a function that tests
// if there are more elements instead of an equality operator.
template <class T, class V>
class RangeMapIterator {

private:
  std::stack<rb_tree::RBTree<Range<T>, V>> stack_;

  void update_stack_state(rb_tree::RBTree<Range<T>, V> const &t) {
    rb_tree::RBTree<Range<T>, V> tmp = t;
    while (!tmp.empty()) {
      stack_.push(tmp);
      tmp = tmp.left();
    }
  }

public:
  // Create an iterator over rangemap m.
  RangeMapIterator(RangeMap<T, V> m) { update_stack_state(m.treemap()); }

  // Prefix increment operator.
  void operator++() {
    rb_tree::RBTree<Range<T>, V> const &t = stack_.top();
    stack_.pop();
    update_stack_state(t.right());
  }

  // Dereference operator.
  std::pair<Range<T>, V> const &operator*() const {
    rb_tree::RBTree<Range<T>, V> const &t = stack_.top();
    return t.root_data();
  }

  std::pair<Range<T>, V> const &operator->() const {
    rb_tree::RBTree<Range<T>, V> const &t = stack_.top();
    return &t.root_data();
  }

  // Return true if there are more elemetns in the underlying rangemap.
  bool has_next() const { return !stack_.empty(); }
};

// Return a rangemap with all elements in m, and then also from the container
// designated by the beginning and end iterator arguments. The container should
// contain elements of type std::pair<Range<T>, V>.
template <class T, class V, class I>
RangeMap<T, V> inserted(RangeMap<T, V> const &m, I it, I end) {
  if (it == end) {
    return m;
  }
  Range<T> key = it->first;
  V val = it->second;
  auto m1 = m.inserted(key, val);
  return inserted(m1, ++it, end);
}

// Apply function f to all elements of rangemap m.
// Function f must accept two arguments of types T, corresponding to the start
// and end of a range, and one of type V correspondong to the mapped value.
template <class T, class V, class F>
void for_each(RangeMap<T, V> const &m, F &&f) {
  for_each(m.treemap(), [&f](Range<T> const &x, V const &v) {
    std::invoke(f, x.start(), x.end(), v);
  });
}

} // namespace rng_map

#endif // RANGEMAP_HEADER_H
