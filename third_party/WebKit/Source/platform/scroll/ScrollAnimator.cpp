/*
 * Copyright (c) 2010, Google Inc. All rights reserved.
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
#include "platform/scroll/ScrollAnimator.h"

#include "platform/RuntimeEnabledFeatures.h"
#include "platform/geometry/FloatPoint.h"
#include "platform/scroll/ScrollableArea.h"
#include "wtf/PassOwnPtr.h"
#include <algorithm>

namespace blink {

ScrollAnimator::ScrollAnimator(ScrollableArea* scrollableArea)
    : m_scrollableArea(scrollableArea)
    , m_currentPosX(0)
    , m_currentPosY(0)
{
}

ScrollAnimator::~ScrollAnimator()
{
}

ScrollResultOneDimensional ScrollAnimator::userScroll(ScrollbarOrientation orientation, ScrollGranularity, float step, float delta)
{
    float& currentPos = (orientation == HorizontalScrollbar) ? m_currentPosX : m_currentPosY;
    float newPos = clampScrollPosition(orientation, currentPos + step * delta);
    if (currentPos == newPos)
        return ScrollResultOneDimensional(false, delta);

    float usedDelta = (newPos - currentPos) / step;
    currentPos = newPos;

    notifyPositionChanged();

    return ScrollResultOneDimensional(true, delta - usedDelta);
}

void ScrollAnimator::scrollToOffsetWithoutAnimation(const FloatPoint& offset)
{
    m_currentPosX = offset.x();
    m_currentPosY = offset.y();
    notifyPositionChanged();
}

ScrollResult ScrollAnimator::handleWheelEvent(const PlatformWheelEvent& e)
{
    bool canScrollX = m_scrollableArea->userInputScrollable(HorizontalScrollbar)
        && e.railsMode() != PlatformEvent::RailsModeVertical;
    bool canScrollY = m_scrollableArea->userInputScrollable(VerticalScrollbar)
        && e.railsMode() != PlatformEvent::RailsModeHorizontal;

    // Accept the event if we are scrollable in that direction and can still
    // scroll any further.
    float deltaX = canScrollX ? e.deltaX() : 0;
    float deltaY = canScrollY ? e.deltaY() : 0;

    ScrollResult result;

#if !OS(MACOSX)
    ScrollGranularity granularity = e.hasPreciseScrollingDeltas() ? ScrollByPrecisePixel : ScrollByPixel;
#else
    ScrollGranularity granularity = ScrollByPixel;
#endif

    if (deltaY) {
        if (e.granularity() == ScrollByPageWheelEvent) {
            bool negative = deltaY < 0;
            deltaY = m_scrollableArea->pageStep(VerticalScrollbar);
            if (negative)
                deltaY = -deltaY;
        }

        ScrollResultOneDimensional resultY = userScroll(
            VerticalScrollbar, granularity, m_scrollableArea->pixelStep(VerticalScrollbar), -deltaY);
        result.didScrollY = resultY.didScroll;
        if (e.granularity() != ScrollByPageWheelEvent) {
            if (resultY.didScroll)
                result.unusedScrollDeltaY = -resultY.unusedScrollDelta;
            else
                result.unusedScrollDeltaY = deltaY;
        }
    }

    if (deltaX) {
        if (e.granularity() == ScrollByPageWheelEvent) {
            bool negative = deltaX < 0;
            deltaX = m_scrollableArea->pageStep(HorizontalScrollbar);
            if (negative)
                deltaX = -deltaX;
        }

        ScrollResultOneDimensional resultX = userScroll(
            HorizontalScrollbar, granularity, m_scrollableArea->pixelStep(HorizontalScrollbar), -deltaX);
        result.didScrollX = resultX.didScroll;
        if (e.granularity() != ScrollByPageWheelEvent) {
            if (resultX.didScroll)
                result.unusedScrollDeltaX = -resultX.unusedScrollDelta;
            else
                result.unusedScrollDeltaX = deltaX;
        }
    }
    return result;
}

void ScrollAnimator::setCurrentPosition(const FloatPoint& position)
{
    m_currentPosX = position.x();
    m_currentPosY = position.y();
}

FloatPoint ScrollAnimator::currentPosition() const
{
    return FloatPoint(m_currentPosX, m_currentPosY);
}

void ScrollAnimator::notifyPositionChanged()
{
    m_scrollableArea->scrollPositionChanged(DoublePoint(m_currentPosX, m_currentPosY), UserScroll);
}

float ScrollAnimator::clampScrollPosition(ScrollbarOrientation orientation, float pos)
{
    float maxScrollPos = m_scrollableArea->maximumScrollPosition(orientation);
    float minScrollPos = m_scrollableArea->minimumScrollPosition(orientation);
    return std::max(std::min(pos, maxScrollPos), minScrollPos);
}

} // namespace blink
