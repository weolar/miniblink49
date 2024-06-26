// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UI_EVENTS_EVENT_UTILS_H_
#define UI_EVENTS_EVENT_UTILS_H_

#include <stdint.h>

#include "base/event_types.h"
#include "base/memory/scoped_ptr.h"
#include "base/strings/string16.h"
#include "build/build_config.h"
#include "ui/events/event.h"
#include "ui/events/event_constants.h"
#include "ui/events/events_export.h"
#include "ui/events/keycodes/keyboard_codes.h"
#include "ui/gfx/display.h"
#include "ui/gfx/native_widget_types.h"

#if defined(OS_WIN)
#include <windows.h>
#endif

namespace gfx {
class Point;
class Vector2d;
}

namespace base {
class TimeDelta;
}

// Common functions to be used for all platforms except Android.
namespace ui {

class Event;
class MouseEvent;
enum class DomCode;

// Updates the list of devices for cached properties.
EVENTS_EXPORT void UpdateDeviceList();

// Returns a ui::Event wrapping a native event. Ownership of the returned value
// is transferred to the caller.
EVENTS_EXPORT scoped_ptr<Event> EventFromNative(
    const base::NativeEvent& native_event);

// Get the EventType from a native event.
EVENTS_EXPORT EventType EventTypeFromNative(
    const base::NativeEvent& native_event);

// Get the EventFlags from a native event.
EVENTS_EXPORT int EventFlagsFromNative(const base::NativeEvent& native_event);

// Get the timestamp from a native event.
// Note: This is not a pure function meaning that multiple applications on the
// same native event may return different values.
EVENTS_EXPORT base::TimeDelta EventTimeFromNative(
    const base::NativeEvent& native_event);

// Create a timestamp based on the current time.
EVENTS_EXPORT base::TimeDelta EventTimeForNow();

// Get the location from a native event.  The coordinate system of the resultant
// |Point| has the origin at top-left of the "root window".  The nature of
// this "root window" and how it maps to platform-specific drawing surfaces is
// defined in ui/aura/root_window.* and ui/aura/window_tree_host*.
// TODO(tdresser): Return gfx::PointF here. See crbug.com/337827.
EVENTS_EXPORT gfx::Point EventLocationFromNative(
    const base::NativeEvent& native_event);

// Gets the location in native system coordinate space.
EVENTS_EXPORT gfx::Point EventSystemLocationFromNative(
    const base::NativeEvent& native_event);

#if defined(USE_X11)
// Returns the 'real' button for an event. The button reported in slave events
// does not take into account any remapping (e.g. using xmodmap), while the
// button reported in master events do. This is a utility function to always
// return the mapped button.
EVENTS_EXPORT int EventButtonFromNative(const base::NativeEvent& native_event);
#endif

// Returns the KeyboardCode from a native event.
EVENTS_EXPORT KeyboardCode KeyboardCodeFromNative(
    const base::NativeEvent& native_event);

// Returns the DOM KeyboardEvent code (physical location in the
// keyboard) from a native event.
EVENTS_EXPORT DomCode CodeFromNative(const base::NativeEvent& native_event);

// Returns true if the keyboard event is a character event rather than
// a keystroke event.
EVENTS_EXPORT bool IsCharFromNative(const base::NativeEvent& native_event);

// Returns the flags of the button that changed during a press/release.
EVENTS_EXPORT int GetChangedMouseButtonFlagsFromNative(
    const base::NativeEvent& native_event);

// Returns the detailed pointer information for mouse events.
EVENTS_EXPORT PointerDetails GetMousePointerDetailsFromNative(
    const base::NativeEvent& native_event);

// Gets the mouse wheel offsets from a native event.
EVENTS_EXPORT gfx::Vector2d GetMouseWheelOffset(
    const base::NativeEvent& native_event);

// Returns a copy of |native_event|. Depending on the platform, this copy may
// need to be deleted with ReleaseCopiedNativeEvent().
base::NativeEvent CopyNativeEvent(
    const base::NativeEvent& native_event);

// Delete a |native_event| previously created by CopyNativeEvent().
void ReleaseCopiedNativeEvent(
    const base::NativeEvent& native_event);

// Returns the detailed pointer information for touch events.
EVENTS_EXPORT PointerDetails
GetTouchPointerDetailsFromNative(const base::NativeEvent& native_event);

// Gets the touch id from a native event.
EVENTS_EXPORT int GetTouchId(const base::NativeEvent& native_event);

// Clear the touch id from bookkeeping if it is a release/cancel event.
EVENTS_EXPORT void ClearTouchIdIfReleased(
    const base::NativeEvent& native_event);

// Gets the angle of the major axis away from the X axis. Default is 0.0.
EVENTS_EXPORT float GetTouchAngle(const base::NativeEvent& native_event);

// Gets the fling velocity from a native event. is_cancel is set to true if
// this was a tap down, intended to stop an ongoing fling.
EVENTS_EXPORT bool GetFlingData(const base::NativeEvent& native_event,
    float* vx,
    float* vy,
    float* vx_ordinal,
    float* vy_ordinal,
    bool* is_cancel);

// Returns whether this is a scroll event and optionally gets the amount to be
// scrolled. |x_offset|, |y_offset| and |finger_count| can be NULL.
EVENTS_EXPORT bool GetScrollOffsets(const base::NativeEvent& native_event,
    float* x_offset,
    float* y_offset,
    float* x_offset_ordinal,
    float* y_offset_ordinal,
    int* finger_count);

// Returns whether natural scrolling should be used for touchpad.
EVENTS_EXPORT bool ShouldDefaultToNaturalScroll();

// Returns whether or not the internal display produces touch events.
EVENTS_EXPORT gfx::Display::TouchSupport GetInternalDisplayTouchSupport();

#if defined(OS_WIN)
EVENTS_EXPORT int GetModifiersFromKeyState();

// Returns true if |message| identifies a mouse event that was generated as the
// result of a touch event.
EVENTS_EXPORT bool IsMouseEventFromTouch(UINT message);

// Converts scan code and lParam each other.  The scan code
// representing an extended key contains 0xE000 bits.
EVENTS_EXPORT uint16_t GetScanCodeFromLParam(LPARAM lParam);
EVENTS_EXPORT LPARAM GetLParamFromScanCode(uint16_t scan_code);

#endif

#if defined(USE_X11)
// Update the native X11 event to correspond to the new flags.
EVENTS_EXPORT void UpdateX11EventForFlags(Event* event);
// Update the native X11 event to correspond to the new button flags.
EVENTS_EXPORT void UpdateX11EventForChangedButtonFlags(MouseEvent* event);
#endif

// Registers a custom event type.
EVENTS_EXPORT int RegisterCustomEventType();

} // namespace ui

#endif // UI_EVENTS_EVENT_UTILS_H_
