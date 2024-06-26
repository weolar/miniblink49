// Copyright 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_INPUT_INPUT_HANDLER_H_
#define CC_INPUT_INPUT_HANDLER_H_

#include "base/basictypes.h"
#include "base/memory/scoped_ptr.h"
#include "base/time/time.h"
#include "cc/base/cc_export.h"
#include "cc/input/scrollbar.h"
#include "cc/trees/swap_promise_monitor.h"

namespace gfx {
class Point;
class PointF;
class ScrollOffset;
class SizeF;
class Vector2d;
class Vector2dF;
}

namespace ui {
class LatencyInfo;
}

namespace cc {

class LayerScrollOffsetDelegate;
class ScrollElasticityHelper;

struct CC_EXPORT InputHandlerScrollResult {
    InputHandlerScrollResult();
    // Did any layer scroll as a result this ScrollBy call?
    bool did_scroll;
    // Was any of the scroll delta argument to this ScrollBy call not used?
    bool did_overscroll_root;
    // The total overscroll that has been accumulated by all ScrollBy calls that
    // have had overscroll since the last ScrollBegin call. This resets upon a
    // ScrollBy with no overscroll.
    gfx::Vector2dF accumulated_root_overscroll;
    // The amount of the scroll delta argument to this ScrollBy call that was not
    // used for scrolling.
    gfx::Vector2dF unused_scroll_delta;
};

class CC_EXPORT InputHandlerClient {
public:
    virtual ~InputHandlerClient() { }

    virtual void WillShutdown() = 0;
    virtual void Animate(base::TimeTicks time) = 0;
    virtual void MainThreadHasStoppedFlinging() = 0;
    virtual void ReconcileElasticOverscrollAndRootScroll() = 0;
    virtual void UpdateRootLayerStateForSynchronousInputHandler(
        const gfx::ScrollOffset& total_scroll_offset,
        const gfx::ScrollOffset& max_scroll_offset,
        const gfx::SizeF& scrollable_size,
        float page_scale_factor,
        float min_page_scale_factor,
        float max_page_scale_factor)
        = 0;

protected:
    InputHandlerClient() { }

private:
    DISALLOW_COPY_AND_ASSIGN(InputHandlerClient);
};

// The InputHandler is a way for the embedders to interact with the impl thread
// side of the compositor implementation. There is one InputHandler per
// LayerTreeHost. To use the input handler, implement the InputHanderClient
// interface and bind it to the handler on the compositor thread.
class CC_EXPORT InputHandler {
public:
    // Note these are used in a histogram. Do not reorder or delete existing
    // entries.
    enum ScrollStatus {
        SCROLL_ON_MAIN_THREAD = 0,
        SCROLL_STARTED,
        SCROLL_IGNORED,
        SCROLL_UNKNOWN,
        // This must be the last entry.
        ScrollStatusCount
    };
    enum ScrollInputType { GESTURE,
        WHEEL,
        ANIMATED_WHEEL,
        NON_BUBBLING_GESTURE };

    // Binds a client to this handler to receive notifications. Only one client
    // can be bound to an InputHandler. The client must live at least until the
    // handler calls WillShutdown() on the client.
    virtual void BindToClient(InputHandlerClient* client) = 0;

    // Selects a layer to be scrolled at a given point in viewport (logical
    // pixel) coordinates. Returns SCROLL_STARTED if the layer at the coordinates
    // can be scrolled, SCROLL_ON_MAIN_THREAD if the scroll event should instead
    // be delegated to the main thread, or SCROLL_IGNORED if there is nothing to
    // be scrolled at the given coordinates.
    virtual ScrollStatus ScrollBegin(const gfx::Point& viewport_point,
        ScrollInputType type)
        = 0;

    // Similar to ScrollBegin, except the hit test is skipped and scroll always
    // targets at the root layer.
    virtual ScrollStatus RootScrollBegin(ScrollInputType type) = 0;
    virtual ScrollStatus ScrollAnimated(const gfx::Point& viewport_point,
        const gfx::Vector2dF& scroll_delta)
        = 0;

    // Scroll the selected layer starting at the given position. If the scroll
    // type given to ScrollBegin was a gesture, then the scroll point and delta
    // should be in viewport (logical pixel) coordinates. Otherwise they are in
    // scrolling layer's (logical pixel) space. If there is no room to move the
    // layer in the requested direction, its first ancestor layer that can be
    // scrolled will be moved instead. The return value's |did_scroll| field is
    // set to false if no layer can be moved in the requested direction at all,
    // and set to true if any layer is moved.
    // If the scroll delta hits the root layer, and the layer can no longer move,
    // the root overscroll accumulated within this ScrollBegin() scope is reported
    // in the return value's |accumulated_overscroll| field.
    // Should only be called if ScrollBegin() returned SCROLL_STARTED.
    virtual InputHandlerScrollResult ScrollBy(
        const gfx::Point& viewport_point,
        const gfx::Vector2dF& scroll_delta)
        = 0;

    virtual bool ScrollVerticallyByPage(const gfx::Point& viewport_point,
        ScrollDirection direction)
        = 0;

    // Returns SCROLL_STARTED if a layer was being actively being scrolled,
    // SCROLL_IGNORED if not.
    virtual ScrollStatus FlingScrollBegin() = 0;

    virtual void MouseMoveAt(const gfx::Point& mouse_position) = 0;

    // Stop scrolling the selected layer. Should only be called if ScrollBegin()
    // returned SCROLL_STARTED.
    virtual void ScrollEnd() = 0;

    // Requests a callback to UpdateRootLayerStateForSynchronousInputHandler()
    // giving the current root scroll and page scale information.
    virtual void RequestUpdateForSynchronousInputHandler() = 0;

    // Called when the root scroll offset has been changed in the synchronous
    // input handler by the application (outside of input event handling).
    virtual void SetSynchronousInputHandlerRootScrollOffset(
        const gfx::ScrollOffset& root_offset)
        = 0;

    virtual void PinchGestureBegin() = 0;
    virtual void PinchGestureUpdate(float magnify_delta,
        const gfx::Point& anchor)
        = 0;
    virtual void PinchGestureEnd() = 0;

    // Request another callback to InputHandlerClient::Animate().
    virtual void SetNeedsAnimateInput() = 0;

    // Returns true if there is an active scroll on the inner viewport layer.
    virtual bool IsCurrentlyScrollingInnerViewport() const = 0;

    // Whether the layer under |viewport_point| is the currently scrolling layer.
    virtual bool IsCurrentlyScrollingLayerAt(const gfx::Point& viewport_point,
        ScrollInputType type) const = 0;

    virtual bool HaveWheelEventHandlersAt(const gfx::Point& viewport_point) = 0;

    // Whether the page should be given the opportunity to suppress scrolling by
    // consuming touch events that started at |viewport_point|.
    virtual bool DoTouchEventsBlockScrollAt(const gfx::Point& viewport_point) = 0;

    // Calling CreateLatencyInfoSwapPromiseMonitor() to get a scoped
    // LatencyInfoSwapPromiseMonitor. During the life time of the
    // LatencyInfoSwapPromiseMonitor, if SetNeedsRedraw() or SetNeedsRedrawRect()
    // is called on LayerTreeHostImpl, the original latency info will be turned
    // into a LatencyInfoSwapPromise.
    virtual scoped_ptr<SwapPromiseMonitor> CreateLatencyInfoSwapPromiseMonitor(
        ui::LatencyInfo* latency)
        = 0;

    virtual ScrollElasticityHelper* CreateScrollElasticityHelper() = 0;

protected:
    InputHandler() { }
    virtual ~InputHandler() { }

private:
    DISALLOW_COPY_AND_ASSIGN(InputHandler);
};

} // namespace cc

#endif // CC_INPUT_INPUT_HANDLER_H_
