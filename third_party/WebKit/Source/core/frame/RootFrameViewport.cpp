// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/frame/RootFrameViewport.h"

#include "core/frame/FrameView.h"
#include "core/layout/ScrollAlignment.h"
#include "platform/geometry/DoubleRect.h"
#include "platform/geometry/FloatRect.h"
#include "platform/geometry/LayoutRect.h"

namespace blink {

RootFrameViewport::RootFrameViewport(ScrollableArea& visualViewport, ScrollableArea& layoutViewport)
    : m_visualViewport(visualViewport)
    , m_layoutViewport(layoutViewport)
{
}

void RootFrameViewport::updateScrollAnimator()
{
    scrollAnimator()->setCurrentPosition(toFloatPoint(scrollOffsetFromScrollAnimators()));
}

DoublePoint RootFrameViewport::scrollOffsetFromScrollAnimators() const
{
    return visualViewport().scrollAnimator()->currentPosition() + layoutViewport().scrollAnimator()->currentPosition();
}

DoubleRect RootFrameViewport::visibleContentRectDouble(IncludeScrollbarsInRect scrollbarInclusion) const
{
    return DoubleRect(scrollPositionDouble(), visualViewport().visibleContentRectDouble(scrollbarInclusion).size());
}

IntRect RootFrameViewport::visibleContentRect(IncludeScrollbarsInRect scrollbarInclusion) const
{
    return enclosingIntRect(visibleContentRectDouble(scrollbarInclusion));
}

bool RootFrameViewport::shouldUseIntegerScrollOffset() const
{
    // Fractionals are floored in the ScrollAnimator but it's important that the ScrollAnimators of the
    // visual and layout viewports get the precise fractional number so never use integer scrolling for
    // RootFrameViewport, we'll let the truncation happen in the subviewports.
    return false;
}

bool RootFrameViewport::isActive() const
{
    return layoutViewport().isActive();
}

int RootFrameViewport::scrollSize(ScrollbarOrientation orientation) const
{
    IntSize scrollDimensions = maximumScrollPosition() - minimumScrollPosition();
    return (orientation == HorizontalScrollbar) ? scrollDimensions.width() : scrollDimensions.height();
}

bool RootFrameViewport::isScrollCornerVisible() const
{
    return layoutViewport().isScrollCornerVisible();
}

IntRect RootFrameViewport::scrollCornerRect() const
{
    return layoutViewport().scrollCornerRect();
}

void RootFrameViewport::setScrollPosition(const DoublePoint& position, ScrollType scrollType, ScrollBehavior scrollBehavior)
{
    updateScrollAnimator();

    // TODO(bokan): Support smooth scrolling the visual viewport.
    if (scrollBehavior == ScrollBehaviorAuto)
        scrollBehavior = scrollBehaviorStyle();
    if (scrollBehavior == ScrollBehaviorSmooth) {
        layoutViewport().setScrollPosition(position, scrollType, scrollBehavior);
        return;
    }

    if (scrollType == ProgrammaticScroll && !layoutViewport().isProgrammaticallyScrollable())
        return;

    DoublePoint clampedPosition = clampScrollPosition(position);
    ScrollableArea::setScrollPosition(clampedPosition, scrollType, scrollBehavior);
}

ScrollBehavior RootFrameViewport::scrollBehaviorStyle() const
{
    return layoutViewport().scrollBehaviorStyle();
}

ScrollResult RootFrameViewport::handleWheel(const PlatformWheelEvent& event)
{
    updateScrollAnimator();

    ScrollResult viewScrollResult = layoutViewport().handleWheel(event);

    // The visual viewport will only accept pixel scrolls.
    if (!event.canScroll() || event.granularity() == ScrollByPageWheelEvent)
        return viewScrollResult;

    // TODO(sataya.m) : The delta in PlatformWheelEvent is negative when scrolling the
    // wheel towards the user, so negate it to get the scroll delta that should be applied
    // to the page. unusedScrollDelta computed in the ScrollResult is also negative. Say
    // there is WheelEvent({0, -10} and page scroll by 2px and unusedScrollDelta computed
    // is {0, -8}. Due to which we have to negate the unusedScrollDelta to obtain the expected
    // animation.Please address http://crbug.com/504389.
    DoublePoint oldOffset = visualViewport().scrollPositionDouble();
    DoublePoint locationDelta;
    if (viewScrollResult.didScroll()) {
        locationDelta = -DoublePoint(viewScrollResult.unusedScrollDeltaX, viewScrollResult.unusedScrollDeltaY);
    } else {
        if (event.railsMode() != PlatformEvent::RailsModeVertical)
            locationDelta.setX(-event.deltaX());
        if (event.railsMode() != PlatformEvent::RailsModeHorizontal)
            locationDelta.setY(-event.deltaY());
    }

    DoublePoint targetPosition = visualViewport().clampScrollPosition(
        visualViewport().scrollPositionDouble() + toDoubleSize(locationDelta));
    visualViewport().setScrollPosition(targetPosition, UserScroll);

    DoublePoint usedLocationDelta(visualViewport().scrollPositionDouble() - oldOffset);

    bool didScrollX = viewScrollResult.didScrollX || usedLocationDelta.x();
    bool didScrollY = viewScrollResult.didScrollY || usedLocationDelta.y();
    return ScrollResult(didScrollX, didScrollY, -viewScrollResult.unusedScrollDeltaX - usedLocationDelta.x(), -viewScrollResult.unusedScrollDeltaY - usedLocationDelta.y());
}

LayoutRect RootFrameViewport::scrollIntoView(const LayoutRect& rectInContent, const ScrollAlignment& alignX, const ScrollAlignment& alignY)
{
    // We want to move the rect into the viewport that excludes the scrollbars so we intersect
    // the pinch viewport with the scrollbar-excluded frameView content rect. However, we don't
    // use visibleContentRect directly since it floors the scroll position. Instead, we use
    // FrameView::scrollPositionDouble and construct a LayoutRect from that (the FrameView size
    // is always integer sized.

    LayoutRect frameRectInContent = LayoutRect(
        layoutViewport().scrollPositionDouble(),
        layoutViewport().visibleContentRect().size());
    LayoutRect pinchRectInContent = LayoutRect(
        layoutViewport().scrollPositionDouble() + toDoubleSize(visualViewport().scrollPositionDouble()),
        visualViewport().visibleContentRect().size());

    LayoutRect viewRectInContent = intersection(pinchRectInContent, frameRectInContent);
    LayoutRect targetViewport =
        ScrollAlignment::getRectToExpose(viewRectInContent, rectInContent, alignX, alignY);

    // pinchViewport.scrollIntoView will attempt to center the given rect within the viewport
    // so to prevent it from adjusting r's coordinates the rect must match the viewport's size
    // i.e. add the subtracted scrollbars from above back in.
    // FIXME: This is hacky and required because getRectToExpose doesn't naturally account
    // for the two viewports. crbug.com/449340.
    targetViewport.setSize(LayoutSize(visualViewport().visibleContentRect().size()));

    // Snap the visible rect to layout units to match the calculated target viewport rect.
    FloatRect visible =
        LayoutRect(visualViewport().scrollPositionDouble(), visualViewport().visibleContentRect().size());

    float centeringOffsetX = (visible.width() - targetViewport.width()) / 2;
    float centeringOffsetY = (visible.height() - targetViewport.height()) / 2;

    DoublePoint targetOffset(
        targetViewport.x() - centeringOffsetX,
        targetViewport.y() - centeringOffsetY);

    setScrollPosition(targetOffset, ProgrammaticScroll);

    // RootFrameViewport only changes the viewport relative to the document so we can't change the input
    // rect's location relative to the document origin.
    return rectInContent;
}

void RootFrameViewport::setScrollOffset(const IntPoint& offset, ScrollType scrollType)
{
    setScrollOffset(DoublePoint(offset), scrollType);
}

void RootFrameViewport::setScrollOffset(const DoublePoint& offset, ScrollType scrollType)
{
    // Make sure we use the scroll positions as reported by each viewport's ScrollAnimator, since its
    // ScrollableArea's position may have the fractional part truncated off.
    DoublePoint oldPosition = scrollOffsetFromScrollAnimators();

    DoubleSize delta = offset - oldPosition;

    if (delta.isZero())
        return;

    DoublePoint targetPosition = layoutViewport().clampScrollPosition(layoutViewport().scrollAnimator()->currentPosition() + delta);
    layoutViewport().setScrollPosition(targetPosition, scrollType);

    DoubleSize applied = scrollOffsetFromScrollAnimators() - oldPosition;
    delta -= applied;

    if (delta.isZero())
        return;

    targetPosition = visualViewport().clampScrollPosition(visualViewport().scrollAnimator()->currentPosition() + delta);
    visualViewport().setScrollPosition(targetPosition, scrollType);
}

IntPoint RootFrameViewport::scrollPosition() const
{
    return flooredIntPoint(scrollPositionDouble());
}

DoublePoint RootFrameViewport::scrollPositionDouble() const
{
    return layoutViewport().scrollPositionDouble() + toDoubleSize(visualViewport().scrollPositionDouble());
}

IntPoint RootFrameViewport::minimumScrollPosition() const
{
    return IntPoint(layoutViewport().minimumScrollPosition() - visualViewport().minimumScrollPosition());
}

IntPoint RootFrameViewport::maximumScrollPosition() const
{
    return layoutViewport().maximumScrollPosition() + visualViewport().maximumScrollPosition();
}

DoublePoint RootFrameViewport::maximumScrollPositionDouble() const
{
    return layoutViewport().maximumScrollPositionDouble() + toDoubleSize(visualViewport().maximumScrollPositionDouble());
}

IntSize RootFrameViewport::contentsSize() const
{
    return layoutViewport().contentsSize();
}

bool RootFrameViewport::scrollbarsCanBeActive() const
{
    return layoutViewport().scrollbarsCanBeActive();
}

IntRect RootFrameViewport::scrollableAreaBoundingBox() const
{
    return layoutViewport().scrollableAreaBoundingBox();
}

bool RootFrameViewport::userInputScrollable(ScrollbarOrientation orientation) const
{
    return visualViewport().userInputScrollable(orientation) || layoutViewport().userInputScrollable(orientation);
}

bool RootFrameViewport::shouldPlaceVerticalScrollbarOnLeft() const
{
    return layoutViewport().shouldPlaceVerticalScrollbarOnLeft();
}

void RootFrameViewport::invalidateScrollbarRect(Scrollbar* scrollbar, const IntRect& rect)
{
    layoutViewport().invalidateScrollbarRect(scrollbar, rect);
}

void RootFrameViewport::invalidateScrollCornerRect(const IntRect& rect)
{
    layoutViewport().invalidateScrollCornerRect(rect);
}

GraphicsLayer* RootFrameViewport::layerForContainer() const
{
    return layoutViewport().layerForContainer();
}

GraphicsLayer* RootFrameViewport::layerForScrolling() const
{
    return layoutViewport().layerForScrolling();
}

GraphicsLayer* RootFrameViewport::layerForHorizontalScrollbar() const
{
    return layoutViewport().layerForHorizontalScrollbar();
}

GraphicsLayer* RootFrameViewport::layerForVerticalScrollbar() const
{
    return layoutViewport().layerForVerticalScrollbar();
}

ScrollResultOneDimensional RootFrameViewport::userScroll(ScrollDirectionPhysical direction, ScrollGranularity granularity, float delta)
{
    updateScrollAnimator();

    ScrollbarOrientation orientation;

    if (direction == ScrollUp || direction == ScrollDown)
        orientation = VerticalScrollbar;
    else
        orientation = HorizontalScrollbar;

    if (layoutViewport().userInputScrollable(orientation) && visualViewport().userInputScrollable(orientation))
        return ScrollableArea::userScroll(direction, granularity, delta);

    if (visualViewport().userInputScrollable(orientation))
        return visualViewport().userScroll(direction, granularity, delta);

    if (layoutViewport().userInputScrollable(orientation))
        return layoutViewport().userScroll(direction, granularity, delta);

    return ScrollResultOneDimensional(false, delta);
}

bool RootFrameViewport::scrollAnimatorEnabled() const
{
    return layoutViewport().scrollAnimatorEnabled();
}

HostWindow* RootFrameViewport::hostWindow() const
{
    return layoutViewport().hostWindow();
}

void RootFrameViewport::serviceScrollAnimations(double monotonicTime)
{
    ScrollableArea::serviceScrollAnimations(monotonicTime);
    layoutViewport().serviceScrollAnimations(monotonicTime);
}

void RootFrameViewport::updateCompositorScrollAnimations()
{
    ScrollableArea::updateCompositorScrollAnimations();
    layoutViewport().updateCompositorScrollAnimations();
}

DEFINE_TRACE(RootFrameViewport)
{
    visitor->trace(m_visualViewport);
    visitor->trace(m_layoutViewport);
    ScrollableArea::trace(visitor);
}

} // namespace blink
