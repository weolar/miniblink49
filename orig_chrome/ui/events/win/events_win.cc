// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <stdint.h>
#include <windowsx.h>

#include "ui/events/event_constants.h"

#include "base/logging.h"
#include "base/time/time.h"
#include "ui/events/event_utils.h"
#include "ui/events/keycodes/keyboard_code_conversion_win.h"
#include "ui/events/win/system_event_state_lookup.h"
#include "ui/gfx/geometry/point.h"
#include "ui/gfx/win/dpi.h"

namespace ui {

namespace {

// From MSDN: "Mouse" events are flagged with 0xFF515700 if they come
// from a touch or stylus device.  In Vista or later, they are also flagged
// with 0x80 if they come from touch.
#define MOUSEEVENTF_FROMTOUCH (0xFF515700 | 0x80)

    // Get the native mouse key state from the native event message type.
    int GetNativeMouseKey(const base::NativeEvent& native_event)
    {
        switch (native_event.message) {
        case WM_LBUTTONDBLCLK:
        case WM_LBUTTONDOWN:
        case WM_LBUTTONUP:
        case WM_NCLBUTTONDBLCLK:
        case WM_NCLBUTTONDOWN:
        case WM_NCLBUTTONUP:
            return MK_LBUTTON;
        case WM_MBUTTONDBLCLK:
        case WM_MBUTTONDOWN:
        case WM_MBUTTONUP:
        case WM_NCMBUTTONDBLCLK:
        case WM_NCMBUTTONDOWN:
        case WM_NCMBUTTONUP:
            return MK_MBUTTON;
        case WM_RBUTTONDBLCLK:
        case WM_RBUTTONDOWN:
        case WM_RBUTTONUP:
        case WM_NCRBUTTONDBLCLK:
        case WM_NCRBUTTONDOWN:
        case WM_NCRBUTTONUP:
            return MK_RBUTTON;
        case WM_NCXBUTTONDBLCLK:
        case WM_NCXBUTTONDOWN:
        case WM_NCXBUTTONUP:
        case WM_XBUTTONDBLCLK:
        case WM_XBUTTONDOWN:
        case WM_XBUTTONUP:
            return MK_XBUTTON1;
        }
        return 0;
    }

    bool IsButtonDown(const base::NativeEvent& native_event)
    {
        return ((MK_LBUTTON | MK_MBUTTON | MK_RBUTTON | MK_XBUTTON1 | MK_XBUTTON2) & native_event.wParam) != 0;
    }

    bool IsClientMouseEvent(const base::NativeEvent& native_event)
    {
        return native_event.message == WM_MOUSELEAVE || native_event.message == WM_MOUSEHOVER || (native_event.message >= WM_MOUSEFIRST && native_event.message <= WM_MOUSELAST);
    }

    bool IsNonClientMouseEvent(const base::NativeEvent& native_event)
    {
        return native_event.message == WM_NCMOUSELEAVE || native_event.message == WM_NCMOUSEHOVER || (native_event.message >= WM_NCMOUSEMOVE && native_event.message <= WM_NCXBUTTONDBLCLK);
    }

    bool IsMouseEvent(const base::NativeEvent& native_event)
    {
        return IsClientMouseEvent(native_event) || IsNonClientMouseEvent(native_event);
    }

    bool IsMouseWheelEvent(const base::NativeEvent& native_event)
    {
        return native_event.message == WM_MOUSEWHEEL || native_event.message == WM_MOUSEHWHEEL;
    }

    bool IsKeyEvent(const base::NativeEvent& native_event)
    {
        return native_event.message == WM_KEYDOWN || native_event.message == WM_SYSKEYDOWN || native_event.message == WM_CHAR || native_event.message == WM_SYSCHAR || native_event.message == WM_KEYUP || native_event.message == WM_SYSKEYUP;
    }

    bool IsScrollEvent(const base::NativeEvent& native_event)
    {
        return native_event.message == WM_VSCROLL || native_event.message == WM_HSCROLL;
    }

    // Returns a mask corresponding to the set of pressed modifier keys.
    // Checks the current global state and the state sent by client mouse messages.
    int KeyStateFlagsFromNative(const base::NativeEvent& native_event)
    {
        int flags = GetModifiersFromKeyState();

        // Check key messages for the extended key flag.
        if (IsKeyEvent(native_event) && (HIWORD(native_event.lParam) & KF_EXTENDED))
            flags |= EF_IS_EXTENDED_KEY;

        // Most client mouse messages include key state information.
        if (IsClientMouseEvent(native_event)) {
            int win_flags = GET_KEYSTATE_WPARAM(native_event.wParam);
            flags |= (win_flags & MK_SHIFT) ? EF_SHIFT_DOWN : 0;
            flags |= (win_flags & MK_CONTROL) ? EF_CONTROL_DOWN : 0;
        }

        return flags;
    }

    // Returns a mask corresponding to the set of pressed mouse buttons.
    // This includes the button of the given message, even if it is being released.
    int MouseStateFlagsFromNative(const base::NativeEvent& native_event)
    {
        int win_flags = GetNativeMouseKey(native_event);

        // Client mouse messages provide key states in their WPARAMs.
        if (IsClientMouseEvent(native_event))
            win_flags |= GET_KEYSTATE_WPARAM(native_event.wParam);

        int flags = 0;
        flags |= (win_flags & MK_LBUTTON) ? EF_LEFT_MOUSE_BUTTON : 0;
        flags |= (win_flags & MK_MBUTTON) ? EF_MIDDLE_MOUSE_BUTTON : 0;
        flags |= (win_flags & MK_RBUTTON) ? EF_RIGHT_MOUSE_BUTTON : 0;
        flags |= IsNonClientMouseEvent(native_event) ? EF_IS_NON_CLIENT : 0;
        return flags;
    }

} // namespace

void UpdateDeviceList()
{
    NOTIMPLEMENTED();
}

EventType EventTypeFromNative(const base::NativeEvent& native_event)
{
    switch (native_event.message) {
    case WM_KEYDOWN:
    case WM_SYSKEYDOWN:
    case WM_CHAR:
    case WM_SYSCHAR:
        return ET_KEY_PRESSED;
    // The WM_DEADCHAR message is posted to the window with the keyboard focus
    // when a WM_KEYUP message is translated. This happens for special keyboard
    // sequences.
    case WM_DEADCHAR:
    case WM_KEYUP:
    // The WM_SYSDEADCHAR message is posted to a window with keyboard focus
    // when the WM_SYSKEYDOWN message is translated by the TranslateMessage
    // function. It specifies the character code of the system dead key.
    case WM_SYSDEADCHAR:
    case WM_SYSKEYUP:
        return ET_KEY_RELEASED;
    case WM_LBUTTONDBLCLK:
    case WM_LBUTTONDOWN:
    case WM_MBUTTONDBLCLK:
    case WM_MBUTTONDOWN:
    case WM_NCLBUTTONDBLCLK:
    case WM_NCLBUTTONDOWN:
    case WM_NCMBUTTONDBLCLK:
    case WM_NCMBUTTONDOWN:
    case WM_NCRBUTTONDBLCLK:
    case WM_NCRBUTTONDOWN:
    case WM_NCXBUTTONDBLCLK:
    case WM_NCXBUTTONDOWN:
    case WM_RBUTTONDBLCLK:
    case WM_RBUTTONDOWN:
    case WM_XBUTTONDBLCLK:
    case WM_XBUTTONDOWN:
        return ET_MOUSE_PRESSED;
    case WM_LBUTTONUP:
    case WM_MBUTTONUP:
    case WM_NCLBUTTONUP:
    case WM_NCMBUTTONUP:
    case WM_NCRBUTTONUP:
    case WM_NCXBUTTONUP:
    case WM_RBUTTONUP:
    case WM_XBUTTONUP:
        return ET_MOUSE_RELEASED;
    case WM_MOUSEMOVE:
        return IsButtonDown(native_event) ? ET_MOUSE_DRAGGED : ET_MOUSE_MOVED;
    case WM_NCMOUSEMOVE:
        return ET_MOUSE_MOVED;
    case WM_MOUSEWHEEL:
    case WM_MOUSEHWHEEL:
        return ET_MOUSEWHEEL;
    case WM_MOUSELEAVE:
    case WM_NCMOUSELEAVE:
        return ET_MOUSE_EXITED;
    case WM_VSCROLL:
    case WM_HSCROLL:
        return ET_SCROLL;
    default:
        // We can't NOTREACHED() here, since this function can be called for any
        // message.
        break;
    }
    return ET_UNKNOWN;
}

int EventFlagsFromNative(const base::NativeEvent& native_event)
{
    int flags = KeyStateFlagsFromNative(native_event);
    if (IsMouseEvent(native_event))
        flags |= MouseStateFlagsFromNative(native_event);

    return flags;
}

base::TimeDelta EventTimeFromNative(const base::NativeEvent& native_event)
{
    // On Windows, the native input event timestamp (|native_event.time|) is
    // coming from |GetTickCount()| clock [1], while in platform independent code
    // path we get timestamps by calling |TimeTicks::Now()|, which, if using high-
    // resolution timer as underlying implementation, could have different time
    // origin than |GetTickCount()|. To avoid the mismatching, we use
    // |TimeTicks::Now()| for event timestamp instead of the native timestamp to
    // ensure computed input latency and web exposed timestamp is consistent with
    // other components.
    // [1] http://blogs.msdn.com/b/oldnewthing/archive/2014/01/22/10491576.aspx
    return EventTimeForNow();
}

gfx::Point EventLocationFromNative(const base::NativeEvent& native_event)
{
    POINT native_point;
    if ((native_event.message == WM_MOUSELEAVE || native_event.message == WM_NCMOUSELEAVE) || IsScrollEvent(native_event)) {
        // These events have no coordinates. For sanity with rest of events grab
        // coordinates from the OS.
        ::GetCursorPos(&native_point);
    } else if (IsClientMouseEvent(native_event) && !IsMouseWheelEvent(native_event)) {
        // Note: Wheel events are considered client, but their position is in screen
        //       coordinates.
        // Client message. The position is contained in the LPARAM.
        return gfx::Point(native_event.lParam);
    } else {
        DCHECK(IsNonClientMouseEvent(native_event) || IsMouseWheelEvent(native_event) || IsScrollEvent(native_event));
        // Non-client message. The position is contained in a POINTS structure in
        // LPARAM, and is in screen coordinates so we have to convert to client.
        native_point.x = GET_X_LPARAM(native_event.lParam);
        native_point.y = GET_Y_LPARAM(native_event.lParam);
    }
    ScreenToClient(native_event.hwnd, &native_point);
    return gfx::Point(native_point);
}

gfx::Point EventSystemLocationFromNative(
    const base::NativeEvent& native_event)
{
    POINT global_point = { static_cast<short>(LOWORD(native_event.lParam)),
        static_cast<short>(HIWORD(native_event.lParam)) };
    ClientToScreen(native_event.hwnd, &global_point);
    return gfx::Point(global_point);
}

KeyboardCode KeyboardCodeFromNative(const base::NativeEvent& native_event)
{
    return KeyboardCodeForWindowsKeyCode(static_cast<WORD>(native_event.wParam));
}

DomCode CodeFromNative(const base::NativeEvent& native_event)
{
    const uint16_t scan_code = GetScanCodeFromLParam(native_event.lParam);
    return CodeForWindowsScanCode(scan_code);
}

bool IsCharFromNative(const base::NativeEvent& native_event)
{
    return native_event.message == WM_CHAR || native_event.message == WM_SYSCHAR;
}

int GetChangedMouseButtonFlagsFromNative(
    const base::NativeEvent& native_event)
{
    switch (GetNativeMouseKey(native_event)) {
    case MK_LBUTTON:
        return EF_LEFT_MOUSE_BUTTON;
    case MK_MBUTTON:
        return EF_MIDDLE_MOUSE_BUTTON;
    case MK_RBUTTON:
        return EF_RIGHT_MOUSE_BUTTON;
    // TODO: add support for MK_XBUTTON1.
    default:
        break;
    }
    return 0;
}

PointerDetails GetMousePointerDetailsFromNative(
    const base::NativeEvent& native_event)
{
    return PointerDetails(EventPointerType::POINTER_TYPE_MOUSE);
}

gfx::Vector2d GetMouseWheelOffset(const base::NativeEvent& native_event)
{
    DCHECK(native_event.message == WM_MOUSEWHEEL || native_event.message == WM_MOUSEHWHEEL);
    if (native_event.message == WM_MOUSEWHEEL)
        return gfx::Vector2d(0, GET_WHEEL_DELTA_WPARAM(native_event.wParam));
    return gfx::Vector2d(GET_WHEEL_DELTA_WPARAM(native_event.wParam), 0);
}

base::NativeEvent CopyNativeEvent(const base::NativeEvent& event)
{
    return event;
}

void ReleaseCopiedNativeEvent(const base::NativeEvent& event)
{
}

void ClearTouchIdIfReleased(const base::NativeEvent& xev)
{
    NOTIMPLEMENTED();
}

int GetTouchId(const base::NativeEvent& xev)
{
    NOTIMPLEMENTED();
    return 0;
}

float GetTouchAngle(const base::NativeEvent& native_event)
{
    NOTIMPLEMENTED();
    return 0.0;
}

PointerDetails GetTouchPointerDetailsFromNative(
    const base::NativeEvent& native_event)
{
    NOTIMPLEMENTED();
    return PointerDetails(EventPointerType::POINTER_TYPE_TOUCH,
        /* radius_x */ 1.0,
        /* radius_y */ 1.0,
        /* force */ 0.f,
        /* tilt_x */ 0.f,
        /* tilt_y */ 0.f);
}

bool GetScrollOffsets(const base::NativeEvent& native_event,
    float* x_offset,
    float* y_offset,
    float* x_offset_ordinal,
    float* y_offset_ordinal,
    int* finger_count)
{
    // TODO(ananta)
    // Support retrieving the scroll offsets from the scroll event.
    if (native_event.message == WM_VSCROLL || native_event.message == WM_HSCROLL)
        return true;
    return false;
}

bool GetFlingData(const base::NativeEvent& native_event,
    float* vx,
    float* vy,
    float* vx_ordinal,
    float* vy_ordinal,
    bool* is_cancel)
{
    // Not supported in Windows.
    NOTIMPLEMENTED();
    return false;
}

int GetModifiersFromKeyState()
{
    int modifiers = EF_NONE;
    if (ui::win::IsShiftPressed())
        modifiers |= EF_SHIFT_DOWN;
    if (ui::win::IsCtrlPressed())
        modifiers |= EF_CONTROL_DOWN;
    if (ui::win::IsAltPressed())
        modifiers |= EF_ALT_DOWN;
    if (ui::win::IsWindowsKeyPressed())
        modifiers |= EF_COMMAND_DOWN;
    if (ui::win::IsAltGrPressed())
        modifiers |= EF_ALTGR_DOWN;
    if (ui::win::IsNumLockOn())
        modifiers |= EF_NUM_LOCK_ON;
    if (ui::win::IsCapsLockOn())
        modifiers |= EF_CAPS_LOCK_ON;
    if (ui::win::IsScrollLockOn())
        modifiers |= EF_SCROLL_LOCK_ON;
    return modifiers;
}

// Windows emulates mouse messages for touch events.
bool IsMouseEventFromTouch(UINT message)
{
    return (message >= WM_MOUSEFIRST) && (message <= WM_MOUSELAST) && (GetMessageExtraInfo() & MOUSEEVENTF_FROMTOUCH) == MOUSEEVENTF_FROMTOUCH;
}

// Conversion scan_code and LParam each other.
// uint16_t scan_code:
//     ui/events/keycodes/dom/keycode_converter_data.inc
// 0 - 15bits: represetns the scan code.
// 28 - 30 bits (0xE000): represents whether this is an extended key or not.
//
// LPARAM lParam:
//     http://msdn.microsoft.com/en-us/library/windows/desktop/ms644984.aspx
// 16 - 23bits: represetns the scan code.
// 24bit (0x0100): represents whether this is an extended key or not.
uint16_t GetScanCodeFromLParam(LPARAM l_param)
{
    uint16_t scan_code = ((l_param >> 16) & 0x00FF);
    if (l_param & (1 << 24))
        scan_code |= 0xE000;
    return scan_code;
}

LPARAM GetLParamFromScanCode(uint16_t scan_code)
{
    LPARAM l_param = static_cast<LPARAM>(scan_code & 0x00FF) << 16;
    if ((scan_code & 0xE000) == 0xE000)
        l_param |= (1 << 24);
    return l_param;
}

} // namespace ui
