/*
 * Copyright (C) 1999 Lars Knoll (knoll@kde.org)
 * Copyright (C) 2000 Dirk Mueller (mueller@kde.org)
 * Copyright (C) 2004, 2006, 2007 Apple Inc. All rights reserved.
 * Copyright (C) Research In Motion Limited 2011-2012. All rights reserved.
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
#include "core/layout/LayoutReplaced.h"

#include "core/editing/PositionWithAffinity.h"
#include "core/layout/LayoutAnalyzer.h"
#include "core/layout/LayoutBlock.h"
#include "core/layout/LayoutImage.h"
#include "core/layout/LayoutView.h"
#include "core/paint/DeprecatedPaintLayer.h"
#include "core/paint/PaintInfo.h"
#include "core/paint/ReplacedPainter.h"
#include "platform/LengthFunctions.h"

#ifndef MINIBLINK_NO_CHANGE
#include "wke/wkeGlobalVar.h"
#endif

namespace blink {

const int LayoutReplaced::defaultWidth = 300;
const int LayoutReplaced::defaultHeight = 150;

LayoutReplaced::LayoutReplaced(Element* element)
    : LayoutBox(element)
    , m_intrinsicSize(defaultWidth, defaultHeight)
{
    setReplaced(true);
}

LayoutReplaced::LayoutReplaced(Element* element, const LayoutSize& intrinsicSize)
    : LayoutBox(element)
    , m_intrinsicSize(intrinsicSize)
{
    setReplaced(true);
}

LayoutReplaced::~LayoutReplaced()
{
}

void LayoutReplaced::willBeDestroyed()
{
    if (!documentBeingDestroyed() && parent())
        parent()->dirtyLinesFromChangedChild(this);

    LayoutBox::willBeDestroyed();
}

void LayoutReplaced::styleDidChange(StyleDifference diff, const ComputedStyle* oldStyle)
{
    LayoutBox::styleDidChange(diff, oldStyle);

    bool hadStyle = (oldStyle != 0);
    float oldZoom = hadStyle ? oldStyle->effectiveZoom() : ComputedStyle::initialZoom();
    if (style() && style()->effectiveZoom() != oldZoom)
        intrinsicSizeChanged();
}

void LayoutReplaced::layout()
{
    ASSERT(needsLayout());
    LayoutAnalyzer::Scope analyzer(*this);

    LayoutRect oldContentRect = replacedContentRect();

    setHeight(minimumReplacedHeight());

    updateLogicalWidth();
    updateLogicalHeight();

    m_overflow.clear();
    addVisualEffectOverflow();
    updateLayerTransformAfterLayout();
    invalidateBackgroundObscurationStatus();

    clearNeedsLayout();

    if (replacedContentRect() != oldContentRect)
        setShouldDoFullPaintInvalidation();
}

void LayoutReplaced::intrinsicSizeChanged()
{
    int scaledWidth = static_cast<int>(defaultWidth * style()->effectiveZoom());
    int scaledHeight = static_cast<int>(defaultHeight * style()->effectiveZoom());
    m_intrinsicSize = LayoutSize(scaledWidth, scaledHeight);
    setNeedsLayoutAndPrefWidthsRecalcAndFullPaintInvalidation(LayoutInvalidationReason::SizeChanged);
}

void LayoutReplaced::paint(const PaintInfo& paintInfo, const LayoutPoint& paintOffset)
{
    ReplacedPainter(*this).paint(paintInfo, paintOffset);
}

bool LayoutReplaced::shouldPaint(const PaintInfo& paintInfo, const LayoutPoint& paintOffset) const
{
    if (paintInfo.phase != PaintPhaseForeground && paintInfo.phase != PaintPhaseOutline && paintInfo.phase != PaintPhaseSelfOutline
        && paintInfo.phase != PaintPhaseSelection && paintInfo.phase != PaintPhaseMask && paintInfo.phase != PaintPhaseClippingMask)
        return false;

    if (!paintInfo.shouldPaintWithinRoot(this))
        return false;

    // if we're invisible or haven't received a layout yet, then just bail.
    if (style()->visibility() != VISIBLE)
        return false;

    LayoutRect paintRect(visualOverflowRect());
    paintRect.moveBy(paintOffset + location());

    // Early exit if the element touches the edges.
    LayoutUnit top = paintRect.y();
    LayoutUnit bottom = paintRect.maxY();
    if (isSelected() && inlineBoxWrapper()) {
        LayoutUnit selTop = paintOffset.y() + inlineBoxWrapper()->root().selectionTop();
        LayoutUnit selBottom = paintOffset.y() + selTop + inlineBoxWrapper()->root().selectionHeight();
        top = std::min(selTop, top);
        bottom = std::max(selBottom, bottom);
    }

    if (paintRect.x() >= paintInfo.rect.maxX() || paintRect.maxX() <= paintInfo.rect.x())
        return false;

    if (top >= paintInfo.rect.maxY() || bottom <= paintInfo.rect.y())
        return false;

    return true;
}

bool LayoutReplaced::hasReplacedLogicalHeight() const
{
    if (style()->logicalHeight().isAuto())
        return false;

    if (style()->logicalHeight().isSpecified()) {
        if (hasAutoHeightOrContainingBlockWithAutoHeight())
            return false;
        return true;
    }

    if (style()->logicalHeight().isIntrinsic())
        return true;

    return false;
}

bool LayoutReplaced::needsPreferredWidthsRecalculation() const
{
    // If the height is a percentage and the width is auto, then the containingBlocks's height changing can cause
    // this node to change it's preferred width because it maintains aspect ratio.
    return hasRelativeLogicalHeight() && style()->logicalWidth().isAuto() && !hasAutoHeightOrContainingBlockWithAutoHeight();
}

static inline bool layoutObjectHasAspectRatio(const LayoutObject* layoutObject)
{
    ASSERT(layoutObject);
    return layoutObject->isImage() || layoutObject->isCanvas() || layoutObject->isVideo();
}

void LayoutReplaced::computeAspectRatioInformationForLayoutBox(LayoutBox* contentLayoutObject, FloatSize& constrainedSize, double& intrinsicRatio) const
{
    FloatSize intrinsicSize;
    if (contentLayoutObject) {
        contentLayoutObject->computeIntrinsicRatioInformation(intrinsicSize, intrinsicRatio);

        // Handle zoom & vertical writing modes here, as the embedded document doesn't know about them.
        intrinsicSize.scale(style()->effectiveZoom());
        if (isLayoutImage())
            intrinsicSize.scale(toLayoutImage(this)->imageDevicePixelRatio());

        // Update our intrinsic size to match what the content layoutObject has computed, so that when we
        // constrain the size below, the correct intrinsic size will be obtained for comparison against
        // min and max widths.
        if (intrinsicRatio && !intrinsicSize.isEmpty())
            m_intrinsicSize = LayoutSize(intrinsicSize);

        if (!isHorizontalWritingMode()) {
            if (intrinsicRatio)
                intrinsicRatio = 1 / intrinsicRatio;
            intrinsicSize = intrinsicSize.transposedSize();
        }
    } else {
        computeIntrinsicRatioInformation(intrinsicSize, intrinsicRatio);
        if (intrinsicRatio && !intrinsicSize.isEmpty())
            m_intrinsicSize = LayoutSize(isHorizontalWritingMode() ? intrinsicSize : intrinsicSize.transposedSize());
    }

    // Now constrain the intrinsic size along each axis according to minimum and maximum width/heights along the
    // opposite axis. So for example a maximum width that shrinks our width will result in the height we compute here
    // having to shrink in order to preserve the aspect ratio. Because we compute these values independently along
    // each axis, the final returned size may in fact not preserve the aspect ratio.
    // FIXME: In the long term, it might be better to just return this code more to the way it used to be before this
    // function was added, since all it has done is make the code more unclear.
    constrainedSize = intrinsicSize;
    if (intrinsicRatio && !intrinsicSize.isEmpty() && style()->logicalWidth().isAuto() && style()->logicalHeight().isAuto()) {
        // We can't multiply or divide by 'intrinsicRatio' here, it breaks tests, like fast/images/zoomed-img-size.html, which
        // can only be fixed once subpixel precision is available for things like intrinsicWidth/Height - which include zoom!
        constrainedSize.setWidth(LayoutBox::computeReplacedLogicalHeight() * intrinsicSize.width() / intrinsicSize.height());
        constrainedSize.setHeight(LayoutBox::computeReplacedLogicalWidth() * intrinsicSize.height() / intrinsicSize.width());
    }
}

LayoutRect LayoutReplaced::replacedContentRect(const LayoutSize* overriddenIntrinsicSize) const
{
    LayoutRect contentRect = contentBoxRect();
    ObjectFit objectFit = style()->objectFit();

    if (objectFit == ObjectFitFill && style()->objectPosition() == ComputedStyle::initialObjectPosition()) {
        return contentRect;
    }

#ifndef MINIBLINK_NO_CHANGE
    String layoutName = name();
    if (wke::g_wkeMediaPlayerFactory && layoutName == "LayoutVideo")
        objectFit = ObjectFitFill;
#endif

    // TODO(davve): intrinsicSize doubles as both intrinsic size and intrinsic ratio. In the case of
    // SVG images this isn't correct since they can have intrinsic ratio but no intrinsic size. In
    // order to maintain aspect ratio, the intrinsic size for SVG might be faked from the aspect
    // ratio, see SVGImage::containerSize().
    LayoutSize intrinsicSize = overriddenIntrinsicSize ? *overriddenIntrinsicSize : this->intrinsicSize();
    if (!intrinsicSize.width() || !intrinsicSize.height())
        return contentRect;

    LayoutRect finalRect = contentRect;
    switch (objectFit) {
    case ObjectFitContain:
    case ObjectFitScaleDown:
    case ObjectFitCover:
        finalRect.setSize(finalRect.size().fitToAspectRatio(intrinsicSize, objectFit == ObjectFitCover ? AspectRatioFitGrow : AspectRatioFitShrink));
        if (objectFit != ObjectFitScaleDown || finalRect.width() <= intrinsicSize.width())
            break;
        // fall through
    case ObjectFitNone:
        finalRect.setSize(intrinsicSize);
        break;
    case ObjectFitFill:
        break;
    default:
        ASSERT_NOT_REACHED();
    }

    LayoutUnit xOffset = minimumValueForLength(style()->objectPosition().x(), contentRect.width() - finalRect.width());
    LayoutUnit yOffset = minimumValueForLength(style()->objectPosition().y(), contentRect.height() - finalRect.height());
    finalRect.move(xOffset, yOffset);

    return finalRect;
}

void LayoutReplaced::computeIntrinsicRatioInformation(FloatSize& intrinsicSize, double& intrinsicRatio) const
{
    // If there's an embeddedContentBox() of a remote, referenced document available, this code-path should never be used.
    ASSERT(!embeddedContentBox());
    intrinsicSize = FloatSize(intrinsicLogicalWidth().toFloat(), intrinsicLogicalHeight().toFloat());

    // Figure out if we need to compute an intrinsic ratio.
    if (intrinsicSize.isEmpty() || !layoutObjectHasAspectRatio(this))
        return;

    intrinsicRatio = intrinsicSize.width() / intrinsicSize.height();
}

LayoutUnit LayoutReplaced::computeReplacedLogicalWidth(ShouldComputePreferred shouldComputePreferred) const
{
    if (style()->logicalWidth().isSpecified() || style()->logicalWidth().isIntrinsic())
        return computeReplacedLogicalWidthRespectingMinMaxWidth(computeReplacedLogicalWidthUsing(MainOrPreferredSize, style()->logicalWidth()), shouldComputePreferred);

    LayoutBox* contentLayoutObject = embeddedContentBox();

    // 10.3.2 Inline, replaced elements: http://www.w3.org/TR/CSS21/visudet.html#inline-replaced-width
    double intrinsicRatio = 0;
    FloatSize constrainedSize;
    computeAspectRatioInformationForLayoutBox(contentLayoutObject, constrainedSize, intrinsicRatio);

    if (style()->logicalWidth().isAuto()) {
        bool computedHeightIsAuto = hasAutoHeightOrContainingBlockWithAutoHeight();
        bool hasIntrinsicWidth = constrainedSize.width() > 0;

        // If 'height' and 'width' both have computed values of 'auto' and the element also has an intrinsic width, then that intrinsic width is the used value of 'width'.
        if (computedHeightIsAuto && hasIntrinsicWidth)
            return computeReplacedLogicalWidthRespectingMinMaxWidth(constrainedSize.width(), shouldComputePreferred);

        bool hasIntrinsicHeight = constrainedSize.height() > 0;
        if (intrinsicRatio) {
            // If 'height' and 'width' both have computed values of 'auto' and the element has no intrinsic width, but does have an intrinsic height and intrinsic ratio;
            // or if 'width' has a computed value of 'auto', 'height' has some other computed value, and the element does have an intrinsic ratio; then the used value
            // of 'width' is: (used height) * (intrinsic ratio)
            if (intrinsicRatio && ((computedHeightIsAuto && !hasIntrinsicWidth && hasIntrinsicHeight) || !computedHeightIsAuto)) {
                LayoutUnit logicalHeight = computeReplacedLogicalHeight();
                return computeReplacedLogicalWidthRespectingMinMaxWidth(roundToInt(round(logicalHeight * intrinsicRatio)), shouldComputePreferred);
            }

            // If 'height' and 'width' both have computed values of 'auto' and the element has an intrinsic ratio but no intrinsic height or width, then the used value of
            // 'width' is undefined in CSS 2.1. However, it is suggested that, if the containing block's width does not itself depend on the replaced element's width, then
            // the used value of 'width' is calculated from the constraint equation used for block-level, non-replaced elements in normal flow.
            if (computedHeightIsAuto && !hasIntrinsicWidth && !hasIntrinsicHeight) {
                if (shouldComputePreferred == ComputePreferred)
                    return 0;
                // The aforementioned 'constraint equation' used for block-level, non-replaced elements in normal flow:
                // 'margin-left' + 'border-left-width' + 'padding-left' + 'width' + 'padding-right' + 'border-right-width' + 'margin-right' = width of containing block
                LayoutUnit logicalWidth = containingBlock()->availableLogicalWidth();

                // This solves above equation for 'width' (== logicalWidth).
                LayoutUnit marginStart = minimumValueForLength(style()->marginStart(), logicalWidth);
                LayoutUnit marginEnd = minimumValueForLength(style()->marginEnd(), logicalWidth);
                logicalWidth = std::max<LayoutUnit>(0, logicalWidth - (marginStart + marginEnd + (size().width() - clientWidth())));
                return computeReplacedLogicalWidthRespectingMinMaxWidth(logicalWidth, shouldComputePreferred);
            }
        }

        // Otherwise, if 'width' has a computed value of 'auto', and the element has an intrinsic width, then that intrinsic width is the used value of 'width'.
        if (hasIntrinsicWidth)
            return computeReplacedLogicalWidthRespectingMinMaxWidth(constrainedSize.width(), shouldComputePreferred);

        // Otherwise, if 'width' has a computed value of 'auto', but none of the conditions above are met, then the used value of 'width' becomes 300px. If 300px is too
        // wide to fit the device, UAs should use the width of the largest rectangle that has a 2:1 ratio and fits the device instead.
        // Note: We fall through and instead return intrinsicLogicalWidth() here - to preserve existing WebKit behavior, which might or might not be correct, or desired.
        // Changing this to return cDefaultWidth, will affect lots of test results. Eg. some tests assume that a blank <img> tag (which implies width/height=auto)
        // has no intrinsic size, which is wrong per CSS 2.1, but matches our behavior since a long time.
    }

    return computeReplacedLogicalWidthRespectingMinMaxWidth(intrinsicLogicalWidth(), shouldComputePreferred);
}

LayoutUnit LayoutReplaced::computeReplacedLogicalHeight() const
{
    // 10.5 Content height: the 'height' property: http://www.w3.org/TR/CSS21/visudet.html#propdef-height
    if (hasReplacedLogicalHeight())
        return computeReplacedLogicalHeightRespectingMinMaxHeight(computeReplacedLogicalHeightUsing(MainOrPreferredSize, style()->logicalHeight()));

    LayoutBox* contentLayoutObject = embeddedContentBox();

    // 10.6.2 Inline, replaced elements: http://www.w3.org/TR/CSS21/visudet.html#inline-replaced-height
    double intrinsicRatio = 0;
    FloatSize constrainedSize;
    computeAspectRatioInformationForLayoutBox(contentLayoutObject, constrainedSize, intrinsicRatio);

    bool widthIsAuto = style()->logicalWidth().isAuto();
    bool hasIntrinsicHeight = constrainedSize.height() > 0;

    // If 'height' and 'width' both have computed values of 'auto' and the element also has an intrinsic height, then that intrinsic height is the used value of 'height'.
    if (widthIsAuto && hasIntrinsicHeight)
        return computeReplacedLogicalHeightRespectingMinMaxHeight(constrainedSize.height());

    // Otherwise, if 'height' has a computed value of 'auto', and the element has an intrinsic ratio then the used value of 'height' is:
    // (used width) / (intrinsic ratio)
    if (intrinsicRatio)
        return computeReplacedLogicalHeightRespectingMinMaxHeight(roundToInt(round(availableLogicalWidth() / intrinsicRatio)));

    // Otherwise, if 'height' has a computed value of 'auto', and the element has an intrinsic height, then that intrinsic height is the used value of 'height'.
    if (hasIntrinsicHeight)
        return computeReplacedLogicalHeightRespectingMinMaxHeight(constrainedSize.height());

    // Otherwise, if 'height' has a computed value of 'auto', but none of the conditions above are met, then the used value of 'height' must be set to the height
    // of the largest rectangle that has a 2:1 ratio, has a height not greater than 150px, and has a width not greater than the device width.
    return computeReplacedLogicalHeightRespectingMinMaxHeight(intrinsicLogicalHeight());
}

void LayoutReplaced::computeIntrinsicLogicalWidths(LayoutUnit& minLogicalWidth, LayoutUnit& maxLogicalWidth) const
{
    minLogicalWidth = maxLogicalWidth = intrinsicLogicalWidth();
}

void LayoutReplaced::computePreferredLogicalWidths()
{
    ASSERT(preferredLogicalWidthsDirty());

    // We cannot resolve any percent logical width here as the available logical
    // width may not be set on our containing block.
    if (style()->logicalWidth().hasPercent())
        computeIntrinsicLogicalWidths(m_minPreferredLogicalWidth, m_maxPreferredLogicalWidth);
    else
        m_minPreferredLogicalWidth = m_maxPreferredLogicalWidth = computeReplacedLogicalWidth(ComputePreferred);

    const ComputedStyle& styleToUse = styleRef();
    if (styleToUse.logicalWidth().hasPercent() || styleToUse.logicalMaxWidth().hasPercent())
        m_minPreferredLogicalWidth = 0;

    if (styleToUse.logicalMinWidth().isFixed() && styleToUse.logicalMinWidth().value() > 0) {
        m_maxPreferredLogicalWidth = std::max(m_maxPreferredLogicalWidth, adjustContentBoxLogicalWidthForBoxSizing(styleToUse.logicalMinWidth().value()));
        m_minPreferredLogicalWidth = std::max(m_minPreferredLogicalWidth, adjustContentBoxLogicalWidthForBoxSizing(styleToUse.logicalMinWidth().value()));
    }

    if (styleToUse.logicalMaxWidth().isFixed()) {
        m_maxPreferredLogicalWidth = std::min(m_maxPreferredLogicalWidth, adjustContentBoxLogicalWidthForBoxSizing(styleToUse.logicalMaxWidth().value()));
        m_minPreferredLogicalWidth = std::min(m_minPreferredLogicalWidth, adjustContentBoxLogicalWidthForBoxSizing(styleToUse.logicalMaxWidth().value()));
    }

    LayoutUnit borderAndPadding = borderAndPaddingLogicalWidth();
    m_minPreferredLogicalWidth += borderAndPadding;
    m_maxPreferredLogicalWidth += borderAndPadding;

    clearPreferredLogicalWidthsDirty();
}

PositionWithAffinity LayoutReplaced::positionForPoint(const LayoutPoint& point)
{
    // FIXME: This code is buggy if the replaced element is relative positioned.
    InlineBox* box = inlineBoxWrapper();
    RootInlineBox* rootBox = box ? &box->root() : 0;

    LayoutUnit top = rootBox ? rootBox->selectionTop() : logicalTop();
    LayoutUnit bottom = rootBox ? rootBox->selectionBottom() : logicalBottom();

    LayoutUnit blockDirectionPosition = isHorizontalWritingMode() ? point.y() + location().y() : point.x() + location().x();
    LayoutUnit lineDirectionPosition = isHorizontalWritingMode() ? point.x() + location().x() : point.y() + location().y();

    if (blockDirectionPosition < top)
        return createPositionWithAffinity(caretMinOffset(), DOWNSTREAM); // coordinates are above

    if (blockDirectionPosition >= bottom)
        return createPositionWithAffinity(caretMaxOffset(), DOWNSTREAM); // coordinates are below

    if (node()) {
        if (lineDirectionPosition <= logicalLeft() + (logicalWidth() / 2))
            return createPositionWithAffinity(0, DOWNSTREAM);
        return createPositionWithAffinity(1, DOWNSTREAM);
    }

    return LayoutBox::positionForPoint(point);
}

LayoutRect LayoutReplaced::selectionRectForPaintInvalidation(const LayoutBoxModelObject* paintInvalidationContainer) const
{
    ASSERT(!needsLayout());

    if (!isSelected())
        return LayoutRect();

    LayoutRect rect = localSelectionRect();
    mapRectToPaintInvalidationBacking(paintInvalidationContainer, rect, 0);
    // FIXME: groupedMapping() leaks the squashing abstraction.
    if (paintInvalidationContainer->layer()->groupedMapping())
        DeprecatedPaintLayer::mapRectToPaintBackingCoordinates(paintInvalidationContainer, rect);
    return rect;
}

LayoutRect LayoutReplaced::localSelectionRect(bool checkWhetherSelected) const
{
    if (checkWhetherSelected && !isSelected())
        return LayoutRect();

    if (!inlineBoxWrapper()) {
        // We're a block-level replaced element.  Just return our own dimensions.
        return LayoutRect(LayoutPoint(), size());
    }

    RootInlineBox& root = inlineBoxWrapper()->root();
    LayoutUnit newLogicalTop = root.block().style()->isFlippedBlocksWritingMode() ? inlineBoxWrapper()->logicalBottom() - root.selectionBottom() : root.selectionTop() - inlineBoxWrapper()->logicalTop();
    if (root.block().style()->isHorizontalWritingMode())
        return LayoutRect(0, newLogicalTop, size().width(), root.selectionHeight());
    return LayoutRect(newLogicalTop, 0, root.selectionHeight(), size().height());
}

void LayoutReplaced::setSelectionState(SelectionState state)
{
    // The selection state for our containing block hierarchy is updated by the base class call.
    LayoutBox::setSelectionState(state);

    if (!inlineBoxWrapper())
        return;

    // We only include the space below the baseline in our layer's cached paint invalidation rect if the
    // image is selected. Since the selection state has changed update the rect.
    if (hasLayer())
        setPreviousPaintInvalidationRect(boundsRectForPaintInvalidation(containerForPaintInvalidation()));

    if (canUpdateSelectionOnRootLineBoxes())
        inlineBoxWrapper()->root().setHasSelectedChildren(isSelected());
}

bool LayoutReplaced::isSelected() const
{
    SelectionState s = selectionState();
    if (s == SelectionNone)
        return false;
    if (s == SelectionInside)
        return true;

    int selectionStart, selectionEnd;
    selectionStartEnd(selectionStart, selectionEnd);
    if (s == SelectionStart)
        return selectionStart == 0;

    int end = node()->hasChildren() ? node()->countChildren() : 1;
    if (s == SelectionEnd)
        return selectionEnd == end;
    if (s == SelectionBoth)
        return selectionStart == 0 && selectionEnd == end;

    ASSERT(0);
    return false;
}
LayoutRect LayoutReplaced::clippedOverflowRectForPaintInvalidation(const LayoutBoxModelObject* paintInvalidationContainer, const PaintInvalidationState* paintInvalidationState) const
{
    if (style()->visibility() != VISIBLE && !enclosingLayer()->hasVisibleContent())
        return LayoutRect();

    // The selectionRect can project outside of the overflowRect, so take their union
    // for paint invalidation to avoid selection painting glitches.
    LayoutRect r = isSelected() ? localSelectionRect() : visualOverflowRect();
    mapRectToPaintInvalidationBacking(paintInvalidationContainer, r, paintInvalidationState);
    return r;
}

}
