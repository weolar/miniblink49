// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UI_EVENTS_BLINK_INPUT_HANDLER_PROXY_CLIENT_H_
#define UI_EVENTS_BLINK_INPUT_HANDLER_PROXY_CLIENT_H_

namespace blink {
class WebGestureCurve;
struct WebActiveWheelFlingParameters;
struct WebFloatPoint;
struct WebSize;
}

namespace ui {

// All callbacks invoked from the compositor thread.
class InputHandlerProxyClient {
public:
    // Called just before the InputHandlerProxy shuts down.
    virtual void WillShutdown() = 0;

    // Transfers an active wheel fling animation initiated by a previously
    // handled input event out to the client.
    virtual void TransferActiveWheelFlingAnimation(
        const blink::WebActiveWheelFlingParameters& params)
        = 0;

    // Creates a new fling animation curve instance for device |device_source|
    // with |velocity| and already scrolled |cumulative_scroll| pixels.
    virtual blink::WebGestureCurve* CreateFlingAnimationCurve(
        blink::WebGestureDevice device_source,
        const blink::WebFloatPoint& velocity,
        const blink::WebSize& cumulative_scroll)
        = 0;

    virtual void DidOverscroll(
        const gfx::Vector2dF& accumulated_overscroll,
        const gfx::Vector2dF& latest_overscroll_delta,
        const gfx::Vector2dF& current_fling_velocity,
        const gfx::PointF& causal_event_viewport_point)
        = 0;

    virtual void DidStopFlinging() = 0;

    virtual void DidAnimateForInput() = 0;

protected:
    virtual ~InputHandlerProxyClient() { }
};

} // namespace ui

#endif // UI_EVENTS_BLINK_INPUT_HANDLER_PROXY_CLIENT_H_
