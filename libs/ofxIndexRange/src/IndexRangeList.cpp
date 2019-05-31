//
// Copyright (c) 2019 Christopher Baker <https://christopherbaker.net>
//
// SPDX-License-Identifier:    MIT
//


#include "ofx/IndexRangeList.h"
#include "ofLog.h"


namespace ofx {


IndexRangeList::IndexRangeList()
{
}


IndexRangeList::IndexRangeList(const std::vector<IndexRange>& ranges)
{
    for (auto& range: ranges)
        add(range);
}


IndexRangeList::~IndexRangeList()
{
}


void IndexRangeList::add(const IndexRange& _range)
{
    IndexRange range = validate(_range);

    if (range.empty())
        return;

    _ranges.push_back(range);
    _sorted = false;
}


void IndexRangeList::remove(const IndexRange& _range)
{
    IndexRange range = validate(_range);

    if (range.empty())
        return;

    _sort();

    bool foundIntersection = false;
    auto iter = _ranges.begin();
    while (iter != _ranges.end())
    {
        IndexRange intersection = range.intersectionWith(*iter);

        if (intersection.size != 0)
        {
            foundIntersection = true;

            if (intersection.getMin() == iter->getMin())
            {
                // Lower overlap.
                if (intersection.getMax() < iter->getMax())
                {
                    iter->setMin(intersection.getMax());
                    ++iter;
                }
                else
                {
                    // Full overlap.
                    iter = _ranges.erase(iter);
                }
            }
            else if (intersection.getMax() == iter->getMax())
            {
                iter->setMax(intersection.getMin());
                ++iter;
            }
            else
            {
                IndexRange hi = *iter;
                hi.setMin(intersection.getMax());
                iter->setMax(intersection.getMin());
                iter = _ranges.insert(iter, hi);
            }
        }
        else if (foundIntersection)
        {
            // We previously found an intersection and now didn't find one,
            // which means we're past since we sorted.
            break;
        }
        else
        {
            ++iter;
        }
    }

    // TODO: we are still sorted?
    _sorted = false;
}


void IndexRangeList::insert(const IndexRange& _range)
{
    IndexRange range = validate(_range);

    if (range.empty())
        return;

    _sort();

    auto iter = _ranges.begin();

    while (iter != _ranges.end())
    {
        auto& curr = *(iter);

        bool overflow = false;

        if (curr.contains(range.location))
        {
            curr.size += range.size;
        }
        else if (curr.location > range.location)
        {
            if (curr.location + range.size < curr.location)
                overflow = true;
            else
                curr.location += range.size;
        }

        // Clear overflow, if present.
        // TODO: Preserve overflow?
        curr.clearOverflow();

        if (overflow)
            iter = _ranges.erase(iter);
        else
            ++iter;
    }

    _sorted = false;
}


void IndexRangeList::erase(const IndexRange& _range)
{
    IndexRange range = validate(_range);

    if (range.empty())
        return;

//    std::cout << "RANGE: " << range << std::endl;

    // No need to sort because all need to be checked, unless we are starting from the high side?

    auto iter = _ranges.begin();

    while (iter != _ranges.end())
    {
        // Something will happen.
        if (range.getMin() < iter->getMax())
        {
            if (range.getMax() >= iter->getMax())
                iter->setMax(range.getMin());
            else if (range.getMin() >= iter->getMin())
                iter->size -= std::min(iter->size, range.size);
            else if (range.getMax() <= iter->getMin())
                iter->location -= std::min(iter->location, range.size);
            else if (range.getMax() < iter->getMax())
            {
                iter->setMin(range.getMax());
                iter->location -= std::min(iter->location, range.size);
            }
        }
        else
        {
            // Nothing will change.
        }

        if (0 == iter->size)
            iter = _ranges.erase(iter);
        else
            ++iter;
    }
}


void IndexRangeList::clear()
{
    _ranges.clear();
    _sorted = true;
}


std::size_t IndexRangeList::size() const
{
    _sort();
    return _ranges.size();
}


bool IndexRangeList::empty() const
{
    _sort();
    return _ranges.empty();
}


void IndexRangeList::_sort() const
{
    if (!_sorted)
    {
        // Nothing to sort otherwise, and iterator math will fail.
        if (_ranges.size() > 1)
        {
            std::sort(_ranges.begin(), _ranges.end());

            auto iter = _ranges.begin() + 1;

            while (iter != _ranges.end())
            {
                auto& last = *(iter - 1); // Last range.
                auto& curr = *(iter);     // Current range.

                IndexRange merged = last.mergeWith(curr);

                if (merged.size != 0)
                {
                    last = merged;
                    iter = _ranges.erase(iter);
                }
                else
                {
                    ++iter;
                }
            }
        }

        _sorted = true;
    }
}


std::vector<IndexRange> IndexRangeList::ranges() const
{
    _sort();
    return _ranges;
}


IndexRange IndexRangeList::validate(const IndexRange& range)
{
    IndexRange result = range;
    result.clearOverflow();
    return result;
}


} // namespace ofx
