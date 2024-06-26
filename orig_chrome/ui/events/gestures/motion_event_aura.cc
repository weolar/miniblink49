// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/events/gestures/motion_event_aura.h"

#include "base/logging.h"
#include "ui/events/gesture_detection/gesture_configuration.h"

namespace ui {
namespace {

    MotionEvent::ToolType EventPointerTypeToMotionEventToolType(
        EventPointerType type)
    {
        switch (type) {
        case EventPointerType::POINTER_TYPE_UNKNOWN:
            return MotionEvent::TOOL_TYPE_UNKNOWN;
        case EventPointerType::POINTER_TYPE_MOUSE:
            return MotionEvent::TOOL_TYPE_MOUSE;
        case EventPointerType::POINTER_TYPE_PEN:
            return MotionEvent::TOOL_TYPE_STYLUS;
        case EventPointerType::POINTER_TYPE_TOUCH:
            return MotionEvent::TOOL_TYPE_FINGER;
        }

        return MotionEvent::TOOL_TYPE_UNKNOWN;
    }

    PointerProperties GetPointerPropertiesFromTouchEvent(const TouchEvent& touch)
    {
        PointerProperties pointer_properties;
        pointer_properties.x = touch.x();
        pointer_properties.y = touch.y();
        pointer_properties.raw_x = touch.root_location_f().x();
        pointer_properties.raw_y = touch.root_location_f().y();
        pointer_properties.id = touch.touch_id();
        pointer_properties.pressure = touch.pointer_details().force;
        pointer_properties.source_device_id = touch.source_device_id();

        pointer_properties.SetAxesAndOrientation(touch.pointer_details().radius_x,
            touch.pointer_details().radius_y,
            touch.rotation_angle());
        if (!pointer_properties.touch_major) {
            pointer_properties.touch_major = 2.f * GestureConfiguration::GetInstance()->default_radius();
            pointer_properties.touch_minor = 2.f * GestureConfiguration::GetInstance()->default_radius();
            pointer_properties.orientation = 0;
        }

        pointer_properties.tool_type = EventPointerTypeToMotionEventToolType(
            touch.pointer_details().pointer_type);

        return pointer_properties;
    }

} // namespace

MotionEventAura::MotionEventAura() { }

MotionEventAura::~MotionEventAura() { }

bool MotionEventAura::OnTouch(const TouchEvent& touch)
{
    int index = FindPointerIndexOfId(touch.touch_id());
    bool pointer_id_is_active = index != -1;

    if (touch.type() == ET_TOUCH_PRESSED && pointer_id_is_active) {
        // TODO(tdresser): This should return false (or NOTREACHED()), and
        // ignore the touch; however, there is at least one case where we
        // need to allow a touch press from a currently used touch id. See
        // crbug.com/446852 for details.

        // Cancel the existing touch, before handling the touch press.
        TouchEvent cancel(ET_TOUCH_CANCELLED, gfx::Point(), touch.touch_id(),
            touch.time_stamp());
        cancel.set_location_f(touch.location_f());
        cancel.set_root_location_f(touch.location_f());
        OnTouch(cancel);
        CleanupRemovedTouchPoints(cancel);
        DCHECK_EQ(-1, FindPointerIndexOfId(touch.touch_id()));
    } else if (touch.type() != ET_TOUCH_PRESSED && !pointer_id_is_active) {
        // We could have an active touch stream transfered to us, resulting in touch
        // move or touch up events without associated touch down events. Ignore
        // them.
        return false;
    }

    if (touch.type() == ET_TOUCH_MOVED && touch.x() == GetX(index) && touch.y() == GetY(index)) {
        return false;
    }

    switch (touch.type()) {
    case ET_TOUCH_PRESSED:
        if (!AddTouch(touch))
            return false;
    case ET_TOUCH_RELEASED:
    case ET_TOUCH_CANCELLED:
        // Removing these touch points needs to be postponed until after the
        // MotionEvent has been dispatched. This cleanup occurs in
        // CleanupRemovedTouchPoints.
        UpdateTouch(touch);
        break;
    case ET_TOUCH_MOVED:
        UpdateTouch(touch);
        break;
    default:
        NOTREACHED();
        return false;
    }

    UpdateCachedAction(touch);
    set_unique_event_id(touch.unique_event_id());
    set_flags(touch.flags());
    set_event_time(touch.time_stamp() + base::TimeTicks());
    return true;
}

void MotionEventAura::CleanupRemovedTouchPoints(const TouchEvent& event)
{
    if (event.type() != ET_TOUCH_RELEASED && event.type() != ET_TOUCH_CANCELLED) {
        return;
    }

    DCHECK(GetPointerCount());
    int index_to_delete = GetIndexFromId(event.touch_id());
    set_action_index(-1);
    set_action(MotionEvent::ACTION_NONE);
    pointer(index_to_delete) = pointer(GetPointerCount() - 1);
    PopPointer();
}

int MotionEventAura::GetSourceDeviceId(size_t pointer_index) const
{
    DCHECK_LT(pointer_index, GetPointerCount());
    return pointer(pointer_index).source_device_id;
}

bool MotionEventAura::AddTouch(const TouchEvent& touch)
{
    if (GetPointerCount() == MotionEvent::MAX_TOUCH_POINT_COUNT)
        return false;

    PushPointer(GetPointerPropertiesFromTouchEvent(touch));
    return true;
}

void MotionEventAura::UpdateTouch(const TouchEvent& touch)
{
    pointer(GetIndexFromId(touch.touch_id())) = GetPointerPropertiesFromTouchEvent(touch);
}

void MotionEventAura::UpdateCachedAction(const TouchEvent& touch)
{
    DCHECK(GetPointerCount());
    switch (touch.type()) {
    case ET_TOUCH_PRESSED:
        if (GetPointerCount() == 1) {
            set_action(ACTION_DOWN);
        } else {
            set_action(ACTION_POINTER_DOWN);
            set_action_index(GetIndexFromId(touch.touch_id()));
        }
        break;
    case ET_TOUCH_RELEASED:
        if (GetPointerCount() == 1) {
            set_action(ACTION_UP);
        } else {
            set_action(ACTION_POINTER_UP);
            set_action_index(GetIndexFromId(touch.touch_id()));
        }
        break;
    case ET_TOUCH_CANCELLED:
        set_action(ACTION_CANCEL);
        break;
    case ET_TOUCH_MOVED:
        set_action(ACTION_MOVE);
        break;
    default:
        NOTREACHED();
        break;
    }
}

int MotionEventAura::GetIndexFromId(int id) const
{
    int index = FindPointerIndexOfId(id);
    // TODO(tdresser): remove these checks once crbug.com/525189 is fixed.
    CHECK_GE(index, 0);
    CHECK_LT(index, static_cast<int>(GetPointerCount()));
    return index;
}

} // namespace ui
