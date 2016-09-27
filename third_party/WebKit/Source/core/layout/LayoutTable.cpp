/*
 * Copyright (C) 1997 Martin Jones (mjones@kde.org)
 *           (C) 1997 Torben Weis (weis@kde.org)
 *           (C) 1998 Waldo Bastian (bastian@kde.org)
 *           (C) 1999 Lars Knoll (knoll@kde.org)
 *           (C) 1999 Antti Koivisto (koivisto@kde.org)
 * Copyright (C) 2003, 2004, 2005, 2006, 2007, 2008, 2009, 2010, 2013 Apple Inc. All rights reserved.
 * Copyright (C) 2006 Alexey Proskuryakov (ap@nypop.com)
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

#include "config.h"
#include "core/layout/LayoutTable.h"

#include "core/HTMLNames.h"
#include "core/dom/Document.h"
#include "core/frame/FrameView.h"
#include "core/html/HTMLTableElement.h"
#include "core/layout/HitTestResult.h"
#include "core/layout/LayoutAnalyzer.h"
#include "core/layout/LayoutTableCaption.h"
#include "core/layout/LayoutTableCell.h"
#include "core/layout/LayoutTableCol.h"
#include "core/layout/LayoutTableSection.h"
#include "core/layout/LayoutView.h"
#include "core/layout/SubtreeLayoutScope.h"
#include "core/layout/TableLayoutAlgorithmAuto.h"
#include "core/layout/TableLayoutAlgorithmFixed.h"
#include "core/layout/TextAutosizer.h"
#include "core/paint/BoxPainter.h"
#include "core/paint/DeprecatedPaintLayer.h"
#include "core/paint/TablePainter.h"
#include "core/style/StyleInheritedData.h"

namespace blink {

using namespace HTMLNames;

LayoutTable::LayoutTable(Element* element)
    : LayoutBlock(element)
    , m_head(nullptr)
    , m_foot(nullptr)
    , m_firstBody(nullptr)
    , m_currentBorder(nullptr)
    , m_collapsedBordersValid(false)
    , m_hasColElements(false)
    , m_needsSectionRecalc(false)
    , m_columnLogicalWidthChanged(false)
    , m_columnLayoutObjectsValid(false)
    , m_hasCellColspanThatDeterminesTableWidth(false)
    , m_hSpacing(0)
    , m_vSpacing(0)
    , m_borderStart(0)
    , m_borderEnd(0)
{
    ASSERT(!childrenInline());
    m_columnPos.fill(0, 1);
}

LayoutTable::~LayoutTable()
{
}

void LayoutTable::styleDidChange(StyleDifference diff, const ComputedStyle* oldStyle)
{
    LayoutBlock::styleDidChange(diff, oldStyle);
    propagateStyleToAnonymousChildren();

    bool oldFixedTableLayout = oldStyle ? oldStyle->isFixedTableLayout() : false;

    // In the collapsed border model, there is no cell spacing.
    m_hSpacing = collapseBorders() ? 0 : style()->horizontalBorderSpacing();
    m_vSpacing = collapseBorders() ? 0 : style()->verticalBorderSpacing();
    m_columnPos[0] = m_hSpacing;

    if (!m_tableLayout || style()->isFixedTableLayout() != oldFixedTableLayout) {
        if (m_tableLayout)
            m_tableLayout->willChangeTableLayout();

        // According to the CSS2 spec, you only use fixed table layout if an
        // explicit width is specified on the table.  Auto width implies auto table layout.
        if (style()->isFixedTableLayout())
            m_tableLayout = adoptPtr(new TableLayoutAlgorithmFixed(this));
        else
            m_tableLayout = adoptPtr(new TableLayoutAlgorithmAuto(this));
    }

    // If border was changed, invalidate collapsed borders cache.
    if (!needsLayout() && oldStyle && oldStyle->border() != style()->border())
        invalidateCollapsedBorders();
}

static inline void resetSectionPointerIfNotBefore(LayoutTableSection*& ptr, LayoutObject* before)
{
    if (!before || !ptr)
        return;
    LayoutObject* o = before->previousSibling();
    while (o && o != ptr)
        o = o->previousSibling();
    if (!o)
        ptr = 0;
}

static inline bool needsTableSection(LayoutObject* object)
{
    // Return true if 'object' can't exist in an anonymous table without being
    // wrapped in a table section box.
    EDisplay display = object->style()->display();
    return display != TABLE_CAPTION && display != TABLE_COLUMN_GROUP && display != TABLE_COLUMN;
}

void LayoutTable::addChild(LayoutObject* child, LayoutObject* beforeChild)
{
    bool wrapInAnonymousSection = !child->isOutOfFlowPositioned();

    if (child->isTableCaption()) {
        wrapInAnonymousSection = false;
    } else if (child->isLayoutTableCol()) {
        m_hasColElements = true;
        wrapInAnonymousSection = false;
    } else if (child->isTableSection()) {
        switch (child->style()->display()) {
        case TABLE_HEADER_GROUP:
            resetSectionPointerIfNotBefore(m_head, beforeChild);
            if (!m_head) {
                m_head = toLayoutTableSection(child);
            } else {
                resetSectionPointerIfNotBefore(m_firstBody, beforeChild);
                if (!m_firstBody)
                    m_firstBody = toLayoutTableSection(child);
            }
            wrapInAnonymousSection = false;
            break;
        case TABLE_FOOTER_GROUP:
            resetSectionPointerIfNotBefore(m_foot, beforeChild);
            if (!m_foot) {
                m_foot = toLayoutTableSection(child);
                wrapInAnonymousSection = false;
                break;
            }
            // Fall through.
        case TABLE_ROW_GROUP:
            resetSectionPointerIfNotBefore(m_firstBody, beforeChild);
            if (!m_firstBody)
                m_firstBody = toLayoutTableSection(child);
            wrapInAnonymousSection = false;
            break;
        default:
            ASSERT_NOT_REACHED();
        }
    } else {
        wrapInAnonymousSection = true;
    }

    if (child->isTableSection())
        setNeedsSectionRecalc();

    if (!wrapInAnonymousSection) {
        if (beforeChild && beforeChild->parent() != this)
            beforeChild = splitAnonymousBoxesAroundChild(beforeChild);

        LayoutBox::addChild(child, beforeChild);
        return;
    }

    if (!beforeChild && lastChild() && lastChild()->isTableSection() && lastChild()->isAnonymous() && !lastChild()->isBeforeContent()) {
        lastChild()->addChild(child);
        return;
    }

    if (beforeChild && !beforeChild->isAnonymous() && beforeChild->parent() == this) {
        LayoutObject* section = beforeChild->previousSibling();
        if (section && section->isTableSection() && section->isAnonymous()) {
            section->addChild(child);
            return;
        }
    }

    LayoutObject* lastBox = beforeChild;
    while (lastBox && lastBox->parent()->isAnonymous() && !lastBox->isTableSection() && needsTableSection(lastBox))
        lastBox = lastBox->parent();
    if (lastBox && lastBox->isAnonymous() && !isAfterContent(lastBox)) {
        if (beforeChild == lastBox)
            beforeChild = lastBox->slowFirstChild();
        lastBox->addChild(child, beforeChild);
        return;
    }

    if (beforeChild && !beforeChild->isTableSection() && needsTableSection(beforeChild))
        beforeChild = 0;

    LayoutTableSection* section = LayoutTableSection::createAnonymousWithParent(this);
    addChild(section, beforeChild);
    section->addChild(child);
}

void LayoutTable::addChildIgnoringContinuation(LayoutObject* newChild, LayoutObject* beforeChild)
{
    // We need to bypass the LayoutBlock implementation and instead do a normal addChild() (or we
    // won't get there at all), so that any missing anonymous table part layoutObjects are
    // inserted. Otherwise we might end up with an insane layout tree with inlines or blocks as
    // direct children of a table, which will break assumptions made all over the code, which may
    // lead to crashers and security issues.
    addChild(newChild, beforeChild);
}

void LayoutTable::addCaption(const LayoutTableCaption* caption)
{
    ASSERT(m_captions.find(caption) == kNotFound);
    m_captions.append(const_cast<LayoutTableCaption*>(caption));
}

void LayoutTable::removeCaption(const LayoutTableCaption* oldCaption)
{
    size_t index = m_captions.find(oldCaption);
    ASSERT(index != kNotFound);
    if (index == kNotFound)
        return;

    m_captions.remove(index);
}

void LayoutTable::invalidateCachedColumns()
{
    m_columnLayoutObjectsValid = false;
    m_columnLayoutObjects.resize(0);
}

void LayoutTable::addColumn(const LayoutTableCol*)
{
    invalidateCachedColumns();
}

void LayoutTable::removeColumn(const LayoutTableCol*)
{
    invalidateCachedColumns();
    // We don't really need to recompute our sections, but we need to update our
    // column count and whether we have a column. Currently, we only have one
    // size-fit-all flag but we may have to consider splitting it.
    setNeedsSectionRecalc();
}

void LayoutTable::updateLogicalWidth()
{
    recalcSectionsIfNeeded();

    if (isOutOfFlowPositioned()) {
        LogicalExtentComputedValues computedValues;
        computePositionedLogicalWidth(computedValues);
        setLogicalWidth(computedValues.m_extent);
        setLogicalLeft(computedValues.m_position);
        setMarginStart(computedValues.m_margins.m_start);
        setMarginEnd(computedValues.m_margins.m_end);
    }

    LayoutBlock* cb = containingBlock();

    LayoutUnit availableLogicalWidth = containingBlockLogicalWidthForContent() + (isOutOfFlowPositioned() ? cb->paddingLogicalWidth() : LayoutUnit());
    bool hasPerpendicularContainingBlock = cb->style()->isHorizontalWritingMode() != style()->isHorizontalWritingMode();
    LayoutUnit containerWidthInInlineDirection = hasPerpendicularContainingBlock ? perpendicularContainingBlockLogicalHeight() : availableLogicalWidth;

    Length styleLogicalWidth = style()->logicalWidth();
    if ((styleLogicalWidth.isSpecified() && styleLogicalWidth.isPositive()) || styleLogicalWidth.isIntrinsic()) {
        setLogicalWidth(convertStyleLogicalWidthToComputedWidth(styleLogicalWidth, containerWidthInInlineDirection));
    } else {
        // Subtract out any fixed margins from our available width for auto width tables.
        LayoutUnit marginStart = minimumValueForLength(style()->marginStart(), availableLogicalWidth);
        LayoutUnit marginEnd = minimumValueForLength(style()->marginEnd(), availableLogicalWidth);
        LayoutUnit marginTotal = marginStart + marginEnd;

        // Subtract out our margins to get the available content width.
        LayoutUnit availableContentLogicalWidth = std::max<LayoutUnit>(0, containerWidthInInlineDirection - marginTotal);
        if (shrinkToAvoidFloats() && cb->isLayoutBlockFlow() && toLayoutBlockFlow(cb)->containsFloats() && !hasPerpendicularContainingBlock)
            availableContentLogicalWidth = shrinkLogicalWidthToAvoidFloats(marginStart, marginEnd, toLayoutBlockFlow(cb));

        // Ensure we aren't bigger than our available width.
        setLogicalWidth(std::min<int>(availableContentLogicalWidth, maxPreferredLogicalWidth()));
    }

    // Ensure we aren't bigger than our max-width style.
    Length styleMaxLogicalWidth = style()->logicalMaxWidth();
    if ((styleMaxLogicalWidth.isSpecified() && !styleMaxLogicalWidth.isNegative()) || styleMaxLogicalWidth.isIntrinsic()) {
        LayoutUnit computedMaxLogicalWidth = convertStyleLogicalWidthToComputedWidth(styleMaxLogicalWidth, availableLogicalWidth);
        setLogicalWidth(std::min<int>(logicalWidth(), computedMaxLogicalWidth));
    }

    // Ensure we aren't smaller than our min preferred width. This MUST be done after 'max-width' as
    // we ignore it if it means we wouldn't accomodate our content.
    setLogicalWidth(std::max<int>(logicalWidth(), minPreferredLogicalWidth()));

    // Ensure we aren't smaller than our min-width style.
    Length styleMinLogicalWidth = style()->logicalMinWidth();
    if ((styleMinLogicalWidth.isSpecified() && !styleMinLogicalWidth.isNegative()) || styleMinLogicalWidth.isIntrinsic()) {
        LayoutUnit computedMinLogicalWidth = convertStyleLogicalWidthToComputedWidth(styleMinLogicalWidth, availableLogicalWidth);
        setLogicalWidth(std::max<int>(logicalWidth(), computedMinLogicalWidth));
    }

    // Finally, with our true width determined, compute our margins for real.
    ComputedMarginValues marginValues;
    computeMarginsForDirection(InlineDirection, cb, availableLogicalWidth, logicalWidth(), marginValues.m_start, marginValues.m_end, style()->marginStart(), style()->marginEnd());
    setMarginStart(marginValues.m_start);
    setMarginEnd(marginValues.m_end);

    // We should NEVER shrink the table below the min-content logical width, or else the table can't accomodate
    // its own content which doesn't match CSS nor what authors expect.
    // FIXME: When we convert to sub-pixel layout for tables we can remove the int conversion
    // https://code.google.com/p/chromium/issues/detail?id=241198
    ASSERT(logicalWidth().toInt() >= minPreferredLogicalWidth().toInt());
}

// This method takes a ComputedStyle's logical width, min-width, or max-width length and computes its actual value.
LayoutUnit LayoutTable::convertStyleLogicalWidthToComputedWidth(const Length& styleLogicalWidth, LayoutUnit availableWidth)
{
    if (styleLogicalWidth.isIntrinsic())
        return computeIntrinsicLogicalWidthUsing(styleLogicalWidth, availableWidth, bordersPaddingAndSpacingInRowDirection());

    // HTML tables' width styles already include borders and paddings, but CSS tables' width styles do not.
    LayoutUnit borders = 0;
    bool isCSSTable = !isHTMLTableElement(node());
    if (isCSSTable && styleLogicalWidth.isSpecified() && styleLogicalWidth.isPositive() && style()->boxSizing() == CONTENT_BOX)
        borders = borderStart() + borderEnd() + (collapseBorders() ? LayoutUnit() : paddingStart() + paddingEnd());

    return minimumValueForLength(styleLogicalWidth, availableWidth) + borders;
}

LayoutUnit LayoutTable::convertStyleLogicalHeightToComputedHeight(const Length& styleLogicalHeight)
{
    LayoutUnit borderAndPaddingBefore = borderBefore() + (collapseBorders() ? LayoutUnit() : paddingBefore());
    LayoutUnit borderAndPaddingAfter = borderAfter() + (collapseBorders() ? LayoutUnit() : paddingAfter());
    LayoutUnit borderAndPadding = borderAndPaddingBefore + borderAndPaddingAfter;
    LayoutUnit computedLogicalHeight = 0;
    if (styleLogicalHeight.isFixed()) {
        // HTML tables size as though CSS height includes border/padding, CSS tables do not.
        LayoutUnit borders = LayoutUnit();
        // FIXME: We cannot apply box-sizing: content-box on <table> which other browsers allow.
        if (isHTMLTableElement(node()) || style()->boxSizing() == BORDER_BOX) {
            borders = borderAndPadding;
        }
        computedLogicalHeight = styleLogicalHeight.value() - borders;
    } else if (styleLogicalHeight.hasPercent()) {
        computedLogicalHeight = computePercentageLogicalHeight(styleLogicalHeight);
    } else if (styleLogicalHeight.isIntrinsic()) {
        computedLogicalHeight = computeIntrinsicLogicalContentHeightUsing(styleLogicalHeight, logicalHeight() - borderAndPadding, borderAndPadding);
    } else {
        ASSERT_NOT_REACHED();
    }
    return std::max<LayoutUnit>(0, computedLogicalHeight);
}

void LayoutTable::layoutCaption(LayoutTableCaption& caption)
{
    if (caption.needsLayout()) {
        // The margins may not be available but ensure the caption is at least located beneath any previous sibling caption
        // so that it does not mistakenly think any floats in the previous caption intrude into it.
        caption.setLogicalLocation(LayoutPoint(caption.marginStart(), collapsedMarginBeforeForChild(caption) + logicalHeight()));
        // If LayoutTableCaption ever gets a layout() function, use it here.
        caption.layoutIfNeeded();
    }
    // Apply the margins to the location now that they are definitely available from layout
    LayoutUnit captionLogicalTop = collapsedMarginBeforeForChild(caption) + logicalHeight();
    if (view()->layoutState()->isPaginated()) {
        captionLogicalTop += caption.paginationStrut();
        caption.setPaginationStrut(0);
    }
    caption.setLogicalLocation(LayoutPoint(caption.marginStart(), captionLogicalTop));

    if (!selfNeedsLayout())
        caption.setMayNeedPaintInvalidation();

    setLogicalHeight(logicalHeight() + caption.logicalHeight() + collapsedMarginBeforeForChild(caption) + collapsedMarginAfterForChild(caption));
}

void LayoutTable::distributeExtraLogicalHeight(int extraLogicalHeight)
{
    if (extraLogicalHeight <= 0)
        return;

    // FIXME: Distribute the extra logical height between all table sections instead of giving it all to the first one.
    if (LayoutTableSection* section = firstBody())
        extraLogicalHeight -= section->distributeExtraLogicalHeightToRows(extraLogicalHeight);

    // FIXME: We really would like to enable this ASSERT to ensure that all the extra space has been distributed.
    // However our current distribution algorithm does not round properly and thus we can have some remaining height.
    // ASSERT(!topSection() || !extraLogicalHeight);
}

void LayoutTable::simplifiedNormalFlowLayout()
{
    // FIXME: We should walk through the items in the tree in tree order to do the layout here
    // instead of walking through individual parts of the tree. crbug.com/442737
    for (auto& caption : m_captions)
        caption->layoutIfNeeded();

    for (LayoutTableSection* section = topSection(); section; section = sectionBelow(section)) {
        section->layoutIfNeeded();
        section->layoutRows();
        section->computeOverflowFromCells();
        section->updateLayerTransformAfterLayout();
        section->addVisualEffectOverflow();
    }
}

void LayoutTable::layout()
{
    ASSERT(needsLayout());
    LayoutAnalyzer::Scope analyzer(*this);

    if (simplifiedLayout())
        return;

    // Note: LayoutTable is handled differently than other LayoutBlocks and the LayoutScope
    //       must be created before the table begins laying out.
    TextAutosizer::LayoutScope textAutosizerLayoutScope(this);

    recalcSectionsIfNeeded();
    // FIXME: We should do this recalc lazily in borderStart/borderEnd so that we don't have to make sure
    // to call this before we call borderStart/borderEnd to avoid getting a stale value.
    recalcBordersInRowDirection();

    SubtreeLayoutScope layouter(*this);

    // If any table section moved vertically, we will just issue paint invalidations for everything from that
    // section down (it is quite unlikely that any of the following sections
    // did not shift).
    bool sectionMoved = false;
    LayoutUnit movedSectionLogicalTop = 0;
    {
        LayoutState state(*this, locationOffset());
        LayoutUnit oldLogicalWidth = logicalWidth();
        LayoutUnit oldLogicalHeight = logicalHeight();

        setLogicalHeight(0);
        updateLogicalWidth();

        if (logicalWidth() != oldLogicalWidth) {
            for (unsigned i = 0; i < m_captions.size(); i++)
                layouter.setNeedsLayout(m_captions[i], LayoutInvalidationReason::TableChanged);
        }
        // FIXME: The optimisation below doesn't work since the internal table
        // layout could have changed. We need to add a flag to the table
        // layout that tells us if something has changed in the min max
        // calculations to do it correctly.
        // if ( oldWidth != width() || columns.size() + 1 != columnPos.size() )
        m_tableLayout->layout();

        LayoutUnit totalSectionLogicalHeight = 0;
        LayoutUnit oldTableLogicalTop = 0;
        for (unsigned i = 0; i < m_captions.size(); i++)
            oldTableLogicalTop += m_captions[i]->logicalHeight() + m_captions[i]->marginBefore() + m_captions[i]->marginAfter();

        bool collapsing = collapseBorders();

        for (LayoutObject* child = firstChild(); child; child = child->nextSibling()) {
            if (!child->needsLayout() && child->isBox())
                toLayoutBox(child)->markForPaginationRelayoutIfNeeded(layouter);
            if (child->isTableSection()) {
                LayoutTableSection* section = toLayoutTableSection(child);
                if (m_columnLogicalWidthChanged)
                    layouter.setChildNeedsLayout(section);
                section->layoutIfNeeded();
                totalSectionLogicalHeight += section->calcRowLogicalHeight();
                if (collapsing)
                    section->recalcOuterBorder();
                ASSERT(!section->needsLayout());
            } else if (child->isLayoutTableCol()) {
                child->layoutIfNeeded();
                ASSERT(!child->needsLayout());
            } else {
                // FIXME: We should never have other type of children (they should be wrapped in an
                // anonymous table section) but our code is too crazy and this can happen in practice.
                // Until this is fixed, let's make sure we don't leave non laid out children in the tree.
                child->layoutIfNeeded();
            }
        }

        // FIXME: Collapse caption margin.
        if (!m_captions.isEmpty()) {
            for (unsigned i = 0; i < m_captions.size(); i++) {
                if (m_captions[i]->style()->captionSide() == CAPBOTTOM)
                    continue;
                layoutCaption(*m_captions[i]);
            }
            if (logicalHeight() != oldTableLogicalTop) {
                sectionMoved = true;
                movedSectionLogicalTop = std::min(logicalHeight(), oldTableLogicalTop);
            }
        }

        LayoutUnit borderAndPaddingBefore = borderBefore() + (collapsing ? LayoutUnit() : paddingBefore());
        LayoutUnit borderAndPaddingAfter = borderAfter() + (collapsing ? LayoutUnit() : paddingAfter());

        setLogicalHeight(logicalHeight() + borderAndPaddingBefore);

        LayoutUnit computedLogicalHeight = 0;

        Length logicalHeightLength = style()->logicalHeight();
        if (logicalHeightLength.isIntrinsic() || (logicalHeightLength.isSpecified() && logicalHeightLength.isPositive()))
            computedLogicalHeight = convertStyleLogicalHeightToComputedHeight(logicalHeightLength);

        Length logicalMaxHeightLength = style()->logicalMaxHeight();
        if (logicalMaxHeightLength.isIntrinsic() || (logicalMaxHeightLength.isSpecified() && !logicalMaxHeightLength.isNegative())) {
            LayoutUnit computedMaxLogicalHeight = convertStyleLogicalHeightToComputedHeight(logicalMaxHeightLength);
            computedLogicalHeight = std::min(computedLogicalHeight, computedMaxLogicalHeight);
        }

        Length logicalMinHeightLength = style()->logicalMinHeight();
        if (logicalMinHeightLength.isIntrinsic() || (logicalMinHeightLength.isSpecified() && !logicalMinHeightLength.isNegative())) {
            LayoutUnit computedMinLogicalHeight = convertStyleLogicalHeightToComputedHeight(logicalMinHeightLength);
            computedLogicalHeight = std::max(computedLogicalHeight, computedMinLogicalHeight);
        }

        distributeExtraLogicalHeight(floorToInt(computedLogicalHeight - totalSectionLogicalHeight));

        for (LayoutTableSection* section = topSection(); section; section = sectionBelow(section))
            section->layoutRows();

        if (!topSection() && computedLogicalHeight > totalSectionLogicalHeight && !document().inQuirksMode()) {
            // Completely empty tables (with no sections or anything) should at least honor specified height
            // in strict mode.
            setLogicalHeight(logicalHeight() + computedLogicalHeight);
        }

        LayoutUnit sectionLogicalLeft = style()->isLeftToRightDirection() ? borderStart() : borderEnd();
        if (!collapsing)
            sectionLogicalLeft += style()->isLeftToRightDirection() ? paddingStart() : paddingEnd();

        // position the table sections
        LayoutTableSection* section = topSection();
        while (section) {
            if (!sectionMoved && section->logicalTop() != logicalHeight()) {
                sectionMoved = true;
                movedSectionLogicalTop = std::min(logicalHeight(), section->logicalTop()) + (style()->isHorizontalWritingMode() ? section->visualOverflowRect().y() : section->visualOverflowRect().x());
            }
            section->setLogicalLocation(LayoutPoint(sectionLogicalLeft, logicalHeight()));

            // As we may skip invalidation on the table, we need to ensure that sections are invalidated when they moved.
            if (sectionMoved && !section->selfNeedsLayout())
                section->setMayNeedPaintInvalidation();

            setLogicalHeight(logicalHeight() + section->logicalHeight());

            section->updateLayerTransformAfterLayout();
            section->addVisualEffectOverflow();

            section = sectionBelow(section);
        }

        setLogicalHeight(logicalHeight() + borderAndPaddingAfter);

        for (unsigned i = 0; i < m_captions.size(); i++) {
            if (m_captions[i]->style()->captionSide() != CAPBOTTOM)
                continue;
            layoutCaption(*m_captions[i]);
        }

        updateLogicalHeight();

        // table can be containing block of positioned elements.
        bool dimensionChanged = oldLogicalWidth != logicalWidth() || oldLogicalHeight != logicalHeight();
        layoutPositionedObjects(dimensionChanged);

        updateLayerTransformAfterLayout();

        // Layout was changed, so probably borders too.
        invalidateCollapsedBorders();

        computeOverflow(clientLogicalBottom());
    }

    // FIXME: This value isn't the intrinsic content logical height, but we need
    // to update the value as its used by flexbox layout. crbug.com/367324
    setIntrinsicContentLogicalHeight(contentLogicalHeight());

    if (view()->layoutState()->pageLogicalHeight())
        setPageLogicalOffset(view()->layoutState()->pageLogicalOffset(*this, logicalTop()));

    m_columnLogicalWidthChanged = false;
    clearNeedsLayout();
}

void LayoutTable::invalidateCollapsedBorders()
{
    m_collapsedBorders.clear();
    if (!collapseBorders())
        return;

    m_collapsedBordersValid = false;
    setMayNeedPaintInvalidation();
}

// Collect all the unique border values that we want to paint in a sorted list.
// During the collection, each cell saves its recalculated borders into the cache
// of its containing section, and invalidates itself if any border changes.
// This method doesn't affect layout.
void LayoutTable::recalcCollapsedBordersIfNeeded()
{
    if (m_collapsedBordersValid || !collapseBorders())
        return;
    m_collapsedBordersValid = true;
    m_collapsedBorders.clear();
    for (LayoutObject* section = firstChild(); section; section = section->nextSibling()) {
        if (!section->isTableSection())
            continue;
        for (LayoutTableRow* row = toLayoutTableSection(section)->firstRow(); row; row = row->nextRow()) {
            for (LayoutTableCell* cell = row->firstCell(); cell; cell = cell->nextCell()) {
                ASSERT(cell->table() == this);
                cell->collectBorderValues(m_collapsedBorders);
            }
        }
    }
    LayoutTableCell::sortBorderValues(m_collapsedBorders);
}

void LayoutTable::addOverflowFromChildren()
{
    // Add overflow from borders.
    // Technically it's odd that we are incorporating the borders into layout overflow, which is only supposed to be about overflow from our
    // descendant objects, but since tables don't support overflow:auto, this works out fine.
    if (collapseBorders()) {
        int rightBorderOverflow = size().width() + outerBorderRight() - borderRight();
        int leftBorderOverflow = borderLeft() - outerBorderLeft();
        int bottomBorderOverflow = size().height() + outerBorderBottom() - borderBottom();
        int topBorderOverflow = borderTop() - outerBorderTop();
        IntRect borderOverflowRect(leftBorderOverflow, topBorderOverflow, rightBorderOverflow - leftBorderOverflow, bottomBorderOverflow - topBorderOverflow);
        if (borderOverflowRect != pixelSnappedBorderBoxRect()) {
            LayoutRect borderLayoutRect(borderOverflowRect);
            addLayoutOverflow(borderLayoutRect);
            addVisualOverflow(borderLayoutRect);
        }
    }

    // Add overflow from our caption.
    for (unsigned i = 0; i < m_captions.size(); i++)
        addOverflowFromChild(m_captions[i]);

    // Add overflow from our sections.
    for (LayoutTableSection* section = topSection(); section; section = sectionBelow(section))
        addOverflowFromChild(section);
}

void LayoutTable::paintObject(const PaintInfo& paintInfo, const LayoutPoint& paintOffset)
{
    TablePainter(*this).paintObject(paintInfo, paintOffset);
}

void LayoutTable::subtractCaptionRect(LayoutRect& rect) const
{
    for (unsigned i = 0; i < m_captions.size(); i++) {
        LayoutUnit captionLogicalHeight = m_captions[i]->logicalHeight() + m_captions[i]->marginBefore() + m_captions[i]->marginAfter();
        bool captionIsBefore = (m_captions[i]->style()->captionSide() != CAPBOTTOM) ^ style()->isFlippedBlocksWritingMode();
        if (style()->isHorizontalWritingMode()) {
            rect.setHeight(rect.height() - captionLogicalHeight);
            if (captionIsBefore)
                rect.move(0, captionLogicalHeight);
        } else {
            rect.setWidth(rect.width() - captionLogicalHeight);
            if (captionIsBefore)
                rect.move(captionLogicalHeight, 0);
        }
    }
}

void LayoutTable::paintBoxDecorationBackground(const PaintInfo& paintInfo, const LayoutPoint& paintOffset)
{
    TablePainter(*this).paintBoxDecorationBackground(paintInfo, paintOffset);
}

void LayoutTable::paintMask(const PaintInfo& paintInfo, const LayoutPoint& paintOffset)
{
    TablePainter(*this).paintMask(paintInfo, paintOffset);
}

void LayoutTable::computeIntrinsicLogicalWidths(LayoutUnit& minWidth, LayoutUnit& maxWidth) const
{
    recalcSectionsIfNeeded();
    // FIXME: Do the recalc in borderStart/borderEnd and make those const_cast this call.
    // Then m_borderStart/m_borderEnd will be transparent a cache and it removes the possibility
    // of reading out stale values.
    const_cast<LayoutTable*>(this)->recalcBordersInRowDirection();
    // FIXME: Restructure the table layout code so that we can make this method const.
    const_cast<LayoutTable*>(this)->m_tableLayout->computeIntrinsicLogicalWidths(minWidth, maxWidth);

    // FIXME: We should include captions widths here like we do in computePreferredLogicalWidths.
}

void LayoutTable::computePreferredLogicalWidths()
{
    ASSERT(preferredLogicalWidthsDirty());

    computeIntrinsicLogicalWidths(m_minPreferredLogicalWidth, m_maxPreferredLogicalWidth);

    int bordersPaddingAndSpacing = bordersPaddingAndSpacingInRowDirection();
    m_minPreferredLogicalWidth += bordersPaddingAndSpacing;
    m_maxPreferredLogicalWidth += bordersPaddingAndSpacing;

    m_tableLayout->applyPreferredLogicalWidthQuirks(m_minPreferredLogicalWidth, m_maxPreferredLogicalWidth);

    for (unsigned i = 0; i < m_captions.size(); i++)
        m_minPreferredLogicalWidth = std::max(m_minPreferredLogicalWidth, m_captions[i]->minPreferredLogicalWidth());

    const ComputedStyle& styleToUse = styleRef();
    // FIXME: This should probably be checking for isSpecified since you should be able to use percentage or calc values for min-width.
    if (styleToUse.logicalMinWidth().isFixed() && styleToUse.logicalMinWidth().value() > 0) {
        m_maxPreferredLogicalWidth = std::max(m_maxPreferredLogicalWidth, adjustContentBoxLogicalWidthForBoxSizing(styleToUse.logicalMinWidth().value()));
        m_minPreferredLogicalWidth = std::max(m_minPreferredLogicalWidth, adjustContentBoxLogicalWidthForBoxSizing(styleToUse.logicalMinWidth().value()));
    }

    // FIXME: This should probably be checking for isSpecified since you should be able to use percentage or calc values for maxWidth.
    if (styleToUse.logicalMaxWidth().isFixed()) {
        // We don't constrain m_minPreferredLogicalWidth as the table should be at least the size of its min-content, regardless of 'max-width'.
        m_maxPreferredLogicalWidth = std::min(m_maxPreferredLogicalWidth, adjustContentBoxLogicalWidthForBoxSizing(styleToUse.logicalMaxWidth().value()));
        m_maxPreferredLogicalWidth = std::max(m_minPreferredLogicalWidth, m_maxPreferredLogicalWidth);
    }

    // FIXME: We should be adding borderAndPaddingLogicalWidth here, but m_tableLayout->computePreferredLogicalWidths already does,
    // so a bunch of tests break doing this naively.
    clearPreferredLogicalWidthsDirty();
}

LayoutTableSection* LayoutTable::topNonEmptySection() const
{
    LayoutTableSection* section = topSection();
    if (section && !section->numRows())
        section = sectionBelow(section, SkipEmptySections);
    return section;
}

void LayoutTable::splitColumn(unsigned position, unsigned firstSpan)
{
    // We split the column at "position", taking "firstSpan" cells from the span.
    ASSERT(m_columns[position].span > firstSpan);
    m_columns.insert(position, ColumnStruct(firstSpan));
    m_columns[position + 1].span -= firstSpan;

    // Propagate the change in our columns representation to the sections that don't need
    // cell recalc. If they do, they will be synced up directly with m_columns later.
    for (LayoutObject* child = firstChild(); child; child = child->nextSibling()) {
        if (!child->isTableSection())
            continue;

        LayoutTableSection* section = toLayoutTableSection(child);
        if (section->needsCellRecalc())
            continue;

        section->splitColumn(position, firstSpan);
    }

    m_columnPos.grow(numEffCols() + 1);
}

void LayoutTable::appendColumn(unsigned span)
{
    unsigned newColumnIndex = m_columns.size();
    m_columns.append(ColumnStruct(span));

    // Unless the table has cell(s) with colspan that exceed the number of columns afforded
    // by the other rows in the table we can use the fast path when mapping columns to effective columns.
    m_hasCellColspanThatDeterminesTableWidth = m_hasCellColspanThatDeterminesTableWidth || span > 1;

    // Propagate the change in our columns representation to the sections that don't need
    // cell recalc. If they do, they will be synced up directly with m_columns later.
    for (LayoutObject* child = firstChild(); child; child = child->nextSibling()) {
        if (!child->isTableSection())
            continue;

        LayoutTableSection* section = toLayoutTableSection(child);
        if (section->needsCellRecalc())
            continue;

        section->appendColumn(newColumnIndex);
    }

    m_columnPos.grow(numEffCols() + 1);
}

LayoutTableCol* LayoutTable::firstColumn() const
{
    for (LayoutObject* child = firstChild(); child; child = child->nextSibling()) {
        if (child->isLayoutTableCol())
            return toLayoutTableCol(child);
    }

    return nullptr;
}

void LayoutTable::updateColumnCache() const
{
    ASSERT(m_hasColElements);
    ASSERT(m_columnLayoutObjects.isEmpty());
    ASSERT(!m_columnLayoutObjectsValid);

    for (LayoutTableCol* columnLayoutObject = firstColumn(); columnLayoutObject; columnLayoutObject = columnLayoutObject->nextColumn()) {
        if (columnLayoutObject->isTableColumnGroupWithColumnChildren())
            continue;
        m_columnLayoutObjects.append(columnLayoutObject);
    }
    m_columnLayoutObjectsValid = true;
}

LayoutTableCol* LayoutTable::slowColElement(unsigned col, bool* startEdge, bool* endEdge) const
{
    ASSERT(m_hasColElements);

    if (!m_columnLayoutObjectsValid)
        updateColumnCache();

    unsigned columnCount = 0;
    for (unsigned i = 0; i < m_columnLayoutObjects.size(); i++) {
        LayoutTableCol* columnLayoutObject = m_columnLayoutObjects[i];
        unsigned span = columnLayoutObject->span();
        unsigned startCol = columnCount;
        ASSERT(span >= 1);
        unsigned endCol = columnCount + span - 1;
        columnCount += span;
        if (columnCount > col) {
            if (startEdge)
                *startEdge = startCol == col;
            if (endEdge)
                *endEdge = endCol == col;
            return columnLayoutObject;
        }
    }
    return nullptr;
}

void LayoutTable::recalcSections() const
{
    ASSERT(m_needsSectionRecalc);

    m_head = nullptr;
    m_foot = nullptr;
    m_firstBody = nullptr;
    m_hasColElements = false;
    m_hasCellColspanThatDeterminesTableWidth = hasCellColspanThatDeterminesTableWidth();

    // We need to get valid pointers to caption, head, foot and first body again
    LayoutObject* nextSibling;
    for (LayoutObject* child = firstChild(); child; child = nextSibling) {
        nextSibling = child->nextSibling();
        switch (child->style()->display()) {
        case TABLE_COLUMN:
        case TABLE_COLUMN_GROUP:
            m_hasColElements = true;
            break;
        case TABLE_HEADER_GROUP:
            if (child->isTableSection()) {
                LayoutTableSection* section = toLayoutTableSection(child);
                if (!m_head)
                    m_head = section;
                else if (!m_firstBody)
                    m_firstBody = section;
                section->recalcCellsIfNeeded();
            }
            break;
        case TABLE_FOOTER_GROUP:
            if (child->isTableSection()) {
                LayoutTableSection* section = toLayoutTableSection(child);
                if (!m_foot)
                    m_foot = section;
                else if (!m_firstBody)
                    m_firstBody = section;
                section->recalcCellsIfNeeded();
            }
            break;
        case TABLE_ROW_GROUP:
            if (child->isTableSection()) {
                LayoutTableSection* section = toLayoutTableSection(child);
                if (!m_firstBody)
                    m_firstBody = section;
                section->recalcCellsIfNeeded();
            }
            break;
        default:
            break;
        }
    }

    // repair column count (addChild can grow it too much, because it always adds elements to the last row of a section)
    unsigned maxCols = 0;
    for (LayoutObject* child = firstChild(); child; child = child->nextSibling()) {
        if (child->isTableSection()) {
            LayoutTableSection* section = toLayoutTableSection(child);
            unsigned sectionCols = section->numColumns();
            if (sectionCols > maxCols)
                maxCols = sectionCols;
        }
    }

    m_columns.resize(maxCols);
    m_columnPos.resize(maxCols + 1);

    ASSERT(selfNeedsLayout());

    m_needsSectionRecalc = false;
}

int LayoutTable::calcBorderStart() const
{
    if (!collapseBorders())
        return LayoutBlock::borderStart();

    // Determined by the first cell of the first row. See the CSS 2.1 spec, section 17.6.2.
    if (!numEffCols())
        return 0;

    int borderWidth = 0;

    const BorderValue& tableStartBorder = style()->borderStart();
    if (tableStartBorder.style() == BHIDDEN)
        return 0;
    if (tableStartBorder.style() > BHIDDEN)
        borderWidth = tableStartBorder.width();

    if (LayoutTableCol* column = colElement(0)) {
        // FIXME: We don't account for direction on columns and column groups.
        const BorderValue& columnAdjoiningBorder = column->style()->borderStart();
        if (columnAdjoiningBorder.style() == BHIDDEN)
            return 0;
        if (columnAdjoiningBorder.style() > BHIDDEN)
            borderWidth = std::max(borderWidth, columnAdjoiningBorder.width());
        // FIXME: This logic doesn't properly account for the first column in the first column-group case.
    }

    if (const LayoutTableSection* topNonEmptySection = this->topNonEmptySection()) {
        const BorderValue& sectionAdjoiningBorder = topNonEmptySection->borderAdjoiningTableStart();
        if (sectionAdjoiningBorder.style() == BHIDDEN)
            return 0;

        if (sectionAdjoiningBorder.style() > BHIDDEN)
            borderWidth = std::max(borderWidth, sectionAdjoiningBorder.width());

        if (const LayoutTableCell* adjoiningStartCell = topNonEmptySection->firstRowCellAdjoiningTableStart()) {
            // FIXME: Make this work with perpendicular and flipped cells.
            const BorderValue& startCellAdjoiningBorder = adjoiningStartCell->borderAdjoiningTableStart();
            if (startCellAdjoiningBorder.style() == BHIDDEN)
                return 0;

            const BorderValue& firstRowAdjoiningBorder = adjoiningStartCell->row()->borderAdjoiningTableStart();
            if (firstRowAdjoiningBorder.style() == BHIDDEN)
                return 0;

            if (startCellAdjoiningBorder.style() > BHIDDEN)
                borderWidth = std::max(borderWidth, startCellAdjoiningBorder.width());
            if (firstRowAdjoiningBorder.style() > BHIDDEN)
                borderWidth = std::max(borderWidth, firstRowAdjoiningBorder.width());
        }
    }
    return (borderWidth + (style()->isLeftToRightDirection() ? 0 : 1)) / 2;
}

int LayoutTable::calcBorderEnd() const
{
    if (!collapseBorders())
        return LayoutBlock::borderEnd();

    // Determined by the last cell of the first row. See the CSS 2.1 spec, section 17.6.2.
    if (!numEffCols())
        return 0;

    int borderWidth = 0;

    const BorderValue& tableEndBorder = style()->borderEnd();
    if (tableEndBorder.style() == BHIDDEN)
        return 0;
    if (tableEndBorder.style() > BHIDDEN)
        borderWidth = tableEndBorder.width();

    unsigned endColumn = numEffCols() - 1;
    if (LayoutTableCol* column = colElement(endColumn)) {
        // FIXME: We don't account for direction on columns and column groups.
        const BorderValue& columnAdjoiningBorder = column->style()->borderEnd();
        if (columnAdjoiningBorder.style() == BHIDDEN)
            return 0;
        if (columnAdjoiningBorder.style() > BHIDDEN)
            borderWidth = std::max(borderWidth, columnAdjoiningBorder.width());
        // FIXME: This logic doesn't properly account for the last column in the last column-group case.
    }

    if (const LayoutTableSection* topNonEmptySection = this->topNonEmptySection()) {
        const BorderValue& sectionAdjoiningBorder = topNonEmptySection->borderAdjoiningTableEnd();
        if (sectionAdjoiningBorder.style() == BHIDDEN)
            return 0;

        if (sectionAdjoiningBorder.style() > BHIDDEN)
            borderWidth = std::max(borderWidth, sectionAdjoiningBorder.width());

        if (const LayoutTableCell* adjoiningEndCell = topNonEmptySection->firstRowCellAdjoiningTableEnd()) {
            // FIXME: Make this work with perpendicular and flipped cells.
            const BorderValue& endCellAdjoiningBorder = adjoiningEndCell->borderAdjoiningTableEnd();
            if (endCellAdjoiningBorder.style() == BHIDDEN)
                return 0;

            const BorderValue& firstRowAdjoiningBorder = adjoiningEndCell->row()->borderAdjoiningTableEnd();
            if (firstRowAdjoiningBorder.style() == BHIDDEN)
                return 0;

            if (endCellAdjoiningBorder.style() > BHIDDEN)
                borderWidth = std::max(borderWidth, endCellAdjoiningBorder.width());
            if (firstRowAdjoiningBorder.style() > BHIDDEN)
                borderWidth = std::max(borderWidth, firstRowAdjoiningBorder.width());
        }
    }
    return (borderWidth + (style()->isLeftToRightDirection() ? 1 : 0)) / 2;
}

void LayoutTable::recalcBordersInRowDirection()
{
    // FIXME: We need to compute the collapsed before / after borders in the same fashion.
    m_borderStart = calcBorderStart();
    m_borderEnd = calcBorderEnd();
}

int LayoutTable::borderBefore() const
{
    if (collapseBorders()) {
        recalcSectionsIfNeeded();
        return outerBorderBefore();
    }
    return LayoutBlock::borderBefore();
}

int LayoutTable::borderAfter() const
{
    if (collapseBorders()) {
        recalcSectionsIfNeeded();
        return outerBorderAfter();
    }
    return LayoutBlock::borderAfter();
}

int LayoutTable::outerBorderBefore() const
{
    if (!collapseBorders())
        return 0;
    int borderWidth = 0;
    if (LayoutTableSection* topSection = this->topSection()) {
        borderWidth = topSection->outerBorderBefore();
        if (borderWidth < 0)
            return 0; // Overridden by hidden
    }
    const BorderValue& tb = style()->borderBefore();
    if (tb.style() == BHIDDEN)
        return 0;
    if (tb.style() > BHIDDEN)
        borderWidth = std::max<int>(borderWidth, tb.width() / 2);
    return borderWidth;
}

int LayoutTable::outerBorderAfter() const
{
    if (!collapseBorders())
        return 0;
    int borderWidth = 0;

    if (LayoutTableSection* section = bottomSection()) {
        borderWidth = section->outerBorderAfter();
        if (borderWidth < 0)
            return 0; // Overridden by hidden
    }
    const BorderValue& tb = style()->borderAfter();
    if (tb.style() == BHIDDEN)
        return 0;
    if (tb.style() > BHIDDEN)
        borderWidth = std::max<int>(borderWidth, (tb.width() + 1) / 2);
    return borderWidth;
}

int LayoutTable::outerBorderStart() const
{
    if (!collapseBorders())
        return 0;

    int borderWidth = 0;

    const BorderValue& tb = style()->borderStart();
    if (tb.style() == BHIDDEN)
        return 0;
    if (tb.style() > BHIDDEN)
        borderWidth = (tb.width() + (style()->isLeftToRightDirection() ? 0 : 1)) / 2;

    bool allHidden = true;
    for (LayoutTableSection* section = topSection(); section; section = sectionBelow(section)) {
        int sw = section->outerBorderStart();
        if (sw < 0)
            continue;
        allHidden = false;
        borderWidth = std::max(borderWidth, sw);
    }
    if (allHidden)
        return 0;

    return borderWidth;
}

int LayoutTable::outerBorderEnd() const
{
    if (!collapseBorders())
        return 0;

    int borderWidth = 0;

    const BorderValue& tb = style()->borderEnd();
    if (tb.style() == BHIDDEN)
        return 0;
    if (tb.style() > BHIDDEN)
        borderWidth = (tb.width() + (style()->isLeftToRightDirection() ? 1 : 0)) / 2;

    bool allHidden = true;
    for (LayoutTableSection* section = topSection(); section; section = sectionBelow(section)) {
        int sw = section->outerBorderEnd();
        if (sw < 0)
            continue;
        allHidden = false;
        borderWidth = std::max(borderWidth, sw);
    }
    if (allHidden)
        return 0;

    return borderWidth;
}

LayoutTableSection* LayoutTable::sectionAbove(const LayoutTableSection* section, SkipEmptySectionsValue skipEmptySections) const
{
    recalcSectionsIfNeeded();

    if (section == m_head)
        return 0;

    LayoutObject* prevSection = section == m_foot ? lastChild() : section->previousSibling();
    while (prevSection) {
        if (prevSection->isTableSection() && prevSection != m_head && prevSection != m_foot && (skipEmptySections == DoNotSkipEmptySections || toLayoutTableSection(prevSection)->numRows()))
            break;
        prevSection = prevSection->previousSibling();
    }
    if (!prevSection && m_head && (skipEmptySections == DoNotSkipEmptySections || m_head->numRows()))
        prevSection = m_head;
    return toLayoutTableSection(prevSection);
}

LayoutTableSection* LayoutTable::sectionBelow(const LayoutTableSection* section, SkipEmptySectionsValue skipEmptySections) const
{
    recalcSectionsIfNeeded();

    if (section == m_foot)
        return nullptr;

    LayoutObject* nextSection = section == m_head ? firstChild() : section->nextSibling();
    while (nextSection) {
        if (nextSection->isTableSection() && nextSection != m_head && nextSection != m_foot && (skipEmptySections  == DoNotSkipEmptySections || toLayoutTableSection(nextSection)->numRows()))
            break;
        nextSection = nextSection->nextSibling();
    }
    if (!nextSection && m_foot && (skipEmptySections == DoNotSkipEmptySections || m_foot->numRows()))
        nextSection = m_foot;
    return toLayoutTableSection(nextSection);
}

LayoutTableSection* LayoutTable::bottomSection() const
{
    recalcSectionsIfNeeded();

    if (m_foot)
        return m_foot;

    for (LayoutObject* child = lastChild(); child; child = child->previousSibling()) {
        if (child->isTableSection())
            return toLayoutTableSection(child);
    }

    return nullptr;
}

LayoutTableCell* LayoutTable::cellAbove(const LayoutTableCell* cell) const
{
    recalcSectionsIfNeeded();

    // Find the section and row to look in
    unsigned r = cell->rowIndex();
    LayoutTableSection* section = nullptr;
    unsigned rAbove = 0;
    if (r > 0) {
        // cell is not in the first row, so use the above row in its own section
        section = cell->section();
        rAbove = r - 1;
    } else {
        section = sectionAbove(cell->section(), SkipEmptySections);
        if (section) {
            ASSERT(section->numRows());
            rAbove = section->numRows() - 1;
        }
    }

    // Look up the cell in the section's grid, which requires effective col index
    if (section) {
        unsigned effCol = colToEffCol(cell->col());
        LayoutTableSection::CellStruct& aboveCell = section->cellAt(rAbove, effCol);
        return aboveCell.primaryCell();
    }
    return nullptr;
}

LayoutTableCell* LayoutTable::cellBelow(const LayoutTableCell* cell) const
{
    recalcSectionsIfNeeded();

    // Find the section and row to look in
    unsigned r = cell->rowIndex() + cell->rowSpan() - 1;
    LayoutTableSection* section = nullptr;
    unsigned rBelow = 0;
    if (r < cell->section()->numRows() - 1) {
        // The cell is not in the last row, so use the next row in the section.
        section = cell->section();
        rBelow = r + 1;
    } else {
        section = sectionBelow(cell->section(), SkipEmptySections);
        if (section)
            rBelow = 0;
    }

    // Look up the cell in the section's grid, which requires effective col index
    if (section) {
        unsigned effCol = colToEffCol(cell->col());
        LayoutTableSection::CellStruct& belowCell = section->cellAt(rBelow, effCol);
        return belowCell.primaryCell();
    }
    return nullptr;
}

LayoutTableCell* LayoutTable::cellBefore(const LayoutTableCell* cell) const
{
    recalcSectionsIfNeeded();

    LayoutTableSection* section = cell->section();
    unsigned effCol = colToEffCol(cell->col());
    if (!effCol)
        return nullptr;

    // If we hit a colspan back up to a real cell.
    LayoutTableSection::CellStruct& prevCell = section->cellAt(cell->rowIndex(), effCol - 1);
    return prevCell.primaryCell();
}

LayoutTableCell* LayoutTable::cellAfter(const LayoutTableCell* cell) const
{
    recalcSectionsIfNeeded();

    unsigned effCol = colToEffCol(cell->col() + cell->colSpan());
    if (effCol >= numEffCols())
        return nullptr;
    return cell->section()->primaryCellAt(cell->rowIndex(), effCol);
}

int LayoutTable::baselinePosition(FontBaseline baselineType, bool firstLine, LineDirectionMode direction, LinePositionMode linePositionMode) const
{
    ASSERT(linePositionMode == PositionOnContainingLine);
    int baseline = firstLineBoxBaseline();
    if (baseline != -1) {
        if (isInline())
            return beforeMarginInLineDirection(direction) + baseline;
        return baseline;
    }

    return LayoutBox::baselinePosition(baselineType, firstLine, direction, linePositionMode);
}

int LayoutTable::inlineBlockBaseline(LineDirectionMode) const
{
    // Tables are skipped when computing an inline-block's baseline.
    return -1;
}

int LayoutTable::firstLineBoxBaseline() const
{
    // The baseline of a 'table' is the same as the 'inline-table' baseline per CSS 3 Flexbox (CSS 2.1
    // doesn't define the baseline of a 'table' only an 'inline-table').
    // This is also needed to properly determine the baseline of a cell if it has a table child.

    if (isWritingModeRoot())
        return -1;

    recalcSectionsIfNeeded();

    const LayoutTableSection* topNonEmptySection = this->topNonEmptySection();
    if (!topNonEmptySection)
        return -1;

    int baseline = topNonEmptySection->firstLineBoxBaseline();
    if (baseline >= 0)
        return topNonEmptySection->logicalTop() + baseline;

    // FF, Presto and IE use the top of the section as the baseline if its first row is empty of cells or content.
    // The baseline of an empty row isn't specified by CSS 2.1.
    if (topNonEmptySection->firstRow() && !topNonEmptySection->firstRow()->firstCell())
        return topNonEmptySection->logicalTop();

    return -1;
}

LayoutRect LayoutTable::overflowClipRect(const LayoutPoint& location, OverlayScrollbarSizeRelevancy relevancy) const
{
    LayoutRect rect = LayoutBlock::overflowClipRect(location, relevancy);

    // If we have a caption, expand the clip to include the caption.
    // FIXME: Technically this is wrong, but it's virtually impossible to fix this
    // for real until captions have been re-written.
    // FIXME: This code assumes (like all our other caption code) that only top/bottom are
    // supported.  When we actually support left/right and stop mapping them to top/bottom,
    // we might have to hack this code first (depending on what order we do these bug fixes in).
    if (!m_captions.isEmpty()) {
        if (style()->isHorizontalWritingMode()) {
            rect.setHeight(size().height());
            rect.setY(location.y());
        } else {
            rect.setWidth(size().width());
            rect.setX(location.x());
        }
    }

    return rect;
}

bool LayoutTable::nodeAtPoint(HitTestResult& result, const HitTestLocation& locationInContainer, const LayoutPoint& accumulatedOffset, HitTestAction action)
{
    LayoutPoint adjustedLocation = accumulatedOffset + location();

    // Check kids first.
    if (!hasOverflowClip() || locationInContainer.intersects(overflowClipRect(adjustedLocation))) {
        for (LayoutObject* child = lastChild(); child; child = child->previousSibling()) {
            if (child->isBox() && !toLayoutBox(child)->hasSelfPaintingLayer() && (child->isTableSection() || child->isTableCaption())) {
                LayoutPoint childPoint = flipForWritingModeForChild(toLayoutBox(child), adjustedLocation);
                if (child->nodeAtPoint(result, locationInContainer, childPoint, action)) {
                    updateHitTestResult(result, toLayoutPoint(locationInContainer.point() - childPoint));
                    return true;
                }
            }
        }
    }

    // Check our bounds next.
    LayoutRect boundsRect(adjustedLocation, size());
    if (visibleToHitTestRequest(result.hitTestRequest()) && (action == HitTestBlockBackground || action == HitTestChildBlockBackground) && locationInContainer.intersects(boundsRect)) {
        updateHitTestResult(result, flipForWritingMode(locationInContainer.point() - toLayoutSize(adjustedLocation)));
        if (!result.addNodeToListBasedTestResult(node(), locationInContainer, boundsRect))
            return true;
    }

    return false;
}

LayoutTable* LayoutTable::createAnonymousWithParent(const LayoutObject* parent)
{
    RefPtr<ComputedStyle> newStyle = ComputedStyle::createAnonymousStyleWithDisplay(parent->styleRef(), TABLE);
    LayoutTable* newTable = new LayoutTable(nullptr);
    newTable->setDocumentForAnonymous(&parent->document());
    newTable->setStyle(newStyle.release());
    return newTable;
}

const BorderValue& LayoutTable::tableStartBorderAdjoiningCell(const LayoutTableCell* cell) const
{
    ASSERT(cell->isFirstOrLastCellInRow());
    if (hasSameDirectionAs(cell->row()))
        return style()->borderStart();

    return style()->borderEnd();
}

const BorderValue& LayoutTable::tableEndBorderAdjoiningCell(const LayoutTableCell* cell) const
{
    ASSERT(cell->isFirstOrLastCellInRow());
    if (hasSameDirectionAs(cell->row()))
        return style()->borderEnd();

    return style()->borderStart();
}

PaintInvalidationReason LayoutTable::invalidatePaintIfNeeded(PaintInvalidationState& paintInvalidationState, const LayoutBoxModelObject& paintInvalidationContainer)
{
    // Information of collapsed borders doesn't affect layout and are for painting only.
    // Do it now instead of during painting to invalidate table cells if needed.
    recalcCollapsedBordersIfNeeded();
    return LayoutBlock::invalidatePaintIfNeeded(paintInvalidationState, paintInvalidationContainer);
}

void LayoutTable::invalidatePaintOfSubtreesIfNeeded(PaintInvalidationState& childPaintInvalidationState)
{
    if (RuntimeEnabledFeatures::slimmingPaintEnabled()) {
        // Table cells paint background from the containing column group, column, section and row.
        // If background of any of them changed, we need to invalidate all affected cells.
        // Here use shouldDoFullPaintInvalidation() as a broader condition of background change.
        for (LayoutObject* section = firstChild(); section; section = section->nextSibling()) {
            if (!section->isTableSection())
                continue;
            for (LayoutTableRow* row = toLayoutTableSection(section)->firstRow(); row; row = row->nextRow()) {
                for (LayoutTableCell* cell = row->firstCell(); cell; cell = cell->nextCell()) {
                    LayoutTableCol* column = colElement(cell->col());
                    LayoutTableCol* columnGroup = column ? column->enclosingColumnGroup() : 0;
                    if ((columnGroup && columnGroup->shouldDoFullPaintInvalidation())
                        || (column && column->shouldDoFullPaintInvalidation())
                        || section->shouldDoFullPaintInvalidation()
                        || row->shouldDoFullPaintInvalidation())
                        cell->invalidateDisplayItemClient(*cell);
                }
            }
        }
    }

    LayoutBlock::invalidatePaintOfSubtreesIfNeeded(childPaintInvalidationState);
}

}
