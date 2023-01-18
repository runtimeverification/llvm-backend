#ifndef RANGEMAP_HEADER_H
#define RANGEMAP_HEADER_H

#include "RBTree.h"

#include <cassert>
#include <iostream>
#include <optional>
#include <stdexcept>

template <class T>
class Range {
private:
    T start_;
    T end_;

public:
  Range(T start, T end)
      : start_(start), end_(end) { }

  T get_start() {
    return start_;
  }

  T get_end() {
    return end_;
  }

  bool operator<(const Range &other) const {
    return this->start_ == other.start_ ? this->end_ < other.end_
                                        : this->start_ < other.start_;
  }

  bool operator>(const Range &other) const {
    return other < *this;
  }

  bool operator==(const Range &other) const {
    return !(*this < other || other < *this);
  }

  bool operator<=(const Range &other) const {
    return !(other < *this);
  }

  bool operator>=(const Range &other) const {
    return !(*this < other);
  }

  bool operator!=(const Range &other) const {
    return *this < other || other < *this;
  }

  // Returns true if this range contains k
  bool contains(T k) const {
    return k >= start_ && k < end_;
  }

  // Returns true if this range is empty
  bool is_empty() const {
    return start_ >= end_;
  }

  // Returns true if this range overlaps with range r
  bool overlaps(Range r) const {
    if (r.end_ <= start_)
      return false;
    else if (end_ <= r.start_)
      return false;
    return true;
  }

  // Returns true if this range and range r overlap or are adjacent, i.e.,
  // share a bound, either inclusive or exclusive.
  bool is_relevant(Range r) const {
    if (r.end_ < start_)
      return false;
    else if (end_ < r.start_)
      return false;
    return true;
  }
};

template <class T, class V>
class RangeMap {

private:
  RBTree<Range<T>, V> map_;

  std::optional<std::pair<Range<T>, V>>
  getKeyValue(RBTree<Range<T>, V> t, T k) const {
    if (t.isEmpty())
      return std::optional<std::pair<Range<T>, V>>();
    Range<T> r = t.root();
    if (r.contains(k))
      return std::make_optional<std::pair<Range<T>, V>>(
          std::make_pair(r, t.rootVal()));
    T start = r.get_start();
    if (k < start)
      return getKeyValue(t.left(), k);
    assert(k >= r.get_end());
    return getKeyValue(t.right(), k);
  }

  bool overlaps(RBTree<Range<T>, V> const &t, Range<T> r) const {
    if (t.isEmpty())
      return false;
    T start = r.get_start();
    T end = r.get_end();
    T rstart = t.root().get_start();
    T rend = t.root().get_end();
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
  void getInsertionRelevantRanges(
      RBTree<Range<T>, V> t, Range<T> r, std::vector<std::pair<Range<T>, V>> &v) {
    if (t.isEmpty())
      return;
    T start = r.get_start();
    T end = r.get_end();
    T rstart = t.root().get_start();
    T rend = t.root().get_end();
    if (rend < start) {
      // The candidate range is to the left of our target range, and does
      // not share a boundary. It is not relevant. Continue looking for
      // relevant ranges to the right of the candidate range.
      getInsertionRelevantRanges(t.right(), r, v);
    } else if (end < rstart) {
      // The candidate range is to the right of our target range, and
      // does not share a boundary. It is not relevant. Continue looking
      // for relevant ranges to the left of the candidate range.
      getInsertionRelevantRanges(t.left(), r, v);
    } else if (start <= rstart && rend <= end) {
      // Our target range contains the candidate range, and may share its
      // boundaries (i.e. equal or larger). The candidate range is
      // relevant, and there may be relevant ranges in both left and
      // right directions.
      getInsertionRelevantRanges(t.left(), r, v);
      v.push_back(std::make_pair(t.root(), t.rootVal()));
      getInsertionRelevantRanges(t.right(), r, v);
    } else if (rstart < start && end < rend) {
      // The candidate range fully contains the target range, i.e. is
      // larger. It is the only relevant range, no need to look for more.
      v.push_back(std::make_pair(t.root(), t.rootVal()));
    } else if (rstart < start) {
      // The candidate range overlapps with the target range on the left
      // side, but does not fully contain it. It is relevant, and we need
      // to continue looking for relevant ranges to the right of the
      // candidate range.
      v.push_back(std::make_pair(t.root(), t.rootVal()));
      getInsertionRelevantRanges(t.right(), r, v);
    } else if (end < rend) {
      // The candidate range overlapps with the target range on the right
      // side, but does not fully contain it. It is relevant, and we need
      // to continue looking for relevant ranges to the left of the
      // candidate range.
      getInsertionRelevantRanges(t.left(), r, v);
      v.push_back(std::make_pair(t.root(), t.rootVal()));
    }
  }

  // Gather all <Range, V> pairs in t that are overlapping with range r, in v.
  void getDeletionRelevantRanges(
      RBTree<Range<T>, V> t, Range<T> r, std::vector<std::pair<Range<T>, V>> &v) {
    if (t.isEmpty())
      return;
    T start = r.get_start();
    T end = r.get_end();
    T rstart = t.root().get_start();
    T rend = t.root().get_end();
    if (rend <= start) {
      // The candidate range is to the left of our target range, and may
      // share a boundary. It is not relevant. Continue looking for
      // relevant ranges to the right of the candidate range.
      getDeletionRelevantRanges(t.right(), r, v);
    } else if (end <= rstart) {
      // The candidate range is to the right of our target range, and
      // may share a boundary. It is not relevant. Continue looking for
      // relevant ranges to the left of the candidate range.
      getDeletionRelevantRanges(t.left(), r, v);
    } else if (start < rstart && rend < end) {
      // Our target range fully contains the candidate range, i.e. is
      // larger. The candidate range is relevant, and there may be
      // relevant ranges in both left and right directions.
      getDeletionRelevantRanges(t.left(), r, v);
      v.push_back(std::make_pair(t.root(), t.rootVal()));
      getDeletionRelevantRanges(t.right(), r, v);
    } else if (rstart <= start && end <= rend)
    // The candidate range contains the target range, i.e. is equal or
    // larger. It is the only relevant range, no need to look for more.
    {
      v.push_back(std::make_pair(t.root(), t.rootVal()));
    } else if (rstart <= start) {
      // The candidate range overlapps with the target range on the left
      // side, but does not fully contain it. It is relevant, and we need
      // to continue looking for relevant ranges to the right of the
      // candidate range.
      v.push_back(std::make_pair(t.root(), t.rootVal()));
      getDeletionRelevantRanges(t.right(), r, v);
    } else if (end <= rend) {
      // The candidate range overlapps with the target range on the right
      // side, but does not fully contain it. It is relevant, and we need
      // to continue looking for relevant ranges to the left of the
      // candidate range.
      getDeletionRelevantRanges(t.left(), r, v);
      v.push_back(std::make_pair(t.root(), t.rootVal()));
    }
  }

public:
  RangeMap()
      : map_(RBTree<Range<T>, V>()) { }

  RangeMap(RBTree<Range<T>, V> t)
      : map_(t) { }

  template <class I>
  RangeMap(I b, I e) {
    RangeMap m = RangeMap();
    for_each(b, e, [&m](std::pair<Range<T>, V> const &p) {
      m = m.inserted(p.first, p.second);
    });
    map_ = m.map_;
  }

  RBTree<Range<T>, V> getTreeMap() const { return map_; }

  // Return the number of key ranges in the map.
  size_t size() const { return map_.size(); }

  // Return true if a range in the map contains the key.
  bool contains(T k) const {
    std::optional<std::pair<Range<T>, V>> opt = getKeyValue(k);
    if (opt.has_value())
      return true;
    return false;
  }

  // If the key is contained in any range in the map, return the value
  // associated with the key.
  std::optional<V> getValue(T k) const {
    std::optional<std::pair<Range<T>, V>> opt = getKeyValue(k);
    if (opt.has_value())
      return std::make_optional<V>(opt.value().second);
    return std::optional<V>();
  }

  // If the key is contained in any range in the map, return the key range-
  // value pair associated with the key.
  std::optional<std::pair<Range<T>, V>> getKeyValue(T k) const {
    return getKeyValue(map_, k);
  }

  /* Given a map and a pair of key range and value, return a map containing
     * the key range-value pair.
     * If the inserted range partially or completely overlaps any existing
     * range in the map, then the existing range (or ranges) will be partially
     * or completely replaced by the inserted range in the resulting map.
     * If the inserted range either overlaps or is immediately adjacent to any
     * existing range mapping to the same value, then the ranges will be
     * coalesced into a single contiguous range in the resulting map.
     */
  RangeMap inserted(Range<T> r, V v) {
    // Empty ranges do not make sense here.
    if (r.is_empty())
      CONSTRUCT_MSG_AND_THROW("Insert empty range in range map");

    std::vector<std::pair<Range<T>, V>> ranges;
    getInsertionRelevantRanges(map_, r, ranges);
    // Each relevant range may lead to changes to the existing underlying
    // treemap data structure, as well as the bounds of the target inserted
    // range. We iterate over the collected relevant ranges to collect
    // these changes.
    T is = r.get_start();
    T ie = r.get_end();
    RBTree<Range<T>, V> tmpmap = map_;
    for (auto &p : ranges) {
      Range<T> rr = p.first;
      V rv = p.second;
      assert(r.is_relevant(rr));
      T rrs = rr.get_start();
      T rre = rr.get_end();

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

  /* Given a range to be removed and a map, return a map that does not
     * contain the removed range or any part of it.
     * If the range to be removed partially overlaps with any ranges in the
     * map, then the boundaries of these ranges are adjusted in the resulting
     * map so that they do not overlap with the removed range.
     */
  RangeMap deleted(Range<T> r) {
    // Empty ranges do not make sense here.
    if (r.is_empty())
      CONSTRUCT_MSG_AND_THROW("Delete empty range from range map");

    std::vector<std::pair<Range<T>, V>> ranges;
    getDeletionRelevantRanges(map_, r, ranges);
    // Each relevant range may lead to changes to the existing underlying
    // treemap data structure.
    // We iterate over the collected relevant ranges to collect and apply
    // these changes.
    T ds = r.get_start();
    T de = r.get_end();
    RBTree<Range<T>, V> tmpmap = map_;
    for (auto &p : ranges) {
      Range<T> rr = p.first;
      V rv = p.second;
      assert(r.overlaps(rr));
      T rrs = rr.get_start();
      T rre = rr.get_end();
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
  // stored in the map
  bool overlaps(Range<T> r) const { return overlaps(map_, r); }

  void print(std::ostream &os) {
    os << "--------------------------" << std::endl;
    forEach(map_, [&os](Range<T> x, V v) {
      os << "[ " << x.get_start() << ".." << x.get_end() << " ) -> " << v
         << std::endl;
    });
    os << "--------------------------" << std::endl;
  }
};

template <class T, class V, class I>
RangeMap<T, V> inserted(RangeMap<T, V> m, I it, I end) {
  if (it == end)
    return m;
  auto key = it->first;
  V val = it->second;
  auto m1 = m.inserted(key, val);
  return inserted(m1, ++it, end);
}

template <class T, class V>
RangeMap<T, V> concat(RangeMap<T, V> const &a, RangeMap<T, V> const &b) {
  RangeMap<T, V> res = a;
  forEach(
      b.getTreeMap(),
      [&res, &a](Range<T> const &x, V const &v) {
        if (!a.overlaps(x))
          res = res.inserted(x, v);
        else
          CONSTRUCT_MSG_AND_THROW(
              "Overlapping key ranges in map concatenation");
      });
  return res;
}

#endif // RANGEMAP_HEADER_H
