// Copyright (c) 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/events/event.h"
#include "ui/events/event_constants.h"
#include "ui/events/event_utils.h"

namespace ui {

void UpdateDeviceList() { NOTIMPLEMENTED(); }

base::TimeDelta EventTimeFromNative(const base::NativeEvent& native_event)
{
    const ui::Event* event = static_cast<const ui::Event*>(native_event);
    return event->time_stamp();
}

int EventFlagsFromNative(const base::NativeEvent& native_event)
{
    const ui::Event* event = static_cast<const ui::Event*>(native_event);
    return event->flags();
}

EventType EventTypeFromNative(const base::NativeEvent& native_event)
{
    const ui::Event* event = static_cast<const ui::Event*>(native_event);
    return event->type();
}

gfx::Point EventSystemLocationFromNative(
    const base::NativeEvent& native_event)
{
    const ui::LocatedEvent* e = static_cast<const ui::LocatedEvent*>(native_event);
    DCHECK(e->IsMouseEvent() || e->IsTouchEvent() || e->IsGestureEvent() || e->IsScrollEvent());
    return e->location();
}

gfx::Point EventLocationFromNative(const base::NativeEvent& native_event)
{
    return EventSystemLocationFromNative(native_event);
}

int GetChangedMouseButtonFlagsFromNative(
    const base::NativeEvent& native_event)
{
    const ui::MouseEvent* event = static_cast<const ui::MouseEvent*>(native_event);
    DCHECK(event->IsMouseEvent() || event->IsScrollEvent());
    return event->changed_button_flags();
}

PointerDetails GetMousePointerDetailsFromNative(
    const base::NativeEvent& native_event)
{
    const ui::MouseEvent* event = static_cast<const ui::MouseEvent*>(native_event);
    DCHECK(event->IsMouseEvent() || event->IsScrollEvent());
    return event->pointer_details();
}

KeyboardCode KeyboardCodeFromNative(const base::NativeEvent& native_event)
{
    const ui::KeyEvent* event = static_cast<const ui::KeyEvent*>(native_event);
    DCHECK(event->IsKeyEvent());
    return event->key_code();
}

DomCode CodeFromNative(const base::NativeEvent& native_event)
{
    const ui::KeyEvent* event = static_cast<const ui::KeyEvent*>(native_event);
    DCHECK(event->IsKeyEvent());
    return event->code();
}

bool IsCharFromNative(const base::NativeEvent& native_event)
{
    const ui::KeyEvent* event = static_cast<const ui::KeyEvent*>(native_event);
    DCHECK(event->IsKeyEvent());
    return event->is_char();
}

gfx::Vector2d GetMouseWheelOffset(const base::NativeEvent& native_event)
{
    const ui::MouseWheelEvent* event = static_cast<const ui::MouseWheelEvent*>(native_event);
    DCHECK(event->type() == ET_MOUSEWHEEL);
    return event->offset();
}

base::NativeEvent CopyNativeEvent(const base::NativeEvent& event)
{
    return NULL;
}

void ReleaseCopiedNativeEvent(const base::NativeEvent& event)
{
}

void ClearTouchIdIfReleased(const base::NativeEvent& xev)
{
}

int GetTouchId(const base::NativeEvent& native_event)
{
    const ui::TouchEvent* event = static_cast<const ui::TouchEvent*>(native_event);
    DCHECK(event->IsTouchEvent());
    return event->touch_id();
}

PointerDetails GetTouchPointerDetailsFromNative(
    const base::NativeEvent& native_event)
{
    const ui::TouchEvent* event = static_cast<const ui::TouchEvent*>(native_event);
    DCHECK(event->IsTouchEvent());
    return event->pointer_details();
}

float GetTouchAngle(const base::NativeEvent& native_event)
{
    const ui::TouchEvent* event = static_cast<const ui::TouchEvent*>(native_event);
    DCHECK(event->IsTouchEvent());
    return event->rotation_angle();
}

bool GetScrollOffsets(const base::NativeEvent& native_event,
    float* x_offset,
    float* y_offset,
    float* x_offset_ordinal,
    float* y_offset_ordinal,
    int* finger_count)
{
    const ui::ScrollEvent* event = static_cast<const ui::ScrollEvent*>(native_event);
    DCHECK(event->IsScrollEvent());
    if (x_offset)
        *x_offset = event->x_offset();
    if (y_offset)
        *y_offset = event->y_offset();
    if (x_offset_ordinal)
        *x_offset_ordinal = event->x_offset_ordinal();
    if (y_offset_ordinal)
        *y_offset_ordinal = event->y_offset_ordinal();
    if (finger_count)
        *finger_count = event->finger_count();

    return true;
}

bool GetFlingData(const base::NativeEvent& native_event,
    float* vx,
    float* vy,
    float* vx_ordinal,
    float* vy_ordinal,
    bool* is_cancel)
{
    const ui::ScrollEvent* event = static_cast<const ui::ScrollEvent*>(native_event);
    DCHECK(event->IsScrollEvent());
    if (vx)
        *vx = event->x_offset();
    if (vy)
        *vy = event->y_offset();
    if (vx_ordinal)
        *vx_ordinal = event->x_offset_ordinal();
    if (vy_ordinal)
        *vy_ordinal = event->y_offset_ordinal();
    if (is_cancel)
        *is_cancel = event->type() == ET_SCROLL_FLING_CANCEL;

    return true;
}

} // namespace ui
