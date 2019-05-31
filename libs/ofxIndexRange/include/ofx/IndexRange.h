//
// Copyright (c) 2019 Christopher Baker <https://christopherbaker.net>
//
// SPDX-License-Identifier:	MIT
//


#pragma once


#include <vector>
#include <iostream>
#include "json.hpp"


namespace ofx {


/// \brief An unsigned integral index range.
class IndexRange
{
public:
    /// \brief Create a default empty range with location 0 and length 0.
    IndexRange();

    /// \brief Create an index range with the given location and length.
    /// \param location The starting location of the range.
    /// \param size The size of the range.
    IndexRange(std::size_t location, std::size_t size);

    /// \returns the location.
    std::size_t getMin() const;

    /// \brief Set the minimum location, keeping max.
    ///
    /// Will adjust location and size to keep the current max and set a new min.
    ///
    /// If new min > current max, min and max will be equal to \p value and size
    /// will be set to zero.
    ///
    /// \param value The new location of the minimum.
    void setMin(std::size_t value);

    /// \brief Get the maximum value in the range.
    /// \note This value has overflowed when high() < low();
    /// \returns the sum of location + size.
    std::size_t getMax() const;

    /// \brief Set the max location, keeping min.
    ///
    /// Will adjust size and keep the current min.
    ///
    /// If new max < current min, min and max will be equal to \p value and size
    /// will become zero.
    ///
    /// \param value The new location of the maximum.
    void setMax(std::size_t value);

    /// \returns true if max() < min().
    bool overflows() const;

    /// \returns true if the size is 0.
    bool empty() const;

    /// \brief Determine if a this range contains the location.
    /// \param location The location to test.
    /// \returns true if the location is within the range.
    bool contains(std::size_t location) const;

    /// \brief Determine if a this range contains the range.
    /// \param other The range to test.
    /// \returns true if the range contains the other range.
    bool contains(const IndexRange& other) const;

    /// \brief Determine if a range is adjacent on the low side of this range.
    ///
    /// Adjacent ranges may not overlap. Adjacent ranges must have sizes > 0.
    ///
    /// \param other The other range to check.
    /// \returns true if the other range is adjacent on the low side.
    bool isHighAdjacentTo(const IndexRange& other) const;

    /// \brief Determine if a range is adjacent on the high side of this range.
    ///
    /// Adjacent ranges may not overlap. Adjacent ranges must have sizes > 0.
    ///
    /// \param other The other range to check.
    /// \returns true if the other range is adjacent on the high side.
    bool isLowAdjacentTo(const IndexRange& other) const;

    /// \brief Determine if a range is adjacent on either side of this range.
    /// \param other The other range to check.
    /// \returns true if adjacentHigh(other) || adjacentLow(other).
    bool isAdjacentTo(const IndexRange& other) const;

    /// \brief Determine if this range intersects with the other.
    /// \param other The other range to check.
    /// \returns true if the ranges intersect.
    bool intersects(const IndexRange& other) const;

    /// \brief Determine the intersection of this range and the other.
    /// \param other The other range to check.
    /// \returns the intersection of the ranges. An intersection with length 0
    /// means the ranges don't intersect.
    IndexRange intersectionWith(const IndexRange& other) const;

    /// \brief Determine the union of this range and the other.
    /// \param other The other range to check.
    /// \returns the union of the ranges. A union with length 0 means both
    /// ranges were empty.
    IndexRange unionWith(const IndexRange& other) const;

    /// \brief Merge this IndexRange with another IndexRange.
    ///
    /// If this IndexRange and the other intersect or are adjacent to each other
    /// their union is returned.
    ///
    /// \param other The range to attempt a merge with.
    /// \returns a valid IndexRange if ranges intersect.
    IndexRange mergeWith(const IndexRange& other) const;

    /// \brief If an IndexRange is in an overflow state, truncate and return the remainder.
    /// \returns the remainder of an overflow state or an IndexRange with size == 0.
    IndexRange clearOverflow();

    bool operator == (const IndexRange& other) const;
    bool operator != (const IndexRange& other) const;
    bool operator <  (const IndexRange& other) const;
    bool operator <= (const IndexRange& other) const;
    bool operator >  (const IndexRange& other) const;
    bool operator >= (const IndexRange& other) const;

    /// \brief Create an IndexRange from an inclusive interval [lower, upper].
    ///
    /// An index range is constructed as follows:
    ///
    ///     if (upper < lower)
    ///         std::swap(lower, upper);
    ///
    ///     return IndexRange(lower, upper - lower + 1);
    ///
    /// \param lower The low side of the interval.
    /// \param upper The high side of the interval (inclusive).
    /// \returns a valid IndexRange representing the inclusive interval.
    static IndexRange fromInterval(std::size_t lower, std::size_t upper);

    /// \brief Create an IndexRange from an exclusive interval [min, max).
    ///
    /// An index range is constructed as follows:
    ///
    ///     if (max < min)
    ///         std::swap(min, max);
    ///
    ///     return IndexRange(min, max - min);
    ///
    /// \param min The low side of the interval.
    /// \param max The high side of the interval (exclusive).
    /// \returns a valid IndexRange representing the inclusive interval.
    static IndexRange fromExclusiveInterval(std::size_t min, std::size_t max);

    /// \brief Alias for std::numeric_limits<std::size_t>::max().
    static const std::size_t MAX;

    /// \brief Alias for std::numeric_limits<std::size_t>::lowest().
    static const std::size_t LOWEST;

    /// \brief The largest range IndexRange(0, MAX).
    static const IndexRange MAXIMUM_RANGE;

    /// \brief The starting location of the Range.
    std::size_t location = 0;

    /// \brief The size of the Range.
    std::size_t size = 0;

    friend std::ostream& operator << (std::ostream& os, const IndexRange& range);
    friend std::istream& operator >> (std::istream& is, IndexRange& range);

};


inline std::ostream& operator << (std::ostream& os, const IndexRange& range)
{
    os << "{" << range.location << "," << range.size << "}";
    return os;
}


inline std::istream& operator >> (std::istream& is, IndexRange& range)
{
    is.ignore(1);
    is >> range.location;
    is.ignore(1);
    is >> range.size;
    is.ignore(1);
    return is;
}


inline void to_json(nlohmann::json& j, const IndexRange& v)
{
    j = { v.location, v.size };
}


inline void from_json(const nlohmann::json& j, IndexRange& v)
{
    v.location = j[0];
    v.size = j[1];
}


} // namespace ofx
