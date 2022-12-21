#ifndef RANGEMAP_HEADER_H
#define RANGEMAP_HEADER_H

#include "RBTree.h"

#include <cassert>
#include <stdexcept>

template<class T, class V>
class RangeMap
{
public:
    using Range = std::pair<T, T>;

private:
    RBTree<Range, V> _map;

    // Returns true if range t contains k
    bool range_contains(Range r, T k) const
    {
        T start = r.first;
        T end = r.second;
        return k >= start && k < end;
    }

    bool range_is_empty(Range r) const
    {
        T start = r.first;
        T end = r.second;
        return start >= end;
    }

    // Returns true if ranges r1 and r2 overlap.
    bool ranges_overlap(Range r1, Range r2) const
    {
        T r1start = r1.first;
        T r1end = r1.second;
        T r2start = r2.first;
        T r2end = r2.second;
        if (r2end <= r1start) return false;
        else if (r1end <= r2start) return false;
        return true;
    }

    // Returns true if ranges r1 and r2 overlap or are adjacent, i.e., share a
    // bound, either inclusive or exclusive.
    bool ranges_are_relevant(Range r1, Range r2) const
    {
        T r1start = r1.first;
        T r1end = r1.second;
        T r2start = r2.first;
        T r2end = r2.second;
        if (r2end < r1start) return false;
        else if (r1end < r2start) return false;
        return true;
    }

    std::optional<std::pair<Range, V> > getKeyValue(RBTree<Range, V> t, T k) const
     {
        if (t.isEmpty())
            return std::optional<std::pair<Range, V> >();
        Range r = t.root();
        if (range_contains(r, k))
            return std::make_optional<std::pair<Range, V> >(std::make_pair(r, t.rootVal()));
        T start = r.first;
        if (k < start)
            return getKeyValue(t.left(), k);
        T end = r.second;
        if (k >= end)
            return getKeyValue(t.right(), k);
        assert(false);
    }

    // Gather all <Range, V> pairs in t that are overlapping or directly
    // adjacent (share a boundary) with range r, in v.
    void getInsertionRelevantRanges(RBTree<Range, V> t, Range r, std::vector<std::pair<Range, V> > &v)
    {
        if (t.isEmpty())
            return;
        T start = r.first;
        T end = r.second;
        T rstart = t.root().first;
        T rend = t.root().second;
        if (rend < start)
        {
            // The candidate range is to the left of our target range, and does
            // not share a boundary. It is not relevant. Continue looking for
            // relevant ranges to the right of the candidate range.
            getInsertionRelevantRanges(t.right(), r, v);
        }
        else if (end < rstart)
        {
            // The candidate range is to the right of our target range, and
            // does not share a boundary. It is not relevant. Continue looking
            // for relevant ranges to the left of the candidate range.
            getInsertionRelevantRanges(t.left(), r, v);
        }
        else if (start <= rstart && rend <= end)
        {
            // Our target range contains the candidate range, and may share its
            // boundaries (i.e. equal or larger). The candidate range is
            // relevant, and there may be relevant ranges in both left and
            // right directions.
            getInsertionRelevantRanges(t.left(), r, v);
            v.push_back(std::make_pair(t.root(), t.rootVal()));
            getInsertionRelevantRanges(t.right(), r, v);
        }
        else if (rstart < start && end < rend)
        {
            // The candidate range fully contains the target range, i.e. is
            // larger. It is the only relevant range, no need to look for more.
            v.push_back(std::make_pair(t.root(), t.rootVal()));
        }
        else if (rstart < start)
        {
            // The candidate range overlapps with the target range on the left
            // side, but does not fully contain it. It is relevant, and we need
            // to continue looking for relevant ranges to the right of the
            // candidate range.
            v.push_back(std::make_pair(t.root(), t.rootVal()));
            getInsertionRelevantRanges(t.right(), r, v);
        }
        else if (end < rend)
        {
            // The candidate range overlapps with the target range on the right
            // side, but does not fully contain it. It is relevant, and we need
            // to continue looking for relevant ranges to the left of the
            // candidate range.
            getInsertionRelevantRanges(t.left(), r, v);
            v.push_back(std::make_pair(t.root(), t.rootVal()));
        }
    }

    // Gather all <Range, V> pairs in t that are overlapping with range r, in v.
    void getDeletionRelevantRanges(RBTree<Range, V> t, Range r, std::vector<std::pair<Range, V> > &v)
    {
        if (t.isEmpty())
            return;
        T start = r.first;
        T end = r.second;
        T rstart = t.root().first;
        T rend = t.root().second;
        if (rend <= start)
        {
            // The candidate range is to the left of our target range, and may
            // share a boundary. It is not relevant. Continue looking for
            // relevant ranges to the right of the candidate range.
            getDeletionRelevantRanges(t.right(), r, v);
        }
        else if (end <= rstart)
        {
            // The candidate range is to the right of our target range, and
            // may share a boundary. It is not relevant. Continue looking for
            // relevant ranges to the left of the candidate range.
            getDeletionRelevantRanges(t.left(), r, v);
        }
        else if (start < rstart && rend < end)
        {
            // Our target range fully contains the candidate range, i.e. is
            // larger. The candidate range is relevant, and there may be
            // relevant ranges in both left and right directions.
            getDeletionRelevantRanges(t.left(), r, v);
            v.push_back(std::make_pair(t.root(), t.rootVal()));
            getDeletionRelevantRanges(t.right(), r, v);
        }
        else if (rstart <= start && end <= rend)
            // The candidate range contains the target range, i.e. is equal or
            // larger. It is the only relevant range, no need to look for more.
        {
            v.push_back(std::make_pair(t.root(), t.rootVal()));
        }
        else if (rstart <= start)
        {
            // The candidate range overlapps with the target range on the left
            // side, but does not fully contain it. It is relevant, and we need
            // to continue looking for relevant ranges to the right of the
            // candidate range.
            v.push_back(std::make_pair(t.root(), t.rootVal()));
            getDeletionRelevantRanges(t.right(), r, v);
        }
        else if (end <= rend)
        {
            // The candidate range overlapps with the target range on the right
            // side, but does not fully contain it. It is relevant, and we need
            // to continue looking for relevant ranges to the left of the
            // candidate range.
            getDeletionRelevantRanges(t.left(), r, v);
            v.push_back(std::make_pair(t.root(), t.rootVal()));
        }
    }
public:
    RangeMap() : _map(RBTree<Range, V>()) {}

    RangeMap(RBTree<Range, V> t) : _map(t) {}

    template<class I>
    RangeMap(I b, I e)
    {
        RangeMap m = RangeMap();
        for_each(b, e, [&m](std::pair<Range, V> const & p){
            m = m.inserted(p.first, p.second);
        });
        _map = m._map;
    }

    // Return the number of key ranges in the map.
    size_t size() const
    {
        return _map.size();
    }

    // Return true if a range in the map contains the key.
    bool containsKey(T k) const
    {
        std::optional<std::pair<Range, V> > opt = getKeyValue(k);
        if (opt.has_value())
            return true;
        return false;
    }

    // If the key is contained in any range in the map, return the value
    // associated with the key.
    std::optional<V> getValue(T k) const
    {
        std::optional<std::pair<Range, V> > opt = getKeyValue(k);
        if (opt.has_value())
            return std::make_optional<V>(opt.value().second);
        return std::optional<V>();
    }
    
    // If the key is contained in any range in the map, return the key range-
    // value pair associated with the key.
    std::optional<std::pair<Range, V> > getKeyValue(T k) const
    {
       return getKeyValue(_map, k);
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
    RangeMap inserted(Range r, V v)
    {
        // Empty ranges do not make sense here.
        if (range_is_empty(r))
            CONSTRUCT_MSG_AND_THROW("Insert empty range in range map");

        std::vector<std::pair<Range, V> > ranges;
        getInsertionRelevantRanges(_map, r, ranges);
        // Each relevant range may lead to changes to the existing underlying
        // treemap data structure, as well as the bounds of the target inserted
        // range. We iterate over the collected relevant ranges to collect
        // these changes.
        T is = r.first;
        T ie = r.second;
        RBTree<Range, V> tmpmap = _map;
        for (auto & p: ranges)
        {
            Range rr = p.first;
            V rv = p.second;
            assert(ranges_are_relevant(r, rr));
            T rrs = rr.first;
            T rre = rr.second;

            if (v == rv)
            {
                // The inserted value is the same as the value stored in
                // the relevant range.
                // Adjust the bounds of the inserted range as needed, and delete
                // the relevant range.
                is = rrs < is ? rrs : is;
                ie = rre > ie ? rre : ie;
                tmpmap = tmpmap.deleted(rr);
            }
            else
            {
                // The inserted value is different from the value stored
                // in the relevant range.
                if (ranges_overlap(r, rr))
                {
                    // The ranges overlap.
                    // Delete the relevant range, and insert up to two ranges to
                    // the left/right of the inserted range, as needed to
                    // account for the bounds of the relevant range.
                    tmpmap = tmpmap.deleted(rr);
                    if (rrs < is)
                    {
                        tmpmap = tmpmap.inserted(std::make_pair(rrs, is), rv);
                    }
                    if (rre > ie)
                    {
                        tmpmap = tmpmap.inserted(std::make_pair(ie, rre), rv);
                    }
                }
                else
                {
                    // The ranges do not overlap. NOOP.
                }
            }
        }

        // Finally, insert a range with the properly adjusted bounds and the
        // new value. Create a new range map.
        tmpmap = tmpmap.inserted(std::make_pair(is, ie), v);
        return RangeMap(tmpmap);
    }

    /* Given a range to be removed and a map, return a map that does not
     * contain the removed range or any part of it.
     * If the range to be removed partially overlaps with any ranges in the
     * map, then the boundaries of these ranges are adjusted in the resulting
     * map so that they do not overlap with the removed range.
     */
    RangeMap deleted(Range r)
    {
        // Empty ranges do not make sense here.
        if (range_is_empty(r))
            CONSTRUCT_MSG_AND_THROW("Delete empty range from range map");

        std::vector<std::pair<Range, V> > ranges;
        getDeletionRelevantRanges(_map, r, ranges);
        // Each relevant range may lead to changes to the existing underlying
        // treemap data structure.
        // We iterate over the collected relevant ranges to collect and apply
        // these changes.
        T ds = r.first;
        T de = r.second;
        RBTree<Range, V> tmpmap = _map;
        for (auto & p: ranges)
        {
            Range rr = p.first;
            V rv = p.second;
            assert(ranges_overlap(r, rr));
            T rrs = rr.first;
            T rre = rr.second;
            tmpmap = tmpmap.deleted(rr);
            if (rrs < ds)
            {
                tmpmap = tmpmap.inserted(std::make_pair(rrs, ds), rv);
            }
            if (rre > de)
            {
                tmpmap = tmpmap.inserted(std::make_pair(de, rre), rv);
            }
        }
        return RangeMap(tmpmap);
    }

    void print()
    {
        std::cout << "--------------------------" << std::endl;
        forEach(_map, [](Range x, V v)
        {
            std::cout << "[ " << x.first
                      << ".." << x.second
                      << " ) -> " << v << std::endl;
        });
        std::cout << "--------------------------" << std::endl;
    }
};

template<class T, class V, class I>
RangeMap<T, V> inserted(RangeMap<T, V> m, I it, I end)
{
    if (it == end)
        return m;
    std::pair<T, T> key = it->first;
    V val = it->second;
    auto m1 = m.inserted(key, val);
    return inserted(m1, ++it, end);
}



#endif // RANGEMAP_HEADER_H
