/*
 * Copyright (C) 1997 Martin Jones (mjones@kde.org)
 *           (C) 1997 Torben Weis (weis@kde.org)
 *           (C) 1998 Waldo Bastian (bastian@kde.org)
 *           (C) 1999 Lars Knoll (knoll@kde.org)
 *           (C) 1999 Antti Koivisto (koivisto@kde.org)
 * Copyright (C) 2003, 2004, 2005, 2006, 2007, 2009, 2013 Apple Inc. All rights reserved.
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
 */

#ifndef LayoutTableCell_h
#define LayoutTableCell_h

#include "core/CoreExport.h"
#include "core/layout/LayoutBlockFlow.h"
#include "core/layout/LayoutTableRow.h"
#include "core/layout/LayoutTableSection.h"
#include "platform/LengthFunctions.h"

namespace blink {

static const unsigned unsetColumnIndex = 0x1FFFFFFF;
static const unsigned maxColumnIndex = 0x1FFFFFFE; // 536,870,910

enum IncludeBorderColorOrNot { DoNotIncludeBorderColor, IncludeBorderColor };

class SubtreeLayoutScope;

class CORE_EXPORT LayoutTableCell final : public LayoutBlockFlow {
public:
    explicit LayoutTableCell(Element*);

    unsigned colSpan() const
    {
        if (!m_hasColSpan)
            return 1;
        return parseColSpanFromDOM();
    }
    unsigned rowSpan() const
    {
        if (!m_hasRowSpan)
            return 1;
        return parseRowSpanFromDOM();
    }

    // Called from HTMLTableCellElement.
    void colSpanOrRowSpanChanged();

    void setCol(unsigned column)
    {
        if (UNLIKELY(column > maxColumnIndex))
            CRASH();

        m_column = column;
    }

    bool hasCol() const { return m_column != unsetColumnIndex; }

    unsigned col() const
    {
        ASSERT(hasCol());
        return m_column;
    }

    LayoutTableRow* row() const { return toLayoutTableRow(parent()); }
    LayoutTableSection* section() const { return toLayoutTableSection(parent()->parent()); }
    LayoutTable* table() const { return toLayoutTable(parent()->parent()->parent()); }

    LayoutTableCell* previousCell() const;
    LayoutTableCell* nextCell() const;

    unsigned rowIndex() const
    {
        // This function shouldn't be called on a detached cell.
        ASSERT(row());
        return row()->rowIndex();
    }

    Length styleOrColLogicalWidth() const
    {
        Length styleWidth = style()->logicalWidth();
        if (!styleWidth.isAuto())
            return styleWidth;
        if (LayoutTableCol* firstColumn = table()->colElement(col()))
            return logicalWidthFromColumns(firstColumn, styleWidth);
        return styleWidth;
    }

    int logicalHeightFromStyle() const
    {
        int styleLogicalHeight = valueForLength(style()->logicalHeight(), 0);
        // In strict mode, box-sizing: content-box do the right thing and actually add in the border and padding.
        // Call computedCSSPadding* directly to avoid including implicitPadding.
        if (!document().inQuirksMode() && style()->boxSizing() != BORDER_BOX)
            styleLogicalHeight += (computedCSSPaddingBefore() + computedCSSPaddingAfter()).floor() + borderBefore() + borderAfter();
        return styleLogicalHeight;
    }

    int logicalHeightForRowSizing() const
    {
        // FIXME: This function does too much work, and is very hot during table layout!
        int adjustedLogicalHeight = pixelSnappedLogicalHeight() - (intrinsicPaddingBefore() + intrinsicPaddingAfter());
        int styleLogicalHeight = logicalHeightFromStyle();
        return max(styleLogicalHeight, adjustedLogicalHeight);
    }


    void setCellLogicalWidth(int constrainedLogicalWidth, SubtreeLayoutScope&);

    virtual int borderLeft() const override;
    virtual int borderRight() const override;
    virtual int borderTop() const override;
    virtual int borderBottom() const override;
    virtual int borderStart() const override;
    virtual int borderEnd() const override;
    virtual int borderBefore() const override;
    virtual int borderAfter() const override;

    void collectBorderValues(LayoutTable::CollapsedBorderValues&);
    static void sortBorderValues(LayoutTable::CollapsedBorderValues&);

    virtual void layout() override;

    virtual void paint(const PaintInfo&, const LayoutPoint&) override;

    LayoutUnit cellBaselinePosition() const;
    bool isBaselineAligned() const
    {
        EVerticalAlign va = style()->verticalAlign();
        return va == BASELINE || va == TEXT_BOTTOM || va == TEXT_TOP || va == SUPER || va == SUB || va == LENGTH;
    }

    void computeIntrinsicPadding(int rowHeight, SubtreeLayoutScope&);
    void clearIntrinsicPadding() { setIntrinsicPadding(0, 0); }

    int intrinsicPaddingBefore() const { return m_intrinsicPaddingBefore; }
    int intrinsicPaddingAfter() const { return m_intrinsicPaddingAfter; }

    virtual LayoutUnit paddingTop() const override;
    virtual LayoutUnit paddingBottom() const override;
    virtual LayoutUnit paddingLeft() const override;
    virtual LayoutUnit paddingRight() const override;

    // FIXME: For now we just assume the cell has the same block flow direction as the table. It's likely we'll
    // create an extra anonymous LayoutBlock to handle mixing directionality anyway, in which case we can lock
    // the block flow directionality of the cells to the table's directionality.
    virtual LayoutUnit paddingBefore() const override;
    virtual LayoutUnit paddingAfter() const override;

    void setOverrideLogicalContentHeightFromRowHeight(LayoutUnit);

    virtual void scrollbarsChanged(bool horizontalScrollbarChanged, bool verticalScrollbarChanged) override;

    bool cellWidthChanged() const { return m_cellWidthChanged; }
    void setCellWidthChanged(bool b = true) { m_cellWidthChanged = b; }

    static LayoutTableCell* createAnonymous(Document*);
    static LayoutTableCell* createAnonymousWithParent(const LayoutObject*);
    virtual LayoutBox* createAnonymousBoxWithSameTypeAs(const LayoutObject* parent) const override
    {
        return createAnonymousWithParent(parent);
    }

    // This function is used to unify which table part's style we use for computing direction and
    // writing mode. Writing modes are not allowed on row group and row but direction is.
    // This means we can safely use the same style in all cases to simplify our code.
    // FIXME: Eventually this function should replaced by style() once we support direction
    // on all table parts and writing-mode on cells.
    const ComputedStyle& styleForCellFlow() const
    {
        return row()->styleRef();
    }

    const BorderValue& borderAdjoiningTableStart() const
    {
        ASSERT(isFirstOrLastCellInRow());
        if (section()->hasSameDirectionAs(table()))
            return style()->borderStart();

        return style()->borderEnd();
    }

    const BorderValue& borderAdjoiningTableEnd() const
    {
        ASSERT(isFirstOrLastCellInRow());
        if (section()->hasSameDirectionAs(table()))
            return style()->borderEnd();

        return style()->borderStart();
    }

    const BorderValue& borderAdjoiningCellBefore(const LayoutTableCell* cell)
    {
        ASSERT_UNUSED(cell, table()->cellAfter(cell) == this);
        // FIXME: https://webkit.org/b/79272 - Add support for mixed directionality at the cell level.
        return style()->borderStart();
    }

    const BorderValue& borderAdjoiningCellAfter(const LayoutTableCell* cell)
    {
        ASSERT_UNUSED(cell, table()->cellBefore(cell) == this);
        // FIXME: https://webkit.org/b/79272 - Add support for mixed directionality at the cell level.
        return style()->borderEnd();
    }

#if ENABLE(ASSERT)
    bool isFirstOrLastCellInRow() const
    {
        return !table()->cellAfter(this) || !table()->cellBefore(this);
    }
#endif

    virtual const char* name() const override { return "LayoutTableCell"; }

protected:
    virtual void styleDidChange(StyleDifference, const ComputedStyle* oldStyle) override;
    virtual void computePreferredLogicalWidths() override;

    virtual void addLayerHitTestRects(LayerHitTestRects&, const DeprecatedPaintLayer* currentCompositedLayer, const LayoutPoint& layerOffset, const LayoutRect& containerRect) const override;

private:
    virtual bool isOfType(LayoutObjectType type) const override { return type == LayoutObjectTableCell || LayoutBlockFlow::isOfType(type); }

    virtual void willBeRemovedFromTree() override;

    virtual void updateLogicalWidth() override;

    virtual void paintBoxDecorationBackground(const PaintInfo&, const LayoutPoint&) override;
    virtual void paintMask(const PaintInfo&, const LayoutPoint&) override;

    virtual bool boxShadowShouldBeAppliedToBackground(BackgroundBleedAvoidance, InlineFlowBox*) const override;

    virtual LayoutSize offsetFromContainer(const LayoutObject*, const LayoutPoint&, bool* offsetDependsOnPoint = nullptr) const override;
    virtual LayoutRect clippedOverflowRectForPaintInvalidation(const LayoutBoxModelObject* paintInvalidationContainer, const PaintInvalidationState* = nullptr) const override;
    virtual void mapRectToPaintInvalidationBacking(const LayoutBoxModelObject* paintInvalidationContainer, LayoutRect&, const PaintInvalidationState*) const override;

    int borderHalfLeft(bool outer) const;
    int borderHalfRight(bool outer) const;
    int borderHalfTop(bool outer) const;
    int borderHalfBottom(bool outer) const;

    int borderHalfStart(bool outer) const;
    int borderHalfEnd(bool outer) const;
    int borderHalfBefore(bool outer) const;
    int borderHalfAfter(bool outer) const;

    void setIntrinsicPaddingBefore(int p) { m_intrinsicPaddingBefore = p; }
    void setIntrinsicPaddingAfter(int p) { m_intrinsicPaddingAfter = p; }
    void setIntrinsicPadding(int before, int after) { setIntrinsicPaddingBefore(before); setIntrinsicPaddingAfter(after); }

    bool hasStartBorderAdjoiningTable() const;
    bool hasEndBorderAdjoiningTable() const;

    CollapsedBorderValue computeCollapsedStartBorder(IncludeBorderColorOrNot = IncludeBorderColor) const;
    CollapsedBorderValue computeCollapsedEndBorder(IncludeBorderColorOrNot = IncludeBorderColor) const;
    CollapsedBorderValue computeCollapsedBeforeBorder(IncludeBorderColorOrNot = IncludeBorderColor) const;
    CollapsedBorderValue computeCollapsedAfterBorder(IncludeBorderColorOrNot = IncludeBorderColor) const;

    Length logicalWidthFromColumns(LayoutTableCol* firstColForThisCell, Length widthFromStyle) const;

    void updateColAndRowSpanFlags();

    unsigned parseRowSpanFromDOM() const;
    unsigned parseColSpanFromDOM() const;

    void nextSibling() const = delete;
    void previousSibling() const = delete;

    // Note MSVC will only pack members if they have identical types, hence we use unsigned instead of bool here.
    unsigned m_column : 29;
    unsigned m_cellWidthChanged : 1;
    unsigned m_hasColSpan: 1;
    unsigned m_hasRowSpan: 1;
    int m_intrinsicPaddingBefore;
    int m_intrinsicPaddingAfter;
};

DEFINE_LAYOUT_OBJECT_TYPE_CASTS(LayoutTableCell, isTableCell());

inline LayoutTableCell* LayoutTableCell::previousCell() const
{
    return toLayoutTableCell(LayoutObject::previousSibling());
}

inline LayoutTableCell* LayoutTableCell::nextCell() const
{
    return toLayoutTableCell(LayoutObject::nextSibling());
}

inline LayoutTableCell* LayoutTableRow::firstCell() const
{
    ASSERT(children() == virtualChildren());
    return toLayoutTableCell(children()->firstChild());
}

inline LayoutTableCell* LayoutTableRow::lastCell() const
{
    ASSERT(children() == virtualChildren());
    return toLayoutTableCell(children()->lastChild());
}

} // namespace blink

#endif // LayoutTableCell_h
