//
// Copyright (c) 2019 Christopher Baker <https://christopherbaker.net>
//
// SPDX-License-Identifier:    MIT
//


#pragma once


#include "ofx/IndexRange.h"


namespace ofx {


/// \brief A list for working with collections of index ranges.
///
/// Ranges can be added, removed, inserted and erased.
class IndexRangeList
{
public:
    /// \brief Create a default empty IndexRangeList.
    IndexRangeList();

    /// \brief Create an IndexRangeList with the given ranges.
    /// \param ranges The ranges to add.
    IndexRangeList(const std::vector<IndexRange>& ranges);

    /// \brief Destroy the IndexRangeList.
    ~IndexRangeList();

    /// \brief Add the given range to the list.
    ///
    /// If the added range overlaps with an existing range it will be merged.
    ///
    /// The added range will be validated.
    ///
    /// \param range The range to add.
    void add(const IndexRange& range);

    /// \brief Add the given range to the list.
    ///
    /// If the removed range overlaps with an existing range all
    /// intersecting portions will be removed.
    ///
    /// \param range The range to remove.
    void remove(const IndexRange& range);

    /// \brief Expand and shift any matching matching range.
    ///
    /// If a range covers this insertion index, the range's size
    /// will be expanded. Then all subsequent ranges will have their locations
    /// increased by size. Ranges that overflow during this shift will be
    /// removed.
    ///
    /// \param range The range to insert.
    void insert(const IndexRange& range);

    /// \brief Truncate and shift any matching ranges.
    ///
    /// If an range is fully enclosed by the erased section, remove it.
    /// If an range is partially covered by the erased section, truncate
    /// it and shift down any subsequent ranges.
    ///
    /// \param index The erase position.
    /// \param size The size of the erased section.
    void erase(const IndexRange& range);

    /// \brief Clear all ranges.
    void clear();

    /// \returns true if there are no ranges.
    bool empty() const;

    /// \returns the number of ranges defined.
    std::size_t size() const;

    /// \returns the sorted, merged ranges.
    std::vector<IndexRange> ranges() const;

    /// \brief Get valid range.
    ///
    /// All functions in the IndexRangeList use validated ranges.
    ///
    /// A validated range is a range with no overflow.
    ///
    /// \returns a well-formed range.
    static IndexRange validate(const IndexRange& range);

private:
    /// \brief Will sort _ranges.
    void _sort() const;

    /// \brief True if _ranges has been sorted via _sort().
    mutable bool _sorted = false;

    /// \brief The ranges.
    mutable std::vector<IndexRange> _ranges;

};


} // namespace ofx
