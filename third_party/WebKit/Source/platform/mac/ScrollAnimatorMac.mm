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

#include "config.h"

#include "platform/mac/ScrollAnimatorMac.h"

#include "platform/PlatformGestureEvent.h"
#include "platform/PlatformWheelEvent.h"
#include "platform/Timer.h"
#include "platform/animation/TimingFunction.h"
#include "platform/geometry/FloatRect.h"
#include "platform/geometry/IntRect.h"
#include "platform/mac/BlockExceptions.h"
#include "platform/mac/NSScrollerImpDetails.h"
#include "platform/scroll/ScrollableArea.h"
#include "platform/scroll/ScrollbarTheme.h"
#include "platform/scroll/ScrollbarThemeMacCommon.h"
#include "platform/scroll/ScrollbarThemeMacOverlayAPI.h"
#include "wtf/MainThread.h"
#include "wtf/PassOwnPtr.h"

using namespace blink;

static bool supportsUIStateTransitionProgress()
{
    // FIXME: This is temporary until all platforms that support ScrollbarPainter support this part of the API.
    static bool globalSupportsUIStateTransitionProgress = [NSClassFromString(@"NSScrollerImp") instancesRespondToSelector:@selector(mouseEnteredScroller)];
    return globalSupportsUIStateTransitionProgress;
}

static bool supportsExpansionTransitionProgress()
{
    static bool globalSupportsExpansionTransitionProgress = [NSClassFromString(@"NSScrollerImp") instancesRespondToSelector:@selector(expansionTransitionProgress)];
    return globalSupportsExpansionTransitionProgress;
}

static bool supportsContentAreaScrolledInDirection()
{
    static bool globalSupportsContentAreaScrolledInDirection = [NSClassFromString(@"NSScrollerImpPair") instancesRespondToSelector:@selector(contentAreaScrolledInDirection:)];
    return globalSupportsContentAreaScrolledInDirection;
}

static ScrollbarThemeMacOverlayAPI* macOverlayScrollbarTheme()
{
    RELEASE_ASSERT_WITH_SECURITY_IMPLICATION(ScrollbarThemeMacCommon::isOverlayAPIAvailable());
    ScrollbarTheme* scrollbarTheme = ScrollbarTheme::theme();
    return !scrollbarTheme->isMockTheme() ? static_cast<ScrollbarThemeMacOverlayAPI*>(scrollbarTheme) : 0;
}

static ScrollbarPainter scrollbarPainterForScrollbar(Scrollbar* scrollbar)
{
    if (ScrollbarThemeMacOverlayAPI* scrollbarTheme = macOverlayScrollbarTheme())
        return scrollbarTheme->painterForScrollbar(scrollbar);

    return nil;
}

@interface NSObject (ScrollAnimationHelperDetails)
- (id)initWithDelegate:(id)delegate;
- (void)_stopRun;
- (BOOL)_isAnimating;
- (NSPoint)targetOrigin;
- (CGFloat)_progress;
@end

@interface WebScrollAnimationHelperDelegate : NSObject
{
    blink::ScrollAnimatorMac* _animator;
}
- (id)initWithScrollAnimator:(blink::ScrollAnimatorMac*)scrollAnimator;
@end

static NSSize abs(NSSize size)
{
    NSSize finalSize = size;
    if (finalSize.width < 0)
        finalSize.width = -finalSize.width;
    if (finalSize.height < 0)
        finalSize.height = -finalSize.height;
    return finalSize;
}

@implementation WebScrollAnimationHelperDelegate

- (id)initWithScrollAnimator:(blink::ScrollAnimatorMac*)scrollAnimator
{
    self = [super init];
    if (!self)
        return nil;

    _animator = scrollAnimator;
    return self;
}

- (void)invalidate
{
    _animator = 0;
}

- (NSRect)bounds
{
    if (!_animator)
        return NSZeroRect;

    blink::FloatPoint currentPosition = _animator->currentPosition();
    return NSMakeRect(currentPosition.x(), currentPosition.y(), 0, 0);
}

- (void)_immediateScrollToPoint:(NSPoint)newPosition
{
    if (!_animator)
        return;
    _animator->immediateScrollToPointForScrollAnimation(newPosition);
}

- (NSPoint)_pixelAlignProposedScrollPosition:(NSPoint)newOrigin
{
    return newOrigin;
}

- (NSSize)convertSizeToBase:(NSSize)size
{
    return abs(size);
}

- (NSSize)convertSizeFromBase:(NSSize)size
{
    return abs(size);
}

- (NSSize)convertSizeToBacking:(NSSize)size
{
    return abs(size);
}

- (NSSize)convertSizeFromBacking:(NSSize)size
{
    return abs(size);
}

- (id)superview
{
    return nil;
}

- (id)documentView
{
    return nil;
}

- (id)window
{
    return nil;
}

- (void)_recursiveRecomputeToolTips
{
}

@end

@interface WebScrollbarPainterControllerDelegate : NSObject
{
    ScrollableArea* _scrollableArea;
}
- (id)initWithScrollableArea:(ScrollableArea*)scrollableArea;
@end

@implementation WebScrollbarPainterControllerDelegate

- (id)initWithScrollableArea:(ScrollableArea*)scrollableArea
{
    self = [super init];
    if (!self)
        return nil;

    _scrollableArea = scrollableArea;
    return self;
}

- (void)invalidate
{
    _scrollableArea = 0;
}

- (NSRect)contentAreaRectForScrollerImpPair:(id)scrollerImpPair
{
    if (!_scrollableArea)
        return NSZeroRect;

    blink::IntSize contentsSize = _scrollableArea->contentsSize();
    return NSMakeRect(0, 0, contentsSize.width(), contentsSize.height());
}

- (BOOL)inLiveResizeForScrollerImpPair:(id)scrollerImpPair
{
    if (!_scrollableArea)
        return NO;

    return _scrollableArea->inLiveResize();
}

- (NSPoint)mouseLocationInContentAreaForScrollerImpPair:(id)scrollerImpPair
{
    if (!_scrollableArea)
        return NSZeroPoint;

    return _scrollableArea->lastKnownMousePosition();
}

- (NSPoint)scrollerImpPair:(id)scrollerImpPair convertContentPoint:(NSPoint)pointInContentArea toScrollerImp:(id)scrollerImp
{

    if (!_scrollableArea || !scrollerImp)
        return NSZeroPoint;

    blink::Scrollbar* scrollbar = 0;
    if ([scrollerImp isHorizontal])
        scrollbar = _scrollableArea->horizontalScrollbar();
    else
        scrollbar = _scrollableArea->verticalScrollbar();

    // It is possible to have a null scrollbar here since it is possible for this delegate
    // method to be called between the moment when a scrollbar has been set to 0 and the
    // moment when its destructor has been called. We should probably de-couple some
    // of the clean-up work in ScrollbarThemeMac::unregisterScrollbar() to avoid this
    // issue.
    if (!scrollbar)
        return NSZeroPoint;

    ASSERT(scrollerImp == scrollbarPainterForScrollbar(scrollbar));

    return scrollbar->convertFromContainingView(blink::IntPoint(pointInContentArea));
}

- (void)scrollerImpPair:(id)scrollerImpPair setContentAreaNeedsDisplayInRect:(NSRect)rect
{
    if (!_scrollableArea)
        return;

    if (!_scrollableArea->scrollbarsCanBeActive())
        return;

    _scrollableArea->scrollAnimator()->contentAreaWillPaint();
}

- (void)scrollerImpPair:(id)scrollerImpPair updateScrollerStyleForNewRecommendedScrollerStyle:(NSScrollerStyle)newRecommendedScrollerStyle
{
    // Chrome has a single process mode which is used for testing on Mac. In that mode, WebKit runs on a thread in the
    // browser process. This notification is called by the OS on the main thread in the browser process, and not on the
    // the WebKit thread. Better to not update the style than crash.
    // http://crbug.com/126514
    if (!isMainThread())
        return;

    if (!_scrollableArea)
        return;

    [scrollerImpPair setScrollerStyle:newRecommendedScrollerStyle];

    static_cast<ScrollAnimatorMac*>(_scrollableArea->scrollAnimator())->updateScrollerStyle();
}

@end

enum FeatureToAnimate {
    ThumbAlpha,
    TrackAlpha,
    UIStateTransition,
    ExpansionTransition
};

@class WebScrollbarPartAnimation;

namespace blink {

// This class is used to drive the animation timer for WebScrollbarPartAnimation
// objects. This is used instead of NSAnimation because CoreAnimation
// establishes connections to the WindowServer, which should not be done in a
// sandboxed renderer process.
class WebScrollbarPartAnimationTimer {
public:
    WebScrollbarPartAnimationTimer(WebScrollbarPartAnimation* animation,
                                   CFTimeInterval duration)
        : m_timer(this, &WebScrollbarPartAnimationTimer::timerFired)
        , m_startTime(0.0)
        , m_duration(duration)
        , m_animation(animation)
        , m_timingFunction(CubicBezierTimingFunction::preset(CubicBezierTimingFunction::EaseInOut))
    {
    }

    ~WebScrollbarPartAnimationTimer() {}

    void start()
    {
        m_startTime = WTF::currentTime();
        // Set the framerate of the animation. NSAnimation uses a default
        // framerate of 60 Hz, so use that here.
        m_timer.startRepeating(1.0 / 60.0, FROM_HERE);
    }

    void stop()
    {
        m_timer.stop();
    }

    void setDuration(CFTimeInterval duration)
    {
        m_duration = duration;
    }

private:
    void timerFired(Timer<WebScrollbarPartAnimationTimer>*)
    {
        double currentTime = WTF::currentTime();
        double delta = currentTime - m_startTime;

        if (delta >= m_duration)
            m_timer.stop();

        double fraction = delta / m_duration;
        fraction = std::min(1.0, fraction);
        fraction = std::max(0.0, fraction);
        double progress = m_timingFunction->evaluate(fraction, 0.001);
        [m_animation setCurrentProgress:progress];
    }

    Timer<WebScrollbarPartAnimationTimer> m_timer;
    double m_startTime;  // In seconds.
    double m_duration;   // In seconds.
    WebScrollbarPartAnimation* m_animation;  // Weak, owns this.
    RefPtr<CubicBezierTimingFunction> m_timingFunction;
};

}  // namespace blink

@interface WebScrollbarPartAnimation : NSObject {
    Scrollbar* _scrollbar;
    OwnPtr<WebScrollbarPartAnimationTimer> _timer;
    RetainPtr<ScrollbarPainter> _scrollbarPainter;
    FeatureToAnimate _featureToAnimate;
    CGFloat _startValue;
    CGFloat _endValue;
}
- (id)initWithScrollbar:(Scrollbar*)scrollbar featureToAnimate:(FeatureToAnimate)featureToAnimate animateFrom:(CGFloat)startValue animateTo:(CGFloat)endValue duration:(NSTimeInterval)duration;
@end

@implementation WebScrollbarPartAnimation

- (id)initWithScrollbar:(Scrollbar*)scrollbar featureToAnimate:(FeatureToAnimate)featureToAnimate animateFrom:(CGFloat)startValue animateTo:(CGFloat)endValue duration:(NSTimeInterval)duration
{
    self = [super init];
    if (!self)
        return nil;

    _timer = adoptPtr(new WebScrollbarPartAnimationTimer(self, duration));
    _scrollbar = scrollbar;
    _featureToAnimate = featureToAnimate;
    _startValue = startValue;
    _endValue = endValue;

    return self;
}

- (void)startAnimation
{
    ASSERT(_scrollbar);

    _scrollbarPainter = scrollbarPainterForScrollbar(_scrollbar);
    _timer->start();
}

- (void)stopAnimation
{
    _timer->stop();
}

- (void)setDuration:(CFTimeInterval)duration
{
    _timer->setDuration(duration);
}

- (void)setStartValue:(CGFloat)startValue
{
    _startValue = startValue;
}

- (void)setEndValue:(CGFloat)endValue
{
    _endValue = endValue;
}

- (void)setCurrentProgress:(NSAnimationProgress)progress
{
    ASSERT(_scrollbar);

    CGFloat currentValue;
    if (_startValue > _endValue)
        currentValue = 1 - progress;
    else
        currentValue = progress;

    switch (_featureToAnimate) {
    case ThumbAlpha:
        [_scrollbarPainter.get() setKnobAlpha:currentValue];
        break;
    case TrackAlpha:
        [_scrollbarPainter.get() setTrackAlpha:currentValue];
        break;
    case UIStateTransition:
        [_scrollbarPainter.get() setUiStateTransitionProgress:currentValue];
        break;
    case ExpansionTransition:
        [_scrollbarPainter.get() setExpansionTransitionProgress:currentValue];
        break;
    }

    _scrollbar->invalidate();
}

- (void)invalidate
{
    BEGIN_BLOCK_OBJC_EXCEPTIONS;
    [self stopAnimation];
    END_BLOCK_OBJC_EXCEPTIONS;
    _scrollbar = 0;
}

@end

@interface WebScrollbarPainterDelegate : NSObject<NSAnimationDelegate>
{
    blink::Scrollbar* _scrollbar;

    RetainPtr<WebScrollbarPartAnimation> _knobAlphaAnimation;
    RetainPtr<WebScrollbarPartAnimation> _trackAlphaAnimation;
    RetainPtr<WebScrollbarPartAnimation> _uiStateTransitionAnimation;
    RetainPtr<WebScrollbarPartAnimation> _expansionTransitionAnimation;
}
- (id)initWithScrollbar:(blink::Scrollbar*)scrollbar;
- (void)cancelAnimations;
@end

@implementation WebScrollbarPainterDelegate

- (id)initWithScrollbar:(blink::Scrollbar*)scrollbar
{
    self = [super init];
    if (!self)
        return nil;

    _scrollbar = scrollbar;
    return self;
}

- (void)cancelAnimations
{
    BEGIN_BLOCK_OBJC_EXCEPTIONS;
    [_knobAlphaAnimation.get() stopAnimation];
    [_trackAlphaAnimation.get() stopAnimation];
    [_uiStateTransitionAnimation.get() stopAnimation];
    [_expansionTransitionAnimation.get() stopAnimation];
    END_BLOCK_OBJC_EXCEPTIONS;
}

- (ScrollAnimatorMac*)scrollAnimator
{
    return static_cast<ScrollAnimatorMac*>(_scrollbar->scrollableArea()->scrollAnimator());
}

- (NSRect)convertRectToBacking:(NSRect)aRect
{
    return aRect;
}

- (NSRect)convertRectFromBacking:(NSRect)aRect
{
    return aRect;
}

- (NSPoint)mouseLocationInScrollerForScrollerImp:(id)scrollerImp
{
    if (!_scrollbar)
        return NSZeroPoint;

    ASSERT_UNUSED(scrollerImp, scrollerImp == scrollbarPainterForScrollbar(_scrollbar));

    return _scrollbar->convertFromContainingView(_scrollbar->scrollableArea()->lastKnownMousePosition());
}

- (void)setUpAlphaAnimation:(RetainPtr<WebScrollbarPartAnimation>&)scrollbarPartAnimation scrollerPainter:(ScrollbarPainter)scrollerPainter part:(blink::ScrollbarPart)part animateAlphaTo:(CGFloat)newAlpha duration:(NSTimeInterval)duration
{
    // If the user has scrolled the page, then the scrollbars must be animated here.
    // This overrides the early returns.
    bool mustAnimate = [self scrollAnimator]->haveScrolledSincePageLoad();

    if ([self scrollAnimator]->scrollbarPaintTimerIsActive() && !mustAnimate)
        return;

    if (_scrollbar->scrollableArea()->shouldSuspendScrollAnimations() && !mustAnimate) {
        [self scrollAnimator]->startScrollbarPaintTimer();
        return;
    }

    // At this point, we are definitely going to animate now, so stop the timer.
    [self scrollAnimator]->stopScrollbarPaintTimer();

    // If we are currently animating, stop
    if (scrollbarPartAnimation) {
        [scrollbarPartAnimation.get() stopAnimation];
        scrollbarPartAnimation = nullptr;
    }

    if (part == blink::ThumbPart && _scrollbar->orientation() == VerticalScrollbar) {
        if (newAlpha == 1) {
            IntRect thumbRect = IntRect([scrollerPainter rectForPart:NSScrollerKnob]);
            [self scrollAnimator]->setVisibleScrollerThumbRect(thumbRect);
        } else
            [self scrollAnimator]->setVisibleScrollerThumbRect(IntRect());
    }

    scrollbarPartAnimation.adoptNS([[WebScrollbarPartAnimation alloc] initWithScrollbar:_scrollbar
                                                                       featureToAnimate:part == ThumbPart ? ThumbAlpha : TrackAlpha
                                                                            animateFrom:part == ThumbPart ? [scrollerPainter knobAlpha] : [scrollerPainter trackAlpha]
                                                                              animateTo:newAlpha
                                                                               duration:duration]);
    [scrollbarPartAnimation.get() startAnimation];
}

- (void)scrollerImp:(id)scrollerImp animateKnobAlphaTo:(CGFloat)newKnobAlpha duration:(NSTimeInterval)duration
{
    if (!_scrollbar)
        return;

    ASSERT(scrollerImp == scrollbarPainterForScrollbar(_scrollbar));

    ScrollbarPainter scrollerPainter = (ScrollbarPainter)scrollerImp;
    [self setUpAlphaAnimation:_knobAlphaAnimation scrollerPainter:scrollerPainter part:blink::ThumbPart animateAlphaTo:newKnobAlpha duration:duration];
}

- (void)scrollerImp:(id)scrollerImp animateTrackAlphaTo:(CGFloat)newTrackAlpha duration:(NSTimeInterval)duration
{
    if (!_scrollbar)
        return;

    ASSERT(scrollerImp == scrollbarPainterForScrollbar(_scrollbar));

    ScrollbarPainter scrollerPainter = (ScrollbarPainter)scrollerImp;
    [self setUpAlphaAnimation:_trackAlphaAnimation scrollerPainter:scrollerPainter part:blink::BackTrackPart animateAlphaTo:newTrackAlpha duration:duration];
}

- (void)scrollerImp:(id)scrollerImp animateUIStateTransitionWithDuration:(NSTimeInterval)duration
{
    if (!_scrollbar)
        return;

    if (!supportsUIStateTransitionProgress())
        return;

    ASSERT(scrollerImp == scrollbarPainterForScrollbar(_scrollbar));

    ScrollbarPainter scrollbarPainter = (ScrollbarPainter)scrollerImp;

    // UIStateTransition always animates to 1. In case an animation is in progress this avoids a hard transition.
    [scrollbarPainter setUiStateTransitionProgress:1 - [scrollerImp uiStateTransitionProgress]];

    if (!_uiStateTransitionAnimation)
        _uiStateTransitionAnimation.adoptNS([[WebScrollbarPartAnimation alloc] initWithScrollbar:_scrollbar
                                                                                featureToAnimate:UIStateTransition
                                                                                     animateFrom:[scrollbarPainter uiStateTransitionProgress]
                                                                                       animateTo:1.0
                                                                                        duration:duration]);
    else {
        // If we don't need to initialize the animation, just reset the values in case they have changed.
        [_uiStateTransitionAnimation.get() setStartValue:[scrollbarPainter uiStateTransitionProgress]];
        [_uiStateTransitionAnimation.get() setEndValue:1.0];
        [_uiStateTransitionAnimation.get() setDuration:duration];
    }
    [_uiStateTransitionAnimation.get() startAnimation];
}

- (void)scrollerImp:(id)scrollerImp animateExpansionTransitionWithDuration:(NSTimeInterval)duration
{
    if (!_scrollbar)
        return;

    if (!supportsExpansionTransitionProgress())
        return;

    ASSERT(scrollerImp == scrollbarPainterForScrollbar(_scrollbar));

    ScrollbarPainter scrollbarPainter = (ScrollbarPainter)scrollerImp;

    // ExpansionTransition always animates to 1. In case an animation is in progress this avoids a hard transition.
    [scrollbarPainter setExpansionTransitionProgress:1 - [scrollerImp expansionTransitionProgress]];

    if (!_expansionTransitionAnimation) {
        _expansionTransitionAnimation.adoptNS([[WebScrollbarPartAnimation alloc] initWithScrollbar:_scrollbar
                                                                                  featureToAnimate:ExpansionTransition
                                                                                       animateFrom:[scrollbarPainter expansionTransitionProgress]
                                                                                         animateTo:1.0
                                                                                          duration:duration]);
    } else {
        // If we don't need to initialize the animation, just reset the values in case they have changed.
        [_expansionTransitionAnimation.get() setStartValue:[scrollbarPainter uiStateTransitionProgress]];
        [_expansionTransitionAnimation.get() setEndValue:1.0];
        [_expansionTransitionAnimation.get() setDuration:duration];
    }
    [_expansionTransitionAnimation.get() startAnimation];
}

- (void)scrollerImp:(id)scrollerImp overlayScrollerStateChangedTo:(NSUInteger)newOverlayScrollerState
{
}

- (void)invalidate
{
    _scrollbar = 0;
    BEGIN_BLOCK_OBJC_EXCEPTIONS;
    [_knobAlphaAnimation.get() invalidate];
    [_trackAlphaAnimation.get() invalidate];
    [_uiStateTransitionAnimation.get() invalidate];
    [_expansionTransitionAnimation.get() invalidate];
    END_BLOCK_OBJC_EXCEPTIONS;
}

@end

namespace blink {

PassOwnPtr<ScrollAnimator> ScrollAnimator::create(ScrollableArea* scrollableArea)
{
    return adoptPtr(new ScrollAnimatorMac(scrollableArea));
}

ScrollAnimatorMac::ScrollAnimatorMac(ScrollableArea* scrollableArea)
    : ScrollAnimator(scrollableArea)
    , m_initialScrollbarPaintTimer(this, &ScrollAnimatorMac::initialScrollbarPaintTimerFired)
    , m_sendContentAreaScrolledTimer(this, &ScrollAnimatorMac::sendContentAreaScrolledTimerFired)
    , m_haveScrolledSincePageLoad(false)
    , m_needsScrollerStyleUpdate(false)
{
    m_scrollAnimationHelperDelegate.adoptNS([[WebScrollAnimationHelperDelegate alloc] initWithScrollAnimator:this]);
    m_scrollAnimationHelper.adoptNS([[NSClassFromString(@"NSScrollAnimationHelper") alloc] initWithDelegate:m_scrollAnimationHelperDelegate.get()]);

    if (ScrollbarThemeMacCommon::isOverlayAPIAvailable()) {
        m_scrollbarPainterControllerDelegate.adoptNS([[WebScrollbarPainterControllerDelegate alloc] initWithScrollableArea:scrollableArea]);
        m_scrollbarPainterController = [[[NSClassFromString(@"NSScrollerImpPair") alloc] init] autorelease];
        [m_scrollbarPainterController.get() performSelector:@selector(setDelegate:) withObject:m_scrollbarPainterControllerDelegate.get()];
        [m_scrollbarPainterController.get() setScrollerStyle:ScrollbarThemeMacCommon::recommendedScrollerStyle()];
    }
}

ScrollAnimatorMac::~ScrollAnimatorMac()
{
    if (ScrollbarThemeMacCommon::isOverlayAPIAvailable()) {
        BEGIN_BLOCK_OBJC_EXCEPTIONS;
        [m_scrollbarPainterControllerDelegate.get() invalidate];
        [m_scrollbarPainterController.get() setDelegate:nil];
        [m_horizontalScrollbarPainterDelegate.get() invalidate];
        [m_verticalScrollbarPainterDelegate.get() invalidate];
        [m_scrollAnimationHelperDelegate.get() invalidate];
        END_BLOCK_OBJC_EXCEPTIONS;
    }
}

static bool scrollAnimationEnabledForSystem()
{
    static bool initialized = false;
    static bool enabled = true;
    if (!initialized) {
        // Check setting for OS X 10.8+.
        id value = [[NSUserDefaults standardUserDefaults] objectForKey:@"NSScrollAnimationEnabled"];
        // Check setting for OS X < 10.8.
        if (!value)
            value = [[NSUserDefaults standardUserDefaults] objectForKey:@"AppleScrollAnimationEnabled"];
        if (value)
            enabled = [value boolValue];
        initialized = true;
    }
    return enabled;
}

ScrollResultOneDimensional ScrollAnimatorMac::userScroll(ScrollbarOrientation orientation, ScrollGranularity granularity, float step, float delta)
{
    m_haveScrolledSincePageLoad = true;

    if (!scrollAnimationEnabledForSystem() || !m_scrollableArea->scrollAnimatorEnabled())
        return ScrollAnimator::userScroll(orientation, granularity, step, delta);

    if (granularity == ScrollByPixel || granularity == ScrollByPrecisePixel)
        return ScrollAnimator::userScroll(orientation, granularity, step, delta);

    float currentPos = orientation == HorizontalScrollbar ? m_currentPosX : m_currentPosY;
    float newPos = std::max<float>(std::min<float>(currentPos + (step * delta), m_scrollableArea->maximumScrollPosition(orientation)), m_scrollableArea->minimumScrollPosition(orientation));
    if (currentPos == newPos)
        return ScrollResultOneDimensional(false);

    NSPoint newPoint;
    if ([m_scrollAnimationHelper.get() _isAnimating]) {
        NSPoint targetOrigin = [m_scrollAnimationHelper.get() targetOrigin];
        newPoint = orientation == HorizontalScrollbar ? NSMakePoint(newPos, targetOrigin.y) : NSMakePoint(targetOrigin.x, newPos);
    } else
        newPoint = orientation == HorizontalScrollbar ? NSMakePoint(newPos, m_currentPosY) : NSMakePoint(m_currentPosX, newPos);

    [m_scrollAnimationHelper.get() scrollToPoint:newPoint];

    float usedDelta = (newPos - currentPos) / step;
    return ScrollResultOneDimensional(true, delta - usedDelta);
}

void ScrollAnimatorMac::scrollToOffsetWithoutAnimation(const FloatPoint& offset)
{
    [m_scrollAnimationHelper.get() _stopRun];
    immediateScrollTo(offset);
}

FloatPoint ScrollAnimatorMac::adjustScrollPositionIfNecessary(const FloatPoint& position) const
{
    IntPoint minPos = m_scrollableArea->minimumScrollPosition();
    IntPoint maxPos = m_scrollableArea->maximumScrollPosition();

    float newX = std::max<float>(std::min<float>(position.x(), maxPos.x()), minPos.x());
    float newY = std::max<float>(std::min<float>(position.y(), maxPos.y()), minPos.y());

    return FloatPoint(newX, newY);
}

void ScrollAnimatorMac::immediateScrollTo(const FloatPoint& newPosition)
{
    FloatPoint adjustedPosition = adjustScrollPositionIfNecessary(newPosition);

    bool positionChanged = adjustedPosition.x() != m_currentPosX || adjustedPosition.y() != m_currentPosY;
    if (!positionChanged && !scrollableArea()->scrollOriginChanged())
        return;

    FloatSize delta = FloatSize(adjustedPosition.x() - m_currentPosX, adjustedPosition.y() - m_currentPosY);

    m_currentPosX = adjustedPosition.x();
    m_currentPosY = adjustedPosition.y();
    notifyContentAreaScrolled(delta);
    notifyPositionChanged();
}

void ScrollAnimatorMac::immediateScrollToPointForScrollAnimation(const FloatPoint& newPosition)
{
    ASSERT(m_scrollAnimationHelper);
    immediateScrollTo(newPosition);
}

void ScrollAnimatorMac::contentAreaWillPaint() const
{
    if (!scrollableArea()->scrollbarsCanBeActive())
        return;
    if (ScrollbarThemeMacCommon::isOverlayAPIAvailable())
        [m_scrollbarPainterController.get() contentAreaWillDraw];
}

void ScrollAnimatorMac::mouseEnteredContentArea() const
{
    if (!scrollableArea()->scrollbarsCanBeActive())
        return;
    if (ScrollbarThemeMacCommon::isOverlayAPIAvailable())
        [m_scrollbarPainterController.get() mouseEnteredContentArea];
}

void ScrollAnimatorMac::mouseExitedContentArea() const
{
    if (!scrollableArea()->scrollbarsCanBeActive())
        return;
    if (ScrollbarThemeMacCommon::isOverlayAPIAvailable())
        [m_scrollbarPainterController.get() mouseExitedContentArea];
}

void ScrollAnimatorMac::mouseMovedInContentArea() const
{
    if (!scrollableArea()->scrollbarsCanBeActive())
        return;
    if (ScrollbarThemeMacCommon::isOverlayAPIAvailable())
        [m_scrollbarPainterController.get() mouseMovedInContentArea];
}

void ScrollAnimatorMac::mouseEnteredScrollbar(Scrollbar* scrollbar) const
{
    // At this time, only legacy scrollbars needs to send notifications here.
    if (ScrollbarThemeMacCommon::recommendedScrollerStyle() != NSScrollerStyleLegacy)
        return;

    if (!scrollableArea()->scrollbarsCanBeActive())
        return;

    if (ScrollbarThemeMacCommon::isOverlayAPIAvailable()) {
        if (!supportsUIStateTransitionProgress())
            return;
        if (ScrollbarPainter painter = scrollbarPainterForScrollbar(scrollbar))
            [painter mouseEnteredScroller];
    }
}

void ScrollAnimatorMac::mouseExitedScrollbar(Scrollbar* scrollbar) const
{
    // At this time, only legacy scrollbars needs to send notifications here.
    if (ScrollbarThemeMacCommon::recommendedScrollerStyle() != NSScrollerStyleLegacy)
        return;

    if (!scrollableArea()->scrollbarsCanBeActive())
        return;

    if (ScrollbarThemeMacCommon::isOverlayAPIAvailable()) {
        if (!supportsUIStateTransitionProgress())
            return;
        if (ScrollbarPainter painter = scrollbarPainterForScrollbar(scrollbar))
            [painter mouseExitedScroller];
    }
}

void ScrollAnimatorMac::willStartLiveResize()
{
    if (!scrollableArea()->scrollbarsCanBeActive())
        return;
    if (ScrollbarThemeMacCommon::isOverlayAPIAvailable())
        [m_scrollbarPainterController.get() startLiveResize];
}

void ScrollAnimatorMac::contentsResized() const
{
    if (!scrollableArea()->scrollbarsCanBeActive())
        return;
    if (ScrollbarThemeMacCommon::isOverlayAPIAvailable())
        [m_scrollbarPainterController.get() contentAreaDidResize];
}

void ScrollAnimatorMac::willEndLiveResize()
{
    if (!scrollableArea()->scrollbarsCanBeActive())
        return;
    if (ScrollbarThemeMacCommon::isOverlayAPIAvailable())
        [m_scrollbarPainterController.get() endLiveResize];
}

void ScrollAnimatorMac::contentAreaDidShow() const
{
    if (!scrollableArea()->scrollbarsCanBeActive())
        return;
    if (ScrollbarThemeMacCommon::isOverlayAPIAvailable())
        [m_scrollbarPainterController.get() windowOrderedIn];
}

void ScrollAnimatorMac::contentAreaDidHide() const
{
    if (!scrollableArea()->scrollbarsCanBeActive())
        return;
    if (ScrollbarThemeMacCommon::isOverlayAPIAvailable())
        [m_scrollbarPainterController.get() windowOrderedOut];
}

void ScrollAnimatorMac::didBeginScrollGesture() const
{
    if (!scrollableArea()->scrollbarsCanBeActive())
        return;
    if (ScrollbarThemeMacCommon::isOverlayAPIAvailable())
        [m_scrollbarPainterController.get() beginScrollGesture];
}

void ScrollAnimatorMac::didEndScrollGesture() const
{
    if (!scrollableArea()->scrollbarsCanBeActive())
        return;
    if (ScrollbarThemeMacCommon::isOverlayAPIAvailable())
        [m_scrollbarPainterController.get() endScrollGesture];
}

void ScrollAnimatorMac::mayBeginScrollGesture() const
{
    if (!scrollableArea()->scrollbarsCanBeActive())
        return;
    if (!ScrollbarThemeMacCommon::isOverlayAPIAvailable())
        return;

    [m_scrollbarPainterController.get() beginScrollGesture];
    [m_scrollbarPainterController.get() contentAreaScrolled];
}

void ScrollAnimatorMac::finishCurrentScrollAnimations()
{
    if (ScrollbarThemeMacCommon::isOverlayAPIAvailable()) {
        [m_scrollbarPainterController.get() hideOverlayScrollers];
    }
}

void ScrollAnimatorMac::didAddVerticalScrollbar(Scrollbar* scrollbar)
{
    if (!ScrollbarThemeMacCommon::isOverlayAPIAvailable())
        return;

    ScrollbarPainter painter = scrollbarPainterForScrollbar(scrollbar);
    if (!painter)
        return;

    ASSERT(!m_verticalScrollbarPainterDelegate);
    m_verticalScrollbarPainterDelegate.adoptNS([[WebScrollbarPainterDelegate alloc] initWithScrollbar:scrollbar]);

    [painter setDelegate:m_verticalScrollbarPainterDelegate.get()];
    [m_scrollbarPainterController.get() setVerticalScrollerImp:painter];
    if (scrollableArea()->inLiveResize())
        [painter setKnobAlpha:1];
}

void ScrollAnimatorMac::willRemoveVerticalScrollbar(Scrollbar* scrollbar)
{
    if (!ScrollbarThemeMacCommon::isOverlayAPIAvailable())
        return;

    ScrollbarPainter painter = scrollbarPainterForScrollbar(scrollbar);
    if (!painter)
        return;

    ASSERT(m_verticalScrollbarPainterDelegate);
    [m_verticalScrollbarPainterDelegate.get() invalidate];
    m_verticalScrollbarPainterDelegate = nullptr;

    [painter setDelegate:nil];
    [m_scrollbarPainterController.get() setVerticalScrollerImp:nil];
}

void ScrollAnimatorMac::didAddHorizontalScrollbar(Scrollbar* scrollbar)
{
    if (!ScrollbarThemeMacCommon::isOverlayAPIAvailable())
        return;

    ScrollbarPainter painter = scrollbarPainterForScrollbar(scrollbar);
    if (!painter)
        return;

    ASSERT(!m_horizontalScrollbarPainterDelegate);
    m_horizontalScrollbarPainterDelegate.adoptNS([[WebScrollbarPainterDelegate alloc] initWithScrollbar:scrollbar]);

    [painter setDelegate:m_horizontalScrollbarPainterDelegate.get()];
    [m_scrollbarPainterController.get() setHorizontalScrollerImp:painter];
    if (scrollableArea()->inLiveResize())
        [painter setKnobAlpha:1];
}

void ScrollAnimatorMac::willRemoveHorizontalScrollbar(Scrollbar* scrollbar)
{
    if (!ScrollbarThemeMacCommon::isOverlayAPIAvailable())
        return;

    ScrollbarPainter painter = scrollbarPainterForScrollbar(scrollbar);
    if (!painter)
        return;

    ASSERT(m_horizontalScrollbarPainterDelegate);
    [m_horizontalScrollbarPainterDelegate.get() invalidate];
    m_horizontalScrollbarPainterDelegate = nullptr;

    [painter setDelegate:nil];
    [m_scrollbarPainterController.get() setHorizontalScrollerImp:nil];
}

bool ScrollAnimatorMac::shouldScrollbarParticipateInHitTesting(Scrollbar* scrollbar)
{
    // Non-overlay scrollbars should always participate in hit testing.
    if (ScrollbarThemeMacCommon::recommendedScrollerStyle() != NSScrollerStyleOverlay)
        return true;

    if (!ScrollbarThemeMacCommon::isOverlayAPIAvailable())
        return true;

    if (scrollbar->isAlphaLocked())
        return true;

    // Overlay scrollbars should participate in hit testing whenever they are at all visible.
    ScrollbarPainter painter = scrollbarPainterForScrollbar(scrollbar);
    if (!painter)
        return false;
    return [painter knobAlpha] > 0;
}

void ScrollAnimatorMac::notifyContentAreaScrolled(const FloatSize& delta)
{
    if (!ScrollbarThemeMacCommon::isOverlayAPIAvailable())
        return;

    // This function is called when a page is going into the page cache, but the page
    // isn't really scrolling in that case. We should only pass the message on to the
    // ScrollbarPainterController when we're really scrolling on an active page.
    if (scrollableArea()->scrollbarsCanBeActive())
        sendContentAreaScrolledSoon(delta);
}

void ScrollAnimatorMac::cancelAnimations()
{
    m_haveScrolledSincePageLoad = false;

    if (ScrollbarThemeMacCommon::isOverlayAPIAvailable()) {
        if (scrollbarPaintTimerIsActive())
            stopScrollbarPaintTimer();
        [m_horizontalScrollbarPainterDelegate.get() cancelAnimations];
        [m_verticalScrollbarPainterDelegate.get() cancelAnimations];
    }
}

void ScrollAnimatorMac::handleWheelEventPhase(PlatformWheelEventPhase phase)
{
    // This may not have been set to true yet if the wheel event was handled by the ScrollingTree,
    // So set it to true here.
    m_haveScrolledSincePageLoad = true;

    if (phase == PlatformWheelEventPhaseBegan)
        didBeginScrollGesture();
    else if (phase == PlatformWheelEventPhaseEnded || phase == PlatformWheelEventPhaseCancelled)
        didEndScrollGesture();
    else if (phase == PlatformWheelEventPhaseMayBegin)
        mayBeginScrollGesture();
}

void ScrollAnimatorMac::setIsActive()
{
    if (!ScrollbarThemeMacCommon::isOverlayAPIAvailable())
        return;

    if (!m_needsScrollerStyleUpdate)
        return;

    updateScrollerStyle();
}

void ScrollAnimatorMac::updateScrollerStyle()
{
    if (!ScrollbarThemeMacCommon::isOverlayAPIAvailable())
        return;

    if (!scrollableArea()->scrollbarsCanBeActive()) {
        m_needsScrollerStyleUpdate = true;
        return;
    }

    ScrollbarThemeMacOverlayAPI* macTheme = macOverlayScrollbarTheme();
    if (!macTheme) {
        m_needsScrollerStyleUpdate = false;
        return;
    }

    NSScrollerStyle newStyle = [m_scrollbarPainterController.get() scrollerStyle];

    if (Scrollbar* verticalScrollbar = scrollableArea()->verticalScrollbar()) {
        verticalScrollbar->invalidate();

        ScrollbarPainter oldVerticalPainter = [m_scrollbarPainterController.get() verticalScrollerImp];
        ScrollbarPainter newVerticalPainter = [NSClassFromString(@"NSScrollerImp") scrollerImpWithStyle:newStyle
                                                                                    controlSize:(NSControlSize)verticalScrollbar->controlSize()
                                                                                    horizontal:NO
                                                                                    replacingScrollerImp:oldVerticalPainter];
        [m_scrollbarPainterController.get() setVerticalScrollerImp:newVerticalPainter];
        macTheme->setNewPainterForScrollbar(verticalScrollbar, newVerticalPainter);

        // The different scrollbar styles have different thicknesses, so we must re-set the
        // frameRect to the new thickness, and the re-layout below will ensure the position
        // and length are properly updated.
        int thickness = macTheme->scrollbarThickness(verticalScrollbar->controlSize());
        verticalScrollbar->setFrameRect(IntRect(0, 0, thickness, thickness));
    }

    if (Scrollbar* horizontalScrollbar = scrollableArea()->horizontalScrollbar()) {
        horizontalScrollbar->invalidate();

        ScrollbarPainter oldHorizontalPainter = [m_scrollbarPainterController.get() horizontalScrollerImp];
        ScrollbarPainter newHorizontalPainter = [NSClassFromString(@"NSScrollerImp") scrollerImpWithStyle:newStyle
                                                                                    controlSize:(NSControlSize)horizontalScrollbar->controlSize()
                                                                                    horizontal:YES
                                                                                    replacingScrollerImp:oldHorizontalPainter];
        [m_scrollbarPainterController.get() setHorizontalScrollerImp:newHorizontalPainter];
        macTheme->setNewPainterForScrollbar(horizontalScrollbar, newHorizontalPainter);

        // The different scrollbar styles have different thicknesses, so we must re-set the
        // frameRect to the new thickness, and the re-layout below will ensure the position
        // and length are properly updated.
        int thickness = macTheme->scrollbarThickness(horizontalScrollbar->controlSize());
        horizontalScrollbar->setFrameRect(IntRect(0, 0, thickness, thickness));
    }

    // If m_needsScrollerStyleUpdate is true, then the page is restoring from the page cache, and
    // a relayout will happen on its own. Otherwise, we must initiate a re-layout ourselves.
    if (!m_needsScrollerStyleUpdate)
        scrollableArea()->scrollbarStyleChanged();

    m_needsScrollerStyleUpdate = false;
}

void ScrollAnimatorMac::startScrollbarPaintTimer()
{
    m_initialScrollbarPaintTimer.startOneShot(0.1, FROM_HERE);
}

bool ScrollAnimatorMac::scrollbarPaintTimerIsActive() const
{
    return m_initialScrollbarPaintTimer.isActive();
}

void ScrollAnimatorMac::stopScrollbarPaintTimer()
{
    m_initialScrollbarPaintTimer.stop();
}

void ScrollAnimatorMac::initialScrollbarPaintTimerFired(Timer<ScrollAnimatorMac>*)
{
    if (ScrollbarThemeMacCommon::isOverlayAPIAvailable()) {
        // To force the scrollbars to flash, we have to call hide first. Otherwise, the ScrollbarPainterController
        // might think that the scrollbars are already showing and bail early.
        [m_scrollbarPainterController.get() hideOverlayScrollers];
        [m_scrollbarPainterController.get() flashScrollers];
    }
}

void ScrollAnimatorMac::sendContentAreaScrolledSoon(const FloatSize& delta)
{
    m_contentAreaScrolledTimerScrollDelta = delta;

    if (!m_sendContentAreaScrolledTimer.isActive())
        m_sendContentAreaScrolledTimer.startOneShot(0, FROM_HERE);
}

void ScrollAnimatorMac::sendContentAreaScrolledTimerFired(Timer<ScrollAnimatorMac>*)
{
    if (supportsContentAreaScrolledInDirection()) {
        [m_scrollbarPainterController.get() contentAreaScrolledInDirection:NSMakePoint(m_contentAreaScrolledTimerScrollDelta.width(), m_contentAreaScrolledTimerScrollDelta.height())];
        m_contentAreaScrolledTimerScrollDelta = FloatSize();
    } else
        [m_scrollbarPainterController.get() contentAreaScrolled];
}

void ScrollAnimatorMac::setVisibleScrollerThumbRect(const IntRect& scrollerThumb)
{
    IntRect rectInViewCoordinates = scrollerThumb;
    if (Scrollbar* verticalScrollbar = m_scrollableArea->verticalScrollbar())
        rectInViewCoordinates = verticalScrollbar->convertToContainingView(scrollerThumb);

    if (rectInViewCoordinates == m_visibleScrollerThumbRect)
        return;

    m_visibleScrollerThumbRect = rectInViewCoordinates;
}

bool ScrollAnimatorMac::canUseCoordinatedScrollbar() {
    return ScrollbarThemeMacCommon::isOverlayAPIAvailable();
}

} // namespace blink
