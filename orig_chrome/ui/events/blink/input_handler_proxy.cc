// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/events/blink/input_handler_proxy.h"

#include <stddef.h>

#include <algorithm>

#include "base/auto_reset.h"
#include "base/command_line.h"
#include "base/location.h"
#include "base/logging.h"
#include "base/metrics/histogram.h"
#include "base/single_thread_task_runner.h"
#include "base/thread_task_runner_handle.h"
#include "base/trace_event/trace_event.h"
#include "cc/input/main_thread_scrolling_reason.h"
#include "third_party/WebKit/public/web/WebInputEvent.h"
#include "ui/events/blink/input_handler_proxy_client.h"
#include "ui/events/blink/input_scroll_elasticity_controller.h"
#include "ui/events/latency_info.h"
#include "ui/gfx/geometry/point_conversions.h"

using blink::WebFloatPoint;
using blink::WebFloatSize;
using blink::WebGestureEvent;
using blink::WebInputEvent;
using blink::WebMouseEvent;
using blink::WebMouseWheelEvent;
using blink::WebPoint;
using blink::WebTouchEvent;
using blink::WebTouchPoint;

namespace {

const int32_t kEventDispositionUndefined = -1;

// Maximum time between a fling event's timestamp and the first |Animate| call
// for the fling curve to use the fling timestamp as the initial animation time.
// Two frames allows a minor delay between event creation and the first animate.
const double kMaxSecondsFromFlingTimestampToFirstAnimate = 2. / 60.;

// Threshold for determining whether a fling scroll delta should have caused the
// client to scroll.
const float kScrollEpsilon = 0.1f;

// Minimum fling velocity required for the active fling and new fling for the
// two to accumulate.
const double kMinBoostFlingSpeedSquare = 350. * 350.;

// Minimum velocity for the active touch scroll to preserve (boost) an active
// fling for which cancellation has been deferred.
const double kMinBoostTouchScrollSpeedSquare = 150 * 150.;

// Timeout window after which the active fling will be cancelled if no animation
// ticks, scrolls or flings of sufficient velocity relative to the current fling
// are received. The default value on Android native views is 40ms, but we use a
// slightly increased value to accomodate small IPC message delays.
const double kFlingBoostTimeoutDelaySeconds = 0.05;

gfx::Vector2dF ToClientScrollIncrement(const WebFloatSize& increment)
{
    return gfx::Vector2dF(-increment.width, -increment.height);
}

double InSecondsF(const base::TimeTicks& time)
{
    return (time - base::TimeTicks()).InSecondsF();
}

bool ShouldSuppressScrollForFlingBoosting(
    const gfx::Vector2dF& current_fling_velocity,
    const WebGestureEvent& scroll_update_event,
    double time_since_last_boost_event,
    double time_since_last_fling_animate)
{
    DCHECK_EQ(WebInputEvent::GestureScrollUpdate, scroll_update_event.type);

    gfx::Vector2dF dx(scroll_update_event.data.scrollUpdate.deltaX,
        scroll_update_event.data.scrollUpdate.deltaY);
    if (gfx::DotProduct(current_fling_velocity, dx) <= 0)
        return false;

    if (time_since_last_fling_animate > kFlingBoostTimeoutDelaySeconds)
        return false;

    if (time_since_last_boost_event < 0.001)
        return true;

    // TODO(jdduke): Use |scroll_update_event.data.scrollUpdate.velocity{X,Y}|.
    // The scroll must be of sufficient velocity to maintain the active fling.
    const gfx::Vector2dF scroll_velocity = gfx::ScaleVector2d(dx, 1. / time_since_last_boost_event);
    if (scroll_velocity.LengthSquared() < kMinBoostTouchScrollSpeedSquare)
        return false;

    return true;
}

bool ShouldBoostFling(const gfx::Vector2dF& current_fling_velocity,
    const WebGestureEvent& fling_start_event)
{
    DCHECK_EQ(WebInputEvent::GestureFlingStart, fling_start_event.type);

    gfx::Vector2dF new_fling_velocity(
        fling_start_event.data.flingStart.velocityX,
        fling_start_event.data.flingStart.velocityY);

    if (gfx::DotProduct(current_fling_velocity, new_fling_velocity) <= 0)
        return false;

    if (current_fling_velocity.LengthSquared() < kMinBoostFlingSpeedSquare)
        return false;

    if (new_fling_velocity.LengthSquared() < kMinBoostFlingSpeedSquare)
        return false;

    return true;
}

WebGestureEvent ObtainGestureScrollBegin(const WebGestureEvent& event)
{
    WebGestureEvent scroll_begin_event = event;
    scroll_begin_event.type = WebInputEvent::GestureScrollBegin;
    scroll_begin_event.data.scrollBegin.deltaXHint = 0;
    scroll_begin_event.data.scrollBegin.deltaYHint = 0;
    return scroll_begin_event;
}

cc::ScrollState CreateScrollStateForGesture(const WebGestureEvent& event)
{
    cc::ScrollStateData scroll_state_data;
    switch (event.type) {
    case WebInputEvent::GestureScrollBegin:
        scroll_state_data.start_position_x = event.x;
        scroll_state_data.start_position_y = event.y;
        scroll_state_data.is_beginning = true;
        break;
    case WebInputEvent::GestureFlingStart:
        scroll_state_data.velocity_x = event.data.flingStart.velocityX;
        scroll_state_data.velocity_y = event.data.flingStart.velocityY;
        scroll_state_data.is_in_inertial_phase = true;
        break;
    case WebInputEvent::GestureScrollUpdate:
        scroll_state_data.delta_x = -event.data.scrollUpdate.deltaX;
        scroll_state_data.delta_y = -event.data.scrollUpdate.deltaY;
        scroll_state_data.velocity_x = event.data.scrollUpdate.velocityX;
        scroll_state_data.velocity_y = event.data.scrollUpdate.velocityY;
        scroll_state_data.is_in_inertial_phase = event.data.scrollUpdate.inertial;
        break;
    case WebInputEvent::GestureScrollEnd:
    case WebInputEvent::GestureFlingCancel:
        scroll_state_data.is_ending = true;
        break;
    default:
        NOTREACHED();
        break;
    }
    return cc::ScrollState(scroll_state_data);
}

void ReportInputEventLatencyUma(const WebInputEvent& event,
    const ui::LatencyInfo& latency_info)
{
    if (!(event.type == WebInputEvent::GestureScrollBegin || event.type == WebInputEvent::GestureScrollUpdate || event.type == WebInputEvent::GesturePinchBegin || event.type == WebInputEvent::GesturePinchUpdate || event.type == WebInputEvent::GestureFlingStart)) {
        return;
    }

    ui::LatencyInfo::LatencyMap::const_iterator it = latency_info.latency_components().find(std::make_pair(
        ui::INPUT_EVENT_LATENCY_ORIGINAL_COMPONENT, 0));

    if (it == latency_info.latency_components().end())
        return;

    base::TimeDelta delta = base::TimeTicks::Now() - it->second.event_time;
    for (size_t i = 0; i < it->second.event_count; ++i) {
        switch (event.type) {
        case blink::WebInputEvent::GestureScrollBegin:
            UMA_HISTOGRAM_CUSTOM_COUNTS(
                "Event.Latency.RendererImpl.GestureScrollBegin",
                delta.InMicroseconds(), 1, 1000000, 100);
            break;
        case blink::WebInputEvent::GestureScrollUpdate:
            UMA_HISTOGRAM_CUSTOM_COUNTS(
                // So named for historical reasons.
                "Event.Latency.RendererImpl.GestureScroll2",
                delta.InMicroseconds(), 1, 1000000, 100);
            break;
        case blink::WebInputEvent::GesturePinchBegin:
            UMA_HISTOGRAM_CUSTOM_COUNTS(
                "Event.Latency.RendererImpl.GesturePinchBegin",
                delta.InMicroseconds(), 1, 1000000, 100);
            break;
        case blink::WebInputEvent::GesturePinchUpdate:
            UMA_HISTOGRAM_CUSTOM_COUNTS(
                "Event.Latency.RendererImpl.GesturePinchUpdate",
                delta.InMicroseconds(), 1, 1000000, 100);
            break;
        case blink::WebInputEvent::GestureFlingStart:
            UMA_HISTOGRAM_CUSTOM_COUNTS(
                "Event.Latency.RendererImpl.GestureFlingStart",
                delta.InMicroseconds(), 1, 1000000, 100);
            break;
        default:
            NOTREACHED();
            break;
        }
    }
}

} // namespace

namespace ui {

InputHandlerProxy::InputHandlerProxy(cc::InputHandler* input_handler,
    InputHandlerProxyClient* client)
    : client_(client)
    , input_handler_(input_handler)
    , deferred_fling_cancel_time_seconds_(0)
    , synchronous_input_handler_(nullptr)
    , allow_root_animate_(true)
    ,
#ifndef NDEBUG
    expect_scroll_update_end_(false)
    ,
#endif
    gesture_scroll_on_impl_thread_(false)
    , gesture_pinch_on_impl_thread_(false)
    , fling_may_be_active_on_main_thread_(false)
    , disallow_horizontal_fling_scroll_(false)
    , disallow_vertical_fling_scroll_(false)
    , has_fling_animation_started_(false)
    , smooth_scroll_enabled_(false)
    , uma_latency_reporting_enabled_(base::TimeTicks::IsHighResolution())
    , use_gesture_events_for_mouse_wheel_(true)
    , touch_start_result_(kEventDispositionUndefined)
{
    DCHECK(client);
    input_handler_->BindToClient(this);
    cc::ScrollElasticityHelper* scroll_elasticity_helper = input_handler_->CreateScrollElasticityHelper();
    if (scroll_elasticity_helper) {
        scroll_elasticity_controller_.reset(
            new InputScrollElasticityController(scroll_elasticity_helper));
    }
}

InputHandlerProxy::~InputHandlerProxy() { }

void InputHandlerProxy::WillShutdown()
{
    scroll_elasticity_controller_.reset();
    input_handler_ = NULL;
    client_->WillShutdown();
}

InputHandlerProxy::EventDisposition
InputHandlerProxy::HandleInputEventWithLatencyInfo(
    const WebInputEvent& event,
    ui::LatencyInfo* latency_info)
{
    DCHECK(input_handler_);

    if (uma_latency_reporting_enabled_)
        ReportInputEventLatencyUma(event, *latency_info);

    TRACE_EVENT_WITH_FLOW1("input,benchmark",
        "LatencyInfo.Flow",
        TRACE_ID_DONT_MANGLE(latency_info->trace_id()),
        TRACE_EVENT_FLAG_FLOW_IN | TRACE_EVENT_FLAG_FLOW_OUT,
        "step", "HandleInputEventImpl");

    scoped_ptr<cc::SwapPromiseMonitor> latency_info_swap_promise_monitor = input_handler_->CreateLatencyInfoSwapPromiseMonitor(latency_info);
    InputHandlerProxy::EventDisposition disposition = HandleInputEvent(event);
    return disposition;
}

InputHandlerProxy::EventDisposition InputHandlerProxy::HandleInputEvent(
    const WebInputEvent& event)
{
    DCHECK(input_handler_);

    if (FilterInputEventForFlingBoosting(event))
        return DID_HANDLE;

    switch (event.type) {
    case WebInputEvent::MouseWheel:
        return HandleMouseWheel(static_cast<const WebMouseWheelEvent&>(event));

    case WebInputEvent::GestureScrollBegin:
        return HandleGestureScrollBegin(
            static_cast<const WebGestureEvent&>(event));

    case WebInputEvent::GestureScrollUpdate:
        return HandleGestureScrollUpdate(
            static_cast<const WebGestureEvent&>(event));

    case WebInputEvent::GestureScrollEnd:
        return HandleGestureScrollEnd(static_cast<const WebGestureEvent&>(event));

    case WebInputEvent::GesturePinchBegin: {
        DCHECK(!gesture_pinch_on_impl_thread_);
        const WebGestureEvent& gesture_event = static_cast<const WebGestureEvent&>(event);
        if (gesture_event.sourceDevice == blink::WebGestureDeviceTouchpad && input_handler_->GetEventListenerProperties(cc::EventListenerClass::kMouseWheel) != cc::EventListenerProperties::kNone) {
            return DID_NOT_HANDLE;
        } else {
            input_handler_->PinchGestureBegin();
            gesture_pinch_on_impl_thread_ = true;
            return DID_HANDLE;
        }
    }

    case WebInputEvent::GesturePinchEnd:
        if (gesture_pinch_on_impl_thread_) {
            gesture_pinch_on_impl_thread_ = false;
            input_handler_->PinchGestureEnd();
            return DID_HANDLE;
        } else {
            return DID_NOT_HANDLE;
        }

    case WebInputEvent::GesturePinchUpdate: {
        if (gesture_pinch_on_impl_thread_) {
            const WebGestureEvent& gesture_event = static_cast<const WebGestureEvent&>(event);
            if (gesture_event.data.pinchUpdate.zoomDisabled)
                return DROP_EVENT;
            input_handler_->PinchGestureUpdate(
                gesture_event.data.pinchUpdate.scale,
                gfx::Point(gesture_event.x, gesture_event.y));
            return DID_HANDLE;
        } else {
            return DID_NOT_HANDLE;
        }
    }

    case WebInputEvent::GestureFlingStart:
        return HandleGestureFlingStart(
            *static_cast<const WebGestureEvent*>(&event));

    case WebInputEvent::GestureFlingCancel:
        if (CancelCurrentFling())
            return DID_HANDLE;
        else if (!fling_may_be_active_on_main_thread_)
            return DROP_EVENT;
        return DID_NOT_HANDLE;

    case WebInputEvent::TouchStart:
        return HandleTouchStart(static_cast<const WebTouchEvent&>(event));

    case WebInputEvent::TouchMove:
        return HandleTouchMove(static_cast<const WebTouchEvent&>(event));

    case WebInputEvent::TouchEnd:
        return HandleTouchEnd(static_cast<const WebTouchEvent&>(event));

    case WebInputEvent::MouseMove: {
        const WebMouseEvent& mouse_event = static_cast<const WebMouseEvent&>(event);
        // TODO(tony): Ignore when mouse buttons are down?
        // TODO(davemoore): This should never happen, but bug #326635 showed some
        // surprising crashes.
        CHECK(input_handler_);
        input_handler_->MouseMoveAt(gfx::Point(mouse_event.x, mouse_event.y));
        return DID_NOT_HANDLE;
    }

    default:
        if (WebInputEvent::isKeyboardEventType(event.type)) {
            // Only call |CancelCurrentFling()| if a fling was active, as it will
            // otherwise disrupt an in-progress touch scroll.
            if (fling_curve_)
                CancelCurrentFling();
        }
        break;
    }

    return DID_NOT_HANDLE;
}

void RecordMainThreadScrollingReasons(WebInputEvent::Type type,
    uint32_t reasons)
{
    static const char* kGestureHistogramName = "Renderer4.MainThreadGestureScrollReason";
    static const char* kWheelHistogramName = "Renderer4.MainThreadWheelScrollReason";

    DCHECK(type == WebInputEvent::GestureScrollBegin || type == WebInputEvent::MouseWheel);

    if (type != WebInputEvent::GestureScrollBegin && type != WebInputEvent::MouseWheel) {
        return;
    }

    if (reasons == cc::MainThreadScrollingReason::kNotScrollingOnMain) {
        if (type == WebInputEvent::GestureScrollBegin) {
            UMA_HISTOGRAM_ENUMERATION(
                kGestureHistogramName,
                cc::MainThreadScrollingReason::kNotScrollingOnMain,
                cc::MainThreadScrollingReason::kMainThreadScrollingReasonCount);
        } else {
            UMA_HISTOGRAM_ENUMERATION(
                kWheelHistogramName,
                cc::MainThreadScrollingReason::kNotScrollingOnMain,
                cc::MainThreadScrollingReason::kMainThreadScrollingReasonCount);
        }
    }

    for (uint32_t i = 0;
         i < cc::MainThreadScrollingReason::kMainThreadScrollingReasonCount - 1;
         ++i) {
        unsigned val = 1 << i;
        if (reasons & val) {
            if (type == WebInputEvent::GestureScrollBegin) {
                UMA_HISTOGRAM_ENUMERATION(
                    kGestureHistogramName, i + 1,
                    cc::MainThreadScrollingReason::kMainThreadScrollingReasonCount);
            } else {
                UMA_HISTOGRAM_ENUMERATION(
                    kWheelHistogramName, i + 1,
                    cc::MainThreadScrollingReason::kMainThreadScrollingReasonCount);
            }
        }
    }
}

bool InputHandlerProxy::ShouldAnimate(
    const blink::WebMouseWheelEvent& event) const
{
#if defined(OS_MACOSX)
    // Mac does not smooth scroll wheel events (crbug.com/574283).
    return false;
#else
    return smooth_scroll_enabled_ && !event.hasPreciseScrollingDeltas;
#endif
}

InputHandlerProxy::EventDisposition InputHandlerProxy::HandleMouseWheel(
    const WebMouseWheelEvent& wheel_event)
{
    if (use_gesture_events_for_mouse_wheel_) {
        cc::EventListenerProperties properties = input_handler_->GetEventListenerProperties(
            cc::EventListenerClass::kMouseWheel);
        switch (properties) {
        case cc::EventListenerProperties::kPassive:
            return DID_HANDLE_NON_BLOCKING;
        case cc::EventListenerProperties::kBlockingAndPassive:
        case cc::EventListenerProperties::kBlocking:
            return DID_NOT_HANDLE;
        case cc::EventListenerProperties::kNone:
            return DROP_EVENT;
        default:
            NOTREACHED();
            return DROP_EVENT;
        }
    }
    return ScrollByMouseWheel(wheel_event);
}

InputHandlerProxy::EventDisposition InputHandlerProxy::ScrollByMouseWheel(
    const WebMouseWheelEvent& wheel_event)
{
    InputHandlerProxy::EventDisposition result = DID_NOT_HANDLE;
    cc::InputHandlerScrollResult scroll_result;

    // TODO(ccameron): The rail information should be pushed down into
    // InputHandler.
    gfx::Vector2dF scroll_delta(
        wheel_event.railsMode != WebInputEvent::RailsModeVertical
            ? -wheel_event.deltaX
            : 0,
        wheel_event.railsMode != WebInputEvent::RailsModeHorizontal
            ? -wheel_event.deltaY
            : 0);

    if (wheel_event.scrollByPage) {
        // TODO(jamesr): We don't properly handle scroll by page in the compositor
        // thread, so punt it to the main thread. http://crbug.com/236639
        result = DID_NOT_HANDLE;
        RecordMainThreadScrollingReasons(
            wheel_event.type, cc::MainThreadScrollingReason::kPageBasedScrolling);

    } else if (!wheel_event.canScroll) {
        // Wheel events with |canScroll| == false will not trigger scrolling,
        // only event handlers.  Forward to the main thread.
        result = DID_NOT_HANDLE;
    } else if (ShouldAnimate(wheel_event)) {
        cc::InputHandler::ScrollStatus scroll_status = input_handler_->ScrollAnimated(gfx::Point(wheel_event.x, wheel_event.y),
            scroll_delta);

        RecordMainThreadScrollingReasons(
            wheel_event.type, scroll_status.main_thread_scrolling_reasons);

        switch (scroll_status.thread) {
        case cc::InputHandler::SCROLL_ON_IMPL_THREAD:
            result = DID_HANDLE;
            break;
        case cc::InputHandler::SCROLL_IGNORED:
            result = DROP_EVENT;
            break;
        default:
            result = DID_NOT_HANDLE;
            break;
        }
    } else {
        cc::ScrollStateData scroll_state_begin_data;
        scroll_state_begin_data.start_position_x = wheel_event.x;
        scroll_state_begin_data.start_position_y = wheel_event.y;
        scroll_state_begin_data.is_beginning = true;
        cc::ScrollState scroll_state_begin(scroll_state_begin_data);
        cc::InputHandler::ScrollStatus scroll_status = input_handler_->ScrollBegin(
            &scroll_state_begin, cc::InputHandler::WHEEL);

        RecordMainThreadScrollingReasons(
            wheel_event.type, scroll_status.main_thread_scrolling_reasons);

        switch (scroll_status.thread) {
        case cc::InputHandler::SCROLL_ON_IMPL_THREAD: {
            TRACE_EVENT_INSTANT2("input",
                "InputHandlerProxy::handle_input wheel scroll",
                TRACE_EVENT_SCOPE_THREAD, "deltaX",
                scroll_delta.x(), "deltaY", scroll_delta.y());

            cc::ScrollStateData scroll_state_update_data;
            scroll_state_update_data.delta_x = scroll_delta.x();
            scroll_state_update_data.delta_y = scroll_delta.y();
            scroll_state_update_data.start_position_x = wheel_event.x;
            scroll_state_update_data.start_position_y = wheel_event.y;
            cc::ScrollState scroll_state_update(scroll_state_update_data);

            scroll_result = input_handler_->ScrollBy(&scroll_state_update);
            HandleOverscroll(gfx::Point(wheel_event.x, wheel_event.y),
                scroll_result);

            cc::ScrollStateData scroll_state_end_data;
            scroll_state_end_data.is_ending = true;
            cc::ScrollState scroll_state_end(scroll_state_end_data);
            input_handler_->ScrollEnd(&scroll_state_end);

            result = scroll_result.did_scroll ? DID_HANDLE : DROP_EVENT;
            break;
        }
        case cc::InputHandler::SCROLL_IGNORED:
            // TODO(jamesr): This should be DROP_EVENT, but in cases where we fail
            // to properly sync scrollability it's safer to send the event to the
            // main thread. Change back to DROP_EVENT once we have synchronization
            // bugs sorted out.
            result = DID_NOT_HANDLE;
            break;
        case cc::InputHandler::SCROLL_UNKNOWN:
        case cc::InputHandler::SCROLL_ON_MAIN_THREAD:
            result = DID_NOT_HANDLE;
            break;
        }
    }

    // Send the event and its disposition to the elasticity controller to update
    // the over-scroll animation. If the event is to be handled on the main
    // thread, the event and its disposition will be sent to the elasticity
    // controller after being handled on the main thread.
    if (scroll_elasticity_controller_ && result != DID_NOT_HANDLE) {
        // Note that the call to the elasticity controller is made asynchronously,
        // to minimize divergence between main thread and impl thread event
        // handling paths.
        base::ThreadTaskRunnerHandle::Get()->PostTask(
            FROM_HERE,
            base::Bind(&InputScrollElasticityController::ObserveWheelEventAndResult,
                scroll_elasticity_controller_->GetWeakPtr(), wheel_event,
                scroll_result));
    }
    return result;
}

InputHandlerProxy::EventDisposition InputHandlerProxy::HandleGestureScrollBegin(
    const WebGestureEvent& gesture_event)
{
    if (gesture_scroll_on_impl_thread_)
        CancelCurrentFling();

#ifndef NDEBUG
    DCHECK(!expect_scroll_update_end_);
    expect_scroll_update_end_ = true;
#endif
    cc::ScrollState scroll_state = CreateScrollStateForGesture(gesture_event);
    cc::InputHandler::ScrollStatus scroll_status;
    if (gesture_event.data.scrollBegin.deltaHintUnits == blink::WebGestureEvent::ScrollUnits::Page) {
        scroll_status.thread = cc::InputHandler::SCROLL_ON_MAIN_THREAD;
        scroll_status.main_thread_scrolling_reasons = cc::MainThreadScrollingReason::kContinuingMainThreadScroll;
    } else if (gesture_event.data.scrollBegin.targetViewport) {
        scroll_status = input_handler_->RootScrollBegin(&scroll_state,
            cc::InputHandler::GESTURE);
    } else if (smooth_scroll_enabled_ && gesture_event.data.scrollBegin.deltaHintUnits == blink::WebGestureEvent::ScrollUnits::Pixels) {
        gfx::Vector2dF scroll_delta(-gesture_event.data.scrollBegin.deltaXHint,
            -gesture_event.data.scrollBegin.deltaYHint);
        scroll_status = input_handler_->ScrollAnimated(
            gfx::Point(gesture_event.x, gesture_event.y), scroll_delta);
    } else {
        scroll_status = input_handler_->ScrollBegin(&scroll_state, cc::InputHandler::GESTURE);
    }
    UMA_HISTOGRAM_ENUMERATION("Renderer4.CompositorScrollHitTestResult",
        scroll_status.thread,
        cc::InputHandler::LAST_SCROLL_STATUS + 1);

    RecordMainThreadScrollingReasons(gesture_event.type,
        scroll_status.main_thread_scrolling_reasons);

    switch (scroll_status.thread) {
    case cc::InputHandler::SCROLL_ON_IMPL_THREAD:
        TRACE_EVENT_INSTANT0("input",
            "InputHandlerProxy::handle_input gesture scroll",
            TRACE_EVENT_SCOPE_THREAD);
        gesture_scroll_on_impl_thread_ = true;
        return DID_HANDLE;
    case cc::InputHandler::SCROLL_UNKNOWN:
    case cc::InputHandler::SCROLL_ON_MAIN_THREAD:
        return DID_NOT_HANDLE;
    case cc::InputHandler::SCROLL_IGNORED:
        return DROP_EVENT;
    }
    return DID_NOT_HANDLE;
}

InputHandlerProxy::EventDisposition
InputHandlerProxy::HandleGestureScrollUpdate(
    const WebGestureEvent& gesture_event)
{
#ifndef NDEBUG
    DCHECK(expect_scroll_update_end_);
#endif
    if (!gesture_scroll_on_impl_thread_ && !gesture_pinch_on_impl_thread_)
        return DID_NOT_HANDLE;

    cc::ScrollState scroll_state = CreateScrollStateForGesture(gesture_event);
    gfx::Point scroll_point(gesture_event.x, gesture_event.y);
    gfx::Vector2dF scroll_delta(-gesture_event.data.scrollUpdate.deltaX,
        -gesture_event.data.scrollUpdate.deltaY);

    if (smooth_scroll_enabled_ && gesture_event.data.scrollUpdate.deltaUnits == blink::WebGestureEvent::ScrollUnits::Pixels) {
        switch (input_handler_->ScrollAnimated(scroll_point, scroll_delta).thread) {
        case cc::InputHandler::SCROLL_ON_IMPL_THREAD:
            return DID_HANDLE;
        case cc::InputHandler::SCROLL_IGNORED:
            return DROP_EVENT;
        default:
            return DID_NOT_HANDLE;
        }
    }
    cc::InputHandlerScrollResult scroll_result = input_handler_->ScrollBy(&scroll_state);
    HandleOverscroll(scroll_point, scroll_result);
    return scroll_result.did_scroll ? DID_HANDLE : DROP_EVENT;
}

InputHandlerProxy::EventDisposition InputHandlerProxy::HandleGestureScrollEnd(
    const WebGestureEvent& gesture_event)
{
#ifndef NDEBUG
    DCHECK(expect_scroll_update_end_);
    expect_scroll_update_end_ = false;
#endif
    cc::ScrollState scroll_state = CreateScrollStateForGesture(gesture_event);
    input_handler_->ScrollEnd(&scroll_state);
    if (!gesture_scroll_on_impl_thread_)
        return DID_NOT_HANDLE;
    gesture_scroll_on_impl_thread_ = false;
    return DID_HANDLE;
}

InputHandlerProxy::EventDisposition InputHandlerProxy::HandleGestureFlingStart(
    const WebGestureEvent& gesture_event)
{
    cc::ScrollState scroll_state = CreateScrollStateForGesture(gesture_event);
    cc::InputHandler::ScrollStatus scroll_status;
    scroll_status.main_thread_scrolling_reasons = cc::MainThreadScrollingReason::kNotScrollingOnMain;
    switch (gesture_event.sourceDevice) {
    case blink::WebGestureDeviceTouchpad:
        if (gesture_event.data.flingStart.targetViewport) {
            scroll_status = input_handler_->RootScrollBegin(
                &scroll_state, cc::InputHandler::NON_BUBBLING_GESTURE);
        } else {
            scroll_status = input_handler_->ScrollBegin(
                &scroll_state, cc::InputHandler::NON_BUBBLING_GESTURE);
        }
        break;
    case blink::WebGestureDeviceTouchscreen:
        if (!gesture_scroll_on_impl_thread_) {
            scroll_status.thread = cc::InputHandler::SCROLL_ON_MAIN_THREAD;
            scroll_status.main_thread_scrolling_reasons = cc::MainThreadScrollingReason::kContinuingMainThreadScroll;
        } else {
            scroll_status = input_handler_->FlingScrollBegin();
        }
        break;
    case blink::WebGestureDeviceUninitialized:
        NOTREACHED();
        return DID_NOT_HANDLE;
    }

#ifndef NDEBUG
    expect_scroll_update_end_ = false;
#endif

    switch (scroll_status.thread) {
    case cc::InputHandler::SCROLL_ON_IMPL_THREAD: {
        if (gesture_event.sourceDevice == blink::WebGestureDeviceTouchpad) {
            scroll_state.set_is_ending(true);
            input_handler_->ScrollEnd(&scroll_state);
        }

        const float vx = gesture_event.data.flingStart.velocityX;
        const float vy = gesture_event.data.flingStart.velocityY;
        current_fling_velocity_ = gfx::Vector2dF(vx, vy);
        DCHECK(!current_fling_velocity_.IsZero());
        fling_curve_.reset(client_->CreateFlingAnimationCurve(
            gesture_event.sourceDevice,
            WebFloatPoint(vx, vy),
            blink::WebSize()));
        disallow_horizontal_fling_scroll_ = !vx;
        disallow_vertical_fling_scroll_ = !vy;
        TRACE_EVENT_ASYNC_BEGIN2("input,benchmark",
            "InputHandlerProxy::HandleGestureFling::started",
            this, "vx", vx, "vy", vy);
        // Note that the timestamp will only be used to kickstart the animation if
        // its sufficiently close to the timestamp of the first call |Animate()|.
        has_fling_animation_started_ = false;
        fling_parameters_.startTime = gesture_event.timeStampSeconds;
        fling_parameters_.delta = WebFloatPoint(vx, vy);
        fling_parameters_.point = WebPoint(gesture_event.x, gesture_event.y);
        fling_parameters_.globalPoint = WebPoint(gesture_event.globalX, gesture_event.globalY);
        fling_parameters_.modifiers = gesture_event.modifiers;
        fling_parameters_.sourceDevice = gesture_event.sourceDevice;
        RequestAnimation();
        return DID_HANDLE;
    }
    case cc::InputHandler::SCROLL_UNKNOWN:
    case cc::InputHandler::SCROLL_ON_MAIN_THREAD: {
        TRACE_EVENT_INSTANT0("input",
            "InputHandlerProxy::HandleGestureFling::"
            "scroll_on_main_thread",
            TRACE_EVENT_SCOPE_THREAD);
        gesture_scroll_on_impl_thread_ = false;
        fling_may_be_active_on_main_thread_ = true;
        return DID_NOT_HANDLE;
    }
    case cc::InputHandler::SCROLL_IGNORED: {
        TRACE_EVENT_INSTANT0(
            "input",
            "InputHandlerProxy::HandleGestureFling::ignored",
            TRACE_EVENT_SCOPE_THREAD);
        gesture_scroll_on_impl_thread_ = false;
        if (gesture_event.sourceDevice == blink::WebGestureDeviceTouchpad) {
            // We still pass the curve to the main thread if there's nothing
            // scrollable, in case something
            // registers a handler before the curve is over.
            return DID_NOT_HANDLE;
        }
        return DROP_EVENT;
    }
    }
    return DID_NOT_HANDLE;
}

InputHandlerProxy::EventDisposition InputHandlerProxy::HandleTouchStart(
    const blink::WebTouchEvent& touch_event)
{
    EventDisposition result = DROP_EVENT;
    for (size_t i = 0; i < touch_event.touchesLength; ++i) {
        if (touch_event.touches[i].state != WebTouchPoint::StatePressed)
            continue;
        if (input_handler_->DoTouchEventsBlockScrollAt(
                gfx::Point(touch_event.touches[i].position.x,
                    touch_event.touches[i].position.y))) {
            result = DID_NOT_HANDLE;
            break;
        }
    }

    // If |result| is DROP_EVENT it wasn't processed above.
    if (result == DROP_EVENT) {
        switch (input_handler_->GetEventListenerProperties(
            cc::EventListenerClass::kTouch)) {
        case cc::EventListenerProperties::kPassive:
            result = DID_HANDLE_NON_BLOCKING;
            break;
        case cc::EventListenerProperties::kBlocking:
            // The touch area rects above already have checked whether it hits
            // a blocking region. Since it does not the event can be dropped.
            result = DROP_EVENT;
            break;
        case cc::EventListenerProperties::kBlockingAndPassive:
            // There is at least one passive listener that needs to possibly
            // be notified so it can't be dropped.
            result = DID_HANDLE_NON_BLOCKING;
            break;
        case cc::EventListenerProperties::kNone:
            result = DROP_EVENT;
            break;
        default:
            NOTREACHED();
            result = DROP_EVENT;
            break;
        }
    }

    // Merge |touch_start_result_| and |result| so the result has the highest
    // priority value according to the sequence; (DROP_EVENT,
    // DID_HANDLE_NON_BLOCKING, DID_NOT_HANDLE).
    if (touch_start_result_ == kEventDispositionUndefined || touch_start_result_ == DROP_EVENT || result == DID_NOT_HANDLE)
        touch_start_result_ = result;

    return result;
}

InputHandlerProxy::EventDisposition InputHandlerProxy::HandleTouchMove(
    const blink::WebTouchEvent& touch_event)
{
    if (touch_start_result_ != kEventDispositionUndefined)
        return static_cast<EventDisposition>(touch_start_result_);
    return DID_NOT_HANDLE;
}

InputHandlerProxy::EventDisposition InputHandlerProxy::HandleTouchEnd(
    const blink::WebTouchEvent& touch_event)
{
    if (touch_event.touchesLength == 1)
        touch_start_result_ = kEventDispositionUndefined;
    return DID_NOT_HANDLE;
}

bool InputHandlerProxy::FilterInputEventForFlingBoosting(
    const WebInputEvent& event)
{
    if (!WebInputEvent::isGestureEventType(event.type))
        return false;

    if (!fling_curve_) {
        DCHECK(!deferred_fling_cancel_time_seconds_);
        return false;
    }

    const WebGestureEvent& gesture_event = static_cast<const WebGestureEvent&>(event);
    if (gesture_event.type == WebInputEvent::GestureFlingCancel) {
        if (gesture_event.data.flingCancel.preventBoosting)
            return false;

        if (current_fling_velocity_.LengthSquared() < kMinBoostFlingSpeedSquare)
            return false;

        TRACE_EVENT_INSTANT0("input",
            "InputHandlerProxy::FlingBoostStart",
            TRACE_EVENT_SCOPE_THREAD);
        deferred_fling_cancel_time_seconds_ = event.timeStampSeconds + kFlingBoostTimeoutDelaySeconds;
        return true;
    }

    // A fling is either inactive or is "free spinning", i.e., has yet to be
    // interrupted by a touch gesture, in which case there is nothing to filter.
    if (!deferred_fling_cancel_time_seconds_)
        return false;

    // Gestures from a different source should immediately interrupt the fling.
    if (gesture_event.sourceDevice != fling_parameters_.sourceDevice) {
        CancelCurrentFling();
        return false;
    }

    switch (gesture_event.type) {
    case WebInputEvent::GestureTapCancel:
    case WebInputEvent::GestureTapDown:
        return false;

    case WebInputEvent::GestureScrollBegin:
        if (!input_handler_->IsCurrentlyScrollingLayerAt(
                gfx::Point(gesture_event.x, gesture_event.y),
                fling_parameters_.sourceDevice == blink::WebGestureDeviceTouchpad
                    ? cc::InputHandler::NON_BUBBLING_GESTURE
                    : cc::InputHandler::GESTURE)) {
            CancelCurrentFling();
            return false;
        }

        // TODO(jdduke): Use |gesture_event.data.scrollBegin.delta{X,Y}Hint| to
        // determine if the ScrollBegin should immediately cancel the fling.
        ExtendBoostedFlingTimeout(gesture_event);
        return true;

    case WebInputEvent::GestureScrollUpdate: {
        const double time_since_last_boost_event = event.timeStampSeconds - last_fling_boost_event_.timeStampSeconds;
        const double time_since_last_fling_animate = std::max(
            0.0, event.timeStampSeconds - InSecondsF(last_fling_animate_time_));
        if (ShouldSuppressScrollForFlingBoosting(current_fling_velocity_,
                gesture_event,
                time_since_last_boost_event,
                time_since_last_fling_animate)) {
            ExtendBoostedFlingTimeout(gesture_event);
            return true;
        }

        CancelCurrentFling();
        return false;
    }

    case WebInputEvent::GestureScrollEnd:
        // Clear the last fling boost event *prior* to fling cancellation,
        // preventing insertion of a synthetic GestureScrollBegin.
        last_fling_boost_event_ = WebGestureEvent();
        CancelCurrentFling();
        return true;

    case WebInputEvent::GestureFlingStart: {
        DCHECK_EQ(fling_parameters_.sourceDevice, gesture_event.sourceDevice);

        bool fling_boosted = fling_parameters_.modifiers == gesture_event.modifiers && ShouldBoostFling(current_fling_velocity_, gesture_event);

        gfx::Vector2dF new_fling_velocity(
            gesture_event.data.flingStart.velocityX,
            gesture_event.data.flingStart.velocityY);
        DCHECK(!new_fling_velocity.IsZero());

        if (fling_boosted)
            current_fling_velocity_ += new_fling_velocity;
        else
            current_fling_velocity_ = new_fling_velocity;

        WebFloatPoint velocity(current_fling_velocity_.x(),
            current_fling_velocity_.y());
        deferred_fling_cancel_time_seconds_ = 0;
        disallow_horizontal_fling_scroll_ = !velocity.x;
        disallow_vertical_fling_scroll_ = !velocity.y;
        last_fling_boost_event_ = WebGestureEvent();
        fling_curve_.reset(client_->CreateFlingAnimationCurve(
            gesture_event.sourceDevice,
            velocity,
            blink::WebSize()));
        fling_parameters_.startTime = gesture_event.timeStampSeconds;
        fling_parameters_.delta = velocity;
        fling_parameters_.point = WebPoint(gesture_event.x, gesture_event.y);
        fling_parameters_.globalPoint = WebPoint(gesture_event.globalX, gesture_event.globalY);

        TRACE_EVENT_INSTANT2("input",
            fling_boosted ? "InputHandlerProxy::FlingBoosted"
                          : "InputHandlerProxy::FlingReplaced",
            TRACE_EVENT_SCOPE_THREAD,
            "vx",
            current_fling_velocity_.x(),
            "vy",
            current_fling_velocity_.y());

        // The client expects balanced calls between a consumed GestureFlingStart
        // and |DidStopFlinging()|.
        client_->DidStopFlinging();
        return true;
    }

    default:
        // All other types of gestures (taps, presses, etc...) will complete the
        // deferred fling cancellation.
        CancelCurrentFling();
        return false;
    }
}

void InputHandlerProxy::ExtendBoostedFlingTimeout(
    const blink::WebGestureEvent& event)
{
    TRACE_EVENT_INSTANT0("input",
        "InputHandlerProxy::ExtendBoostedFlingTimeout",
        TRACE_EVENT_SCOPE_THREAD);
    deferred_fling_cancel_time_seconds_ = event.timeStampSeconds + kFlingBoostTimeoutDelaySeconds;
    last_fling_boost_event_ = event;
}

void InputHandlerProxy::Animate(base::TimeTicks time)
{
    // If using synchronous animate, then only expect Animate attempts started by
    // the synchronous system. Don't let the InputHandler try to Animate also.
    DCHECK(!input_handler_->IsCurrentlyScrollingInnerViewport() || allow_root_animate_);

    if (scroll_elasticity_controller_)
        scroll_elasticity_controller_->Animate(time);

    if (!fling_curve_)
        return;

    last_fling_animate_time_ = time;
    double monotonic_time_sec = InSecondsF(time);

    if (deferred_fling_cancel_time_seconds_ && monotonic_time_sec > deferred_fling_cancel_time_seconds_) {
        CancelCurrentFling();
        return;
    }

    client_->DidAnimateForInput();

    if (!has_fling_animation_started_) {
        has_fling_animation_started_ = true;
        // Guard against invalid, future or sufficiently stale start times, as there
        // are no guarantees fling event and animation timestamps are compatible.
        if (!fling_parameters_.startTime || monotonic_time_sec <= fling_parameters_.startTime || monotonic_time_sec >= fling_parameters_.startTime + kMaxSecondsFromFlingTimestampToFirstAnimate) {
            fling_parameters_.startTime = monotonic_time_sec;
            RequestAnimation();
            return;
        }
    }

    bool fling_is_active = fling_curve_->apply(monotonic_time_sec - fling_parameters_.startTime,
        this);

    if (disallow_vertical_fling_scroll_ && disallow_horizontal_fling_scroll_)
        fling_is_active = false;

    if (fling_is_active) {
        RequestAnimation();
    } else {
        TRACE_EVENT_INSTANT0("input",
            "InputHandlerProxy::animate::flingOver",
            TRACE_EVENT_SCOPE_THREAD);
        CancelCurrentFling();
    }
}

void InputHandlerProxy::MainThreadHasStoppedFlinging()
{
    fling_may_be_active_on_main_thread_ = false;
    client_->DidStopFlinging();
}

void InputHandlerProxy::ReconcileElasticOverscrollAndRootScroll()
{
    if (scroll_elasticity_controller_)
        scroll_elasticity_controller_->ReconcileStretchAndScroll();
}

void InputHandlerProxy::UpdateRootLayerStateForSynchronousInputHandler(
    const gfx::ScrollOffset& total_scroll_offset,
    const gfx::ScrollOffset& max_scroll_offset,
    const gfx::SizeF& scrollable_size,
    float page_scale_factor,
    float min_page_scale_factor,
    float max_page_scale_factor)
{
    if (synchronous_input_handler_) {
        synchronous_input_handler_->UpdateRootLayerState(
            total_scroll_offset, max_scroll_offset, scrollable_size,
            page_scale_factor, min_page_scale_factor, max_page_scale_factor);
    }
}

void InputHandlerProxy::SetOnlySynchronouslyAnimateRootFlings(
    SynchronousInputHandler* synchronous_input_handler)
{
    allow_root_animate_ = !synchronous_input_handler;
    synchronous_input_handler_ = synchronous_input_handler;
    if (synchronous_input_handler_)
        input_handler_->RequestUpdateForSynchronousInputHandler();
}

void InputHandlerProxy::SynchronouslyAnimate(base::TimeTicks time)
{
    // When this function is used, SetOnlySynchronouslyAnimate() should have been
    // previously called. IOW you should either be entirely in synchronous mode or
    // not.
    DCHECK(synchronous_input_handler_);
    DCHECK(!allow_root_animate_);
    base::AutoReset<bool> reset(&allow_root_animate_, true);
    Animate(time);
}

void InputHandlerProxy::SynchronouslySetRootScrollOffset(
    const gfx::ScrollOffset& root_offset)
{
    DCHECK(synchronous_input_handler_);
    input_handler_->SetSynchronousInputHandlerRootScrollOffset(root_offset);
}

void InputHandlerProxy::HandleOverscroll(
    const gfx::Point& causal_event_viewport_point,
    const cc::InputHandlerScrollResult& scroll_result)
{
    DCHECK(client_);
    if (!scroll_result.did_overscroll_root)
        return;

    TRACE_EVENT2("input",
        "InputHandlerProxy::DidOverscroll",
        "dx",
        scroll_result.unused_scroll_delta.x(),
        "dy",
        scroll_result.unused_scroll_delta.y());

    if (fling_curve_) {
        static const int kFlingOverscrollThreshold = 1;
        disallow_horizontal_fling_scroll_ |= std::abs(scroll_result.accumulated_root_overscroll.x()) >= kFlingOverscrollThreshold;
        disallow_vertical_fling_scroll_ |= std::abs(scroll_result.accumulated_root_overscroll.y()) >= kFlingOverscrollThreshold;
    }

    client_->DidOverscroll(scroll_result.accumulated_root_overscroll,
        scroll_result.unused_scroll_delta,
        ToClientScrollIncrement(current_fling_velocity_),
        gfx::PointF(causal_event_viewport_point));
}

bool InputHandlerProxy::CancelCurrentFling()
{
    if (CancelCurrentFlingWithoutNotifyingClient()) {
        client_->DidStopFlinging();
        return true;
    }
    return false;
}

bool InputHandlerProxy::CancelCurrentFlingWithoutNotifyingClient()
{
    bool had_fling_animation = !!fling_curve_;
    if (had_fling_animation && fling_parameters_.sourceDevice == blink::WebGestureDeviceTouchscreen) {
        cc::ScrollStateData scroll_state_data;
        scroll_state_data.is_ending = true;
        cc::ScrollState scroll_state(scroll_state_data);
        input_handler_->ScrollEnd(&scroll_state);
        TRACE_EVENT_ASYNC_END0(
            "input",
            "InputHandlerProxy::HandleGestureFling::started",
            this);
    }

    TRACE_EVENT_INSTANT1("input",
        "InputHandlerProxy::CancelCurrentFling",
        TRACE_EVENT_SCOPE_THREAD,
        "had_fling_animation",
        had_fling_animation);
    fling_curve_.reset();
    has_fling_animation_started_ = false;
    gesture_scroll_on_impl_thread_ = false;
    current_fling_velocity_ = gfx::Vector2dF();
    fling_parameters_ = blink::WebActiveWheelFlingParameters();

    if (deferred_fling_cancel_time_seconds_) {
        deferred_fling_cancel_time_seconds_ = 0;

        WebGestureEvent last_fling_boost_event = last_fling_boost_event_;
        last_fling_boost_event_ = WebGestureEvent();
        if (last_fling_boost_event.type == WebInputEvent::GestureScrollBegin || last_fling_boost_event.type == WebInputEvent::GestureScrollUpdate) {
            // Synthesize a GestureScrollBegin, as the original was suppressed.
            HandleInputEvent(ObtainGestureScrollBegin(last_fling_boost_event));
        }
    }

    return had_fling_animation;
}

void InputHandlerProxy::RequestAnimation()
{
    // When a SynchronousInputHandler is present, root flings should go through
    // it to allow it to control when or if the root fling is animated. Non-root
    // flings always go through the normal InputHandler.
    if (synchronous_input_handler_ && input_handler_->IsCurrentlyScrollingInnerViewport())
        synchronous_input_handler_->SetNeedsSynchronousAnimateInput();
    else
        input_handler_->SetNeedsAnimateInput();
}

bool InputHandlerProxy::TouchpadFlingScroll(
    const WebFloatSize& increment)
{
    InputHandlerProxy::EventDisposition disposition;
    cc::EventListenerProperties properties = input_handler_->GetEventListenerProperties(
        cc::EventListenerClass::kMouseWheel);
    switch (properties) {
    case cc::EventListenerProperties::kPassive:
        disposition = DID_HANDLE_NON_BLOCKING;
        break;
    case cc::EventListenerProperties::kBlocking:
        disposition = DID_NOT_HANDLE;
        break;
    case cc::EventListenerProperties::kNone: {
        WebMouseWheelEvent synthetic_wheel;
        synthetic_wheel.type = WebInputEvent::MouseWheel;
        synthetic_wheel.timeStampSeconds = InSecondsF(base::TimeTicks::Now());
        synthetic_wheel.deltaX = increment.width;
        synthetic_wheel.deltaY = increment.height;
        synthetic_wheel.hasPreciseScrollingDeltas = true;
        synthetic_wheel.x = fling_parameters_.point.x;
        synthetic_wheel.y = fling_parameters_.point.y;
        synthetic_wheel.globalX = fling_parameters_.globalPoint.x;
        synthetic_wheel.globalY = fling_parameters_.globalPoint.y;
        synthetic_wheel.modifiers = fling_parameters_.modifiers;

        disposition = ScrollByMouseWheel(synthetic_wheel);
        break;
    }
    default:
        NOTREACHED();
        return false;
    }

    switch (disposition) {
    case DID_HANDLE:
        return true;
    case DROP_EVENT:
        break;
    case DID_HANDLE_NON_BLOCKING:
    // TODO(dtapuska): Process the fling on the compositor thread
    // but post the events to the main thread; for now just pass it to the
    // main thread.
    case DID_NOT_HANDLE:
        TRACE_EVENT_INSTANT0("input",
            "InputHandlerProxy::scrollBy::AbortFling",
            TRACE_EVENT_SCOPE_THREAD);
        // If we got a DID_NOT_HANDLE, that means we need to deliver wheels on the
        // main thread. In this case we need to schedule a commit and transfer the
        // fling curve over to the main thread and run the rest of the wheels from
        // there. This can happen when flinging a page that contains a scrollable
        // subarea that we can't scroll on the thread if the fling starts outside
        // the subarea but then is flung "under" the pointer.
        client_->TransferActiveWheelFlingAnimation(fling_parameters_);
        fling_may_be_active_on_main_thread_ = true;
        CancelCurrentFlingWithoutNotifyingClient();
        break;
    }

    return false;
}

bool InputHandlerProxy::scrollBy(const WebFloatSize& increment,
    const WebFloatSize& velocity)
{
    WebFloatSize clipped_increment;
    WebFloatSize clipped_velocity;
    if (!disallow_horizontal_fling_scroll_) {
        clipped_increment.width = increment.width;
        clipped_velocity.width = velocity.width;
    }
    if (!disallow_vertical_fling_scroll_) {
        clipped_increment.height = increment.height;
        clipped_velocity.height = velocity.height;
    }

    current_fling_velocity_ = clipped_velocity;

    // Early out if the increment is zero, but avoid early termination if the
    // velocity is still non-zero.
    if (clipped_increment == WebFloatSize())
        return clipped_velocity != WebFloatSize();

    TRACE_EVENT2("input",
        "InputHandlerProxy::scrollBy",
        "x",
        clipped_increment.width,
        "y",
        clipped_increment.height);

    bool did_scroll = false;

    switch (fling_parameters_.sourceDevice) {
    case blink::WebGestureDeviceTouchpad:
        did_scroll = TouchpadFlingScroll(clipped_increment);
        break;
    case blink::WebGestureDeviceTouchscreen: {
        clipped_increment = ToClientScrollIncrement(clipped_increment);
        cc::ScrollStateData scroll_state_data;
        scroll_state_data.delta_x = clipped_increment.width;
        scroll_state_data.delta_y = clipped_increment.height;
        scroll_state_data.velocity_x = clipped_velocity.width;
        scroll_state_data.velocity_y = clipped_velocity.height;
        scroll_state_data.is_in_inertial_phase = true;
        cc::ScrollState scroll_state(scroll_state_data);
        cc::InputHandlerScrollResult scroll_result = input_handler_->ScrollBy(&scroll_state);
        HandleOverscroll(fling_parameters_.point, scroll_result);
        did_scroll = scroll_result.did_scroll;
    } break;
    case blink::WebGestureDeviceUninitialized:
        NOTREACHED();
        return false;
    }

    if (did_scroll) {
        fling_parameters_.cumulativeScroll.width += clipped_increment.width;
        fling_parameters_.cumulativeScroll.height += clipped_increment.height;
    }

    // It's possible the provided |increment| is sufficiently small as to not
    // trigger a scroll, e.g., with a trivial time delta between fling updates.
    // Return true in this case to prevent early fling termination.
    if (std::abs(clipped_increment.width) < kScrollEpsilon && std::abs(clipped_increment.height) < kScrollEpsilon)
        return true;

    return did_scroll;
}

} // namespace ui
