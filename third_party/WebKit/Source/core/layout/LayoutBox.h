/*
 * Copyright (C) 1999 Lars Knoll (knoll@kde.org)
 *           (C) 1999 Antti Koivisto (koivisto@kde.org)
 * Copyright (C) 2003, 2006, 2007 Apple Inc. All rights reserved.
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

#ifndef LayoutBox_h
#define LayoutBox_h

#include "core/CoreExport.h"
#include "core/layout/LayoutBoxModelObject.h"
#include "core/layout/OverflowModel.h"
#include "platform/scroll/ScrollTypes.h"
#include "platform/scroll/ScrollableArea.h"

namespace blink {

class LayoutBlockFlow;
class LayoutMultiColumnSpannerPlaceholder;
class ShapeOutsideInfo;

struct PaintInfo;

enum SizeType { MainOrPreferredSize, MinSize, MaxSize };
enum AvailableLogicalHeightType { ExcludeMarginBorderPadding, IncludeMarginBorderPadding };
enum OverlayScrollbarSizeRelevancy { IgnoreOverlayScrollbarSize, IncludeOverlayScrollbarSize };
enum MarginDirection { BlockDirection, InlineDirection };

enum ShouldComputePreferred { ComputeActual, ComputePreferred };

enum ScrollOffsetClamping {
    ScrollOffsetUnclamped,
    ScrollOffsetClamped
};

struct LayoutBoxRareData {
    WTF_MAKE_NONCOPYABLE(LayoutBoxRareData); WTF_MAKE_FAST_ALLOCATED(LayoutBoxRareData);
public:
    LayoutBoxRareData()
        : m_inlineBoxWrapper(nullptr)
        , m_spannerPlaceholder(nullptr)
        , m_overrideLogicalContentHeight(-1)
        , m_overrideLogicalContentWidth(-1)
        , m_previousBorderBoxSize(-1, -1)
    {
    }

    // For inline replaced elements, the inline box that owns us.
    InlineBox* m_inlineBoxWrapper;

    // For spanners, the spanner placeholder that lays us out within the multicol container.
    LayoutMultiColumnSpannerPlaceholder* m_spannerPlaceholder;

    LayoutUnit m_overrideLogicalContentHeight;
    LayoutUnit m_overrideLogicalContentWidth;

    // Set by LayoutBox::savePreviousBoxSizesIfNeeded().
    LayoutSize m_previousBorderBoxSize;
    LayoutRect m_previousContentBoxRect;
    LayoutRect m_previousLayoutOverflowRect;

    LayoutUnit m_pageLogicalOffset;
};

class CORE_EXPORT LayoutBox : public LayoutBoxModelObject {
public:
    explicit LayoutBox(ContainerNode*);

    virtual DeprecatedPaintLayerType layerTypeRequired() const override;

    virtual bool backgroundIsKnownToBeOpaqueInRect(const LayoutRect& localRect) const override;

    virtual bool backgroundShouldAlwaysBeClipped() const { return false; }

    // Use this with caution! No type checking is done!
    LayoutBox* firstChildBox() const;
    LayoutBox* lastChildBox() const;

    int pixelSnappedWidth() const { return m_frameRect.pixelSnappedWidth(); }
    int pixelSnappedHeight() const { return m_frameRect.pixelSnappedHeight(); }

//     void setX(LayoutUnit x)
//     {
//         if (x == m_frameRect.x())
//             return;
//         m_frameRect.setX(x);
//         frameRectChanged();
//     }
//     void setY(LayoutUnit y)
//     {
//         if (y == m_frameRect.y())
//             return;
//         m_frameRect.setY(y);
//         frameRectChanged();
//     }
//     void setWidth(LayoutUnit width)
//     {
//         if (width == m_frameRect.width())
//             return;
//         m_frameRect.setWidth(width);
//         frameRectChanged();
//     }
//     void setHeight(LayoutUnit height)
//     {
//         if (height == m_frameRect.height())
//             return;
//         m_frameRect.setHeight(height);
//         frameRectChanged();
//     }
// 
//     LayoutUnit logicalLeft() const { return style()->isHorizontalWritingMode() ? m_frameRect.x() : m_frameRect.y(); }
//     LayoutUnit logicalRight() const { return logicalLeft() + logicalWidth(); }
//     LayoutUnit logicalTop() const { return style()->isHorizontalWritingMode() ? m_frameRect.y() : m_frameRect.x(); }
//     LayoutUnit logicalBottom() const { return logicalTop() + logicalHeight(); }
//     LayoutUnit logicalWidth() const { return style()->isHorizontalWritingMode() ? m_frameRect.width() : m_frameRect.height(); }
//     LayoutUnit logicalHeight() const { return style()->isHorizontalWritingMode() ? m_frameRect.height() : m_frameRect.width(); }
// 
//     LayoutUnit constrainLogicalWidthByMinMax(LayoutUnit, LayoutUnit, LayoutBlock*) const;
//     LayoutUnit constrainLogicalHeightByMinMax(LayoutUnit logicalHeight, LayoutUnit intrinsicContentHeight) const;
//     LayoutUnit constrainContentBoxLogicalHeightByMinMax(LayoutUnit logicalHeight, LayoutUnit intrinsicContentHeight) const;
// 
//     int pixelSnappedLogicalHeight() const { return style()->isHorizontalWritingMode() ? pixelSnappedHeight() : pixelSnappedWidth(); }
//     int pixelSnappedLogicalWidth() const { return style()->isHorizontalWritingMode() ? pixelSnappedWidth() : pixelSnappedHeight(); }
// 
//     LayoutUnit minimumLogicalHeightForEmptyLine() const
//     {
//         return borderAndPaddingLogicalHeight() + scrollbarLogicalHeight()
//             + lineHeight(true, isHorizontalWritingMode() ? HorizontalLine : VerticalLine, PositionOfInteriorLineBoxes);
//     }
// 
//     void setLogicalLeft(LayoutUnit left)
//     {
//         if (style()->isHorizontalWritingMode())
//             setX(left);
//         else
//             setY(left);
//     }
//     void setLogicalTop(LayoutUnit top)
//     {
//         if (style()->isHorizontalWritingMode())
//             setY(top);
//         else
//             setX(top);
//     }
//     void setLogicalLocation(const LayoutPoint& location)
//     {
//         if (style()->isHorizontalWritingMode())
//             setLocation(location);
//         else
//             setLocation(location.transposedPoint());
//     }
//     void setLogicalWidth(LayoutUnit size)
//     {
//         if (style()->isHorizontalWritingMode())
//             setWidth(size);
//         else
//             setHeight(size);
//     }
//     void setLogicalHeight(LayoutUnit size)
//     {
//         if (style()->isHorizontalWritingMode())
//             setHeight(size);
//         else
//             setWidth(size);
//     }
// 
//     LayoutPoint location() const { return m_frameRect.location(); }
//     LayoutSize locationOffset() const { return LayoutSize(m_frameRect.x(), m_frameRect.y()); }
//     LayoutSize size() const { return m_frameRect.size(); }
//     IntSize pixelSnappedSize() const { return m_frameRect.pixelSnappedSize(); }
// 
//     void setLocation(const LayoutPoint& location)
//     {
//         if (location == m_frameRect.location())
//             return;
//         m_frameRect.setLocation(location);
//         frameRectChanged();
//     }
// 
//     // FIXME: Currently scrollbars are using int geometry and positioned based on
//     // pixelSnappedBorderBoxRect whose size may change when location changes because of
//     // pixel snapping. This function is used to change location of the LayoutBox outside
//     // of LayoutBox::layout(). Will remove when we use LayoutUnits for scrollbars.
//     void setLocationAndUpdateOverflowControlsIfNeeded(const LayoutPoint&);
// 
//     void setSize(const LayoutSize& size)
//     {
//         if (size == m_frameRect.size())
//             return;
//         m_frameRect.setSize(size);
//         frameRectChanged();
//     }
//     void move(LayoutUnit dx, LayoutUnit dy)
//     {
//         if (!dx && !dy)
//             return;
//         m_frameRect.move(dx, dy);
//         frameRectChanged();
//     }
// 
//     LayoutRect frameRect() const { return m_frameRect; }
//     void setFrameRect(const LayoutRect& rect)
//     {
//         if (rect == m_frameRect)
//             return;
//         m_frameRect = rect;
//         frameRectChanged();
//     }

    void setX(LayoutUnit x);
    void setY(LayoutUnit y);
    void setWidth(LayoutUnit width);
    void setHeight(LayoutUnit height);

    LayoutUnit logicalLeft() const;
    LayoutUnit logicalRight() const;
    LayoutUnit logicalTop() const;
    LayoutUnit logicalBottom() const;
    LayoutUnit logicalWidth() const;
    LayoutUnit logicalHeight() const;

    LayoutUnit constrainLogicalWidthByMinMax(LayoutUnit, LayoutUnit, LayoutBlock*) const;
    LayoutUnit constrainLogicalHeightByMinMax(LayoutUnit logicalHeight, LayoutUnit intrinsicContentHeight) const;
    LayoutUnit constrainContentBoxLogicalHeightByMinMax(LayoutUnit logicalHeight, LayoutUnit intrinsicContentHeight) const;

    int pixelSnappedLogicalHeight() const;
    int pixelSnappedLogicalWidth() const;

    LayoutUnit minimumLogicalHeightForEmptyLine() const;

    void setLogicalLeft(LayoutUnit left);
    void setLogicalTop(LayoutUnit top);
    void setLogicalLocation(const LayoutPoint& location);
    void setLogicalWidth(LayoutUnit size);
    void setLogicalHeight(LayoutUnit size);

    LayoutPoint location() const;
    LayoutSize locationOffset() const;
    LayoutSize size() const;
    IntSize pixelSnappedSize() const;

    void setLocation(const LayoutPoint& location);

    // FIXME: Currently scrollbars are using int geometry and positioned based on
    // pixelSnappedBorderBoxRect whose size may change when location changes because of
    // pixel snapping. This function is used to change location of the LayoutBox outside
    // of LayoutBox::layout(). Will remove when we use LayoutUnits for scrollbars.
    void setLocationAndUpdateOverflowControlsIfNeeded(const LayoutPoint&);

    void setSize(const LayoutSize& size);
    void move(LayoutUnit dx, LayoutUnit dy);

    LayoutRect frameRect() const;
    void setFrameRect(const LayoutRect& rect);

    LayoutRect borderBoxRect() const { return LayoutRect(LayoutPoint(), size()); }
    LayoutRect paddingBoxRect() const { return LayoutRect(borderLeft(), borderTop(), clientWidth(), clientHeight()); }
    IntRect pixelSnappedBorderBoxRect() const { return IntRect(IntPoint(), m_frameRect.pixelSnappedSize()); }
    virtual IntRect borderBoundingBox() const override final { return pixelSnappedBorderBoxRect(); }

    // The content area of the box (excludes padding - and intrinsic padding for table cells, etc... - and border).
    LayoutRect contentBoxRect() const { return LayoutRect(borderLeft() + paddingLeft(), borderTop() + paddingTop(), contentWidth(), contentHeight()); }
    LayoutSize contentBoxOffset() const { return LayoutSize(borderLeft() + paddingLeft(), borderTop() + paddingTop()); }
    // The content box in absolute coords. Ignores transforms.
    IntRect absoluteContentBox() const;
    // The offset of the content box in absolute coords, ignoring transforms.
    IntSize absoluteContentBoxOffset() const;
    // The content box converted to absolute coords (taking transforms into account).
    FloatQuad absoluteContentQuad() const;

    // This returns the content area of the box (excluding padding and border). The only difference with contentBoxRect is that computedCSSContentBoxRect
    // does include the intrinsic padding in the content box as this is what some callers expect (like getComputedStyle).
    LayoutRect computedCSSContentBoxRect() const { return LayoutRect(borderLeft() + computedCSSPaddingLeft(), borderTop() + computedCSSPaddingTop(), clientWidth() - computedCSSPaddingLeft() - computedCSSPaddingRight(), clientHeight() - computedCSSPaddingTop() - computedCSSPaddingBottom()); }

    virtual void addFocusRingRects(Vector<LayoutRect>&, const LayoutPoint& additionalOffset) const override;

    // Use this with caution! No type checking is done!
    LayoutBox* previousSiblingBox() const;
    LayoutBox* previousInFlowSiblingBox() const;
    LayoutBox* nextSiblingBox() const;
    LayoutBox* nextInFlowSiblingBox() const;
    LayoutBox* parentBox() const;

    // Return the previous sibling column set or spanner placeholder. Only to be used on multicol container children.
    LayoutBox* previousSiblingMultiColumnBox() const;
    // Return the next sibling column set or spanner placeholder. Only to be used on multicol container children.
    LayoutBox* nextSiblingMultiColumnBox() const;

    bool canResize() const;

    // Visual and layout overflow are in the coordinate space of the box.  This means that they aren't purely physical directions.
    // For horizontal-tb and vertical-lr they will match physical directions, but for horizontal-bt and vertical-rl, the top/bottom and left/right
    // respectively are flipped when compared to their physical counterparts.  For example minX is on the left in vertical-lr,
    // but it is on the right in vertical-rl.
    LayoutRect noOverflowRect() const;
    LayoutRect layoutOverflowRect() const { return m_overflow ? m_overflow->layoutOverflowRect() : noOverflowRect(); }
    IntRect pixelSnappedLayoutOverflowRect() const { return pixelSnappedIntRect(layoutOverflowRect()); }
    LayoutSize maxLayoutOverflow() const { return LayoutSize(layoutOverflowRect().maxX(), layoutOverflowRect().maxY()); }
    LayoutUnit logicalLeftLayoutOverflow() const { return style()->isHorizontalWritingMode() ? layoutOverflowRect().x() : layoutOverflowRect().y(); }
    LayoutUnit logicalRightLayoutOverflow() const { return style()->isHorizontalWritingMode() ? layoutOverflowRect().maxX() : layoutOverflowRect().maxY(); }

    virtual LayoutRect visualOverflowRect() const { return m_overflow ? m_overflow->visualOverflowRect() : borderBoxRect(); }
    LayoutUnit logicalLeftVisualOverflow() const { return style()->isHorizontalWritingMode() ? visualOverflowRect().x() : visualOverflowRect().y(); }
    LayoutUnit logicalRightVisualOverflow() const { return style()->isHorizontalWritingMode() ? visualOverflowRect().maxX() : visualOverflowRect().maxY(); }

    LayoutRect contentsVisualOverflowRect() const { return m_overflow ? m_overflow->contentsVisualOverflowRect() : LayoutRect(); }

    void addLayoutOverflow(const LayoutRect&);
    void addVisualOverflow(const LayoutRect&);

    // Clipped by the contents clip, if one exists.
    void addContentsVisualOverflow(const LayoutRect&);

    void addVisualEffectOverflow();
    LayoutRectOutsets computeVisualEffectOverflowOutsets() const;
    void addOverflowFromChild(LayoutBox* child) { addOverflowFromChild(child, child->locationOffset()); }
    void addOverflowFromChild(LayoutBox* child, const LayoutSize& delta);
    void clearLayoutOverflow();
    void clearAllOverflows() { m_overflow.clear(); }

    void updateLayerTransformAfterLayout();

    LayoutUnit contentWidth() const { return clientWidth() - paddingLeft() - paddingRight(); }
    LayoutUnit contentHeight() const { return clientHeight() - paddingTop() - paddingBottom(); }
    LayoutSize contentSize() const { return LayoutSize(contentWidth(), contentHeight()); }
    LayoutUnit contentLogicalWidth() const { return style()->isHorizontalWritingMode() ? contentWidth() : contentHeight(); }
    LayoutUnit contentLogicalHeight() const { return style()->isHorizontalWritingMode() ? contentHeight() : contentWidth(); }

    // IE extensions. Used to calculate offsetWidth/Height.  Overridden by inlines (LayoutFlow)
    // to return the remaining width on a given line (and the height of a single line).
    virtual LayoutUnit offsetWidth() const override { return m_frameRect.width(); }
    virtual LayoutUnit offsetHeight() const override { return m_frameRect.height(); }

    virtual int pixelSnappedOffsetWidth() const override final;
    virtual int pixelSnappedOffsetHeight() const override final;

    // More IE extensions.  clientWidth and clientHeight represent the interior of an object
    // excluding border and scrollbar.  clientLeft/Top are just the borderLeftWidth and borderTopWidth.
    LayoutUnit clientLeft() const { return borderLeft() + (style()->shouldPlaceBlockDirectionScrollbarOnLogicalLeft() ? verticalScrollbarWidth() : 0); }
    LayoutUnit clientTop() const { return borderTop(); }
    LayoutUnit clientWidth() const;
    LayoutUnit clientHeight() const;
    LayoutUnit clientLogicalWidth() const { return style()->isHorizontalWritingMode() ? clientWidth() : clientHeight(); }
    LayoutUnit clientLogicalHeight() const { return style()->isHorizontalWritingMode() ? clientHeight() : clientWidth(); }
    LayoutUnit clientLogicalBottom() const { return borderBefore() + clientLogicalHeight(); }
    LayoutRect clientBoxRect() const { return LayoutRect(clientLeft(), clientTop(), clientWidth(), clientHeight()); }

    int pixelSnappedClientWidth() const;
    int pixelSnappedClientHeight() const;

    // scrollWidth/scrollHeight will be the same as clientWidth/clientHeight unless the
    // object has overflow:hidden/scroll/auto specified and also has overflow.
    // scrollLeft/Top return the current scroll position.  These methods are virtual so that objects like
    // textareas can scroll shadow content (but pretend that they are the objects that are
    // scrolling).
    virtual LayoutUnit scrollLeft() const;
    virtual LayoutUnit scrollTop() const;
    virtual LayoutUnit scrollWidth() const;
    virtual LayoutUnit scrollHeight() const;
    int pixelSnappedScrollWidth() const;
    int pixelSnappedScrollHeight() const;
    virtual void setScrollLeft(LayoutUnit);
    virtual void setScrollTop(LayoutUnit);

    void scrollToOffset(const DoubleSize&, ScrollBehavior = ScrollBehaviorInstant);
    void scrollByRecursively(const DoubleSize& delta, ScrollOffsetClamping = ScrollOffsetUnclamped);
    void scrollRectToVisible(const LayoutRect&, const ScrollAlignment& alignX, const ScrollAlignment& alignY);

    virtual LayoutRectOutsets marginBoxOutsets() const override { return m_marginBoxOutsets; }
    virtual LayoutUnit marginTop() const override { return m_marginBoxOutsets.top(); }
    virtual LayoutUnit marginBottom() const override { return m_marginBoxOutsets.bottom(); }
    virtual LayoutUnit marginLeft() const override { return m_marginBoxOutsets.left(); }
    virtual LayoutUnit marginRight() const override { return m_marginBoxOutsets.right(); }
    void setMarginTop(LayoutUnit margin) { m_marginBoxOutsets.setTop(margin); }
    void setMarginBottom(LayoutUnit margin) { m_marginBoxOutsets.setBottom(margin); }
    void setMarginLeft(LayoutUnit margin) { m_marginBoxOutsets.setLeft(margin); }
    void setMarginRight(LayoutUnit margin) { m_marginBoxOutsets.setRight(margin); }

    LayoutUnit marginLogicalLeft() const { return m_marginBoxOutsets.logicalLeft(style()->writingMode()); }
    LayoutUnit marginLogicalRight() const { return m_marginBoxOutsets.logicalRight(style()->writingMode()); }

    virtual LayoutUnit marginBefore(const ComputedStyle* overrideStyle = nullptr) const override final { return m_marginBoxOutsets.before((overrideStyle ? overrideStyle : style())->writingMode()); }
    virtual LayoutUnit marginAfter(const ComputedStyle* overrideStyle = nullptr) const override final { return m_marginBoxOutsets.after((overrideStyle ? overrideStyle : style())->writingMode()); }
    virtual LayoutUnit marginStart(const ComputedStyle* overrideStyle = nullptr) const override final
    {
        const ComputedStyle* styleToUse = overrideStyle ? overrideStyle : style();
        return m_marginBoxOutsets.start(styleToUse->writingMode(), styleToUse->direction());
    }
    virtual LayoutUnit marginEnd(const ComputedStyle* overrideStyle = nullptr) const override final
    {
        const ComputedStyle* styleToUse = overrideStyle ? overrideStyle : style();
        return m_marginBoxOutsets.end(styleToUse->writingMode(), styleToUse->direction());
    }
    void setMarginBefore(LayoutUnit value, const ComputedStyle* overrideStyle = nullptr) { m_marginBoxOutsets.setBefore((overrideStyle ? overrideStyle : style())->writingMode(), value); }
    void setMarginAfter(LayoutUnit value, const ComputedStyle* overrideStyle = nullptr) { m_marginBoxOutsets.setAfter((overrideStyle ? overrideStyle : style())->writingMode(), value); }
    void setMarginStart(LayoutUnit value, const ComputedStyle* overrideStyle = nullptr)
    {
        const ComputedStyle* styleToUse = overrideStyle ? overrideStyle : style();
        m_marginBoxOutsets.setStart(styleToUse->writingMode(), styleToUse->direction(), value);
    }
    void setMarginEnd(LayoutUnit value, const ComputedStyle* overrideStyle = nullptr)
    {
        const ComputedStyle* styleToUse = overrideStyle ? overrideStyle : style();
        m_marginBoxOutsets.setEnd(styleToUse->writingMode(), styleToUse->direction(), value);
    }

    // The following functions are used to implement collapsing margins.
    // All objects know their maximal positive and negative margins.  The
    // formula for computing a collapsed margin is |maxPosMargin| - |maxNegmargin|.
    // For a non-collapsing box, such as a leaf element, this formula will simply return
    // the margin of the element.  Blocks override the maxMarginBefore and maxMarginAfter
    // methods.
    virtual bool isSelfCollapsingBlock() const { return false; }
    virtual LayoutUnit collapsedMarginBefore() const { return marginBefore(); }
    virtual LayoutUnit collapsedMarginAfter() const { return marginAfter(); }
    LayoutRectOutsets collapsedMarginBoxLogicalOutsets() const { return LayoutRectOutsets(collapsedMarginBefore(), 0, collapsedMarginAfter(), 0); }

    virtual void absoluteRects(Vector<IntRect>&, const LayoutPoint& accumulatedOffset) const override;
    virtual void absoluteQuads(Vector<FloatQuad>&, bool* wasFixed) const override;

    int reflectionOffset() const;
    // Given a rect in the object's coordinate space, returns the corresponding rect in the reflection.
    LayoutRect reflectedRect(const LayoutRect&) const;

    virtual void layout() override;
    virtual void paint(const PaintInfo&, const LayoutPoint&) override;
    virtual bool nodeAtPoint(HitTestResult&, const HitTestLocation& locationInContainer, const LayoutPoint& accumulatedOffset, HitTestAction) override;

    virtual LayoutUnit minPreferredLogicalWidth() const override;
    virtual LayoutUnit maxPreferredLogicalWidth() const override;

    // FIXME: We should rename these back to overrideLogicalHeight/Width and have them store
    // the border-box height/width like the regular height/width accessors on LayoutBox.
    // Right now, these are different than contentHeight/contentWidth because they still
    // include the scrollbar height/width.
    LayoutUnit overrideLogicalContentWidth() const;
    LayoutUnit overrideLogicalContentHeight() const;
    bool hasOverrideLogicalContentHeight() const;
    bool hasOverrideLogicalContentWidth() const;
    void setOverrideLogicalContentHeight(LayoutUnit);
    void setOverrideLogicalContentWidth(LayoutUnit);
    void clearOverrideSize();
    void clearOverrideLogicalContentHeight();
    void clearOverrideLogicalContentWidth();

    LayoutUnit overrideContainingBlockContentLogicalWidth() const;
    LayoutUnit overrideContainingBlockContentLogicalHeight() const;
    bool hasOverrideContainingBlockLogicalWidth() const;
    bool hasOverrideContainingBlockLogicalHeight() const;
    void setOverrideContainingBlockContentLogicalWidth(LayoutUnit);
    void setOverrideContainingBlockContentLogicalHeight(LayoutUnit);
    void clearContainingBlockOverrideSize();
    void clearOverrideContainingBlockContentLogicalHeight();

    LayoutUnit extraInlineOffset() const;
    LayoutUnit extraBlockOffset() const;
    void setExtraInlineOffset(LayoutUnit inlineOffest);
    void setExtraBlockOffset(LayoutUnit blockOffest);
    void clearExtraInlineAndBlockOffests();

    virtual LayoutSize offsetFromContainer(const LayoutObject*, const LayoutPoint&, bool* offsetDependsOnPoint = nullptr) const override;

    LayoutUnit adjustBorderBoxLogicalWidthForBoxSizing(LayoutUnit width) const;
    LayoutUnit adjustBorderBoxLogicalHeightForBoxSizing(LayoutUnit height) const;
    LayoutUnit adjustContentBoxLogicalWidthForBoxSizing(LayoutUnit width) const;
    LayoutUnit adjustContentBoxLogicalHeightForBoxSizing(LayoutUnit height) const;

    struct ComputedMarginValues {
        ComputedMarginValues() { }

        LayoutUnit m_before;
        LayoutUnit m_after;
        LayoutUnit m_start;
        LayoutUnit m_end;
    };
    struct LogicalExtentComputedValues {
        LogicalExtentComputedValues() { }

        LayoutUnit m_extent;
        LayoutUnit m_position;
        ComputedMarginValues m_margins;
    };
    // Resolve auto margins in the chosen direction of the containing block so that objects can be pushed to the start, middle or end
    // of the containing block.
    void computeMarginsForDirection(MarginDirection forDirection, const LayoutBlock* containingBlock, LayoutUnit containerWidth, LayoutUnit childWidth, LayoutUnit& marginStart, LayoutUnit& marginEnd, Length marginStartLength, Length marginStartEnd) const;

    // Used to resolve margins in the containing block's block-flow direction.
    void computeAndSetBlockDirectionMargins(const LayoutBlock* containingBlock);

    LayoutUnit offsetFromLogicalTopOfFirstPage() const;

    // The page logical offset is the object's offset from the top of the page in the page progression
    // direction (so an x-offset in vertical text and a y-offset for horizontal text).
    LayoutUnit pageLogicalOffset() const { return m_rareData ? m_rareData->m_pageLogicalOffset : LayoutUnit(); }
    void setPageLogicalOffset(LayoutUnit);

    void positionLineBox(InlineBox*);
    void moveWithEdgeOfInlineContainerIfNecessary(bool isHorizontal);

    virtual InlineBox* createInlineBox();
    void dirtyLineBoxes(bool fullLayout);

    // For inline replaced elements, this function returns the inline box that owns us.  Enables
    // the replaced LayoutObject to quickly determine what line it is contained on and to easily
    // iterate over structures on the line.
    InlineBox* inlineBoxWrapper() const { return m_rareData ? m_rareData->m_inlineBoxWrapper : 0; }
    void setInlineBoxWrapper(InlineBox*);
    void deleteLineBoxWrapper();

    void setSpannerPlaceholder(LayoutMultiColumnSpannerPlaceholder&);
    void clearSpannerPlaceholder();
    virtual LayoutMultiColumnSpannerPlaceholder* spannerPlaceholder() const final { return m_rareData ? m_rareData->m_spannerPlaceholder : 0; }

    virtual LayoutRect clippedOverflowRectForPaintInvalidation(const LayoutBoxModelObject* paintInvalidationContainer, const PaintInvalidationState* = nullptr) const override;
    virtual void mapRectToPaintInvalidationBacking(const LayoutBoxModelObject* paintInvalidationContainer, LayoutRect&, const PaintInvalidationState*) const override;
    virtual void invalidatePaintForOverhangingFloats(bool paintAllDescendants);

    LayoutUnit containingBlockLogicalHeightForGetComputedStyle() const;

    virtual LayoutUnit containingBlockLogicalWidthForContent() const override;
    LayoutUnit containingBlockLogicalHeightForContent(AvailableLogicalHeightType) const;

    LayoutUnit containingBlockAvailableLineWidth() const;
    LayoutUnit perpendicularContainingBlockLogicalHeight() const;

    virtual void updateLogicalWidth();
    void updateLogicalHeight();
    virtual void computeLogicalHeight(LayoutUnit logicalHeight, LayoutUnit logicalTop, LogicalExtentComputedValues&) const;
    // This function will compute the logical border-box height, without laying out the box. This means that the result
    // is only "correct" when the height is explicitly specified. This function exists so that intrinsic width calculations
    // have a way to deal with children that have orthogonal flows.
    // When there is no explicit height, this function assumes a content height of zero (and returns just border+padding)
    LayoutUnit computeLogicalHeightWithoutLayout() const;

    void computeLogicalWidth(LogicalExtentComputedValues&) const;

    bool stretchesToViewport() const
    {
        return document().inQuirksMode() && style()->logicalHeight().isAuto() && !isFloatingOrOutOfFlowPositioned() && (isDocumentElement() || isBody()) && !isInline();
    }

    virtual LayoutSize intrinsicSize() const { return LayoutSize(); }
    LayoutUnit intrinsicLogicalWidth() const { return style()->isHorizontalWritingMode() ? intrinsicSize().width() : intrinsicSize().height(); }
    LayoutUnit intrinsicLogicalHeight() const { return style()->isHorizontalWritingMode() ? intrinsicSize().height() : intrinsicSize().width(); }
    virtual LayoutUnit intrinsicContentLogicalHeight() const { return m_intrinsicContentLogicalHeight; }

    // Whether or not the element shrinks to its intrinsic width (rather than filling the width
    // of a containing block).  HTML4 buttons, <select>s, <input>s, legends, and floating/compact elements do this.
    bool sizesLogicalWidthToFitContent(const Length& logicalWidth) const;

    LayoutUnit shrinkLogicalWidthToAvoidFloats(LayoutUnit childMarginStart, LayoutUnit childMarginEnd, const LayoutBlockFlow* cb) const;

    LayoutUnit computeLogicalWidthUsing(SizeType, const Length& logicalWidth, LayoutUnit availableLogicalWidth, const LayoutBlock* containingBlock) const;
    LayoutUnit computeLogicalHeightUsing(SizeType, const Length& height, LayoutUnit intrinsicContentHeight) const;
    LayoutUnit computeContentLogicalHeight(SizeType, const Length& height, LayoutUnit intrinsicContentHeight) const;
    LayoutUnit computeContentAndScrollbarLogicalHeightUsing(SizeType, const Length& height, LayoutUnit intrinsicContentHeight) const;
    LayoutUnit computeReplacedLogicalWidthUsing(SizeType, const Length& width) const;
    LayoutUnit computeReplacedLogicalWidthRespectingMinMaxWidth(LayoutUnit logicalWidth, ShouldComputePreferred  = ComputeActual) const;
    LayoutUnit computeReplacedLogicalHeightUsing(SizeType, const Length& height) const;
    LayoutUnit computeReplacedLogicalHeightRespectingMinMaxHeight(LayoutUnit logicalHeight) const;

    virtual LayoutUnit computeReplacedLogicalWidth(ShouldComputePreferred  = ComputeActual) const;
    virtual LayoutUnit computeReplacedLogicalHeight() const;

    bool hasDefiniteLogicalWidth() const;
    static bool percentageLogicalHeightIsResolvableFromBlock(const LayoutBlock* containingBlock, bool outOfFlowPositioned);
    bool hasDefiniteLogicalHeight() const;
    LayoutUnit computePercentageLogicalHeight(const Length& height) const;

    // Block flows subclass availableWidth/Height to handle multi column layout (shrinking the width/height available to children when laying out.)
    LayoutUnit availableLogicalWidth() const { return contentLogicalWidth(); }
    LayoutUnit availableLogicalHeight(AvailableLogicalHeightType) const;
    LayoutUnit availableLogicalHeightUsing(const Length&, AvailableLogicalHeightType) const;

    // There are a few cases where we need to refer specifically to the available physical width and available physical height.
    // Relative positioning is one of those cases, since left/top offsets are physical.
    LayoutUnit availableWidth() const { return style()->isHorizontalWritingMode() ? availableLogicalWidth() : availableLogicalHeight(IncludeMarginBorderPadding); }
    LayoutUnit availableHeight() const { return style()->isHorizontalWritingMode() ? availableLogicalHeight(IncludeMarginBorderPadding) : availableLogicalWidth(); }

    virtual int verticalScrollbarWidth() const;
    int horizontalScrollbarHeight() const;
    int intrinsicScrollbarLogicalWidth() const;
    int scrollbarLogicalWidth() const { return style()->isHorizontalWritingMode() ? verticalScrollbarWidth() : horizontalScrollbarHeight(); }
    int scrollbarLogicalHeight() const { return style()->isHorizontalWritingMode() ? horizontalScrollbarHeight() : verticalScrollbarWidth(); }
    virtual ScrollResultOneDimensional scroll(ScrollDirectionPhysical, ScrollGranularity, float delta = 1);
    bool canBeScrolledAndHasScrollableArea() const;
    virtual bool canBeProgramaticallyScrolled() const;
    virtual void autoscroll(const IntPoint&);
    bool canAutoscroll() const;
    IntSize calculateAutoscrollDirection(const IntPoint& pointInRootFrame) const;
    static LayoutBox* findAutoscrollable(LayoutObject*);
    virtual void stopAutoscroll() { }
    virtual void panScroll(const IntPoint&);

    bool hasAutoVerticalScrollbar() const { return hasOverflowClip() && (style()->overflowY() == OAUTO || style()->overflowY() == OPAGEDY || style()->overflowY() == OOVERLAY); }
    bool hasAutoHorizontalScrollbar() const { return hasOverflowClip() && (style()->overflowX() == OAUTO || style()->overflowX() == OOVERLAY); }
    bool scrollsOverflow() const { return scrollsOverflowX() || scrollsOverflowY(); }

    bool hasScrollableOverflowX() const { return scrollsOverflowX() && pixelSnappedScrollWidth() != pixelSnappedClientWidth(); }
    bool hasScrollableOverflowY() const { return scrollsOverflowY() && pixelSnappedScrollHeight() != pixelSnappedClientHeight(); }
    virtual bool scrollsOverflowX() const { return hasOverflowClip() && (style()->overflowX() == OSCROLL || hasAutoHorizontalScrollbar()); }
    virtual bool scrollsOverflowY() const { return hasOverflowClip() && (style()->overflowY() == OSCROLL || hasAutoVerticalScrollbar()); }
    bool usesCompositedScrolling() const;

    // Elements such as the <input> field override this to specify that they are scrollable
    // outside the context of the CSS overflow style
    virtual bool isIntrinsicallyScrollable(ScrollbarOrientation orientation) const { return false; }

    bool hasUnsplittableScrollingOverflow() const;
    bool isUnsplittableForPagination() const;

    virtual LayoutRect localCaretRect(InlineBox*, int caretOffset, LayoutUnit* extraWidthToEndOfLine = nullptr) override;

    virtual LayoutRect overflowClipRect(const LayoutPoint& location, OverlayScrollbarSizeRelevancy = IgnoreOverlayScrollbarSize) const;
    LayoutRect clipRect(const LayoutPoint& location);
    virtual bool hasControlClip() const { return false; }
    virtual LayoutRect controlClipRect(const LayoutPoint&) const { return LayoutRect(); }

    virtual void paintBoxDecorationBackground(const PaintInfo&, const LayoutPoint&);
    virtual void paintMask(const PaintInfo&, const LayoutPoint&);
    virtual void imageChanged(WrappedImagePtr, const IntRect* = nullptr) override;

    void logicalExtentAfterUpdatingLogicalWidth(const LayoutUnit& logicalTop, LogicalExtentComputedValues&);

    virtual PositionWithAffinity positionForPoint(const LayoutPoint&) override;

    void removeFloatingOrPositionedChildFromBlockLists();

    DeprecatedPaintLayer* enclosingFloatPaintingLayer() const;

    virtual int firstLineBoxBaseline() const { return -1; }
    virtual int inlineBlockBaseline(LineDirectionMode) const { return -1; } // Returns -1 if we should skip this box when computing the baseline of an inline-block.

    bool shrinkToAvoidFloats() const;
    virtual bool avoidsFloats() const;

    virtual void markForPaginationRelayoutIfNeeded(SubtreeLayoutScope&);

    bool isWritingModeRoot() const { return !parent() || parent()->style()->writingMode() != style()->writingMode(); }

    bool isDeprecatedFlexItem() const { return !isInline() && !isFloatingOrOutOfFlowPositioned() && parent() && parent()->isDeprecatedFlexibleBox(); }
    bool isFlexItemIncludingDeprecated() const { return !isInline() && !isFloatingOrOutOfFlowPositioned() && parent() && parent()->isFlexibleBoxIncludingDeprecated(); }

    bool isGridItem() const { return parent() && parent()->isLayoutGrid(); }

    virtual LayoutUnit lineHeight(bool firstLine, LineDirectionMode, LinePositionMode = PositionOnContainingLine) const override;
    virtual int baselinePosition(FontBaseline, bool firstLine, LineDirectionMode, LinePositionMode = PositionOnContainingLine) const override;

    virtual LayoutUnit offsetLeft() const override;
    virtual LayoutUnit offsetTop() const override;

    LayoutPoint flipForWritingModeForChild(const LayoutBox* child, const LayoutPoint&) const;
    LayoutUnit flipForWritingMode(LayoutUnit position) const WARN_UNUSED_RETURN {
        // The offset is in the block direction (y for horizontal writing modes, x for vertical writing modes).
        if (!UNLIKELY(hasFlippedBlocksWritingMode()))
            return position;
        return logicalHeight() - position;
    }
    LayoutPoint flipForWritingMode(const LayoutPoint& position) const WARN_UNUSED_RETURN {
        if (!UNLIKELY(hasFlippedBlocksWritingMode()))
            return position;
        return isHorizontalWritingMode() ? LayoutPoint(position.x(), m_frameRect.height() - position.y()) : LayoutPoint(m_frameRect.width() - position.x(), position.y());
    }
    LayoutSize flipForWritingMode(const LayoutSize& offset) const WARN_UNUSED_RETURN {
        if (!UNLIKELY(hasFlippedBlocksWritingMode()))
            return offset;
        return isHorizontalWritingMode() ? LayoutSize(offset.width(), m_frameRect.height() - offset.height()) : LayoutSize(m_frameRect.width() - offset.width(), offset.height());
    }
    void flipForWritingMode(LayoutRect& rect) const
    {
        if (!UNLIKELY(hasFlippedBlocksWritingMode()))
            return;
        if (isHorizontalWritingMode())
            rect.setY(m_frameRect.height() - rect.maxY());
        else
            rect.setX(m_frameRect.width() - rect.maxX());
    }
    FloatPoint flipForWritingMode(const FloatPoint& position) const WARN_UNUSED_RETURN {
        if (!UNLIKELY(hasFlippedBlocksWritingMode()))
            return position;
        return isHorizontalWritingMode() ? FloatPoint(position.x(), m_frameRect.height() - position.y()) : FloatPoint(m_frameRect.width() - position.x(), position.y());
    }
    void flipForWritingMode(FloatRect& rect) const
    {
        if (!UNLIKELY(hasFlippedBlocksWritingMode()))
            return;
        if (isHorizontalWritingMode())
            rect.setY(m_frameRect.height() - rect.maxY());
        else
            rect.setX(m_frameRect.width() - rect.maxX());
    }
    // These represent your location relative to your container as a physical offset.
    // In layout related methods you almost always want the logical location (e.g. x() and y()).
    LayoutPoint topLeftLocation() const;
    LayoutSize topLeftLocationOffset() const { return toLayoutSize(topLeftLocation()); }

    LayoutRect logicalVisualOverflowRectForPropagation(const ComputedStyle&) const;
    LayoutRect visualOverflowRectForPropagation(const ComputedStyle&) const;
    LayoutRect logicalLayoutOverflowRectForPropagation(const ComputedStyle&) const;
    LayoutRect layoutOverflowRectForPropagation(const ComputedStyle&) const;

    bool hasOverflowModel() const { return m_overflow; }
    bool hasVisualOverflow() const { return m_overflow && !borderBoxRect().contains(m_overflow->visualOverflowRect()); }

    virtual bool needsPreferredWidthsRecalculation() const;
    virtual void computeIntrinsicRatioInformation(FloatSize& /* intrinsicSize */, double& /* intrinsicRatio */) const { }

    IntSize scrolledContentOffset() const;
    void applyCachedClipAndScrollOffsetForPaintInvalidation(LayoutRect& paintRect) const;

    virtual bool hasRelativeLogicalWidth() const;
    virtual bool hasRelativeLogicalHeight() const;

    bool hasHorizontalLayoutOverflow() const
    {
        if (!m_overflow)
            return false;

        LayoutRect layoutOverflowRect = m_overflow->layoutOverflowRect();
        LayoutRect noOverflowRect = this->noOverflowRect();
        return layoutOverflowRect.x() < noOverflowRect.x() || layoutOverflowRect.maxX() > noOverflowRect.maxX();
    }

    bool hasVerticalLayoutOverflow() const
    {
        if (!m_overflow)
            return false;

        LayoutRect layoutOverflowRect = m_overflow->layoutOverflowRect();
        LayoutRect noOverflowRect = this->noOverflowRect();
        return layoutOverflowRect.y() < noOverflowRect.y() || layoutOverflowRect.maxY() > noOverflowRect.maxY();
    }

    virtual LayoutBox* createAnonymousBoxWithSameTypeAs(const LayoutObject*) const
    {
        ASSERT_NOT_REACHED();
        return nullptr;
    }

    bool hasSameDirectionAs(const LayoutBox* object) const { return style()->direction() == object->style()->direction(); }

    ShapeOutsideInfo* shapeOutsideInfo() const;

    void markShapeOutsideDependentsForLayout()
    {
        if (isFloating())
            removeFloatingOrPositionedChildFromBlockLists();
    }

    void setIntrinsicContentLogicalHeight(LayoutUnit intrinsicContentLogicalHeight) const { m_intrinsicContentLogicalHeight = intrinsicContentLogicalHeight; }

    bool canRenderBorderImage() const;

protected:
    virtual void willBeDestroyed() override;

    virtual void styleWillChange(StyleDifference, const ComputedStyle& newStyle) override;
    virtual void styleDidChange(StyleDifference, const ComputedStyle* oldStyle) override;
    virtual void updateFromStyle() override;

    // Returns false if it could not cheaply compute the extent (e.g. fixed background), in which case the returned rect may be incorrect.
    // FIXME: make this a const method once the LayoutBox reference in BoxPainter is const.
    bool getBackgroundPaintedExtent(LayoutRect&);
    virtual bool foregroundIsKnownToBeOpaqueInRect(const LayoutRect& localRect, unsigned maxDepthToTest) const;
    virtual bool computeBackgroundIsKnownToBeObscured() override;

    void computePositionedLogicalWidth(LogicalExtentComputedValues&) const;

    LayoutUnit computeIntrinsicLogicalWidthUsing(const Length& logicalWidthLength, LayoutUnit availableLogicalWidth, LayoutUnit borderAndPadding) const;
    LayoutUnit computeIntrinsicLogicalContentHeightUsing(const Length& logicalHeightLength, LayoutUnit intrinsicContentHeight, LayoutUnit borderAndPadding) const;

    virtual bool shouldComputeSizeAsReplaced() const { return isReplaced() && !isInlineBlockOrInlineTable(); }

    virtual void mapLocalToContainer(const LayoutBoxModelObject* paintInvalidationContainer, TransformState&, MapCoordinatesFlags = ApplyContainerFlip, bool* wasFixed = nullptr, const PaintInvalidationState* = nullptr) const override;
    virtual void mapAbsoluteToLocalPoint(MapCoordinatesFlags, TransformState&) const override;

    LayoutObject* splitAnonymousBoxesAroundChild(LayoutObject* beforeChild);

    virtual void addLayerHitTestRects(LayerHitTestRects&, const DeprecatedPaintLayer* currentCompositedLayer, const LayoutPoint& layerOffset, const LayoutRect& containerRect) const override;
    virtual void computeSelfHitTestRects(Vector<LayoutRect>&, const LayoutPoint& layerOffset) const override;

    virtual PaintInvalidationReason paintInvalidationReason(const LayoutBoxModelObject& paintInvalidationContainer,
        const LayoutRect& oldBounds, const LayoutPoint& oldPositionFromPaintInvalidationContainer,
        const LayoutRect& newBounds, const LayoutPoint& newPositionFromPaintInvalidationContainer) const override;
    virtual void incrementallyInvalidatePaint(const LayoutBoxModelObject& paintInvalidationContainer, const LayoutRect& oldBounds, const LayoutRect& newBounds, const LayoutPoint& positionFromPaintInvalidationContainer) override;

    virtual void clearPaintInvalidationState(const PaintInvalidationState&) override;
#if ENABLE(ASSERT)
    virtual bool paintInvalidationStateIsDirty() const override;
#endif

    virtual PaintInvalidationReason invalidatePaintIfNeeded(PaintInvalidationState&, const LayoutBoxModelObject& newPaintInvalidationContainer) override;

    virtual bool hasNonCompositedScrollbars() const override final;

private:
    bool mustInvalidateBackgroundOrBorderPaintOnHeightChange() const;
    bool mustInvalidateBackgroundOrBorderPaintOnWidthChange() const;
    inline bool mustInvalidateFillLayersPaintOnWidthChange(const FillLayer&) const;

    void invalidatePaintRectClippedByOldAndNewBounds(const LayoutBoxModelObject& paintInvalidationContainer, const LayoutRect&, const LayoutRect& oldBounds, const LayoutRect& newBounds);

    void updateShapeOutsideInfoAfterStyleChange(const ComputedStyle&, const ComputedStyle* oldStyle);
    void updateGridPositionAfterStyleChange(const ComputedStyle*);

    bool autoWidthShouldFitContent() const;
    LayoutUnit shrinkToFitLogicalWidth(LayoutUnit availableLogicalWidth, LayoutUnit bordersPlusPadding) const;

    // Returns true if we queued up a paint invalidation.
    bool invalidatePaintOfLayerRectsForImage(WrappedImagePtr, const FillLayer&, bool drawingBackground);

    bool skipContainingBlockForPercentHeightCalculation(const LayoutBox* containingBlock) const;

    LayoutUnit containingBlockLogicalWidthForPositioned(const LayoutBoxModelObject* containingBlock, bool checkForPerpendicularWritingMode = true) const;
    LayoutUnit containingBlockLogicalHeightForPositioned(const LayoutBoxModelObject* containingBlock, bool checkForPerpendicularWritingMode = true) const;

    void computePositionedLogicalHeight(LogicalExtentComputedValues&) const;
    void computePositionedLogicalWidthUsing(SizeType, Length logicalWidth, const LayoutBoxModelObject* containerBlock, TextDirection containerDirection,
        LayoutUnit containerLogicalWidth, LayoutUnit bordersPlusPadding,
        const Length& logicalLeft, const Length& logicalRight, const Length& marginLogicalLeft,
        const Length& marginLogicalRight, LogicalExtentComputedValues&) const;
    void computePositionedLogicalHeightUsing(SizeType, Length logicalHeightLength, const LayoutBoxModelObject* containerBlock,
        LayoutUnit containerLogicalHeight, LayoutUnit bordersPlusPadding, LayoutUnit logicalHeight,
        const Length& logicalTop, const Length& logicalBottom, const Length& marginLogicalTop,
        const Length& marginLogicalBottom, LogicalExtentComputedValues&) const;

    void computePositionedLogicalHeightReplaced(LogicalExtentComputedValues&) const;
    void computePositionedLogicalWidthReplaced(LogicalExtentComputedValues&) const;

    LayoutUnit fillAvailableMeasure(LayoutUnit availableLogicalWidth) const;
    LayoutUnit fillAvailableMeasure(LayoutUnit availableLogicalWidth, LayoutUnit& marginStart, LayoutUnit& marginEnd) const;

    virtual void computeIntrinsicLogicalWidths(LayoutUnit& minLogicalWidth, LayoutUnit& maxLogicalWidth) const;

    // This function calculates the minimum and maximum preferred widths for an object.
    // These values are used in shrink-to-fit layout systems.
    // These include tables, positioned objects, floats and flexible boxes.
    virtual void computePreferredLogicalWidths() { clearPreferredLogicalWidthsDirty(); }

    LayoutBoxRareData& ensureRareData()
    {
        if (!m_rareData)
            m_rareData = adoptPtr(new LayoutBoxRareData());
        return *m_rareData.get();
    }

    bool needToSavePreviousBoxSizes();
    void savePreviousBoxSizesIfNeeded();
    LayoutSize computePreviousBorderBoxSize(const LayoutSize& previousBoundsSize) const;

    bool logicalHeightComputesAsNone(SizeType) const;

    bool isBox() const = delete; // This will catch anyone doing an unnecessary check.

    void frameRectChanged()
    {
        // The frame rect may change because of layout of other objects.
        // Should check this object for paint invalidation.
        if (!needsLayout())
            setMayNeedPaintInvalidation();
    }

    // Returns true if the box intersects the viewport visible to the user.
    bool intersectsVisibleViewport();

    void updateSlowRepaintStatusAfterStyleChange();

    // The width/height of the contents + borders + padding.  The x/y location is relative to our container (which is not always our parent).
    LayoutRect m_frameRect;

    // Our intrinsic height, used for min-height: min-content etc. Maintained by
    // updateLogicalHeight. This is logicalHeight() before it is clamped to
    // min/max.
    mutable LayoutUnit m_intrinsicContentLogicalHeight;

    void inflatePaintInvalidationRectForReflectionAndFilter(LayoutRect&) const;

    LayoutRectOutsets m_marginBoxOutsets;

protected:
    // The preferred logical width of the element if it were to break its lines at every possible opportunity.
    LayoutUnit m_minPreferredLogicalWidth;

    // The preferred logical width of the element if it never breaks any lines at all.
    LayoutUnit m_maxPreferredLogicalWidth;

    // Our overflow information.
    OwnPtr<OverflowModel> m_overflow;

private:
    OwnPtr<LayoutBoxRareData> m_rareData;
};

DEFINE_LAYOUT_OBJECT_TYPE_CASTS(LayoutBox, isBox());

inline LayoutBox* LayoutBox::previousSiblingBox() const
{
    return toLayoutBox(previousSibling());
}

inline LayoutBox* LayoutBox::previousInFlowSiblingBox() const
{
    LayoutBox* previous = previousSiblingBox();
    while (previous && previous->isOutOfFlowPositioned())
        previous = previous->previousSiblingBox();
    return previous;
}

inline LayoutBox* LayoutBox::nextSiblingBox() const
{
    return toLayoutBox(nextSibling());
}

inline LayoutBox* LayoutBox::nextInFlowSiblingBox() const
{
    LayoutBox* next = nextSiblingBox();
    while (next && next->isOutOfFlowPositioned())
        next = next->nextSiblingBox();
    return next;
}

inline LayoutBox* LayoutBox::parentBox() const
{
    return toLayoutBox(parent());
}

inline LayoutBox* LayoutBox::firstChildBox() const
{
    LayoutObject* ret = slowFirstChild();
    if (ret) {
        if (!ret || !(ret->isBox())) {
            return nullptr;
        }
        return static_cast<LayoutBox*>(ret);
    }
    return nullptr;
}

inline LayoutBox* LayoutBox::lastChildBox() const
{
    return toLayoutBox(slowLastChild());
}

inline LayoutBox* LayoutBox::previousSiblingMultiColumnBox() const
{
    ASSERT(isLayoutMultiColumnSpannerPlaceholder() || isLayoutMultiColumnSet());
    LayoutBox* previousBox = previousSiblingBox();
    if (previousBox->isLayoutFlowThread())
        return nullptr;
    return previousBox;
}

inline LayoutBox* LayoutBox::nextSiblingMultiColumnBox() const
{
    ASSERT(isLayoutMultiColumnSpannerPlaceholder() || isLayoutMultiColumnSet());
    return nextSiblingBox();
}

inline void LayoutBox::setInlineBoxWrapper(InlineBox* boxWrapper)
{
    if (boxWrapper) {
        ASSERT(!inlineBoxWrapper());
        // m_inlineBoxWrapper should already be 0. Deleting it is a safeguard against security issues.
        // Otherwise, there will two line box wrappers keeping the reference to this layoutObject, and
        // only one will be notified when the layoutObject is getting destroyed. The second line box wrapper
        // will keep a stale reference.
        if (UNLIKELY(inlineBoxWrapper() != nullptr))
            deleteLineBoxWrapper();
    }

    ensureRareData().m_inlineBoxWrapper = boxWrapper;
}

} // namespace blink

#endif // LayoutBox_h
