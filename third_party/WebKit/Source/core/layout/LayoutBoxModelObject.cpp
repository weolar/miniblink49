/*
 * Copyright (C) 1999 Lars Knoll (knoll@kde.org)
 *           (C) 1999 Antti Koivisto (koivisto@kde.org)
 *           (C) 2005 Allan Sandfeld Jensen (kde@carewolf.com)
 *           (C) 2005, 2006 Samuel Weinig (sam.weinig@gmail.com)
 * Copyright (C) 2005, 2006, 2007, 2008, 2009 Apple Inc. All rights reserved.
 * Copyright (C) 2010 Google Inc. All rights reserved.
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
#include "core/layout/LayoutBoxModelObject.h"

#include "core/dom/NodeComputedStyle.h"
#include "core/html/HTMLBodyElement.h"
#include "core/layout/ImageQualityController.h"
#include "core/layout/LayoutBlock.h"
#include "core/layout/LayoutFlowThread.h"
#include "core/layout/LayoutGeometryMap.h"
#include "core/layout/LayoutInline.h"
#include "core/layout/LayoutObject.h"
#include "core/layout/LayoutTextFragment.h"
#include "core/layout/LayoutView.h"
#include "core/layout/compositing/CompositedDeprecatedPaintLayerMapping.h"
#include "core/layout/compositing/DeprecatedPaintLayerCompositor.h"
#include "core/page/scrolling/ScrollingConstraints.h"
#include "core/paint/DeprecatedPaintLayer.h"
#include "core/style/BorderEdge.h"
#include "core/style/ShadowList.h"
#include "platform/LengthFunctions.h"
#include "platform/geometry/TransformState.h"
#include "wtf/CurrentTime.h"

namespace blink {

class FloatStateForStyleChange {
public:
    static void setWasFloating(LayoutBoxModelObject* boxModelObject, bool wasFloating)
    {
        s_wasFloating = wasFloating;
        s_boxModelObject = boxModelObject;
    }

    static bool wasFloating(LayoutBoxModelObject* boxModelObject)
    {
        ASSERT(boxModelObject == s_boxModelObject);
        return s_wasFloating;
    }

private:
    // Used to store state between styleWillChange and styleDidChange
    static bool s_wasFloating;
    static LayoutBoxModelObject* s_boxModelObject;
};

bool FloatStateForStyleChange::s_wasFloating = false;
LayoutBoxModelObject* FloatStateForStyleChange::s_boxModelObject = nullptr;

// The HashMap for storing continuation pointers.
// An inline can be split with blocks occuring in between the inline content.
// When this occurs we need a pointer to the next object. We can basically be
// split into a sequence of inlines and blocks. The continuation will either be
// an anonymous block (that houses other blocks) or it will be an inline flow.
// <b><i><p>Hello</p></i></b>. In this example the <i> will have a block as
// its continuation but the <b> will just have an inline as its continuation.
typedef HashMap<const LayoutBoxModelObject*, LayoutBoxModelObject*> ContinuationMap;
static ContinuationMap* continuationMap = nullptr;

void LayoutBoxModelObject::setSelectionState(SelectionState state)
{
    if (state == SelectionInside && selectionState() != SelectionNone)
        return;

    if ((state == SelectionStart && selectionState() == SelectionEnd)
        || (state == SelectionEnd && selectionState() == SelectionStart))
        LayoutObject::setSelectionState(SelectionBoth);
    else
        LayoutObject::setSelectionState(state);

    // FIXME: We should consider whether it is OK propagating to ancestor LayoutInlines.
    // This is a workaround for http://webkit.org/b/32123
    // The containing block can be null in case of an orphaned tree.
    LayoutBlock* containingBlock = this->containingBlock();
    if (containingBlock && !containingBlock->isLayoutView())
        containingBlock->setSelectionState(state);
}

void LayoutBoxModelObject::contentChanged(ContentChangeType changeType)
{
    if (!hasLayer())
        return;

    layer()->contentChanged(changeType);
}

bool LayoutBoxModelObject::hasAcceleratedCompositing() const
{
    return view()->compositor()->hasAcceleratedCompositing();
}

LayoutBoxModelObject::LayoutBoxModelObject(ContainerNode* node)
    : LayoutObject(node)
{
}

LayoutBoxModelObject::~LayoutBoxModelObject()
{
    // Our layer should have been destroyed and cleared by now
    ASSERT(!hasLayer());
    ASSERT(!m_layer);
}

void LayoutBoxModelObject::willBeDestroyed()
{
    ImageQualityController::remove(this);

    // A continuation of this LayoutObject should be destroyed at subclasses.
    ASSERT(!continuation());

    if (isPositioned()) {
        // Don't use this->view() because the document's layoutView has been set to 0 during destruction.
        if (LocalFrame* frame = this->frame()) {
            if (FrameView* frameView = frame->view()) {
                if (style()->hasViewportConstrainedPosition())
                    frameView->removeViewportConstrainedObject(this);
            }
        }
    }

    LayoutObject::willBeDestroyed();

    destroyLayer();
}

void LayoutBoxModelObject::styleWillChange(StyleDifference diff, const ComputedStyle& newStyle)
{
    // This object's layer may cease to be a stacking context, in which case the paint
    // invalidation container of the children may change. Thus we need to invalidate paint
    // eagerly for all such children.
    if (hasLayer()
        && enclosingLayer()->stackingNode()
        && enclosingLayer()->stackingNode()->isStackingContext()
        && newStyle.hasAutoZIndex()) {
        // The following disablers are valid because we need to invalidate based on the current
        // status.
        DisableCompositingQueryAsserts compositingDisabler;
        DisablePaintInvalidationStateAsserts paintDisabler;
        invalidatePaintIncludingNonCompositingDescendants();
    } else { // weolar
        bool b0 = hasLayer();
        bool b1 = false;
        bool b2 = false;
        if (b0) {
            b1 = enclosingLayer()->stackingNode();
            if (b1)
                b2 = enclosingLayer()->stackingNode()->isStackingContext();
        }
        bool b3 = newStyle.hasAutoZIndex();
        b3 = b3;
    }

    FloatStateForStyleChange::setWasFloating(this, isFloating());

    if (const ComputedStyle* oldStyle = style()) {
        if (parent() && diff.needsPaintInvalidationLayer()) {
            if (oldStyle->hasAutoClip() != newStyle.hasAutoClip()
                || oldStyle->clip() != newStyle.clip())
                layer()->clipper().clearClipRectsIncludingDescendants();
        }
    }

    LayoutObject::styleWillChange(diff, newStyle);
}

void LayoutBoxModelObject::styleDidChange(StyleDifference diff, const ComputedStyle* oldStyle)
{
    bool hadTransform = hasTransformRelatedProperty();
    bool hadLayer = hasLayer();
    bool layerWasSelfPainting = hadLayer && layer()->isSelfPaintingLayer();
    bool wasFloatingBeforeStyleChanged = FloatStateForStyleChange::wasFloating(this);

    LayoutObject::styleDidChange(diff, oldStyle);
    updateFromStyle();

    // When an out-of-flow-positioned element changes its display between block and inline-block,
    // then an incremental layout on the element's containing block lays out the element through
    // LayoutPositionedObjects, which skips laying out the element's parent.
    // The element's parent needs to relayout so that it calls
    // LayoutBlockFlow::setStaticInlinePositionForChild with the out-of-flow-positioned child, so
    // that when it's laid out, its LayoutBox::computePositionedLogicalWidth/Height takes into
    // account its new inline/block position rather than its old block/inline position.
    // Position changes and other types of display changes are handled elsewhere.
    if (oldStyle && isOutOfFlowPositioned() && parent() && (parent() != containingBlock())
        && (styleRef().position() == oldStyle->position())
        && (styleRef().originalDisplay() != oldStyle->originalDisplay())
        && ((styleRef().originalDisplay() == BLOCK) || (styleRef().originalDisplay() == INLINE_BLOCK))
        && ((oldStyle->originalDisplay() == BLOCK) || (oldStyle->originalDisplay() == INLINE_BLOCK)))
        parent()->setNeedsLayout(LayoutInvalidationReason::ChildChanged, MarkContainerChain);

    DeprecatedPaintLayerType type = layerTypeRequired();
    if (type != NoDeprecatedPaintLayer) {
        if (!layer() && layerCreationAllowedForSubtree()) {
            if (wasFloatingBeforeStyleChanged && isFloating())
                setChildNeedsLayout();
            createLayer(type);
            if (parent() && !needsLayout()) {
                // FIXME: We should call a specialized version of this function.
                layer()->updateLayerPositionsAfterLayout();
            }
        }
    } else if (layer() && layer()->parent()) {
        DeprecatedPaintLayer* parentLayer = layer()->parent();
        setHasTransformRelatedProperty(false); // Either a transform wasn't specified or the object doesn't support transforms, so just null out the bit.
        setHasReflection(false);
        layer()->removeOnlyThisLayer(); // calls destroyLayer() which clears m_layer
        if (wasFloatingBeforeStyleChanged && isFloating())
            setChildNeedsLayout();
        if (hadTransform)
            setNeedsLayoutAndPrefWidthsRecalcAndFullPaintInvalidation(LayoutInvalidationReason::StyleChange);
        if (!needsLayout()) {
            // FIXME: We should call a specialized version of this function.
            parentLayer->updateLayerPositionsAfterLayout();
        }
    }

    if (layer()) {
        // FIXME: Ideally we shouldn't need this setter but we can't easily infer an overflow-only layer
        // from the style.
        layer()->setLayerType(type);

        layer()->styleChanged(diff, oldStyle);
        if (hadLayer && layer()->isSelfPaintingLayer() != layerWasSelfPainting)
            setChildNeedsLayout();
    }

    // Fixed-position is painted using transform. In the case that the object
    // gets the same layout after changing position property, although no
    // re-raster (rect-based invalidation) is needed, display items should
    // still update their paint offset.
    if (RuntimeEnabledFeatures::slimmingPaintEnabled() && oldStyle) {
        bool newStyleIsFixedPosition = style()->position() == FixedPosition;
        bool oldStyleIsFixedPosition = oldStyle->position() == FixedPosition;
        if (newStyleIsFixedPosition != oldStyleIsFixedPosition)
            invalidateDisplayItemClientForNonCompositingDescendants();
    }

    // The used style for body background may change due to computed style change
    // on the document element because of background stealing.
    // Refer to backgroundStolenForBeingBody() and
    // http://www.w3.org/TR/css3-background/#body-background for more info.
    if (isDocumentElement()) {
        HTMLBodyElement* body = document().firstBodyElement();
        LayoutObject* bodyLayout = body ? body->layoutObject() : nullptr;
        if (bodyLayout && bodyLayout->isBoxModelObject()) {
            bool newStoleBodyBackground = toLayoutBoxModelObject(bodyLayout)->backgroundStolenForBeingBody(style());
            bool oldStoleBodyBackground = oldStyle && toLayoutBoxModelObject(bodyLayout)->backgroundStolenForBeingBody(oldStyle);
            if (newStoleBodyBackground != oldStoleBodyBackground
                && bodyLayout->style() && bodyLayout->style()->hasBackground()) {
                bodyLayout->setShouldDoFullPaintInvalidation();
            }
        }
    }

    if (FrameView *frameView = view()->frameView()) {
        bool newStyleIsViewportConstained = style()->hasViewportConstrainedPosition();
        bool oldStyleIsViewportConstrained = oldStyle && oldStyle->hasViewportConstrainedPosition();
        if (newStyleIsViewportConstained != oldStyleIsViewportConstrained) {
            if (newStyleIsViewportConstained && layer())
                frameView->addViewportConstrainedObject(this);
            else
                frameView->removeViewportConstrainedObject(this);
        }
    }
}

void LayoutBoxModelObject::createLayer(DeprecatedPaintLayerType type)
{
    ASSERT(!m_layer);
    m_layer = adoptPtr(new DeprecatedPaintLayer(this, type));
    setHasLayer(true);
    m_layer->insertOnlyThisLayer();
}

void LayoutBoxModelObject::destroyLayer()
{
    setHasLayer(false);
    m_layer = nullptr;
}

bool LayoutBoxModelObject::hasSelfPaintingLayer() const
{
    return m_layer && m_layer->isSelfPaintingLayer();
}

DeprecatedPaintLayerScrollableArea* LayoutBoxModelObject::scrollableArea() const
{
    return m_layer ? m_layer->scrollableArea() : 0;
}

void LayoutBoxModelObject::addLayerHitTestRects(LayerHitTestRects& rects, const DeprecatedPaintLayer* currentLayer, const LayoutPoint& layerOffset, const LayoutRect& containerRect) const
{
    if (hasLayer()) {
        if (isLayoutView()) {
            // LayoutView is handled with a special fast-path, but it needs to know the current layer.
            LayoutObject::addLayerHitTestRects(rects, layer(), LayoutPoint(), LayoutRect());
        } else {
            // Since a LayoutObject never lives outside it's container Layer, we can switch
            // to marking entire layers instead. This may sometimes mark more than necessary (when
            // a layer is made of disjoint objects) but in practice is a significant performance
            // savings.
            layer()->addLayerHitTestRects(rects);
        }
    } else {
        LayoutObject::addLayerHitTestRects(rects, currentLayer, layerOffset, containerRect);
    }
}

void LayoutBoxModelObject::invalidateTreeIfNeeded(PaintInvalidationState& paintInvalidationState)
{
    ASSERT(!needsLayout());

    if (!shouldCheckForPaintInvalidation(paintInvalidationState))
        return;

    bool establishesNewPaintInvalidationContainer = isPaintInvalidationContainer();
    const LayoutBoxModelObject& newPaintInvalidationContainer = *adjustCompositedContainerForSpecialAncestors(establishesNewPaintInvalidationContainer ? this : &paintInvalidationState.paintInvalidationContainer());
    // FIXME: This assert should be re-enabled when we move paint invalidation to after compositing update. crbug.com/360286
    // ASSERT(&newPaintInvalidationContainer == containerForPaintInvalidation());

    PaintInvalidationReason reason = invalidatePaintIfNeeded(paintInvalidationState, newPaintInvalidationContainer);
    clearPaintInvalidationState(paintInvalidationState);

    if (reason == PaintInvalidationDelayedFull)
        paintInvalidationState.pushDelayedPaintInvalidationTarget(*this);

    PaintInvalidationState childTreeWalkState(paintInvalidationState, *this, newPaintInvalidationContainer);
    if (reason == PaintInvalidationLocationChange)
        childTreeWalkState.setAncestorHadPaintInvalidationForLocationChange();
    invalidatePaintOfSubtreesIfNeeded(childTreeWalkState);
}

void LayoutBoxModelObject::setBackingNeedsPaintInvalidationInRect(const LayoutRect& r, PaintInvalidationReason invalidationReason) const
{
    // https://bugs.webkit.org/show_bug.cgi?id=61159 describes an unreproducible crash here,
    // so assert but check that the layer is composited.
    ASSERT(compositingState() != NotComposited);

    // FIXME: generalize accessors to backing GraphicsLayers so that this code is squashing-agnostic.
    if (layer()->groupedMapping()) {
        LayoutRect paintInvalidationRect = r;
        if (GraphicsLayer* squashingLayer = layer()->groupedMapping()->squashingLayer()) {
            // Note: the subpixel accumulation of layer() does not need to be added here. It is already taken into account.
            squashingLayer->setNeedsDisplayInRect(pixelSnappedIntRect(paintInvalidationRect), invalidationReason);
        }
    } else {
        layer()->compositedDeprecatedPaintLayerMapping()->setContentsNeedDisplayInRect(r, invalidationReason);
    }
}

void LayoutBoxModelObject::invalidateDisplayItemClientOnBacking(const DisplayItemClientWrapper& displayItemClient) const
{
    ASSERT(RuntimeEnabledFeatures::slimmingPaintEnabled());

    if (layer()->groupedMapping()) {
        if (GraphicsLayer* squashingLayer = layer()->groupedMapping()->squashingLayer())
            squashingLayer->invalidateDisplayItemClient(displayItemClient);
    } else if (CompositedDeprecatedPaintLayerMapping* compositedDeprecatedPaintLayerMapping = layer()->compositedDeprecatedPaintLayerMapping()) {
        compositedDeprecatedPaintLayerMapping->invalidateDisplayItemClient(displayItemClient);
    }
}

void LayoutBoxModelObject::addFocusRingRectsForNormalChildren(Vector<LayoutRect>& rects, const LayoutPoint& additionalOffset) const
{
    for (LayoutObject* child = slowFirstChild(); child; child = child->nextSibling()) {
        // Focus rings of out-of-flow positioned descendants are handled in LayoutBlock::addFocusRingRects().
        if (child->isOutOfFlowPositioned())
            continue;

        // Focus ring of an element continuation or anonymous block continuation is added when we iterate the continuation chain.
        // See LayoutBlock::addFocusRingRects() and LayoutInline::addFocusRingRects().
        if (child->isElementContinuation()
            || (child->isLayoutBlock() && toLayoutBlock(child)->isAnonymousBlockContinuation()))
            continue;

        addFocusRingRectsForDescendant(*child, rects, additionalOffset);
    }
}

void LayoutBoxModelObject::addFocusRingRectsForDescendant(const LayoutObject& descendant, Vector<LayoutRect>& rects, const LayoutPoint& additionalOffset) const
{
    if (descendant.isText() || descendant.isListMarker())
        return;

    if (descendant.hasLayer()) {
        Vector<LayoutRect> layerFocusRingRects;
        descendant.addFocusRingRects(layerFocusRingRects, LayoutPoint());
        for (size_t i = 0; i < layerFocusRingRects.size(); ++i) {
            FloatQuad quadInBox = toLayoutBoxModelObject(descendant).localToContainerQuad(FloatQuad(layerFocusRingRects[i]), this);
            LayoutRect rect = LayoutRect(quadInBox.boundingBox());
            if (!rect.isEmpty()) {
                rect.moveBy(additionalOffset);
                rects.append(rect);
            }
        }
        return;
    }

    if (descendant.isBox()) {
        descendant.addFocusRingRects(rects, additionalOffset + toLayoutBox(descendant).locationOffset());
        return;
    }

    descendant.addFocusRingRects(rects, additionalOffset);
}

bool LayoutBoxModelObject::calculateHasBoxDecorations() const
{
    const ComputedStyle& styleToUse = styleRef();
    return hasBackground() || styleToUse.hasBorderDecoration() || styleToUse.hasAppearance() || styleToUse.boxShadow();
}

bool LayoutBoxModelObject::hasNonEmptyLayoutSize() const
{
    for (const LayoutBoxModelObject* root = this; root; root = root->continuation()) {
        for (const LayoutObject* object = root; object; object = object->nextInPreOrder(object)) {
            if (object->isBox()) {
                const LayoutBox& box = toLayoutBox(*object);
                if (box.logicalHeight() && box.logicalWidth())
                    return true;
            } else if (object->isLayoutInline()) {
                const LayoutInline& layoutInline = toLayoutInline(*object);
                if (!layoutInline.linesVisualOverflowBoundingBox().isEmpty())
                    return true;
            } else {
                ASSERT(object->isText());
            }
        }
    }
    return false;
}

void LayoutBoxModelObject::updateFromStyle()
{
    const ComputedStyle& styleToUse = styleRef();
    setHasBoxDecorationBackground(calculateHasBoxDecorations());
    setInline(styleToUse.isDisplayInlineType());
    setPositionState(styleToUse.position());
    setHorizontalWritingMode(styleToUse.isHorizontalWritingMode());
}

static LayoutSize accumulateInFlowPositionOffsets(const LayoutObject* child)
{
    if (!child->isAnonymousBlock() || !child->isRelPositioned())
        return LayoutSize();
    LayoutSize offset;
    LayoutObject* p = toLayoutBlock(child)->inlineElementContinuation();
    while (p && p->isLayoutInline()) {
        if (p->isRelPositioned()) {
            LayoutInline* layoutInline = toLayoutInline(p);
            offset += layoutInline->offsetForInFlowPosition();
        }
        p = p->parent();
    }
    return offset;
}

LayoutBlock* LayoutBoxModelObject::containingBlockForAutoHeightDetection(Length logicalHeight) const
{
    // For percentage heights: The percentage is calculated with respect to the height of the generated box's
    // containing block. If the height of the containing block is not specified explicitly (i.e., it depends
    // on content height), and this element is not absolutely positioned, the value computes to 'auto'.
    if (!logicalHeight.hasPercent() || isOutOfFlowPositioned())
        return nullptr;

    // Anonymous block boxes are ignored when resolving percentage values that would refer to it:
    // the closest non-anonymous ancestor box is used instead.
    LayoutBlock* cb = containingBlock();
    while (cb->isAnonymous())
        cb = cb->containingBlock();

    // Matching LayoutBox::percentageLogicalHeightIsResolvableFromBlock() by
    // ignoring table cell's attribute value, where it says that table cells violate
    // what the CSS spec says to do with heights. Basically we
    // don't care if the cell specified a height or not.
    if (cb->isTableCell())
        return nullptr;

    // Match LayoutBox::availableLogicalHeightUsing by special casing
    // the layout view. The available height is taken from the frame.
    if (cb->isLayoutView())
        return nullptr;

    if (cb->isOutOfFlowPositioned() && !cb->style()->logicalTop().isAuto() && !cb->style()->logicalBottom().isAuto())
        return nullptr;

    return cb;
}

bool LayoutBoxModelObject::hasAutoHeightOrContainingBlockWithAutoHeight() const
{
    Length logicalHeightLength = style()->logicalHeight();
    if (logicalHeightLength.isAuto())
        return true;

    if (document().inQuirksMode())
        return false;

    // If the height of the containing block computes to 'auto', then it hasn't been 'specified explicitly'.
    if (LayoutBlock* cb = containingBlockForAutoHeightDetection(logicalHeightLength))
        return cb->hasAutoHeightOrContainingBlockWithAutoHeight();
    return false;
}

LayoutSize LayoutBoxModelObject::relativePositionOffset() const
{
    LayoutSize offset = accumulateInFlowPositionOffsets(this);

    LayoutBlock* containingBlock = this->containingBlock();

    // Objects that shrink to avoid floats normally use available line width when computing containing block width.  However
    // in the case of relative positioning using percentages, we can't do this.  The offset should always be resolved using the
    // available width of the containing block.  Therefore we don't use containingBlockLogicalWidthForContent() here, but instead explicitly
    // call availableWidth on our containing block.
    if (!style()->left().isAuto()) {
        if (!style()->right().isAuto() && !containingBlock->style()->isLeftToRightDirection())
            offset.setWidth(-valueForLength(style()->right(), containingBlock->availableWidth()));
        else
            offset.expand(valueForLength(style()->left(), containingBlock->availableWidth()), 0);
    } else if (!style()->right().isAuto()) {
        offset.expand(-valueForLength(style()->right(), containingBlock->availableWidth()), 0);
    }

    // If the containing block of a relatively positioned element does not
    // specify a height, a percentage top or bottom offset should be resolved as
    // auto. An exception to this is if the containing block has the WinIE quirk
    // where <html> and <body> assume the size of the viewport. In this case,
    // calculate the percent offset based on this height.
    // See <https://bugs.webkit.org/show_bug.cgi?id=26396>.
    if (!style()->top().isAuto()
        && (!containingBlock->hasAutoHeightOrContainingBlockWithAutoHeight()
            || !style()->top().hasPercent()
            || containingBlock->stretchesToViewport()))
        offset.expand(0, valueForLength(style()->top(), containingBlock->availableHeight()));

    else if (!style()->bottom().isAuto()
        && (!containingBlock->hasAutoHeightOrContainingBlockWithAutoHeight()
            || !style()->bottom().hasPercent()
            || containingBlock->stretchesToViewport()))
        offset.expand(0, -valueForLength(style()->bottom(), containingBlock->availableHeight()));

    return offset;
}

LayoutPoint LayoutBoxModelObject::adjustedPositionRelativeToOffsetParent(const LayoutPoint& startPoint) const
{
    // If the element is the HTML body element or doesn't have a parent
    // return 0 and stop this algorithm.
    if (isBody() || !parent())
        return LayoutPoint();

    LayoutPoint referencePoint = startPoint;
    referencePoint.move(parent()->columnOffset(referencePoint));

    // If the offsetParent of the element is null, or is the HTML body element,
    // return the distance between the canvas origin and the left border edge
    // of the element and stop this algorithm.
    Element* element = offsetParent();
    if (!element)
        return referencePoint;

    if (const LayoutBoxModelObject* offsetParent = element->layoutBoxModelObject()) {
        if (offsetParent->isBox() && !offsetParent->isBody())
            referencePoint.move(-toLayoutBox(offsetParent)->borderLeft(), -toLayoutBox(offsetParent)->borderTop());
        if (!isOutOfFlowPositioned() || flowThreadContainingBlock()) {
            if (isRelPositioned())
                referencePoint.move(relativePositionOffset());

            LayoutObject* current;
            for (current = parent(); current != offsetParent && current->parent(); current = current->parent()) {
                // FIXME: What are we supposed to do inside SVG content?
                if (!isOutOfFlowPositioned()) {
                    if (current->isBox() && !current->isTableRow())
                        referencePoint.moveBy(toLayoutBox(current)->topLeftLocation());
                    referencePoint.move(current->parent()->columnOffset(referencePoint));
                }
            }

            if (offsetParent->isBox() && offsetParent->isBody() && !offsetParent->isPositioned())
                referencePoint.moveBy(toLayoutBox(offsetParent)->topLeftLocation());
        }
    }

    return referencePoint;
}

LayoutSize LayoutBoxModelObject::offsetForInFlowPosition() const
{
    return isRelPositioned() ? relativePositionOffset() : LayoutSize();
}

LayoutUnit LayoutBoxModelObject::offsetLeft() const
{
    // Note that LayoutInline and LayoutBox override this to pass a different
    // startPoint to adjustedPositionRelativeToOffsetParent.
    return adjustedPositionRelativeToOffsetParent(LayoutPoint()).x();
}

LayoutUnit LayoutBoxModelObject::offsetTop() const
{
    // Note that LayoutInline and LayoutBox override this to pass a different
    // startPoint to adjustedPositionRelativeToOffsetParent.
    return adjustedPositionRelativeToOffsetParent(LayoutPoint()).y();
}

int LayoutBoxModelObject::pixelSnappedOffsetWidth() const
{
    return snapSizeToPixel(offsetWidth(), offsetLeft());
}

int LayoutBoxModelObject::pixelSnappedOffsetHeight() const
{
    return snapSizeToPixel(offsetHeight(), offsetTop());
}

LayoutUnit LayoutBoxModelObject::computedCSSPadding(const Length& padding) const
{
    LayoutUnit w = 0;
    if (padding.hasPercent())
        w = containingBlockLogicalWidthForContent();
    return minimumValueForLength(padding, w);
}

static inline int resolveWidthForRatio(int height, const FloatSize& intrinsicRatio)
{
    return ceilf(height * intrinsicRatio.width() / intrinsicRatio.height());
}

static inline int resolveHeightForRatio(int width, const FloatSize& intrinsicRatio)
{
    return ceilf(width * intrinsicRatio.height() / intrinsicRatio.width());
}

static inline IntSize resolveAgainstIntrinsicWidthOrHeightAndRatio(const IntSize& size, const FloatSize& intrinsicRatio, int useWidth, int useHeight)
{
    if (intrinsicRatio.isEmpty()) {
        if (useWidth)
            return IntSize(useWidth, size.height());
        return IntSize(size.width(), useHeight);
    }

    if (useWidth)
        return IntSize(useWidth, resolveHeightForRatio(useWidth, intrinsicRatio));
    return IntSize(resolveWidthForRatio(useHeight, intrinsicRatio), useHeight);
}

static inline IntSize resolveAgainstIntrinsicRatio(const IntSize& size, const FloatSize& intrinsicRatio)
{
    // Two possible solutions: (size.width(), solutionHeight) or (solutionWidth, size.height())
    // "... must be assumed to be the largest dimensions..." = easiest answer: the rect with the largest surface area.

    int solutionWidth = resolveWidthForRatio(size.height(), intrinsicRatio);
    int solutionHeight = resolveHeightForRatio(size.width(), intrinsicRatio);
    if (solutionWidth <= size.width()) {
        if (solutionHeight <= size.height()) {
            // If both solutions fit, choose the one covering the larger area.
            int areaOne = solutionWidth * size.height();
            int areaTwo = size.width() * solutionHeight;
            if (areaOne < areaTwo)
                return IntSize(size.width(), solutionHeight);
            return IntSize(solutionWidth, size.height());
        }

        // Only the first solution fits.
        return IntSize(solutionWidth, size.height());
    }

    // Only the second solution fits, assert that.
    ASSERT(solutionHeight <= size.height());
    return IntSize(size.width(), solutionHeight);
}

IntSize LayoutBoxModelObject::calculateImageIntrinsicDimensions(StyleImage* image, const IntSize& positioningAreaSize, ScaleByEffectiveZoomOrNot shouldScaleOrNot) const
{
    // A generated image without a fixed size, will always return the container size as intrinsic size.
    if (image->isGeneratedImage() && image->usesImageContainerSize())
        return IntSize(positioningAreaSize.width(), positioningAreaSize.height());

    Length intrinsicWidth(Fixed);
    Length intrinsicHeight(Fixed);
    FloatSize intrinsicRatio;
    image->computeIntrinsicDimensions(this, intrinsicWidth, intrinsicHeight, intrinsicRatio);

    ASSERT(intrinsicWidth.isFixed());
    ASSERT(intrinsicHeight.isFixed());

    IntSize resolvedSize(intrinsicWidth.value(), intrinsicHeight.value());
    IntSize minimumSize(resolvedSize.width() > 0 ? 1 : 0, resolvedSize.height() > 0 ? 1 : 0);
    if (shouldScaleOrNot == ScaleByEffectiveZoom)
        resolvedSize.scale(style()->effectiveZoom());
    resolvedSize.clampToMinimumSize(minimumSize);

    if (!resolvedSize.isEmpty())
        return resolvedSize;

    // If the image has one of either an intrinsic width or an intrinsic height:
    // * and an intrinsic aspect ratio, then the missing dimension is calculated from the given dimension and the ratio.
    // * and no intrinsic aspect ratio, then the missing dimension is assumed to be the size of the rectangle that
    //   establishes the coordinate system for the 'background-position' property.
    if (resolvedSize.width() > 0 || resolvedSize.height() > 0)
        return resolveAgainstIntrinsicWidthOrHeightAndRatio(positioningAreaSize, intrinsicRatio, resolvedSize.width(), resolvedSize.height());

    // If the image has no intrinsic dimensions and has an intrinsic ratio the dimensions must be assumed to be the
    // largest dimensions at that ratio such that neither dimension exceeds the dimensions of the rectangle that
    // establishes the coordinate system for the 'background-position' property.
    if (!intrinsicRatio.isEmpty())
        return resolveAgainstIntrinsicRatio(positioningAreaSize, intrinsicRatio);

    // If the image has no intrinsic ratio either, then the dimensions must be assumed to be the rectangle that
    // establishes the coordinate system for the 'background-position' property.
    return positioningAreaSize;
}

bool LayoutBoxModelObject::boxShadowShouldBeAppliedToBackground(BackgroundBleedAvoidance bleedAvoidance, InlineFlowBox* inlineFlowBox) const
{
    if (bleedAvoidance != BackgroundBleedNone)
        return false;

    if (style()->hasAppearance())
        return false;

    const ShadowList* shadowList = style()->boxShadow();
    if (!shadowList)
        return false;

    bool hasOneNormalBoxShadow = false;
    size_t shadowCount = shadowList->shadows().size();
    for (size_t i = 0; i < shadowCount; ++i) {
        const ShadowData& currentShadow = shadowList->shadows()[i];
        if (currentShadow.style() != Normal)
            continue;

        if (hasOneNormalBoxShadow)
            return false;
        hasOneNormalBoxShadow = true;

        if (currentShadow.spread())
            return false;
    }

    if (!hasOneNormalBoxShadow)
        return false;

    Color backgroundColor = resolveColor(CSSPropertyBackgroundColor);
    if (backgroundColor.hasAlpha())
        return false;

    const FillLayer* lastBackgroundLayer = &style()->backgroundLayers();
    for (const FillLayer* next = lastBackgroundLayer->next(); next; next = lastBackgroundLayer->next())
        lastBackgroundLayer = next;

    if (lastBackgroundLayer->clip() != BorderFillBox)
        return false;

    if (lastBackgroundLayer->image() && style()->hasBorderRadius())
        return false;

    if (inlineFlowBox && !inlineFlowBox->boxShadowCanBeAppliedToBackground(*lastBackgroundLayer))
        return false;

    if (hasOverflowClip() && lastBackgroundLayer->attachment() == LocalBackgroundAttachment)
        return false;

    return true;
}



LayoutUnit LayoutBoxModelObject::containingBlockLogicalWidthForContent() const
{
    return containingBlock()->availableLogicalWidth();
}

LayoutBoxModelObject* LayoutBoxModelObject::continuation() const
{
    return (!continuationMap) ? nullptr : continuationMap->get(this);
}

void LayoutBoxModelObject::setContinuation(LayoutBoxModelObject* continuation)
{
    if (continuation) {
        if (!continuationMap)
            continuationMap = new ContinuationMap;
        continuationMap->set(this, continuation);
    } else {
        if (continuationMap)
            continuationMap->remove(this);
    }
}

void LayoutBoxModelObject::computeLayerHitTestRects(LayerHitTestRects& rects) const
{
    LayoutObject::computeLayerHitTestRects(rects);

    // If there is a continuation then we need to consult it here, since this is
    // the root of the tree walk and it wouldn't otherwise get picked up.
    // Continuations should always be siblings in the tree, so any others should
    // get picked up already by the tree walk.
    if (continuation())
        continuation()->computeLayerHitTestRects(rects);
}

LayoutRect LayoutBoxModelObject::localCaretRectForEmptyElement(LayoutUnit width, LayoutUnit textIndentOffset)
{
    ASSERT(!slowFirstChild());

    // FIXME: This does not take into account either :first-line or :first-letter
    // However, as soon as some content is entered, the line boxes will be
    // constructed and this kludge is not called any more. So only the caret size
    // of an empty :first-line'd block is wrong. I think we can live with that.
    const ComputedStyle& currentStyle = firstLineStyleRef();

    enum CaretAlignment { AlignLeft, AlignRight, AlignCenter };

    CaretAlignment alignment = AlignLeft;

    switch (currentStyle.textAlign()) {
    case LEFT:
    case WEBKIT_LEFT:
        break;
    case CENTER:
    case WEBKIT_CENTER:
        alignment = AlignCenter;
        break;
    case RIGHT:
    case WEBKIT_RIGHT:
        alignment = AlignRight;
        break;
    case JUSTIFY:
    case TASTART:
        if (!currentStyle.isLeftToRightDirection())
            alignment = AlignRight;
        break;
    case TAEND:
        if (currentStyle.isLeftToRightDirection())
            alignment = AlignRight;
        break;
    }

    LayoutUnit x = borderLeft() + paddingLeft();
    LayoutUnit maxX = width - borderRight() - paddingRight();

    switch (alignment) {
    case AlignLeft:
        if (currentStyle.isLeftToRightDirection())
            x += textIndentOffset;
        break;
    case AlignCenter:
        x = (x + maxX) / 2;
        if (currentStyle.isLeftToRightDirection())
            x += textIndentOffset / 2;
        else
            x -= textIndentOffset / 2;
        break;
    case AlignRight:
        x = maxX - caretWidth();
        if (!currentStyle.isLeftToRightDirection())
            x -= textIndentOffset;
        break;
    }
    x = std::min(x, std::max<LayoutUnit>(maxX - caretWidth(), 0));

    LayoutUnit height = style()->fontMetrics().height();
    LayoutUnit verticalSpace = lineHeight(true, currentStyle.isHorizontalWritingMode() ? HorizontalLine : VerticalLine,  PositionOfInteriorLineBoxes) - height;
    LayoutUnit y = paddingTop() + borderTop() + (verticalSpace / 2);
    return currentStyle.isHorizontalWritingMode() ? LayoutRect(x, y, caretWidth(), height) : LayoutRect(y, x, height, caretWidth());
}

void LayoutBoxModelObject::mapAbsoluteToLocalPoint(MapCoordinatesFlags mode, TransformState& transformState) const
{
    LayoutObject* o = container();
    if (!o)
        return;

    o->mapAbsoluteToLocalPoint(mode, transformState);

    LayoutSize containerOffset = offsetFromContainer(o, LayoutPoint());

    if (o->isLayoutFlowThread()) {
        // Descending into a flow thread. Convert to the local coordinate space, i.e. flow thread coordinates.
        const LayoutFlowThread* flowThread = toLayoutFlowThread(o);
        LayoutPoint visualPoint = LayoutPoint(transformState.mappedPoint());
        transformState.move(visualPoint - flowThread->visualPointToFlowThreadPoint(visualPoint));
        // |containerOffset| is also in visual coordinates. Convert to flow thread coordinates.
        // TODO(mstensho): Wouldn't it be better add a parameter to instruct offsetFromContainer()
        // to return flowthread coordinates in the first place? We're effectively performing two
        // conversions here, when in fact none is needed.
        containerOffset = toLayoutSize(flowThread->visualPointToFlowThreadPoint(toLayoutPoint(containerOffset)));
    }

    bool preserve3D = mode & UseTransforms && (o->style()->preserves3D() || style()->preserves3D());
    if (mode & UseTransforms && shouldUseTransformFromContainer(o)) {
        TransformationMatrix t;
        getTransformFromContainer(o, containerOffset, t);
        transformState.applyTransform(t, preserve3D ? TransformState::AccumulateTransform : TransformState::FlattenTransform);
    } else {
        transformState.move(containerOffset.width(), containerOffset.height(), preserve3D ? TransformState::AccumulateTransform : TransformState::FlattenTransform);
    }
}

const LayoutObject* LayoutBoxModelObject::pushMappingToContainer(const LayoutBoxModelObject* ancestorToStopAt, LayoutGeometryMap& geometryMap) const
{
    ASSERT(ancestorToStopAt != this);

    bool ancestorSkipped;
    LayoutObject* container = this->container(ancestorToStopAt, &ancestorSkipped);
    if (!container)
        return nullptr;

    bool isInline = isLayoutInline();
    bool isFixedPos = !isInline && style()->position() == FixedPosition;
    bool hasTransform = !isInline && hasLayer() && layer()->transform();

    LayoutSize adjustmentForSkippedAncestor;
    if (ancestorSkipped) {
        // There can't be a transform between paintInvalidationContainer and ancestorToStopAt, because transforms create containers, so it should be safe
        // to just subtract the delta between the ancestor and ancestorToStopAt.
        adjustmentForSkippedAncestor = -ancestorToStopAt->offsetFromAncestorContainer(container);
    }

    bool offsetDependsOnPoint = false;
    LayoutSize containerOffset = offsetFromContainer(container, LayoutPoint(), &offsetDependsOnPoint);

    bool preserve3D = container->style()->preserves3D() || style()->preserves3D();
    if (shouldUseTransformFromContainer(container)) {
        TransformationMatrix t;
        getTransformFromContainer(container, containerOffset, t);
        t.translateRight(adjustmentForSkippedAncestor.width().toFloat(), adjustmentForSkippedAncestor.height().toFloat());
        geometryMap.push(this, t, preserve3D, offsetDependsOnPoint, isFixedPos, hasTransform);
    } else {
        containerOffset += adjustmentForSkippedAncestor;
        geometryMap.push(this, containerOffset, preserve3D, offsetDependsOnPoint, isFixedPos, hasTransform);
    }

    return ancestorSkipped ? ancestorToStopAt : container;
}

void LayoutBoxModelObject::moveChildTo(LayoutBoxModelObject* toBoxModelObject, LayoutObject* child, LayoutObject* beforeChild, bool fullRemoveInsert)
{
    // We assume that callers have cleared their positioned objects list for child moves (!fullRemoveInsert) so the
    // positioned layoutObject maps don't become stale. It would be too slow to do the map lookup on each call.
    ASSERT(!fullRemoveInsert || !isLayoutBlock() || !toLayoutBlock(this)->hasPositionedObjects());

    // https://chromium.googlesource.com/chromium/src/+/7afb0474da50c93fa2360fc70ccbb73a313a45f7%5E%21/#F4
    // If a child is moving from a block-flow to an inline-flow parent then any
    // floats currently intruding into the child can no longer do so. This can
    // happen if a block becomes floating or out-of-flow and is moved to an
    // anonymous block. Remove all floats from their float-lists immediately as
    // markAllDescendantsWithFloatsForLayout won't attempt to remove floats from
    // parents that have inline-flow if we try later.
//     if (child->isLayoutBlockFlow() && toBoxModelObject->childrenInline() &&
//         !childrenInline()) {
//         toLayoutBlockFlow(child)->removeFloatingObjectsFromDescendants();
//         ASSERT(!toLayoutBlockFlow(child)->containsFloats());
//     }

    if (fullRemoveInsert && isLayoutBlock() && child->isBox())
        LayoutBlock::removePercentHeightDescendantIfNeeded(toLayoutBox(child));

    ASSERT(this == child->parent());
    ASSERT(!beforeChild || toBoxModelObject == beforeChild->parent());
    if (fullRemoveInsert && (toBoxModelObject->isLayoutBlock() || toBoxModelObject->isLayoutInline())) {
        // Takes care of adding the new child correctly if toBlock and fromBlock
        // have different kind of children (block vs inline).
        toBoxModelObject->addChild(virtualChildren()->removeChildNode(this, child), beforeChild);
    } else {
        toBoxModelObject->virtualChildren()->insertChildNode(toBoxModelObject, virtualChildren()->removeChildNode(this, child, fullRemoveInsert), beforeChild, fullRemoveInsert);
    }
}

void LayoutBoxModelObject::moveChildrenTo(LayoutBoxModelObject* toBoxModelObject, LayoutObject* startChild, LayoutObject* endChild, LayoutObject* beforeChild, bool fullRemoveInsert)
{
    // This condition is rarely hit since this function is usually called on
    // anonymous blocks which can no longer carry positioned objects (see r120761)
    // or when fullRemoveInsert is false.
    if (fullRemoveInsert && isLayoutBlock()) {
        LayoutBlock* block = toLayoutBlock(this);
        block->removePositionedObjects(nullptr);

        if (isBox())
            LayoutBlock::removePercentHeightDescendantIfNeeded(toLayoutBox(this));
        
        if (block->isLayoutBlockFlow())
            toLayoutBlockFlow(block)->removeFloatingObjects();
    }

    ASSERT(!beforeChild || toBoxModelObject == beforeChild->parent());
    for (LayoutObject* child = startChild; child && child != endChild; ) {
        // Save our next sibling as moveChildTo will clear it.
        LayoutObject* nextSibling = child->nextSibling();
        moveChildTo(toBoxModelObject, child, beforeChild, fullRemoveInsert);
        child = nextSibling;
    }
}

bool LayoutBoxModelObject::backgroundStolenForBeingBody(const ComputedStyle* rootElementStyle) const
{
    // http://www.w3.org/TR/css3-background/#body-background
    // If the root element is <html> with no background, and a <body> child element exists,
    // the root element steals the first <body> child element's background.
    if (!isBody())
        return false;

    Element* rootElement = document().documentElement();
    if (!isHTMLHtmlElement(rootElement))
        return false;

    if (!rootElementStyle)
        rootElementStyle = rootElement->ensureComputedStyle();
    if (rootElementStyle->hasBackground())
        return false;

    if (node() != document().firstBodyElement())
        return false;

    return true;
}

} // namespace blink
