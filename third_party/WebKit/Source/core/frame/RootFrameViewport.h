// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef RootFrameViewport_h
#define RootFrameViewport_h

#include "core/CoreExport.h"
#include "platform/scroll/ScrollableArea.h"

namespace blink {

class LayoutRect;

// ScrollableArea for the root frame's viewport. This class ties together the
// concepts of layout and visual viewports, used in pinch-to-zoom. This class
// takes two ScrollableAreas, one for the visual viewport and one for the
// layout viewport, and delegates and composes the ScrollableArea API as needed
// between them. For most scrolling APIs, this class will split the scroll up
// between the two viewports in accord with the pinch-zoom semantics. For other
// APIs that don't make sense on the combined viewport, the call is delegated to
// the layout viewport. Thus, we could say this class is a decorator on the
// FrameView scrollable area that adds pinch-zoom semantics to scrolling.
class CORE_EXPORT RootFrameViewport final : public NoBaseWillBeGarbageCollectedFinalized<RootFrameViewport>, public ScrollableArea {
    WILL_BE_USING_GARBAGE_COLLECTED_MIXIN(RootFrameViewport);
public:
    static PassOwnPtrWillBeRawPtr<RootFrameViewport> create(ScrollableArea& visualViewport, ScrollableArea& layoutViewport)
    {
        return adoptPtrWillBeNoop(new RootFrameViewport(visualViewport, layoutViewport));
    }

    DECLARE_VIRTUAL_TRACE();

    // ScrollableArea Implementation
    void setScrollPosition(const DoublePoint&, ScrollType, ScrollBehavior = ScrollBehaviorInstant) override;
    LayoutRect scrollIntoView(
        const LayoutRect& rectInContent,
        const ScrollAlignment& alignX,
        const ScrollAlignment& alignY) override;
    DoubleRect visibleContentRectDouble(IncludeScrollbarsInRect = ExcludeScrollbars) const override;
    IntRect visibleContentRect(IncludeScrollbarsInRect = ExcludeScrollbars) const override;
    bool shouldUseIntegerScrollOffset() const override;
    bool isActive() const override;
    int scrollSize(ScrollbarOrientation) const override;
    bool isScrollCornerVisible() const override;
    IntRect scrollCornerRect() const override;
    void setScrollOffset(const IntPoint&, ScrollType) override;
    void setScrollOffset(const DoublePoint&, ScrollType) override;
    IntPoint scrollPosition() const override;
    DoublePoint scrollPositionDouble() const override;
    IntPoint minimumScrollPosition() const override;
    IntPoint maximumScrollPosition() const override;
    DoublePoint maximumScrollPositionDouble() const override;
    IntSize contentsSize() const override;
    bool scrollbarsCanBeActive() const override;
    IntRect scrollableAreaBoundingBox() const override;
    bool userInputScrollable(ScrollbarOrientation) const override;
    bool shouldPlaceVerticalScrollbarOnLeft() const override;
    void invalidateScrollbarRect(Scrollbar*, const IntRect&) override;
    void invalidateScrollCornerRect(const IntRect&) override;
    GraphicsLayer* layerForContainer() const override;
    GraphicsLayer* layerForScrolling() const override;
    GraphicsLayer* layerForHorizontalScrollbar() const override;
    GraphicsLayer* layerForVerticalScrollbar() const override;
    ScrollResultOneDimensional userScroll(ScrollDirectionPhysical, ScrollGranularity, float delta = 1) override;
    bool scrollAnimatorEnabled() const override;
    HostWindow* hostWindow() const override;
    void serviceScrollAnimations(double) override;
    void updateCompositorScrollAnimations() override;
    virtual ScrollBehavior scrollBehaviorStyle() const override;
    // TODO(bokan): This method should be removed. It should be replaced by
    // making EventHandler::handleWheelEvent unpack the WheelEvent and make a
    // call to this class' scroll method.
    ScrollResult handleWheel(const PlatformWheelEvent&) override;

private:
    RootFrameViewport(ScrollableArea& visualViewport, ScrollableArea& layoutViewport);

    DoublePoint scrollOffsetFromScrollAnimators() const;

    // If either of the layout or visual viewports are scrolled explicitly (i.e. not
    // through this class), their updated offset will not be reflected in this class'
    // animator so use this method to pull updated values when necessary.
    void updateScrollAnimator();

    ScrollableArea& visualViewport() const { ASSERT(m_visualViewport); return *m_visualViewport; }
    ScrollableArea& layoutViewport() const { ASSERT(m_layoutViewport); return *m_layoutViewport; }

    RawPtrWillBeMember<ScrollableArea> m_visualViewport;
    RawPtrWillBeMember<ScrollableArea> m_layoutViewport;
};

} // namespace blink

#endif // RootFrameViewport_h
