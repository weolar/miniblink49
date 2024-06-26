// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UI_EVENTS_BLINK_SYNCHRONOUS_INPUT_HANDLER_PROXY_H_
#define UI_EVENTS_BLINK_SYNCHRONOUS_INPUT_HANDLER_PROXY_H_

#include "base/time/time.h"

namespace gfx {
class ScrollOffset;
class SizeF;
}

namespace ui {

class SynchronousInputHandler {
public:
    virtual ~SynchronousInputHandler() { }

    // Informs the Android WebView embedder that a fling animation is running, and
    // that it should call SynchronouslyAnimate() if it wants to execute that
    // animation. The embedder/app may choose to override and ignore the
    // request for animation.
    virtual void SetNeedsSynchronousAnimateInput() = 0;

    // Informs the Android WebView embedder of the current root scroll and page
    // scale state.
    virtual void UpdateRootLayerState(
        const gfx::ScrollOffset& total_scroll_offset,
        const gfx::ScrollOffset& max_scroll_offset,
        const gfx::SizeF& scrollable_size,
        float page_scale_factor,
        float min_page_scale_factor,
        float max_page_scale_factor)
        = 0;
};

// Android WebView requires synchronous scrolling from the WebView application.
// This interface provides support for that behaviour. The WebView embedder will
// act as the InputHandler for controlling the timing of input (fling)
// animations.
class SynchronousInputHandlerProxy {
public:
    virtual ~SynchronousInputHandlerProxy() { }

    // Tell the proxy that we will control the timing of root fling animations
    // from the SynchronousInputHandler. Once this is set, the InputHandler is
    // not requested to Animate() the InputHandlerProxy for root layer flings.
    // Instead, requests for animation will go to the SynchronousInputHandler and
    // animation ticks will only come back through SynchronouslyAnimate().
    // Non-root flings are not affected.
    virtual void SetOnlySynchronouslyAnimateRootFlings(
        SynchronousInputHandler* synchronous_input_handler)
        = 0;

    // Tick input (fling) animations. This may happen out of phase with the frame
    // timing, or not at all, as it is controlled by the WebView application. When
    // it returns, it expects the animation scroll offsets to be visible to the
    // application.
    virtual void SynchronouslyAnimate(base::TimeTicks time) = 0;

    // Called when the synchronous input handler wants to change the root scroll
    // offset. Since it has the final say, this overrides values from compositor-
    // controlled behaviour. After the offset is applied, the
    // SynchronousInputHandler should be given back the result in case it differs
    // from what was sent.
    virtual void SynchronouslySetRootScrollOffset(
        const gfx::ScrollOffset& root_offset)
        = 0;
};

} // namespace ui

#endif // UI_EVENTS_BLINK_SYNCHRONOUS_INPUT_HANDLER_PROXY_H_
