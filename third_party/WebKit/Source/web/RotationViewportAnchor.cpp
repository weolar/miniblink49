// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "web/RotationViewportAnchor.h"

#include "core/dom/ContainerNode.h"
#include "core/dom/Node.h"
#include "core/frame/FrameView.h"
#include "core/frame/LocalFrame.h"
#include "core/frame/PageScaleConstraintsSet.h"
#include "core/frame/PinchViewport.h"
#include "core/input/EventHandler.h"
#include "core/layout/HitTestResult.h"
#include "platform/geometry/DoubleRect.h"

namespace blink {

namespace {

static const float viewportAnchorRelativeEpsilon = 0.1f;
static const int viewportToNodeMaxRelativeArea = 2;

template <typename RectType>
int area(const RectType& rect)
{
    return rect.width() * rect.height();
}

Node* findNonEmptyAnchorNode(const IntPoint& point, const IntRect& viewRect, EventHandler& eventHandler)
{
    Node* node = eventHandler.hitTestResultAtPoint(point, HitTestRequest::ReadOnly | HitTestRequest::Active).innerNode();

    // If the node bounding box is sufficiently large, make a single attempt to
    // find a smaller node; the larger the node bounds, the greater the
    // variability under resize.
    const int maxNodeArea = area(viewRect) * viewportToNodeMaxRelativeArea;
    if (node && area(node->boundingBox()) > maxNodeArea) {
        IntSize pointOffset = viewRect.size();
        pointOffset.scale(viewportAnchorRelativeEpsilon);
        node = eventHandler.hitTestResultAtPoint(point + pointOffset, HitTestRequest::ReadOnly | HitTestRequest::Active).innerNode();
    }

    while (node && node->boundingBox().isEmpty())
        node = node->parentNode();

    return node;
}

void moveToEncloseRect(IntRect& outer, const FloatRect& inner)
{
    IntPoint minimumPosition = ceiledIntPoint(inner.location() + inner.size() - FloatSize(outer.size()));
    IntPoint maximumPosition = flooredIntPoint(inner.location());

    IntPoint outerOrigin = outer.location();
    outerOrigin = outerOrigin.expandedTo(minimumPosition);
    outerOrigin = outerOrigin.shrunkTo(maximumPosition);

    outer.setLocation(outerOrigin);
}

void moveIntoRect(FloatRect& inner, const IntRect& outer)
{
    FloatPoint minimumPosition = FloatPoint(outer.location());
    FloatPoint maximumPosition = minimumPosition + outer.size() - inner.size();

    // Adjust maximumPosition to the nearest lower integer because
    // PinchViewport::maximumScrollPosition() does the same.
    // The value of minumumPosition is already adjusted since it is
    // constructed from an integer point.
    maximumPosition = flooredIntPoint(maximumPosition);

    FloatPoint innerOrigin = inner.location();
    innerOrigin = innerOrigin.expandedTo(minimumPosition);
    innerOrigin = innerOrigin.shrunkTo(maximumPosition);

    inner.setLocation(innerOrigin);
}

} // namespace

RotationViewportAnchor::RotationViewportAnchor(
    FrameView& rootFrameView,
    PinchViewport& pinchViewport,
    const FloatSize& anchorInInnerViewCoords,
    PageScaleConstraintsSet& pageScaleConstraintsSet)
    : ViewportAnchor(rootFrameView, pinchViewport)
    , m_anchorInInnerViewCoords(anchorInInnerViewCoords)
    , m_pageScaleConstraintsSet(pageScaleConstraintsSet)
{
    setAnchor();
}

RotationViewportAnchor::~RotationViewportAnchor()
{
    restoreToAnchor();
}

void RotationViewportAnchor::setAnchor()
{
    // FIXME: Scroll offsets are now fractional (DoublePoint and FloatPoint for the FrameView and PinchViewport
    //        respectively. This path should be rewritten without pixel snapping.
    IntRect outerViewRect = m_rootFrameView->layoutViewportScrollableArea()->visibleContentRect();
    IntRect innerViewRect = enclosedIntRect(m_rootFrameView->scrollableArea()->visibleContentRectDouble());

    m_oldPageScaleFactor = m_pinchViewport->scale();
    m_oldMinimumPageScaleFactor = m_pageScaleConstraintsSet.finalConstraints().minimumScale;

    // Save the absolute location in case we won't find the anchor node, we'll fall back to that.
    m_pinchViewportInDocument = FloatPoint(m_rootFrameView->scrollableArea()->visibleContentRectDouble().location());

    m_anchorNode.clear();
    m_anchorNodeBounds = LayoutRect();
    m_anchorInNodeCoords = FloatSize();
    m_normalizedPinchViewportOffset = FloatSize();

    if (innerViewRect.isEmpty())
        return;

    // Preserve origins at the absolute screen origin
    if (innerViewRect.location() == IntPoint::zero())
        return;

    // Inner rectangle should be within the outer one.
    ASSERT(outerViewRect.contains(innerViewRect));

    // Outer rectangle is used as a scale, we need positive width and height.
    ASSERT(!outerViewRect.isEmpty());

    m_normalizedPinchViewportOffset = innerViewRect.location() - outerViewRect.location();

    // Normalize by the size of the outer rect
    m_normalizedPinchViewportOffset.scale(1.0 / outerViewRect.width(), 1.0 / outerViewRect.height());

    FloatSize anchorOffset = innerViewRect.size();
    anchorOffset.scale(m_anchorInInnerViewCoords.width(), m_anchorInInnerViewCoords.height());
    const FloatPoint anchorPoint = FloatPoint(innerViewRect.location()) + anchorOffset;

    Node* node = findNonEmptyAnchorNode(flooredIntPoint(anchorPoint), innerViewRect, m_rootFrameView->frame().eventHandler());
    if (!node)
        return;

    m_anchorNode = node;
    m_anchorNodeBounds = node->boundingBox();
    m_anchorInNodeCoords = anchorPoint - FloatPoint(m_anchorNodeBounds.location());
    m_anchorInNodeCoords.scale(1.f / m_anchorNodeBounds.width(), 1.f / m_anchorNodeBounds.height());
}

void RotationViewportAnchor::restoreToAnchor()
{
    float newPageScaleFactor = m_oldPageScaleFactor / m_oldMinimumPageScaleFactor * m_pageScaleConstraintsSet.finalConstraints().minimumScale;
    newPageScaleFactor = m_pageScaleConstraintsSet.finalConstraints().clampToConstraints(newPageScaleFactor);

    FloatSize pinchViewportSize = m_pinchViewport->size();
    pinchViewportSize.scale(1 / newPageScaleFactor);

    IntPoint mainFrameOrigin;
    FloatPoint pinchViewportOrigin;

    computeOrigins(pinchViewportSize, mainFrameOrigin, pinchViewportOrigin);

    m_rootFrameView->layoutViewportScrollableArea()->setScrollPosition(mainFrameOrigin, ProgrammaticScroll);

    // Set scale before location, since location can be clamped on setting scale.
    m_pinchViewport->setScale(newPageScaleFactor);
    m_pinchViewport->setLocation(pinchViewportOrigin);
}

void RotationViewportAnchor::computeOrigins(const FloatSize& innerSize, IntPoint& mainFrameOffset, FloatPoint& pinchViewportOffset) const
{
    IntSize outerSize = m_rootFrameView->layoutViewportScrollableArea()->visibleContentRect().size();

    // Compute the viewport origins in CSS pixels relative to the document.
    FloatSize absPinchViewportOffset = m_normalizedPinchViewportOffset;
    absPinchViewportOffset.scale(outerSize.width(), outerSize.height());

    FloatPoint innerOrigin = getInnerOrigin(innerSize);
    FloatPoint outerOrigin = innerOrigin - absPinchViewportOffset;

    IntRect outerRect = IntRect(flooredIntPoint(outerOrigin), outerSize);
    FloatRect innerRect = FloatRect(innerOrigin, innerSize);

    moveToEncloseRect(outerRect, innerRect);

    outerRect.setLocation(m_rootFrameView->layoutViewportScrollableArea()->clampScrollPosition(outerRect.location()));

    moveIntoRect(innerRect, outerRect);

    mainFrameOffset = outerRect.location();
    pinchViewportOffset = FloatPoint(innerRect.location() - outerRect.location());
}

FloatPoint RotationViewportAnchor::getInnerOrigin(const FloatSize& innerSize) const
{
    if (!m_anchorNode || !m_anchorNode->inDocument())
        return m_pinchViewportInDocument;

    const LayoutRect currentNodeBounds = m_anchorNode->boundingBox();
    if (m_anchorNodeBounds == currentNodeBounds)
        return m_pinchViewportInDocument;

    // Compute the new anchor point relative to the node position
    FloatSize anchorOffsetFromNode(currentNodeBounds.size());
    anchorOffsetFromNode.scale(m_anchorInNodeCoords.width(), m_anchorInNodeCoords.height());
    FloatPoint anchorPoint = FloatPoint(currentNodeBounds.location()) + anchorOffsetFromNode;

    // Compute the new origin point relative to the new anchor point
    FloatSize anchorOffsetFromOrigin = innerSize;
    anchorOffsetFromOrigin.scale(m_anchorInInnerViewCoords.width(), m_anchorInInnerViewCoords.height());
    return anchorPoint - anchorOffsetFromOrigin;
}

} // namespace blink
