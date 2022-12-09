#ifndef RANGEMAP_HEADER_H
#define RANGEMAP_HEADER_H

#include "RBTree.h"

#include <cassert>

template<class T, class V>
class RangeMap
{
public:
    using Range = std::pair<T, T>;

private:
    RBTree<Range, V> _map;

//    // Granting access to left(), right()
//    friend class RBTree;

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

    void getRelevantRanges (RBTree<Range, V> t, Range r, std::vector<Range> &v)
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
        else if ( end < rend)
        {
            getRelevantRanges(t.left(), r, v);
            v.push_back(t.root());
        }
    }
public:
    RangeMap() : _map(RBTree<Range, V>()) {}

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
        // TODO: throw an exception here instead
        assert(r.first < r.second);

        std::vector<Range> ranges;
        getRelevantRanges(_map, r, ranges);
        

        return RangeMap();
    }

    RangeMap deleted(Range r)
    {
        return RangeMap();
    }
};



#endif // RANGEMAP_HEADER_H
