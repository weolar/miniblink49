// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// MSVC++ requires this to be set before any other includes to get M_PI.
#define _USE_MATH_DEFINES

#include "ui/events/blink/blink_event_util.h"

#include <stddef.h>

#include <algorithm>
#include <cmath>

#include "base/time/time.h"
#include "build/build_config.h"
#include "third_party/WebKit/public/web/WebInputEvent.h"
#include "ui/events/event_constants.h"
#include "ui/events/gesture_detection/gesture_event_data.h"
#include "ui/events/gesture_detection/motion_event.h"
#include "ui/events/gesture_event_details.h"
#include "ui/gfx/geometry/safe_integer_conversions.h"

using blink::WebGestureEvent;
using blink::WebInputEvent;
using blink::WebTouchEvent;
using blink::WebTouchPoint;

namespace ui {
namespace {

    WebInputEvent::Type ToWebInputEventType(MotionEvent::Action action)
    {
        switch (action) {
        case MotionEvent::ACTION_DOWN:
            return WebInputEvent::TouchStart;
        case MotionEvent::ACTION_MOVE:
            return WebInputEvent::TouchMove;
        case MotionEvent::ACTION_UP:
            return WebInputEvent::TouchEnd;
        case MotionEvent::ACTION_CANCEL:
            return WebInputEvent::TouchCancel;
        case MotionEvent::ACTION_POINTER_DOWN:
            return WebInputEvent::TouchStart;
        case MotionEvent::ACTION_POINTER_UP:
            return WebInputEvent::TouchEnd;
        case MotionEvent::ACTION_NONE:
            NOTREACHED();
            return WebInputEvent::Undefined;
        }
        NOTREACHED() << "Invalid MotionEvent::Action.";
        return WebInputEvent::Undefined;
    }

    // Note that the action index is meaningful only in the context of
    // |ACTION_POINTER_UP| and |ACTION_POINTER_DOWN|; other actions map directly to
    // WebTouchPoint::State.
    WebTouchPoint::State ToWebTouchPointState(const MotionEvent& event,
        size_t pointer_index)
    {
        switch (event.GetAction()) {
        case MotionEvent::ACTION_DOWN:
            return WebTouchPoint::StatePressed;
        case MotionEvent::ACTION_MOVE:
            return WebTouchPoint::StateMoved;
        case MotionEvent::ACTION_UP:
            return WebTouchPoint::StateReleased;
        case MotionEvent::ACTION_CANCEL:
            return WebTouchPoint::StateCancelled;
        case MotionEvent::ACTION_POINTER_DOWN:
            return static_cast<int>(pointer_index) == event.GetActionIndex()
                ? WebTouchPoint::StatePressed
                : WebTouchPoint::StateStationary;
        case MotionEvent::ACTION_POINTER_UP:
            return static_cast<int>(pointer_index) == event.GetActionIndex()
                ? WebTouchPoint::StateReleased
                : WebTouchPoint::StateStationary;
        case MotionEvent::ACTION_NONE:
            NOTREACHED();
            return WebTouchPoint::StateUndefined;
        }
        NOTREACHED() << "Invalid MotionEvent::Action.";
        return WebTouchPoint::StateUndefined;
    }

    WebTouchPoint::PointerType ToWebTouchPointPointerType(const MotionEvent& event,
        size_t pointer_index)
    {
        switch (event.GetToolType(pointer_index)) {
        case MotionEvent::TOOL_TYPE_UNKNOWN:
            return WebTouchPoint::PointerType::Unknown;
        case MotionEvent::TOOL_TYPE_FINGER:
            return WebTouchPoint::PointerType::Touch;
        case MotionEvent::TOOL_TYPE_STYLUS:
            return WebTouchPoint::PointerType::Pen;
        case MotionEvent::TOOL_TYPE_MOUSE:
            return WebTouchPoint::PointerType::Mouse;
        case MotionEvent::TOOL_TYPE_ERASER:
            return WebTouchPoint::PointerType::Unknown;
        }
        NOTREACHED() << "Invalid MotionEvent::ToolType = "
                     << event.GetToolType(pointer_index);
        return WebTouchPoint::PointerType::Unknown;
    }

    WebTouchPoint CreateWebTouchPoint(const MotionEvent& event,
        size_t pointer_index)
    {
        WebTouchPoint touch;
        touch.id = event.GetPointerId(pointer_index);
        touch.pointerType = ToWebTouchPointPointerType(event, pointer_index);
        touch.state = ToWebTouchPointState(event, pointer_index);
        touch.position.x = event.GetX(pointer_index);
        touch.position.y = event.GetY(pointer_index);
        touch.screenPosition.x = event.GetRawX(pointer_index);
        touch.screenPosition.y = event.GetRawY(pointer_index);

        // A note on touch ellipse specifications:
        //
        // Android MotionEvent provides the major and minor axes of the touch ellipse,
        // as well as the orientation of the major axis clockwise from vertical, in
        // radians. See:
        // http://developer.android.com/reference/android/view/MotionEvent.html
        //
        // The proposed extension to W3C Touch Events specifies the touch ellipse
        // using two radii along x- & y-axes and a positive acute rotation angle in
        // degrees. See:
        // http://dvcs.w3.org/hg/webevents/raw-file/default/touchevents.html

        float major_radius = event.GetTouchMajor(pointer_index) / 2.f;
        float minor_radius = event.GetTouchMinor(pointer_index) / 2.f;

        float orientation_rad = event.GetOrientation(pointer_index);
        float orientation_deg = orientation_rad * 180.f / M_PI;
        DCHECK_GE(major_radius, 0);
        DCHECK_GE(minor_radius, 0);
        DCHECK_GE(major_radius, minor_radius);
        // Orientation lies in [-180, 180] for a stylus, and [-90, 90] for other
        // touchscreen inputs. There are exceptions on Android when a device is
        // rotated, yielding touch orientations in the range of [-180, 180].
        // Regardless, normalise to [-90, 90), allowing a small tolerance to account
        // for floating point conversion.
        // TODO(e_hakkinen): Also pass unaltered stylus orientation, avoiding loss of
        // quadrant information, see crbug.com/493728.
        DCHECK_GT(orientation_deg, -180.01f);
        DCHECK_LT(orientation_deg, 180.01f);
        if (orientation_deg >= 90.f)
            orientation_deg -= 180.f;
        else if (orientation_deg < -90.f)
            orientation_deg += 180.f;
        if (orientation_deg >= 0) {
            // The case orientation_deg == 0 is handled here on purpose: although the
            // 'else' block is equivalent in this case, we want to pass the 0 value
            // unchanged (and 0 is the default value for many devices that don't
            // report elliptical touches).
            touch.radiusX = minor_radius;
            touch.radiusY = major_radius;
            touch.rotationAngle = orientation_deg;
        } else {
            touch.radiusX = major_radius;
            touch.radiusY = minor_radius;
            touch.rotationAngle = orientation_deg + 90;
        }

        touch.force = event.GetPressure(pointer_index);

        if (event.GetToolType(pointer_index) == MotionEvent::TOOL_TYPE_STYLUS) {
            // A stylus points to a direction specified by orientation and tilts to
            // the opposite direction. Coordinate system is left-handed.
            float tilt_rad = event.GetTilt(pointer_index);
            float r = sin(tilt_rad);
            float z = cos(tilt_rad);
            touch.tiltX = lround(atan2(sin(-orientation_rad) * r, z) * 180.f / M_PI);
            touch.tiltY = lround(atan2(cos(-orientation_rad) * r, z) * 180.f / M_PI);
        } else {
            touch.tiltX = touch.tiltY = 0;
        }

        return touch;
    }

} // namespace

blink::WebTouchEvent CreateWebTouchEventFromMotionEvent(
    const MotionEvent& event,
    bool moved_beyond_slop_region)
{
    static_assert(static_cast<int>(MotionEvent::MAX_TOUCH_POINT_COUNT) == static_cast<int>(blink::WebTouchEvent::touchesLengthCap),
        "inconsistent maximum number of active touch points");

    blink::WebTouchEvent result;

    result.type = ToWebInputEventType(event.GetAction());
    result.cancelable = (result.type != WebInputEvent::TouchCancel);
    result.timeStampSeconds = (event.GetEventTime() - base::TimeTicks()).InSecondsF();
    result.movedBeyondSlopRegion = moved_beyond_slop_region;
    result.modifiers = EventFlagsToWebEventModifiers(event.GetFlags());
    DCHECK_NE(event.GetUniqueEventId(), 0U);
    result.uniqueTouchEventId = event.GetUniqueEventId();
    result.touchesLength = std::min(static_cast<unsigned>(event.GetPointerCount()),
        static_cast<unsigned>(WebTouchEvent::touchesLengthCap));
    DCHECK_GT(result.touchesLength, 0U);

    for (size_t i = 0; i < result.touchesLength; ++i)
        result.touches[i] = CreateWebTouchPoint(event, i);

    return result;
}

int EventFlagsToWebEventModifiers(int flags)
{
    int modifiers = 0;

    if (flags & EF_SHIFT_DOWN)
        modifiers |= blink::WebInputEvent::ShiftKey;
    if (flags & EF_CONTROL_DOWN)
        modifiers |= blink::WebInputEvent::ControlKey;
    if (flags & EF_ALT_DOWN)
        modifiers |= blink::WebInputEvent::AltKey;
    if (flags & EF_COMMAND_DOWN)
        modifiers |= blink::WebInputEvent::MetaKey;
    if (flags & EF_ALTGR_DOWN)
        modifiers |= blink::WebInputEvent::AltGrKey;
    if (flags & EF_NUM_LOCK_ON)
        modifiers |= blink::WebInputEvent::NumLockOn;
    if (flags & EF_CAPS_LOCK_ON)
        modifiers |= blink::WebInputEvent::CapsLockOn;
    if (flags & EF_SCROLL_LOCK_ON)
        modifiers |= blink::WebInputEvent::ScrollLockOn;
    if (flags & EF_LEFT_MOUSE_BUTTON)
        modifiers |= blink::WebInputEvent::LeftButtonDown;
    if (flags & EF_MIDDLE_MOUSE_BUTTON)
        modifiers |= blink::WebInputEvent::MiddleButtonDown;
    if (flags & EF_RIGHT_MOUSE_BUTTON)
        modifiers |= blink::WebInputEvent::RightButtonDown;
    if (flags & EF_IS_REPEAT)
        modifiers |= blink::WebInputEvent::IsAutoRepeat;
    if (flags & EF_TOUCH_ACCESSIBILITY)
        modifiers |= blink::WebInputEvent::IsTouchAccessibility;

    return modifiers;
}

WebGestureEvent CreateWebGestureEvent(const GestureEventDetails& details,
    base::TimeDelta timestamp,
    const gfx::PointF& location,
    const gfx::PointF& raw_location,
    int flags)
{
    WebGestureEvent gesture;
    gesture.timeStampSeconds = timestamp.InSecondsF();
    gesture.x = gfx::ToFlooredInt(location.x());
    gesture.y = gfx::ToFlooredInt(location.y());
    gesture.globalX = gfx::ToFlooredInt(raw_location.x());
    gesture.globalY = gfx::ToFlooredInt(raw_location.y());
    gesture.modifiers = EventFlagsToWebEventModifiers(flags);
    gesture.sourceDevice = blink::WebGestureDeviceTouchscreen;

    switch (details.type()) {
    case ET_GESTURE_SHOW_PRESS:
        gesture.type = WebInputEvent::GestureShowPress;
        gesture.data.showPress.width = details.bounding_box_f().width();
        gesture.data.showPress.height = details.bounding_box_f().height();
        break;
    case ET_GESTURE_DOUBLE_TAP:
        gesture.type = WebInputEvent::GestureDoubleTap;
        DCHECK_EQ(1, details.tap_count());
        gesture.data.tap.tapCount = details.tap_count();
        gesture.data.tap.width = details.bounding_box_f().width();
        gesture.data.tap.height = details.bounding_box_f().height();
        break;
    case ET_GESTURE_TAP:
        gesture.type = WebInputEvent::GestureTap;
        DCHECK_GE(details.tap_count(), 1);
        gesture.data.tap.tapCount = details.tap_count();
        gesture.data.tap.width = details.bounding_box_f().width();
        gesture.data.tap.height = details.bounding_box_f().height();
        break;
    case ET_GESTURE_TAP_UNCONFIRMED:
        gesture.type = WebInputEvent::GestureTapUnconfirmed;
        DCHECK_EQ(1, details.tap_count());
        gesture.data.tap.tapCount = details.tap_count();
        gesture.data.tap.width = details.bounding_box_f().width();
        gesture.data.tap.height = details.bounding_box_f().height();
        break;
    case ET_GESTURE_LONG_PRESS:
        gesture.type = WebInputEvent::GestureLongPress;
        gesture.data.longPress.width = details.bounding_box_f().width();
        gesture.data.longPress.height = details.bounding_box_f().height();
        break;
    case ET_GESTURE_LONG_TAP:
        gesture.type = WebInputEvent::GestureLongTap;
        gesture.data.longPress.width = details.bounding_box_f().width();
        gesture.data.longPress.height = details.bounding_box_f().height();
        break;
    case ET_GESTURE_TWO_FINGER_TAP:
        gesture.type = blink::WebInputEvent::GestureTwoFingerTap;
        gesture.data.twoFingerTap.firstFingerWidth = details.first_finger_width();
        gesture.data.twoFingerTap.firstFingerHeight = details.first_finger_height();
        break;
    case ET_GESTURE_SCROLL_BEGIN:
        gesture.type = WebInputEvent::GestureScrollBegin;
        gesture.data.scrollBegin.deltaXHint = details.scroll_x_hint();
        gesture.data.scrollBegin.deltaYHint = details.scroll_y_hint();
        break;
    case ET_GESTURE_SCROLL_UPDATE:
        gesture.type = WebInputEvent::GestureScrollUpdate;
        gesture.data.scrollUpdate.deltaX = details.scroll_x();
        gesture.data.scrollUpdate.deltaY = details.scroll_y();
        gesture.data.scrollUpdate.previousUpdateInSequencePrevented = details.previous_scroll_update_in_sequence_prevented();
        break;
    case ET_GESTURE_SCROLL_END:
        gesture.type = WebInputEvent::GestureScrollEnd;
        break;
    case ET_SCROLL_FLING_START:
        gesture.type = WebInputEvent::GestureFlingStart;
        gesture.data.flingStart.velocityX = details.velocity_x();
        gesture.data.flingStart.velocityY = details.velocity_y();
        break;
    case ET_SCROLL_FLING_CANCEL:
        gesture.type = WebInputEvent::GestureFlingCancel;
        break;
    case ET_GESTURE_PINCH_BEGIN:
        gesture.type = WebInputEvent::GesturePinchBegin;
        break;
    case ET_GESTURE_PINCH_UPDATE:
        gesture.type = WebInputEvent::GesturePinchUpdate;
        gesture.data.pinchUpdate.scale = details.scale();
        break;
    case ET_GESTURE_PINCH_END:
        gesture.type = WebInputEvent::GesturePinchEnd;
        break;
    case ET_GESTURE_TAP_CANCEL:
        gesture.type = WebInputEvent::GestureTapCancel;
        break;
    case ET_GESTURE_TAP_DOWN:
        gesture.type = WebInputEvent::GestureTapDown;
        gesture.data.tapDown.width = details.bounding_box_f().width();
        gesture.data.tapDown.height = details.bounding_box_f().height();
        break;
    case ET_GESTURE_BEGIN:
    case ET_GESTURE_END:
    case ET_GESTURE_SWIPE:
        // The caller is responsible for discarding these gestures appropriately.
        gesture.type = WebInputEvent::Undefined;
        break;
    default:
        NOTREACHED() << "EventType provided wasn't a valid gesture event: "
                     << details.type();
    }

    return gesture;
}

WebGestureEvent CreateWebGestureEventFromGestureEventData(
    const GestureEventData& data)
{
    return CreateWebGestureEvent(data.details, data.time - base::TimeTicks(),
        gfx::PointF(data.x, data.y),
        gfx::PointF(data.raw_x, data.raw_y), data.flags);
}

scoped_ptr<blink::WebInputEvent> ScaleWebInputEvent(
    const blink::WebInputEvent& event,
    float scale)
{
    scoped_ptr<blink::WebInputEvent> scaled_event;
    if (scale == 1.f)
        return scaled_event;
    if (event.type == blink::WebMouseEvent::MouseWheel) {
        blink::WebMouseWheelEvent* wheel_event = new blink::WebMouseWheelEvent;
        scaled_event.reset(wheel_event);
        *wheel_event = static_cast<const blink::WebMouseWheelEvent&>(event);
        wheel_event->x *= scale;
        wheel_event->y *= scale;
        wheel_event->deltaX *= scale;
        wheel_event->deltaY *= scale;
        wheel_event->wheelTicksX *= scale;
        wheel_event->wheelTicksY *= scale;
    } else if (blink::WebInputEvent::isMouseEventType(event.type)) {
        blink::WebMouseEvent* mouse_event = new blink::WebMouseEvent;
        scaled_event.reset(mouse_event);
        *mouse_event = static_cast<const blink::WebMouseEvent&>(event);
        mouse_event->x *= scale;
        mouse_event->y *= scale;
        mouse_event->windowX = mouse_event->x;
        mouse_event->windowY = mouse_event->y;
        mouse_event->movementX *= scale;
        mouse_event->movementY *= scale;
    } else if (blink::WebInputEvent::isTouchEventType(event.type)) {
        blink::WebTouchEvent* touch_event = new blink::WebTouchEvent;
        scaled_event.reset(touch_event);
        *touch_event = static_cast<const blink::WebTouchEvent&>(event);
        for (unsigned i = 0; i < touch_event->touchesLength; i++) {
            touch_event->touches[i].position.x *= scale;
            touch_event->touches[i].position.y *= scale;
            touch_event->touches[i].radiusX *= scale;
            touch_event->touches[i].radiusY *= scale;
        }
    } else if (blink::WebInputEvent::isGestureEventType(event.type)) {
        blink::WebGestureEvent* gesture_event = new blink::WebGestureEvent;
        scaled_event.reset(gesture_event);
        *gesture_event = static_cast<const blink::WebGestureEvent&>(event);
        gesture_event->x *= scale;
        gesture_event->y *= scale;
        switch (gesture_event->type) {
        case blink::WebInputEvent::GestureScrollUpdate:
            gesture_event->data.scrollUpdate.deltaX *= scale;
            gesture_event->data.scrollUpdate.deltaY *= scale;
            break;
        case blink::WebInputEvent::GestureScrollBegin:
            gesture_event->data.scrollBegin.deltaXHint *= scale;
            gesture_event->data.scrollBegin.deltaYHint *= scale;
            break;

        case blink::WebInputEvent::GesturePinchUpdate:
            // Scale in pinch gesture is DSF agnostic.
            break;

        case blink::WebInputEvent::GestureDoubleTap:
        case blink::WebInputEvent::GestureTap:
        case blink::WebInputEvent::GestureTapUnconfirmed:
            gesture_event->data.tap.width *= scale;
            gesture_event->data.tap.height *= scale;
            break;

        case blink::WebInputEvent::GestureTapDown:
            gesture_event->data.tapDown.width *= scale;
            gesture_event->data.tapDown.height *= scale;
            break;

        case blink::WebInputEvent::GestureShowPress:
            gesture_event->data.showPress.width *= scale;
            gesture_event->data.showPress.height *= scale;
            break;

        case blink::WebInputEvent::GestureLongPress:
        case blink::WebInputEvent::GestureLongTap:
            gesture_event->data.longPress.width *= scale;
            gesture_event->data.longPress.height *= scale;
            break;

        case blink::WebInputEvent::GestureTwoFingerTap:
            gesture_event->data.twoFingerTap.firstFingerWidth *= scale;
            gesture_event->data.twoFingerTap.firstFingerHeight *= scale;
            break;

        case blink::WebInputEvent::GestureFlingStart:
            gesture_event->data.flingStart.velocityX *= scale;
            gesture_event->data.flingStart.velocityY *= scale;
            break;

        // These event does not have location data.
        case blink::WebInputEvent::GesturePinchBegin:
        case blink::WebInputEvent::GesturePinchEnd:
        case blink::WebInputEvent::GestureTapCancel:
        case blink::WebInputEvent::GestureFlingCancel:
        case blink::WebInputEvent::GestureScrollEnd:
            break;

        // TODO(oshima): Find out if ContextMenu needs to be scaled.
        default:
            break;
        }
    }
    return scaled_event;
}

} // namespace ui
