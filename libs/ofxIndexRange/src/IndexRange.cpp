//
// Copyright (c) 2019 Christopher Baker <https://christopherbaker.net>
//
// SPDX-License-Identifier:	MIT
//


#include "ofx/IndexRange.h"


namespace ofx {


const std::size_t IndexRange::MAX(std::numeric_limits<std::size_t>::max());
const std::size_t IndexRange::LOWEST(std::numeric_limits<std::size_t>::lowest());
const IndexRange IndexRange::MAXIMUM_RANGE(0, IndexRange::MAX);


IndexRange::IndexRange(): IndexRange(0, 0)
{
}


IndexRange::IndexRange(std::size_t _location, std::size_t _size):
    location(_location),
    size(_size)
{
}


std::size_t IndexRange::getMin() const
{
    return location;
}


void IndexRange::setMin(std::size_t value)
{
    std::size_t max = location + size;

    if (value > max)
    {
        location = value;
        size = 0;
    }
    else
    {
        size = max - value;
        location = value;
    }
}


std::size_t IndexRange::getMax() const
{
    return location + size;
}


void IndexRange::setMax(std::size_t value)
{
    if (value < location)
    {
        location = value;
        size = 0;
    }
    else
    {
        size = value - location;
    }
}


bool IndexRange::overflows() const
{
    return (location + size) < location;
}


bool IndexRange::empty() const
{
    return 0 == size;
}


bool IndexRange::contains(std::size_t i) const
{
    return i >= location && (i - location) < size;
}


bool IndexRange::contains(const IndexRange& other) const
{
    return contains(other.location)
        && contains(other.location + other.size - 1);
}


bool IndexRange::isHighAdjacentTo(const IndexRange& other) const
{
    return (other.location + other.size) == (location);
}


bool IndexRange::isLowAdjacentTo(const IndexRange& other) const
{
    return (other.location) == (location + size);
}


bool IndexRange::isAdjacentTo(const IndexRange& other) const
{
    return (other.location + other.size) == (location       )
        || (other.location             ) == (location + size);
}


bool IndexRange::intersects(const IndexRange& other) const
{
    return intersectionWith(other).size != 0;
}


IndexRange IndexRange::intersectionWith(const IndexRange& other) const
{
    std::size_t _thisMax = location + size;
    std::size_t _otherMax = other.location + other.size;
    std::size_t _minMax = std::min(_thisMax, _otherMax);

    if (other.location <= location && location < _otherMax)
        return IndexRange(location, _minMax - location);
    else if (location <= other.location && other.location < _thisMax)
        return IndexRange(other.location, _minMax - other.location);
    return IndexRange();
}


IndexRange IndexRange::unionWith(const IndexRange& other) const
{
    IndexRange result;
    result.location = std::min(location, other.location);
    result.size = std::max(getMax(), other.getMax()) - result.location;
    return result;
}


IndexRange IndexRange::mergeWith(const IndexRange& other) const
{
    if (intersects(other) || isAdjacentTo(other))
        return unionWith(other);

    return IndexRange();
}


IndexRange IndexRange::clearOverflow()
{
    IndexRange result;

    std::size_t max = location + size;

    if (max < location)
    {
        std::size_t over = MAX + max + 1 + 1;
        result.location = 0;
        result.size = over;
        size -= over;
    }

    return result;
}


bool IndexRange::operator == (const IndexRange& rhs) const
{
    return location == rhs.location
        &&     size == rhs.size;
}


bool IndexRange::operator != (const IndexRange& rhs) const
{
    return !(*this == rhs);
}


bool IndexRange::operator < (const IndexRange& rhs) const
{
    if (location < rhs.location)
        return true;

    if (rhs.location < location)
        return false;

    if (location + size < rhs.location + rhs.size)
        return true;

    return false;
}


bool IndexRange::operator <= (const IndexRange& rhs) const
{
    return !(rhs < *this);
}


bool IndexRange::operator > (const IndexRange& rhs) const
{
    return rhs < *this;
}


bool IndexRange::operator >= (const IndexRange& rhs) const
{
    return !(*this < rhs);
}


IndexRange IndexRange::fromInterval(std::size_t lower, std::size_t upper)
{
    if (upper < lower)
        std::swap(lower, upper);

    return IndexRange(lower, upper - lower + 1);
}


IndexRange IndexRange::fromExclusiveInterval(std::size_t min, std::size_t max)
{
    if (max < min)
        std::swap(min, max);

    return IndexRange(min, max - min);
}


} // namespace ofx
