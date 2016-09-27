/*
 * Copyright (C) 2010, 2011 Apple Inc. All rights reserved.
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
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. AND ITS CONTRIBUTORS ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef ScrollAnimatorMac_h
#define ScrollAnimatorMac_h

#include "platform/Timer.h"
#include "platform/geometry/FloatPoint.h"
#include "platform/geometry/FloatSize.h"
#include "platform/geometry/IntRect.h"
#include "platform/scroll/ScrollAnimator.h"
#include "wtf/RetainPtr.h"

OBJC_CLASS WebScrollAnimationHelperDelegate;
OBJC_CLASS WebScrollbarPainterControllerDelegate;
OBJC_CLASS WebScrollbarPainterDelegate;

typedef id ScrollbarPainterController;

namespace blink {

class Scrollbar;

class PLATFORM_EXPORT ScrollAnimatorMac : public ScrollAnimator {

public:
    ScrollAnimatorMac(ScrollableArea*);
    ~ScrollAnimatorMac() override;

    void immediateScrollToPointForScrollAnimation(const FloatPoint& newPosition);
    bool haveScrolledSincePageLoad() const { return m_haveScrolledSincePageLoad; }

    void updateScrollerStyle();

    bool scrollbarPaintTimerIsActive() const;
    void startScrollbarPaintTimer();
    void stopScrollbarPaintTimer();

    void sendContentAreaScrolledSoon(const FloatSize& scrollDelta);

    void setVisibleScrollerThumbRect(const IntRect&);

    static bool canUseCoordinatedScrollbar();

private:
    RetainPtr<id> m_scrollAnimationHelper;
    RetainPtr<WebScrollAnimationHelperDelegate> m_scrollAnimationHelperDelegate;

    RetainPtr<ScrollbarPainterController> m_scrollbarPainterController;
    RetainPtr<WebScrollbarPainterControllerDelegate> m_scrollbarPainterControllerDelegate;
    RetainPtr<WebScrollbarPainterDelegate> m_horizontalScrollbarPainterDelegate;
    RetainPtr<WebScrollbarPainterDelegate> m_verticalScrollbarPainterDelegate;

    void initialScrollbarPaintTimerFired(Timer<ScrollAnimatorMac>*);
    Timer<ScrollAnimatorMac> m_initialScrollbarPaintTimer;

    void sendContentAreaScrolledTimerFired(Timer<ScrollAnimatorMac>*);
    Timer<ScrollAnimatorMac> m_sendContentAreaScrolledTimer;
    FloatSize m_contentAreaScrolledTimerScrollDelta;

    ScrollResultOneDimensional userScroll(ScrollbarOrientation, ScrollGranularity, float step, float delta) override;
    void scrollToOffsetWithoutAnimation(const FloatPoint&) override;

    void handleWheelEventPhase(PlatformWheelEventPhase) override;

    void cancelAnimations() override;
    void setIsActive() override;

    void contentAreaWillPaint() const override;
    void mouseEnteredContentArea() const override;
    void mouseExitedContentArea() const override;
    void mouseMovedInContentArea() const override;
    void mouseEnteredScrollbar(Scrollbar*) const override;
    void mouseExitedScrollbar(Scrollbar*) const override;
    void willStartLiveResize() override;
    void contentsResized() const override;
    void willEndLiveResize() override;
    void contentAreaDidShow() const override;
    void contentAreaDidHide() const override;
    void didBeginScrollGesture() const;
    void didEndScrollGesture() const;
    void mayBeginScrollGesture() const;

    void finishCurrentScrollAnimations() override;

    void didAddVerticalScrollbar(Scrollbar*) override;
    void willRemoveVerticalScrollbar(Scrollbar*) override;
    void didAddHorizontalScrollbar(Scrollbar*) override;
    void willRemoveHorizontalScrollbar(Scrollbar*) override;

    bool shouldScrollbarParticipateInHitTesting(Scrollbar*) override;

    void notifyContentAreaScrolled(const FloatSize& delta) override;

    FloatPoint adjustScrollPositionIfNecessary(const FloatPoint&) const;

    void immediateScrollTo(const FloatPoint&);

    bool m_haveScrolledSincePageLoad;
    bool m_needsScrollerStyleUpdate;
    IntRect m_visibleScrollerThumbRect;
};

} // namespace blink

#endif // ScrollAnimatorMac_h
