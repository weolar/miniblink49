/*
 * Copyright (c) 2010, Google Inc. All rights reserved.
 * Copyright (C) 2008, 2011 Apple Inc. All Rights Reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "platform/scroll/ScrollableArea.h"

#include "platform/HostWindow.h"
#include "platform/Logging.h"
#include "platform/geometry/DoubleRect.h"
#include "platform/geometry/FloatPoint.h"
#include "platform/geometry/LayoutRect.h"
#include "platform/graphics/GraphicsLayer.h"
#include "platform/scroll/ProgrammaticScrollAnimator.h"
#include "platform/scroll/ScrollbarTheme.h"
#include "wtf/PassOwnPtr.h"

#include "platform/TraceEvent.h"

static const int kPixelsPerLineStep = 40;
static const float kMinFractionToStepWhenPaging = 0.875f;

namespace blink {

struct SameSizeAsScrollableArea {
    virtual ~SameSizeAsScrollableArea();
    IntRect scrollbarDamage[2];
#if ENABLE(ASSERT) && ENABLE(OILPAN)
    VerifyEagerFinalization verifyEager;
#endif
    void* pointer;
    unsigned bitfields : 16;
    IntPoint origin;
};

static_assert(sizeof(ScrollableArea) == sizeof(SameSizeAsScrollableArea), "ScrollableArea should stay small");

int ScrollableArea::pixelsPerLineStep()
{
    return kPixelsPerLineStep;
}

float ScrollableArea::minFractionToStepWhenPaging()
{
    return kMinFractionToStepWhenPaging;
}

int ScrollableArea::maxOverlapBetweenPages()
{
    static int maxOverlapBetweenPages = ScrollbarTheme::theme()->maxOverlapBetweenPages();
    return maxOverlapBetweenPages;
}

ScrollableArea::ScrollableArea()
    : m_inLiveResize(false)
    , m_scrollbarOverlayStyle(ScrollbarOverlayStyleDefault)
    , m_scrollOriginChanged(false)
{
}

ScrollableArea::~ScrollableArea()
{
}

void ScrollableArea::clearScrollAnimators()
{
    m_animators.clear();
}

ScrollAnimator* ScrollableArea::scrollAnimator() const
{
    if (!m_animators)
        m_animators = adoptPtr(new ScrollableAreaAnimators);

    if (!m_animators->scrollAnimator)
        m_animators->scrollAnimator = ScrollAnimator::create(const_cast<ScrollableArea*>(this));

    return m_animators->scrollAnimator.get();
}

ProgrammaticScrollAnimator* ScrollableArea::programmaticScrollAnimator() const
{
    if (!m_animators)
        m_animators = adoptPtr(new ScrollableAreaAnimators);

    if (!m_animators->programmaticScrollAnimator)
        m_animators->programmaticScrollAnimator = ProgrammaticScrollAnimator::create(const_cast<ScrollableArea*>(this));

    return m_animators->programmaticScrollAnimator.get();
}

void ScrollableArea::setScrollOrigin(const IntPoint& origin)
{
    if (m_scrollOrigin != origin) {
        m_scrollOrigin = origin;
        m_scrollOriginChanged = true;
    }
}

GraphicsLayer* ScrollableArea::layerForContainer() const
{
    return layerForScrolling() ? layerForScrolling()->parent() : 0;
}

ScrollResultOneDimensional ScrollableArea::userScroll(ScrollDirectionPhysical direction, ScrollGranularity granularity, float delta)
{
    ScrollbarOrientation orientation;
    if (direction == ScrollUp || direction == ScrollDown)
        orientation = VerticalScrollbar;
    else
        orientation = HorizontalScrollbar;

    if (!userInputScrollable(orientation))
        return ScrollResultOneDimensional(false, delta);

    cancelProgrammaticScrollAnimation();

    float step = 0;
    switch (granularity) {
    case ScrollByLine:
        step = lineStep(orientation);
        break;
    case ScrollByPage:
        step = pageStep(orientation);
        break;
    case ScrollByDocument:
        step = documentStep(orientation);
        break;
    case ScrollByPixel:
    case ScrollByPrecisePixel:
        step = pixelStep(orientation);
        break;
    }

    if (direction == ScrollUp || direction == ScrollLeft)
        delta = -delta;

    return scrollAnimator()->userScroll(orientation, granularity, step, delta);
}

void ScrollableArea::setScrollPosition(const DoublePoint& position, ScrollType scrollType, ScrollBehavior behavior)
{
    if (behavior == ScrollBehaviorAuto)
        behavior = scrollBehaviorStyle();

    if (scrollType == CompositorScroll)
        scrollPositionChanged(clampScrollPosition(position), CompositorScroll);
    else if (scrollType == ProgrammaticScroll)
        programmaticScrollHelper(position, behavior);
    else if (scrollType == UserScroll)
        userScrollHelper(position, behavior);
    else
        ASSERT_NOT_REACHED();
}

void ScrollableArea::scrollBy(const DoubleSize& delta, ScrollType type, ScrollBehavior behavior)
{
    setScrollPosition(scrollPositionDouble() + delta, type, behavior);
}

void ScrollableArea::setScrollPositionSingleAxis(ScrollbarOrientation orientation, double position, ScrollType scrollType, ScrollBehavior behavior)
{
    DoublePoint newPosition;
    if (orientation == HorizontalScrollbar)
        newPosition = DoublePoint(position, scrollAnimator()->currentPosition().y());
    else
        newPosition = DoublePoint(scrollAnimator()->currentPosition().x(), position);

    // TODO(bokan): Note, this doesn't use the derived class versions since this method is currently used
    // exclusively by code that adjusts the position by the scroll origin and the derived class versions
    // differ on whether they take that into account or not.
    ScrollableArea::setScrollPosition(newPosition, scrollType, behavior);
}

void ScrollableArea::programmaticScrollHelper(const DoublePoint& position, ScrollBehavior scrollBehavior)
{
    cancelScrollAnimation();

    if (scrollBehavior == ScrollBehaviorSmooth)
        programmaticScrollAnimator()->animateToOffset(toFloatPoint(position));
    else
        programmaticScrollAnimator()->scrollToOffsetWithoutAnimation(toFloatPoint(position));
}

void ScrollableArea::userScrollHelper(const DoublePoint& position, ScrollBehavior scrollBehavior)
{
    cancelProgrammaticScrollAnimation();

    // Smooth user scrolls (keyboard, wheel clicks) are handled via the userScroll method.
    // TODO(bokan): The userScroll method should probably be modified to call this method
    //              and ScrollAnimator to have a simpler animateToOffset method like the
    //              ProgrammaticScrollAnimator.
    ASSERT(scrollBehavior == ScrollBehaviorInstant);
    scrollAnimator()->scrollToOffsetWithoutAnimation(toFloatPoint(position));
}

void ScrollableArea::scrollIntoRect(const LayoutRect& rectInContent, const FloatRect& targetRectInFrame)
{
    // Use |pixelSnappedIntRect| for rounding to pixel as opposed to |enclosingIntRect|. It gives a better
    // combined (location and size) rounding error resulting in a more accurate scroll offset.
    // FIXME: It would probably be best to do the whole calculation in LayoutUnits but contentsToRootFrame
    // and friends don't have LayoutRect/Point versions yet.
    IntRect boundsInContent = pixelSnappedIntRect(rectInContent);
    IntRect boundsInFrame(boundsInContent.location() - toIntSize(scrollPosition()), boundsInContent.size());

    int centeringOffsetX = (targetRectInFrame.width() - boundsInFrame.width()) / 2;
    int centeringOffsetY = (targetRectInFrame.height() - boundsInFrame.height()) / 2;

    IntSize scrollDelta(
        boundsInFrame.x() - centeringOffsetX - targetRectInFrame.x(),
        boundsInFrame.y() - centeringOffsetY - targetRectInFrame.y());

    DoublePoint targetOffset = DoublePoint(scrollPosition() + scrollDelta);

    setScrollPosition(targetOffset, ProgrammaticScroll);
}

LayoutRect ScrollableArea::scrollIntoView(const LayoutRect& rectInContent, const ScrollAlignment& alignX, const ScrollAlignment& alignY)
{
    // TODO(bokan): This should really be implemented here but ScrollAlignment is in Core which is a dependency violation.
    ASSERT_NOT_REACHED();
    return LayoutRect();
}

void ScrollableArea::scrollPositionChanged(const DoublePoint& position, ScrollType scrollType)
{
    TRACE_EVENT0("blink", "ScrollableArea::scrollPositionChanged");

    DoublePoint oldPosition = scrollPositionDouble();
    DoublePoint truncatedPosition = shouldUseIntegerScrollOffset() ? flooredIntPoint(position) : position;

    // Tell the derived class to scroll its contents.
    setScrollOffset(truncatedPosition, scrollType);

    Scrollbar* verticalScrollbar = this->verticalScrollbar();

    // Tell the scrollbars to update their thumb postions.
    if (Scrollbar* horizontalScrollbar = this->horizontalScrollbar()) {
        horizontalScrollbar->offsetDidChange();
        if (horizontalScrollbar->isOverlayScrollbar() && !hasLayerForHorizontalScrollbar()) {
            if (!verticalScrollbar)
                horizontalScrollbar->invalidate();
            else {
                // If there is both a horizontalScrollbar and a verticalScrollbar,
                // then we must also invalidate the corner between them.
                IntRect boundsAndCorner = horizontalScrollbar->boundsRect();
                boundsAndCorner.setWidth(boundsAndCorner.width() + verticalScrollbar->width());
                horizontalScrollbar->invalidateRect(boundsAndCorner);
            }
        }
    }
    if (verticalScrollbar) {
        verticalScrollbar->offsetDidChange();
        if (verticalScrollbar->isOverlayScrollbar() && !hasLayerForVerticalScrollbar())
            verticalScrollbar->invalidate();
    }

    if (scrollPositionDouble() != oldPosition) {
        // FIXME: Pass in DoubleSize. crbug.com/414283.
        scrollAnimator()->notifyContentAreaScrolled(toFloatSize(scrollPositionDouble() - oldPosition));
    }

    scrollAnimator()->setCurrentPosition(toFloatPoint(position));
}

bool ScrollableArea::scrollBehaviorFromString(const String& behaviorString, ScrollBehavior& behavior)
{
    if (behaviorString == "auto")
        behavior = ScrollBehaviorAuto;
    else if (behaviorString == "instant")
        behavior = ScrollBehaviorInstant;
    else if (behaviorString == "smooth")
        behavior = ScrollBehaviorSmooth;
    else
        return false;

    return true;
}

ScrollResult ScrollableArea::handleWheel(const PlatformWheelEvent& wheelEvent)
{
    // Wheel events which do not scroll are used to trigger zooming.
    if (!wheelEvent.canScroll())
        return ScrollResult();

    cancelProgrammaticScrollAnimation();
    return scrollAnimator()->handleWheelEvent(wheelEvent);
}

// NOTE: Only called from Internals for testing.
void ScrollableArea::setScrollOffsetFromInternals(const IntPoint& offset)
{
    scrollPositionChanged(DoublePoint(offset), ProgrammaticScroll);
}

void ScrollableArea::willStartLiveResize()
{
    if (m_inLiveResize)
        return;
    m_inLiveResize = true;
    if (ScrollAnimator* scrollAnimator = existingScrollAnimator())
        scrollAnimator->willStartLiveResize();
}

void ScrollableArea::willEndLiveResize()
{
    if (!m_inLiveResize)
        return;
    m_inLiveResize = false;
    if (ScrollAnimator* scrollAnimator = existingScrollAnimator())
        scrollAnimator->willEndLiveResize();
}

void ScrollableArea::contentAreaWillPaint() const
{
    if (ScrollAnimator* scrollAnimator = existingScrollAnimator())
        scrollAnimator->contentAreaWillPaint();
}

void ScrollableArea::mouseEnteredContentArea() const
{
    if (ScrollAnimator* scrollAnimator = existingScrollAnimator())
        scrollAnimator->mouseEnteredContentArea();
}

void ScrollableArea::mouseExitedContentArea() const
{
    if (ScrollAnimator* scrollAnimator = existingScrollAnimator())
        scrollAnimator->mouseEnteredContentArea();
}

void ScrollableArea::mouseMovedInContentArea() const
{
    if (ScrollAnimator* scrollAnimator = existingScrollAnimator())
        scrollAnimator->mouseMovedInContentArea();
}

void ScrollableArea::mouseEnteredScrollbar(Scrollbar* scrollbar) const
{
    scrollAnimator()->mouseEnteredScrollbar(scrollbar);
}

void ScrollableArea::mouseExitedScrollbar(Scrollbar* scrollbar) const
{
    scrollAnimator()->mouseExitedScrollbar(scrollbar);
}

void ScrollableArea::contentAreaDidShow() const
{
    if (ScrollAnimator* scrollAnimator = existingScrollAnimator())
        scrollAnimator->contentAreaDidShow();
}

void ScrollableArea::contentAreaDidHide() const
{
    if (ScrollAnimator* scrollAnimator = existingScrollAnimator())
        scrollAnimator->contentAreaDidHide();
}

void ScrollableArea::finishCurrentScrollAnimations() const
{
    if (ScrollAnimator* scrollAnimator = existingScrollAnimator())
        scrollAnimator->finishCurrentScrollAnimations();
}

void ScrollableArea::didAddScrollbar(Scrollbar* scrollbar, ScrollbarOrientation orientation)
{
    if (orientation == VerticalScrollbar)
        scrollAnimator()->didAddVerticalScrollbar(scrollbar);
    else
        scrollAnimator()->didAddHorizontalScrollbar(scrollbar);

    // <rdar://problem/9797253> AppKit resets the scrollbar's style when you attach a scrollbar
    setScrollbarOverlayStyle(scrollbarOverlayStyle());
}

void ScrollableArea::willRemoveScrollbar(Scrollbar* scrollbar, ScrollbarOrientation orientation)
{
    if (orientation == VerticalScrollbar)
        scrollAnimator()->willRemoveVerticalScrollbar(scrollbar);
    else
        scrollAnimator()->willRemoveHorizontalScrollbar(scrollbar);
}

void ScrollableArea::contentsResized()
{
    if (ScrollAnimator* scrollAnimator = existingScrollAnimator())
        scrollAnimator->contentsResized();
}

bool ScrollableArea::hasOverlayScrollbars() const
{
    Scrollbar* vScrollbar = verticalScrollbar();
    if (vScrollbar && vScrollbar->isOverlayScrollbar())
        return true;
    Scrollbar* hScrollbar = horizontalScrollbar();
    return hScrollbar && hScrollbar->isOverlayScrollbar();
}

void ScrollableArea::setScrollbarOverlayStyle(ScrollbarOverlayStyle overlayStyle)
{
    m_scrollbarOverlayStyle = overlayStyle;

    if (Scrollbar* scrollbar = horizontalScrollbar()) {
        ScrollbarTheme::theme()->updateScrollbarOverlayStyle(scrollbar);
        scrollbar->invalidate();
    }

    if (Scrollbar* scrollbar = verticalScrollbar()) {
        ScrollbarTheme::theme()->updateScrollbarOverlayStyle(scrollbar);
        scrollbar->invalidate();
    }
}

void ScrollableArea::invalidateScrollbar(Scrollbar* scrollbar, const IntRect& rect)
{
    if (scrollbar == horizontalScrollbar()) {
        if (GraphicsLayer* graphicsLayer = layerForHorizontalScrollbar()) {
            graphicsLayer->setNeedsDisplay();
            graphicsLayer->setContentsNeedsDisplay();
            return;
        }
        invalidateScrollbarRect(scrollbar, rect);
        return;
    }
    if (scrollbar == verticalScrollbar()) {
        if (GraphicsLayer* graphicsLayer = layerForVerticalScrollbar()) {
            graphicsLayer->setNeedsDisplay();
            graphicsLayer->setContentsNeedsDisplay();
            return;
        }
        invalidateScrollbarRect(scrollbar, rect);
        return;
    }
    // Otherwise the scrollbar is just created and has not been set as either
    // horizontalScrollbar() or verticalScrollbar().
}

void ScrollableArea::invalidateScrollCorner(const IntRect& rect)
{
    if (GraphicsLayer* graphicsLayer = layerForScrollCorner()) {
        graphicsLayer->setNeedsDisplay();
        return;
    }
    invalidateScrollCornerRect(rect);
}

bool ScrollableArea::hasLayerForHorizontalScrollbar() const
{
    return layerForHorizontalScrollbar();
}

bool ScrollableArea::hasLayerForVerticalScrollbar() const
{
    return layerForVerticalScrollbar();
}

bool ScrollableArea::hasLayerForScrollCorner() const
{
    return layerForScrollCorner();
}

void ScrollableArea::layerForScrollingDidChange()
{
    if (ProgrammaticScrollAnimator* programmaticScrollAnimator = existingProgrammaticScrollAnimator())
        programmaticScrollAnimator->layerForCompositedScrollingDidChange();
}

bool ScrollableArea::scheduleAnimation()
{
    if (HostWindow* window = hostWindow()) {
        window->scheduleAnimation();
        return true;
    }
    return false;
}

void ScrollableArea::serviceScrollAnimations(double monotonicTime)
{
    bool requiresAnimationService = false;
    if (ScrollAnimator* scrollAnimator = existingScrollAnimator()) {
        scrollAnimator->serviceScrollAnimations();
        if (scrollAnimator->hasRunningAnimation())
            requiresAnimationService = true;
    }
    if (ProgrammaticScrollAnimator* programmaticScrollAnimator = existingProgrammaticScrollAnimator()) {
        programmaticScrollAnimator->tickAnimation(monotonicTime);
        if (programmaticScrollAnimator->hasAnimationThatRequiresService())
            requiresAnimationService = true;
    }
    if (!requiresAnimationService)
        deregisterForAnimation();
}

void ScrollableArea::updateCompositorScrollAnimations()
{
    if (ProgrammaticScrollAnimator* programmaticScrollAnimator = existingProgrammaticScrollAnimator())
        programmaticScrollAnimator->updateCompositorAnimations();
}

void ScrollableArea::notifyCompositorAnimationFinished(int groupId)
{
    if (ProgrammaticScrollAnimator* programmaticScrollAnimator = existingProgrammaticScrollAnimator())
        programmaticScrollAnimator->notifyCompositorAnimationFinished(groupId);
}

void ScrollableArea::cancelScrollAnimation()
{
    if (ScrollAnimator* scrollAnimator = existingScrollAnimator())
        scrollAnimator->cancelAnimations();
}

void ScrollableArea::cancelProgrammaticScrollAnimation()
{
    if (ProgrammaticScrollAnimator* programmaticScrollAnimator = existingProgrammaticScrollAnimator())
        programmaticScrollAnimator->cancelAnimation();
}

DoubleRect ScrollableArea::visibleContentRectDouble(IncludeScrollbarsInRect scrollbarInclusion) const
{
    return visibleContentRect(scrollbarInclusion);
}

IntRect ScrollableArea::visibleContentRect(IncludeScrollbarsInRect scrollbarInclusion) const
{
    int verticalScrollbarWidth = 0;
    int horizontalScrollbarHeight = 0;

    if (scrollbarInclusion == IncludeScrollbars) {
        if (Scrollbar* verticalBar = verticalScrollbar())
            verticalScrollbarWidth = !verticalBar->isOverlayScrollbar() ? verticalBar->width() : 0;
        if (Scrollbar* horizontalBar = horizontalScrollbar())
            horizontalScrollbarHeight = !horizontalBar->isOverlayScrollbar() ? horizontalBar->height() : 0;
    }

    return IntRect(scrollPosition().x(),
                   scrollPosition().y(),
                   std::max(0, visibleWidth() + verticalScrollbarWidth),
                   std::max(0, visibleHeight() + horizontalScrollbarHeight));
}

IntPoint ScrollableArea::clampScrollPosition(const IntPoint& scrollPosition) const
{
    return scrollPosition.shrunkTo(maximumScrollPosition()).expandedTo(minimumScrollPosition());
}

DoublePoint ScrollableArea::clampScrollPosition(const DoublePoint& scrollPosition) const
{
    return scrollPosition.shrunkTo(maximumScrollPositionDouble()).expandedTo(minimumScrollPositionDouble());
}


int ScrollableArea::lineStep(ScrollbarOrientation) const
{
    return pixelsPerLineStep();
}

int ScrollableArea::pageStep(ScrollbarOrientation orientation) const
{
    int length = (orientation == HorizontalScrollbar) ? visibleWidth() : visibleHeight();
    int minPageStep = static_cast<float>(length) * minFractionToStepWhenPaging();
    int pageStep = std::max(minPageStep, length - maxOverlapBetweenPages());

    return std::max(pageStep, 1);
}

int ScrollableArea::documentStep(ScrollbarOrientation orientation) const
{
    return scrollSize(orientation);
}

float ScrollableArea::pixelStep(ScrollbarOrientation) const
{
    return 1;
}

IntSize ScrollableArea::excludeScrollbars(const IntSize& size) const
{
    int verticalScrollbarWidth = 0;
    int horizontalScrollbarHeight = 0;

    if (Scrollbar* verticalBar = verticalScrollbar())
        verticalScrollbarWidth = !verticalBar->isOverlayScrollbar() ? verticalBar->width() : 0;
    if (Scrollbar* horizontalBar = horizontalScrollbar())
        horizontalScrollbarHeight = !horizontalBar->isOverlayScrollbar() ? horizontalBar->height() : 0;

    return IntSize(std::max(0, size.width() - verticalScrollbarWidth),
        std::max(0, size.height() - horizontalScrollbarHeight));

}

} // namespace blink
