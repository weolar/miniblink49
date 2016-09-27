/*
 * Copyright (C) 1997 Martin Jones (mjones@kde.org)
 *           (C) 1997 Torben Weis (weis@kde.org)
 *           (C) 1998 Waldo Bastian (bastian@kde.org)
 *           (C) 1999 Lars Knoll (knoll@kde.org)
 *           (C) 1999 Antti Koivisto (koivisto@kde.org)
 * Copyright (C) 2003, 2004, 2005, 2006, 2009, 2010 Apple Inc. All rights reserved.
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

#ifndef LayoutTable_h
#define LayoutTable_h

#include "core/CSSPropertyNames.h"
#include "core/CoreExport.h"
#include "core/layout/LayoutBlock.h"
#include "core/style/CollapsedBorderValue.h"
#include "wtf/Vector.h"

namespace blink {

class LayoutTableCol;
class LayoutTableCaption;
class LayoutTableCell;
class LayoutTableSection;
class TableLayoutAlgorithm;

enum SkipEmptySectionsValue { DoNotSkipEmptySections, SkipEmptySections };

class CORE_EXPORT LayoutTable final : public LayoutBlock {
public:
    explicit LayoutTable(Element*);
    virtual ~LayoutTable();

    // Per CSS 3 writing-mode: "The first and second values of the 'border-spacing' property represent spacing between columns
    // and rows respectively, not necessarily the horizontal and vertical spacing respectively".
    int hBorderSpacing() const { return m_hSpacing; }
    int vBorderSpacing() const { return m_vSpacing; }

    bool collapseBorders() const { return style()->borderCollapse(); }

    virtual int borderStart() const override { return m_borderStart; }
    virtual int borderEnd() const override { return m_borderEnd; }
    virtual int borderBefore() const override;
    virtual int borderAfter() const override;

    virtual int borderLeft() const override
    {
        if (style()->isHorizontalWritingMode())
            return style()->isLeftToRightDirection() ? borderStart() : borderEnd();
        return style()->isFlippedBlocksWritingMode() ? borderAfter() : borderBefore();
    }

    virtual int borderRight() const override
    {
        if (style()->isHorizontalWritingMode())
            return style()->isLeftToRightDirection() ? borderEnd() : borderStart();
        return style()->isFlippedBlocksWritingMode() ? borderBefore() : borderAfter();
    }

    virtual int borderTop() const override
    {
        if (style()->isHorizontalWritingMode())
            return style()->isFlippedBlocksWritingMode() ? borderAfter() : borderBefore();
        return style()->isLeftToRightDirection() ? borderStart() : borderEnd();
    }

    virtual int borderBottom() const override
    {
        if (style()->isHorizontalWritingMode())
            return style()->isFlippedBlocksWritingMode() ? borderBefore() : borderAfter();
        return style()->isLeftToRightDirection() ? borderEnd() : borderStart();
    }

    int outerBorderBefore() const;
    int outerBorderAfter() const;
    int outerBorderStart() const;
    int outerBorderEnd() const;

    int outerBorderLeft() const
    {
        if (style()->isHorizontalWritingMode())
            return style()->isLeftToRightDirection() ? outerBorderStart() : outerBorderEnd();
        return style()->isFlippedBlocksWritingMode() ? outerBorderAfter() : outerBorderBefore();
    }

    int outerBorderRight() const
    {
        if (style()->isHorizontalWritingMode())
            return style()->isLeftToRightDirection() ? outerBorderEnd() : outerBorderStart();
        return style()->isFlippedBlocksWritingMode() ? outerBorderBefore() : outerBorderAfter();
    }

    int outerBorderTop() const
    {
        if (style()->isHorizontalWritingMode())
            return style()->isFlippedBlocksWritingMode() ? outerBorderAfter() : outerBorderBefore();
        return style()->isLeftToRightDirection() ? outerBorderStart() : outerBorderEnd();
    }

    int outerBorderBottom() const
    {
        if (style()->isHorizontalWritingMode())
            return style()->isFlippedBlocksWritingMode() ? outerBorderBefore() : outerBorderAfter();
        return style()->isLeftToRightDirection() ? outerBorderEnd() : outerBorderStart();
    }

    int calcBorderStart() const;
    int calcBorderEnd() const;
    void recalcBordersInRowDirection();

    virtual void addChild(LayoutObject* child, LayoutObject* beforeChild = nullptr) override;
    virtual void addChildIgnoringContinuation(LayoutObject* newChild, LayoutObject* beforeChild = nullptr) override;

    struct ColumnStruct {
        explicit ColumnStruct(unsigned initialSpan = 1)
            : span(initialSpan)
        {
        }

        unsigned span;
    };

    void forceSectionsRecalc()
    {
        setNeedsSectionRecalc();
        recalcSections();
    }

    const Vector<ColumnStruct>& columns() const { return m_columns; }
    const Vector<int>& columnPositions() const { return m_columnPos; }
    void setColumnPosition(unsigned index, int position)
    {
        // Note that if our horizontal border-spacing changed, our position will change but not
        // our column's width. In practice, horizontal border-spacing won't change often.
        m_columnLogicalWidthChanged |= m_columnPos[index] != position;
        m_columnPos[index] = position;
    }

    LayoutTableSection* header() const { return m_head; }
    LayoutTableSection* footer() const { return m_foot; }
    LayoutTableSection* firstBody() const { return m_firstBody; }

    // This function returns 0 if the table has no section.
    LayoutTableSection* topSection() const;
    LayoutTableSection* bottomSection() const;

    // This function returns 0 if the table has no non-empty sections.
    LayoutTableSection* topNonEmptySection() const;

    unsigned lastColumnIndex() const { return numEffCols() - 1; }

    void splitColumn(unsigned position, unsigned firstSpan);
    void appendColumn(unsigned span);
    unsigned numEffCols() const { return m_columns.size(); }
    unsigned spanOfEffCol(unsigned effCol) const { return m_columns[effCol].span; }

    unsigned colToEffCol(unsigned column) const
    {
        if (!m_hasCellColspanThatDeterminesTableWidth)
            return column;

        unsigned effColumn = 0;
        unsigned numColumns = numEffCols();
        for (unsigned c = 0; effColumn < numColumns && c + m_columns[effColumn].span - 1 < column; ++effColumn)
            c += m_columns[effColumn].span;
        return effColumn;
    }

    unsigned effColToCol(unsigned effCol) const
    {
        if (!m_hasCellColspanThatDeterminesTableWidth)
            return effCol;

        unsigned c = 0;
        for (unsigned i = 0; i < effCol; i++)
            c += m_columns[i].span;
        return c;
    }

    LayoutUnit borderSpacingInRowDirection() const
    {
        if (unsigned effectiveColumnCount = numEffCols())
            return static_cast<LayoutUnit>(effectiveColumnCount + 1) * hBorderSpacing();

        return 0;
    }

    // Override paddingStart/End to return pixel values to match behavor of LayoutTableCell.
    virtual LayoutUnit paddingEnd() const override { return static_cast<int>(LayoutBlock::paddingEnd()); }
    virtual LayoutUnit paddingStart() const override { return static_cast<int>(LayoutBlock::paddingStart()); }

    LayoutUnit bordersPaddingAndSpacingInRowDirection() const
    {
        // 'border-spacing' only applies to separate borders (see 17.6.1 The separated borders model).
        return borderStart() + borderEnd() + (collapseBorders() ? LayoutUnit() : (paddingStart() + paddingEnd() + borderSpacingInRowDirection()));
    }

    // Return the first column or column-group.
    LayoutTableCol* firstColumn() const;

    LayoutTableCol* colElement(unsigned col, bool* startEdge = nullptr, bool* endEdge = nullptr) const
    {
        // The common case is to not have columns, make that case fast.
        if (!m_hasColElements)
            return nullptr;
        return slowColElement(col, startEdge, endEdge);
    }

    bool needsSectionRecalc() const { return m_needsSectionRecalc; }
    void setNeedsSectionRecalc()
    {
        if (documentBeingDestroyed())
            return;
        m_needsSectionRecalc = true;
        setNeedsLayoutAndFullPaintInvalidation(LayoutInvalidationReason::TableChanged);
    }

    LayoutTableSection* sectionAbove(const LayoutTableSection*, SkipEmptySectionsValue = DoNotSkipEmptySections) const;
    LayoutTableSection* sectionBelow(const LayoutTableSection*, SkipEmptySectionsValue = DoNotSkipEmptySections) const;

    LayoutTableCell* cellAbove(const LayoutTableCell*) const;
    LayoutTableCell* cellBelow(const LayoutTableCell*) const;
    LayoutTableCell* cellBefore(const LayoutTableCell*) const;
    LayoutTableCell* cellAfter(const LayoutTableCell*) const;

    typedef Vector<CollapsedBorderValue> CollapsedBorderValues;
    void invalidateCollapsedBorders();

    // FIXME: This method should be moved into TablePainter.
    const CollapsedBorderValue* currentBorderValue() const { return m_currentBorder; }
    void setCurrentBorderValue(const CollapsedBorderValue* val) { m_currentBorder = val; }

    bool hasSections() const { return m_head || m_foot || m_firstBody; }

    void recalcSectionsIfNeeded() const
    {
        if (m_needsSectionRecalc)
            recalcSections();
    }

    static LayoutTable* createAnonymousWithParent(const LayoutObject*);
    virtual LayoutBox* createAnonymousBoxWithSameTypeAs(const LayoutObject* parent) const override
    {
        return createAnonymousWithParent(parent);
    }

    const BorderValue& tableStartBorderAdjoiningCell(const LayoutTableCell*) const;
    const BorderValue& tableEndBorderAdjoiningCell(const LayoutTableCell*) const;

    void addCaption(const LayoutTableCaption*);
    void removeCaption(const LayoutTableCaption*);
    void addColumn(const LayoutTableCol*);
    void removeColumn(const LayoutTableCol*);

    virtual void paintBoxDecorationBackground(const PaintInfo&, const LayoutPoint&) override final;

    virtual void paintMask(const PaintInfo&, const LayoutPoint&) override final;

    const CollapsedBorderValues& collapsedBorders()
    {
        ASSERT(m_collapsedBordersValid);
        return m_collapsedBorders;
    }

    void subtractCaptionRect(LayoutRect&) const;

    virtual const char* name() const override { return "LayoutTable"; }

protected:
    virtual void styleDidChange(StyleDifference, const ComputedStyle* oldStyle) override;
    virtual void simplifiedNormalFlowLayout() override;
    virtual PaintInvalidationReason invalidatePaintIfNeeded(PaintInvalidationState&, const LayoutBoxModelObject& paintInvalidationContainer) override;
    virtual void invalidatePaintOfSubtreesIfNeeded(PaintInvalidationState&) override;

private:
    virtual bool isOfType(LayoutObjectType type) const override { return type == LayoutObjectTable || LayoutBlock::isOfType(type); }

    virtual void paintObject(const PaintInfo&, const LayoutPoint&) override;
    virtual void layout() override;
    virtual void computeIntrinsicLogicalWidths(LayoutUnit& minWidth, LayoutUnit& maxWidth) const override;
    virtual void computePreferredLogicalWidths() override;
    virtual bool nodeAtPoint(HitTestResult&, const HitTestLocation& locationInContainer, const LayoutPoint& accumulatedOffset, HitTestAction) override;

    virtual int baselinePosition(FontBaseline, bool firstLine, LineDirectionMode, LinePositionMode = PositionOnContainingLine) const override;
    virtual int firstLineBoxBaseline() const override;
    virtual int inlineBlockBaseline(LineDirectionMode) const override;

    LayoutTableCol* slowColElement(unsigned col, bool* startEdge, bool* endEdge) const;

    void updateColumnCache() const;
    void invalidateCachedColumns();

    virtual void updateLogicalWidth() override;

    LayoutUnit convertStyleLogicalWidthToComputedWidth(const Length& styleLogicalWidth, LayoutUnit availableWidth);
    LayoutUnit convertStyleLogicalHeightToComputedHeight(const Length& styleLogicalHeight);

    virtual LayoutRect overflowClipRect(const LayoutPoint& location, OverlayScrollbarSizeRelevancy = IgnoreOverlayScrollbarSize) const override;

    virtual void addOverflowFromChildren() override;

    void recalcSections() const;
    void layoutCaption(LayoutTableCaption&);

    void distributeExtraLogicalHeight(int extraLogicalHeight);

    void recalcCollapsedBordersIfNeeded();

    mutable Vector<int> m_columnPos;
    mutable Vector<ColumnStruct> m_columns;
    mutable Vector<LayoutTableCaption*> m_captions;
    mutable Vector<LayoutTableCol*> m_columnLayoutObjects;

    mutable LayoutTableSection* m_head;
    mutable LayoutTableSection* m_foot;
    mutable LayoutTableSection* m_firstBody;

    OwnPtr<TableLayoutAlgorithm> m_tableLayout;

    // A sorted list of all unique border values that we want to paint.
    CollapsedBorderValues m_collapsedBorders;
    const CollapsedBorderValue* m_currentBorder;
    bool m_collapsedBordersValid : 1;

    mutable bool m_hasColElements : 1;
    mutable bool m_needsSectionRecalc : 1;

    bool m_columnLogicalWidthChanged : 1;
    mutable bool m_columnLayoutObjectsValid: 1;
    mutable bool m_hasCellColspanThatDeterminesTableWidth : 1;
    bool hasCellColspanThatDeterminesTableWidth() const
    {
        for (unsigned c = 0; c < numEffCols(); c++) {
            if (m_columns[c].span > 1)
                return true;
        }
        return false;
    }

    short m_hSpacing;
    short m_vSpacing;
    int m_borderStart;
    int m_borderEnd;
};

inline LayoutTableSection* LayoutTable::topSection() const
{
    ASSERT(!needsSectionRecalc());
    if (m_head)
        return m_head;
    if (m_firstBody)
        return m_firstBody;
    return m_foot;
}

DEFINE_LAYOUT_OBJECT_TYPE_CASTS(LayoutTable, isTable());

} // namespace blink

#endif // LayoutTable_h
