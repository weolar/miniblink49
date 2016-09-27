// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/paint/GridPainter.h"

#include "core/layout/LayoutGrid.h"
#include "core/paint/BlockPainter.h"
#include "core/paint/PaintInfo.h"

namespace blink {

static GridSpan dirtiedGridAreas(const Vector<LayoutUnit>& coordinates, LayoutUnit start, LayoutUnit end)
{
    // This function does a binary search over the coordinates.
    // This doesn't work with grid items overflowing their grid areas, but that is managed with m_gridItemsOverflowingGridArea.

    size_t startGridAreaIndex = std::upper_bound(coordinates.begin(), coordinates.end() - 1, start) - coordinates.begin();
    if (startGridAreaIndex > 0)
        --startGridAreaIndex;

    size_t endGridAreaIndex = std::upper_bound(coordinates.begin() + startGridAreaIndex, coordinates.end() - 1, end) - coordinates.begin();
    if (endGridAreaIndex > 0)
        --endGridAreaIndex;

    return GridSpan(startGridAreaIndex, endGridAreaIndex);
}

class GridItemsSorter {
public:
    bool operator()(const std::pair<LayoutBox*, size_t>& firstChild, const std::pair<LayoutBox*, size_t>& secondChild) const
    {
        if (firstChild.first->style()->order() != secondChild.first->style()->order())
            return firstChild.first->style()->order() < secondChild.first->style()->order();

        return firstChild.second < secondChild.second;
    }
};

void GridPainter::paintChildren(const PaintInfo& paintInfo, const LayoutPoint& paintOffset)
{
    ASSERT(!m_layoutGrid.needsLayout());

    LayoutRect localPaintInvalidationRect = LayoutRect(paintInfo.rect);
    localPaintInvalidationRect.moveBy(-paintOffset);

    GridSpan dirtiedColumns = dirtiedGridAreas(m_layoutGrid.columnPositions(), localPaintInvalidationRect.x(), localPaintInvalidationRect.maxX());
    GridSpan dirtiedRows = dirtiedGridAreas(m_layoutGrid.rowPositions(), localPaintInvalidationRect.y(), localPaintInvalidationRect.maxY());

    Vector<std::pair<LayoutBox*, size_t>> gridItemsToBePainted;

    for (GridSpan::iterator row = dirtiedRows.begin(); row != dirtiedRows.end(); ++row) {
        for (GridSpan::iterator column = dirtiedColumns.begin(); column != dirtiedColumns.end(); ++column) {
            const Vector<LayoutBox*, 1>& children = m_layoutGrid.gridCell(row.toInt(), column.toInt());
            for (auto* child : children)
                gridItemsToBePainted.append(std::make_pair(child, m_layoutGrid.paintIndexForGridItem(child)));
        }
    }

    for (auto* item: m_layoutGrid.itemsOverflowingGridArea()) {
        if (item->frameRect().intersects(localPaintInvalidationRect))
            gridItemsToBePainted.append(std::make_pair(item, m_layoutGrid.paintIndexForGridItem(item)));
    }

    // Sort grid items following order-modified document order.
    // See http://www.w3.org/TR/css-flexbox/#order-modified-document-order
    std::stable_sort(gridItemsToBePainted.begin(), gridItemsToBePainted.end(), GridItemsSorter());

    LayoutBox* previous = 0;
    for (const auto& gridItemAndPaintIndex : gridItemsToBePainted) {
        // We might have duplicates because of spanning children are included in all cells they span.
        // Skip them here to avoid painting items several times.
        LayoutBox* current = gridItemAndPaintIndex.first;
        if (current == previous)
            continue;

        BlockPainter(m_layoutGrid).paintChildAsInlineBlock(*current, paintInfo, paintOffset);
        previous = current;
    }
}

} // namespace blink
