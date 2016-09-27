/*
 * Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies)
 * Copyright (C) 2009 Antonio Gomes <tonikitoo@webkit.org>
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE COMPUTER, INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "core/page/SpatialNavigation.h"

#include "core/HTMLNames.h"
#include "core/frame/FrameView.h"
#include "core/frame/LocalFrame.h"
#include "core/frame/Settings.h"
#include "core/html/HTMLAreaElement.h"
#include "core/html/HTMLFrameOwnerElement.h"
#include "core/html/HTMLImageElement.h"
#include "core/layout/LayoutBox.h"
#include "core/page/FrameTree.h"
#include "core/page/Page.h"
#include "platform/geometry/IntRect.h"

namespace blink {

using namespace HTMLNames;

static void deflateIfOverlapped(LayoutRect&, LayoutRect&);
static LayoutRect rectToAbsoluteCoordinates(LocalFrame* initialFrame, const LayoutRect&);
static bool isScrollableNode(const Node*);

FocusCandidate::FocusCandidate(Node* node, WebFocusType type)
    : visibleNode(nullptr)
    , focusableNode(nullptr)
    , enclosingScrollableBox(nullptr)
    , distance(maxDistance())
    , isOffscreen(true)
    , isOffscreenAfterScrolling(true)
{
    ASSERT(node);
    ASSERT(node->isElementNode());

    if (isHTMLAreaElement(*node)) {
        HTMLAreaElement& area = toHTMLAreaElement(*node);
        HTMLImageElement* image = area.imageElement();
        if (!image || !image->layoutObject())
            return;

        visibleNode = image;
        rect = virtualRectForAreaElementAndDirection(area, type);
    } else {
        if (!node->layoutObject())
            return;

        visibleNode = node;
        rect = nodeRectInAbsoluteCoordinates(node, true /* ignore border */);
    }

    focusableNode = node;
    isOffscreen = hasOffscreenRect(visibleNode);
    isOffscreenAfterScrolling = hasOffscreenRect(visibleNode, type);
}

bool isSpatialNavigationEnabled(const LocalFrame* frame)
{
    return (frame && frame->settings() && frame->settings()->spatialNavigationEnabled());
}

bool spatialNavigationIgnoresEventHandlers(const LocalFrame* frame)
{
    return (frame && frame->settings() && frame->settings()->deviceSupportsTouch());
}

static bool rectsIntersectOnOrthogonalAxis(WebFocusType type, const LayoutRect& a, const LayoutRect& b)
{
    switch (type) {
    case WebFocusTypeLeft:
    case WebFocusTypeRight:
        return a.maxY() > b.y() && a.y() < b.maxY();
    case WebFocusTypeUp:
    case WebFocusTypeDown:
        return a.maxX() > b.x() && a.x() < b.maxX();
    default:
        ASSERT_NOT_REACHED();
        return false;
    }
}

// Return true if rect |a| is below |b|. False otherwise.
// For overlapping rects, |a| is considered to be below |b|
// if both edges of |a| are below the respective ones of |b|
static inline bool below(const LayoutRect& a, const LayoutRect& b)
{
    return a.y() >= b.maxY()
        || (a.y() >= b.y() && a.maxY() > b.maxY() && a.x() < b.maxX() && a.maxX() > b.x());
}

// Return true if rect |a| is on the right of |b|. False otherwise.
// For overlapping rects, |a| is considered to be on the right of |b|
// if both edges of |a| are on the right of the respective ones of |b|
static inline bool rightOf(const LayoutRect& a, const LayoutRect& b)
{
    return a.x() >= b.maxX()
        || (a.x() >= b.x() && a.maxX() > b.maxX() && a.y() < b.maxY() && a.maxY() > b.y());
}

static bool isRectInDirection(WebFocusType type, const LayoutRect& curRect, const LayoutRect& targetRect)
{
    switch (type) {
    case WebFocusTypeLeft:
        return rightOf(curRect, targetRect);
    case WebFocusTypeRight:
        return rightOf(targetRect, curRect);
    case WebFocusTypeUp:
        return below(curRect, targetRect);
    case WebFocusTypeDown:
        return below(targetRect, curRect);
    default:
        ASSERT_NOT_REACHED();
        return false;
    }
}

// Checks if |node| is offscreen the visible area (viewport) of its container
// document. In case it is, one can scroll in direction or take any different
// desired action later on.
bool hasOffscreenRect(Node* node, WebFocusType type)
{
    // Get the FrameView in which |node| is (which means the current viewport if |node|
    // is not in an inner document), so we can check if its content rect is visible
    // before we actually move the focus to it.
    FrameView* frameView = node->document().view();
    if (!frameView)
        return true;

    ASSERT(!frameView->needsLayout());

    LayoutRect containerViewportRect(frameView->visibleContentRect());
    // We want to select a node if it is currently off screen, but will be
    // exposed after we scroll. Adjust the viewport to post-scrolling position.
    // If the container has overflow:hidden, we cannot scroll, so we do not pass direction
    // and we do not adjust for scrolling.
    switch (type) {
    case WebFocusTypeLeft:
        containerViewportRect.setX(containerViewportRect.x() - ScrollableArea::pixelsPerLineStep());
        containerViewportRect.setWidth(containerViewportRect.width() + ScrollableArea::pixelsPerLineStep());
        break;
    case WebFocusTypeRight:
        containerViewportRect.setWidth(containerViewportRect.width() + ScrollableArea::pixelsPerLineStep());
        break;
    case WebFocusTypeUp:
        containerViewportRect.setY(containerViewportRect.y() - ScrollableArea::pixelsPerLineStep());
        containerViewportRect.setHeight(containerViewportRect.height() + ScrollableArea::pixelsPerLineStep());
        break;
    case WebFocusTypeDown:
        containerViewportRect.setHeight(containerViewportRect.height() + ScrollableArea::pixelsPerLineStep());
        break;
    default:
        break;
    }

    LayoutObject* layoutObject = node->layoutObject();
    if (!layoutObject)
        return true;

    LayoutRect rect(layoutObject->absoluteClippedOverflowRect());
    if (rect.isEmpty())
        return true;

    return !containerViewportRect.intersects(rect);
}

bool scrollInDirection(LocalFrame* frame, WebFocusType type)
{
    ASSERT(frame);

    if (frame && canScrollInDirection(frame->document(), type)) {
        LayoutUnit dx = 0;
        LayoutUnit dy = 0;
        switch (type) {
        case WebFocusTypeLeft:
            dx = - ScrollableArea::pixelsPerLineStep();
            break;
        case WebFocusTypeRight:
            dx = ScrollableArea::pixelsPerLineStep();
            break;
        case WebFocusTypeUp:
            dy = - ScrollableArea::pixelsPerLineStep();
            break;
        case WebFocusTypeDown:
            dy = ScrollableArea::pixelsPerLineStep();
            break;
        default:
            ASSERT_NOT_REACHED();
            return false;
        }

        frame->view()->scrollBy(IntSize(dx, dy), UserScroll);
        return true;
    }
    return false;
}

bool scrollInDirection(Node* container, WebFocusType type)
{
    ASSERT(container);
    if (container->isDocumentNode())
        return scrollInDirection(toDocument(container)->frame(), type);

    if (!container->layoutBox())
        return false;

    if (canScrollInDirection(container, type)) {
        LayoutUnit dx = 0;
        LayoutUnit dy = 0;
        switch (type) {
        case WebFocusTypeLeft:
            dx = - std::min<LayoutUnit>(ScrollableArea::pixelsPerLineStep(), container->layoutBox()->scrollLeft());
            break;
        case WebFocusTypeRight:
            ASSERT(container->layoutBox()->scrollWidth() > (container->layoutBox()->scrollLeft() + container->layoutBox()->clientWidth()));
            dx = std::min<LayoutUnit>(ScrollableArea::pixelsPerLineStep(), container->layoutBox()->scrollWidth() - (container->layoutBox()->scrollLeft() + container->layoutBox()->clientWidth()));
            break;
        case WebFocusTypeUp:
            dy = - std::min<LayoutUnit>(ScrollableArea::pixelsPerLineStep(), container->layoutBox()->scrollTop());
            break;
        case WebFocusTypeDown:
            ASSERT(container->layoutBox()->scrollHeight() - (container->layoutBox()->scrollTop() + container->layoutBox()->clientHeight()));
            dy = std::min<LayoutUnit>(ScrollableArea::pixelsPerLineStep(), container->layoutBox()->scrollHeight() - (container->layoutBox()->scrollTop() + container->layoutBox()->clientHeight()));
            break;
        default:
            ASSERT_NOT_REACHED();
            return false;
        }

        container->layoutBox()->scrollByRecursively(IntSize(dx, dy));
        return true;
    }

    return false;
}

static void deflateIfOverlapped(LayoutRect& a, LayoutRect& b)
{
    if (!a.intersects(b) || a.contains(b) || b.contains(a))
        return;

    LayoutUnit deflateFactor = -fudgeFactor();

    // Avoid negative width or height values.
    if ((a.width() + 2 * deflateFactor > 0) && (a.height() + 2 * deflateFactor > 0))
        a.inflate(deflateFactor);

    if ((b.width() + 2 * deflateFactor > 0) && (b.height() + 2 * deflateFactor > 0))
        b.inflate(deflateFactor);
}

bool isScrollableNode(const Node* node)
{
    ASSERT(!node->isDocumentNode());

    if (!node)
        return false;

    if (LayoutObject* layoutObject = node->layoutObject())
        return layoutObject->isBox() && toLayoutBox(layoutObject)->canBeScrolledAndHasScrollableArea() && node->hasChildren();

    return false;
}

Node* scrollableEnclosingBoxOrParentFrameForNodeInDirection(WebFocusType type, Node* node)
{
    ASSERT(node);
    Node* parent = node;
    do {
        // FIXME: Spatial navigation is broken for OOPI.
        if (parent->isDocumentNode())
            parent = toDocument(parent)->frame()->deprecatedLocalOwner();
        else
            parent = parent->parentOrShadowHostNode();
    } while (parent && !canScrollInDirection(parent, type) && !parent->isDocumentNode());

    return parent;
}

bool canScrollInDirection(const Node* container, WebFocusType type)
{
    ASSERT(container);
    if (container->isDocumentNode())
        return canScrollInDirection(toDocument(container)->frame(), type);

    if (!isScrollableNode(container))
        return false;

    switch (type) {
    case WebFocusTypeLeft:
        return (container->layoutObject()->style()->overflowX() != OHIDDEN && container->layoutBox()->scrollLeft() > 0);
    case WebFocusTypeUp:
        return (container->layoutObject()->style()->overflowY() != OHIDDEN && container->layoutBox()->scrollTop() > 0);
    case WebFocusTypeRight:
        return (container->layoutObject()->style()->overflowX() != OHIDDEN && container->layoutBox()->scrollLeft() + container->layoutBox()->clientWidth() < container->layoutBox()->scrollWidth());
    case WebFocusTypeDown:
        return (container->layoutObject()->style()->overflowY() != OHIDDEN && container->layoutBox()->scrollTop() + container->layoutBox()->clientHeight() < container->layoutBox()->scrollHeight());
    default:
        ASSERT_NOT_REACHED();
        return false;
    }
}

bool canScrollInDirection(const LocalFrame* frame, WebFocusType type)
{
    if (!frame->view())
        return false;
    ScrollbarMode verticalMode;
    ScrollbarMode horizontalMode;
    frame->view()->calculateScrollbarModesForLayout(horizontalMode, verticalMode);
    if ((type == WebFocusTypeLeft || type == WebFocusTypeRight) && ScrollbarAlwaysOff == horizontalMode)
        return false;
    if ((type == WebFocusTypeUp || type == WebFocusTypeDown) &&  ScrollbarAlwaysOff == verticalMode)
        return false;
    LayoutSize size(frame->view()->contentsSize());
    LayoutSize offset(frame->view()->scrollOffset());
    LayoutRect rect(frame->view()->visibleContentRect(IncludeScrollbars));

    switch (type) {
    case WebFocusTypeLeft:
        return offset.width() > 0;
    case WebFocusTypeUp:
        return offset.height() > 0;
    case WebFocusTypeRight:
        return rect.width() + offset.width() < size.width();
    case WebFocusTypeDown:
        return rect.height() + offset.height() < size.height();
    default:
        ASSERT_NOT_REACHED();
        return false;
    }
}

static LayoutRect rectToAbsoluteCoordinates(LocalFrame* initialFrame, const LayoutRect& initialRect)
{
    LayoutRect rect = initialRect;
    for (Frame* frame = initialFrame; frame; frame = frame->tree().parent()) {
        if (!frame->isLocalFrame())
            continue;
        // FIXME: Spatial navigation is broken for OOPI.
        Element* element = frame->deprecatedLocalOwner();
        if (element) {
            do {
                rect.move(element->offsetLeft(), element->offsetTop());
                LayoutObject* layoutObject = element->layoutObject();
                element = layoutObject ? layoutObject->offsetParent() : nullptr;
            } while (element);
            rect.move((-toLocalFrame(frame)->view()->scrollOffset()));
        }
    }
    return rect;
}

LayoutRect nodeRectInAbsoluteCoordinates(Node* node, bool ignoreBorder)
{
    ASSERT(node && node->layoutObject() && !node->document().view()->needsLayout());

    if (node->isDocumentNode())
        return frameRectInAbsoluteCoordinates(toDocument(node)->frame());
    LayoutRect rect = rectToAbsoluteCoordinates(node->document().frame(), node->boundingBox());

    // For authors that use border instead of outline in their CSS, we compensate by ignoring the border when calculating
    // the rect of the focused element.
    if (ignoreBorder) {
        rect.move(node->layoutObject()->style()->borderLeftWidth(), node->layoutObject()->style()->borderTopWidth());
        rect.setWidth(rect.width() - node->layoutObject()->style()->borderLeftWidth() - node->layoutObject()->style()->borderRightWidth());
        rect.setHeight(rect.height() - node->layoutObject()->style()->borderTopWidth() - node->layoutObject()->style()->borderBottomWidth());
    }
    return rect;
}

LayoutRect frameRectInAbsoluteCoordinates(LocalFrame* frame)
{
    return rectToAbsoluteCoordinates(frame, LayoutRect(frame->view()->visibleContentRect()));
}

// This method calculates the exitPoint from the startingRect and the entryPoint into the candidate rect.
// The line between those 2 points is the closest distance between the 2 rects.
// Takes care of overlapping rects, defining points so that the distance between them
// is zero where necessary
void entryAndExitPointsForDirection(WebFocusType type, const LayoutRect& startingRect, const LayoutRect& potentialRect, LayoutPoint& exitPoint, LayoutPoint& entryPoint)
{
    switch (type) {
    case WebFocusTypeLeft:
        exitPoint.setX(startingRect.x());
        if (potentialRect.maxX() < startingRect.x())
            entryPoint.setX(potentialRect.maxX());
        else
            entryPoint.setX(startingRect.x());
        break;
    case WebFocusTypeUp:
        exitPoint.setY(startingRect.y());
        if (potentialRect.maxY() < startingRect.y())
            entryPoint.setY(potentialRect.maxY());
        else
            entryPoint.setY(startingRect.y());
        break;
    case WebFocusTypeRight:
        exitPoint.setX(startingRect.maxX());
        if (potentialRect.x() > startingRect.maxX())
            entryPoint.setX(potentialRect.x());
        else
            entryPoint.setX(startingRect.maxX());
        break;
    case WebFocusTypeDown:
        exitPoint.setY(startingRect.maxY());
        if (potentialRect.y() > startingRect.maxY())
            entryPoint.setY(potentialRect.y());
        else
            entryPoint.setY(startingRect.maxY());
        break;
    default:
        ASSERT_NOT_REACHED();
    }

    switch (type) {
    case WebFocusTypeLeft:
    case WebFocusTypeRight:
        if (below(startingRect, potentialRect)) {
            exitPoint.setY(startingRect.y());
            if (potentialRect.maxY() < startingRect.y())
                entryPoint.setY(potentialRect.maxY());
            else
                entryPoint.setY(startingRect.y());
        } else if (below(potentialRect, startingRect)) {
            exitPoint.setY(startingRect.maxY());
            if (potentialRect.y() > startingRect.maxY())
                entryPoint.setY(potentialRect.y());
            else
                entryPoint.setY(startingRect.maxY());
        } else {
            exitPoint.setY(max(startingRect.y(), potentialRect.y()));
            entryPoint.setY(exitPoint.y());
        }
        break;
    case WebFocusTypeUp:
    case WebFocusTypeDown:
        if (rightOf(startingRect, potentialRect)) {
            exitPoint.setX(startingRect.x());
            if (potentialRect.maxX() < startingRect.x())
                entryPoint.setX(potentialRect.maxX());
            else
                entryPoint.setX(startingRect.x());
        } else if (rightOf(potentialRect, startingRect)) {
            exitPoint.setX(startingRect.maxX());
            if (potentialRect.x() > startingRect.maxX())
                entryPoint.setX(potentialRect.x());
            else
                entryPoint.setX(startingRect.maxX());
        } else {
            exitPoint.setX(max(startingRect.x(), potentialRect.x()));
            entryPoint.setX(exitPoint.x());
        }
        break;
    default:
        ASSERT_NOT_REACHED();
    }
}

bool areElementsOnSameLine(const FocusCandidate& firstCandidate, const FocusCandidate& secondCandidate)
{
    if (firstCandidate.isNull() || secondCandidate.isNull())
        return false;

    if (!firstCandidate.visibleNode->layoutObject() || !secondCandidate.visibleNode->layoutObject())
        return false;

    if (!firstCandidate.rect.intersects(secondCandidate.rect))
        return false;

    if (isHTMLAreaElement(*firstCandidate.focusableNode) || isHTMLAreaElement(*secondCandidate.focusableNode))
        return false;

    if (!firstCandidate.visibleNode->layoutObject()->isLayoutInline() || !secondCandidate.visibleNode->layoutObject()->isLayoutInline())
        return false;

    if (firstCandidate.visibleNode->layoutObject()->containingBlock() != secondCandidate.visibleNode->layoutObject()->containingBlock())
        return false;

    return true;
}

void distanceDataForNode(WebFocusType type, const FocusCandidate& current, FocusCandidate& candidate)
{
    if (!isRectInDirection(type, current.rect, candidate.rect))
        return;

    if (areElementsOnSameLine(current, candidate)) {
        if ((type == WebFocusTypeUp && current.rect.y() > candidate.rect.y()) || (type == WebFocusTypeDown && candidate.rect.y() > current.rect.y())) {
            candidate.distance = 0;
            return;
        }
    }

    LayoutRect nodeRect = candidate.rect;
    LayoutRect currentRect = current.rect;
    deflateIfOverlapped(currentRect, nodeRect);

    LayoutPoint exitPoint;
    LayoutPoint entryPoint;
    entryAndExitPointsForDirection(type, currentRect, nodeRect, exitPoint, entryPoint);

    LayoutUnit xAxis = (exitPoint.x() - entryPoint.x()).abs();
    LayoutUnit yAxis = (exitPoint.y() - entryPoint.y()).abs();

    LayoutUnit navigationAxisDistance;
    LayoutUnit weightedOrthogonalAxisDistance;

    // Bias and weights are put to the orthogonal axis distance calculation
    // so aligned candidates would have advantage over partially-aligned ones
    // and then over not-aligned candidates. The bias is given to not-aligned
    // candidates with respect to size of the current rect. The weight for
    // left/right direction is given a higher value to allow navigation on
    // common horizonally-aligned elements. The hardcoded values are based on
    // tests and experiments.
    const int orthogonalWeightForLeftRight = 30;
    const int orthogonalWeightForUpDown = 2;
    int orthogonalBias = 0;

    switch (type) {
    case WebFocusTypeLeft:
    case WebFocusTypeRight:
        navigationAxisDistance = xAxis;
        if (!rectsIntersectOnOrthogonalAxis(type, currentRect, nodeRect))
            orthogonalBias = currentRect.height() / 2;
        weightedOrthogonalAxisDistance = (yAxis + orthogonalBias) * orthogonalWeightForLeftRight;
        break;
    case WebFocusTypeUp:
    case WebFocusTypeDown:
        navigationAxisDistance = yAxis;
        if (!rectsIntersectOnOrthogonalAxis(type, currentRect, nodeRect))
            orthogonalBias = currentRect.width() / 2;
        weightedOrthogonalAxisDistance = (xAxis + orthogonalBias) * orthogonalWeightForUpDown;
        break;
    default:
        ASSERT_NOT_REACHED();
        return;
    }

    double euclidianDistancePow2 = (xAxis * xAxis + yAxis * yAxis).toDouble();
    LayoutRect intersectionRect = intersection(currentRect, nodeRect);
    double overlap = (intersectionRect.width() * intersectionRect.height()).toDouble();

    // Distance calculation is based on http://www.w3.org/TR/WICD/#focus-handling
    candidate.distance = sqrt(euclidianDistancePow2) + navigationAxisDistance + weightedOrthogonalAxisDistance - sqrt(overlap);
}

bool canBeScrolledIntoView(WebFocusType type, const FocusCandidate& candidate)
{
    ASSERT(candidate.visibleNode && candidate.isOffscreen);
    LayoutRect candidateRect = candidate.rect;
    for (Node* parentNode = candidate.visibleNode->parentNode(); parentNode; parentNode = parentNode->parentNode()) {
        LayoutRect parentRect = nodeRectInAbsoluteCoordinates(parentNode);
        if (!candidateRect.intersects(parentRect)) {
            if (((type == WebFocusTypeLeft || type == WebFocusTypeRight) && parentNode->layoutObject()->style()->overflowX() == OHIDDEN)
                || ((type == WebFocusTypeUp || type == WebFocusTypeDown) && parentNode->layoutObject()->style()->overflowY() == OHIDDEN))
                return false;
        }
        if (parentNode == candidate.enclosingScrollableBox)
            return canScrollInDirection(parentNode, type);
    }
    return true;
}

// The starting rect is the rect of the focused node, in document coordinates.
// Compose a virtual starting rect if there is no focused node or if it is off screen.
// The virtual rect is the edge of the container or frame. We select which
// edge depending on the direction of the navigation.
LayoutRect virtualRectForDirection(WebFocusType type, const LayoutRect& startingRect, LayoutUnit width)
{
    LayoutRect virtualStartingRect = startingRect;
    switch (type) {
    case WebFocusTypeLeft:
        virtualStartingRect.setX(virtualStartingRect.maxX() - width);
        virtualStartingRect.setWidth(width);
        break;
    case WebFocusTypeUp:
        virtualStartingRect.setY(virtualStartingRect.maxY() - width);
        virtualStartingRect.setHeight(width);
        break;
    case WebFocusTypeRight:
        virtualStartingRect.setWidth(width);
        break;
    case WebFocusTypeDown:
        virtualStartingRect.setHeight(width);
        break;
    default:
        ASSERT_NOT_REACHED();
    }

    return virtualStartingRect;
}

LayoutRect virtualRectForAreaElementAndDirection(HTMLAreaElement& area, WebFocusType type)
{
    ASSERT(area.imageElement());
    // Area elements tend to overlap more than other focusable elements. We flatten the rect of the area elements
    // to minimize the effect of overlapping areas.
    LayoutRect rect = virtualRectForDirection(type, rectToAbsoluteCoordinates(area.document().frame(), area.computeRect(area.imageElement()->layoutObject())), 1);
    return rect;
}

HTMLFrameOwnerElement* frameOwnerElement(FocusCandidate& candidate)
{
    return candidate.isFrameOwnerElement() ? toHTMLFrameOwnerElement(candidate.visibleNode) : nullptr;
};

} // namespace blink
