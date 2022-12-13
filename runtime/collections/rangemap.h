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

    // Returns true if range c overlaps or is adjacent, i.e., shares a bound
    // either inclusive or exclusive with range r.
    bool range_is_relevant(Range r, Range c) const
    {
        T start = r.first;
        T end = r.second;
        T cstart = c.first;
        T cend = c.second;
        if (cend < start) return false;
        else if (end < cstart) return false;
        assert(start <= cend);
        assert(cstart <= end);
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

    void getRelevantRanges(RBTree<Range, V> t, Range r, std::vector<std::pair<Range, V> > &v)
    {
        if (t.isEmpty())
            return;
        T start = r.first;
        T end = r.second;
        T rstart = t.root().first;
        T rend = t.root().second;
        if (rend < start)
            getRelevantRanges(t.right(), r, v);
        else if (end < rstart)
            getRelevantRanges(t.left(), r, v);
        else if (start <= rstart && rend <= end)
        {
            getRelevantRanges(t.left(), r, v);
            v.push_back(std::make_pair(t.root(), t.rootVal()));
            getRelevantRanges(t.right(), r, v);
        }
        else if (rstart < start && end < rend)
        {
            v.push_back(std::make_pair(t.root(), t.rootVal()));
        }
        else if (rstart < start)
        {
            v.push_back(std::make_pair(t.root(), t.rootVal()));
            getRelevantRanges(t.right(), r, v);
        }
        else if (end < rend)
        {
            getRelevantRanges(t.left(), r, v);
            v.push_back(std::make_pair(t.root(), t.rootVal()));
        }
    }

    void getRelevantRanges(RBTree<Range, V> t, Range r, std::vector<Range> &v)
    {
        if (t.isEmpty())
            return;
        T start = r.first;
        T end = r.second;
        T rstart = t.root().first;
        T rend = t.root().second;
        if (rend < start)
            getRelevantRanges(t.right(), r, v);
        else if (end < rstart)
            getRelevantRanges(t.left(), r, v);
        else if (start <= rstart && rend <= end)
        {
            getRelevantRanges(t.left(), r, v);
            v.push_back(t.root());
            getRelevantRanges(t.right(), r, v);
        }
        else if (rstart < start && end < rend)
        {
            v.push_back(t.root());
        }
        else if (rstart < start)
        {
            v.push_back(t.root());
            getRelevantRanges(t.right(), r, v);
        }
        else if (end < rend)
        {
            getRelevantRanges(t.left(), r, v);
            v.push_back(t.root());
        }
    }
public:
    RangeMap() : _map(RBTree<Range, V>()) {}

    RangeMap(RBTree<Range, V> t) : _map(t) {}

    size_t size() const
    {
        return _map.size();
    }

    bool containsKey(T k) const
    {
        std::optional<std::pair<Range, V> > opt = getKeyValue(k);
        if (opt.has_value())
            return true;
        return false;
    }

    std::optional<V> getValue(T k) const
    {
        std::optional<std::pair<Range, V> > opt = getKeyValue(k);
        if (opt.has_value())
            return std::make_optional<V>(opt.value().second);
        return std::optional<V>();
    }
    
     std::optional<std::pair<Range, V> > getKeyValue(T k) const
     {
        return getKeyValue(_map, k);
     }

    /* Insert a pair of key range and value into the map, and return the
       resulting map.
       If the inserted range partially or completely overlaps any existing
       range in the map, then the existing range (or ranges) will be partially
       or completely replaced by the inserted range.
       If the inserted range either overlaps or is immediately adjacent any
       existing range mapping to the same value, then the ranges will be
       coalesced into a single contiguous range.
    */
    RangeMap inserted(Range r, V v)
    {
        // Empty ranges do not make sense here.
        if (range_is_empty(r))
            CONSTRUCT_MSG_AND_THROW("Insert empty range in range map");

        std::vector<std::pair<Range, V> > ranges;
        getRelevantRanges(_map, r, ranges);

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
            assert(range_is_relevant(r, rr));
            T rrs = rr.first;
            T rre = rr.second;

            if (rrs < is && rre > ie)
            {
                // Relevant range is larger than the inserted range.
                // In this case, there should be only one relevant range.
                if (v == rv)
                {
                    // The inserted value is the same as the value stored in
                    // the relevant range.
                    // Extend the inserted range to the bounds of the relevant
                    // range and delete the relevant range.
                    is = rrs;
                    ie = rre;
                    tmpmap = tmpmap.deleted(rr);
                }
                else
                {
                    // The inserted value is different from the value stored
                    // in the relevant range.
                    // Delete the relevant range, and insert two new ranges
                    // accounting for the difference between the relevant and
                    // the inserted range, containing the value of the relevant
                    // range.
                    tmpmap = tmpmap.deleted(rr);
                    tmpmap = tmpmap.inserted(std::make_pair(rrs, is), rv);
                    tmpmap = tmpmap.inserted(std::make_pair(ie, rre), rv);
                }
            }
            else if (rrs >= is && rre <= ie)
            {
                // Relevant range is contained within the inserted range.
                // Delete relevant range. The value is not important.
                tmpmap = tmpmap.deleted(rr);
            }
            else if (rrs < is && rre <= ie)
            {
                // Relevant range is overlapping with or adjacent to the
                // inserted range on the left side.
                if (v == rv)
                {
                    // The inserted value is the same as the value stored in
                    // the relevant range.
                    // Extend the inserted range left-wise to the start of the
                    // relevant range and delete the relevant range.
                    is = rrs;
                    tmpmap = tmpmap.deleted(rr);
                }
                else
                {
                    // The inserted value is different from the value stored
                    // in the relevant range.
                    // Delete the relevant range, and insert a new range from
                    // the start of the relevant range to the start of the
                    // inserted range, with the value of the relevant range.
                    tmpmap = tmpmap.deleted(rr);
                    tmpmap = tmpmap.inserted(std::make_pair(rrs, is), rv);
                }
            }
            else if (rrs <= is && rre > ie)
            {
                // Relevant range is overlapping with or adjacent to the
                // inserted range on the right side.
                if (v == rv)
                {
                    // The inserted value is the same as the value stored in
                    // the relevant range.
                    // Extend the inserted range right-wise to the end of the
                    // relevant range and delete the relevant range.
                    ie = rre;
                    tmpmap = tmpmap.deleted(rr);
                }
                else
                {
                    // The inserted value is different from the value stored
                    // in the relevant range.
                    // Delete the relevant range, and insert a new range from
                    // the end of the inserted range to the end of the
                    // relevant range, with the value of the relevant range.
                    tmpmap = tmpmap.deleted(rr);
                    tmpmap = tmpmap.inserted(std::make_pair(ie, rre), rv);
                }
            }
        }

        // Finally, insert a range with the properly adjusted bounds and the
        // new value. Create a new range map.
        tmpmap = tmpmap.inserted(std::make_pair(is, ie), v);
        return RangeMap(tmpmap);
    }

    RangeMap deleted(Range r)
    {
        return RangeMap();
    }

    void print()
    {
        forEach(_map, [](Range x, V v)
        {
            std::cout << "[ " << x.first
                      << ".." << x.second
                      << " ) -> " << v << std::endl;
        });
    }
};




#endif // RANGEMAP_HEADER_H
