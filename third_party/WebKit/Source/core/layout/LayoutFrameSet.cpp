/**
 * Copyright (C) 1999 Lars Knoll (knoll@kde.org)
 *           (C) 2000 Simon Hausmann <hausmann@kde.org>
 *           (C) 2000 Stefan Schimanski (1Stein@gmx.de)
 * Copyright (C) 2004, 2005, 2006 Apple Computer, Inc.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 */

#include "config.h"
#include "core/layout/LayoutFrameSet.h"

#include "core/dom/Document.h"
#include "core/events/MouseEvent.h"
#include "core/frame/LocalFrame.h"
#include "core/html/HTMLDimension.h"
#include "core/html/HTMLFrameSetElement.h"
#include "core/input/EventHandler.h"
#include "core/layout/LayoutFrame.h"
#include "core/layout/LayoutView.h"
#include "core/paint/FrameSetPainter.h"
#include "platform/Cursor.h"
#include "platform/graphics/GraphicsContext.h"

namespace blink {

LayoutFrameSet::LayoutFrameSet(HTMLFrameSetElement* frameSet)
    : LayoutBox(frameSet)
    , m_isResizing(false)
    , m_isChildResizing(false)
{
    setInline(false);
}

LayoutFrameSet::~LayoutFrameSet()
{
}

LayoutFrameSet::GridAxis::GridAxis()
    : m_splitBeingResized(noSplit)
{
}

HTMLFrameSetElement* LayoutFrameSet::frameSet() const
{
    return toHTMLFrameSetElement(node());
}

void LayoutFrameSet::paint(const PaintInfo& paintInfo, const LayoutPoint& paintOffset)
{
    FrameSetPainter(*this).paint(paintInfo, paintOffset);
}

void LayoutFrameSet::computePreferredLogicalWidths()
{
    m_minPreferredLogicalWidth = 0;
    m_maxPreferredLogicalWidth = 0;
    clearPreferredLogicalWidthsDirty();
}

void LayoutFrameSet::GridAxis::resize(int size)
{
    m_sizes.resize(size);
    m_deltas.resize(size);
    m_deltas.fill(0);

    // To track edges for resizability and borders, we need to be (size + 1). This is because a parent frameset
    // may ask us for information about our left/top/right/bottom edges in order to make its own decisions about
    // what to do. We are capable of tainting that parent frameset's borders, so we have to cache this info.
    m_preventResize.resize(size + 1);
    m_allowBorder.resize(size + 1);
}

void LayoutFrameSet::layOutAxis(GridAxis& axis, const Vector<HTMLDimension>& grid, int availableLen)
{
    availableLen = max(availableLen, 0);

    int* gridLayout = axis.m_sizes.data();

    if (grid.isEmpty()) {
        gridLayout[0] = availableLen;
        return;
    }

    int gridLen = axis.m_sizes.size();
    ASSERT(gridLen);

    int totalRelative = 0;
    int totalFixed = 0;
    int totalPercent = 0;
    int countRelative = 0;
    int countFixed = 0;
    int countPercent = 0;

    // First we need to investigate how many columns of each type we have and
    // how much space these columns are going to require.
    for (int i = 0; i < gridLen; ++i) {
        // Count the total length of all of the fixed columns/rows -> totalFixed
        // Count the number of columns/rows which are fixed -> countFixed
        if (grid[i].isAbsolute()) {
            gridLayout[i] = max<int>(grid[i].value(), 0);
            totalFixed += gridLayout[i];
            countFixed++;
        }

        // Count the total percentage of all of the percentage columns/rows -> totalPercent
        // Count the number of columns/rows which are percentages -> countPercent
        if (grid[i].isPercentage()) {
            gridLayout[i] = max<int>(grid[i].value() * availableLen / 100., 0);
            totalPercent += gridLayout[i];
            countPercent++;
        }

        // Count the total relative of all the relative columns/rows -> totalRelative
        // Count the number of columns/rows which are relative -> countRelative
        if (grid[i].isRelative()) {
            totalRelative += max<int>(grid[i].value(), 1);
            countRelative++;
        }
    }

    int remainingLen = availableLen;

    // Fixed columns/rows are our first priority. If there is not enough space to fit all fixed
    // columns/rows we need to proportionally adjust their size.
    if (totalFixed > remainingLen) {
        int remainingFixed = remainingLen;

        for (int i = 0; i < gridLen; ++i) {
            if (grid[i].isAbsolute()) {
                gridLayout[i] = (gridLayout[i] * remainingFixed) / totalFixed;
                remainingLen -= gridLayout[i];
            }
        }
    } else {
        remainingLen -= totalFixed;
    }

    // Percentage columns/rows are our second priority. Divide the remaining space proportionally
    // over all percentage columns/rows. IMPORTANT: the size of each column/row is not relative
    // to 100%, but to the total percentage. For example, if there are three columns, each of 75%,
    // and the available space is 300px, each column will become 100px in width.
    if (totalPercent > remainingLen) {
        int remainingPercent = remainingLen;

        for (int i = 0; i < gridLen; ++i) {
            if (grid[i].isPercentage()) {
                gridLayout[i] = (gridLayout[i] * remainingPercent) / totalPercent;
                remainingLen -= gridLayout[i];
            }
        }
    } else {
        remainingLen -= totalPercent;
    }

    // Relative columns/rows are our last priority. Divide the remaining space proportionally
    // over all relative columns/rows. IMPORTANT: the relative value of 0* is treated as 1*.
    if (countRelative) {
        int lastRelative = 0;
        int remainingRelative = remainingLen;

        for (int i = 0; i < gridLen; ++i) {
            if (grid[i].isRelative()) {
                gridLayout[i] = (max(grid[i].value(), 1.) * remainingRelative) / totalRelative;
                remainingLen -= gridLayout[i];
                lastRelative = i;
            }
        }

        // If we could not evenly distribute the available space of all of the relative
        // columns/rows, the remainder will be added to the last column/row.
        // For example: if we have a space of 100px and three columns (*,*,*), the remainder will
        // be 1px and will be added to the last column: 33px, 33px, 34px.
        if (remainingLen) {
            gridLayout[lastRelative] += remainingLen;
            remainingLen = 0;
        }
    }

    // If we still have some left over space we need to divide it over the already existing
    // columns/rows
    if (remainingLen) {
        // Our first priority is to spread if over the percentage columns. The remaining
        // space is spread evenly, for example: if we have a space of 100px, the columns
        // definition of 25%,25% used to result in two columns of 25px. After this the
        // columns will each be 50px in width.
        if (countPercent && totalPercent) {
            int remainingPercent = remainingLen;
            int changePercent = 0;

            for (int i = 0; i < gridLen; ++i) {
                if (grid[i].isPercentage()) {
                    changePercent = (remainingPercent * gridLayout[i]) / totalPercent;
                    gridLayout[i] += changePercent;
                    remainingLen -= changePercent;
                }
            }
        } else if (totalFixed) {
            // Our last priority is to spread the remaining space over the fixed columns.
            // For example if we have 100px of space and two column of each 40px, both
            // columns will become exactly 50px.
            int remainingFixed = remainingLen;
            int changeFixed = 0;

            for (int i = 0; i < gridLen; ++i) {
                if (grid[i].isAbsolute()) {
                    changeFixed = (remainingFixed * gridLayout[i]) / totalFixed;
                    gridLayout[i] += changeFixed;
                    remainingLen -= changeFixed;
                }
            }
        }
    }

    // If we still have some left over space we probably ended up with a remainder of
    // a division. We cannot spread it evenly anymore. If we have any percentage
    // columns/rows simply spread the remainder equally over all available percentage columns,
    // regardless of their size.
    if (remainingLen && countPercent) {
        int remainingPercent = remainingLen;
        int changePercent = 0;

        for (int i = 0; i < gridLen; ++i) {
            if (grid[i].isPercentage()) {
                changePercent = remainingPercent / countPercent;
                gridLayout[i] += changePercent;
                remainingLen -= changePercent;
            }
        }
    } else if (remainingLen && countFixed) {
        // If we don't have any percentage columns/rows we only have
        // fixed columns. Spread the remainder equally over all fixed
        // columns/rows.
        int remainingFixed = remainingLen;
        int changeFixed = 0;

        for (int i = 0; i < gridLen; ++i) {
            if (grid[i].isAbsolute()) {
                changeFixed = remainingFixed / countFixed;
                gridLayout[i] += changeFixed;
                remainingLen -= changeFixed;
            }
        }
    }

    // Still some left over. Add it to the last column, because it is impossible
    // spread it evenly or equally.
    if (remainingLen)
        gridLayout[gridLen - 1] += remainingLen;

    // now we have the final layout, distribute the delta over it
    bool worked = true;
    int* gridDelta = axis.m_deltas.data();
    for (int i = 0; i < gridLen; ++i) {
        if (gridLayout[i] && gridLayout[i] + gridDelta[i] <= 0)
            worked = false;
        gridLayout[i] += gridDelta[i];
    }
    // if the deltas broke something, undo them
    if (!worked) {
        for (int i = 0; i < gridLen; ++i)
            gridLayout[i] -= gridDelta[i];
        axis.m_deltas.fill(0);
    }
}

void LayoutFrameSet::notifyFrameEdgeInfoChanged()
{
    if (needsLayout())
        return;
    // FIXME: We should only recompute the edge info with respect to the frame that changed
    // and its adjacent frame(s) instead of recomputing the edge info for the entire frameset.
    computeEdgeInfo();
}

void LayoutFrameSet::fillFromEdgeInfo(const FrameEdgeInfo& edgeInfo, int r, int c)
{
    if (edgeInfo.allowBorder(LeftFrameEdge))
        m_cols.m_allowBorder[c] = true;
    if (edgeInfo.allowBorder(RightFrameEdge))
        m_cols.m_allowBorder[c + 1] = true;
    if (edgeInfo.preventResize(LeftFrameEdge))
        m_cols.m_preventResize[c] = true;
    if (edgeInfo.preventResize(RightFrameEdge))
        m_cols.m_preventResize[c + 1] = true;

    if (edgeInfo.allowBorder(TopFrameEdge))
        m_rows.m_allowBorder[r] = true;
    if (edgeInfo.allowBorder(BottomFrameEdge))
        m_rows.m_allowBorder[r + 1] = true;
    if (edgeInfo.preventResize(TopFrameEdge))
        m_rows.m_preventResize[r] = true;
    if (edgeInfo.preventResize(BottomFrameEdge))
        m_rows.m_preventResize[r + 1] = true;
}

void LayoutFrameSet::computeEdgeInfo()
{
    m_rows.m_preventResize.fill(frameSet()->noResize());
    m_rows.m_allowBorder.fill(false);
    m_cols.m_preventResize.fill(frameSet()->noResize());
    m_cols.m_allowBorder.fill(false);

    LayoutObject* child = firstChild();
    if (!child)
        return;

    size_t rows = m_rows.m_sizes.size();
    size_t cols = m_cols.m_sizes.size();
    for (size_t r = 0; r < rows; ++r) {
        for (size_t c = 0; c < cols; ++c) {
            FrameEdgeInfo edgeInfo;
            if (child->isFrameSet())
                edgeInfo = toLayoutFrameSet(child)->edgeInfo();
            else
                edgeInfo = toLayoutFrame(child)->edgeInfo();
            fillFromEdgeInfo(edgeInfo, r, c);
            child = child->nextSibling();
            if (!child)
                return;
        }
    }
}

FrameEdgeInfo LayoutFrameSet::edgeInfo() const
{
    FrameEdgeInfo result(frameSet()->noResize(), true);

    int rows = frameSet()->totalRows();
    int cols = frameSet()->totalCols();
    if (rows && cols) {
        result.setPreventResize(LeftFrameEdge, m_cols.m_preventResize[0]);
        result.setAllowBorder(LeftFrameEdge, m_cols.m_allowBorder[0]);
        result.setPreventResize(RightFrameEdge, m_cols.m_preventResize[cols]);
        result.setAllowBorder(RightFrameEdge, m_cols.m_allowBorder[cols]);
        result.setPreventResize(TopFrameEdge, m_rows.m_preventResize[0]);
        result.setAllowBorder(TopFrameEdge, m_rows.m_allowBorder[0]);
        result.setPreventResize(BottomFrameEdge, m_rows.m_preventResize[rows]);
        result.setAllowBorder(BottomFrameEdge, m_rows.m_allowBorder[rows]);
    }

    return result;
}

void LayoutFrameSet::layout()
{
    ASSERT(needsLayout());

    if (!parent()->isFrameSet() && !document().printing()) {
        setWidth(view()->viewWidth());
        setHeight(view()->viewHeight());
    }

    unsigned cols = frameSet()->totalCols();
    unsigned rows = frameSet()->totalRows();

    if (m_rows.m_sizes.size() != rows || m_cols.m_sizes.size() != cols) {
        m_rows.resize(rows);
        m_cols.resize(cols);
    }

    LayoutUnit borderThickness = frameSet()->border();
    layOutAxis(m_rows, frameSet()->rowLengths(), size().height() - (rows - 1) * borderThickness);
    layOutAxis(m_cols, frameSet()->colLengths(), size().width() - (cols - 1) * borderThickness);

    positionFrames();

    LayoutBox::layout();

    computeEdgeInfo();

    updateLayerTransformAfterLayout();

    clearNeedsLayout();
}

static void clearNeedsLayoutOnHiddenFrames(LayoutBox* frame)
{
    for (; frame; frame = frame->nextSiblingBox()) {
        frame->setWidth(0);
        frame->setHeight(0);
        frame->clearNeedsLayout();
        clearNeedsLayoutOnHiddenFrames(frame->firstChildBox());
    }
}

void LayoutFrameSet::positionFrames()
{
    LayoutBox* child = firstChildBox();
    if (!child)
        return;

    int rows = frameSet()->totalRows();
    int cols = frameSet()->totalCols();

    int borderThickness = frameSet()->border();
    LayoutSize size;
    LayoutPoint position;
    for (int r = 0; r < rows; r++) {
        position.setX(0);
        size.setHeight(m_rows.m_sizes[r]);
        for (int c = 0; c < cols; c++) {
            child->setLocation(position);
            size.setWidth(m_cols.m_sizes[c]);

            // has to be resized and itself resize its contents
            if (size != child->size()) {
                child->setSize(size);
                child->setNeedsLayoutAndFullPaintInvalidation(LayoutInvalidationReason::SizeChanged);
                child->layout();
            }

            position.setX(position.x() + size.width() + borderThickness);

            child = child->nextSiblingBox();
            if (!child)
                return;
        }
        position.setY(position.y() + size.height() + borderThickness);
    }

    // All the remaining frames are hidden to avoid ugly spurious unflowed frames.
    clearNeedsLayoutOnHiddenFrames(child);
}

void LayoutFrameSet::startResizing(GridAxis& axis, int position)
{
    int split = hitTestSplit(axis, position);
    if (split == noSplit || axis.m_preventResize[split]) {
        axis.m_splitBeingResized = noSplit;
        return;
    }
    axis.m_splitBeingResized = split;
    axis.m_splitResizeOffset = position - splitPosition(axis, split);
}

void LayoutFrameSet::continueResizing(GridAxis& axis, int position)
{
    if (needsLayout())
        return;
    if (axis.m_splitBeingResized == noSplit)
        return;
    int currentSplitPosition = splitPosition(axis, axis.m_splitBeingResized);
    int delta = (position - currentSplitPosition) - axis.m_splitResizeOffset;
    if (!delta)
        return;
    axis.m_deltas[axis.m_splitBeingResized - 1] += delta;
    axis.m_deltas[axis.m_splitBeingResized] -= delta;
    setNeedsLayoutAndFullPaintInvalidation(LayoutInvalidationReason::SizeChanged);
}

bool LayoutFrameSet::userResize(MouseEvent* evt)
{
    if (!m_isResizing) {
        if (needsLayout())
            return false;
        if (evt->type() == EventTypeNames::mousedown && evt->button() == LeftButton) {
            FloatPoint localPos = absoluteToLocal(FloatPoint(evt->absoluteLocation()), UseTransforms);
            startResizing(m_cols, localPos.x());
            startResizing(m_rows, localPos.y());
            if (m_cols.m_splitBeingResized != noSplit || m_rows.m_splitBeingResized != noSplit) {
                setIsResizing(true);
                return true;
            }
        }
    } else {
        if (evt->type() == EventTypeNames::mousemove || (evt->type() == EventTypeNames::mouseup && evt->button() == LeftButton)) {
            FloatPoint localPos = absoluteToLocal(FloatPoint(evt->absoluteLocation()), UseTransforms);
            continueResizing(m_cols, localPos.x());
            continueResizing(m_rows, localPos.y());
            if (evt->type() == EventTypeNames::mouseup && evt->button() == LeftButton) {
                setIsResizing(false);
                return true;
            }
        }
    }

    return false;
}

void LayoutFrameSet::setIsResizing(bool isResizing)
{
    m_isResizing = isResizing;
    for (LayoutObject* ancestor = parent(); ancestor; ancestor = ancestor->parent()) {
        if (ancestor->isFrameSet())
            toLayoutFrameSet(ancestor)->m_isChildResizing = isResizing;
    }
    if (LocalFrame* frame = this->frame())
        frame->eventHandler().setResizingFrameSet(isResizing ? frameSet() : 0);
}

bool LayoutFrameSet::canResizeRow(const IntPoint& p) const
{
    int r = hitTestSplit(m_rows, p.y());
    return r != noSplit && !m_rows.m_preventResize[r];
}

bool LayoutFrameSet::canResizeColumn(const IntPoint& p) const
{
    int c = hitTestSplit(m_cols, p.x());
    return c != noSplit && !m_cols.m_preventResize[c];
}

int LayoutFrameSet::splitPosition(const GridAxis& axis, int split) const
{
    if (needsLayout())
        return 0;

    int borderThickness = frameSet()->border();

    int size = axis.m_sizes.size();
    if (!size)
        return 0;

    int position = 0;
    for (int i = 0; i < split && i < size; ++i)
        position += axis.m_sizes[i] + borderThickness;
    return position - borderThickness;
}

int LayoutFrameSet::hitTestSplit(const GridAxis& axis, int position) const
{
    if (needsLayout())
        return noSplit;

    int borderThickness = frameSet()->border();
    if (borderThickness <= 0)
        return noSplit;

    size_t size = axis.m_sizes.size();
    if (!size)
        return noSplit;

    int splitPosition = axis.m_sizes[0];
    for (size_t i = 1; i < size; ++i) {
        if (position >= splitPosition && position < splitPosition + borderThickness)
            return i;
        splitPosition += borderThickness + axis.m_sizes[i];
    }
    return noSplit;
}

bool LayoutFrameSet::isChildAllowed(LayoutObject* child, const ComputedStyle&) const
{
    return child->isFrame() || child->isFrameSet();
}

CursorDirective LayoutFrameSet::getCursor(const LayoutPoint& point, Cursor& cursor) const
{
    IntPoint roundedPoint = roundedIntPoint(point);
    if (canResizeRow(roundedPoint)) {
        cursor = rowResizeCursor();
        return SetCursor;
    }
    if (canResizeColumn(roundedPoint)) {
        cursor = columnResizeCursor();
        return SetCursor;
    }
    return LayoutBox::getCursor(point, cursor);
}

} // namespace blink
