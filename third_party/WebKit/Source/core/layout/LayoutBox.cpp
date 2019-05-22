/*
 * Copyright (C) 1999 Lars Knoll (knoll@kde.org)
 *           (C) 1999 Antti Koivisto (koivisto@kde.org)
 *           (C) 2005 Allan Sandfeld Jensen (kde@carewolf.com)
 *           (C) 2005, 2006 Samuel Weinig (sam.weinig@gmail.com)
 * Copyright (C) 2005, 2006, 2007, 2008, 2009, 2010 Apple Inc. All rights reserved.
 * Copyright (C) 2013 Adobe Systems Incorporated. All rights reserved.
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
#include "core/layout/LayoutBox.h"

#include "core/HTMLNames.h"
#include "core/dom/Document.h"
#include "core/editing/htmlediting.h"
#include "core/frame/FrameHost.h"
#include "core/frame/FrameView.h"
#include "core/frame/LocalFrame.h"
#include "core/frame/PinchViewport.h"
#include "core/frame/Settings.h"
#include "core/html/HTMLElement.h"
#include "core/html/HTMLFrameElementBase.h"
#include "core/html/HTMLFrameOwnerElement.h"
#include "core/input/EventHandler.h"
#include "core/layout/HitTestResult.h"
#include "core/layout/LayoutAnalyzer.h"
#include "core/layout/LayoutDeprecatedFlexibleBox.h"
#include "core/layout/LayoutFlexibleBox.h"
#include "core/layout/LayoutGeometryMap.h"
#include "core/layout/LayoutGrid.h"
#include "core/layout/LayoutInline.h"
#include "core/layout/LayoutListBox.h"
#include "core/layout/LayoutListMarker.h"
#include "core/layout/LayoutMultiColumnSpannerPlaceholder.h"
#include "core/layout/LayoutPart.h"
#include "core/layout/LayoutScrollbarPart.h"
#include "core/layout/LayoutTableCell.h"
#include "core/layout/LayoutView.h"
#include "core/layout/compositing/DeprecatedPaintLayerCompositor.h"
#include "core/layout/shapes/ShapeOutsideInfo.h"
#include "core/page/AutoscrollController.h"
#include "core/page/Page.h"
#include "core/paint/BackgroundImageGeometry.h"
#include "core/paint/BoxPainter.h"
#include "core/paint/DeprecatedPaintLayer.h"
#include "core/style/ShadowList.h"
#include "platform/LengthFunctions.h"
#include "platform/geometry/DoubleRect.h"
#include "platform/geometry/FloatQuad.h"
#include "platform/geometry/FloatRoundedRect.h"
#include "platform/geometry/TransformState.h"
#include "platform/graphics/GraphicsContext.h"
#include "platform/graphics/paint/DisplayItemList.h"
#include <algorithm>
#include <math.h>

namespace blink {

using namespace HTMLNames;

// Used by flexible boxes when flexing this element and by table cells.
typedef WTF::HashMap<const LayoutBox*, LayoutUnit> OverrideSizeMap;

// Used by grid elements to properly size their grid items.
// FIXME: Move these into LayoutBoxRareData.
static OverrideSizeMap* gOverrideContainingBlockLogicalHeightMap = nullptr;
static OverrideSizeMap* gOverrideContainingBlockLogicalWidthMap = nullptr;
static OverrideSizeMap* gExtraInlineOffsetMap = nullptr;
static OverrideSizeMap* gExtraBlockOffsetMap = nullptr;


// Size of border belt for autoscroll. When mouse pointer in border belt,
// autoscroll is started.
static const int autoscrollBeltSize = 20;
static const unsigned backgroundObscurationTestMaxDepth = 4;

LayoutBox::LayoutBox(ContainerNode* node)
    : LayoutBoxModelObject(node)
    , m_intrinsicContentLogicalHeight(-1)
    , m_minPreferredLogicalWidth(-1)
    , m_maxPreferredLogicalWidth(-1)
{
    setIsBox();
}

DeprecatedPaintLayerType LayoutBox::layerTypeRequired() const
{
    // hasAutoZIndex only returns true if the element is positioned or a flex-item since
    // position:static elements that are not flex-items get their z-index coerced to auto.
    if (isPositioned() || createsGroup() || hasClipPath() || hasTransformRelatedProperty()
        || style()->hasCompositorProxy() || hasHiddenBackface() || hasReflection() || style()->specifiesColumns()
        || !style()->hasAutoZIndex() || style()->shouldCompositeForCurrentAnimations())
        return NormalDeprecatedPaintLayer;

    // Ensure that explicit use of scroll-blocks-on creates a Layer (since we might need
    // it to be composited).
    if (style()->hasScrollBlocksOn()) {
        if (isDocumentElement()) {
            ASSERT(style()->scrollBlocksOn() == view()->style()->scrollBlocksOn());
            return NoDeprecatedPaintLayer;
        }
        return NormalDeprecatedPaintLayer;
    }
    if (hasOverflowClip())
        return OverflowClipDeprecatedPaintLayer;

    return NoDeprecatedPaintLayer;
}

void LayoutBox::willBeDestroyed()
{
    clearOverrideSize();
    clearContainingBlockOverrideSize();
    clearExtraInlineAndBlockOffests();

    LayoutBlock::removePercentHeightDescendantIfNeeded(this);

    ShapeOutsideInfo::removeInfo(*this);

    LayoutBoxModelObject::willBeDestroyed();
}

void LayoutBox::removeFloatingOrPositionedChildFromBlockLists()
{
    ASSERT(isFloatingOrOutOfFlowPositioned());

    if (documentBeingDestroyed())
        return;

    if (isFloating()) {
        LayoutBlockFlow* parentBlockFlow = nullptr;
        for (LayoutObject* curr = parent(); curr && !curr->isLayoutView(); curr = curr->parent()) {
            if (curr->isLayoutBlockFlow()) {
                LayoutBlockFlow* currBlockFlow = toLayoutBlockFlow(curr);
                if (!parentBlockFlow || currBlockFlow->containsFloat(this))
                    parentBlockFlow = currBlockFlow;
            }
        }

        if (parentBlockFlow) {
            parentBlockFlow->markSiblingsWithFloatsForLayout(this);
            parentBlockFlow->markAllDescendantsWithFloatsForLayout(this, false);
        }
    }

    if (isOutOfFlowPositioned())
        LayoutBlock::removePositionedObject(this);
}

void LayoutBox::styleWillChange(StyleDifference diff, const ComputedStyle& newStyle)
{
    const ComputedStyle* oldStyle = style();
    if (oldStyle) {
        // The background of the root element or the body element could propagate up to
        // the canvas. Just dirty the entire canvas when our style changes substantially.
        if ((diff.needsPaintInvalidation() || diff.needsLayout()) && node()
            && (isHTMLHtmlElement(*node()) || isHTMLBodyElement(*node()))) {
            view()->setShouldDoFullPaintInvalidation();

            if (oldStyle->hasEntirelyFixedBackground() != newStyle.hasEntirelyFixedBackground())
                view()->compositor()->setNeedsUpdateFixedBackground();
        }

        // When a layout hint happens and an object's position style changes, we have to do a layout
        // to dirty the layout tree using the old position value now.
        if (diff.needsFullLayout() && parent() && oldStyle->position() != newStyle.position()) {
            markContainerChainForLayout();
            if (oldStyle->position() == StaticPosition)
                setShouldDoFullPaintInvalidation();
            else if (newStyle.hasOutOfFlowPosition())
                parent()->setChildNeedsLayout();
            if (isFloating() && !isOutOfFlowPositioned() && newStyle.hasOutOfFlowPosition())
                removeFloatingOrPositionedChildFromBlockLists();
        }
    // FIXME: This branch runs when !oldStyle, which means that layout was never called
    // so what's the point in invalidating the whole view that we never painted?
    } else if (isBody()) {
        view()->setShouldDoFullPaintInvalidation();
    }

    LayoutBoxModelObject::styleWillChange(diff, newStyle);
}

void LayoutBox::styleDidChange(StyleDifference diff, const ComputedStyle* oldStyle)
{
    // Horizontal writing mode definition is updated in LayoutBoxModelObject::updateFromStyle,
    // (as part of the LayoutBoxModelObject::styleDidChange call below). So, we can safely cache the horizontal
    // writing mode value before style change here.
    bool oldHorizontalWritingMode = isHorizontalWritingMode();

    LayoutBoxModelObject::styleDidChange(diff, oldStyle);

    const ComputedStyle& newStyle = styleRef();
    if (needsLayout() && oldStyle)
        LayoutBlock::removePercentHeightDescendantIfNeeded(this);

    if (LayoutBlock::hasPercentHeightContainerMap() && slowFirstChild()
        && oldHorizontalWritingMode != isHorizontalWritingMode())
        LayoutBlock::clearPercentHeightDescendantsFrom(this);

    // If our zoom factor changes and we have a defined scrollLeft/Top, we need to adjust that value into the
    // new zoomed coordinate space.
    if (hasOverflowClip() && oldStyle && oldStyle->effectiveZoom() != newStyle.effectiveZoom() && layer()) {
        if (int left = layer()->scrollableArea()->scrollXOffset()) {
            left = (left / oldStyle->effectiveZoom()) * newStyle.effectiveZoom();
            layer()->scrollableArea()->scrollToXOffset(left);
        }
        if (int top = layer()->scrollableArea()->scrollYOffset()) {
            top = (top / oldStyle->effectiveZoom()) * newStyle.effectiveZoom();
            layer()->scrollableArea()->scrollToYOffset(top);
        }
    }

    // Our opaqueness might have changed without triggering layout.
    if (diff.needsPaintInvalidation()) {
        LayoutObject* parentToInvalidate = parent();
        for (unsigned i = 0; i < backgroundObscurationTestMaxDepth && parentToInvalidate; ++i) {
            parentToInvalidate->invalidateBackgroundObscurationStatus();
            parentToInvalidate = parentToInvalidate->parent();
        }
    }

    if (isDocumentElement() || isBody()) {
        document().view()->recalculateScrollbarOverlayStyle();
        document().view()->recalculateCustomScrollbarStyle();
    }
    updateShapeOutsideInfoAfterStyleChange(*style(), oldStyle);
    updateGridPositionAfterStyleChange(oldStyle);

    if (LayoutMultiColumnSpannerPlaceholder* placeholder = this->spannerPlaceholder())
        placeholder->layoutObjectInFlowThreadStyleDidChange(oldStyle);

    updateSlowRepaintStatusAfterStyleChange();
}

void LayoutBox::updateSlowRepaintStatusAfterStyleChange()
{
    if (!frameView())
        return;

    // On low-powered/mobile devices, preventing blitting on a scroll can cause noticeable delays
    // when scrolling a page with a fixed background image. As an optimization, assuming there are
    // no fixed positoned elements on the page, we can acclerate scrolling (via blitting) if we
    // ignore the CSS property "background-attachment: fixed".
    bool ignoreFixedBackgroundAttachment = RuntimeEnabledFeatures::fastMobileScrollingEnabled();
    if (ignoreFixedBackgroundAttachment)
        return;

    // An object needs to be repainted on frame scroll when it has background-attachment:fixed.
    // LayoutObject is responsible for painting root background, thus the root element (and the
    // body element if html element has no background) skips painting backgrounds.
    bool isSlowRepaintObject = !isDocumentElement() && !backgroundStolenForBeingBody() && styleRef().hasFixedBackgroundImage();
    if (isLayoutView() && view()->compositor()->supportsFixedRootBackgroundCompositing()) {
        if (styleRef().hasEntirelyFixedBackground())
            isSlowRepaintObject = false;
    }

    setIsSlowRepaintObject(isSlowRepaintObject);
}

void LayoutBox::updateShapeOutsideInfoAfterStyleChange(const ComputedStyle& style, const ComputedStyle* oldStyle)
{
    const ShapeValue* shapeOutside = style.shapeOutside();
    const ShapeValue* oldShapeOutside = oldStyle ? oldStyle->shapeOutside() : ComputedStyle::initialShapeOutside();

    Length shapeMargin = style.shapeMargin();
    Length oldShapeMargin = oldStyle ? oldStyle->shapeMargin() : ComputedStyle::initialShapeMargin();

    float shapeImageThreshold = style.shapeImageThreshold();
    float oldShapeImageThreshold = oldStyle ? oldStyle->shapeImageThreshold() : ComputedStyle::initialShapeImageThreshold();

    // FIXME: A future optimization would do a deep comparison for equality. (bug 100811)
    if (shapeOutside == oldShapeOutside && shapeMargin == oldShapeMargin && shapeImageThreshold == oldShapeImageThreshold)
        return;

    if (!shapeOutside)
        ShapeOutsideInfo::removeInfo(*this);
    else
        ShapeOutsideInfo::ensureInfo(*this).markShapeAsDirty();

    if (shapeOutside || shapeOutside != oldShapeOutside)
        markShapeOutsideDependentsForLayout();
}

void LayoutBox::updateGridPositionAfterStyleChange(const ComputedStyle* oldStyle)
{
    if (!oldStyle || !parent() || !parent()->isLayoutGrid())
        return;

    if (oldStyle->gridColumnStart() == style()->gridColumnStart()
        && oldStyle->gridColumnEnd() == style()->gridColumnEnd()
        && oldStyle->gridRowStart() == style()->gridRowStart()
        && oldStyle->gridRowEnd() == style()->gridRowEnd()
        && oldStyle->order() == style()->order()
        && oldStyle->hasOutOfFlowPosition() == style()->hasOutOfFlowPosition())
        return;

    // It should be possible to not dirty the grid in some cases (like moving an explicitly placed grid item).
    // For now, it's more simple to just always recompute the grid.
    toLayoutGrid(parent())->dirtyGrid();
}

void LayoutBox::updateFromStyle()
{
    LayoutBoxModelObject::updateFromStyle();

    const ComputedStyle& styleToUse = styleRef();
    bool isViewObject = isLayoutView();
    bool rootLayerScrolls = document().settings() && document().settings()->rootLayerScrolls();

    // LayoutView of the main frame is resposible from painting base background.
    if (isViewObject && !document().ownerElement())
        setHasBoxDecorationBackground(true);

    setFloating(!isOutOfFlowPositioned() && styleToUse.isFloating());

    bool boxHasOverflowClip = false;
    if (!styleToUse.isOverflowVisible() && isLayoutBlock() && (rootLayerScrolls || !isViewObject)) {
        // If overflow has been propagated to the viewport, it has no effect here.
        if (node() != document().viewportDefiningElement())
            boxHasOverflowClip = true;
    }

    if (boxHasOverflowClip != hasOverflowClip()) {
        // FIXME: This shouldn't be required if we tracked the visual overflow
        // generated by positioned children or self painting layers. crbug.com/345403
        for (LayoutObject* child = slowFirstChild(); child; child = child->nextSibling())
            child->setMayNeedPaintInvalidation();
    }

    setHasOverflowClip(boxHasOverflowClip);

    setHasTransformRelatedProperty(styleToUse.hasTransformRelatedProperty());
    setHasReflection(styleToUse.boxReflect());
}

void LayoutBox::layout()
{
    ASSERT(needsLayout());
    LayoutAnalyzer::Scope analyzer(*this);

    LayoutObject* child = slowFirstChild();
    if (!child) {
        clearNeedsLayout();
        return;
    }

    LayoutState state(*this, locationOffset());
    while (child) {
        child->layoutIfNeeded();
        ASSERT(!child->needsLayout());
        child = child->nextSibling();
    }
    invalidateBackgroundObscurationStatus();
    clearNeedsLayout();
}

// More IE extensions.  clientWidth and clientHeight represent the interior of an object
// excluding border and scrollbar.
LayoutUnit LayoutBox::clientWidth() const
{
    return m_frameRect.width() - borderLeft() - borderRight() - verticalScrollbarWidth();
}

LayoutUnit LayoutBox::clientHeight() const
{
    return m_frameRect.height() - borderTop() - borderBottom() - horizontalScrollbarHeight();
}

int LayoutBox::pixelSnappedClientWidth() const
{
    return snapSizeToPixel(clientWidth(), location().x() + clientLeft());
}

int LayoutBox::pixelSnappedClientHeight() const
{
    return snapSizeToPixel(clientHeight(), location().y() + clientTop());
}

int LayoutBox::pixelSnappedOffsetWidth() const
{
    return snapSizeToPixel(offsetWidth(), location().x() + clientLeft());
}

int LayoutBox::pixelSnappedOffsetHeight() const
{
    return snapSizeToPixel(offsetHeight(), location().y() + clientTop());
}

LayoutUnit LayoutBox::scrollWidth() const
{
    if (hasOverflowClip())
        return layer()->scrollableArea()->scrollWidth();
    // For objects with visible overflow, this matches IE.
    // FIXME: Need to work right with writing modes.
    if (style()->isLeftToRightDirection())
        return std::max(clientWidth(), layoutOverflowRect().maxX() - borderLeft());
    return clientWidth() - std::min(LayoutUnit(), layoutOverflowRect().x() - borderLeft());
}

LayoutUnit LayoutBox::scrollHeight() const
{
    if (hasOverflowClip())
        return layer()->scrollableArea()->scrollHeight();
    // For objects with visible overflow, this matches IE.
    // FIXME: Need to work right with writing modes.
    return std::max(clientHeight(), layoutOverflowRect().maxY() - borderTop());
}

LayoutUnit LayoutBox::scrollLeft() const
{
    return hasOverflowClip() ? layer()->scrollableArea()->scrollXOffset() : 0;
}

LayoutUnit LayoutBox::scrollTop() const
{
    return hasOverflowClip() ? layer()->scrollableArea()->scrollYOffset() : 0;
}

int LayoutBox::pixelSnappedScrollWidth() const
{
    return snapSizeToPixel(scrollWidth(), location().x() + clientLeft());
}

int LayoutBox::pixelSnappedScrollHeight() const
{
    if (hasOverflowClip())
        return layer()->scrollableArea()->scrollHeight();
    // For objects with visible overflow, this matches IE.
    // FIXME: Need to work right with writing modes.
    return snapSizeToPixel(scrollHeight(), location().y() + clientTop());
}

void LayoutBox::setScrollLeft(LayoutUnit newLeft)
{
    // This doesn't hit in any tests, but since the equivalent code in setScrollTop
    // does, presumably this code does as well.
    DisableCompositingQueryAsserts disabler;

    if (hasOverflowClip())
        layer()->scrollableArea()->scrollToXOffset(newLeft, ScrollOffsetClamped, ScrollBehaviorAuto);
}

void LayoutBox::setScrollTop(LayoutUnit newTop)
{
    // Hits in compositing/overflow/do-not-assert-on-invisible-composited-layers.html
    DisableCompositingQueryAsserts disabler;

    if (hasOverflowClip())
        layer()->scrollableArea()->scrollToYOffset(newTop, ScrollOffsetClamped, ScrollBehaviorAuto);
}

void LayoutBox::scrollToOffset(const DoubleSize& offset, ScrollBehavior scrollBehavior)
{
    // This doesn't hit in any tests, but since the equivalent code in setScrollTop
    // does, presumably this code does as well.
    DisableCompositingQueryAsserts disabler;

    if (hasOverflowClip())
        layer()->scrollableArea()->scrollToOffset(offset, ScrollOffsetClamped, scrollBehavior);
}

// Returns true iff we are attempting an autoscroll inside an iframe with scrolling="no".
static bool isDisallowedAutoscroll(HTMLFrameOwnerElement* ownerElement, FrameView* frameView)
{
    if (ownerElement && isHTMLFrameElementBase(*ownerElement)) {
        HTMLFrameElementBase* frameElementBase = toHTMLFrameElementBase(ownerElement);
        if (Page* page = frameView->frame().page()) {
            return page->autoscrollController().autoscrollInProgress()
                && frameElementBase->scrollingMode() == ScrollbarAlwaysOff;
        }
    }
    return false;
}

void LayoutBox::scrollRectToVisible(const LayoutRect& rect, const ScrollAlignment& alignX, const ScrollAlignment& alignY)
{
    // Presumably the same issue as in setScrollTop. See crbug.com/343132.
    DisableCompositingQueryAsserts disabler;

    LayoutBox* parentBox = nullptr;
    LayoutRect newRect = rect;

    bool restrictedByLineClamp = false;
    if (parent()) {
        parentBox = parent()->enclosingBox();
        restrictedByLineClamp = !parent()->style()->lineClamp().isNone();
    }

    if (hasOverflowClip() && !restrictedByLineClamp) {
        // Don't scroll to reveal an overflow layer that is restricted by the -webkit-line-clamp property.
        // This will prevent us from revealing text hidden by the slider in Safari RSS.
        newRect = layer()->scrollableArea()->scrollIntoView(rect, alignX, alignY);
    } else if (!parentBox && canBeProgramaticallyScrolled()) {
        if (FrameView* frameView = this->frameView()) {
            HTMLFrameOwnerElement* ownerElement = document().ownerElement();
            if (!isDisallowedAutoscroll(ownerElement, frameView)) {
                frameView->scrollableArea()->scrollIntoView(rect, alignX, alignY);

                if (ownerElement && ownerElement->layoutObject()) {
                    if (frameView->safeToPropagateScrollToParent()) {
                        parentBox = ownerElement->layoutObject()->enclosingBox();
                        // FIXME: This doesn't correctly convert the rect to
                        // absolute coordinates in the parent.
                        newRect.setX(rect.x() - frameView->scrollX() + frameView->x());
                        newRect.setY(rect.y() - frameView->scrollY() + frameView->y());
                    } else {
                        parentBox = nullptr;
                    }
                }
            }
        }
    }

    // If we are fixed-position, it is useless to scroll the parent.
    if (hasLayer() && layer()->scrollsWithViewport())
        return;

    if (frame()->page()->autoscrollController().autoscrollInProgress())
        parentBox = enclosingScrollableBox();

    if (parentBox)
        parentBox->scrollRectToVisible(newRect, alignX, alignY);
}

void LayoutBox::absoluteRects(Vector<IntRect>& rects, const LayoutPoint& accumulatedOffset) const
{
    rects.append(pixelSnappedIntRect(accumulatedOffset, size()));
}

void LayoutBox::absoluteQuads(Vector<FloatQuad>& quads, bool* wasFixed) const
{
    quads.append(localToAbsoluteQuad(FloatRect(0, 0, m_frameRect.width().toFloat(), m_frameRect.height().toFloat()), 0 /* mode */, wasFixed));
}

void LayoutBox::updateLayerTransformAfterLayout()
{
    // Transform-origin depends on box size, so we need to update the layer transform after layout.
    if (hasLayer())
        layer()->updateTransformationMatrix();
}

LayoutUnit LayoutBox::constrainLogicalWidthByMinMax(LayoutUnit logicalWidth, LayoutUnit availableWidth, LayoutBlock* cb) const
{
    const ComputedStyle& styleToUse = styleRef();
    if (!styleToUse.logicalMaxWidth().isMaxSizeNone())
        logicalWidth = std::min(logicalWidth, computeLogicalWidthUsing(MaxSize, styleToUse.logicalMaxWidth(), availableWidth, cb));
    return std::max(logicalWidth, computeLogicalWidthUsing(MinSize, styleToUse.logicalMinWidth(), availableWidth, cb));
}

LayoutUnit LayoutBox::constrainLogicalHeightByMinMax(LayoutUnit logicalHeight, LayoutUnit intrinsicContentHeight) const
{
    const ComputedStyle& styleToUse = styleRef();
    if (!styleToUse.logicalMaxHeight().isMaxSizeNone()) {
        LayoutUnit maxH = computeLogicalHeightUsing(MaxSize, styleToUse.logicalMaxHeight(), intrinsicContentHeight);
        if (maxH != -1)
            logicalHeight = std::min(logicalHeight, maxH);
    }
    return std::max(logicalHeight, computeLogicalHeightUsing(MinSize, styleToUse.logicalMinHeight(), intrinsicContentHeight));
}

LayoutUnit LayoutBox::constrainContentBoxLogicalHeightByMinMax(LayoutUnit logicalHeight, LayoutUnit intrinsicContentHeight) const
{
    const ComputedStyle& styleToUse = styleRef();
    if (!styleToUse.logicalMaxHeight().isMaxSizeNone()) {
        LayoutUnit maxH = computeContentLogicalHeight(MaxSize, styleToUse.logicalMaxHeight(), intrinsicContentHeight);
        if (maxH != -1)
            logicalHeight = std::min(logicalHeight, maxH);
    }
    return std::max(logicalHeight, computeContentLogicalHeight(MinSize, styleToUse.logicalMinHeight(), intrinsicContentHeight));
}

void LayoutBox::setLocationAndUpdateOverflowControlsIfNeeded(const LayoutPoint& location)
{
    if (hasOverflowClip()) {
        IntSize oldPixelSnappedBorderRectSize = pixelSnappedBorderBoxRect().size();
        setLocation(location);
        if (pixelSnappedBorderBoxRect().size() != oldPixelSnappedBorderRectSize)
            scrollableArea()->updateAfterLayout();
        return;
    }

    setLocation(location);
}

IntRect LayoutBox::absoluteContentBox() const
{
    // This is wrong with transforms and flipped writing modes.
    IntRect rect = pixelSnappedIntRect(contentBoxRect());
    FloatPoint absPos = localToAbsolute();
    rect.move(absPos.x(), absPos.y());
    return rect;
}

IntSize LayoutBox::absoluteContentBoxOffset() const
{
    IntPoint offset = roundedIntPoint(contentBoxOffset());
    FloatPoint absPos = localToAbsolute();
    offset.move(absPos.x(), absPos.y());
    return toIntSize(offset);
}

FloatQuad LayoutBox::absoluteContentQuad() const
{
    LayoutRect rect = contentBoxRect();
    return localToAbsoluteQuad(FloatRect(rect));
}

void LayoutBox::addFocusRingRects(Vector<LayoutRect>& rects, const LayoutPoint& additionalOffset) const
{
    if (!size().isEmpty())
        rects.append(LayoutRect(additionalOffset, size()));
}

bool LayoutBox::canResize() const
{
    // We need a special case for <iframe> because they never have
    // hasOverflowClip(). However, they do "implicitly" clip their contents, so
    // we want to allow resizing them also.
    return (hasOverflowClip() || isLayoutIFrame()) && style()->resize() != RESIZE_NONE;
}

void LayoutBox::addLayerHitTestRects(LayerHitTestRects& layerRects, const DeprecatedPaintLayer* currentLayer, const LayoutPoint& layerOffset, const LayoutRect& containerRect) const
{
    LayoutPoint adjustedLayerOffset = layerOffset + locationOffset();
    LayoutBoxModelObject::addLayerHitTestRects(layerRects, currentLayer, adjustedLayerOffset, containerRect);
}

void LayoutBox::computeSelfHitTestRects(Vector<LayoutRect>& rects, const LayoutPoint& layerOffset) const
{
    if (!size().isEmpty())
        rects.append(LayoutRect(layerOffset, size()));
}

int LayoutBox::reflectionOffset() const
{
    if (!style()->boxReflect())
        return 0;
    if (style()->boxReflect()->direction() == ReflectionLeft || style()->boxReflect()->direction() == ReflectionRight)
        return valueForLength(style()->boxReflect()->offset(), borderBoxRect().width());
    return valueForLength(style()->boxReflect()->offset(), borderBoxRect().height());
}

LayoutRect LayoutBox::reflectedRect(const LayoutRect& r) const
{
    if (!style()->boxReflect())
        return LayoutRect();

    LayoutRect box = borderBoxRect();
    LayoutRect result = r;
    switch (style()->boxReflect()->direction()) {
    case ReflectionBelow:
        result.setY(box.maxY() + reflectionOffset() + (box.maxY() - r.maxY()));
        break;
    case ReflectionAbove:
        result.setY(box.y() - reflectionOffset() - box.height() + (box.maxY() - r.maxY()));
        break;
    case ReflectionLeft:
        result.setX(box.x() - reflectionOffset() - box.width() + (box.maxX() - r.maxX()));
        break;
    case ReflectionRight:
        result.setX(box.maxX() + reflectionOffset() + (box.maxX() - r.maxX()));
        break;
    }
    return result;
}

int LayoutBox::verticalScrollbarWidth() const
{
    if (!hasOverflowClip() || style()->overflowY() == OOVERLAY)
        return 0;

    return layer()->scrollableArea()->verticalScrollbarWidth();
}

int LayoutBox::horizontalScrollbarHeight() const
{
    if (!hasOverflowClip() || style()->overflowX() == OOVERLAY)
        return 0;

    return layer()->scrollableArea()->horizontalScrollbarHeight();
}

int LayoutBox::intrinsicScrollbarLogicalWidth() const
{
    if (!hasOverflowClip())
        return 0;

    if (isHorizontalWritingMode() && style()->overflowY() == OSCROLL) {
        ASSERT(layer()->scrollableArea() && layer()->scrollableArea()->hasVerticalScrollbar());
        return verticalScrollbarWidth();
    }

    if (!isHorizontalWritingMode() && style()->overflowX() == OSCROLL) {
        ASSERT(layer()->scrollableArea() && layer()->scrollableArea()->hasHorizontalScrollbar());
        return horizontalScrollbarHeight();
    }

    return 0;
}

ScrollResultOneDimensional LayoutBox::scroll(ScrollDirectionPhysical direction, ScrollGranularity granularity, float delta)
{
    // Presumably the same issue as in setScrollTop. See crbug.com/343132.
    DisableCompositingQueryAsserts disabler;

    if (!layer() || !layer()->scrollableArea())
        return ScrollResultOneDimensional(false);

    return layer()->scrollableArea()->userScroll(direction, granularity, delta);
}

bool LayoutBox::canBeScrolledAndHasScrollableArea() const
{
    return canBeProgramaticallyScrolled() && (pixelSnappedScrollHeight() != pixelSnappedClientHeight() || pixelSnappedScrollWidth() != pixelSnappedClientWidth());
}

bool LayoutBox::canBeProgramaticallyScrolled() const
{
    Node* node = this->node();
    if (node && node->isDocumentNode())
        return true;

    if (!hasOverflowClip())
        return false;

    bool hasScrollableOverflow = hasScrollableOverflowX() || hasScrollableOverflowY();
    if (scrollsOverflow() && hasScrollableOverflow)
        return true;

    return node && node->hasEditableStyle();
}

bool LayoutBox::usesCompositedScrolling() const
{
    return hasOverflowClip() && hasLayer() && layer()->scrollableArea()->usesCompositedScrolling();
}

void LayoutBox::autoscroll(const IntPoint& positionInRootFrame)
{
    LocalFrame* frame = this->frame();
    if (!frame)
        return;

    FrameView* frameView = frame->view();
    if (!frameView)
        return;

    IntPoint positionInContent = frameView->rootFrameToContents(positionInRootFrame);
    scrollRectToVisible(LayoutRect(positionInContent, LayoutSize(1, 1)), ScrollAlignment::alignToEdgeIfNeeded, ScrollAlignment::alignToEdgeIfNeeded);
}

// There are two kinds of layoutObject that can autoscroll.
bool LayoutBox::canAutoscroll() const
{
    if (node() && node()->isDocumentNode())
        return view()->frameView()->isScrollable();

    // Check for a box that can be scrolled in its own right.
    return canBeScrolledAndHasScrollableArea();
}

// If specified point is in border belt, returned offset denotes direction of
// scrolling.
IntSize LayoutBox::calculateAutoscrollDirection(const IntPoint& pointInRootFrame) const
{
    if (!frame())
        return IntSize();

    FrameView* frameView = frame()->view();
    if (!frameView)
        return IntSize();

    IntRect box(absoluteBoundingBoxRect());
    box.move(view()->frameView()->scrollOffset());
    IntRect windowBox = view()->frameView()->contentsToRootFrame(box);

    IntPoint windowAutoscrollPoint = pointInRootFrame;

    if (windowAutoscrollPoint.x() < windowBox.x() + autoscrollBeltSize)
        windowAutoscrollPoint.move(-autoscrollBeltSize, 0);
    else if (windowAutoscrollPoint.x() > windowBox.maxX() - autoscrollBeltSize)
        windowAutoscrollPoint.move(autoscrollBeltSize, 0);

    if (windowAutoscrollPoint.y() < windowBox.y() + autoscrollBeltSize)
        windowAutoscrollPoint.move(0, -autoscrollBeltSize);
    else if (windowAutoscrollPoint.y() > windowBox.maxY() - autoscrollBeltSize)
        windowAutoscrollPoint.move(0, autoscrollBeltSize);

    return windowAutoscrollPoint - pointInRootFrame;
}

LayoutBox* LayoutBox::findAutoscrollable(LayoutObject* layoutObject)
{
    while (layoutObject && !(layoutObject->isBox() && toLayoutBox(layoutObject)->canAutoscroll())) {
        if (!layoutObject->parent() && layoutObject->node() == layoutObject->document() && layoutObject->document().ownerElement())
            layoutObject = layoutObject->document().ownerElement()->layoutObject();
        else
            layoutObject = layoutObject->parent();
    }

    return layoutObject && layoutObject->isBox() ? toLayoutBox(layoutObject) : 0;
}

static inline int adjustedScrollDelta(int beginningDelta)
{
    // This implemention matches Firefox's.
    // http://mxr.mozilla.org/firefox/source/toolkit/content/widgets/browser.xml#856.
    const int speedReducer = 12;

    int adjustedDelta = beginningDelta / speedReducer;
    if (adjustedDelta > 1)
        adjustedDelta = static_cast<int>(adjustedDelta * sqrt(static_cast<double>(adjustedDelta))) - 1;
    else if (adjustedDelta < -1)
        adjustedDelta = static_cast<int>(adjustedDelta * sqrt(static_cast<double>(-adjustedDelta))) + 1;

    return adjustedDelta;
}

static inline IntSize adjustedScrollDelta(const IntSize& delta)
{
    return IntSize(adjustedScrollDelta(delta.width()), adjustedScrollDelta(delta.height()));
}

void LayoutBox::panScroll(const IntPoint& sourcePoint)
{
    LocalFrame* frame = this->frame();
    if (!frame)
        return;

    IntPoint lastKnownMousePosition = frame->eventHandler().lastKnownMousePosition();

    // We need to check if the last known mouse position is out of the window. When the mouse is out of the window, the position is incoherent
    static IntPoint previousMousePosition;
    if (lastKnownMousePosition.x() < 0 || lastKnownMousePosition.y() < 0)
        lastKnownMousePosition = previousMousePosition;
    else
        previousMousePosition = lastKnownMousePosition;

    IntSize delta = lastKnownMousePosition - sourcePoint;

    if (abs(delta.width()) <= AutoscrollController::noPanScrollRadius) // at the center we let the space for the icon
        delta.setWidth(0);
    if (abs(delta.height()) <= AutoscrollController::noPanScrollRadius)
        delta.setHeight(0);
    scrollByRecursively(adjustedScrollDelta(delta), ScrollOffsetClamped);
}

void LayoutBox::scrollByRecursively(const DoubleSize& delta, ScrollOffsetClamping clamp)
{
    if (delta.isZero())
        return;

    bool restrictedByLineClamp = false;
    if (parent())
        restrictedByLineClamp = !parent()->style()->lineClamp().isNone();

    if (hasOverflowClip() && !restrictedByLineClamp) {
        DoubleSize newScrollOffset = layer()->scrollableArea()->adjustedScrollOffset() + delta;
        layer()->scrollableArea()->scrollToOffset(newScrollOffset, clamp);

        // If this layer can't do the scroll we ask the next layer up that can scroll to try
        DoubleSize remainingScrollOffset = newScrollOffset - layer()->scrollableArea()->adjustedScrollOffset();
        if (!remainingScrollOffset.isZero() && parent()) {
            if (LayoutBox* scrollableBox = enclosingScrollableBox())
                scrollableBox->scrollByRecursively(remainingScrollOffset, clamp);

            LocalFrame* frame = this->frame();
            if (frame && frame->page())
                frame->page()->autoscrollController().updateAutoscrollLayoutObject();
        }
    } else if (view()->frameView()) {
        // If we are here, we were called on a layoutObject that can be programmatically scrolled, but doesn't
        // have an overflow clip. Which means that it is a document node that can be scrolled.
        // FIXME: Pass in DoubleSize. crbug.com/414283.
        view()->frameView()->scrollBy(flooredIntSize(delta), UserScroll);

        // FIXME: If we didn't scroll the whole way, do we want to try looking at the frames ownerElement?
        // https://bugs.webkit.org/show_bug.cgi?id=28237
    }
}

bool LayoutBox::needsPreferredWidthsRecalculation() const
{
    return style()->paddingStart().hasPercent() || style()->paddingEnd().hasPercent();
}

IntSize LayoutBox::scrolledContentOffset() const
{
    ASSERT(hasOverflowClip());
    ASSERT(hasLayer());
    // FIXME: Return DoubleSize here. crbug.com/414283.
    return flooredIntSize(layer()->scrollableArea()->scrollOffset());
}

void LayoutBox::applyCachedClipAndScrollOffsetForPaintInvalidation(LayoutRect& paintRect) const
{
    ASSERT(hasLayer());
    ASSERT(hasOverflowClip());

    flipForWritingMode(paintRect);
    paintRect.move(-scrolledContentOffset()); // For overflow:auto/scroll/hidden.

    // Do not clip scroll layer contents because the compositor expects the whole layer
    // to be always invalidated in-time.
    if (usesCompositedScrolling()) {
        flipForWritingMode(paintRect);
        return;
    }

    // size() is inaccurate if we're in the middle of a layout of this LayoutBox, so use the
    // layer's size instead. Even if the layer's size is wrong, the layer itself will issue paint invalidations
    // anyway if its size does change.
    LayoutRect clipRect(LayoutPoint(), LayoutSize(layer()->size()));
    paintRect = intersection(paintRect, clipRect);
    flipForWritingMode(paintRect);
}

void LayoutBox::computeIntrinsicLogicalWidths(LayoutUnit& minLogicalWidth, LayoutUnit& maxLogicalWidth) const
{
    minLogicalWidth = minPreferredLogicalWidth() - borderAndPaddingLogicalWidth();
    maxLogicalWidth = maxPreferredLogicalWidth() - borderAndPaddingLogicalWidth();
}

LayoutUnit LayoutBox::minPreferredLogicalWidth() const
{
    if (preferredLogicalWidthsDirty()) {
#if ENABLE(ASSERT)
        SetLayoutNeededForbiddenScope layoutForbiddenScope(const_cast<LayoutBox&>(*this));
#endif
        const_cast<LayoutBox*>(this)->computePreferredLogicalWidths();
    }

    return m_minPreferredLogicalWidth;
}

LayoutUnit LayoutBox::maxPreferredLogicalWidth() const
{
    if (preferredLogicalWidthsDirty()) {
#if ENABLE(ASSERT)
        SetLayoutNeededForbiddenScope layoutForbiddenScope(const_cast<LayoutBox&>(*this));
#endif
        const_cast<LayoutBox*>(this)->computePreferredLogicalWidths();
    }

    return m_maxPreferredLogicalWidth;
}

bool LayoutBox::hasOverrideLogicalContentHeight() const
{
    return m_rareData && m_rareData->m_overrideLogicalContentHeight != -1;
}

bool LayoutBox::hasOverrideLogicalContentWidth() const
{
    return m_rareData && m_rareData->m_overrideLogicalContentWidth != -1;
}

void LayoutBox::setOverrideLogicalContentHeight(LayoutUnit height)
{
    ASSERT(height >= 0);
    ensureRareData().m_overrideLogicalContentHeight = height;
}

void LayoutBox::setOverrideLogicalContentWidth(LayoutUnit width)
{
    ASSERT(width >= 0);
    ensureRareData().m_overrideLogicalContentWidth = width;
}

void LayoutBox::clearOverrideLogicalContentHeight()
{
    if (m_rareData)
        m_rareData->m_overrideLogicalContentHeight = -1;
}

void LayoutBox::clearOverrideLogicalContentWidth()
{
    if (m_rareData)
        m_rareData->m_overrideLogicalContentWidth = -1;
}

void LayoutBox::clearOverrideSize()
{
    clearOverrideLogicalContentHeight();
    clearOverrideLogicalContentWidth();
}

LayoutUnit LayoutBox::overrideLogicalContentWidth() const
{
    ASSERT(hasOverrideLogicalContentWidth());
    return m_rareData->m_overrideLogicalContentWidth;
}

LayoutUnit LayoutBox::overrideLogicalContentHeight() const
{
    ASSERT(hasOverrideLogicalContentHeight());
    return m_rareData->m_overrideLogicalContentHeight;
}

LayoutUnit LayoutBox::overrideContainingBlockContentLogicalWidth() const
{
    ASSERT(hasOverrideContainingBlockLogicalWidth());
    return gOverrideContainingBlockLogicalWidthMap->get(this);
}

LayoutUnit LayoutBox::overrideContainingBlockContentLogicalHeight() const
{
    ASSERT(hasOverrideContainingBlockLogicalHeight());
    return gOverrideContainingBlockLogicalHeightMap->get(this);
}

bool LayoutBox::hasOverrideContainingBlockLogicalWidth() const
{
    return gOverrideContainingBlockLogicalWidthMap && gOverrideContainingBlockLogicalWidthMap->contains(this);
}

bool LayoutBox::hasOverrideContainingBlockLogicalHeight() const
{
    return gOverrideContainingBlockLogicalHeightMap && gOverrideContainingBlockLogicalHeightMap->contains(this);
}

void LayoutBox::setOverrideContainingBlockContentLogicalWidth(LayoutUnit logicalWidth)
{
    if (!gOverrideContainingBlockLogicalWidthMap)
        gOverrideContainingBlockLogicalWidthMap = new OverrideSizeMap;
    gOverrideContainingBlockLogicalWidthMap->set(this, logicalWidth);
}

void LayoutBox::setOverrideContainingBlockContentLogicalHeight(LayoutUnit logicalHeight)
{
    if (!gOverrideContainingBlockLogicalHeightMap)
        gOverrideContainingBlockLogicalHeightMap = new OverrideSizeMap;
    gOverrideContainingBlockLogicalHeightMap->set(this, logicalHeight);
}

void LayoutBox::clearContainingBlockOverrideSize()
{
    if (gOverrideContainingBlockLogicalWidthMap)
        gOverrideContainingBlockLogicalWidthMap->remove(this);
    clearOverrideContainingBlockContentLogicalHeight();
}

void LayoutBox::clearOverrideContainingBlockContentLogicalHeight()
{
    if (gOverrideContainingBlockLogicalHeightMap)
        gOverrideContainingBlockLogicalHeightMap->remove(this);
}

LayoutUnit LayoutBox::extraInlineOffset() const
{
    return gExtraInlineOffsetMap ? gExtraInlineOffsetMap->get(this) : LayoutUnit();
}

LayoutUnit LayoutBox::extraBlockOffset() const
{
    return gExtraBlockOffsetMap ? gExtraBlockOffsetMap->get(this) : LayoutUnit();
}

void LayoutBox::setExtraInlineOffset(LayoutUnit inlineOffest)
{
    if (!gExtraInlineOffsetMap)
        gExtraInlineOffsetMap = new OverrideSizeMap;
    gExtraInlineOffsetMap->set(this, inlineOffest);
}

void LayoutBox::setExtraBlockOffset(LayoutUnit blockOffest)
{
    if (!gExtraBlockOffsetMap)
        gExtraBlockOffsetMap = new OverrideSizeMap;
    gExtraBlockOffsetMap->set(this, blockOffest);
}

void LayoutBox::clearExtraInlineAndBlockOffests()
{
    if (gExtraInlineOffsetMap)
        gExtraInlineOffsetMap->remove(this);
    if (gExtraBlockOffsetMap)
        gExtraBlockOffsetMap->remove(this);
}

LayoutUnit LayoutBox::adjustBorderBoxLogicalWidthForBoxSizing(LayoutUnit width) const
{
    LayoutUnit bordersPlusPadding = borderAndPaddingLogicalWidth();
    if (style()->boxSizing() == CONTENT_BOX)
        return width + bordersPlusPadding;
    return std::max(width, bordersPlusPadding);
}

LayoutUnit LayoutBox::adjustBorderBoxLogicalHeightForBoxSizing(LayoutUnit height) const
{
    LayoutUnit bordersPlusPadding = borderAndPaddingLogicalHeight();
    if (style()->boxSizing() == CONTENT_BOX)
        return height + bordersPlusPadding;
    return std::max(height, bordersPlusPadding);
}

LayoutUnit LayoutBox::adjustContentBoxLogicalWidthForBoxSizing(LayoutUnit width) const
{
    if (style()->boxSizing() == BORDER_BOX)
        width -= borderAndPaddingLogicalWidth();
    return std::max(LayoutUnit(), width);
}

LayoutUnit LayoutBox::adjustContentBoxLogicalHeightForBoxSizing(LayoutUnit height) const
{
    if (style()->boxSizing() == BORDER_BOX)
        height -= borderAndPaddingLogicalHeight();
    return std::max(LayoutUnit(), height);
}

// Hit Testing
bool LayoutBox::nodeAtPoint(HitTestResult& result, const HitTestLocation& locationInContainer, const LayoutPoint& accumulatedOffset, HitTestAction action)
{
    LayoutPoint adjustedLocation = accumulatedOffset + location();

    // Exit early if no children can be hit.
    LayoutRect overflowRect = visualOverflowRect();
    overflowRect.moveBy(adjustedLocation);
    if (!locationInContainer.intersects(overflowRect)) {
        return false;
    }

    // Check kids first.
    for (LayoutObject* child = slowLastChild(); child; child = child->previousSibling()) {
        if ((!child->hasLayer() || !toLayoutBoxModelObject(child)->layer()->isSelfPaintingLayer()) && child->nodeAtPoint(result, locationInContainer, adjustedLocation, action)) {
            updateHitTestResult(result, locationInContainer.point() - toLayoutSize(adjustedLocation));
            return true;
        }
    }

    // Check our bounds next. For this purpose always assume that we can only be hit in the
    // foreground phase (which is true for replaced elements like images).
    LayoutRect boundsRect = borderBoxRect();
    boundsRect.moveBy(adjustedLocation);
    if (visibleToHitTestRequest(result.hitTestRequest()) && action == HitTestForeground && locationInContainer.intersects(boundsRect)) {
        updateHitTestResult(result, locationInContainer.point() - toLayoutSize(adjustedLocation));
        if (!result.addNodeToListBasedTestResult(node(), locationInContainer, boundsRect))
            return true;
    }

    return false;
}

void LayoutBox::paint(const PaintInfo& paintInfo, const LayoutPoint& paintOffset)
{
    BoxPainter(*this).paint(paintInfo, paintOffset);
}

void LayoutBox::paintBoxDecorationBackground(const PaintInfo& paintInfo, const LayoutPoint& paintOffset)
{
    BoxPainter(*this).paintBoxDecorationBackground(paintInfo, paintOffset);
}

bool LayoutBox::getBackgroundPaintedExtent(LayoutRect& paintedExtent)
{
    ASSERT(hasBackground());

    // LayoutView is special in the sense that it expands to the whole canvas,
    // thus can't be handled by this function.
    ASSERT(!isLayoutView());

    LayoutRect backgroundRect(enclosingIntRect(borderBoxRect()));

    Color backgroundColor = resolveColor(CSSPropertyBackgroundColor);
    if (backgroundColor.alpha()) {
        paintedExtent = backgroundRect;
        return true;
    }

    if (!style()->backgroundLayers().image() || style()->backgroundLayers().next()) {
        paintedExtent =  backgroundRect;
        return true;
    }

    BackgroundImageGeometry geometry;
    BoxPainter::calculateBackgroundImageGeometry(*this, 0, style()->backgroundLayers(), backgroundRect, geometry);
    if (geometry.hasNonLocalGeometry())
        return false;
    paintedExtent = LayoutRect(geometry.destRect());
    return true;
}

bool LayoutBox::backgroundIsKnownToBeOpaqueInRect(const LayoutRect& localRect) const
{
    if (isDocumentElement() || backgroundStolenForBeingBody())
        return false;

    Color backgroundColor = resolveColor(CSSPropertyBackgroundColor);
    if (backgroundColor.hasAlpha())
        return false;

    // If the element has appearance, it might be painted by theme.
    // We cannot be sure if theme paints the background opaque.
    // In this case it is safe to not assume opaqueness.
    // FIXME: May be ask theme if it paints opaque.
    if (style()->hasAppearance())
        return false;
    // FIXME: Check the opaqueness of background images.

    // FIXME: Use rounded rect if border radius is present.
    if (style()->hasBorderRadius())
        return false;
    // FIXME: The background color clip is defined by the last layer.
    if (style()->backgroundLayers().next())
        return false;
    LayoutRect backgroundRect;
    switch (style()->backgroundClip()) {
    case BorderFillBox:
        backgroundRect = borderBoxRect();
        break;
    case PaddingFillBox:
        backgroundRect = paddingBoxRect();
        break;
    case ContentFillBox:
        backgroundRect = contentBoxRect();
        break;
    default:
        break;
    }
    return backgroundRect.contains(localRect);
}

static bool isCandidateForOpaquenessTest(const LayoutBox& childBox)
{
    const ComputedStyle& childStyle = childBox.styleRef();
    if (childStyle.position() != StaticPosition && childBox.containingBlock() != childBox.parent())
        return false;
    if (childStyle.visibility() != VISIBLE || childStyle.shapeOutside())
        return false;
    if (childBox.size().isZero())
        return false;
    if (DeprecatedPaintLayer* childLayer = childBox.layer()) {
        // FIXME: perhaps this could be less conservative?
        if (childLayer->compositingState() != NotComposited)
            return false;
        // FIXME: Deal with z-index.
        if (!childStyle.hasAutoZIndex())
            return false;
        if (childLayer->hasTransformRelatedProperty() || childLayer->isTransparent() || childLayer->hasFilter())
            return false;
        if (childBox.hasOverflowClip() && childStyle.hasBorderRadius())
            return false;
    }
    return true;
}

bool LayoutBox::foregroundIsKnownToBeOpaqueInRect(const LayoutRect& localRect, unsigned maxDepthToTest) const
{
    if (!maxDepthToTest)
        return false;
    for (LayoutObject* child = slowFirstChild(); child; child = child->nextSibling()) {
        if (!child->isBox())
            continue;
        LayoutBox* childBox = toLayoutBox(child);
        if (!isCandidateForOpaquenessTest(*childBox))
            continue;
        LayoutPoint childLocation = childBox->location();
        if (childBox->isRelPositioned())
            childLocation.move(childBox->relativePositionOffset());
        LayoutRect childLocalRect = localRect;
        childLocalRect.moveBy(-childLocation);
        if (childLocalRect.y() < 0 || childLocalRect.x() < 0) {
            // If there is unobscured area above/left of a static positioned box then the rect is probably not covered.
            if (childBox->style()->position() == StaticPosition)
                return false;
            continue;
        }
        if (childLocalRect.maxY() > childBox->size().height() || childLocalRect.maxX() > childBox->size().width())
            continue;
        if (childBox->backgroundIsKnownToBeOpaqueInRect(childLocalRect))
            return true;
        if (childBox->foregroundIsKnownToBeOpaqueInRect(childLocalRect, maxDepthToTest - 1))
            return true;
    }
    return false;
}

bool LayoutBox::computeBackgroundIsKnownToBeObscured()
{
    // Test to see if the children trivially obscure the background.
    // FIXME: This test can be much more comprehensive.
    if (!hasBackground())
        return false;
    // Table and root background painting is special.
    if (isTable() || isLayoutView())
        return false;
    // FIXME: box-shadow is painted while background painting.
    if (style()->boxShadow())
        return false;
    LayoutRect backgroundRect;
    if (!getBackgroundPaintedExtent(backgroundRect))
        return false;
    return foregroundIsKnownToBeOpaqueInRect(backgroundRect, backgroundObscurationTestMaxDepth);
}

void LayoutBox::paintMask(const PaintInfo& paintInfo, const LayoutPoint& paintOffset)
{
    BoxPainter(*this).paintMask(paintInfo, paintOffset);
}

void LayoutBox::imageChanged(WrappedImagePtr image, const IntRect*)
{
    // TODO(chrishtr): support PaintInvalidationDelayedFull for animated border images.
    if ((style()->borderImage().image() && style()->borderImage().image()->data() == image)
        || (style()->maskBoxImage().image() && style()->maskBoxImage().image()->data() == image)) {
        setShouldDoFullPaintInvalidation();
        return;
    }

    ShapeValue* shapeOutsideValue = style()->shapeOutside();
    if (!frameView()->isInPerformLayout() && isFloating() && shapeOutsideValue && shapeOutsideValue->image() && shapeOutsideValue->image()->data() == image) {
        ShapeOutsideInfo& info = ShapeOutsideInfo::ensureInfo(*this);
        if (!info.isComputingShape()) {
            info.markShapeAsDirty();
            markShapeOutsideDependentsForLayout();
        }
    }

    if (!invalidatePaintOfLayerRectsForImage(image, style()->backgroundLayers(), true))
        invalidatePaintOfLayerRectsForImage(image, style()->maskLayers(), false);
}

bool LayoutBox::invalidatePaintOfLayerRectsForImage(WrappedImagePtr image, const FillLayer& layers, bool drawingBackground)
{
    if (drawingBackground && (isDocumentElement() || backgroundStolenForBeingBody()))
        return false;
    for (const FillLayer* curLayer = &layers; curLayer; curLayer = curLayer->next()) {
        if (curLayer->image() && image == curLayer->image()->data()) {
            bool maybeAnimated = curLayer->image()->cachedImage() && curLayer->image()->cachedImage()->image() && curLayer->image()->cachedImage()->image()->maybeAnimated();
            if (maybeAnimated && drawingBackground)
                setShouldDoFullPaintInvalidation(PaintInvalidationDelayedFull);
            else
                setShouldDoFullPaintInvalidation();
            return true;
        }
    }
    return false;
}

bool LayoutBox::intersectsVisibleViewport()
{
    LayoutRect rect = visualOverflowRect();
    LayoutView* layoutView = view();
    while (layoutView->frame()->ownerLayoutObject())
        layoutView = layoutView->frame()->ownerLayoutObject()->view();
    mapRectToPaintInvalidationBacking(layoutView, rect, 0);
    return rect.intersects(LayoutRect(layoutView->frameView()->scrollableArea()->visibleContentRectDouble()));
}

PaintInvalidationReason LayoutBox::invalidatePaintIfNeeded(PaintInvalidationState& paintInvalidationState, const LayoutBoxModelObject& newPaintInvalidationContainer)
{
    PaintInvalidationReason fullInvalidationReason = fullPaintInvalidationReason();
    // If the current paint invalidation reason is PaintInvalidationDelayedFull, then this paint invalidation can delayed if the
    // LayoutBox in question is not on-screen. The logic to decide whether this is appropriate exists at the site of the original
    // paint invalidation that chose PaintInvalidationDelayedFull.
    if (fullInvalidationReason == PaintInvalidationDelayedFull) {
        if (!intersectsVisibleViewport())
            return PaintInvalidationDelayedFull;

        // Reset state back to regular full paint invalidation if the object is onscreen.
        setShouldDoFullPaintInvalidation(PaintInvalidationFull);
    }

    PaintInvalidationReason reason = LayoutBoxModelObject::invalidatePaintIfNeeded(paintInvalidationState, newPaintInvalidationContainer);

    // If we are set to do a full paint invalidation that means the LayoutView will be
    // issue paint invalidations. We can then skip issuing of paint invalidations for the child
    // layoutObjects as they'll be covered by the LayoutView.
    if (!view()->doingFullPaintInvalidation() && !isFullPaintInvalidationReason(reason)) {
        invalidatePaintForOverflowIfNeeded();

        // Issue paint invalidations for any scrollbars if there is a scrollable area for this layoutObject.
        if (ScrollableArea* area = scrollableArea()) {
            // In slimming paint mode, we already invalidated the display item clients of the scrollbars
            // during DeprecatedPaintLayerScrollableArea::invalidateScrollbarRect(). However, for now we still need to
            // invalidate the rectangles to trigger repaints.
            if (area->hasVerticalBarDamage())
                invalidatePaintRectangleNotInvalidatingDisplayItemClients(LayoutRect(area->verticalBarDamage()));
            if (area->hasHorizontalBarDamage())
                invalidatePaintRectangleNotInvalidatingDisplayItemClients(LayoutRect(area->horizontalBarDamage()));
        }
    }

    // This is for the next invalidatePaintIfNeeded so must be at the end.
    savePreviousBoxSizesIfNeeded();
    return reason;
}

void LayoutBox::clearPaintInvalidationState(const PaintInvalidationState& paintInvalidationState)
{
    LayoutBoxModelObject::clearPaintInvalidationState(paintInvalidationState);

    if (ScrollableArea* area = scrollableArea())
        area->resetScrollbarDamage();
}

#if ENABLE(ASSERT)
bool LayoutBox::paintInvalidationStateIsDirty() const
{
    if (ScrollableArea* area = scrollableArea()) {
        if (area->hasVerticalBarDamage() || area->hasHorizontalBarDamage())
            return true;
    }
    return LayoutBoxModelObject::paintInvalidationStateIsDirty();
}
#endif

LayoutRect LayoutBox::overflowClipRect(const LayoutPoint& location, OverlayScrollbarSizeRelevancy relevancy) const
{
    // FIXME: When overflow-clip (CSS3) is implemented, we'll obtain the property
    // here.
    LayoutRect clipRect = borderBoxRect();
    clipRect.setLocation(location + clipRect.location() + LayoutSize(borderLeft(), borderTop()));
    clipRect.setSize(clipRect.size() - LayoutSize(borderLeft() + borderRight(), borderTop() + borderBottom()));

    if (!hasOverflowClip())
        return clipRect;

    // Subtract out scrollbars if we have them.
    if (style()->shouldPlaceBlockDirectionScrollbarOnLogicalLeft())
        clipRect.move(layer()->scrollableArea()->verticalScrollbarWidth(relevancy), 0);
    clipRect.contract(layer()->scrollableArea()->verticalScrollbarWidth(relevancy), layer()->scrollableArea()->horizontalScrollbarHeight(relevancy));

    return clipRect;
}

LayoutRect LayoutBox::clipRect(const LayoutPoint& location)
{
    LayoutRect borderBoxRect = this->borderBoxRect();
    LayoutRect clipRect = LayoutRect(borderBoxRect.location() + location, borderBoxRect.size());

    if (!style()->clipLeft().isAuto()) {
        LayoutUnit c = valueForLength(style()->clipLeft(), borderBoxRect.width());
        clipRect.move(c, 0);
        clipRect.contract(c, 0);
    }

    if (!style()->clipRight().isAuto())
        clipRect.contract(size().width() - valueForLength(style()->clipRight(), size().width()), 0);

    if (!style()->clipTop().isAuto()) {
        LayoutUnit c = valueForLength(style()->clipTop(), borderBoxRect.height());
        clipRect.move(0, c);
        clipRect.contract(0, c);
    }

    if (!style()->clipBottom().isAuto())
        clipRect.contract(0, size().height() - valueForLength(style()->clipBottom(), size().height()));

    return clipRect;
}

static LayoutUnit portionOfMarginNotConsumedByFloat(LayoutUnit childMargin, LayoutUnit contentSide, LayoutUnit offset)
{
    if (childMargin <= 0)
        return LayoutUnit();
    LayoutUnit contentSideWithMargin = contentSide + childMargin;
    if (offset > contentSideWithMargin)
        return childMargin;
    return offset - contentSide;
}

LayoutUnit LayoutBox::shrinkLogicalWidthToAvoidFloats(LayoutUnit childMarginStart, LayoutUnit childMarginEnd, const LayoutBlockFlow* cb) const
{
    LayoutUnit logicalTopPosition = logicalTop();
    LayoutUnit startOffsetForContent = cb->startOffsetForContent();
    LayoutUnit endOffsetForContent = cb->endOffsetForContent();
    LayoutUnit startOffsetForLine = cb->startOffsetForLine(logicalTopPosition, false);
    LayoutUnit endOffsetForLine = cb->endOffsetForLine(logicalTopPosition, false);

    // If there aren't any floats constraining us then allow the margins to shrink/expand the width as much as they want.
    if (startOffsetForContent == startOffsetForLine && endOffsetForContent == endOffsetForLine)
        return cb->availableLogicalWidthForLine(logicalTopPosition, false) - childMarginStart - childMarginEnd;

    LayoutUnit width = cb->availableLogicalWidthForLine(logicalTopPosition, false) - std::max(LayoutUnit(), childMarginStart) - std::max(LayoutUnit(), childMarginEnd);
    // We need to see if margins on either the start side or the end side can contain the floats in question. If they can,
    // then just using the line width is inaccurate. In the case where a float completely fits, we don't need to use the line
    // offset at all, but can instead push all the way to the content edge of the containing block. In the case where the float
    // doesn't fit, we can use the line offset, but we need to grow it by the margin to reflect the fact that the margin was
    // "consumed" by the float. Negative margins aren't consumed by the float, and so we ignore them.
    width += portionOfMarginNotConsumedByFloat(childMarginStart, startOffsetForContent, startOffsetForLine);
    width += portionOfMarginNotConsumedByFloat(childMarginEnd, endOffsetForContent, endOffsetForLine);
    return width;
}

LayoutUnit LayoutBox::containingBlockLogicalHeightForGetComputedStyle() const
{
    if (hasOverrideContainingBlockLogicalHeight())
        return overrideContainingBlockContentLogicalHeight();

    if (!isPositioned())
        return containingBlockLogicalHeightForContent(ExcludeMarginBorderPadding);

    LayoutBoxModelObject* cb = toLayoutBoxModelObject(container());
    LayoutUnit height = containingBlockLogicalHeightForPositioned(cb);
    if (styleRef().position() != AbsolutePosition)
        height -= cb->paddingLogicalHeight();
    return height;
}

LayoutUnit LayoutBox::containingBlockLogicalWidthForContent() const
{
    if (hasOverrideContainingBlockLogicalWidth())
        return overrideContainingBlockContentLogicalWidth();

    LayoutBlock* cb = containingBlock();
    return cb->availableLogicalWidth();
}

LayoutUnit LayoutBox::containingBlockLogicalHeightForContent(AvailableLogicalHeightType heightType) const
{
    if (hasOverrideContainingBlockLogicalHeight())
        return overrideContainingBlockContentLogicalHeight();

    LayoutBlock* cb = containingBlock();
    return cb->availableLogicalHeight(heightType);
}

LayoutUnit LayoutBox::containingBlockAvailableLineWidth() const
{
    LayoutBlock* cb = containingBlock();
    if (cb->isLayoutBlockFlow())
        return toLayoutBlockFlow(cb)->availableLogicalWidthForLine(logicalTop(), false, availableLogicalHeight(IncludeMarginBorderPadding));
    return LayoutUnit();
}

LayoutUnit LayoutBox::perpendicularContainingBlockLogicalHeight() const
{
    if (hasOverrideContainingBlockLogicalHeight())
        return overrideContainingBlockContentLogicalHeight();

    LayoutBlock* cb = containingBlock();
    if (cb->hasOverrideLogicalContentHeight())
        return cb->overrideLogicalContentHeight();

    const ComputedStyle& containingBlockStyle = cb->styleRef();
    Length logicalHeightLength = containingBlockStyle.logicalHeight();

    // FIXME: For now just support fixed heights.  Eventually should support percentage heights as well.
    if (!logicalHeightLength.isFixed()) {
        LayoutUnit fillFallbackExtent = containingBlockStyle.isHorizontalWritingMode()
            ? view()->frameView()->visibleContentSize().height()
            : view()->frameView()->visibleContentSize().width();
        LayoutUnit fillAvailableExtent = containingBlock()->availableLogicalHeight(ExcludeMarginBorderPadding);
        return std::min(fillAvailableExtent, fillFallbackExtent);
    }

    // Use the content box logical height as specified by the style.
    return cb->adjustContentBoxLogicalHeightForBoxSizing(logicalHeightLength.value());
}

void LayoutBox::mapLocalToContainer(const LayoutBoxModelObject* paintInvalidationContainer, TransformState& transformState, MapCoordinatesFlags mode, bool* wasFixed, const PaintInvalidationState* paintInvalidationState) const
{
    if (paintInvalidationContainer == this)
        return;

    if (paintInvalidationState && paintInvalidationState->canMapToContainer(paintInvalidationContainer)) {
        LayoutSize offset = paintInvalidationState->paintOffset() + locationOffset();
        if (style()->hasInFlowPosition() && layer())
            offset += layer()->offsetForInFlowPosition();
        transformState.move(offset);
        return;
    }

    bool containerSkipped;
    LayoutObject* o = container(paintInvalidationContainer, &containerSkipped);
    if (!o)
        return;

    bool isFixedPos = style()->position() == FixedPosition;
    bool hasTransform = hasLayer() && layer()->transform();
    // If this box has a transform, it acts as a fixed position container for fixed descendants,
    // and may itself also be fixed position. So propagate 'fixed' up only if this box is fixed position.
    if (hasTransform && !isFixedPos)
        mode &= ~IsFixed;
    else if (isFixedPos)
        mode |= IsFixed;

    if (wasFixed)
        *wasFixed = mode & IsFixed;

    LayoutSize containerOffset = offsetFromContainer(o, roundedLayoutPoint(transformState.mappedPoint()));

    bool preserve3D = mode & UseTransforms && (o->style()->preserves3D() || style()->preserves3D());
    if (mode & UseTransforms && shouldUseTransformFromContainer(o)) {
        TransformationMatrix t;
        getTransformFromContainer(o, containerOffset, t);
        transformState.applyTransform(t, preserve3D ? TransformState::AccumulateTransform : TransformState::FlattenTransform);
    } else {
        transformState.move(containerOffset.width(), containerOffset.height(), preserve3D ? TransformState::AccumulateTransform : TransformState::FlattenTransform);
    }

    if (containerSkipped) {
        // There can't be a transform between paintInvalidationContainer and o, because transforms create containers, so it should be safe
        // to just subtract the delta between the paintInvalidationContainer and o.
        LayoutSize containerOffset = paintInvalidationContainer->offsetFromAncestorContainer(o);
        transformState.move(-containerOffset.width(), -containerOffset.height(), preserve3D ? TransformState::AccumulateTransform : TransformState::FlattenTransform);
        return;
    }

    mode &= ~ApplyContainerFlip;

    o->mapLocalToContainer(paintInvalidationContainer, transformState, mode, wasFixed);
}

void LayoutBox::mapAbsoluteToLocalPoint(MapCoordinatesFlags mode, TransformState& transformState) const
{
    bool isFixedPos = style()->position() == FixedPosition;
    bool hasTransform = hasLayer() && layer()->transform();
    if (hasTransform && !isFixedPos) {
        // If this box has a transform, it acts as a fixed position container for fixed descendants,
        // and may itself also be fixed position. So propagate 'fixed' up only if this box is fixed position.
        mode &= ~IsFixed;
    } else if (isFixedPos) {
        mode |= IsFixed;
    }

    LayoutBoxModelObject::mapAbsoluteToLocalPoint(mode, transformState);
}

LayoutSize LayoutBox::offsetFromContainer(const LayoutObject* o, const LayoutPoint& point, bool* offsetDependsOnPoint) const
{
    ASSERT(o == container());

    LayoutSize offset;
    if (isRelPositioned())
        offset += offsetForInFlowPosition();

    if (!isInline() || isReplaced()) {
        offset += topLeftLocationOffset();
        if (o->isLayoutFlowThread()) {
            // So far the point has been in flow thread coordinates (i.e. as if everything in
            // the fragmentation context lived in one tall single column). Convert it to a
            // visual point now.
            LayoutPoint pointInContainer = point + offset;
            offset += o->columnOffset(pointInContainer);
            if (offsetDependsOnPoint)
                *offsetDependsOnPoint = true;
        }
    }

    if (o->hasOverflowClip())
        offset -= toLayoutBox(o)->scrolledContentOffset();

    if (style()->position() == AbsolutePosition && o->isRelPositioned() && o->isLayoutInline())
        offset += toLayoutInline(o)->offsetForInFlowPositionedInline(*this);

    return offset;
}

InlineBox* LayoutBox::createInlineBox()
{
    return new InlineBox(*this);
}

void LayoutBox::dirtyLineBoxes(bool fullLayout)
{
    if (inlineBoxWrapper()) {
        if (fullLayout) {
            inlineBoxWrapper()->destroy();
            ASSERT(m_rareData);
            m_rareData->m_inlineBoxWrapper = nullptr;
        } else {
            inlineBoxWrapper()->dirtyLineBoxes();
        }
    }
}

void LayoutBox::positionLineBox(InlineBox* box)
{
    if (isOutOfFlowPositioned()) {
        // Cache the x position only if we were an INLINE type originally.
        bool originallyInline = style()->isOriginalDisplayInlineType();
        if (originallyInline) {
            // The value is cached in the xPos of the box.  We only need this value if
            // our object was inline originally, since otherwise it would have ended up underneath
            // the inlines.
            RootInlineBox& root = box->root();
            root.block().setStaticInlinePositionForChild(*this, box->logicalLeft());
        } else {
            // Our object was a block originally, so we make our normal flow position be
            // just below the line box (as though all the inlines that came before us got
            // wrapped in an anonymous block, which is what would have happened had we been
            // in flow).  This value was cached in the y() of the box.
            layer()->setStaticBlockPosition(box->logicalTop());
        }

        if (container()->isLayoutInline())
            moveWithEdgeOfInlineContainerIfNecessary(box->isHorizontal());

        // Nuke the box.
        box->remove(DontMarkLineBoxes);
        box->destroy();
    } else if (isReplaced()) {
        // FIXME: the call to roundedLayoutPoint() below is temporary and should be removed once
        // the transition to LayoutUnit-based types is complete (crbug.com/321237)
        setLocationAndUpdateOverflowControlsIfNeeded(box->topLeft());
        setInlineBoxWrapper(box);
    }
}

void LayoutBox::moveWithEdgeOfInlineContainerIfNecessary(bool isHorizontal)
{
    ASSERT(isOutOfFlowPositioned() && container()->isLayoutInline() && container()->isRelPositioned());
    // If this object is inside a relative positioned inline and its inline position is an explicit offset from the edge of its container
    // then it will need to move if its inline container has changed width. We do not track if the width has changed
    // but if we are here then we are laying out lines inside it, so it probably has - mark our object for layout so that it can
    // move to the new offset created by the new width.
    if (!normalChildNeedsLayout() && !style()->hasStaticInlinePosition(isHorizontal))
        setChildNeedsLayout(MarkOnlyThis);
}

void LayoutBox::deleteLineBoxWrapper()
{
    if (inlineBoxWrapper()) {
        if (!documentBeingDestroyed())
            inlineBoxWrapper()->remove();
        inlineBoxWrapper()->destroy();
        ASSERT(m_rareData);
        m_rareData->m_inlineBoxWrapper = nullptr;
    }
}

void LayoutBox::setSpannerPlaceholder(LayoutMultiColumnSpannerPlaceholder& placeholder)
{
    RELEASE_ASSERT(!m_rareData || !m_rareData->m_spannerPlaceholder); // not expected to change directly from one spanner to another.
    ensureRareData().m_spannerPlaceholder = &placeholder;
}

void LayoutBox::clearSpannerPlaceholder()
{
    if (!m_rareData)
        return;
    m_rareData->m_spannerPlaceholder = nullptr;
}

LayoutRect LayoutBox::clippedOverflowRectForPaintInvalidation(const LayoutBoxModelObject* paintInvalidationContainer, const PaintInvalidationState* paintInvalidationState) const
{
    if (style()->visibility() != VISIBLE) {
        DeprecatedPaintLayer* layer = enclosingLayer();
        layer->updateDescendantDependentFlags();
        if (layer->subtreeIsInvisible())
            return LayoutRect();
    }

    LayoutRect r = visualOverflowRect();
    mapRectToPaintInvalidationBacking(paintInvalidationContainer, r, paintInvalidationState);
    return r;
}

void LayoutBox::mapRectToPaintInvalidationBacking(const LayoutBoxModelObject* paintInvalidationContainer, LayoutRect& rect, const PaintInvalidationState* paintInvalidationState) const
{
    // The rect we compute at each step is shifted by our x/y offset in the parent container's coordinate space.
    // Only when we cross a writing mode boundary will we have to possibly flipForWritingMode (to convert into a more appropriate
    // offset corner for the enclosing container). This allows for a fully RL or BT document to issue paint invalidations
    // properly even during layout, since the rect remains flipped all the way until the end.
    //
    // LayoutView::computeRectForPaintInvalidation then converts the rect to physical coordinates. We also convert to
    // physical when we hit a paintInvalidationContainer boundary. Therefore the final rect returned is always in the
    // physical coordinate space of the paintInvalidationContainer.
    const ComputedStyle& styleToUse = styleRef();

    EPosition position = styleToUse.position();

    // We need to inflate the paint invalidation rect before we use paintInvalidationState,
    // else we would forget to inflate it for the current layoutObject. FIXME: If these were
    // included into the visual overflow for repaint, we wouldn't have this issue.
    inflatePaintInvalidationRectForReflectionAndFilter(rect);

    if (paintInvalidationState && paintInvalidationState->canMapToContainer(paintInvalidationContainer) && position != FixedPosition) {
        if (layer() && layer()->transform())
            rect = LayoutRect(layer()->transform()->mapRect(pixelSnappedIntRect(rect)));

        // We can't trust the bits on LayoutObject, because this might be called while re-resolving style.
        if (styleToUse.hasInFlowPosition() && layer())
            rect.move(layer()->offsetForInFlowPosition());

        rect.moveBy(location());
        rect.move(paintInvalidationState->paintOffset());
        if (paintInvalidationState->isClipped())
            rect.intersect(paintInvalidationState->clipRect());
        return;
    }

    if (paintInvalidationContainer == this) {
        if (paintInvalidationContainer->style()->isFlippedBlocksWritingMode())
            flipForWritingMode(rect);
        return;
    }

    bool containerSkipped;
    LayoutObject* o = container(paintInvalidationContainer, &containerSkipped);
    if (!o)
        return;

    if (isWritingModeRoot())
        flipForWritingMode(rect);

    LayoutPoint topLeft = rect.location();
    topLeft.move(locationOffset());

    // We are now in our parent container's coordinate space.  Apply our transform to obtain a bounding box
    // in the parent's coordinate space that encloses us.
    if (hasLayer() && layer()->transform()) {
        rect = LayoutRect(layer()->transform()->mapRect(pixelSnappedIntRect(rect)));
        topLeft = rect.location();
        topLeft.move(locationOffset());
    }

    if (position == AbsolutePosition && o->isRelPositioned() && o->isLayoutInline()) {
        topLeft += toLayoutInline(o)->offsetForInFlowPositionedInline(*this);
    } else if (styleToUse.hasInFlowPosition() && layer()) {
        // Apply the relative position offset when invalidating a rectangle.  The layer
        // is translated, but the layout box isn't, so we need to do this to get the
        // right dirty rect.  Since this is called from LayoutObject::setStyle, the relative position
        // flag on the LayoutObject has been cleared, so use the one on the style().
        topLeft += layer()->offsetForInFlowPosition();
    }

    // FIXME: We ignore the lightweight clipping rect that controls use, since if |o| is in mid-layout,
    // its controlClipRect will be wrong. For overflow clip we use the values cached by the layer.
    rect.setLocation(topLeft);
    if (o->hasOverflowClip()) {
        LayoutBox* containerBox = toLayoutBox(o);
        containerBox->applyCachedClipAndScrollOffsetForPaintInvalidation(rect);
        if (rect.isEmpty())
            return;
    }

    if (containerSkipped) {
        // If the paintInvalidationContainer is below o, then we need to map the rect into paintInvalidationContainer's coordinates.
        LayoutSize containerOffset = paintInvalidationContainer->offsetFromAncestorContainer(o);
        rect.move(-containerOffset);
        // If the paintInvalidationContainer is fixed, then the rect is already in its coordinates so doesn't need viewport-adjusting.
        if (paintInvalidationContainer->style()->position() != FixedPosition && o->isLayoutView())
            toLayoutView(o)->adjustViewportConstrainedOffset(rect, LayoutView::viewportConstrainedPosition(position));
        return;
    }

    if (o->isLayoutView())
        toLayoutView(o)->mapRectToPaintInvalidationBacking(paintInvalidationContainer, rect, LayoutView::viewportConstrainedPosition(position), paintInvalidationState);
    else
        o->mapRectToPaintInvalidationBacking(paintInvalidationContainer, rect, paintInvalidationState);
}

void LayoutBox::inflatePaintInvalidationRectForReflectionAndFilter(LayoutRect& paintInvalidationRect) const
{
    if (hasReflection())
        paintInvalidationRect.unite(reflectedRect(paintInvalidationRect));

    if (style()->hasFilter())
        paintInvalidationRect.expand(style()->filterOutsets());
}

void LayoutBox::invalidatePaintForOverhangingFloats(bool)
{
}

void LayoutBox::updateLogicalWidth()
{
    LogicalExtentComputedValues computedValues;
    computeLogicalWidth(computedValues);

    setLogicalWidth(computedValues.m_extent);
    setLogicalLeft(computedValues.m_position);
    setMarginStart(computedValues.m_margins.m_start);
    setMarginEnd(computedValues.m_margins.m_end);
}

static float getMaxWidthListMarker(const LayoutBox* layoutObject)
{
#if ENABLE(ASSERT)
    ASSERT(layoutObject);
    Node* parentNode = layoutObject->generatingNode();
    ASSERT(parentNode);
    ASSERT(isHTMLOListElement(parentNode) || isHTMLUListElement(parentNode));
    ASSERT(layoutObject->style()->textAutosizingMultiplier() != 1);
#endif
    float maxWidth = 0;
    for (LayoutObject* child = layoutObject->slowFirstChild(); child; child = child->nextSibling()) {
        if (!child->isListItem())
            continue;

        LayoutBox* listItem = toLayoutBox(child);
        for (LayoutObject* itemChild = listItem->slowFirstChild(); itemChild; itemChild = itemChild->nextSibling()) {
            if (!itemChild->isListMarker())
                continue;
            LayoutBox* itemMarker = toLayoutBox(itemChild);
            // Make sure to compute the autosized width.
            if (itemMarker->needsLayout())
                itemMarker->layout();
            maxWidth = std::max<float>(maxWidth, toLayoutListMarker(itemMarker)->logicalWidth().toFloat());
            break;
        }
    }
    return maxWidth;
}

void LayoutBox::computeLogicalWidth(LogicalExtentComputedValues& computedValues) const
{
    computedValues.m_extent = logicalWidth();
    computedValues.m_position = logicalLeft();
    computedValues.m_margins.m_start = marginStart();
    computedValues.m_margins.m_end = marginEnd();

    if (isOutOfFlowPositioned()) {
        computePositionedLogicalWidth(computedValues);
        return;
    }

    // The parent box is flexing us, so it has increased or decreased our
    // width.  Use the width from the style context.
    // FIXME: Account for writing-mode in flexible boxes.
    // https://bugs.webkit.org/show_bug.cgi?id=46418
    if (hasOverrideLogicalContentWidth() && parent()->isFlexibleBoxIncludingDeprecated()) {
        computedValues.m_extent = overrideLogicalContentWidth() + borderAndPaddingLogicalWidth();
        return;
    }

    // FIXME: Account for writing-mode in flexible boxes.
    // https://bugs.webkit.org/show_bug.cgi?id=46418
    bool inVerticalBox = parent()->isDeprecatedFlexibleBox() && (parent()->style()->boxOrient() == VERTICAL);
    bool stretching = (parent()->style()->boxAlign() == BSTRETCH);
    bool treatAsReplaced = shouldComputeSizeAsReplaced() && (!inVerticalBox || !stretching);

    const ComputedStyle& styleToUse = styleRef();
    Length logicalWidthLength = treatAsReplaced ? Length(computeReplacedLogicalWidth(), Fixed) : styleToUse.logicalWidth();

    LayoutBlock* cb = containingBlock();
    LayoutUnit containerLogicalWidth = std::max(LayoutUnit(), containingBlockLogicalWidthForContent());
    bool hasPerpendicularContainingBlock = cb->isHorizontalWritingMode() != isHorizontalWritingMode();

    if (parent()->isLayoutGrid() && style()->logicalWidth().isAuto() && style()->minWidth().isAuto() && style()->overflowX() == OVISIBLE) {
        LayoutUnit minLogicalWidth = minPreferredLogicalWidth();
        if (containerLogicalWidth < minLogicalWidth) {
            computedValues.m_extent = constrainLogicalWidthByMinMax(minLogicalWidth, containerLogicalWidth, cb);
            return;
        }
    }

    if (isInline() && !isInlineBlockOrInlineTable()) {
        // just calculate margins
        computedValues.m_margins.m_start = minimumValueForLength(styleToUse.marginStart(), containerLogicalWidth);
        computedValues.m_margins.m_end = minimumValueForLength(styleToUse.marginEnd(), containerLogicalWidth);
        if (treatAsReplaced)
            computedValues.m_extent = std::max<LayoutUnit>(floatValueForLength(logicalWidthLength, 0) + borderAndPaddingLogicalWidth(), minPreferredLogicalWidth());
        return;
    }

    // Width calculations
    if (treatAsReplaced) {
        computedValues.m_extent = logicalWidthLength.value() + borderAndPaddingLogicalWidth();
    } else {
        LayoutUnit containerWidthInInlineDirection = containerLogicalWidth;
        if (hasPerpendicularContainingBlock)
            containerWidthInInlineDirection = perpendicularContainingBlockLogicalHeight();
        LayoutUnit preferredWidth = computeLogicalWidthUsing(MainOrPreferredSize, styleToUse.logicalWidth(), containerWidthInInlineDirection, cb);
        computedValues.m_extent = constrainLogicalWidthByMinMax(preferredWidth, containerWidthInInlineDirection, cb);
    }

    // Margin calculations.
    computeMarginsForDirection(InlineDirection, cb, containerLogicalWidth, computedValues.m_extent, computedValues.m_margins.m_start,
        computedValues.m_margins.m_end, style()->marginStart(), style()->marginEnd());

    if (!hasPerpendicularContainingBlock && containerLogicalWidth && containerLogicalWidth != (computedValues.m_extent + computedValues.m_margins.m_start + computedValues.m_margins.m_end)
        && !isFloating() && !isInline() && !cb->isFlexibleBoxIncludingDeprecated() && !cb->isLayoutGrid()) {
        LayoutUnit newMargin = containerLogicalWidth - computedValues.m_extent - cb->marginStartForChild(*this);
        bool hasInvertedDirection = cb->style()->isLeftToRightDirection() != style()->isLeftToRightDirection();
        if (hasInvertedDirection)
            computedValues.m_margins.m_start = newMargin;
        else
            computedValues.m_margins.m_end = newMargin;
    }

    if (styleToUse.textAutosizingMultiplier() != 1 && styleToUse.marginStart().type() == Fixed) {
        Node* parentNode = generatingNode();
        if (parentNode && (isHTMLOListElement(*parentNode) || isHTMLUListElement(*parentNode))) {
            // Make sure the markers in a list are properly positioned (i.e. not chopped off) when autosized.
            const float adjustedMargin = (1 - 1.0 / styleToUse.textAutosizingMultiplier()) * getMaxWidthListMarker(this);
            bool hasInvertedDirection = cb->style()->isLeftToRightDirection() != style()->isLeftToRightDirection();
            if (hasInvertedDirection)
                computedValues.m_margins.m_end += adjustedMargin;
            else
                computedValues.m_margins.m_start += adjustedMargin;
        }
    }
}

LayoutUnit LayoutBox::fillAvailableMeasure(LayoutUnit availableLogicalWidth) const
{
    LayoutUnit marginStart = 0;
    LayoutUnit marginEnd = 0;
    return fillAvailableMeasure(availableLogicalWidth, marginStart, marginEnd);
}

LayoutUnit LayoutBox::fillAvailableMeasure(LayoutUnit availableLogicalWidth, LayoutUnit& marginStart, LayoutUnit& marginEnd) const
{
    marginStart = minimumValueForLength(style()->marginStart(), availableLogicalWidth);
    marginEnd = minimumValueForLength(style()->marginEnd(), availableLogicalWidth);
    return availableLogicalWidth - marginStart - marginEnd;
}

LayoutUnit LayoutBox::computeIntrinsicLogicalWidthUsing(const Length& logicalWidthLength, LayoutUnit availableLogicalWidth, LayoutUnit borderAndPadding) const
{
    if (logicalWidthLength.type() == FillAvailable)
        return fillAvailableMeasure(availableLogicalWidth);

    LayoutUnit minLogicalWidth = 0;
    LayoutUnit maxLogicalWidth = 0;
    computeIntrinsicLogicalWidths(minLogicalWidth, maxLogicalWidth);

    if (logicalWidthLength.type() == MinContent)
        return minLogicalWidth + borderAndPadding;

    if (logicalWidthLength.type() == MaxContent)
        return maxLogicalWidth + borderAndPadding;

    if (logicalWidthLength.type() == FitContent) {
        minLogicalWidth += borderAndPadding;
        maxLogicalWidth += borderAndPadding;
        return std::max(minLogicalWidth, std::min(maxLogicalWidth, fillAvailableMeasure(availableLogicalWidth)));
    }

    ASSERT_NOT_REACHED();
    return 0;
}

LayoutUnit LayoutBox::computeLogicalWidthUsing(SizeType widthType, const Length& logicalWidth, LayoutUnit availableLogicalWidth, const LayoutBlock* cb) const
{
    ASSERT(widthType == MinSize || widthType == MainOrPreferredSize || !logicalWidth.isAuto());
    if (widthType == MinSize && logicalWidth.isAuto())
        return adjustBorderBoxLogicalWidthForBoxSizing(0);

    if (!logicalWidth.isIntrinsicOrAuto()) {
        // FIXME: If the containing block flow is perpendicular to our direction we need to use the available logical height instead.
        return adjustBorderBoxLogicalWidthForBoxSizing(valueForLength(logicalWidth, availableLogicalWidth));
    }

    if (logicalWidth.isIntrinsic())
        return computeIntrinsicLogicalWidthUsing(logicalWidth, availableLogicalWidth, borderAndPaddingLogicalWidth());

    LayoutUnit marginStart = 0;
    LayoutUnit marginEnd = 0;
    LayoutUnit logicalWidthResult = fillAvailableMeasure(availableLogicalWidth, marginStart, marginEnd);

    if (shrinkToAvoidFloats() && cb->isLayoutBlockFlow() && toLayoutBlockFlow(cb)->containsFloats())
        logicalWidthResult = std::min(logicalWidthResult, shrinkLogicalWidthToAvoidFloats(marginStart, marginEnd, toLayoutBlockFlow(cb)));

    if (widthType == MainOrPreferredSize && sizesLogicalWidthToFitContent(logicalWidth))
        return std::max(minPreferredLogicalWidth(), std::min(maxPreferredLogicalWidth(), logicalWidthResult));
    return logicalWidthResult;
}

static bool columnFlexItemHasStretchAlignment(const LayoutObject* flexitem)
{
    LayoutObject* parent = flexitem->parent();
    // auto margins mean we don't stretch. Note that this function will only be used for
    // widths, so we don't have to check marginBefore/marginAfter.
    ASSERT(parent->style()->isColumnFlexDirection());
    if (flexitem->style()->marginStart().isAuto() || flexitem->style()->marginEnd().isAuto())
        return false;
    return flexitem->style()->alignSelfPosition() == ItemPositionStretch || (flexitem->style()->alignSelfPosition() == ItemPositionAuto && parent->style()->alignItemsPosition() == ItemPositionStretch);
}

static bool isStretchingColumnFlexItem(const LayoutObject* flexitem)
{
    LayoutObject* parent = flexitem->parent();
    if (parent->isDeprecatedFlexibleBox() && parent->style()->boxOrient() == VERTICAL && parent->style()->boxAlign() == BSTRETCH)
        return true;

    // We don't stretch multiline flexboxes because they need to apply line spacing (align-content) first.
    if (parent->isFlexibleBox() && parent->style()->flexWrap() == FlexNoWrap && parent->style()->isColumnFlexDirection() && columnFlexItemHasStretchAlignment(flexitem))
        return true;
    return false;
}

bool LayoutBox::sizesLogicalWidthToFitContent(const Length& logicalWidth) const
{
    if (isFloating() || isInlineBlockOrInlineTable())
        return true;

    if (logicalWidth.type() == Intrinsic)
        return true;

    // Flexible box items should shrink wrap, so we lay them out at their intrinsic widths.
    // In the case of columns that have a stretch alignment, we go ahead and layout at the
    // stretched size to avoid an extra layout when applying alignment.
    if (parent()->isFlexibleBox()) {
        // For multiline columns, we need to apply align-content first, so we can't stretch now.
        if (!parent()->style()->isColumnFlexDirection() || parent()->style()->flexWrap() != FlexNoWrap)
            return true;
        if (!columnFlexItemHasStretchAlignment(this))
            return true;
    }

    // Flexible horizontal boxes lay out children at their intrinsic widths.  Also vertical boxes
    // that don't stretch their kids lay out their children at their intrinsic widths.
    // FIXME: Think about writing-mode here.
    // https://bugs.webkit.org/show_bug.cgi?id=46473
    if (parent()->isDeprecatedFlexibleBox() && (parent()->style()->boxOrient() == HORIZONTAL || parent()->style()->boxAlign() != BSTRETCH))
        return true;

    // Button, input, select, textarea, and legend treat width value of 'auto' as 'intrinsic' unless it's in a
    // stretching column flexbox.
    // FIXME: Think about writing-mode here.
    // https://bugs.webkit.org/show_bug.cgi?id=46473
    if (logicalWidth.isAuto() && !isStretchingColumnFlexItem(this) && autoWidthShouldFitContent())
        return true;

    if (isHorizontalWritingMode() != containingBlock()->isHorizontalWritingMode())
        return true;

    return false;
}

bool LayoutBox::autoWidthShouldFitContent() const
{
    return node() && (isHTMLInputElement(*node()) || isHTMLSelectElement(*node()) || isHTMLButtonElement(*node())
        || isHTMLTextAreaElement(*node()) || (isHTMLLegendElement(*node()) && !style()->hasOutOfFlowPosition()));
}

void LayoutBox::computeMarginsForDirection(MarginDirection flowDirection, const LayoutBlock* containingBlock, LayoutUnit containerWidth, LayoutUnit childWidth, LayoutUnit& marginStart, LayoutUnit& marginEnd, Length marginStartLength, Length marginEndLength) const
{
    // First assert that we're not calling this method on box types that don't support margins.
    ASSERT(!isTableCell());
    ASSERT(!isTableRow());
    ASSERT(!isTableSection());
    ASSERT(!isLayoutTableCol());
    if (flowDirection == BlockDirection || isFloating() || isInline()) {
        // Margins are calculated with respect to the logical width of
        // the containing block (8.3)
        // Inline blocks/tables and floats don't have their margins increased.
        marginStart = minimumValueForLength(marginStartLength, containerWidth);
        marginEnd = minimumValueForLength(marginEndLength, containerWidth);
        return;
    }

    if (containingBlock->isFlexibleBox()) {
        // We need to let flexbox handle the margin adjustment - otherwise, flexbox
        // will think we're wider than we actually are and calculate line sizes wrong.
        // See also http://dev.w3.org/csswg/css-flexbox/#auto-margins
        if (marginStartLength.isAuto())
            marginStartLength.setValue(0);
        if (marginEndLength.isAuto())
            marginEndLength.setValue(0);
    }

    LayoutUnit marginStartWidth = minimumValueForLength(marginStartLength, containerWidth);
    LayoutUnit marginEndWidth = minimumValueForLength(marginEndLength, containerWidth);

    LayoutUnit availableWidth = containerWidth;
    if (avoidsFloats() && containingBlock->isLayoutBlockFlow() && toLayoutBlockFlow(containingBlock)->containsFloats()) {
        availableWidth = containingBlockAvailableLineWidth();
        if (shrinkToAvoidFloats() && availableWidth < containerWidth) {
            marginStart = std::max(LayoutUnit(), marginStartWidth);
            marginEnd = std::max(LayoutUnit(), marginEndWidth);
        }
    }

    // CSS 2.1 (10.3.3): "If 'width' is not 'auto' and 'border-left-width' + 'padding-left' + 'width' + 'padding-right' + 'border-right-width'
    // (plus any of 'margin-left' or 'margin-right' that are not 'auto') is larger than the width of the containing block, then any 'auto'
    // values for 'margin-left' or 'margin-right' are, for the following rules, treated as zero.
    LayoutUnit marginBoxWidth = childWidth + (!style()->width().isAuto() ? marginStartWidth + marginEndWidth : LayoutUnit());

    if (marginBoxWidth < availableWidth) {
        // CSS 2.1: "If both 'margin-left' and 'margin-right' are 'auto', their used values are equal. This horizontally centers the element
        // with respect to the edges of the containing block."
        const ComputedStyle& containingBlockStyle = containingBlock->styleRef();
        if ((marginStartLength.isAuto() && marginEndLength.isAuto())
            || (!marginStartLength.isAuto() && !marginEndLength.isAuto() && containingBlockStyle.textAlign() == WEBKIT_CENTER)) {
            // Other browsers center the margin box for align=center elements so we match them here.
            LayoutUnit centeredMarginBoxStart = std::max(LayoutUnit(), (availableWidth - childWidth - marginStartWidth - marginEndWidth) / 2);
            marginStart = centeredMarginBoxStart + marginStartWidth;
            marginEnd = availableWidth - childWidth - marginStart + marginEndWidth;
            return;
        }

        // Adjust margins for the align attribute
        if ((!containingBlockStyle.isLeftToRightDirection() && containingBlockStyle.textAlign() == WEBKIT_LEFT)
            || (containingBlockStyle.isLeftToRightDirection() && containingBlockStyle.textAlign() == WEBKIT_RIGHT)) {
            if (containingBlockStyle.isLeftToRightDirection() != styleRef().isLeftToRightDirection()) {
                if (!marginStartLength.isAuto())
                    marginEndLength = Length(Auto);
            } else {
                if (!marginEndLength.isAuto())
                    marginStartLength = Length(Auto);
            }
        }

        // CSS 2.1: "If there is exactly one value specified as 'auto', its used value follows from the equality."
        if (marginEndLength.isAuto()) {
            marginStart = marginStartWidth;
            marginEnd = availableWidth - childWidth - marginStart;
            return;
        }

        if (marginStartLength.isAuto()) {
            marginEnd = marginEndWidth;
            marginStart = availableWidth - childWidth - marginEnd;
            return;
        }
    }

    // Either no auto margins, or our margin box width is >= the container width, auto margins will just turn into 0.
    marginStart = marginStartWidth;
    marginEnd = marginEndWidth;
}

void LayoutBox::updateLogicalHeight()
{
    m_intrinsicContentLogicalHeight = contentLogicalHeight();

    LogicalExtentComputedValues computedValues;
    computeLogicalHeight(logicalHeight(), logicalTop(), computedValues);
        
    setLogicalHeight(computedValues.m_extent);
    setLogicalTop(computedValues.m_position);
    setMarginBefore(computedValues.m_margins.m_before);
    setMarginAfter(computedValues.m_margins.m_after);
}

void LayoutBox::computeLogicalHeight(LayoutUnit logicalHeight, LayoutUnit logicalTop, LogicalExtentComputedValues& computedValues) const
{
    computedValues.m_extent = logicalHeight;
    computedValues.m_position = logicalTop;

    // Cell height is managed by the table and inline non-replaced elements do not support a height property.
    if (isTableCell() || (isInline() && !isReplaced()))
        return;

    Length h;
    if (isOutOfFlowPositioned()) {
        computePositionedLogicalHeight(computedValues);
    } else {
        LayoutBlock* cb = containingBlock();

        // If we are perpendicular to our containing block then we need to resolve our block-start and block-end margins so that if they
        // are 'auto' we are centred or aligned within the inline flow containing block: this is done by computing the margins as though they are inline.
        // Note that as this is the 'sizing phase' we are using our own writing mode rather than the containing block's. We use the containing block's
        // writing mode when figuring out the block-direction margins for positioning in |computeAndSetBlockDirectionMargins| (i.e. margin collapsing etc.).
        // See http://www.w3.org/TR/2014/CR-css-writing-modes-3-20140320/#orthogonal-flows
        MarginDirection flowDirection = isHorizontalWritingMode() != cb->isHorizontalWritingMode() ? InlineDirection : BlockDirection;

        // For tables, calculate margins only.
        if (isTable()) {
            computeMarginsForDirection(flowDirection, cb, containingBlockLogicalWidthForContent(), computedValues.m_extent, computedValues.m_margins.m_before,
                computedValues.m_margins.m_after, style()->marginBefore(), style()->marginAfter());
            return;
        }

        // FIXME: Account for writing-mode in flexible boxes.
        // https://bugs.webkit.org/show_bug.cgi?id=46418
        bool inHorizontalBox = parent()->isDeprecatedFlexibleBox() && parent()->style()->boxOrient() == HORIZONTAL;
        bool stretching = parent()->style()->boxAlign() == BSTRETCH;
        bool treatAsReplaced = shouldComputeSizeAsReplaced() && (!inHorizontalBox || !stretching);
        bool checkMinMaxHeight = false;

        // The parent box is flexing us, so it has increased or decreased our height.  We have to
        // grab our cached flexible height.
        // FIXME: Account for writing-mode in flexible boxes.
        // https://bugs.webkit.org/show_bug.cgi?id=46418
        if (hasOverrideLogicalContentHeight() && (parent()->isFlexibleBoxIncludingDeprecated() || parent()->isLayoutGrid())) {
            LayoutUnit contentHeight = overrideLogicalContentHeight();
            if (parent()->isLayoutGrid() && style()->minHeight().isAuto() && style()->overflowY() == OVISIBLE) {
                ASSERT(style()->logicalHeight().isAuto());
                LayoutUnit minContentHeight = computeContentLogicalHeight(MinSize, Length(MinContent), computedValues.m_extent - borderAndPaddingLogicalHeight());
                contentHeight = std::max(contentHeight, constrainLogicalHeightByMinMax(minContentHeight, computedValues.m_extent - borderAndPaddingLogicalHeight()));
            }
            h = Length(contentHeight, Fixed);
        } else if (treatAsReplaced) {
            h = Length(computeReplacedLogicalHeight(), Fixed);
        } else {
            h = style()->logicalHeight();
            checkMinMaxHeight = true;
        }

        // Block children of horizontal flexible boxes fill the height of the box.
        // FIXME: Account for writing-mode in flexible boxes.
        // https://bugs.webkit.org/show_bug.cgi?id=46418
        if (h.isAuto() && inHorizontalBox && toLayoutDeprecatedFlexibleBox(parent())->isStretchingChildren()) {
            h = Length(parentBox()->contentLogicalHeight() - marginBefore() - marginAfter() - borderAndPaddingLogicalHeight(), Fixed);
            checkMinMaxHeight = false;
        }

        LayoutUnit heightResult;
        if (checkMinMaxHeight) {
            heightResult = computeLogicalHeightUsing(MainOrPreferredSize, style()->logicalHeight(), computedValues.m_extent - borderAndPaddingLogicalHeight());
            if (heightResult == -1)
                heightResult = computedValues.m_extent;
            heightResult = constrainLogicalHeightByMinMax(heightResult, computedValues.m_extent - borderAndPaddingLogicalHeight());
        } else {
            // The only times we don't check min/max height are when a fixed length has
            // been given as an override.  Just use that.  The value has already been adjusted
            // for box-sizing.
            ASSERT(h.isFixed());
            heightResult = h.value() + borderAndPaddingLogicalHeight();
        }

        computedValues.m_extent = heightResult;
        computeMarginsForDirection(flowDirection, cb, containingBlockLogicalWidthForContent(), computedValues.m_extent, computedValues.m_margins.m_before,
            computedValues.m_margins.m_after, style()->marginBefore(), style()->marginAfter());
    }

    // WinIE quirk: The <html> block always fills the entire canvas in quirks mode.  The <body> always fills the
    // <html> block in quirks mode.  Only apply this quirk if the block is normal flow and no height
    // is specified. When we're printing, we also need this quirk if the body or root has a percentage
    // height since we don't set a height in LayoutView when we're printing. So without this quirk, the
    // height has nothing to be a percentage of, and it ends up being 0. That is bad.
    bool paginatedContentNeedsBaseHeight = document().printing() && h.hasPercent()
        && (isDocumentElement() || (isBody() && document().documentElement()->layoutObject()->style()->logicalHeight().hasPercent())) && !isInline();
    if (stretchesToViewport() || paginatedContentNeedsBaseHeight) {
        LayoutUnit margins = collapsedMarginBefore() + collapsedMarginAfter();
        LayoutUnit visibleHeight = view()->viewLogicalHeightForPercentages();
        if (isDocumentElement()) {
            computedValues.m_extent = std::max(computedValues.m_extent, visibleHeight - margins);
        } else {
            LayoutUnit marginsBordersPadding = margins + parentBox()->marginBefore() + parentBox()->marginAfter() + parentBox()->borderAndPaddingLogicalHeight();
            computedValues.m_extent = std::max(computedValues.m_extent, visibleHeight - marginsBordersPadding);
        }
    }
}

LayoutUnit LayoutBox::computeLogicalHeightWithoutLayout() const
{
    // TODO(cbiesinger): We should probably return something other than just border + padding, but for now
    // we have no good way to do anything else without layout, so we just use that.
    LogicalExtentComputedValues computedValues;
    computeLogicalHeight(borderAndPaddingLogicalHeight(), 0, computedValues);
    return computedValues.m_extent;
}

LayoutUnit LayoutBox::computeLogicalHeightUsing(SizeType heightType, const Length& height, LayoutUnit intrinsicContentHeight) const
{
    LayoutUnit logicalHeight = computeContentAndScrollbarLogicalHeightUsing(heightType, height, intrinsicContentHeight);
    if (logicalHeight != -1)
        logicalHeight = adjustBorderBoxLogicalHeightForBoxSizing(logicalHeight);
    return logicalHeight;
}

LayoutUnit LayoutBox::computeContentLogicalHeight(SizeType heightType, const Length& height, LayoutUnit intrinsicContentHeight) const
{
    LayoutUnit heightIncludingScrollbar = computeContentAndScrollbarLogicalHeightUsing(heightType, height, intrinsicContentHeight);
    if (heightIncludingScrollbar == -1)
        return -1;
    return std::max(LayoutUnit(), adjustContentBoxLogicalHeightForBoxSizing(heightIncludingScrollbar) - scrollbarLogicalHeight());
}

LayoutUnit LayoutBox::computeIntrinsicLogicalContentHeightUsing(const Length& logicalHeightLength, LayoutUnit intrinsicContentHeight, LayoutUnit borderAndPadding) const
{
    // FIXME(cbiesinger): The css-sizing spec is considering changing what min-content/max-content should resolve to.
    // If that happens, this code will have to change.
    if (logicalHeightLength.isMinContent() || logicalHeightLength.isMaxContent() || logicalHeightLength.isFitContent()) {
        if (isReplaced())
            return intrinsicSize().height();
        if (m_intrinsicContentLogicalHeight != -1)
            return m_intrinsicContentLogicalHeight;
        return intrinsicContentHeight;
    }
    if (logicalHeightLength.isFillAvailable())
        return containingBlock()->availableLogicalHeight(ExcludeMarginBorderPadding) - borderAndPadding;
    ASSERT_NOT_REACHED();
    return 0;
}

LayoutUnit LayoutBox::computeContentAndScrollbarLogicalHeightUsing(SizeType heightType, const Length& height, LayoutUnit intrinsicContentHeight) const
{
    if (height.isAuto())
        return heightType == MinSize ? 0 : -1;
    // FIXME(cbiesinger): The css-sizing spec is considering changing what min-content/max-content should resolve to.
    // If that happens, this code will have to change.
    if (height.isIntrinsic()) {
        if (intrinsicContentHeight == -1)
            return -1; // Intrinsic height isn't available.
        return computeIntrinsicLogicalContentHeightUsing(height, intrinsicContentHeight, borderAndPaddingLogicalHeight());
    }
    if (height.isFixed())
        return height.value();
    if (height.hasPercent())
        return computePercentageLogicalHeight(height);
    return -1;
}

bool LayoutBox::skipContainingBlockForPercentHeightCalculation(const LayoutBox* containingBlock) const
{
    // If the writing mode of the containing block is orthogonal to ours, it means that we shouldn't
    // skip anything, since we're going to resolve the percentage height against a containing block *width*.
    if (isHorizontalWritingMode() != containingBlock->isHorizontalWritingMode())
        return false;

    // Anonymous blocks should not impede percentage resolution on a child. Examples of such
    // anonymous blocks are blocks wrapped around inlines that have block siblings (from the CSS
    // spec) and multicol flow threads (an implementation detail). Another implementation detail,
    // ruby runs, create anonymous inline-blocks, so skip those too. All other types of anonymous
    // objects, such as table-cells, will be treated just as if they were non-anonymous.
    if (containingBlock->isAnonymous()) {
        EDisplay display = containingBlock->styleRef().display();
        return display == BLOCK || display == INLINE_BLOCK;
    }

    // For quirks mode, we skip most auto-height containing blocks when computing percentages.
    return document().inQuirksMode() && !containingBlock->isTableCell() && !containingBlock->isOutOfFlowPositioned() && containingBlock->style()->logicalHeight().isAuto();
}

static bool isFlexItem(const LayoutBox& box)
{
    return !box.isInline() && !box.isFloatingOrOutOfFlowPositioned() && box.parent() && box.parent()->isFlexibleBox();
}

LayoutUnit LayoutBox::computePercentageLogicalHeight(const Length& height) const
{
    LayoutUnit availableHeight = -1;

    bool skippedAutoHeightContainingBlock = false;
    LayoutBlock* cb = containingBlock();
    const LayoutBox* containingBlockChild = this;
    LayoutUnit rootMarginBorderPaddingHeight = 0;
    while (!cb->isLayoutView() && skipContainingBlockForPercentHeightCalculation(cb)) {
        if (cb->isBody() || cb->isDocumentElement())
            rootMarginBorderPaddingHeight += cb->marginBefore() + cb->marginAfter() + cb->borderAndPaddingLogicalHeight();
        skippedAutoHeightContainingBlock = true;
        containingBlockChild = cb;
        cb = cb->containingBlock();
    }
    cb->addPercentHeightDescendant(const_cast<LayoutBox*>(this));

    const ComputedStyle& cbstyle = cb->styleRef();

    // A positioned element that specified both top/bottom or that specifies height should be treated as though it has a height
    // explicitly specified that can be used for any percentage computations.
    bool isOutOfFlowPositionedWithSpecifiedHeight = cb->isOutOfFlowPositioned() && (!cbstyle.logicalHeight().isAuto() || (!cbstyle.logicalTop().isAuto() && !cbstyle.logicalBottom().isAuto()));

    bool includeBorderPadding = isTable();

    LayoutUnit stretchedFlexHeight(-1);
    if (isFlexItem(*cb))
        stretchedFlexHeight = toLayoutFlexibleBox(cb->parent())->childLogicalHeightForPercentageResolution(*cb);

    if (isHorizontalWritingMode() != cb->isHorizontalWritingMode()) {
        availableHeight = containingBlockChild->containingBlockLogicalWidthForContent();
    } else if (stretchedFlexHeight != LayoutUnit(-1)) {
        availableHeight = stretchedFlexHeight;
    } else if (hasOverrideContainingBlockLogicalHeight()) {
        availableHeight = overrideContainingBlockContentLogicalHeight();
    } else if (cb->isTableCell()) {
        if (!skippedAutoHeightContainingBlock) {
            // Table cells violate what the CSS spec says to do with heights. Basically we
            // don't care if the cell specified a height or not. We just always make ourselves
            // be a percentage of the cell's current content height.
            if (!cb->hasOverrideLogicalContentHeight()) {
                // Normally we would let the cell size intrinsically, but scrolling overflow has to be
                // treated differently, since WinIE lets scrolled overflow regions shrink as needed.
                // While we can't get all cases right, we can at least detect when the cell has a specified
                // height or when the table has a specified height. In these cases we want to initially have
                // no size and allow the flexing of the table or the cell to its specified height to cause us
                // to grow to fill the space. This could end up being wrong in some cases, but it is
                // preferable to the alternative (sizing intrinsically and making the row end up too big).
                LayoutTableCell* cell = toLayoutTableCell(cb);
                if (scrollsOverflowY() && (!cell->style()->logicalHeight().isAuto() || !cell->table()->style()->logicalHeight().isAuto()))
                    return LayoutUnit();
                return -1;
            }
            availableHeight = cb->overrideLogicalContentHeight();
            includeBorderPadding = true;
        }
    } else if (cbstyle.logicalHeight().isFixed()) {
        LayoutUnit contentBoxHeight = cb->adjustContentBoxLogicalHeightForBoxSizing(cbstyle.logicalHeight().value());
        availableHeight = std::max(LayoutUnit(), cb->constrainContentBoxLogicalHeightByMinMax(contentBoxHeight - cb->scrollbarLogicalHeight(), -1));
    } else if (cbstyle.logicalHeight().hasPercent() && !isOutOfFlowPositionedWithSpecifiedHeight) {
        // We need to recur and compute the percentage height for our containing block.
        LayoutUnit heightWithScrollbar = cb->computePercentageLogicalHeight(cbstyle.logicalHeight());
        if (heightWithScrollbar != -1) {
            LayoutUnit contentBoxHeightWithScrollbar = cb->adjustContentBoxLogicalHeightForBoxSizing(heightWithScrollbar);
            // We need to adjust for min/max height because this method does not
            // handle the min/max of the current block, its caller does. So the
            // return value from the recursive call will not have been adjusted
            // yet.
            LayoutUnit contentBoxHeight = cb->constrainContentBoxLogicalHeightByMinMax(contentBoxHeightWithScrollbar - cb->scrollbarLogicalHeight(), -1);
            availableHeight = std::max(LayoutUnit(), contentBoxHeight);
        }
    } else if (isOutOfFlowPositionedWithSpecifiedHeight) {
        // Don't allow this to affect the block' size() member variable, since this
        // can get called while the block is still laying out its kids.
        LogicalExtentComputedValues computedValues;
        cb->computeLogicalHeight(cb->logicalHeight(), 0, computedValues);
        availableHeight = computedValues.m_extent - cb->borderAndPaddingLogicalHeight() - cb->scrollbarLogicalHeight();
    } else if (cb->isLayoutView()) {
        availableHeight = view()->viewLogicalHeightForPercentages();
    }

    if (availableHeight == -1)
        return availableHeight;

    availableHeight -= rootMarginBorderPaddingHeight;

    if (isTable() && isOutOfFlowPositioned())
        availableHeight += cb->paddingLogicalHeight();

    LayoutUnit result = valueForLength(height, availableHeight);
    if (includeBorderPadding) {
        // FIXME: Table cells should default to box-sizing: border-box so we can avoid this hack.
        // It is necessary to use the border-box to match WinIE's broken
        // box model. This is essential for sizing inside
        // table cells using percentage heights.
        result -= borderAndPaddingLogicalHeight();
        return std::max(LayoutUnit(), result);
    }
    return result;
}

LayoutUnit LayoutBox::computeReplacedLogicalWidth(ShouldComputePreferred shouldComputePreferred) const
{
    return computeReplacedLogicalWidthRespectingMinMaxWidth(computeReplacedLogicalWidthUsing(MainOrPreferredSize, style()->logicalWidth()), shouldComputePreferred);
}

LayoutUnit LayoutBox::computeReplacedLogicalWidthRespectingMinMaxWidth(LayoutUnit logicalWidth, ShouldComputePreferred shouldComputePreferred) const
{
    LayoutUnit minLogicalWidth = (shouldComputePreferred == ComputePreferred && style()->logicalMinWidth().hasPercent()) || style()->logicalMinWidth().isMaxSizeNone() ? logicalWidth : computeReplacedLogicalWidthUsing(MinSize, style()->logicalMinWidth());
    LayoutUnit maxLogicalWidth = (shouldComputePreferred == ComputePreferred && style()->logicalMaxWidth().hasPercent()) || style()->logicalMaxWidth().isMaxSizeNone() ? logicalWidth : computeReplacedLogicalWidthUsing(MaxSize, style()->logicalMaxWidth());
    return std::max(minLogicalWidth, std::min(logicalWidth, maxLogicalWidth));
}

LayoutUnit LayoutBox::computeReplacedLogicalWidthUsing(SizeType sizeType, const Length& logicalWidth) const
{
    ASSERT(sizeType == MinSize || sizeType == MainOrPreferredSize || !logicalWidth.isAuto());
    if (sizeType == MinSize && logicalWidth.isAuto())
        return adjustContentBoxLogicalWidthForBoxSizing(0);

    switch (logicalWidth.type()) {
    case Fixed:
        return adjustContentBoxLogicalWidthForBoxSizing(logicalWidth.value());
    case MinContent:
    case MaxContent: {
        // MinContent/MaxContent don't need the availableLogicalWidth argument.
        LayoutUnit availableLogicalWidth = 0;
        return computeIntrinsicLogicalWidthUsing(logicalWidth, availableLogicalWidth, borderAndPaddingLogicalWidth()) - borderAndPaddingLogicalWidth();
    }
    case FitContent:
    case FillAvailable:
    case Percent:
    case Calculated: {
        // FIXME: containingBlockLogicalWidthForContent() is wrong if the replaced element's writing-mode is perpendicular to the
        // containing block's writing-mode.
        // https://bugs.webkit.org/show_bug.cgi?id=46496
        const LayoutUnit cw = isOutOfFlowPositioned() ? containingBlockLogicalWidthForPositioned(toLayoutBoxModelObject(container())) : containingBlockLogicalWidthForContent();
        Length containerLogicalWidth = containingBlock()->style()->logicalWidth();
        // FIXME: Handle cases when containing block width is calculated or viewport percent.
        // https://bugs.webkit.org/show_bug.cgi?id=91071
        if (logicalWidth.isIntrinsic())
            return computeIntrinsicLogicalWidthUsing(logicalWidth, cw, borderAndPaddingLogicalWidth()) - borderAndPaddingLogicalWidth();
        if (cw > 0 || (!cw && (containerLogicalWidth.isFixed() || containerLogicalWidth.hasPercent())))
            return adjustContentBoxLogicalWidthForBoxSizing(minimumValueForLength(logicalWidth, cw));
        return LayoutUnit();
    }
    case Intrinsic:
    case MinIntrinsic:
    case Auto:
    case MaxSizeNone:
        return intrinsicLogicalWidth();
    case ExtendToZoom:
    case DeviceWidth:
    case DeviceHeight:
        break;
    }

    ASSERT_NOT_REACHED();
    return 0;
}

LayoutUnit LayoutBox::computeReplacedLogicalHeight() const
{
    return computeReplacedLogicalHeightRespectingMinMaxHeight(computeReplacedLogicalHeightUsing(MainOrPreferredSize, style()->logicalHeight()));
}

bool LayoutBox::logicalHeightComputesAsNone(SizeType sizeType) const
{
    ASSERT(sizeType == MinSize || sizeType == MaxSize);
    Length logicalHeight = sizeType == MinSize ? style()->logicalMinHeight() : style()->logicalMaxHeight();
    Length initialLogicalHeight = sizeType == MinSize ? ComputedStyle::initialMinSize() : ComputedStyle::initialMaxSize();

    if (logicalHeight == initialLogicalHeight)
        return true;

    if (LayoutBlock* cb = containingBlockForAutoHeightDetection(logicalHeight))
        return cb->hasAutoHeightOrContainingBlockWithAutoHeight();
    return false;
}

LayoutUnit LayoutBox::computeReplacedLogicalHeightRespectingMinMaxHeight(LayoutUnit logicalHeight) const
{
    // If the height of the containing block is not specified explicitly (i.e., it depends on content height), and this element is not absolutely positioned,
    // the percentage value is treated as '0' (for 'min-height') or 'none' (for 'max-height').
    LayoutUnit minLogicalHeight;
    if (!logicalHeightComputesAsNone(MinSize))
        minLogicalHeight = computeReplacedLogicalHeightUsing(MinSize, style()->logicalMinHeight());
    LayoutUnit maxLogicalHeight = logicalHeight;
    if (!logicalHeightComputesAsNone(MaxSize))
        maxLogicalHeight =  computeReplacedLogicalHeightUsing(MaxSize, style()->logicalMaxHeight());
    return std::max(minLogicalHeight, std::min(logicalHeight, maxLogicalHeight));
}

LayoutUnit LayoutBox::computeReplacedLogicalHeightUsing(SizeType sizeType, const Length& logicalHeight) const
{
    ASSERT(sizeType == MinSize || sizeType == MainOrPreferredSize || !logicalHeight.isAuto());
    if (sizeType == MinSize && logicalHeight.isAuto())
        return adjustContentBoxLogicalHeightForBoxSizing(0);

    switch (logicalHeight.type()) {
    case Fixed:
        return adjustContentBoxLogicalHeightForBoxSizing(logicalHeight.value());
    case Percent:
    case Calculated:
    {
        LayoutObject* cb = isOutOfFlowPositioned() ? container() : containingBlock();
        while (cb->isAnonymous())
            cb = cb->containingBlock();
        if (cb->isLayoutBlock())
            toLayoutBlock(cb)->addPercentHeightDescendant(const_cast<LayoutBox*>(this));

        if (cb->isOutOfFlowPositioned() && cb->style()->height().isAuto() && !(cb->style()->top().isAuto() || cb->style()->bottom().isAuto())) {
            ASSERT_WITH_SECURITY_IMPLICATION(cb->isLayoutBlock());
            LayoutBlock* block = toLayoutBlock(cb);
            LogicalExtentComputedValues computedValues;
            block->computeLogicalHeight(block->logicalHeight(), 0, computedValues);
            LayoutUnit newContentHeight = computedValues.m_extent - block->borderAndPaddingLogicalHeight() - block->scrollbarLogicalHeight();
            LayoutUnit newHeight = block->adjustContentBoxLogicalHeightForBoxSizing(newContentHeight);
            return adjustContentBoxLogicalHeightForBoxSizing(valueForLength(logicalHeight, newHeight));
        }

        // FIXME: availableLogicalHeight() is wrong if the replaced element's writing-mode is perpendicular to the
        // containing block's writing-mode.
        // https://bugs.webkit.org/show_bug.cgi?id=46496
        LayoutUnit availableHeight;
        if (isOutOfFlowPositioned()) {
            availableHeight = containingBlockLogicalHeightForPositioned(toLayoutBoxModelObject(cb));
        } else {
            availableHeight = containingBlockLogicalHeightForContent(IncludeMarginBorderPadding);
            // It is necessary to use the border-box to match WinIE's broken
            // box model.  This is essential for sizing inside
            // table cells using percentage heights.
            // FIXME: This needs to be made writing-mode-aware. If the cell and image are perpendicular writing-modes, this isn't right.
            // https://bugs.webkit.org/show_bug.cgi?id=46997
            while (cb && !cb->isLayoutView() && (cb->style()->logicalHeight().isAuto() || cb->style()->logicalHeight().hasPercent())) {
                if (cb->isTableCell()) {
                    // Don't let table cells squeeze percent-height replaced elements
                    // <http://bugs.webkit.org/show_bug.cgi?id=15359>
                    availableHeight = std::max(availableHeight, intrinsicLogicalHeight());
                    return valueForLength(logicalHeight, availableHeight - borderAndPaddingLogicalHeight());
                }
                toLayoutBlock(cb)->addPercentHeightDescendant(const_cast<LayoutBox*>(this));
                cb = cb->containingBlock();
            }
        }
        return adjustContentBoxLogicalHeightForBoxSizing(valueForLength(logicalHeight, availableHeight));
    }
    case MinContent:
    case MaxContent:
    case FitContent:
    case FillAvailable:
        return adjustContentBoxLogicalHeightForBoxSizing(computeIntrinsicLogicalContentHeightUsing(logicalHeight, intrinsicLogicalHeight(), borderAndPaddingHeight()));
    default:
        return intrinsicLogicalHeight();
    }
}

LayoutUnit LayoutBox::availableLogicalHeight(AvailableLogicalHeightType heightType) const
{
    // http://www.w3.org/TR/CSS2/visudet.html#propdef-height - We are interested in the content height.
    return constrainContentBoxLogicalHeightByMinMax(availableLogicalHeightUsing(style()->logicalHeight(), heightType), -1);
}

LayoutUnit LayoutBox::availableLogicalHeightUsing(const Length& h, AvailableLogicalHeightType heightType) const
{
    if (isLayoutView())
        return isHorizontalWritingMode() ? toLayoutView(this)->frameView()->visibleContentSize().height() : toLayoutView(this)->frameView()->visibleContentSize().width();

    // We need to stop here, since we don't want to increase the height of the table
    // artificially.  We're going to rely on this cell getting expanded to some new
    // height, and then when we lay out again we'll use the calculation below.
    if (isTableCell() && (h.isAuto() || h.hasPercent())) {
        if (hasOverrideLogicalContentHeight())
            return overrideLogicalContentHeight();
        return logicalHeight() - borderAndPaddingLogicalHeight();
    }

    if (h.hasPercent() && isOutOfFlowPositioned()) {
        // FIXME: This is wrong if the containingBlock has a perpendicular writing mode.
        LayoutUnit availableHeight = containingBlockLogicalHeightForPositioned(containingBlock());
        return adjustContentBoxLogicalHeightForBoxSizing(valueForLength(h, availableHeight));
    }

    LayoutUnit heightIncludingScrollbar = computeContentAndScrollbarLogicalHeightUsing(MainOrPreferredSize, h, -1);
    if (heightIncludingScrollbar != -1)
        return std::max(LayoutUnit(), adjustContentBoxLogicalHeightForBoxSizing(heightIncludingScrollbar) - scrollbarLogicalHeight());

    // FIXME: Check logicalTop/logicalBottom here to correctly handle vertical writing-mode.
    // https://bugs.webkit.org/show_bug.cgi?id=46500
    if (isLayoutBlock() && isOutOfFlowPositioned() && style()->height().isAuto() && !(style()->top().isAuto() || style()->bottom().isAuto())) {
        LayoutBlock* block = const_cast<LayoutBlock*>(toLayoutBlock(this));
        LogicalExtentComputedValues computedValues;
        block->computeLogicalHeight(block->logicalHeight(), 0, computedValues);
        LayoutUnit newContentHeight = computedValues.m_extent - block->borderAndPaddingLogicalHeight() - block->scrollbarLogicalHeight();
        return adjustContentBoxLogicalHeightForBoxSizing(newContentHeight);
    }

    // FIXME: This is wrong if the containingBlock has a perpendicular writing mode.
    LayoutUnit availableHeight = containingBlockLogicalHeightForContent(heightType);
    if (heightType == ExcludeMarginBorderPadding) {
        // FIXME: Margin collapsing hasn't happened yet, so this incorrectly removes collapsed margins.
        availableHeight -= marginBefore() + marginAfter() + borderAndPaddingLogicalHeight();
    }
    return availableHeight;
}

void LayoutBox::computeAndSetBlockDirectionMargins(const LayoutBlock* containingBlock)
{
    LayoutUnit marginBefore;
    LayoutUnit marginAfter;
    computeMarginsForDirection(BlockDirection, containingBlock, containingBlockLogicalWidthForContent(), logicalHeight(), marginBefore, marginAfter,
        style()->marginBeforeUsing(containingBlock->style()),
        style()->marginAfterUsing(containingBlock->style()));
    // Note that in this 'positioning phase' of the layout we are using the containing block's writing mode rather than our own when calculating margins.
    // See http://www.w3.org/TR/2014/CR-css-writing-modes-3-20140320/#orthogonal-flows
    containingBlock->setMarginBeforeForChild(*this, marginBefore);
    containingBlock->setMarginAfterForChild(*this, marginAfter);
}

LayoutUnit LayoutBox::containingBlockLogicalWidthForPositioned(const LayoutBoxModelObject* containingBlock, bool checkForPerpendicularWritingMode) const
{
    if (checkForPerpendicularWritingMode && containingBlock->isHorizontalWritingMode() != isHorizontalWritingMode())
        return containingBlockLogicalHeightForPositioned(containingBlock, false);

    // Use viewport as container for top-level fixed-position elements.
    if (style()->position() == FixedPosition && containingBlock->isLayoutView()) {
        const LayoutView* view = toLayoutView(containingBlock);
        if (FrameView* frameView = view->frameView()) {
            // Don't use visibleContentRect since the DeprecatedPaintLayer's size has not been set yet.
            IntSize viewportSize = frameView->layoutViewportScrollableArea()->excludeScrollbars(frameView->frameRect().size());
            return containingBlock->isHorizontalWritingMode() ? viewportSize.width() : viewportSize.height();
        }
    }

    if (hasOverrideContainingBlockLogicalWidth())
        return overrideContainingBlockContentLogicalWidth();

    if (containingBlock->isBox())
        return toLayoutBox(containingBlock)->clientLogicalWidth();

    ASSERT(containingBlock->isLayoutInline() && containingBlock->isRelPositioned());

    const LayoutInline* flow = toLayoutInline(containingBlock);
    InlineFlowBox* first = flow->firstLineBox();
    InlineFlowBox* last = flow->lastLineBox();

    // If the containing block is empty, return a width of 0.
    if (!first || !last)
        return LayoutUnit();

    LayoutUnit fromLeft;
    LayoutUnit fromRight;
    if (containingBlock->style()->isLeftToRightDirection()) {
        fromLeft = first->logicalLeft() + first->borderLogicalLeft();
        fromRight = last->logicalLeft() + last->logicalWidth() - last->borderLogicalRight();
    } else {
        fromRight = first->logicalLeft() + first->logicalWidth() - first->borderLogicalRight();
        fromLeft = last->logicalLeft() + last->borderLogicalLeft();
    }

    return std::max(LayoutUnit(), fromRight - fromLeft);
}

LayoutUnit LayoutBox::containingBlockLogicalHeightForPositioned(const LayoutBoxModelObject* containingBlock, bool checkForPerpendicularWritingMode) const
{
    if (checkForPerpendicularWritingMode && containingBlock->isHorizontalWritingMode() != isHorizontalWritingMode())
        return containingBlockLogicalWidthForPositioned(containingBlock, false);

    // Use viewport as container for top-level fixed-position elements.
    if (style()->position() == FixedPosition && containingBlock->isLayoutView()) {
        const LayoutView* view = toLayoutView(containingBlock);
        if (FrameView* frameView = view->frameView()) {
            // Don't use visibleContentRect since the DeprecatedPaintLayer's size has not been set yet.
            IntSize viewportSize = frameView->layoutViewportScrollableArea()->excludeScrollbars(frameView->frameRect().size());
            return containingBlock->isHorizontalWritingMode() ? viewportSize.height() : viewportSize.width();
        }
    }

    if (hasOverrideContainingBlockLogicalHeight())
        return overrideContainingBlockContentLogicalHeight();

    if (containingBlock->isBox()) {
        const LayoutBlock* cb = containingBlock->isLayoutBlock() ?
            toLayoutBlock(containingBlock) : containingBlock->containingBlock();
        return cb->clientLogicalHeight();
    }

    ASSERT(containingBlock->isLayoutInline() && containingBlock->isRelPositioned());

    const LayoutInline* flow = toLayoutInline(containingBlock);
    InlineFlowBox* first = flow->firstLineBox();
    InlineFlowBox* last = flow->lastLineBox();

    // If the containing block is empty, return a height of 0.
    if (!first || !last)
        return LayoutUnit();

    LayoutUnit heightResult;
    LayoutRect boundingBox(flow->linesBoundingBox());
    if (containingBlock->isHorizontalWritingMode())
        heightResult = boundingBox.height();
    else
        heightResult = boundingBox.width();
    heightResult -= (containingBlock->borderBefore() + containingBlock->borderAfter());
    return heightResult;
}

static void computeInlineStaticDistance(Length& logicalLeft, Length& logicalRight, const LayoutBox* child, const LayoutBoxModelObject* containerBlock, LayoutUnit containerLogicalWidth)
{
    if (!logicalLeft.isAuto() || !logicalRight.isAuto())
        return;

    // FIXME: The static distance computation has not been patched for mixed writing modes yet.
    if (child->parent()->style()->direction() == LTR) {
        LayoutUnit staticPosition = child->layer()->staticInlinePosition() - containerBlock->borderLogicalLeft();
        for (LayoutObject* curr = child->parent(); curr && curr != containerBlock; curr = curr->container()) {
            if (curr->isBox()) {
                staticPosition += toLayoutBox(curr)->logicalLeft();
                if (toLayoutBox(curr)->isRelPositioned())
                    staticPosition += toLayoutBox(curr)->relativePositionOffset().width();
            } else if (curr->isInline()) {
                if (curr->isRelPositioned()) {
                    if (!curr->style()->logicalLeft().isAuto())
                        staticPosition += valueForLength(curr->style()->logicalLeft(), curr->containingBlock()->availableWidth());
                    else
                        staticPosition -= valueForLength(curr->style()->logicalRight(), curr->containingBlock()->availableWidth());
                }
            }
        }
        logicalLeft.setValue(Fixed, staticPosition);
    } else {
        LayoutBox* enclosingBox = child->parent()->enclosingBox();
        LayoutUnit staticPosition = child->layer()->staticInlinePosition() + containerLogicalWidth + containerBlock->borderLogicalLeft();
        for (LayoutObject* curr = child->parent(); curr; curr = curr->container()) {
            if (curr->isBox()) {
                if (curr != containerBlock) {
                    staticPosition -= toLayoutBox(curr)->logicalLeft();
                    if (toLayoutBox(curr)->isRelPositioned())
                        staticPosition -= toLayoutBox(curr)->relativePositionOffset().width();
                }
                if (curr == enclosingBox)
                    staticPosition -= enclosingBox->logicalWidth();
            } else if (curr->isInline()) {
                if (curr->isRelPositioned()) {
                    if (!curr->style()->logicalLeft().isAuto())
                        staticPosition -= valueForLength(curr->style()->logicalLeft(), curr->containingBlock()->availableWidth());
                    else
                        staticPosition += valueForLength(curr->style()->logicalRight(), curr->containingBlock()->availableWidth());
                }
            }
            if (curr == containerBlock)
                break;
        }
        logicalRight.setValue(Fixed, staticPosition);
    }
}

void LayoutBox::computePositionedLogicalWidth(LogicalExtentComputedValues& computedValues) const
{
    if (isReplaced()) {
        computePositionedLogicalWidthReplaced(computedValues);
        return;
    }

    // QUESTIONS
    // FIXME 1: Should we still deal with these the cases of 'left' or 'right' having
    // the type 'static' in determining whether to calculate the static distance?
    // NOTE: 'static' is not a legal value for 'left' or 'right' as of CSS 2.1.

    // FIXME 2: Can perhaps optimize out cases when max-width/min-width are greater
    // than or less than the computed width().  Be careful of box-sizing and
    // percentage issues.

    // The following is based off of the W3C Working Draft from April 11, 2006 of
    // CSS 2.1: Section 10.3.7 "Absolutely positioned, non-replaced elements"
    // <http://www.w3.org/TR/CSS21/visudet.html#abs-non-replaced-width>
    // (block-style-comments in this function and in computePositionedLogicalWidthUsing()
    // correspond to text from the spec)


    // We don't use containingBlock(), since we may be positioned by an enclosing
    // relative positioned inline.
    const LayoutBoxModelObject* containerBlock = toLayoutBoxModelObject(container());

    const LayoutUnit containerLogicalWidth = containingBlockLogicalWidthForPositioned(containerBlock);

    // Use the container block's direction except when calculating the static distance
    // This conforms with the reference results for abspos-replaced-width-margin-000.htm
    // of the CSS 2.1 test suite
    TextDirection containerDirection = containerBlock->style()->direction();

    bool isHorizontal = isHorizontalWritingMode();
    const LayoutUnit bordersPlusPadding = borderAndPaddingLogicalWidth();
    const Length marginLogicalLeft = isHorizontal ? style()->marginLeft() : style()->marginTop();
    const Length marginLogicalRight = isHorizontal ? style()->marginRight() : style()->marginBottom();

    Length logicalLeftLength = style()->logicalLeft();
    Length logicalRightLength = style()->logicalRight();

    /*---------------------------------------------------------------------------*\
     * For the purposes of this section and the next, the term "static position"
     * (of an element) refers, roughly, to the position an element would have had
     * in the normal flow. More precisely:
     *
     * * The static position for 'left' is the distance from the left edge of the
     *   containing block to the left margin edge of a hypothetical box that would
     *   have been the first box of the element if its 'position' property had
     *   been 'static' and 'float' had been 'none'. The value is negative if the
     *   hypothetical box is to the left of the containing block.
     * * The static position for 'right' is the distance from the right edge of the
     *   containing block to the right margin edge of the same hypothetical box as
     *   above. The value is positive if the hypothetical box is to the left of the
     *   containing block's edge.
     *
     * But rather than actually calculating the dimensions of that hypothetical box,
     * user agents are free to make a guess at its probable position.
     *
     * For the purposes of calculating the static position, the containing block of
     * fixed positioned elements is the initial containing block instead of the
     * viewport, and all scrollable boxes should be assumed to be scrolled to their
     * origin.
    \*---------------------------------------------------------------------------*/

    // see FIXME 1
    // Calculate the static distance if needed.
    computeInlineStaticDistance(logicalLeftLength, logicalRightLength, this, containerBlock, containerLogicalWidth);

    // Calculate constraint equation values for 'width' case.
    computePositionedLogicalWidthUsing(MainOrPreferredSize, style()->logicalWidth(), containerBlock, containerDirection,
        containerLogicalWidth, bordersPlusPadding,
        logicalLeftLength, logicalRightLength, marginLogicalLeft, marginLogicalRight,
        computedValues);

    // Calculate constraint equation values for 'max-width' case.
    if (!style()->logicalMaxWidth().isMaxSizeNone()) {
        LogicalExtentComputedValues maxValues;

        computePositionedLogicalWidthUsing(MaxSize, style()->logicalMaxWidth(), containerBlock, containerDirection,
            containerLogicalWidth, bordersPlusPadding,
            logicalLeftLength, logicalRightLength, marginLogicalLeft, marginLogicalRight,
            maxValues);

        if (computedValues.m_extent > maxValues.m_extent) {
            computedValues.m_extent = maxValues.m_extent;
            computedValues.m_position = maxValues.m_position;
            computedValues.m_margins.m_start = maxValues.m_margins.m_start;
            computedValues.m_margins.m_end = maxValues.m_margins.m_end;
        }
    }

    // Calculate constraint equation values for 'min-width' case.
    if (!style()->logicalMinWidth().isZero() || style()->logicalMinWidth().isIntrinsic()) {
        LogicalExtentComputedValues minValues;

        computePositionedLogicalWidthUsing(MinSize, style()->logicalMinWidth(), containerBlock, containerDirection,
            containerLogicalWidth, bordersPlusPadding,
            logicalLeftLength, logicalRightLength, marginLogicalLeft, marginLogicalRight,
            minValues);

        if (computedValues.m_extent < minValues.m_extent) {
            computedValues.m_extent = minValues.m_extent;
            computedValues.m_position = minValues.m_position;
            computedValues.m_margins.m_start = minValues.m_margins.m_start;
            computedValues.m_margins.m_end = minValues.m_margins.m_end;
        }
    }

    if (!style()->hasStaticInlinePosition(isHorizontal))
        computedValues.m_position += extraInlineOffset();

    computedValues.m_extent += bordersPlusPadding;
}

static void computeLogicalLeftPositionedOffset(LayoutUnit& logicalLeftPos, const LayoutBox* child, LayoutUnit logicalWidthValue, const LayoutBoxModelObject* containerBlock, LayoutUnit containerLogicalWidth)
{
    // Deal with differing writing modes here.  Our offset needs to be in the containing block's coordinate space. If the containing block is flipped
    // along this axis, then we need to flip the coordinate.  This can only happen if the containing block is both a flipped mode and perpendicular to us.
    if (containerBlock->isHorizontalWritingMode() != child->isHorizontalWritingMode() && containerBlock->style()->isFlippedBlocksWritingMode()) {
        logicalLeftPos = containerLogicalWidth - logicalWidthValue - logicalLeftPos;
        logicalLeftPos += (child->isHorizontalWritingMode() ? containerBlock->borderRight() : containerBlock->borderBottom());
    } else {
        logicalLeftPos += (child->isHorizontalWritingMode() ? containerBlock->borderLeft() : containerBlock->borderTop());
    }
}

LayoutUnit LayoutBox::shrinkToFitLogicalWidth(LayoutUnit availableLogicalWidth, LayoutUnit bordersPlusPadding) const
{
    LayoutUnit preferredLogicalWidth = maxPreferredLogicalWidth() - bordersPlusPadding;
    LayoutUnit preferredMinLogicalWidth = minPreferredLogicalWidth() - bordersPlusPadding;
    return std::min(std::max(preferredMinLogicalWidth, availableLogicalWidth), preferredLogicalWidth);
}

void LayoutBox::computePositionedLogicalWidthUsing(SizeType widthSizeType, Length logicalWidth, const LayoutBoxModelObject* containerBlock, TextDirection containerDirection,
    LayoutUnit containerLogicalWidth, LayoutUnit bordersPlusPadding,
    const Length& logicalLeft, const Length& logicalRight, const Length& marginLogicalLeft,
    const Length& marginLogicalRight, LogicalExtentComputedValues& computedValues) const
{
    ASSERT(widthSizeType == MinSize || widthSizeType == MainOrPreferredSize || !logicalWidth.isAuto());
    if (widthSizeType == MinSize && logicalWidth.isAuto())
        logicalWidth = Length(0, Fixed);
    else if (logicalWidth.isIntrinsic())
        logicalWidth = Length(computeIntrinsicLogicalWidthUsing(logicalWidth, containerLogicalWidth, bordersPlusPadding) - bordersPlusPadding, Fixed);

    // 'left' and 'right' cannot both be 'auto' because one would of been
    // converted to the static position already
    ASSERT(!(logicalLeft.isAuto() && logicalRight.isAuto()));

    // minimumValueForLength will convert 'auto' to 0 so that it doesn't impact the available space computation below.
    LayoutUnit logicalLeftValue = minimumValueForLength(logicalLeft, containerLogicalWidth);
    LayoutUnit logicalRightValue = minimumValueForLength(logicalRight, containerLogicalWidth);

    const LayoutUnit containerRelativeLogicalWidth = containingBlockLogicalWidthForPositioned(containerBlock, false);

    bool logicalWidthIsAuto = logicalWidth.isIntrinsicOrAuto();
    bool logicalLeftIsAuto = logicalLeft.isAuto();
    bool logicalRightIsAuto = logicalRight.isAuto();
    LayoutUnit& marginLogicalLeftValue = style()->isLeftToRightDirection() ? computedValues.m_margins.m_start : computedValues.m_margins.m_end;
    LayoutUnit& marginLogicalRightValue = style()->isLeftToRightDirection() ? computedValues.m_margins.m_end : computedValues.m_margins.m_start;
    if (!logicalLeftIsAuto && !logicalWidthIsAuto && !logicalRightIsAuto) {
        /*-----------------------------------------------------------------------*\
         * If none of the three is 'auto': If both 'margin-left' and 'margin-
         * right' are 'auto', solve the equation under the extra constraint that
         * the two margins get equal values, unless this would make them negative,
         * in which case when direction of the containing block is 'ltr' ('rtl'),
         * set 'margin-left' ('margin-right') to zero and solve for 'margin-right'
         * ('margin-left'). If one of 'margin-left' or 'margin-right' is 'auto',
         * solve the equation for that value. If the values are over-constrained,
         * ignore the value for 'left' (in case the 'direction' property of the
         * containing block is 'rtl') or 'right' (in case 'direction' is 'ltr')
         * and solve for that value.
        \*-----------------------------------------------------------------------*/
        // NOTE:  It is not necessary to solve for 'right' in the over constrained
        // case because the value is not used for any further calculations.

        computedValues.m_extent = adjustContentBoxLogicalWidthForBoxSizing(valueForLength(logicalWidth, containerLogicalWidth));

        const LayoutUnit availableSpace = containerLogicalWidth - (logicalLeftValue + computedValues.m_extent + logicalRightValue + bordersPlusPadding);

        // Margins are now the only unknown
        if (marginLogicalLeft.isAuto() && marginLogicalRight.isAuto()) {
            // Both margins auto, solve for equality
            if (availableSpace >= 0) {
                marginLogicalLeftValue = availableSpace / 2; // split the difference
                marginLogicalRightValue = availableSpace - marginLogicalLeftValue; // account for odd valued differences
            } else {
                // Use the containing block's direction rather than the parent block's
                // per CSS 2.1 reference test abspos-non-replaced-width-margin-000.
                if (containerDirection == LTR) {
                    marginLogicalLeftValue = 0;
                    marginLogicalRightValue = availableSpace; // will be negative
                } else {
                    marginLogicalLeftValue = availableSpace; // will be negative
                    marginLogicalRightValue = 0;
                }
            }
        } else if (marginLogicalLeft.isAuto()) {
            // Solve for left margin
            marginLogicalRightValue = valueForLength(marginLogicalRight, containerRelativeLogicalWidth);
            marginLogicalLeftValue = availableSpace - marginLogicalRightValue;
        } else if (marginLogicalRight.isAuto()) {
            // Solve for right margin
            marginLogicalLeftValue = valueForLength(marginLogicalLeft, containerRelativeLogicalWidth);
            marginLogicalRightValue = availableSpace - marginLogicalLeftValue;
        } else {
            // Over-constrained, solve for left if direction is RTL
            marginLogicalLeftValue = valueForLength(marginLogicalLeft, containerRelativeLogicalWidth);
            marginLogicalRightValue = valueForLength(marginLogicalRight, containerRelativeLogicalWidth);

            // Use the containing block's direction rather than the parent block's
            // per CSS 2.1 reference test abspos-non-replaced-width-margin-000.
            if (containerDirection == RTL)
                logicalLeftValue = (availableSpace + logicalLeftValue) - marginLogicalLeftValue - marginLogicalRightValue;
        }
    } else {
        /*--------------------------------------------------------------------*\
         * Otherwise, set 'auto' values for 'margin-left' and 'margin-right'
         * to 0, and pick the one of the following six rules that applies.
         *
         * 1. 'left' and 'width' are 'auto' and 'right' is not 'auto', then the
         *    width is shrink-to-fit. Then solve for 'left'
         *
         *              OMIT RULE 2 AS IT SHOULD NEVER BE HIT
         * ------------------------------------------------------------------
         * 2. 'left' and 'right' are 'auto' and 'width' is not 'auto', then if
         *    the 'direction' property of the containing block is 'ltr' set
         *    'left' to the static position, otherwise set 'right' to the
         *    static position. Then solve for 'left' (if 'direction is 'rtl')
         *    or 'right' (if 'direction' is 'ltr').
         * ------------------------------------------------------------------
         *
         * 3. 'width' and 'right' are 'auto' and 'left' is not 'auto', then the
         *    width is shrink-to-fit . Then solve for 'right'
         * 4. 'left' is 'auto', 'width' and 'right' are not 'auto', then solve
         *    for 'left'
         * 5. 'width' is 'auto', 'left' and 'right' are not 'auto', then solve
         *    for 'width'
         * 6. 'right' is 'auto', 'left' and 'width' are not 'auto', then solve
         *    for 'right'
         *
         * Calculation of the shrink-to-fit width is similar to calculating the
         * width of a table cell using the automatic table layout algorithm.
         * Roughly: calculate the preferred width by formatting the content
         * without breaking lines other than where explicit line breaks occur,
         * and also calculate the preferred minimum width, e.g., by trying all
         * possible line breaks. CSS 2.1 does not define the exact algorithm.
         * Thirdly, calculate the available width: this is found by solving
         * for 'width' after setting 'left' (in case 1) or 'right' (in case 3)
         * to 0.
         *
         * Then the shrink-to-fit width is:
         * min(max(preferred minimum width, available width), preferred width).
        \*--------------------------------------------------------------------*/
        // NOTE: For rules 3 and 6 it is not necessary to solve for 'right'
        // because the value is not used for any further calculations.

        // Calculate margins, 'auto' margins are ignored.
        marginLogicalLeftValue = minimumValueForLength(marginLogicalLeft, containerRelativeLogicalWidth);
        marginLogicalRightValue = minimumValueForLength(marginLogicalRight, containerRelativeLogicalWidth);

        const LayoutUnit availableSpace = containerLogicalWidth - (marginLogicalLeftValue + marginLogicalRightValue + logicalLeftValue + logicalRightValue + bordersPlusPadding);

        // FIXME: Is there a faster way to find the correct case?
        // Use rule/case that applies.
        if (logicalLeftIsAuto && logicalWidthIsAuto && !logicalRightIsAuto) {
            // RULE 1: (use shrink-to-fit for width, and solve of left)
            computedValues.m_extent = shrinkToFitLogicalWidth(availableSpace, bordersPlusPadding);
            logicalLeftValue = availableSpace - computedValues.m_extent;
        } else if (!logicalLeftIsAuto && logicalWidthIsAuto && logicalRightIsAuto) {
            // RULE 3: (use shrink-to-fit for width, and no need solve of right)
            computedValues.m_extent = shrinkToFitLogicalWidth(availableSpace, bordersPlusPadding);
        } else if (logicalLeftIsAuto && !logicalWidthIsAuto && !logicalRightIsAuto) {
            // RULE 4: (solve for left)
            computedValues.m_extent = adjustContentBoxLogicalWidthForBoxSizing(valueForLength(logicalWidth, containerLogicalWidth));
            logicalLeftValue = availableSpace - computedValues.m_extent;
        } else if (!logicalLeftIsAuto && logicalWidthIsAuto && !logicalRightIsAuto) {
            // RULE 5: (solve for width)
            if (autoWidthShouldFitContent())
                computedValues.m_extent = shrinkToFitLogicalWidth(availableSpace, bordersPlusPadding);
            else
                computedValues.m_extent = std::max(LayoutUnit(), availableSpace);
        } else if (!logicalLeftIsAuto && !logicalWidthIsAuto && logicalRightIsAuto) {
            // RULE 6: (no need solve for right)
            computedValues.m_extent = adjustContentBoxLogicalWidthForBoxSizing(valueForLength(logicalWidth, containerLogicalWidth));
        }
    }

    // Use computed values to calculate the horizontal position.

    // FIXME: This hack is needed to calculate the  logical left position for a 'rtl' relatively
    // positioned, inline because right now, it is using the logical left position
    // of the first line box when really it should use the last line box.  When
    // this is fixed elsewhere, this block should be removed.
    if (containerBlock->isLayoutInline() && !containerBlock->style()->isLeftToRightDirection()) {
        const LayoutInline* flow = toLayoutInline(containerBlock);
        InlineFlowBox* firstLine = flow->firstLineBox();
        InlineFlowBox* lastLine = flow->lastLineBox();
        if (firstLine && lastLine && firstLine != lastLine) {
            computedValues.m_position = logicalLeftValue + marginLogicalLeftValue + lastLine->borderLogicalLeft() + (lastLine->logicalLeft() - firstLine->logicalLeft());
            return;
        }
    }

    if (containerBlock->isBox() && toLayoutBox(containerBlock)->scrollsOverflowY() && containerBlock->style()->shouldPlaceBlockDirectionScrollbarOnLogicalLeft()) {
        logicalLeftValue = logicalLeftValue + toLayoutBox(containerBlock)->verticalScrollbarWidth();
    }

    computedValues.m_position = logicalLeftValue + marginLogicalLeftValue;
    computeLogicalLeftPositionedOffset(computedValues.m_position, this, computedValues.m_extent, containerBlock, containerLogicalWidth);
}

static void computeBlockStaticDistance(Length& logicalTop, Length& logicalBottom, const LayoutBox* child, const LayoutBoxModelObject* containerBlock)
{
    if (!logicalTop.isAuto() || !logicalBottom.isAuto())
        return;

    // FIXME: The static distance computation has not been patched for mixed writing modes.
    LayoutUnit staticLogicalTop = child->layer()->staticBlockPosition();
    for (LayoutObject* curr = child->parent(); curr && curr != containerBlock; curr = curr->container()) {
        if (!curr->isBox() || curr->isTableRow())
            continue;
        const LayoutBox& box = *toLayoutBox(curr);
        staticLogicalTop += box.logicalTop();
        if (box.isRelPositioned()) // isInFlowPositioned
            staticLogicalTop += box.offsetForInFlowPosition().height();
        if (!box.isLayoutFlowThread())
            continue;
        // We're walking out of a flowthread here. This flow thread is not in the
        // containing block chain, so we need to convert the position from the
        // coordinate space of this flowthread to the containing coordinate space.
        // The inline position cannot affect the block position, so we don't bother
        // calculating it.
        LayoutUnit dummyInlinePosition;
        toLayoutFlowThread(box).flowThreadToContainingCoordinateSpace(staticLogicalTop, dummyInlinePosition);
    }
    logicalTop.setValue(Fixed, staticLogicalTop - containerBlock->borderBefore());
}

void LayoutBox::computePositionedLogicalHeight(LogicalExtentComputedValues& computedValues) const
{
    if (isReplaced()) {
        computePositionedLogicalHeightReplaced(computedValues);
        return;
    }

    // The following is based off of the W3C Working Draft from April 11, 2006 of
    // CSS 2.1: Section 10.6.4 "Absolutely positioned, non-replaced elements"
    // <http://www.w3.org/TR/2005/WD-CSS21-20050613/visudet.html#abs-non-replaced-height>
    // (block-style-comments in this function and in computePositionedLogicalHeightUsing()
    // correspond to text from the spec)


    // We don't use containingBlock(), since we may be positioned by an enclosing relpositioned inline.
    const LayoutBoxModelObject* containerBlock = toLayoutBoxModelObject(container());

    const LayoutUnit containerLogicalHeight = containingBlockLogicalHeightForPositioned(containerBlock);

    const ComputedStyle& styleToUse = styleRef();
    const LayoutUnit bordersPlusPadding = borderAndPaddingLogicalHeight();
    const Length marginBefore = styleToUse.marginBefore();
    const Length marginAfter = styleToUse.marginAfter();
    Length logicalTopLength = styleToUse.logicalTop();
    Length logicalBottomLength = styleToUse.logicalBottom();

    /*---------------------------------------------------------------------------*\
     * For the purposes of this section and the next, the term "static position"
     * (of an element) refers, roughly, to the position an element would have had
     * in the normal flow. More precisely, the static position for 'top' is the
     * distance from the top edge of the containing block to the top margin edge
     * of a hypothetical box that would have been the first box of the element if
     * its 'position' property had been 'static' and 'float' had been 'none'. The
     * value is negative if the hypothetical box is above the containing block.
     *
     * But rather than actually calculating the dimensions of that hypothetical
     * box, user agents are free to make a guess at its probable position.
     *
     * For the purposes of calculating the static position, the containing block
     * of fixed positioned elements is the initial containing block instead of
     * the viewport.
    \*---------------------------------------------------------------------------*/

    // see FIXME 1
    // Calculate the static distance if needed.
    computeBlockStaticDistance(logicalTopLength, logicalBottomLength, this, containerBlock);

    // Calculate constraint equation values for 'height' case.
    LayoutUnit logicalHeight = computedValues.m_extent;
    computePositionedLogicalHeightUsing(MainOrPreferredSize, styleToUse.logicalHeight(), containerBlock, containerLogicalHeight, bordersPlusPadding, logicalHeight,
        logicalTopLength, logicalBottomLength, marginBefore, marginAfter,
        computedValues);

    // Avoid doing any work in the common case (where the values of min-height and max-height are their defaults).
    // see FIXME 2

    // Calculate constraint equation values for 'max-height' case.
    if (!styleToUse.logicalMaxHeight().isMaxSizeNone()) {
        LogicalExtentComputedValues maxValues;

        computePositionedLogicalHeightUsing(MaxSize, styleToUse.logicalMaxHeight(), containerBlock, containerLogicalHeight, bordersPlusPadding, logicalHeight,
            logicalTopLength, logicalBottomLength, marginBefore, marginAfter,
            maxValues);

        if (computedValues.m_extent > maxValues.m_extent) {
            computedValues.m_extent = maxValues.m_extent;
            computedValues.m_position = maxValues.m_position;
            computedValues.m_margins.m_before = maxValues.m_margins.m_before;
            computedValues.m_margins.m_after = maxValues.m_margins.m_after;
        }
    }

    // Calculate constraint equation values for 'min-height' case.
    if (!styleToUse.logicalMinHeight().isZero() || styleToUse.logicalMinHeight().isIntrinsic()) {
        LogicalExtentComputedValues minValues;

        computePositionedLogicalHeightUsing(MinSize, styleToUse.logicalMinHeight(), containerBlock, containerLogicalHeight, bordersPlusPadding, logicalHeight,
            logicalTopLength, logicalBottomLength, marginBefore, marginAfter,
            minValues);

        if (computedValues.m_extent < minValues.m_extent) {
            computedValues.m_extent = minValues.m_extent;
            computedValues.m_position = minValues.m_position;
            computedValues.m_margins.m_before = minValues.m_margins.m_before;
            computedValues.m_margins.m_after = minValues.m_margins.m_after;
        }
    }

    if (!style()->hasStaticBlockPosition(isHorizontalWritingMode()))
        computedValues.m_position += extraBlockOffset();

    // Set final height value.
    computedValues.m_extent += bordersPlusPadding;
}

static void computeLogicalTopPositionedOffset(LayoutUnit& logicalTopPos, const LayoutBox* child, LayoutUnit logicalHeightValue, const LayoutBoxModelObject* containerBlock, LayoutUnit containerLogicalHeight)
{
    // Deal with differing writing modes here.  Our offset needs to be in the containing block's coordinate space. If the containing block is flipped
    // along this axis, then we need to flip the coordinate.  This can only happen if the containing block is both a flipped mode and perpendicular to us.
    if ((child->style()->isFlippedBlocksWritingMode() && child->isHorizontalWritingMode() != containerBlock->isHorizontalWritingMode())
        || (child->style()->isFlippedBlocksWritingMode() != containerBlock->style()->isFlippedBlocksWritingMode() && child->isHorizontalWritingMode() == containerBlock->isHorizontalWritingMode()))
        logicalTopPos = containerLogicalHeight - logicalHeightValue - logicalTopPos;

    // Our offset is from the logical bottom edge in a flipped environment, e.g., right for vertical-rl and bottom for horizontal-bt.
    if (containerBlock->style()->isFlippedBlocksWritingMode() && child->isHorizontalWritingMode() == containerBlock->isHorizontalWritingMode()) {
        if (child->isHorizontalWritingMode())
            logicalTopPos += containerBlock->borderBottom();
        else
            logicalTopPos += containerBlock->borderRight();
    } else {
        if (child->isHorizontalWritingMode())
            logicalTopPos += containerBlock->borderTop();
        else
            logicalTopPos += containerBlock->borderLeft();
    }
}

void LayoutBox::computePositionedLogicalHeightUsing(SizeType heightSizeType, Length logicalHeightLength, const LayoutBoxModelObject* containerBlock,
    LayoutUnit containerLogicalHeight, LayoutUnit bordersPlusPadding, LayoutUnit logicalHeight,
    const Length& logicalTop, const Length& logicalBottom, const Length& marginBefore,
    const Length& marginAfter, LogicalExtentComputedValues& computedValues) const
{
    ASSERT(heightSizeType == MinSize || heightSizeType == MainOrPreferredSize || !logicalHeightLength.isAuto());
    if (heightSizeType == MinSize && logicalHeightLength.isAuto())
        logicalHeightLength = Length(0, Fixed);

    // 'top' and 'bottom' cannot both be 'auto' because 'top would of been
    // converted to the static position in computePositionedLogicalHeight()
    ASSERT(!(logicalTop.isAuto() && logicalBottom.isAuto()));

    LayoutUnit logicalHeightValue;
    LayoutUnit contentLogicalHeight = logicalHeight - bordersPlusPadding;

    const LayoutUnit containerRelativeLogicalWidth = containingBlockLogicalWidthForPositioned(containerBlock, false);

    LayoutUnit logicalTopValue = 0;

    bool logicalHeightIsAuto = logicalHeightLength.isAuto();
    bool logicalTopIsAuto = logicalTop.isAuto();
    bool logicalBottomIsAuto = logicalBottom.isAuto();

    LayoutUnit resolvedLogicalHeight;
    // Height is never unsolved for tables.
    if (isTable()) {
        resolvedLogicalHeight = contentLogicalHeight;
        logicalHeightIsAuto = false;
    } else {
        if (logicalHeightLength.isIntrinsic())
            resolvedLogicalHeight = computeIntrinsicLogicalContentHeightUsing(logicalHeightLength, contentLogicalHeight, bordersPlusPadding);
        else
            resolvedLogicalHeight = adjustContentBoxLogicalHeightForBoxSizing(valueForLength(logicalHeightLength, containerLogicalHeight));
    }

    if (!logicalTopIsAuto && !logicalHeightIsAuto && !logicalBottomIsAuto) {
        /*-----------------------------------------------------------------------*\
         * If none of the three are 'auto': If both 'margin-top' and 'margin-
         * bottom' are 'auto', solve the equation under the extra constraint that
         * the two margins get equal values. If one of 'margin-top' or 'margin-
         * bottom' is 'auto', solve the equation for that value. If the values
         * are over-constrained, ignore the value for 'bottom' and solve for that
         * value.
        \*-----------------------------------------------------------------------*/
        // NOTE:  It is not necessary to solve for 'bottom' in the over constrained
        // case because the value is not used for any further calculations.

        logicalHeightValue = resolvedLogicalHeight;
        logicalTopValue = valueForLength(logicalTop, containerLogicalHeight);

        const LayoutUnit availableSpace = containerLogicalHeight - (logicalTopValue + logicalHeightValue + valueForLength(logicalBottom, containerLogicalHeight) + bordersPlusPadding);

        // Margins are now the only unknown
        if (marginBefore.isAuto() && marginAfter.isAuto()) {
            // Both margins auto, solve for equality
            // NOTE: This may result in negative values.
            computedValues.m_margins.m_before = availableSpace / 2; // split the difference
            computedValues.m_margins.m_after = availableSpace - computedValues.m_margins.m_before; // account for odd valued differences
        } else if (marginBefore.isAuto()) {
            // Solve for top margin
            computedValues.m_margins.m_after = valueForLength(marginAfter, containerRelativeLogicalWidth);
            computedValues.m_margins.m_before = availableSpace - computedValues.m_margins.m_after;
        } else if (marginAfter.isAuto()) {
            // Solve for bottom margin
            computedValues.m_margins.m_before = valueForLength(marginBefore, containerRelativeLogicalWidth);
            computedValues.m_margins.m_after = availableSpace - computedValues.m_margins.m_before;
        } else {
            // Over-constrained, (no need solve for bottom)
            computedValues.m_margins.m_before = valueForLength(marginBefore, containerRelativeLogicalWidth);
            computedValues.m_margins.m_after = valueForLength(marginAfter, containerRelativeLogicalWidth);
        }
    } else {
        /*--------------------------------------------------------------------*\
         * Otherwise, set 'auto' values for 'margin-top' and 'margin-bottom'
         * to 0, and pick the one of the following six rules that applies.
         *
         * 1. 'top' and 'height' are 'auto' and 'bottom' is not 'auto', then
         *    the height is based on the content, and solve for 'top'.
         *
         *              OMIT RULE 2 AS IT SHOULD NEVER BE HIT
         * ------------------------------------------------------------------
         * 2. 'top' and 'bottom' are 'auto' and 'height' is not 'auto', then
         *    set 'top' to the static position, and solve for 'bottom'.
         * ------------------------------------------------------------------
         *
         * 3. 'height' and 'bottom' are 'auto' and 'top' is not 'auto', then
         *    the height is based on the content, and solve for 'bottom'.
         * 4. 'top' is 'auto', 'height' and 'bottom' are not 'auto', and
         *    solve for 'top'.
         * 5. 'height' is 'auto', 'top' and 'bottom' are not 'auto', and
         *    solve for 'height'.
         * 6. 'bottom' is 'auto', 'top' and 'height' are not 'auto', and
         *    solve for 'bottom'.
        \*--------------------------------------------------------------------*/
        // NOTE: For rules 3 and 6 it is not necessary to solve for 'bottom'
        // because the value is not used for any further calculations.

        // Calculate margins, 'auto' margins are ignored.
        computedValues.m_margins.m_before = minimumValueForLength(marginBefore, containerRelativeLogicalWidth);
        computedValues.m_margins.m_after = minimumValueForLength(marginAfter, containerRelativeLogicalWidth);

        const LayoutUnit availableSpace = containerLogicalHeight - (computedValues.m_margins.m_before + computedValues.m_margins.m_after + bordersPlusPadding);

        // Use rule/case that applies.
        if (logicalTopIsAuto && logicalHeightIsAuto && !logicalBottomIsAuto) {
            // RULE 1: (height is content based, solve of top)
            logicalHeightValue = contentLogicalHeight;
            logicalTopValue = availableSpace - (logicalHeightValue + valueForLength(logicalBottom, containerLogicalHeight));
        } else if (!logicalTopIsAuto && logicalHeightIsAuto && logicalBottomIsAuto) {
            // RULE 3: (height is content based, no need solve of bottom)
            logicalTopValue = valueForLength(logicalTop, containerLogicalHeight);
            logicalHeightValue = contentLogicalHeight;
        } else if (logicalTopIsAuto && !logicalHeightIsAuto && !logicalBottomIsAuto) {
            // RULE 4: (solve of top)
            logicalHeightValue = resolvedLogicalHeight;
            logicalTopValue = availableSpace - (logicalHeightValue + valueForLength(logicalBottom, containerLogicalHeight));
        } else if (!logicalTopIsAuto && logicalHeightIsAuto && !logicalBottomIsAuto) {
            // RULE 5: (solve of height)
            logicalTopValue = valueForLength(logicalTop, containerLogicalHeight);
            logicalHeightValue = std::max(LayoutUnit(), availableSpace - (logicalTopValue + valueForLength(logicalBottom, containerLogicalHeight)));
        } else if (!logicalTopIsAuto && !logicalHeightIsAuto && logicalBottomIsAuto) {
            // RULE 6: (no need solve of bottom)
            logicalHeightValue = resolvedLogicalHeight;
            logicalTopValue = valueForLength(logicalTop, containerLogicalHeight);
        }
    }
    computedValues.m_extent = logicalHeightValue;

    // Use computed values to calculate the vertical position.
    computedValues.m_position = logicalTopValue + computedValues.m_margins.m_before;
    computeLogicalTopPositionedOffset(computedValues.m_position, this, logicalHeightValue, containerBlock, containerLogicalHeight);
}

void LayoutBox::computePositionedLogicalWidthReplaced(LogicalExtentComputedValues& computedValues) const
{
    // The following is based off of the W3C Working Draft from April 11, 2006 of
    // CSS 2.1: Section 10.3.8 "Absolutely positioned, replaced elements"
    // <http://www.w3.org/TR/2005/WD-CSS21-20050613/visudet.html#abs-replaced-width>
    // (block-style-comments in this function correspond to text from the spec and
    // the numbers correspond to numbers in spec)

    // We don't use containingBlock(), since we may be positioned by an enclosing
    // relative positioned inline.
    const LayoutBoxModelObject* containerBlock = toLayoutBoxModelObject(container());

    const LayoutUnit containerLogicalWidth = containingBlockLogicalWidthForPositioned(containerBlock);
    const LayoutUnit containerRelativeLogicalWidth = containingBlockLogicalWidthForPositioned(containerBlock, false);

    // To match WinIE, in quirks mode use the parent's 'direction' property
    // instead of the the container block's.
    TextDirection containerDirection = containerBlock->style()->direction();

    // Variables to solve.
    bool isHorizontal = isHorizontalWritingMode();
    Length logicalLeft = style()->logicalLeft();
    Length logicalRight = style()->logicalRight();
    Length marginLogicalLeft = isHorizontal ? style()->marginLeft() : style()->marginTop();
    Length marginLogicalRight = isHorizontal ? style()->marginRight() : style()->marginBottom();
    LayoutUnit& marginLogicalLeftAlias = style()->isLeftToRightDirection() ? computedValues.m_margins.m_start : computedValues.m_margins.m_end;
    LayoutUnit& marginLogicalRightAlias = style()->isLeftToRightDirection() ? computedValues.m_margins.m_end : computedValues.m_margins.m_start;

    /*-----------------------------------------------------------------------*\
     * 1. The used value of 'width' is determined as for inline replaced
     *    elements.
    \*-----------------------------------------------------------------------*/
    // NOTE: This value of width is final in that the min/max width calculations
    // are dealt with in computeReplacedWidth().  This means that the steps to produce
    // correct max/min in the non-replaced version, are not necessary.
    computedValues.m_extent = computeReplacedLogicalWidth() + borderAndPaddingLogicalWidth();

    const LayoutUnit availableSpace = containerLogicalWidth - computedValues.m_extent;

    /*-----------------------------------------------------------------------*\
     * 2. If both 'left' and 'right' have the value 'auto', then if 'direction'
     *    of the containing block is 'ltr', set 'left' to the static position;
     *    else if 'direction' is 'rtl', set 'right' to the static position.
    \*-----------------------------------------------------------------------*/
    // see FIXME 1
    computeInlineStaticDistance(logicalLeft, logicalRight, this, containerBlock, containerLogicalWidth);

    /*-----------------------------------------------------------------------*\
     * 3. If 'left' or 'right' are 'auto', replace any 'auto' on 'margin-left'
     *    or 'margin-right' with '0'.
    \*-----------------------------------------------------------------------*/
    if (logicalLeft.isAuto() || logicalRight.isAuto()) {
        if (marginLogicalLeft.isAuto())
            marginLogicalLeft.setValue(Fixed, 0);
        if (marginLogicalRight.isAuto())
            marginLogicalRight.setValue(Fixed, 0);
    }

    /*-----------------------------------------------------------------------*\
     * 4. If at this point both 'margin-left' and 'margin-right' are still
     *    'auto', solve the equation under the extra constraint that the two
     *    margins must get equal values, unless this would make them negative,
     *    in which case when the direction of the containing block is 'ltr'
     *    ('rtl'), set 'margin-left' ('margin-right') to zero and solve for
     *    'margin-right' ('margin-left').
    \*-----------------------------------------------------------------------*/
    LayoutUnit logicalLeftValue = 0;
    LayoutUnit logicalRightValue = 0;

    if (marginLogicalLeft.isAuto() && marginLogicalRight.isAuto()) {
        // 'left' and 'right' cannot be 'auto' due to step 3
        ASSERT(!(logicalLeft.isAuto() && logicalRight.isAuto()));

        logicalLeftValue = valueForLength(logicalLeft, containerLogicalWidth);
        logicalRightValue = valueForLength(logicalRight, containerLogicalWidth);

        LayoutUnit difference = availableSpace - (logicalLeftValue + logicalRightValue);
        if (difference > 0) {
            marginLogicalLeftAlias = difference / 2; // split the difference
            marginLogicalRightAlias = difference - marginLogicalLeftAlias; // account for odd valued differences
        } else {
            // Use the containing block's direction rather than the parent block's
            // per CSS 2.1 reference test abspos-replaced-width-margin-000.
            if (containerDirection == LTR) {
                marginLogicalLeftAlias = 0;
                marginLogicalRightAlias = difference; // will be negative
            } else {
                marginLogicalLeftAlias = difference; // will be negative
                marginLogicalRightAlias = 0;
            }
        }

    /*-----------------------------------------------------------------------*\
     * 5. If at this point there is an 'auto' left, solve the equation for
     *    that value.
    \*-----------------------------------------------------------------------*/
    } else if (logicalLeft.isAuto()) {
        marginLogicalLeftAlias = valueForLength(marginLogicalLeft, containerRelativeLogicalWidth);
        marginLogicalRightAlias = valueForLength(marginLogicalRight, containerRelativeLogicalWidth);
        logicalRightValue = valueForLength(logicalRight, containerLogicalWidth);

        // Solve for 'left'
        logicalLeftValue = availableSpace - (logicalRightValue + marginLogicalLeftAlias + marginLogicalRightAlias);
    } else if (logicalRight.isAuto()) {
        marginLogicalLeftAlias = valueForLength(marginLogicalLeft, containerRelativeLogicalWidth);
        marginLogicalRightAlias = valueForLength(marginLogicalRight, containerRelativeLogicalWidth);
        logicalLeftValue = valueForLength(logicalLeft, containerLogicalWidth);

        // Solve for 'right'
        logicalRightValue = availableSpace - (logicalLeftValue + marginLogicalLeftAlias + marginLogicalRightAlias);
    } else if (marginLogicalLeft.isAuto()) {
        marginLogicalRightAlias = valueForLength(marginLogicalRight, containerRelativeLogicalWidth);
        logicalLeftValue = valueForLength(logicalLeft, containerLogicalWidth);
        logicalRightValue = valueForLength(logicalRight, containerLogicalWidth);

        // Solve for 'margin-left'
        marginLogicalLeftAlias = availableSpace - (logicalLeftValue + logicalRightValue + marginLogicalRightAlias);
    } else if (marginLogicalRight.isAuto()) {
        marginLogicalLeftAlias = valueForLength(marginLogicalLeft, containerRelativeLogicalWidth);
        logicalLeftValue = valueForLength(logicalLeft, containerLogicalWidth);
        logicalRightValue = valueForLength(logicalRight, containerLogicalWidth);

        // Solve for 'margin-right'
        marginLogicalRightAlias = availableSpace - (logicalLeftValue + logicalRightValue + marginLogicalLeftAlias);
    } else {
        // Nothing is 'auto', just calculate the values.
        marginLogicalLeftAlias = valueForLength(marginLogicalLeft, containerRelativeLogicalWidth);
        marginLogicalRightAlias = valueForLength(marginLogicalRight, containerRelativeLogicalWidth);
        logicalRightValue = valueForLength(logicalRight, containerLogicalWidth);
        logicalLeftValue = valueForLength(logicalLeft, containerLogicalWidth);
        // If the containing block is right-to-left, then push the left position as far to the right as possible
        if (containerDirection == RTL) {
            int totalLogicalWidth = computedValues.m_extent + logicalLeftValue + logicalRightValue +  marginLogicalLeftAlias + marginLogicalRightAlias;
            logicalLeftValue = containerLogicalWidth - (totalLogicalWidth - logicalLeftValue);
        }
    }

    /*-----------------------------------------------------------------------*\
     * 6. If at this point the values are over-constrained, ignore the value
     *    for either 'left' (in case the 'direction' property of the
     *    containing block is 'rtl') or 'right' (in case 'direction' is
     *    'ltr') and solve for that value.
    \*-----------------------------------------------------------------------*/
    // NOTE: Constraints imposed by the width of the containing block and its content have already been accounted for above.

    // FIXME: Deal with differing writing modes here.  Our offset needs to be in the containing block's coordinate space, so that
    // can make the result here rather complicated to compute.

    // Use computed values to calculate the horizontal position.

    // FIXME: This hack is needed to calculate the logical left position for a 'rtl' relatively
    // positioned, inline containing block because right now, it is using the logical left position
    // of the first line box when really it should use the last line box.  When
    // this is fixed elsewhere, this block should be removed.
    if (containerBlock->isLayoutInline() && !containerBlock->style()->isLeftToRightDirection()) {
        const LayoutInline* flow = toLayoutInline(containerBlock);
        InlineFlowBox* firstLine = flow->firstLineBox();
        InlineFlowBox* lastLine = flow->lastLineBox();
        if (firstLine && lastLine && firstLine != lastLine) {
            computedValues.m_position = logicalLeftValue + marginLogicalLeftAlias + lastLine->borderLogicalLeft() + (lastLine->logicalLeft() - firstLine->logicalLeft());
            return;
        }
    }

    LayoutUnit logicalLeftPos = logicalLeftValue + marginLogicalLeftAlias;
    computeLogicalLeftPositionedOffset(logicalLeftPos, this, computedValues.m_extent, containerBlock, containerLogicalWidth);
    computedValues.m_position = logicalLeftPos;
}

void LayoutBox::computePositionedLogicalHeightReplaced(LogicalExtentComputedValues& computedValues) const
{
    // The following is based off of the W3C Working Draft from April 11, 2006 of
    // CSS 2.1: Section 10.6.5 "Absolutely positioned, replaced elements"
    // <http://www.w3.org/TR/2005/WD-CSS21-20050613/visudet.html#abs-replaced-height>
    // (block-style-comments in this function correspond to text from the spec and
    // the numbers correspond to numbers in spec)

    // We don't use containingBlock(), since we may be positioned by an enclosing relpositioned inline.
    const LayoutBoxModelObject* containerBlock = toLayoutBoxModelObject(container());

    const LayoutUnit containerLogicalHeight = containingBlockLogicalHeightForPositioned(containerBlock);
    const LayoutUnit containerRelativeLogicalWidth = containingBlockLogicalWidthForPositioned(containerBlock, false);

    // Variables to solve.
    Length marginBefore = style()->marginBefore();
    Length marginAfter = style()->marginAfter();
    LayoutUnit& marginBeforeAlias = computedValues.m_margins.m_before;
    LayoutUnit& marginAfterAlias = computedValues.m_margins.m_after;

    Length logicalTop = style()->logicalTop();
    Length logicalBottom = style()->logicalBottom();

    /*-----------------------------------------------------------------------*\
     * 1. The used value of 'height' is determined as for inline replaced
     *    elements.
    \*-----------------------------------------------------------------------*/
    // NOTE: This value of height is final in that the min/max height calculations
    // are dealt with in computeReplacedHeight().  This means that the steps to produce
    // correct max/min in the non-replaced version, are not necessary.
    computedValues.m_extent = computeReplacedLogicalHeight() + borderAndPaddingLogicalHeight();
    const LayoutUnit availableSpace = containerLogicalHeight - computedValues.m_extent;

    /*-----------------------------------------------------------------------*\
     * 2. If both 'top' and 'bottom' have the value 'auto', replace 'top'
     *    with the element's static position.
    \*-----------------------------------------------------------------------*/
    // see FIXME 1
    computeBlockStaticDistance(logicalTop, logicalBottom, this, containerBlock);

    /*-----------------------------------------------------------------------*\
     * 3. If 'bottom' is 'auto', replace any 'auto' on 'margin-top' or
     *    'margin-bottom' with '0'.
    \*-----------------------------------------------------------------------*/
    // FIXME: The spec. says that this step should only be taken when bottom is
    // auto, but if only top is auto, this makes step 4 impossible.
    if (logicalTop.isAuto() || logicalBottom.isAuto()) {
        if (marginBefore.isAuto())
            marginBefore.setValue(Fixed, 0);
        if (marginAfter.isAuto())
            marginAfter.setValue(Fixed, 0);
    }

    /*-----------------------------------------------------------------------*\
     * 4. If at this point both 'margin-top' and 'margin-bottom' are still
     *    'auto', solve the equation under the extra constraint that the two
     *    margins must get equal values.
    \*-----------------------------------------------------------------------*/
    LayoutUnit logicalTopValue = 0;
    LayoutUnit logicalBottomValue = 0;

    if (marginBefore.isAuto() && marginAfter.isAuto()) {
        // 'top' and 'bottom' cannot be 'auto' due to step 2 and 3 combined.
        ASSERT(!(logicalTop.isAuto() || logicalBottom.isAuto()));

        logicalTopValue = valueForLength(logicalTop, containerLogicalHeight);
        logicalBottomValue = valueForLength(logicalBottom, containerLogicalHeight);

        LayoutUnit difference = availableSpace - (logicalTopValue + logicalBottomValue);
        // NOTE: This may result in negative values.
        marginBeforeAlias =  difference / 2; // split the difference
        marginAfterAlias = difference - marginBeforeAlias; // account for odd valued differences

    /*-----------------------------------------------------------------------*\
     * 5. If at this point there is only one 'auto' left, solve the equation
     *    for that value.
    \*-----------------------------------------------------------------------*/
    } else if (logicalTop.isAuto()) {
        marginBeforeAlias = valueForLength(marginBefore, containerRelativeLogicalWidth);
        marginAfterAlias = valueForLength(marginAfter, containerRelativeLogicalWidth);
        logicalBottomValue = valueForLength(logicalBottom, containerLogicalHeight);

        // Solve for 'top'
        logicalTopValue = availableSpace - (logicalBottomValue + marginBeforeAlias + marginAfterAlias);
    } else if (logicalBottom.isAuto()) {
        marginBeforeAlias = valueForLength(marginBefore, containerRelativeLogicalWidth);
        marginAfterAlias = valueForLength(marginAfter, containerRelativeLogicalWidth);
        logicalTopValue = valueForLength(logicalTop, containerLogicalHeight);

        // Solve for 'bottom'
        // NOTE: It is not necessary to solve for 'bottom' because we don't ever
        // use the value.
    } else if (marginBefore.isAuto()) {
        marginAfterAlias = valueForLength(marginAfter, containerRelativeLogicalWidth);
        logicalTopValue = valueForLength(logicalTop, containerLogicalHeight);
        logicalBottomValue = valueForLength(logicalBottom, containerLogicalHeight);

        // Solve for 'margin-top'
        marginBeforeAlias = availableSpace - (logicalTopValue + logicalBottomValue + marginAfterAlias);
    } else if (marginAfter.isAuto()) {
        marginBeforeAlias = valueForLength(marginBefore, containerRelativeLogicalWidth);
        logicalTopValue = valueForLength(logicalTop, containerLogicalHeight);
        logicalBottomValue = valueForLength(logicalBottom, containerLogicalHeight);

        // Solve for 'margin-bottom'
        marginAfterAlias = availableSpace - (logicalTopValue + logicalBottomValue + marginBeforeAlias);
    } else {
        // Nothing is 'auto', just calculate the values.
        marginBeforeAlias = valueForLength(marginBefore, containerRelativeLogicalWidth);
        marginAfterAlias = valueForLength(marginAfter, containerRelativeLogicalWidth);
        logicalTopValue = valueForLength(logicalTop, containerLogicalHeight);
        // NOTE: It is not necessary to solve for 'bottom' because we don't ever
        // use the value.
    }

    /*-----------------------------------------------------------------------*\
     * 6. If at this point the values are over-constrained, ignore the value
     *    for 'bottom' and solve for that value.
    \*-----------------------------------------------------------------------*/
    // NOTE: It is not necessary to do this step because we don't end up using
    // the value of 'bottom' regardless of whether the values are over-constrained
    // or not.

    // Use computed values to calculate the vertical position.
    LayoutUnit logicalTopPos = logicalTopValue + marginBeforeAlias;
    computeLogicalTopPositionedOffset(logicalTopPos, this, computedValues.m_extent, containerBlock, containerLogicalHeight);
    computedValues.m_position = logicalTopPos;
}

LayoutRect LayoutBox::localCaretRect(InlineBox* box, int caretOffset, LayoutUnit* extraWidthToEndOfLine)
{
    // VisiblePositions at offsets inside containers either a) refer to the positions before/after
    // those containers (tables and select elements) or b) refer to the position inside an empty block.
    // They never refer to children.
    // FIXME: Paint the carets inside empty blocks differently than the carets before/after elements.

    LayoutRect rect(location(), LayoutSize(caretWidth(), size().height()));
    bool ltr = box ? box->isLeftToRightDirection() : style()->isLeftToRightDirection();

    if ((!caretOffset) ^ ltr)
        rect.move(LayoutSize(size().width() - caretWidth(), 0));

    if (box) {
        RootInlineBox& rootBox = box->root();
        LayoutUnit top = rootBox.lineTop();
        rect.setY(top);
        rect.setHeight(rootBox.lineBottom() - top);
    }

    // If height of box is smaller than font height, use the latter one,
    // otherwise the caret might become invisible.
    //
    // Also, if the box is not a replaced element, always use the font height.
    // This prevents the "big caret" bug described in:
    // <rdar://problem/3777804> Deleting all content in a document can result in giant tall-as-window insertion point
    //
    // FIXME: ignoring :first-line, missing good reason to take care of
    LayoutUnit fontHeight = style()->fontMetrics().height();
    if (fontHeight > rect.height() || (!isReplaced() && !isTable()))
        rect.setHeight(fontHeight);

    if (extraWidthToEndOfLine)
        *extraWidthToEndOfLine = location().x() + size().width() - rect.maxX();

    // Move to local coords
    rect.moveBy(-location());

    // FIXME: Border/padding should be added for all elements but this workaround
    // is needed because we use offsets inside an "atomic" element to represent
    // positions before and after the element in deprecated editing offsets.
    if (node() && !(editingIgnoresContent(node()) || isRenderedTableElement(node()))) {
        rect.setX(rect.x() + borderLeft() + paddingLeft());
        rect.setY(rect.y() + paddingTop() + borderTop());
    }

    if (!isHorizontalWritingMode())
        return rect.transposedRect();

    return rect;
}

PositionWithAffinity LayoutBox::positionForPoint(const LayoutPoint& point)
{
    // no children...return this layout object's element, if there is one, and offset 0
    LayoutObject* firstChild = slowFirstChild();
    if (!firstChild)
        return createPositionWithAffinity(nonPseudoNode() ? firstPositionInOrBeforeNode(nonPseudoNode()) : Position());

    if (isTable() && nonPseudoNode()) {
        LayoutUnit right = size().width() - verticalScrollbarWidth();
        LayoutUnit bottom = size().height() - horizontalScrollbarHeight();

        if (point.x() < 0 || point.x() > right || point.y() < 0 || point.y() > bottom) {
            if (point.x() <= right / 2)
                return createPositionWithAffinity(firstPositionInOrBeforeNode(nonPseudoNode()));
            return createPositionWithAffinity(lastPositionInOrAfterNode(nonPseudoNode()));
        }
    }

    // Pass off to the closest child.
    LayoutUnit minDist = LayoutUnit::max();
    LayoutBox* closestLayoutObject = nullptr;
    LayoutPoint adjustedPoint = point;
    if (isTableRow())
        adjustedPoint.moveBy(location());

    for (LayoutObject* layoutObject = firstChild; layoutObject; layoutObject = layoutObject->nextSibling()) {
        if ((!layoutObject->slowFirstChild() && !layoutObject->isInline() && !layoutObject->isLayoutBlockFlow() )
            || layoutObject->style()->visibility() != VISIBLE)
            continue;

        if (!layoutObject->isBox())
            continue;

        LayoutBox* layoutBox = toLayoutBox(layoutObject);

        LayoutUnit top = layoutBox->borderTop() + layoutBox->paddingTop() + (isTableRow() ? LayoutUnit() : layoutBox->location().y());
        LayoutUnit bottom = top + layoutBox->contentHeight();
        LayoutUnit left = layoutBox->borderLeft() + layoutBox->paddingLeft() + (isTableRow() ? LayoutUnit() : layoutBox->location().x());
        LayoutUnit right = left + layoutBox->contentWidth();

        if (point.x() <= right && point.x() >= left && point.y() <= top && point.y() >= bottom) {
            if (layoutBox->isTableRow())
                return layoutBox->positionForPoint(point + adjustedPoint - layoutBox->locationOffset());
            return layoutBox->positionForPoint(point - layoutBox->locationOffset());
        }

        // Find the distance from (x, y) to the box.  Split the space around the box into 8 pieces
        // and use a different compare depending on which piece (x, y) is in.
        LayoutPoint cmp;
        if (point.x() > right) {
            if (point.y() < top)
                cmp = LayoutPoint(right, top);
            else if (point.y() > bottom)
                cmp = LayoutPoint(right, bottom);
            else
                cmp = LayoutPoint(right, point.y());
        } else if (point.x() < left) {
            if (point.y() < top)
                cmp = LayoutPoint(left, top);
            else if (point.y() > bottom)
                cmp = LayoutPoint(left, bottom);
            else
                cmp = LayoutPoint(left, point.y());
        } else {
            if (point.y() < top)
                cmp = LayoutPoint(point.x(), top);
            else
                cmp = LayoutPoint(point.x(), bottom);
        }

        LayoutSize difference = cmp - point;

        LayoutUnit dist = difference.width() * difference.width() + difference.height() * difference.height();
        if (dist < minDist) {
            closestLayoutObject = layoutBox;
            minDist = dist;
        }
    }

    if (closestLayoutObject)
        return closestLayoutObject->positionForPoint(adjustedPoint - closestLayoutObject->locationOffset());
    return createPositionWithAffinity(firstPositionInOrBeforeNode(nonPseudoNode()));
}

bool LayoutBox::shrinkToAvoidFloats() const
{
    // Floating objects don't shrink.  Objects that don't avoid floats don't shrink.
    if (isInline() || !avoidsFloats() || isFloating())
        return false;

    // Only auto width objects can possibly shrink to avoid floats.
    return style()->width().isAuto();
}

static bool isReplacedElement(Node* node)
{
    // Checkboxes and radioboxes are not isReplaced() nor do they have their own layoutObject in which to override avoidFloats().
    return node && node->isElementNode() && (toElement(node)->isFormControlElement() || isHTMLImageElement(toElement(node)));
}

bool LayoutBox::avoidsFloats() const
{
    return isReplaced() || isReplacedElement(node()) || hasOverflowClip() || isHR() || isLegend() || isWritingModeRoot() || isFlexItemIncludingDeprecated();
}

bool LayoutBox::hasNonCompositedScrollbars() const
{
    if (DeprecatedPaintLayer* layer = this->layer()) {
        if (DeprecatedPaintLayerScrollableArea* scrollableArea = layer->scrollableArea()) {
            if (scrollableArea->hasHorizontalScrollbar() && !scrollableArea->layerForHorizontalScrollbar())
                return true;
            if (scrollableArea->hasVerticalScrollbar() && !scrollableArea->layerForVerticalScrollbar())
                return true;
        }
    }
    return false;
}

PaintInvalidationReason LayoutBox::paintInvalidationReason(const LayoutBoxModelObject& paintInvalidationContainer,
    const LayoutRect& oldBounds, const LayoutPoint& oldLocation, const LayoutRect& newBounds, const LayoutPoint& newLocation) const
{
    PaintInvalidationReason invalidationReason = LayoutBoxModelObject::paintInvalidationReason(paintInvalidationContainer, oldBounds, oldLocation, newBounds, newLocation);
    if (isFullPaintInvalidationReason(invalidationReason))
        return invalidationReason;

    if (isLayoutView()) {
        const LayoutView* layoutView = toLayoutView(this);
        // In normal compositing mode, root background doesn't need to be invalidated for
        // box changes, because the background always covers the whole document rect
        // and clipping is done by compositor()->m_containerLayer. Also the scrollbars
        // are always composited. There are no other box decoration on the LayoutView thus
        // we can safely exit here.
        if (layoutView->usesCompositing() && (!document().settings() || !document().settings()->rootLayerScrolls()))
            return invalidationReason;
    }

    // If the transform is not identity or translation, incremental invalidation is not applicable
    // because the difference between oldBounds and newBounds doesn't cover all area needing invalidation.
    // FIXME: Should also consider ancestor transforms since paintInvalidationContainer. crbug.com/426111.
    if (invalidationReason == PaintInvalidationIncremental
        && paintInvalidationContainer != this
        && hasLayer() && layer()->transform() && !layer()->transform()->isIdentityOrTranslation())
        return PaintInvalidationBoundsChange;

    if (!style()->hasBackground() && !style()->hasBoxDecorations()) {
        // We could let incremental invalidation cover non-composited scrollbars, but just
        // do a full invalidation because incremental invalidation will go away with slimming paint.
        if (invalidationReason == PaintInvalidationIncremental && hasNonCompositedScrollbars())
            return PaintInvalidationBorderBoxChange;
        return invalidationReason;
    }

    if (style()->backgroundLayers().thisOrNextLayersUseContentBox() || style()->maskLayers().thisOrNextLayersUseContentBox()) {
        LayoutRect oldContentBoxRect = m_rareData ? m_rareData->m_previousContentBoxRect : LayoutRect();
        LayoutRect newContentBoxRect = contentBoxRect();
        if (oldContentBoxRect != newContentBoxRect)
            return PaintInvalidationContentBoxChange;
    }

    if (style()->backgroundLayers().thisOrNextLayersHaveLocalAttachment()) {
        LayoutRect oldLayoutOverflowRect = m_rareData ? m_rareData->m_previousLayoutOverflowRect : LayoutRect();
        LayoutRect newLayoutOverflowRect = layoutOverflowRect();
        if (oldLayoutOverflowRect != newLayoutOverflowRect)
            return PaintInvalidationLayoutOverflowBoxChange;
    }

    LayoutSize oldBorderBoxSize = computePreviousBorderBoxSize(oldBounds.size());
    LayoutSize newBorderBoxSize = size();

    if (oldBorderBoxSize == newBorderBoxSize)
        return invalidationReason;

    // See another hasNonCompositedScrollbars() callsite above.
    if (hasNonCompositedScrollbars())
        return PaintInvalidationBorderBoxChange;

    if (style()->hasVisualOverflowingEffect() || style()->hasAppearance() || style()->hasFilter() || style()->resize() != RESIZE_NONE)
        return PaintInvalidationBorderBoxChange;

    if (style()->hasBorderRadius()) {
        // If a border-radius exists and width/height is smaller than radius width/height,
        // we need to fully invalidate to cover the changed radius.
        FloatRoundedRect oldRoundedRect = style()->getRoundedBorderFor(LayoutRect(LayoutPoint(0, 0), oldBorderBoxSize));
        FloatRoundedRect newRoundedRect = style()->getRoundedBorderFor(LayoutRect(LayoutPoint(0, 0), newBorderBoxSize));
        if (oldRoundedRect.radii() != newRoundedRect.radii())
            return PaintInvalidationBorderBoxChange;
    }

    if (oldBorderBoxSize.width() != newBorderBoxSize.width() && mustInvalidateBackgroundOrBorderPaintOnWidthChange())
        return PaintInvalidationBorderBoxChange;
    if (oldBorderBoxSize.height() != newBorderBoxSize.height() && mustInvalidateBackgroundOrBorderPaintOnHeightChange())
        return PaintInvalidationBorderBoxChange;

    return PaintInvalidationIncremental;
}

void LayoutBox::incrementallyInvalidatePaint(const LayoutBoxModelObject& paintInvalidationContainer, const LayoutRect& oldBounds, const LayoutRect& newBounds, const LayoutPoint& positionFromPaintInvalidationBacking)
{
    LayoutObject::incrementallyInvalidatePaint(paintInvalidationContainer, oldBounds, newBounds, positionFromPaintInvalidationBacking);

    bool hasBoxDecorations = style()->hasBoxDecorations();
    if (!style()->hasBackground() && !hasBoxDecorations)
        return;

    LayoutSize oldBorderBoxSize = computePreviousBorderBoxSize(oldBounds.size());
    LayoutSize newBorderBoxSize = size();

    // If border box size didn't change, LayoutObject's incrementallyInvalidatePaint() is good.
    if (oldBorderBoxSize == newBorderBoxSize)
        return;

    // If size of the paint invalidation rect equals to size of border box, LayoutObject::incrementallyInvalidatePaint()
    // is good for boxes having background without box decorations.
    ASSERT(oldBounds.location() == newBounds.location()); // Otherwise we won't do incremental invalidation.
    if (!hasBoxDecorations
        && positionFromPaintInvalidationBacking == newBounds.location()
        && oldBorderBoxSize == oldBounds.size()
        && newBorderBoxSize == newBounds.size())
        return;

    // Invalidate the right delta part and the right border of the old or new box which has smaller width.
    LayoutUnit deltaWidth = absoluteValue(oldBorderBoxSize.width() - newBorderBoxSize.width());
    if (deltaWidth) {
        LayoutUnit smallerWidth = std::min(oldBorderBoxSize.width(), newBorderBoxSize.width());
        LayoutUnit borderTopRightRadiusWidth = valueForLength(style()->borderTopRightRadius().width(), smallerWidth);
        LayoutUnit borderBottomRightRadiusWidth = valueForLength(style()->borderBottomRightRadius().width(), smallerWidth);
        LayoutUnit borderWidth = std::max<LayoutUnit>(borderRight(), std::max(borderTopRightRadiusWidth, borderBottomRightRadiusWidth));
        LayoutRect rightDeltaRect(positionFromPaintInvalidationBacking.x() + smallerWidth - borderWidth,
            positionFromPaintInvalidationBacking.y(),
            deltaWidth + borderWidth,
            std::max(oldBorderBoxSize.height(), newBorderBoxSize.height()));
        invalidatePaintRectClippedByOldAndNewBounds(paintInvalidationContainer, rightDeltaRect, oldBounds, newBounds);
    }

    // Invalidate the bottom delta part and the bottom border of the old or new box which has smaller height.
    LayoutUnit deltaHeight = absoluteValue(oldBorderBoxSize.height() - newBorderBoxSize.height());
    if (deltaHeight) {
        LayoutUnit smallerHeight = std::min(oldBorderBoxSize.height(), newBorderBoxSize.height());
        LayoutUnit borderBottomLeftRadiusHeight = valueForLength(style()->borderBottomLeftRadius().height(), smallerHeight);
        LayoutUnit borderBottomRightRadiusHeight = valueForLength(style()->borderBottomRightRadius().height(), smallerHeight);
        LayoutUnit borderHeight = std::max<LayoutUnit>(borderBottom(), std::max(borderBottomLeftRadiusHeight, borderBottomRightRadiusHeight));
        LayoutRect bottomDeltaRect(positionFromPaintInvalidationBacking.x(),
            positionFromPaintInvalidationBacking.y() + smallerHeight - borderHeight,
            std::max(oldBorderBoxSize.width(), newBorderBoxSize.width()),
            deltaHeight + borderHeight);
        invalidatePaintRectClippedByOldAndNewBounds(paintInvalidationContainer, bottomDeltaRect, oldBounds, newBounds);
    }
}

void LayoutBox::invalidatePaintRectClippedByOldAndNewBounds(const LayoutBoxModelObject& paintInvalidationContainer, const LayoutRect& rect, const LayoutRect& oldBounds, const LayoutRect& newBounds)
{
    if (rect.isEmpty())
        return;
    LayoutRect rectClippedByOldBounds = intersection(rect, oldBounds);
    LayoutRect rectClippedByNewBounds = intersection(rect, newBounds);
    // Invalidate only once if the clipped rects equal.
    if (rectClippedByOldBounds == rectClippedByNewBounds) {
        invalidatePaintUsingContainer(paintInvalidationContainer, rectClippedByOldBounds, PaintInvalidationIncremental);
        return;
    }
    // Invalidate the bigger one if one contains another. Otherwise invalidate both.
    if (!rectClippedByNewBounds.contains(rectClippedByOldBounds))
        invalidatePaintUsingContainer(paintInvalidationContainer, rectClippedByOldBounds, PaintInvalidationIncremental);
    if (!rectClippedByOldBounds.contains(rectClippedByNewBounds))
        invalidatePaintUsingContainer(paintInvalidationContainer, rectClippedByNewBounds, PaintInvalidationIncremental);
}

void LayoutBox::markForPaginationRelayoutIfNeeded(SubtreeLayoutScope& layoutScope)
{
    ASSERT(!needsLayout());
    // If fragmentation height has changed, we need to lay out. No need to enter the layoutObject if it
    // is childless, though.
    if (view()->layoutState()->pageLogicalHeightChanged() && slowFirstChild())
        layoutScope.setChildNeedsLayout(this);
}

void LayoutBox::addVisualEffectOverflow()
{
    if (!style()->hasVisualOverflowingEffect())
        return;

    // Add in the final overflow with shadows, outsets and outline combined.
    LayoutRect visualEffectOverflow = borderBoxRect();
    visualEffectOverflow.expand(computeVisualEffectOverflowOutsets());
    addVisualOverflow(visualEffectOverflow);
}

LayoutRectOutsets LayoutBox::computeVisualEffectOverflowOutsets() const
{
    ASSERT(style()->hasVisualOverflowingEffect());

    LayoutUnit top;
    LayoutUnit right;
    LayoutUnit bottom;
    LayoutUnit left;

    if (const ShadowList* boxShadow = style()->boxShadow()) {
        // FIXME: Use LayoutUnit edge outsets, and then simplify this.
        FloatRectOutsets outsets = boxShadow->rectOutsetsIncludingOriginal();
        top = outsets.top();
        right = outsets.right();
        bottom = outsets.bottom();
        left = outsets.left();
    }

    if (style()->hasBorderImageOutsets()) {
        LayoutRectOutsets borderOutsets = style()->borderImageOutsets();
        top = std::max(top, borderOutsets.top());
        right = std::max(right, borderOutsets.right());
        bottom = std::max(bottom, borderOutsets.bottom());
        left = std::max(left, borderOutsets.left());
    }

    if (style()->hasOutline()) {
        if (style()->outlineStyleIsAuto()) {
            // The result focus ring rects are in coordinates of this object's border box.
            Vector<LayoutRect> focusRingRects;
            addFocusRingRects(focusRingRects, LayoutPoint());
            LayoutRect rect = unionRect(focusRingRects);

            int outlineSize = GraphicsContext::focusRingOutsetExtent(style()->outlineOffset(), style()->outlineWidth());
            top = std::max(top, -rect.y() + outlineSize);
            right = std::max(right, rect.maxX() - size().width() + outlineSize);
            bottom = std::max(bottom, rect.maxY() - size().height() + outlineSize);
            left = std::max(left, -rect.x() + outlineSize);
        } else {
            LayoutUnit outlineSize = style()->outlineSize();
            top = std::max(top, outlineSize);
            right = std::max(right, outlineSize);
            bottom = std::max(bottom, outlineSize);
            left = std::max(left, outlineSize);
        }
    }

    return LayoutRectOutsets(top, right, bottom, left);
}

void LayoutBox::addOverflowFromChild(LayoutBox* child, const LayoutSize& delta)
{
    // Never allow flow threads to propagate overflow up to a parent.
    if (child->isLayoutFlowThread())
        return;

    // Only propagate layout overflow from the child if the child isn't clipping its overflow.  If it is, then
    // its overflow is internal to it, and we don't care about it.  layoutOverflowRectForPropagation takes care of this
    // and just propagates the border box rect instead.
    LayoutRect childLayoutOverflowRect = child->layoutOverflowRectForPropagation(styleRef());
    childLayoutOverflowRect.move(delta);
    addLayoutOverflow(childLayoutOverflowRect);

    // Add in visual overflow from the child.  Even if the child clips its overflow, it may still
    // have visual overflow of its own set from box shadows or reflections.  It is unnecessary to propagate this
    // overflow if we are clipping our own overflow.
    if (child->hasSelfPaintingLayer())
        return;
    LayoutRect childVisualOverflowRect = child->visualOverflowRectForPropagation(styleRef());
    childVisualOverflowRect.move(delta);
    addContentsVisualOverflow(childVisualOverflowRect);
}

void LayoutBox::addLayoutOverflow(const LayoutRect& rect)
{
    LayoutRect clientBox = noOverflowRect();
    if (clientBox.contains(rect) || rect.isEmpty())
        return;

    // For overflow clip objects, we don't want to propagate overflow into unreachable areas.
    LayoutRect overflowRect(rect);
    if (hasOverflowClip() || isLayoutView()) {
        // Overflow is in the block's coordinate space and thus is flipped for horizontal-bt and vertical-rl
        // writing modes.  At this stage that is actually a simplification, since we can treat horizontal-tb/bt as the same
        // and vertical-lr/rl as the same.
        bool hasTopOverflow = !style()->isLeftToRightDirection() && !isHorizontalWritingMode();
        bool hasLeftOverflow = !style()->isLeftToRightDirection() && isHorizontalWritingMode();
        if (isFlexibleBox() && style()->isReverseFlexDirection()) {
            LayoutFlexibleBox* flexibleBox = toLayoutFlexibleBox(this);
            if (flexibleBox->isHorizontalFlow())
                hasLeftOverflow = true;
            else
                hasTopOverflow = true;
        }

        if (!hasTopOverflow)
            overflowRect.shiftYEdgeTo(std::max(overflowRect.y(), clientBox.y()));
        else
            overflowRect.shiftMaxYEdgeTo(std::min(overflowRect.maxY(), clientBox.maxY()));
        if (!hasLeftOverflow)
            overflowRect.shiftXEdgeTo(std::max(overflowRect.x(), clientBox.x()));
        else
            overflowRect.shiftMaxXEdgeTo(std::min(overflowRect.maxX(), clientBox.maxX()));

        // Now re-test with the adjusted rectangle and see if it has become unreachable or fully
        // contained.
        if (clientBox.contains(overflowRect) || overflowRect.isEmpty())
            return;
    }

    if (!m_overflow)
        m_overflow = adoptPtr(new OverflowModel(clientBox, borderBoxRect()));

    m_overflow->addLayoutOverflow(overflowRect);
}

void LayoutBox::addVisualOverflow(const LayoutRect& rect)
{
    LayoutRect borderBox = borderBoxRect();
    if (borderBox.contains(rect) || rect.isEmpty())
        return;

    if (!m_overflow)
        m_overflow = adoptPtr(new OverflowModel(noOverflowRect(), borderBox));

    m_overflow->addVisualOverflow(rect);
}

void LayoutBox::addContentsVisualOverflow(const LayoutRect& rect)
{
    if (!hasOverflowClip()) {
        addVisualOverflow(rect);
        return;
    }

    if (!m_overflow)
        m_overflow = adoptPtr(new OverflowModel(noOverflowRect(), borderBoxRect()));
    m_overflow->addContentsVisualOverflow(rect);
}

void LayoutBox::clearLayoutOverflow()
{
    if (!m_overflow)
        return;

    if (!hasVisualOverflow() && contentsVisualOverflowRect().isEmpty()) {
        clearAllOverflows();
        return;
    }

    m_overflow->setLayoutOverflow(noOverflowRect());
}

static bool logicalWidthIsResolvable(const LayoutBox& layoutBox)
{
    const LayoutBox* box = &layoutBox;
    while (!box->isLayoutView() && !box->isOutOfFlowPositioned()
        && (box->style()->logicalWidth().isAuto() || box->isAnonymousBlock())
        && !box->hasOverrideContainingBlockLogicalWidth())
        box = box->containingBlock();

    if (box->style()->logicalWidth().isFixed())
        return true;
    if (box->isLayoutView())
        return true;
    // The size of the containing block of an absolutely positioned element is always definite with respect to that
    // element (http://dev.w3.org/csswg/css-sizing-3/#definite).
    if (box->isOutOfFlowPositioned())
        return true;
    if (box->hasOverrideContainingBlockLogicalWidth())
        return box->overrideContainingBlockContentLogicalWidth() != -1;
    if (box->style()->logicalWidth().hasPercent())
        return logicalWidthIsResolvable(*box->containingBlock());

    return false;
}

bool LayoutBox::hasDefiniteLogicalWidth() const
{
    return logicalWidthIsResolvable(*this);
}

inline static bool percentageLogicalHeightIsResolvable(const LayoutBox* box)
{
    return LayoutBox::percentageLogicalHeightIsResolvableFromBlock(box->containingBlock(), box->isOutOfFlowPositioned());
}

bool LayoutBox::percentageLogicalHeightIsResolvableFromBlock(const LayoutBlock* containingBlock, bool isOutOfFlowPositioned)
{
    // In quirks mode, blocks with auto height are skipped, and we keep looking for an enclosing
    // block that may have a specified height and then use it. In strict mode, this violates the
    // specification, which states that percentage heights just revert to auto if the containing
    // block has an auto height. We still skip anonymous containing blocks in both modes, though, and look
    // only at explicit containers.
    const LayoutBlock* cb = containingBlock;
    bool inQuirksMode = cb->document().inQuirksMode();
    while (!cb->isLayoutView() && !cb->isBody() && !cb->isTableCell() && !cb->isOutOfFlowPositioned() && cb->style()->logicalHeight().isAuto()) {
        if (!inQuirksMode && !cb->isAnonymousBlock())
            break;
        if (cb->hasOverrideContainingBlockLogicalHeight())
            return cb->overrideContainingBlockContentLogicalHeight() != -1;

        cb = cb->containingBlock();
    }

    // A positioned element that specified both top/bottom or that specifies height should be treated as though it has a height
    // explicitly specified that can be used for any percentage computations.
    // FIXME: We can't just check top/bottom here.
    // https://bugs.webkit.org/show_bug.cgi?id=46500
    bool isOutOfFlowPositionedWithSpecifiedHeight = cb->isOutOfFlowPositioned() && (!cb->style()->logicalHeight().isAuto() || (!cb->style()->top().isAuto() && !cb->style()->bottom().isAuto()));

    // Table cells violate what the CSS spec says to do with heights.  Basically we
    // don't care if the cell specified a height or not.  We just always make ourselves
    // be a percentage of the cell's current content height.
    if (cb->isTableCell())
        return true;

    // Otherwise we only use our percentage height if our containing block had a specified
    // height.
    if (cb->style()->logicalHeight().isFixed())
        return true;
    if (cb->style()->logicalHeight().hasPercent() && !isOutOfFlowPositionedWithSpecifiedHeight)
        return percentageLogicalHeightIsResolvableFromBlock(cb->containingBlock(), cb->isOutOfFlowPositioned());
    if (cb->isLayoutView() || inQuirksMode || isOutOfFlowPositionedWithSpecifiedHeight)
        return true;
    if (cb->isDocumentElement() && isOutOfFlowPositioned) {
        // Match the positioned objects behavior, which is that positioned objects will fill their viewport
        // always.  Note we could only hit this case by recurring into computePercentageLogicalHeight on a positioned containing block.
        return true;
    }

    return false;
}

bool LayoutBox::hasDefiniteLogicalHeight() const
{
    const Length& logicalHeight = style()->logicalHeight();
    if (logicalHeight.isIntrinsicOrAuto())
        return false;
    if (logicalHeight.isFixed())
        return true;
    // The size of the containing block of an absolutely positioned element is always definite with respect to that
    // element (http://dev.w3.org/csswg/css-sizing-3/#definite).
    if (isOutOfFlowPositioned())
        return true;
    if (hasOverrideContainingBlockLogicalHeight())
        return overrideContainingBlockContentLogicalHeight() != -1;

    return percentageLogicalHeightIsResolvable(this);
}

bool LayoutBox::hasUnsplittableScrollingOverflow() const
{
    // We will paginate as long as we don't scroll overflow in the pagination direction.
    bool isHorizontal = isHorizontalWritingMode();
    if ((isHorizontal && !scrollsOverflowY()) || (!isHorizontal && !scrollsOverflowX()))
        return false;

    // We do have overflow. We'll still be willing to paginate as long as the block
    // has auto logical height, auto or undefined max-logical-height and a zero or auto min-logical-height.
    // Note this is just a heuristic, and it's still possible to have overflow under these
    // conditions, but it should work out to be good enough for common cases. Paginating overflow
    // with scrollbars present is not the end of the world and is what we used to do in the old model anyway.
    return !style()->logicalHeight().isIntrinsicOrAuto()
        || (!style()->logicalMaxHeight().isIntrinsicOrAuto() && !style()->logicalMaxHeight().isMaxSizeNone() && (!style()->logicalMaxHeight().hasPercent() || percentageLogicalHeightIsResolvable(this)))
        || (!style()->logicalMinHeight().isIntrinsicOrAuto() && style()->logicalMinHeight().isPositive() && (!style()->logicalMinHeight().hasPercent() || percentageLogicalHeightIsResolvable(this)));
}

bool LayoutBox::isUnsplittableForPagination() const
{
    return isReplaced() || hasUnsplittableScrollingOverflow() || (parent() && isWritingModeRoot());
}

LayoutUnit LayoutBox::lineHeight(bool /*firstLine*/, LineDirectionMode direction, LinePositionMode /*linePositionMode*/) const
{
    if (isReplaced())
        return direction == HorizontalLine ? marginHeight() + size().height() : marginWidth() + size().width();
    return LayoutUnit();
}

int LayoutBox::baselinePosition(FontBaseline baselineType, bool /*firstLine*/, LineDirectionMode direction, LinePositionMode linePositionMode) const
{
    ASSERT(linePositionMode == PositionOnContainingLine);
    if (isReplaced()) {
        int result = direction == HorizontalLine ? marginHeight() + size().height() : marginWidth() + size().width();
        if (baselineType == AlphabeticBaseline)
            return result;
        return result - result / 2;
    }
    return 0;
}


DeprecatedPaintLayer* LayoutBox::enclosingFloatPaintingLayer() const
{
    const LayoutObject* curr = this;
    while (curr) {
        DeprecatedPaintLayer* layer = curr->hasLayer() && curr->isBox() ? toLayoutBox(curr)->layer() : 0;
        if (layer && layer->isSelfPaintingLayer())
            return layer;
        curr = curr->parent();
    }
    return nullptr;
}

LayoutRect LayoutBox::logicalVisualOverflowRectForPropagation(const ComputedStyle& parentStyle) const
{
    LayoutRect rect = visualOverflowRectForPropagation(parentStyle);
    if (!parentStyle.isHorizontalWritingMode())
        return rect.transposedRect();
    return rect;
}

LayoutRect LayoutBox::visualOverflowRectForPropagation(const ComputedStyle& parentStyle) const
{
    // If the writing modes of the child and parent match, then we don't have to
    // do anything fancy. Just return the result.
    LayoutRect rect = visualOverflowRect();
    if (parentStyle.writingMode() == style()->writingMode())
        return rect;

    // We are putting ourselves into our parent's coordinate space.  If there is a flipped block mismatch
    // in a particular axis, then we have to flip the rect along that axis.
    if (style()->writingMode() == RightToLeftWritingMode || parentStyle.writingMode() == RightToLeftWritingMode)
        rect.setX(size().width() - rect.maxX());
    else if (style()->writingMode() == BottomToTopWritingMode || parentStyle.writingMode() == BottomToTopWritingMode)
        rect.setY(size().height() - rect.maxY());

    return rect;
}

LayoutRect LayoutBox::logicalLayoutOverflowRectForPropagation(const ComputedStyle& parentStyle) const
{
    LayoutRect rect = layoutOverflowRectForPropagation(parentStyle);
    if (!parentStyle.isHorizontalWritingMode())
        return rect.transposedRect();
    return rect;
}

LayoutRect LayoutBox::layoutOverflowRectForPropagation(const ComputedStyle& parentStyle) const
{
    // Only propagate interior layout overflow if we don't clip it.
    LayoutRect rect = borderBoxRect();
    // We want to include the margin, but only when it adds height. Quirky margins don't contribute height
    // nor do the margins of self-collapsing blocks.
    if (!styleRef().hasMarginAfterQuirk() && !isSelfCollapsingBlock())
        rect.expand(isHorizontalWritingMode() ? LayoutSize(LayoutUnit(), marginAfter()) : LayoutSize(marginAfter(), LayoutUnit()));

    if (!hasOverflowClip())
        rect.unite(layoutOverflowRect());

    bool hasTransform = hasLayer() && layer()->transform();
    if (isRelPositioned() || hasTransform) {
        // If we are relatively positioned or if we have a transform, then we have to convert
        // this rectangle into physical coordinates, apply relative positioning and transforms
        // to it, and then convert it back.
        flipForWritingMode(rect);

        if (hasTransform)
            rect = layer()->currentTransform().mapRect(rect);

        if (isRelPositioned())
            rect.move(offsetForInFlowPosition());

        // Now we need to flip back.
        flipForWritingMode(rect);
    }

    // If the writing modes of the child and parent match, then we don't have to
    // do anything fancy. Just return the result.
    if (parentStyle.writingMode() == style()->writingMode())
        return rect;

    // We are putting ourselves into our parent's coordinate space.  If there is a flipped block mismatch
    // in a particular axis, then we have to flip the rect along that axis.
    if (style()->writingMode() == RightToLeftWritingMode || parentStyle.writingMode() == RightToLeftWritingMode)
        rect.setX(size().width() - rect.maxX());
    else if (style()->writingMode() == BottomToTopWritingMode || parentStyle.writingMode() == BottomToTopWritingMode)
        rect.setY(size().height() - rect.maxY());

    return rect;
}

LayoutRect LayoutBox::noOverflowRect() const
{
    // Because of the special coordinate system used for overflow rectangles and many other
    // rectangles (not quite logical, not quite physical), we need to flip the block progression
    // coordinate in vertical-rl and horizontal-bt writing modes. In other words, the rectangle
    // returned is physical, except for the block direction progression coordinate (y in horizontal
    // writing modes, x in vertical writing modes), which is always "logical top". Apart from the
    // flipping, this method does the same as clientBoxRect().

    const int scrollBarWidth = verticalScrollbarWidth();
    const int scrollBarHeight = horizontalScrollbarHeight();
    LayoutUnit left = borderLeft() + (style()->shouldPlaceBlockDirectionScrollbarOnLogicalLeft() ? scrollBarWidth : 0);
    LayoutUnit top = borderTop();
    LayoutUnit right = borderRight();
    LayoutUnit bottom = borderBottom();
    LayoutRect rect(left, top, size().width() - left - right, size().height() - top - bottom);
    flipForWritingMode(rect);
    // Subtract space occupied by scrollbars. Order is important here: first flip, then subtract
    // scrollbars. This may seem backwards and weird, since one would think that a horizontal
    // scrollbar at the physical bottom in horizontal-bt ought to be at the logical top (physical
    // bottom), between the logical top (physical bottom) border and the logical top (physical
    // bottom) padding. But this is how the rest of the code expects us to behave. This is highly
    // related to https://bugs.webkit.org/show_bug.cgi?id=76129
    // FIXME: when the above mentioned bug is fixed, it should hopefully be possible to call
    // clientBoxRect() or paddingBoxRect() in this method, rather than fiddling with the edges on
    // our own.
    if (style()->shouldPlaceBlockDirectionScrollbarOnLogicalLeft())
        rect.contract(0, scrollBarHeight);
    else
        rect.contract(scrollBarWidth, scrollBarHeight);
    return rect;
}

LayoutUnit LayoutBox::offsetLeft() const
{
    return adjustedPositionRelativeToOffsetParent(topLeftLocation()).x();
}

LayoutUnit LayoutBox::offsetTop() const
{
    return adjustedPositionRelativeToOffsetParent(topLeftLocation()).y();
}

LayoutPoint LayoutBox::flipForWritingModeForChild(const LayoutBox* child, const LayoutPoint& point) const
{
    if (!style()->isFlippedBlocksWritingMode())
        return point;

    // The child is going to add in its x() and y(), so we have to make sure it ends up in
    // the right place.
    if (isHorizontalWritingMode())
        return LayoutPoint(point.x(), point.y() + size().height() - child->size().height() - (2 * child->location().y()));
    return LayoutPoint(point.x() + size().width() - child->size().width() - (2 * child->location().x()), point.y());
}

LayoutPoint LayoutBox::topLeftLocation() const
{
    LayoutBlock* containerBlock = containingBlock();
    if (!containerBlock || containerBlock == this)
        return location();
    return containerBlock->flipForWritingModeForChild(this, location());
}

bool LayoutBox::hasRelativeLogicalWidth() const
{
    return style()->logicalWidth().hasPercent()
        || style()->logicalMinWidth().hasPercent()
        || style()->logicalMaxWidth().hasPercent();
}

bool LayoutBox::hasRelativeLogicalHeight() const
{
    return style()->logicalHeight().hasPercent()
        || style()->logicalMinHeight().hasPercent()
        || style()->logicalMaxHeight().hasPercent();
}

static void markBoxForRelayoutAfterSplit(LayoutBox* box)
{
    // FIXME: The table code should handle that automatically. If not,
    // we should fix it and remove the table part checks.
    if (box->isTable()) {
        // Because we may have added some sections with already computed column structures, we need to
        // sync the table structure with them now. This avoids crashes when adding new cells to the table.
        toLayoutTable(box)->forceSectionsRecalc();
    } else if (box->isTableSection()) {
        toLayoutTableSection(box)->setNeedsCellRecalc();
    }

    box->setNeedsLayoutAndPrefWidthsRecalcAndFullPaintInvalidation(LayoutInvalidationReason::AnonymousBlockChange);
}

LayoutObject* LayoutBox::splitAnonymousBoxesAroundChild(LayoutObject* beforeChild)
{
    bool didSplitParentAnonymousBoxes = false;

    while (beforeChild->parent() != this) {
        LayoutBox* boxToSplit = toLayoutBox(beforeChild->parent());
        if (boxToSplit->slowFirstChild() != beforeChild && boxToSplit->isAnonymous()) {
            didSplitParentAnonymousBoxes = true;

            // We have to split the parent box into two boxes and move children
            // from |beforeChild| to end into the new post box.
            LayoutBox* postBox = boxToSplit->createAnonymousBoxWithSameTypeAs(this);
            postBox->setChildrenInline(boxToSplit->childrenInline());
            LayoutBox* parentBox = toLayoutBox(boxToSplit->parent());
            // We need to invalidate the |parentBox| before inserting the new node
            // so that the table paint invalidation logic knows the structure is dirty.
            // See for example LayoutTableCell:clippedOverflowRectForPaintInvalidation.
            markBoxForRelayoutAfterSplit(parentBox);
            parentBox->virtualChildren()->insertChildNode(parentBox, postBox, boxToSplit->nextSibling());
            boxToSplit->moveChildrenTo(postBox, beforeChild, 0, true);

            markBoxForRelayoutAfterSplit(boxToSplit);
            markBoxForRelayoutAfterSplit(postBox);

            beforeChild = postBox;
        } else {
            beforeChild = boxToSplit;
        }
    }

    if (didSplitParentAnonymousBoxes)
        markBoxForRelayoutAfterSplit(this);

    ASSERT(beforeChild->parent() == this);
    return beforeChild;
}

LayoutUnit LayoutBox::offsetFromLogicalTopOfFirstPage() const
{
    LayoutState* layoutState = view()->layoutState();
    if (!layoutState || !layoutState->isPaginated())
        return LayoutUnit();

    if (layoutState->layoutObject() == this) {
        LayoutSize offsetDelta = layoutState->layoutOffset() - layoutState->pageOffset();
        return isHorizontalWritingMode() ? offsetDelta.height() : offsetDelta.width();
    }

    // A LayoutBlock always establishes a layout state, and this method is only meant to be called
    // on the object currently being laid out.
    ASSERT(!isLayoutBlock());

    // In case this box doesn't establish a layout state, try the containing block.
    LayoutBlock* containerBlock = containingBlock();
    ASSERT(layoutState->layoutObject() == containerBlock);
    return containerBlock->offsetFromLogicalTopOfFirstPage() + logicalTop();
}

void LayoutBox::setPageLogicalOffset(LayoutUnit offset)
{
    if (!m_rareData && !offset)
        return;
    ensureRareData().m_pageLogicalOffset = offset;
}

bool LayoutBox::needToSavePreviousBoxSizes()
{
    // If m_rareData is already created, always save.
    if (m_rareData)
        return true;

    LayoutSize paintInvalidationSize = previousPaintInvalidationRect().size();
    // Don't save old box sizes if the paint rect is empty because we'll
    // full invalidate once the paint rect becomes non-empty.
    if (paintInvalidationSize.isEmpty())
        return false;

    // We need the old box sizes only when the box has background, decorations, or masks.
    // Main LayoutView paints base background, thus interested in box size.
    if (!isLayoutView() && !style()->hasBackground() && !style()->hasBoxDecorations() && !style()->hasMask())
        return false;

    // No need to save old border box size if we can use size of the old paint
    // rect as the old border box size in the next invalidation.
    if (paintInvalidationSize != size())
        return true;

    // Background and mask layers can depend on other boxes than border box. See crbug.com/490533
    if (style()->backgroundLayers().thisOrNextLayersUseContentBox() || style()->backgroundLayers().thisOrNextLayersHaveLocalAttachment()
        || style()->maskLayers().thisOrNextLayersUseContentBox())
        return true;

    return false;
}

void LayoutBox::savePreviousBoxSizesIfNeeded()
{
    if (!needToSavePreviousBoxSizes())
        return;

    LayoutBoxRareData& rareData = ensureRareData();
    rareData.m_previousBorderBoxSize = size();
    rareData.m_previousContentBoxRect = contentBoxRect();
    rareData.m_previousLayoutOverflowRect = layoutOverflowRect();
}

LayoutSize LayoutBox::computePreviousBorderBoxSize(const LayoutSize& previousBoundsSize) const
{
    // PreviousBorderBoxSize is only valid when there is background or box decorations.
    ASSERT(style()->hasBackground() || style()->hasBoxDecorations());

    if (m_rareData && m_rareData->m_previousBorderBoxSize.width() != -1)
        return m_rareData->m_previousBorderBoxSize;

    // We didn't save the old border box size because it was the same as the size of oldBounds.
    return previousBoundsSize;
}

void LayoutBox::logicalExtentAfterUpdatingLogicalWidth(const LayoutUnit& newLogicalTop, LayoutBox::LogicalExtentComputedValues& computedValues)
{
    // FIXME: None of this is right for perpendicular writing-mode children.
    LayoutUnit oldLogicalWidth = logicalWidth();
    LayoutUnit oldLogicalLeft = logicalLeft();
    LayoutUnit oldMarginLeft = marginLeft();
    LayoutUnit oldMarginRight = marginRight();
    LayoutUnit oldLogicalTop = logicalTop();

    setLogicalTop(newLogicalTop);
    updateLogicalWidth();

    computedValues.m_extent = logicalWidth();
    computedValues.m_position = logicalLeft();
    computedValues.m_margins.m_start = marginStart();
    computedValues.m_margins.m_end = marginEnd();

    setLogicalTop(oldLogicalTop);
    setLogicalWidth(oldLogicalWidth);
    setLogicalLeft(oldLogicalLeft);
    setMarginLeft(oldMarginLeft);
    setMarginRight(oldMarginRight);
}

inline bool LayoutBox::mustInvalidateFillLayersPaintOnWidthChange(const FillLayer& layer) const
{
    // Nobody will use multiple layers without wanting fancy positioning.
    if (layer.next())
        return true;

    // Make sure we have a valid image.
    StyleImage* img = layer.image();
    if (!img || !img->canRender(*this, style()->effectiveZoom()))
        return false;

    if (layer.repeatX() != RepeatFill && layer.repeatX() != NoRepeatFill)
        return true;

    // TODO(alancutter): Make this work correctly for calc lengths.
    if (layer.xPosition().hasPercent() && !layer.xPosition().isZero())
        return true;

    if (layer.backgroundXOrigin() != LeftEdge)
        return true;

    EFillSizeType sizeType = layer.sizeType();

    if (sizeType == Contain || sizeType == Cover)
        return true;

    if (sizeType == SizeLength) {
        // TODO(alancutter): Make this work correctly for calc lengths.
        if (layer.sizeLength().width().hasPercent() && !layer.sizeLength().width().isZero())
            return true;
        if (img->isGeneratedImage() && layer.sizeLength().width().isAuto())
            return true;
    } else if (img->usesImageContainerSize()) {
        return true;
    }

    return false;
}

bool LayoutBox::mustInvalidateBackgroundOrBorderPaintOnWidthChange() const
{
    if (hasMask() && mustInvalidateFillLayersPaintOnWidthChange(style()->maskLayers()))
        return true;

    // If we don't have a background/border/mask, then nothing to do.
    if (!hasBoxDecorationBackground())
        return false;

    if (mustInvalidateFillLayersPaintOnWidthChange(style()->backgroundLayers()))
        return true;

    // Our fill layers are ok. Let's check border.
    if (style()->hasBorderDecoration() && canRenderBorderImage())
        return true;

    return false;
}

bool LayoutBox::mustInvalidateBackgroundOrBorderPaintOnHeightChange() const
{
    if (hasMask() && mustInvalidateFillLayersPaintOnHeightChange(style()->maskLayers()))
        return true;

    // If we don't have a background/border/mask, then nothing to do.
    if (!hasBoxDecorationBackground())
        return false;

    if (mustInvalidateFillLayersPaintOnHeightChange(style()->backgroundLayers()))
        return true;

    // Our fill layers are ok.  Let's check border.
    if (style()->hasBorderDecoration() && canRenderBorderImage())
        return true;

    return false;
}

bool LayoutBox::canRenderBorderImage() const
{
    if (!style()->hasBorderDecoration())
        return false;

    StyleImage* borderImage = style()->borderImage().image();
    return borderImage && borderImage->canRender(*this, style()->effectiveZoom()) && borderImage->isLoaded();
}

ShapeOutsideInfo* LayoutBox::shapeOutsideInfo() const
{
    return ShapeOutsideInfo::isEnabledFor(*this) ? ShapeOutsideInfo::info(*this) : nullptr;
}

//////////////////////////////////////////////////////////////////////////

void LayoutBox::setX(LayoutUnit x)
{
    if (x == m_frameRect.x())
        return;
    m_frameRect.setX(x);
    frameRectChanged();
}

void LayoutBox::setY(LayoutUnit y)
{
    if (y == m_frameRect.y())
        return;
    m_frameRect.setY(y);
    frameRectChanged();
}

void LayoutBox::setWidth(LayoutUnit width)
{
    if (width == m_frameRect.width())
        return;
    m_frameRect.setWidth(width);
    frameRectChanged();
}

void LayoutBox::setHeight(LayoutUnit height)
{
    if (height == m_frameRect.height())
        return;
    m_frameRect.setHeight(height);
    frameRectChanged();
}

LayoutUnit LayoutBox::logicalLeft() const { return style()->isHorizontalWritingMode() ? m_frameRect.x() : m_frameRect.y(); }
LayoutUnit LayoutBox::logicalRight() const { return logicalLeft() + logicalWidth(); }
LayoutUnit LayoutBox::logicalTop() const { return style()->isHorizontalWritingMode() ? m_frameRect.y() : m_frameRect.x(); }
LayoutUnit LayoutBox::logicalBottom() const { return logicalTop() + logicalHeight(); }
LayoutUnit LayoutBox::logicalWidth() const { return style()->isHorizontalWritingMode() ? m_frameRect.width() : m_frameRect.height(); }
LayoutUnit LayoutBox::logicalHeight() const { return style()->isHorizontalWritingMode() ? m_frameRect.height() : m_frameRect.width(); }

int LayoutBox::pixelSnappedLogicalHeight() const { return style()->isHorizontalWritingMode() ? pixelSnappedHeight() : pixelSnappedWidth(); }
int LayoutBox::pixelSnappedLogicalWidth() const { return style()->isHorizontalWritingMode() ? pixelSnappedWidth() : pixelSnappedHeight(); }

LayoutUnit LayoutBox::minimumLogicalHeightForEmptyLine() const {
    return borderAndPaddingLogicalHeight() + scrollbarLogicalHeight()
        + lineHeight(true, isHorizontalWritingMode() ? HorizontalLine : VerticalLine, PositionOfInteriorLineBoxes);
}

void LayoutBox::setLogicalLeft(LayoutUnit left)
{
    if (style()->isHorizontalWritingMode())
        setX(left);
    else
        setY(left);
}

void LayoutBox::setLogicalTop(LayoutUnit top)
{
    if (style()->isHorizontalWritingMode())
        setY(top);
    else
        setX(top);
}

void LayoutBox::setLogicalLocation(const LayoutPoint& location)
{
    if (style()->isHorizontalWritingMode())
        setLocation(location);
    else
        setLocation(location.transposedPoint());
}

void LayoutBox::setLogicalWidth(LayoutUnit size)
{
    if (style()->isHorizontalWritingMode())
        setWidth(size);
    else
        setHeight(size);
}

void LayoutBox::setLogicalHeight(LayoutUnit size)
{
    if (style()->isHorizontalWritingMode())
        setHeight(size);
    else
        setWidth(size);
}

LayoutPoint LayoutBox::location() const { return m_frameRect.location(); }
LayoutSize LayoutBox::locationOffset() const { return LayoutSize(m_frameRect.x(), m_frameRect.y()); }
LayoutSize LayoutBox::size() const { return m_frameRect.size(); }
IntSize LayoutBox::pixelSnappedSize() const { return m_frameRect.pixelSnappedSize(); }

void LayoutBox::setLocation(const LayoutPoint& location)
{
    if (location == m_frameRect.location())
        return;
    m_frameRect.setLocation(location);
    frameRectChanged();
}

void LayoutBox::setSize(const LayoutSize& size)
{
    if (size == m_frameRect.size())
        return;
    m_frameRect.setSize(size);

    frameRectChanged();
}

void LayoutBox::move(LayoutUnit dx, LayoutUnit dy)
{
    if (!dx && !dy)
        return;
    m_frameRect.move(dx, dy);
    frameRectChanged();
}

LayoutRect LayoutBox::frameRect() const { return m_frameRect; }

void LayoutBox::setFrameRect(const LayoutRect& rect) {
    if (rect == m_frameRect)
        return;
    m_frameRect = rect;

    frameRectChanged();
}

} // namespace blink
