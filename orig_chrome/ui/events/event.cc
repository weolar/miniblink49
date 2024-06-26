// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/events/event.h"

#include <utility>

#if defined(USE_X11)
#include <X11/Xlib.h>
#include <X11/extensions/XInput2.h>
#include <X11/keysym.h>
#endif

#include <cmath>
#include <cstring>

#include "base/metrics/histogram.h"
#include "base/strings/stringprintf.h"
#include "build/build_config.h"
#include "ui/events/base_event_utils.h"
#include "ui/events/event_utils.h"
#include "ui/events/keycodes/dom/dom_code.h"
#include "ui/events/keycodes/dom/dom_key.h"
#include "ui/events/keycodes/dom/keycode_converter.h"
#include "ui/events/keycodes/keyboard_code_conversion.h"
#include "ui/gfx/geometry/point3_f.h"
#include "ui/gfx/geometry/point_conversions.h"
#include "ui/gfx/geometry/safe_integer_conversions.h"
#include "ui/gfx/transform.h"
#include "ui/gfx/transform_util.h"

#if defined(USE_X11)
#include "ui/events/keycodes/keyboard_code_conversion_x.h"
#elif defined(USE_OZONE)
#include "ui/events/ozone/layout/keyboard_layout_engine.h" // nogncheck
#include "ui/events/ozone/layout/keyboard_layout_engine_manager.h" // nogncheck
#endif

#if defined(OS_WIN)
#include "ui/events/keycodes/platform_key_map_win.h"
#endif

namespace {

std::string EventTypeName(ui::EventType type)
{
#define RETURN_IF_TYPE(t) \
    if (type == ui::t)    \
    return #t
#define CASE_TYPE(t) \
    case ui::t:      \
        return #t
    switch (type) {
        CASE_TYPE(ET_UNKNOWN);
        CASE_TYPE(ET_MOUSE_PRESSED);
        CASE_TYPE(ET_MOUSE_DRAGGED);
        CASE_TYPE(ET_MOUSE_RELEASED);
        CASE_TYPE(ET_MOUSE_MOVED);
        CASE_TYPE(ET_MOUSE_ENTERED);
        CASE_TYPE(ET_MOUSE_EXITED);
        CASE_TYPE(ET_KEY_PRESSED);
        CASE_TYPE(ET_KEY_RELEASED);
        CASE_TYPE(ET_MOUSEWHEEL);
        CASE_TYPE(ET_MOUSE_CAPTURE_CHANGED);
        CASE_TYPE(ET_TOUCH_RELEASED);
        CASE_TYPE(ET_TOUCH_PRESSED);
        CASE_TYPE(ET_TOUCH_MOVED);
        CASE_TYPE(ET_TOUCH_CANCELLED);
        CASE_TYPE(ET_DROP_TARGET_EVENT);
        CASE_TYPE(ET_POINTER_DOWN);
        CASE_TYPE(ET_POINTER_MOVED);
        CASE_TYPE(ET_POINTER_UP);
        CASE_TYPE(ET_POINTER_CANCELLED);
        CASE_TYPE(ET_POINTER_ENTERED);
        CASE_TYPE(ET_POINTER_EXITED);
        CASE_TYPE(ET_GESTURE_SCROLL_BEGIN);
        CASE_TYPE(ET_GESTURE_SCROLL_END);
        CASE_TYPE(ET_GESTURE_SCROLL_UPDATE);
        CASE_TYPE(ET_GESTURE_SHOW_PRESS);
        CASE_TYPE(ET_GESTURE_WIN8_EDGE_SWIPE);
        CASE_TYPE(ET_GESTURE_TAP);
        CASE_TYPE(ET_GESTURE_TAP_DOWN);
        CASE_TYPE(ET_GESTURE_TAP_CANCEL);
        CASE_TYPE(ET_GESTURE_BEGIN);
        CASE_TYPE(ET_GESTURE_END);
        CASE_TYPE(ET_GESTURE_TWO_FINGER_TAP);
        CASE_TYPE(ET_GESTURE_PINCH_BEGIN);
        CASE_TYPE(ET_GESTURE_PINCH_END);
        CASE_TYPE(ET_GESTURE_PINCH_UPDATE);
        CASE_TYPE(ET_GESTURE_LONG_PRESS);
        CASE_TYPE(ET_GESTURE_LONG_TAP);
        CASE_TYPE(ET_GESTURE_SWIPE);
        CASE_TYPE(ET_GESTURE_TAP_UNCONFIRMED);
        CASE_TYPE(ET_GESTURE_DOUBLE_TAP);
        CASE_TYPE(ET_SCROLL);
        CASE_TYPE(ET_SCROLL_FLING_START);
        CASE_TYPE(ET_SCROLL_FLING_CANCEL);
        CASE_TYPE(ET_CANCEL_MODE);
        CASE_TYPE(ET_UMA_DATA);
    case ui::ET_LAST:
        NOTREACHED();
        return std::string();
        // Don't include default, so that we get an error when new type is added.
    }
#undef CASE_TYPE

    NOTREACHED();
    return std::string();
}

bool IsX11SendEventTrue(const base::NativeEvent& event)
{
#if defined(USE_X11)
    return event && event->xany.send_event;
#else
    return false;
#endif
}

bool X11EventHasNonStandardState(const base::NativeEvent& event)
{
#if defined(USE_X11)
    const unsigned int kAllStateMask = Button1Mask | Button2Mask | Button3Mask | Button4Mask | Button5Mask | Mod1Mask | Mod2Mask | Mod3Mask | Mod4Mask | Mod5Mask | ShiftMask | LockMask | ControlMask | AnyModifier;
    return event && (event->xkey.state & ~kAllStateMask) != 0;
#else
    return false;
#endif
}

} // namespace

namespace ui {

////////////////////////////////////////////////////////////////////////////////
// Event

// static
scoped_ptr<Event> Event::Clone(const Event& event)
{
    if (event.IsKeyEvent()) {
        return make_scoped_ptr(new KeyEvent(static_cast<const KeyEvent&>(event)));
    }

    if (event.IsMouseEvent()) {
        if (event.IsMouseWheelEvent()) {
            return make_scoped_ptr(
                new MouseWheelEvent(static_cast<const MouseWheelEvent&>(event)));
        }

        return make_scoped_ptr(
            new MouseEvent(static_cast<const MouseEvent&>(event)));
    }

    if (event.IsTouchEvent()) {
        return make_scoped_ptr(
            new TouchEvent(static_cast<const TouchEvent&>(event)));
    }

    if (event.IsGestureEvent()) {
        return make_scoped_ptr(
            new GestureEvent(static_cast<const GestureEvent&>(event)));
    }

    if (event.IsScrollEvent()) {
        return make_scoped_ptr(
            new ScrollEvent(static_cast<const ScrollEvent&>(event)));
    }

    return make_scoped_ptr(new Event(event));
}

Event::~Event()
{
    if (delete_native_event_)
        ReleaseCopiedNativeEvent(native_event_);
}

GestureEvent* Event::AsGestureEvent()
{
    CHECK(IsGestureEvent());
    return static_cast<GestureEvent*>(this);
}

const GestureEvent* Event::AsGestureEvent() const
{
    CHECK(IsGestureEvent());
    return static_cast<const GestureEvent*>(this);
}

bool Event::HasNativeEvent() const
{
    base::NativeEvent null_event;
    std::memset(&null_event, 0, sizeof(null_event));
    return !!std::memcmp(&native_event_, &null_event, sizeof(null_event));
}

void Event::StopPropagation()
{
    // TODO(sad): Re-enable these checks once View uses dispatcher to dispatch
    // events.
    // CHECK(phase_ != EP_PREDISPATCH && phase_ != EP_POSTDISPATCH);
    CHECK(cancelable_);
    result_ = static_cast<EventResult>(result_ | ER_CONSUMED);
}

void Event::SetHandled()
{
    // TODO(sad): Re-enable these checks once View uses dispatcher to dispatch
    // events.
    // CHECK(phase_ != EP_PREDISPATCH && phase_ != EP_POSTDISPATCH);
    CHECK(cancelable_);
    result_ = static_cast<EventResult>(result_ | ER_HANDLED);
}

Event::Event(EventType type, base::TimeDelta time_stamp, int flags)
    : type_(type)
    , time_stamp_(time_stamp)
    , flags_(flags)
    , native_event_(base::NativeEvent())
    , delete_native_event_(false)
    , cancelable_(true)
    , target_(NULL)
    , phase_(EP_PREDISPATCH)
    , result_(ER_UNHANDLED)
    , source_device_id_(ED_UNKNOWN_DEVICE)
{
    if (type_ < ET_LAST)
        name_ = EventTypeName(type_);
}

Event::Event(const base::NativeEvent& native_event,
    EventType type,
    int flags)
    : type_(type)
    , time_stamp_(EventTimeFromNative(native_event))
    , flags_(flags)
    , native_event_(native_event)
    , delete_native_event_(false)
    , cancelable_(true)
    , target_(NULL)
    , phase_(EP_PREDISPATCH)
    , result_(ER_UNHANDLED)
    , source_device_id_(ED_UNKNOWN_DEVICE)
{
    base::TimeDelta delta = EventTimeForNow() - time_stamp_;
    if (type_ < ET_LAST)
        name_ = EventTypeName(type_);
    base::HistogramBase::Sample delta_sample = static_cast<base::HistogramBase::Sample>(delta.InMicroseconds());
    UMA_HISTOGRAM_CUSTOM_COUNTS("Event.Latency.Browser", delta_sample, 1, 1000000,
        100);
    std::string name_for_event = base::StringPrintf("Event.Latency.Browser.%s", name_.c_str());
    base::HistogramBase* counter_for_type = base::Histogram::FactoryGet(
        name_for_event,
        1,
        1000000,
        100,
        base::HistogramBase::kUmaTargetedHistogramFlag);
    counter_for_type->Add(delta_sample);

#if defined(USE_X11)
    if (native_event->type == GenericEvent) {
        XIDeviceEvent* xiev = static_cast<XIDeviceEvent*>(native_event->xcookie.data);
        source_device_id_ = xiev->sourceid;
    }
#endif
#if defined(USE_OZONE)
    source_device_id_ = static_cast<const Event*>(native_event)->source_device_id();
#endif
}

Event::Event(const Event& copy)
    : type_(copy.type_)
    , time_stamp_(copy.time_stamp_)
    , latency_(copy.latency_)
    , flags_(copy.flags_)
    , native_event_(CopyNativeEvent(copy.native_event_))
    , delete_native_event_(true)
    , cancelable_(true)
    , target_(NULL)
    , phase_(EP_PREDISPATCH)
    , result_(ER_UNHANDLED)
    , source_device_id_(copy.source_device_id_)
{
    if (type_ < ET_LAST)
        name_ = EventTypeName(type_);
}

void Event::SetType(EventType type)
{
    if (type_ < ET_LAST)
        name_ = std::string();
    type_ = type;
    if (type_ < ET_LAST)
        name_ = EventTypeName(type_);
}

////////////////////////////////////////////////////////////////////////////////
// CancelModeEvent

CancelModeEvent::CancelModeEvent()
    : Event(ET_CANCEL_MODE, base::TimeDelta(), 0)
{
    set_cancelable(false);
}

CancelModeEvent::~CancelModeEvent()
{
}

////////////////////////////////////////////////////////////////////////////////
// LocatedEvent

LocatedEvent::~LocatedEvent()
{
}

LocatedEvent::LocatedEvent(const base::NativeEvent& native_event)
    : Event(native_event,
        EventTypeFromNative(native_event),
        EventFlagsFromNative(native_event))
    , location_(EventLocationFromNative(native_event))
    , root_location_(location_)
{
}

LocatedEvent::LocatedEvent(EventType type,
    const gfx::PointF& location,
    const gfx::PointF& root_location,
    base::TimeDelta time_stamp,
    int flags)
    : Event(type, time_stamp, flags)
    , location_(location)
    , root_location_(root_location)
{
}

void LocatedEvent::UpdateForRootTransform(
    const gfx::Transform& reversed_root_transform)
{
    // Transform has to be done at root level.
    gfx::Point3F p(location_);
    reversed_root_transform.TransformPoint(&p);
    location_ = p.AsPointF();
    root_location_ = location_;
}

////////////////////////////////////////////////////////////////////////////////
// MouseEvent

MouseEvent::MouseEvent(const base::NativeEvent& native_event)
    : LocatedEvent(native_event)
    , changed_button_flags_(GetChangedMouseButtonFlagsFromNative(native_event))
    , pointer_details_(GetMousePointerDetailsFromNative(native_event))
{
    if (type() == ET_MOUSE_PRESSED || type() == ET_MOUSE_RELEASED)
        SetClickCount(GetRepeatCount(*this));
}

MouseEvent::MouseEvent(EventType type,
    const gfx::Point& location,
    const gfx::Point& root_location,
    base::TimeDelta time_stamp,
    int flags,
    int changed_button_flags)
    : LocatedEvent(type,
        gfx::PointF(location),
        gfx::PointF(root_location),
        time_stamp,
        flags)
    , changed_button_flags_(changed_button_flags)
    , pointer_details_(PointerDetails(EventPointerType::POINTER_TYPE_MOUSE))
{
    if (this->type() == ET_MOUSE_MOVED && IsAnyButton())
        SetType(ET_MOUSE_DRAGGED);
}

// static
bool MouseEvent::IsRepeatedClickEvent(
    const MouseEvent& event1,
    const MouseEvent& event2)
{
    // These values match the Windows defaults.
    static const int kDoubleClickTimeMS = 500;
    static const int kDoubleClickWidth = 4;
    static const int kDoubleClickHeight = 4;

    if (event1.type() != ET_MOUSE_PRESSED || event2.type() != ET_MOUSE_PRESSED)
        return false;

    // Compare flags, but ignore EF_IS_DOUBLE_CLICK to allow triple clicks.
    if ((event1.flags() & ~EF_IS_DOUBLE_CLICK) != (event2.flags() & ~EF_IS_DOUBLE_CLICK))
        return false;

    // The new event has been created from the same native event.
    if (event1.time_stamp() == event2.time_stamp())
        return false;

    base::TimeDelta time_difference = event2.time_stamp() - event1.time_stamp();

    if (time_difference.InMilliseconds() > kDoubleClickTimeMS)
        return false;

    if (std::abs(event2.x() - event1.x()) > kDoubleClickWidth / 2)
        return false;

    if (std::abs(event2.y() - event1.y()) > kDoubleClickHeight / 2)
        return false;

    return true;
}

// static
int MouseEvent::GetRepeatCount(const MouseEvent& event)
{
    int click_count = 1;
    if (last_click_event_) {
        if (event.type() == ui::ET_MOUSE_RELEASED) {
            if (event.changed_button_flags() == last_click_event_->changed_button_flags()) {
                last_click_complete_ = true;
                return last_click_event_->GetClickCount();
            } else {
                // If last_click_event_ has changed since this button was pressed
                // return a click count of 1.
                return click_count;
            }
        }
        if (event.time_stamp() != last_click_event_->time_stamp())
            last_click_complete_ = true;
        if (!last_click_complete_ || IsX11SendEventTrue(event.native_event())) {
            click_count = last_click_event_->GetClickCount();
        } else if (IsRepeatedClickEvent(*last_click_event_, event)) {
            click_count = last_click_event_->GetClickCount() + 1;
        }
        delete last_click_event_;
    }
    last_click_event_ = new MouseEvent(event);
    last_click_complete_ = false;
    if (click_count > 3)
        click_count = 3;
    last_click_event_->SetClickCount(click_count);
    return click_count;
}

void MouseEvent::ResetLastClickForTest()
{
    if (last_click_event_) {
        delete last_click_event_;
        last_click_event_ = NULL;
        last_click_complete_ = false;
    }
}

// static
MouseEvent* MouseEvent::last_click_event_ = NULL;
bool MouseEvent::last_click_complete_ = false;

int MouseEvent::GetClickCount() const
{
    if (type() != ET_MOUSE_PRESSED && type() != ET_MOUSE_RELEASED)
        return 0;

    if (flags() & EF_IS_TRIPLE_CLICK)
        return 3;
    else if (flags() & EF_IS_DOUBLE_CLICK)
        return 2;
    else
        return 1;
}

void MouseEvent::SetClickCount(int click_count)
{
    if (type() != ET_MOUSE_PRESSED && type() != ET_MOUSE_RELEASED)
        return;

    DCHECK(click_count > 0);
    DCHECK(click_count <= 3);

    int f = flags();
    switch (click_count) {
    case 1:
        f &= ~EF_IS_DOUBLE_CLICK;
        f &= ~EF_IS_TRIPLE_CLICK;
        break;
    case 2:
        f |= EF_IS_DOUBLE_CLICK;
        f &= ~EF_IS_TRIPLE_CLICK;
        break;
    case 3:
        f &= ~EF_IS_DOUBLE_CLICK;
        f |= EF_IS_TRIPLE_CLICK;
        break;
    }
    set_flags(f);
}

////////////////////////////////////////////////////////////////////////////////
// MouseWheelEvent

MouseWheelEvent::MouseWheelEvent(const base::NativeEvent& native_event)
    : MouseEvent(native_event)
    , offset_(GetMouseWheelOffset(native_event))
{
}

MouseWheelEvent::MouseWheelEvent(const ScrollEvent& scroll_event)
    : MouseEvent(scroll_event)
    , offset_(gfx::ToRoundedInt(scroll_event.x_offset()),
          gfx::ToRoundedInt(scroll_event.y_offset()))
{
    SetType(ET_MOUSEWHEEL);
}

MouseWheelEvent::MouseWheelEvent(const MouseEvent& mouse_event,
    int x_offset,
    int y_offset)
    : MouseEvent(mouse_event)
    , offset_(x_offset, y_offset)
{
    DCHECK(type() == ET_MOUSEWHEEL);
}

MouseWheelEvent::MouseWheelEvent(const MouseWheelEvent& mouse_wheel_event)
    : MouseEvent(mouse_wheel_event)
    , offset_(mouse_wheel_event.offset())
{
    DCHECK(type() == ET_MOUSEWHEEL);
}

MouseWheelEvent::MouseWheelEvent(const gfx::Vector2d& offset,
    const gfx::Point& location,
    const gfx::Point& root_location,
    base::TimeDelta time_stamp,
    int flags,
    int changed_button_flags)
    : MouseEvent(ui::ET_MOUSEWHEEL,
        location,
        root_location,
        time_stamp,
        flags,
        changed_button_flags)
    , offset_(offset)
{
}

#if defined(OS_WIN)
// This value matches windows WHEEL_DELTA.
// static
const int MouseWheelEvent::kWheelDelta = 120;
#else
// This value matches GTK+ wheel scroll amount.
const int MouseWheelEvent::kWheelDelta = 53;
#endif

////////////////////////////////////////////////////////////////////////////////
// TouchEvent

TouchEvent::TouchEvent(const base::NativeEvent& native_event)
    : LocatedEvent(native_event)
    , touch_id_(GetTouchId(native_event))
    , unique_event_id_(ui::GetNextTouchEventId())
    , rotation_angle_(GetTouchAngle(native_event))
    , may_cause_scrolling_(false)
    , should_remove_native_touch_id_mapping_(false)
    , pointer_details_(GetTouchPointerDetailsFromNative(native_event))
{
    latency()->AddLatencyNumberWithTimestamp(
        INPUT_EVENT_LATENCY_ORIGINAL_COMPONENT, 0, 0,
        base::TimeTicks::FromInternalValue(time_stamp().ToInternalValue()), 1);
    latency()->AddLatencyNumber(INPUT_EVENT_LATENCY_UI_COMPONENT, 0, 0);

    FixRotationAngle();
    if (type() == ET_TOUCH_RELEASED || type() == ET_TOUCH_CANCELLED)
        should_remove_native_touch_id_mapping_ = true;
}

TouchEvent::TouchEvent(EventType type,
    const gfx::Point& location,
    int touch_id,
    base::TimeDelta time_stamp)
    : LocatedEvent(type,
        gfx::PointF(location),
        gfx::PointF(location),
        time_stamp,
        0)
    , touch_id_(touch_id)
    , unique_event_id_(ui::GetNextTouchEventId())
    , rotation_angle_(0.0f)
    , may_cause_scrolling_(false)
    , should_remove_native_touch_id_mapping_(false)
    , pointer_details_(PointerDetails(EventPointerType::POINTER_TYPE_TOUCH))
{
    latency()->AddLatencyNumber(INPUT_EVENT_LATENCY_UI_COMPONENT, 0, 0);
}

TouchEvent::TouchEvent(EventType type,
    const gfx::Point& location,
    int flags,
    int touch_id,
    base::TimeDelta time_stamp,
    float radius_x,
    float radius_y,
    float angle,
    float force)
    : LocatedEvent(type,
        gfx::PointF(location),
        gfx::PointF(location),
        time_stamp,
        flags)
    , touch_id_(touch_id)
    , unique_event_id_(ui::GetNextTouchEventId())
    , rotation_angle_(angle)
    , may_cause_scrolling_(false)
    , should_remove_native_touch_id_mapping_(false)
    , pointer_details_(PointerDetails(EventPointerType::POINTER_TYPE_TOUCH,
          radius_x,
          radius_y,
          force,
          /* tilt_x */ 0.0f,
          /* tilt_y */ 0.0f))
{
    latency()->AddLatencyNumber(INPUT_EVENT_LATENCY_UI_COMPONENT, 0, 0);
    FixRotationAngle();
}

TouchEvent::TouchEvent(const TouchEvent& copy)
    : LocatedEvent(copy)
    , touch_id_(copy.touch_id_)
    , unique_event_id_(copy.unique_event_id_)
    , rotation_angle_(copy.rotation_angle_)
    , may_cause_scrolling_(copy.may_cause_scrolling_)
    , should_remove_native_touch_id_mapping_(false)
    , pointer_details_(copy.pointer_details_)
{
    // Copied events should not remove touch id mapping, as this either causes the
    // mapping to be lost before the initial event has finished dispatching, or
    // the copy to attempt to remove the mapping from a null |native_event_|.
}

TouchEvent::~TouchEvent()
{
    // In ctor TouchEvent(native_event) we call GetTouchId() which in X11
    // platform setups the tracking_id to slot mapping. So in dtor here,
    // if this touch event is a release event, we clear the mapping accordingly.
    if (should_remove_native_touch_id_mapping_) {
        DCHECK(type() == ET_TOUCH_RELEASED || type() == ET_TOUCH_CANCELLED);
        if (type() == ET_TOUCH_RELEASED || type() == ET_TOUCH_CANCELLED)
            ClearTouchIdIfReleased(native_event());
    }
}

void TouchEvent::UpdateForRootTransform(
    const gfx::Transform& inverted_root_transform)
{
    LocatedEvent::UpdateForRootTransform(inverted_root_transform);
    gfx::DecomposedTransform decomp;
    bool success = gfx::DecomposeTransform(&decomp, inverted_root_transform);
    DCHECK(success);
    if (decomp.scale[0])
        pointer_details_.radius_x *= decomp.scale[0];
    if (decomp.scale[1])
        pointer_details_.radius_y *= decomp.scale[1];
}

void TouchEvent::DisableSynchronousHandling()
{
    DispatcherApi dispatcher_api(this);
    dispatcher_api.set_result(
        static_cast<EventResult>(result() | ER_DISABLE_SYNC_HANDLING));
}

void TouchEvent::FixRotationAngle()
{
    while (rotation_angle_ < 0)
        rotation_angle_ += 180;
    while (rotation_angle_ >= 180)
        rotation_angle_ -= 180;
}

////////////////////////////////////////////////////////////////////////////////
// PointerEvent

PointerEvent::PointerEvent(const MouseEvent& mouse_event)
    : LocatedEvent(mouse_event)
    , pointer_id_(kMousePointerId)
    , details_(mouse_event.pointer_details())
{
    switch (mouse_event.type()) {
    case ET_MOUSE_PRESSED:
        SetType(ET_POINTER_DOWN);
        break;

    case ET_MOUSE_DRAGGED:
    case ET_MOUSE_MOVED:
        SetType(ET_POINTER_MOVED);
        break;

    case ET_MOUSE_ENTERED:
        SetType(ET_POINTER_ENTERED);
        break;

    case ET_MOUSE_EXITED:
        SetType(ET_POINTER_EXITED);
        break;

    case ET_MOUSE_RELEASED:
        SetType(ET_POINTER_UP);
        break;

    default:
        NOTREACHED();
    }
}

PointerEvent::PointerEvent(const TouchEvent& touch_event)
    : LocatedEvent(touch_event)
    , pointer_id_(touch_event.touch_id())
    , details_(touch_event.pointer_details())
{
    switch (touch_event.type()) {
    case ET_TOUCH_PRESSED:
        SetType(ET_POINTER_DOWN);
        break;

    case ET_TOUCH_MOVED:
        SetType(ET_POINTER_MOVED);
        break;

    case ET_TOUCH_RELEASED:
        SetType(ET_POINTER_UP);
        break;

    case ET_TOUCH_CANCELLED:
        SetType(ET_POINTER_CANCELLED);
        break;

    default:
        NOTREACHED();
    }
}

const int PointerEvent::kMousePointerId = std::numeric_limits<int32_t>::max();

////////////////////////////////////////////////////////////////////////////////
// KeyEvent

// static
KeyEvent* KeyEvent::last_key_event_ = NULL;

// static
bool KeyEvent::IsRepeated(const KeyEvent& event)
{
    // A safe guard in case if there were continous key pressed events that are
    // not auto repeat.
    const int kMaxAutoRepeatTimeMs = 2000;
    // Ignore key events that have non standard state masks as it may be
    // reposted by an IME. IBUS-GTK uses this field to detect the
    // re-posted event for example. crbug.com/385873.
    if (X11EventHasNonStandardState(event.native_event()))
        return false;
    if (event.is_char())
        return false;
    if (event.type() == ui::ET_KEY_RELEASED) {
        delete last_key_event_;
        last_key_event_ = NULL;
        return false;
    }
    CHECK_EQ(ui::ET_KEY_PRESSED, event.type());
    if (!last_key_event_) {
        last_key_event_ = new KeyEvent(event);
        return false;
    } else if (event.time_stamp() == last_key_event_->time_stamp()) {
        // The KeyEvent is created from the same native event.
        return (last_key_event_->flags() & ui::EF_IS_REPEAT) != 0;
    }
    if (event.key_code() == last_key_event_->key_code() && event.flags() == (last_key_event_->flags() & ~ui::EF_IS_REPEAT) && (event.time_stamp() - last_key_event_->time_stamp()).InMilliseconds() < kMaxAutoRepeatTimeMs) {
        last_key_event_->set_time_stamp(event.time_stamp());
        last_key_event_->set_flags(last_key_event_->flags() | ui::EF_IS_REPEAT);
        return true;
    }
    delete last_key_event_;
    last_key_event_ = new KeyEvent(event);
    return false;
}

KeyEvent::KeyEvent(const base::NativeEvent& native_event)
    : Event(native_event,
        EventTypeFromNative(native_event),
        EventFlagsFromNative(native_event))
    , key_code_(KeyboardCodeFromNative(native_event))
    , code_(CodeFromNative(native_event))
    , is_char_(IsCharFromNative(native_event))
{
    if (IsRepeated(*this))
        set_flags(flags() | ui::EF_IS_REPEAT);

#if defined(USE_X11)
    NormalizeFlags();
#endif
#if defined(OS_WIN)
    // Only Windows has native character events.
    if (is_char_)
        key_ = DomKey::FromCharacter(native_event.wParam);
    else
        key_ = PlatformKeyMap::DomCodeAndFlagsToDomKeyStatic(code_, flags());
#endif
}

KeyEvent::KeyEvent(EventType type,
    KeyboardCode key_code,
    int flags)
    : Event(type, EventTimeForNow(), flags)
    , key_code_(key_code)
    , code_(UsLayoutKeyboardCodeToDomCode(key_code))
{
}

KeyEvent::KeyEvent(EventType type,
    KeyboardCode key_code,
    DomCode code,
    int flags)
    : Event(type, EventTimeForNow(), flags)
    , key_code_(key_code)
    , code_(code)
{
}

KeyEvent::KeyEvent(EventType type,
    KeyboardCode key_code,
    DomCode code,
    int flags,
    DomKey key,
    base::TimeDelta time_stamp)
    : Event(type, time_stamp, flags)
    , key_code_(key_code)
    , code_(code)
    , key_(key)
{
}

KeyEvent::KeyEvent(base::char16 character, KeyboardCode key_code, int flags)
    : Event(ET_KEY_PRESSED, EventTimeForNow(), flags)
    , key_code_(key_code)
    , code_(DomCode::NONE)
    , is_char_(true)
    , key_(DomKey::FromCharacter(character))
{
}

KeyEvent::KeyEvent(const KeyEvent& rhs)
    : Event(rhs)
    , key_code_(rhs.key_code_)
    , code_(rhs.code_)
    , is_char_(rhs.is_char_)
    , key_(rhs.key_)
{
    if (rhs.extended_key_event_data_)
        extended_key_event_data_.reset(rhs.extended_key_event_data_->Clone());
}

KeyEvent& KeyEvent::operator=(const KeyEvent& rhs)
{
    if (this != &rhs) {
        Event::operator=(rhs);
        key_code_ = rhs.key_code_;
        code_ = rhs.code_;
        key_ = rhs.key_;
        is_char_ = rhs.is_char_;

        if (rhs.extended_key_event_data_)
            extended_key_event_data_.reset(rhs.extended_key_event_data_->Clone());
    }
    return *this;
}

KeyEvent::~KeyEvent() { }

void KeyEvent::SetExtendedKeyEventData(scoped_ptr<ExtendedKeyEventData> data)
{
    extended_key_event_data_ = std::move(data);
}

void KeyEvent::ApplyLayout() const
{
    ui::DomCode code = code_;
    if (code == DomCode::NONE) {
        // Catch old code that tries to do layout without a physical key, and try
        // to recover using the KeyboardCode. Once key events are fully defined
        // on construction (see TODO in event.h) this will go away.
        VLOG(2) << "DomCode::NONE keycode=" << key_code_;
        code = UsLayoutKeyboardCodeToDomCode(key_code_);
        if (code == DomCode::NONE) {
            key_ = DomKey::UNIDENTIFIED;
            return;
        }
    }
    KeyboardCode dummy_key_code;
#if defined(OS_WIN)
// Native Windows character events always have is_char_ == true,
// so this is a synthetic or native keystroke event.
// Therefore, perform only the fallback action.
#elif defined(USE_X11)
    // When a control key is held, prefer ASCII characters to non ASCII
    // characters in order to use it for shortcut keys.  GetCharacterFromKeyCode
    // returns 'a' for VKEY_A even if the key is actually bound to 'à' in X11.
    // GetCharacterFromXEvent returns 'à' in that case.
    if (!IsControlDown() && native_event()) {
        key_ = GetDomKeyFromXEvent(native_event());
        return;
    }
#elif defined(USE_OZONE)
    if (KeyboardLayoutEngineManager::GetKeyboardLayoutEngine()->Lookup(
            code, flags(), &key_, &dummy_key_code)) {
        return;
    }
#else
    if (native_event()) {
        DCHECK(EventTypeFromNative(native_event()) == ET_KEY_PRESSED || EventTypeFromNative(native_event()) == ET_KEY_RELEASED);
    }
#endif
    if (!DomCodeToUsLayoutDomKey(code, flags(), &key_, &dummy_key_code))
        key_ = DomKey::UNIDENTIFIED;
}

DomKey KeyEvent::GetDomKey() const
{
    // Determination of key_ may be done lazily.
    if (key_ == DomKey::NONE)
        ApplyLayout();
    return key_;
}

base::char16 KeyEvent::GetCharacter() const
{
    // Determination of key_ may be done lazily.
    if (key_ == DomKey::NONE)
        ApplyLayout();
    if (key_.IsCharacter()) {
        // Historically ui::KeyEvent has held only BMP characters.
        // Until this explicitly changes, require |key_| to hold a BMP character.
        DomKey::Base utf32_character = key_.ToCharacter();
        base::char16 ucs2_character = static_cast<base::char16>(utf32_character);
        DCHECK(static_cast<DomKey::Base>(ucs2_character) == utf32_character);
        // Check if the control character is down. Note that ALTGR is represented
        // on Windows as CTRL|ALT, so we need to make sure that is not set.
        if ((flags() & (EF_ALTGR_DOWN | EF_CONTROL_DOWN)) == EF_CONTROL_DOWN) {
            // For a control character, key_ contains the corresponding printable
            // character. To preserve existing behaviour for now, return the control
            // character here; this will likely change -- see e.g. crbug.com/471488.
            if (ucs2_character >= 0x20 && ucs2_character <= 0x7E)
                return ucs2_character & 0x1F;
            if (ucs2_character == '\r')
                return '\n';
        }
        return ucs2_character;
    }
    return 0;
}

base::char16 KeyEvent::GetText() const
{
    if ((flags() & EF_CONTROL_DOWN) != 0) {
        ui::DomKey key;
        ui::KeyboardCode key_code;
        if (DomCodeToControlCharacter(code_, flags(), &key, &key_code))
            return key.ToCharacter();
    }
    return GetUnmodifiedText();
}

base::char16 KeyEvent::GetUnmodifiedText() const
{
    if (!is_char_ && (key_code_ == VKEY_RETURN))
        return '\r';
    return GetCharacter();
}

bool KeyEvent::IsUnicodeKeyCode() const
{
#if defined(OS_WIN)
    if (!IsAltDown())
        return false;
    const int key = key_code();
    if (key >= VKEY_NUMPAD0 && key <= VKEY_NUMPAD9)
        return true;
    // Check whether the user is using the numeric keypad with num-lock off.
    // In that case, EF_EXTENDED will not be set; if it is set, the key event
    // originated from the relevant non-numpad dedicated key, e.g. [Insert].
    return (!(flags() & EF_IS_EXTENDED_KEY) && (key == VKEY_INSERT || key == VKEY_END || key == VKEY_DOWN || key == VKEY_NEXT || key == VKEY_LEFT || key == VKEY_CLEAR || key == VKEY_RIGHT || key == VKEY_HOME || key == VKEY_UP || key == VKEY_PRIOR));
#else
    return false;
#endif
}

void KeyEvent::NormalizeFlags()
{
    int mask = 0;
    switch (key_code()) {
    case VKEY_CONTROL:
        mask = EF_CONTROL_DOWN;
        break;
    case VKEY_SHIFT:
        mask = EF_SHIFT_DOWN;
        break;
    case VKEY_MENU:
        mask = EF_ALT_DOWN;
        break;
    default:
        return;
    }
    if (type() == ET_KEY_PRESSED)
        set_flags(flags() | mask);
    else
        set_flags(flags() & ~mask);
}

KeyboardCode KeyEvent::GetLocatedWindowsKeyboardCode() const
{
    return NonLocatedToLocatedKeyboardCode(key_code_, code_);
}

uint16_t KeyEvent::GetConflatedWindowsKeyCode() const
{
    if (is_char_)
        return key_.ToCharacter();
    return key_code_;
}

std::string KeyEvent::GetCodeString() const
{
    return KeycodeConverter::DomCodeToCodeString(code_);
}

////////////////////////////////////////////////////////////////////////////////
// ScrollEvent

ScrollEvent::ScrollEvent(const base::NativeEvent& native_event)
    : MouseEvent(native_event)
    , x_offset_(0.0f)
    , y_offset_(0.0f)
    , x_offset_ordinal_(0.0f)
    , y_offset_ordinal_(0.0f)
    , finger_count_(0)
{
    if (type() == ET_SCROLL) {
        GetScrollOffsets(native_event,
            &x_offset_, &y_offset_,
            &x_offset_ordinal_, &y_offset_ordinal_,
            &finger_count_);
    } else if (type() == ET_SCROLL_FLING_START || type() == ET_SCROLL_FLING_CANCEL) {
        GetFlingData(native_event,
            &x_offset_, &y_offset_,
            &x_offset_ordinal_, &y_offset_ordinal_,
            NULL);
    } else {
        NOTREACHED() << "Unexpected event type " << type()
                     << " when constructing a ScrollEvent.";
    }
}

ScrollEvent::ScrollEvent(EventType type,
    const gfx::Point& location,
    base::TimeDelta time_stamp,
    int flags,
    float x_offset,
    float y_offset,
    float x_offset_ordinal,
    float y_offset_ordinal,
    int finger_count)
    : MouseEvent(type, location, location, time_stamp, flags, 0)
    , x_offset_(x_offset)
    , y_offset_(y_offset)
    , x_offset_ordinal_(x_offset_ordinal)
    , y_offset_ordinal_(y_offset_ordinal)
    , finger_count_(finger_count)
{
    CHECK(IsScrollEvent());
}

void ScrollEvent::Scale(const float factor)
{
    x_offset_ *= factor;
    y_offset_ *= factor;
    x_offset_ordinal_ *= factor;
    y_offset_ordinal_ *= factor;
}

////////////////////////////////////////////////////////////////////////////////
// GestureEvent

GestureEvent::GestureEvent(float x,
    float y,
    int flags,
    base::TimeDelta time_stamp,
    const GestureEventDetails& details)
    : LocatedEvent(details.type(),
        gfx::PointF(x, y),
        gfx::PointF(x, y),
        time_stamp,
        flags | EF_FROM_TOUCH)
    , details_(details)
{
}

GestureEvent::~GestureEvent()
{
}

} // namespace ui
