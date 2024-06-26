// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <stddef.h>
#include <stdint.h>

#include "base/macros.h"
#include "base/memory/scoped_ptr.h"
#include "build/build_config.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "ui/events/event.h"
#include "ui/events/event_utils.h"
#include "ui/events/keycodes/dom/dom_code.h"
#include "ui/events/keycodes/dom/keycode_converter.h"
#include "ui/events/test/events_test_utils.h"

#if defined(USE_X11)
#include "ui/events/test/events_test_utils_x11.h"
#include "ui/gfx/x/x11_types.h"
#include <X11/Xlib.h>
#endif

namespace ui {

TEST(EventTest, NoNativeEvent)
{
    KeyEvent keyev(ET_KEY_PRESSED, VKEY_SPACE, EF_NONE);
    EXPECT_FALSE(keyev.HasNativeEvent());
}

TEST(EventTest, NativeEvent)
{
#if defined(OS_WIN)
    MSG native_event = { NULL, WM_KEYUP, VKEY_A, 0 };
    KeyEvent keyev(native_event);
    EXPECT_TRUE(keyev.HasNativeEvent());
#elif defined(USE_X11)
    ScopedXI2Event event;
    event.InitKeyEvent(ET_KEY_RELEASED, VKEY_A, EF_NONE);
    KeyEvent keyev(event);
    EXPECT_TRUE(keyev.HasNativeEvent());
#endif
}

TEST(EventTest, GetCharacter)
{
    // Check if Control+Enter returns 10.
    KeyEvent keyev1(ET_KEY_PRESSED, VKEY_RETURN, EF_CONTROL_DOWN);
    EXPECT_EQ(10, keyev1.GetCharacter());
    // Check if Enter returns 13.
    KeyEvent keyev2(ET_KEY_PRESSED, VKEY_RETURN, EF_NONE);
    EXPECT_EQ(13, keyev2.GetCharacter());

#if defined(USE_X11)
    // For X11, test the functions with native_event() as well. crbug.com/107837
    ScopedXI2Event event;
    event.InitKeyEvent(ET_KEY_PRESSED, VKEY_RETURN, EF_CONTROL_DOWN);
    KeyEvent keyev3(event);
    EXPECT_EQ(10, keyev3.GetCharacter());

    event.InitKeyEvent(ET_KEY_PRESSED, VKEY_RETURN, EF_NONE);
    KeyEvent keyev4(event);
    EXPECT_EQ(13, keyev4.GetCharacter());
#endif

    // Check if expected Unicode character was returned for a key combination
    // contains Control.
    // e.g. Control+Shift+2 produces U+200C on "Persian" keyboard.
    // http://crbug.com/582453
    KeyEvent keyev5(0x200C, VKEY_UNKNOWN, EF_CONTROL_DOWN | EF_SHIFT_DOWN);
    EXPECT_EQ(0x200C, keyev5.GetCharacter());
}

TEST(EventTest, ClickCount)
{
    const gfx::Point origin(0, 0);
    MouseEvent mouseev(ET_MOUSE_PRESSED, origin, origin, EventTimeForNow(), 0, 0);
    for (int i = 1; i <= 3; ++i) {
        mouseev.SetClickCount(i);
        EXPECT_EQ(i, mouseev.GetClickCount());
    }
}

TEST(EventTest, RepeatedClick)
{
    const gfx::Point origin(0, 0);
    MouseEvent mouse_ev1(ET_MOUSE_PRESSED, origin, origin, EventTimeForNow(), 0,
        0);
    MouseEvent mouse_ev2(ET_MOUSE_PRESSED, origin, origin, EventTimeForNow(), 0,
        0);
    LocatedEventTestApi test_ev1(&mouse_ev1);
    LocatedEventTestApi test_ev2(&mouse_ev2);

    base::TimeDelta start = base::TimeDelta::FromMilliseconds(0);
    base::TimeDelta soon = start + base::TimeDelta::FromMilliseconds(1);
    base::TimeDelta later = start + base::TimeDelta::FromMilliseconds(1000);

    // Same event.
    test_ev1.set_location(gfx::Point(0, 0));
    test_ev2.set_location(gfx::Point(1, 0));
    test_ev1.set_time_stamp(start);
    test_ev2.set_time_stamp(start);
    EXPECT_FALSE(MouseEvent::IsRepeatedClickEvent(mouse_ev1, mouse_ev2));
    MouseEvent mouse_ev3(mouse_ev1);
    EXPECT_FALSE(MouseEvent::IsRepeatedClickEvent(mouse_ev1, mouse_ev3));

    // Close point.
    test_ev1.set_location(gfx::Point(0, 0));
    test_ev2.set_location(gfx::Point(1, 0));
    test_ev1.set_time_stamp(start);
    test_ev2.set_time_stamp(soon);
    EXPECT_TRUE(MouseEvent::IsRepeatedClickEvent(mouse_ev1, mouse_ev2));

    // Too far.
    test_ev1.set_location(gfx::Point(0, 0));
    test_ev2.set_location(gfx::Point(10, 0));
    test_ev1.set_time_stamp(start);
    test_ev2.set_time_stamp(soon);
    EXPECT_FALSE(MouseEvent::IsRepeatedClickEvent(mouse_ev1, mouse_ev2));

    // Too long a time between clicks.
    test_ev1.set_location(gfx::Point(0, 0));
    test_ev2.set_location(gfx::Point(0, 0));
    test_ev1.set_time_stamp(start);
    test_ev2.set_time_stamp(later);
    EXPECT_FALSE(MouseEvent::IsRepeatedClickEvent(mouse_ev1, mouse_ev2));
}

// Tests that an event only increases the click count and gets marked as a
// double click if a release event was seen for the previous click. This
// prevents the same PRESSED event from being processed twice:
// http://crbug.com/389162
TEST(EventTest, DoubleClickRequiresRelease)
{
    const gfx::Point origin1(0, 0);
    const gfx::Point origin2(100, 0);
    scoped_ptr<MouseEvent> ev;
    base::TimeDelta start = base::TimeDelta::FromMilliseconds(0);
    base::TimeDelta soon = start + base::TimeDelta::FromMilliseconds(1);

    ev.reset(new MouseEvent(ET_MOUSE_PRESSED, origin1, origin1, EventTimeForNow(),
        0, 0));
    ev->set_time_stamp(start);
    EXPECT_EQ(1, MouseEvent::GetRepeatCount(*ev));
    ev.reset(new MouseEvent(ET_MOUSE_PRESSED, origin1, origin1, EventTimeForNow(),
        0, 0));
    ev->set_time_stamp(start);
    EXPECT_EQ(1, MouseEvent::GetRepeatCount(*ev));

    ev.reset(new MouseEvent(ET_MOUSE_PRESSED, origin2, origin2, EventTimeForNow(),
        0, 0));
    ev->set_time_stamp(start);
    EXPECT_EQ(1, MouseEvent::GetRepeatCount(*ev));
    ev.reset(new MouseEvent(ET_MOUSE_RELEASED, origin2, origin2,
        EventTimeForNow(), 0, 0));
    ev->set_time_stamp(start);
    EXPECT_EQ(1, MouseEvent::GetRepeatCount(*ev));
    ev.reset(new MouseEvent(ET_MOUSE_PRESSED, origin2, origin2, EventTimeForNow(),
        0, 0));
    ev->set_time_stamp(soon);
    EXPECT_EQ(2, MouseEvent::GetRepeatCount(*ev));
    ev.reset(new MouseEvent(ET_MOUSE_RELEASED, origin2, origin2,
        EventTimeForNow(), 0, 0));
    ev->set_time_stamp(soon);
    EXPECT_EQ(2, MouseEvent::GetRepeatCount(*ev));
    MouseEvent::ResetLastClickForTest();
}

// Tests that clicking right and then left clicking does not generate a double
// click.
TEST(EventTest, SingleClickRightLeft)
{
    const gfx::Point origin(0, 0);
    scoped_ptr<MouseEvent> ev;
    base::TimeDelta start = base::TimeDelta::FromMilliseconds(0);
    base::TimeDelta soon = start + base::TimeDelta::FromMilliseconds(1);

    ev.reset(new MouseEvent(ET_MOUSE_PRESSED, origin, origin, EventTimeForNow(),
        ui::EF_RIGHT_MOUSE_BUTTON,
        ui::EF_RIGHT_MOUSE_BUTTON));
    ev->set_time_stamp(start);
    EXPECT_EQ(1, MouseEvent::GetRepeatCount(*ev));
    ev.reset(new MouseEvent(ET_MOUSE_PRESSED, origin, origin, EventTimeForNow(),
        ui::EF_LEFT_MOUSE_BUTTON, ui::EF_LEFT_MOUSE_BUTTON));
    ev->set_time_stamp(start);
    EXPECT_EQ(1, MouseEvent::GetRepeatCount(*ev));
    ev.reset(new MouseEvent(ET_MOUSE_RELEASED, origin, origin, EventTimeForNow(),
        ui::EF_LEFT_MOUSE_BUTTON, ui::EF_LEFT_MOUSE_BUTTON));
    ev->set_time_stamp(start);
    EXPECT_EQ(1, MouseEvent::GetRepeatCount(*ev));
    ev.reset(new MouseEvent(ET_MOUSE_PRESSED, origin, origin, EventTimeForNow(),
        ui::EF_LEFT_MOUSE_BUTTON, ui::EF_LEFT_MOUSE_BUTTON));
    ev->set_time_stamp(soon);
    EXPECT_EQ(2, MouseEvent::GetRepeatCount(*ev));
    MouseEvent::ResetLastClickForTest();
}

TEST(EventTest, KeyEvent)
{
    static const struct {
        KeyboardCode key_code;
        int flags;
        uint16_t character;
    } kTestData[] = {
        { VKEY_A, 0, 'a' },
        { VKEY_A, EF_SHIFT_DOWN, 'A' },
        { VKEY_A, EF_CAPS_LOCK_ON, 'A' },
        { VKEY_A, EF_SHIFT_DOWN | EF_CAPS_LOCK_ON, 'a' },
        { VKEY_A, EF_CONTROL_DOWN, 0x01 },
        { VKEY_A, EF_SHIFT_DOWN | EF_CONTROL_DOWN, '\x01' },
        { VKEY_Z, 0, 'z' },
        { VKEY_Z, EF_SHIFT_DOWN, 'Z' },
        { VKEY_Z, EF_CAPS_LOCK_ON, 'Z' },
        { VKEY_Z, EF_SHIFT_DOWN | EF_CAPS_LOCK_ON, 'z' },
        { VKEY_Z, EF_CONTROL_DOWN, '\x1A' },
        { VKEY_Z, EF_SHIFT_DOWN | EF_CONTROL_DOWN, '\x1A' },

        { VKEY_2, EF_CONTROL_DOWN, '\x12' },
        { VKEY_2, EF_SHIFT_DOWN | EF_CONTROL_DOWN, '\0' },
        { VKEY_6, EF_CONTROL_DOWN, '\x16' },
        { VKEY_6, EF_SHIFT_DOWN | EF_CONTROL_DOWN, '\x1E' },
        { VKEY_OEM_MINUS, EF_CONTROL_DOWN, '\x0D' },
        { VKEY_OEM_MINUS, EF_SHIFT_DOWN | EF_CONTROL_DOWN, '\x1F' },
        { VKEY_OEM_4, EF_CONTROL_DOWN, '\x1B' },
        { VKEY_OEM_4, EF_SHIFT_DOWN | EF_CONTROL_DOWN, '\x1B' },
        { VKEY_OEM_5, EF_CONTROL_DOWN, '\x1C' },
        { VKEY_OEM_5, EF_SHIFT_DOWN | EF_CONTROL_DOWN, '\x1C' },
        { VKEY_OEM_6, EF_CONTROL_DOWN, '\x1D' },
        { VKEY_OEM_6, EF_SHIFT_DOWN | EF_CONTROL_DOWN, '\x1D' },
        { VKEY_RETURN, EF_CONTROL_DOWN, '\x0A' },

        { VKEY_0, 0, '0' },
        { VKEY_0, EF_SHIFT_DOWN, ')' },
        { VKEY_0, EF_SHIFT_DOWN | EF_CAPS_LOCK_ON, ')' },
        { VKEY_0, EF_SHIFT_DOWN | EF_CONTROL_DOWN, '\x09' },

        { VKEY_9, 0, '9' },
        { VKEY_9, EF_SHIFT_DOWN, '(' },
        { VKEY_9, EF_SHIFT_DOWN | EF_CAPS_LOCK_ON, '(' },
        { VKEY_9, EF_SHIFT_DOWN | EF_CONTROL_DOWN, '\x08' },

        { VKEY_NUMPAD0, EF_CONTROL_DOWN, '\x10' },
        { VKEY_NUMPAD0, EF_SHIFT_DOWN, '0' },

        { VKEY_NUMPAD9, EF_CONTROL_DOWN, '\x19' },
        { VKEY_NUMPAD9, EF_SHIFT_DOWN, '9' },

        { VKEY_TAB, EF_NONE, '\t' },
        { VKEY_TAB, EF_CONTROL_DOWN, '\t' },
        { VKEY_TAB, EF_SHIFT_DOWN, '\t' },

        { VKEY_MULTIPLY, EF_CONTROL_DOWN, '\x0A' },
        { VKEY_MULTIPLY, EF_SHIFT_DOWN, '*' },
        { VKEY_ADD, EF_CONTROL_DOWN, '\x0B' },
        { VKEY_ADD, EF_SHIFT_DOWN, '+' },
        { VKEY_SUBTRACT, EF_CONTROL_DOWN, '\x0D' },
        { VKEY_SUBTRACT, EF_SHIFT_DOWN, '-' },
        { VKEY_DECIMAL, EF_CONTROL_DOWN, '\x0E' },
        { VKEY_DECIMAL, EF_SHIFT_DOWN, '.' },
        { VKEY_DIVIDE, EF_CONTROL_DOWN, '\x0F' },
        { VKEY_DIVIDE, EF_SHIFT_DOWN, '/' },

        { VKEY_OEM_1, EF_CONTROL_DOWN, '\x1B' },
        { VKEY_OEM_1, EF_SHIFT_DOWN, ':' },
        { VKEY_OEM_PLUS, EF_CONTROL_DOWN, '\x1D' },
        { VKEY_OEM_PLUS, EF_SHIFT_DOWN, '+' },
        { VKEY_OEM_COMMA, EF_CONTROL_DOWN, '\x0C' },
        { VKEY_OEM_COMMA, EF_SHIFT_DOWN, '<' },
        { VKEY_OEM_PERIOD, EF_CONTROL_DOWN, '\x0E' },
        { VKEY_OEM_PERIOD, EF_SHIFT_DOWN, '>' },
        { VKEY_OEM_3, EF_CONTROL_DOWN, '\x0' },
        { VKEY_OEM_3, EF_SHIFT_DOWN, '~' },
    };

    for (size_t i = 0; i < arraysize(kTestData); ++i) {
        KeyEvent key(ET_KEY_PRESSED,
            kTestData[i].key_code,
            kTestData[i].flags);
        EXPECT_EQ(kTestData[i].character, key.GetCharacter())
            << " Index:" << i << " key_code:" << kTestData[i].key_code;
    }
}

TEST(EventTest, KeyEventDirectUnicode)
{
    KeyEvent key(0x1234U, ui::VKEY_UNKNOWN, ui::EF_NONE);
    EXPECT_EQ(0x1234U, key.GetCharacter());
    EXPECT_EQ(ET_KEY_PRESSED, key.type());
    EXPECT_TRUE(key.is_char());
}

TEST(EventTest, NormalizeKeyEventFlags)
{
#if defined(USE_X11)
    // Normalize flags when KeyEvent is created from XEvent.
    ScopedXI2Event event;
    {
        event.InitKeyEvent(ET_KEY_PRESSED, VKEY_SHIFT, EF_SHIFT_DOWN);
        KeyEvent keyev(event);
        EXPECT_EQ(EF_SHIFT_DOWN, keyev.flags());
    }
    {
        event.InitKeyEvent(ET_KEY_RELEASED, VKEY_SHIFT, EF_SHIFT_DOWN);
        KeyEvent keyev(event);
        EXPECT_EQ(EF_NONE, keyev.flags());
    }
    {
        event.InitKeyEvent(ET_KEY_PRESSED, VKEY_CONTROL, EF_CONTROL_DOWN);
        KeyEvent keyev(event);
        EXPECT_EQ(EF_CONTROL_DOWN, keyev.flags());
    }
    {
        event.InitKeyEvent(ET_KEY_RELEASED, VKEY_CONTROL, EF_CONTROL_DOWN);
        KeyEvent keyev(event);
        EXPECT_EQ(EF_NONE, keyev.flags());
    }
    {
        event.InitKeyEvent(ET_KEY_PRESSED, VKEY_MENU, EF_ALT_DOWN);
        KeyEvent keyev(event);
        EXPECT_EQ(EF_ALT_DOWN, keyev.flags());
    }
    {
        event.InitKeyEvent(ET_KEY_RELEASED, VKEY_MENU, EF_ALT_DOWN);
        KeyEvent keyev(event);
        EXPECT_EQ(EF_NONE, keyev.flags());
    }
#endif

    // Do not normalize flags for synthesized events without
    // KeyEvent::NormalizeFlags called explicitly.
    {
        KeyEvent keyev(ET_KEY_PRESSED, VKEY_SHIFT, EF_SHIFT_DOWN);
        EXPECT_EQ(EF_SHIFT_DOWN, keyev.flags());
    }
    {
        KeyEvent keyev(ET_KEY_RELEASED, VKEY_SHIFT, EF_SHIFT_DOWN);
        EXPECT_EQ(EF_SHIFT_DOWN, keyev.flags());
        keyev.NormalizeFlags();
        EXPECT_EQ(EF_NONE, keyev.flags());
    }
    {
        KeyEvent keyev(ET_KEY_PRESSED, VKEY_CONTROL, EF_CONTROL_DOWN);
        EXPECT_EQ(EF_CONTROL_DOWN, keyev.flags());
    }
    {
        KeyEvent keyev(ET_KEY_RELEASED, VKEY_CONTROL, EF_CONTROL_DOWN);
        EXPECT_EQ(EF_CONTROL_DOWN, keyev.flags());
        keyev.NormalizeFlags();
        EXPECT_EQ(EF_NONE, keyev.flags());
    }
    {
        KeyEvent keyev(ET_KEY_PRESSED, VKEY_MENU, EF_ALT_DOWN);
        EXPECT_EQ(EF_ALT_DOWN, keyev.flags());
    }
    {
        KeyEvent keyev(ET_KEY_RELEASED, VKEY_MENU, EF_ALT_DOWN);
        EXPECT_EQ(EF_ALT_DOWN, keyev.flags());
        keyev.NormalizeFlags();
        EXPECT_EQ(EF_NONE, keyev.flags());
    }
}

TEST(EventTest, KeyEventCopy)
{
    KeyEvent key(ET_KEY_PRESSED, VKEY_A, EF_NONE);
    scoped_ptr<KeyEvent> copied_key(new KeyEvent(key));
    EXPECT_EQ(copied_key->type(), key.type());
    EXPECT_EQ(copied_key->key_code(), key.key_code());
}

TEST(EventTest, KeyEventCode)
{
    const DomCode kDomCodeForSpace = DomCode::SPACE;
    const char kCodeForSpace[] = "Space";
    ASSERT_EQ(kDomCodeForSpace,
        ui::KeycodeConverter::CodeStringToDomCode(kCodeForSpace));
    const uint16_t kNativeCodeSpace = ui::KeycodeConverter::DomCodeToNativeKeycode(kDomCodeForSpace);
    ASSERT_NE(ui::KeycodeConverter::InvalidNativeKeycode(), kNativeCodeSpace);
    ASSERT_EQ(kNativeCodeSpace,
        ui::KeycodeConverter::DomCodeToNativeKeycode(kDomCodeForSpace));

    {
        KeyEvent key(ET_KEY_PRESSED, VKEY_SPACE, kDomCodeForSpace, EF_NONE);
        EXPECT_EQ(kCodeForSpace, key.GetCodeString());
    }
    {
        // Regardless the KeyEvent.key_code (VKEY_RETURN), code should be
        // the specified value.
        KeyEvent key(ET_KEY_PRESSED, VKEY_RETURN, kDomCodeForSpace, EF_NONE);
        EXPECT_EQ(kCodeForSpace, key.GetCodeString());
    }
    {
        // If the synthetic event is initialized without code, the code is
        // determined from the KeyboardCode assuming a US keyboard layout.
        KeyEvent key(ET_KEY_PRESSED, VKEY_SPACE, EF_NONE);
        EXPECT_EQ(kCodeForSpace, key.GetCodeString());
    }
#if defined(USE_X11)
    {
        // KeyEvent converts from the native keycode (XKB) to the code.
        ScopedXI2Event xevent;
        xevent.InitKeyEvent(ET_KEY_PRESSED, VKEY_SPACE, kNativeCodeSpace);
        KeyEvent key(xevent);
        EXPECT_EQ(kCodeForSpace, key.GetCodeString());
    }
#endif // USE_X11
#if defined(OS_WIN)
    {
        // Test a non extended key.
        ASSERT_EQ((kNativeCodeSpace & 0xFF), kNativeCodeSpace);

        const LPARAM lParam = GetLParamFromScanCode(kNativeCodeSpace);
        MSG native_event = { NULL, WM_KEYUP, VKEY_SPACE, lParam };
        KeyEvent key(native_event);

        // KeyEvent converts from the native keycode (scan code) to the code.
        EXPECT_EQ(kCodeForSpace, key.GetCodeString());
    }
    {
        const char kCodeForHome[] = "Home";
        const uint16_t kNativeCodeHome = 0xe047;

        // 'Home' is an extended key with 0xe000 bits.
        ASSERT_NE((kNativeCodeHome & 0xFF), kNativeCodeHome);
        const LPARAM lParam = GetLParamFromScanCode(kNativeCodeHome);

        MSG native_event = { NULL, WM_KEYUP, VKEY_HOME, lParam };
        KeyEvent key(native_event);

        // KeyEvent converts from the native keycode (scan code) to the code.
        EXPECT_EQ(kCodeForHome, key.GetCodeString());
    }
#endif // OS_WIN
}

namespace {
#if defined(USE_X11)
    void SetKeyEventTimestamp(XEvent* event, long time)
    {
        event->xkey.time = time;
    }

    void AdvanceKeyEventTimestamp(XEvent* event)
    {
        event->xkey.time++;
    }

#elif defined(OS_WIN)
    void SetKeyEventTimestamp(MSG& msg, long time)
    {
        msg.time = time;
    }

    void AdvanceKeyEventTimestamp(MSG& msg)
    {
        msg.time++;
    }
#endif
} // namespace

#if defined(USE_X11) || defined(OS_WIN)
TEST(EventTest, AutoRepeat)
{
    const uint16_t kNativeCodeA = ui::KeycodeConverter::DomCodeToNativeKeycode(DomCode::US_A);
    const uint16_t kNativeCodeB = ui::KeycodeConverter::DomCodeToNativeKeycode(DomCode::US_B);
#if defined(USE_X11)
    ScopedXI2Event native_event_a_pressed;
    native_event_a_pressed.InitKeyEvent(ET_KEY_PRESSED, VKEY_A, kNativeCodeA);
    ScopedXI2Event native_event_a_pressed_1500;
    native_event_a_pressed_1500.InitKeyEvent(
        ET_KEY_PRESSED, VKEY_A, kNativeCodeA);
    ScopedXI2Event native_event_a_pressed_3000;
    native_event_a_pressed_3000.InitKeyEvent(
        ET_KEY_PRESSED, VKEY_A, kNativeCodeA);

    ScopedXI2Event native_event_a_released;
    native_event_a_released.InitKeyEvent(ET_KEY_RELEASED, VKEY_A, kNativeCodeA);
    ScopedXI2Event native_event_b_pressed;
    native_event_b_pressed.InitKeyEvent(ET_KEY_PRESSED, VKEY_B, kNativeCodeB);
    ScopedXI2Event native_event_a_pressed_nonstandard_state;
    native_event_a_pressed_nonstandard_state.InitKeyEvent(
        ET_KEY_PRESSED, VKEY_A, kNativeCodeA);
    // IBUS-GTK uses the mask (1 << 25) to detect reposted event.
    static_cast<XEvent*>(native_event_a_pressed_nonstandard_state)->xkey.state |= 1 << 25;
#elif defined(OS_WIN)
    const LPARAM lParam_a = GetLParamFromScanCode(kNativeCodeA);
    const LPARAM lParam_b = GetLParamFromScanCode(kNativeCodeB);
    MSG native_event_a_pressed = { NULL, WM_KEYDOWN, VKEY_A, lParam_a };
    MSG native_event_a_pressed_1500 = { NULL, WM_KEYDOWN, VKEY_A, lParam_a };
    MSG native_event_a_pressed_3000 = { NULL, WM_KEYDOWN, VKEY_A, lParam_a };
    MSG native_event_a_released = { NULL, WM_KEYUP, VKEY_A, lParam_a };
    MSG native_event_b_pressed = { NULL, WM_KEYUP, VKEY_B, lParam_b };
#endif
    SetKeyEventTimestamp(native_event_a_pressed_1500, 1500);
    SetKeyEventTimestamp(native_event_a_pressed_3000, 3000);

    {
        KeyEvent key_a1(native_event_a_pressed);
        EXPECT_FALSE(key_a1.is_repeat());

        KeyEvent key_a1_released(native_event_a_released);
        EXPECT_FALSE(key_a1_released.is_repeat());

        KeyEvent key_a2(native_event_a_pressed);
        EXPECT_FALSE(key_a2.is_repeat());

        AdvanceKeyEventTimestamp(native_event_a_pressed);
        KeyEvent key_a2_repeated(native_event_a_pressed);
        EXPECT_TRUE(key_a2_repeated.is_repeat());

        KeyEvent key_a2_released(native_event_a_released);
        EXPECT_FALSE(key_a2_released.is_repeat());
    }

    // Interleaved with different key press.
    {
        KeyEvent key_a3(native_event_a_pressed);
        EXPECT_FALSE(key_a3.is_repeat());

        KeyEvent key_b(native_event_b_pressed);
        EXPECT_FALSE(key_b.is_repeat());

        AdvanceKeyEventTimestamp(native_event_a_pressed);
        KeyEvent key_a3_again(native_event_a_pressed);
        EXPECT_FALSE(key_a3_again.is_repeat());

        AdvanceKeyEventTimestamp(native_event_a_pressed);
        KeyEvent key_a3_repeated(native_event_a_pressed);
        EXPECT_TRUE(key_a3_repeated.is_repeat());

        AdvanceKeyEventTimestamp(native_event_a_pressed);
        KeyEvent key_a3_repeated2(native_event_a_pressed);
        EXPECT_TRUE(key_a3_repeated2.is_repeat());

        KeyEvent key_a3_released(native_event_a_released);
        EXPECT_FALSE(key_a3_released.is_repeat());
    }

    // Hold the key longer than max auto repeat timeout.
    {
        KeyEvent key_a4_0(native_event_a_pressed);
        EXPECT_FALSE(key_a4_0.is_repeat());

        KeyEvent key_a4_1500(native_event_a_pressed_1500);
        EXPECT_TRUE(key_a4_1500.is_repeat());

        KeyEvent key_a4_3000(native_event_a_pressed_3000);
        EXPECT_TRUE(key_a4_3000.is_repeat());

        KeyEvent key_a4_released(native_event_a_released);
        EXPECT_FALSE(key_a4_released.is_repeat());
    }

#if defined(USE_X11)
    {
        KeyEvent key_a4_pressed(native_event_a_pressed);
        EXPECT_FALSE(key_a4_pressed.is_repeat());

        KeyEvent key_a4_pressed_nonstandard_state(
            native_event_a_pressed_nonstandard_state);
        EXPECT_FALSE(key_a4_pressed_nonstandard_state.is_repeat());
    }

    {
        KeyEvent key_a1(native_event_a_pressed);
        EXPECT_FALSE(key_a1.is_repeat());

        KeyEvent key_a1_with_same_event(native_event_a_pressed);
        EXPECT_FALSE(key_a1_with_same_event.is_repeat());
    }
#endif
}
#endif // USE_X11 || OS_WIN

TEST(EventTest, TouchEventRadiusDefaultsToOtherAxis)
{
    const base::TimeDelta time = base::TimeDelta::FromMilliseconds(0);
    const float non_zero_length1 = 30;
    const float non_zero_length2 = 46;

    TouchEvent event1(ui::ET_TOUCH_PRESSED, gfx::Point(0, 0), 0, 0, time,
        non_zero_length1, 0, 0, 0);
    EXPECT_EQ(non_zero_length1, event1.pointer_details().radius_x);
    EXPECT_EQ(non_zero_length1, event1.pointer_details().radius_y);

    TouchEvent event2(ui::ET_TOUCH_PRESSED, gfx::Point(0, 0), 0, 0, time,
        0, non_zero_length2, 0, 0);
    EXPECT_EQ(non_zero_length2, event2.pointer_details().radius_x);
    EXPECT_EQ(non_zero_length2, event2.pointer_details().radius_y);
}

TEST(EventTest, TouchEventRotationAngleFixing)
{
    const base::TimeDelta time = base::TimeDelta::FromMilliseconds(0);
    const float radius_x = 20;
    const float radius_y = 10;

    {
        const float angle_in_range = 0;
        TouchEvent event(ui::ET_TOUCH_PRESSED, gfx::Point(0, 0), 0, 0, time,
            radius_x, radius_y, angle_in_range, 0);
        EXPECT_FLOAT_EQ(angle_in_range, event.rotation_angle());
    }

    {
        const float angle_in_range = 179.9f;
        TouchEvent event(ui::ET_TOUCH_PRESSED, gfx::Point(0, 0), 0, 0, time,
            radius_x, radius_y, angle_in_range, 0);
        EXPECT_FLOAT_EQ(angle_in_range, event.rotation_angle());
    }

    {
        const float angle_negative = -0.1f;
        TouchEvent event(ui::ET_TOUCH_PRESSED, gfx::Point(0, 0), 0, 0, time,
            radius_x, radius_y, angle_negative, 0);
        EXPECT_FLOAT_EQ(180 - 0.1f, event.rotation_angle());
    }

    {
        const float angle_negative = -200;
        TouchEvent event(ui::ET_TOUCH_PRESSED, gfx::Point(0, 0), 0, 0, time,
            radius_x, radius_y, angle_negative, 0);
        EXPECT_FLOAT_EQ(360 - 200, event.rotation_angle());
    }

    {
        const float angle_too_big = 180;
        TouchEvent event(ui::ET_TOUCH_PRESSED, gfx::Point(0, 0), 0, 0, time,
            radius_x, radius_y, angle_too_big, 0);
        EXPECT_FLOAT_EQ(0, event.rotation_angle());
    }

    {
        const float angle_too_big = 400;
        TouchEvent event(ui::ET_TOUCH_PRESSED, gfx::Point(0, 0), 0, 0, time,
            radius_x, radius_y, angle_too_big, 0);
        EXPECT_FLOAT_EQ(400 - 360, event.rotation_angle());
    }
}

TEST(EventTest, PointerDetailsTouch)
{
    ui::TouchEvent touch_event_plain(ET_TOUCH_PRESSED, gfx::Point(0, 0), 0,
        ui::EventTimeForNow());

    EXPECT_EQ(EventPointerType::POINTER_TYPE_TOUCH,
        touch_event_plain.pointer_details().pointer_type);
    EXPECT_EQ(0.0f, touch_event_plain.pointer_details().radius_x);
    EXPECT_EQ(0.0f, touch_event_plain.pointer_details().radius_y);
    EXPECT_TRUE(std::isnan(touch_event_plain.pointer_details().force));
    EXPECT_EQ(0.0f, touch_event_plain.pointer_details().tilt_x);
    EXPECT_EQ(0.0f, touch_event_plain.pointer_details().tilt_y);

    ui::TouchEvent touch_event_with_details(ET_TOUCH_PRESSED, gfx::Point(0, 0), 0,
        0, ui::EventTimeForNow(), 10.0f, 5.0f,
        0.0f, 15.0f);

    EXPECT_EQ(EventPointerType::POINTER_TYPE_TOUCH,
        touch_event_with_details.pointer_details().pointer_type);
    EXPECT_EQ(10.0f, touch_event_with_details.pointer_details().radius_x);
    EXPECT_EQ(5.0f, touch_event_with_details.pointer_details().radius_y);
    EXPECT_EQ(15.0f, touch_event_with_details.pointer_details().force);
    EXPECT_EQ(0.0f, touch_event_with_details.pointer_details().tilt_x);
    EXPECT_EQ(0.0f, touch_event_with_details.pointer_details().tilt_y);

    ui::TouchEvent touch_event_copy(touch_event_with_details);
    EXPECT_EQ(touch_event_with_details.pointer_details(),
        touch_event_copy.pointer_details());
}

TEST(EventTest, PointerDetailsMouse)
{
    ui::MouseEvent mouse_event(ET_MOUSE_PRESSED, gfx::Point(0, 0),
        gfx::Point(0, 0), ui::EventTimeForNow(), 0, 0);

    EXPECT_EQ(EventPointerType::POINTER_TYPE_MOUSE,
        mouse_event.pointer_details().pointer_type);
    EXPECT_EQ(0.0f, mouse_event.pointer_details().radius_x);
    EXPECT_EQ(0.0f, mouse_event.pointer_details().radius_y);
    EXPECT_TRUE(std::isnan(mouse_event.pointer_details().force));
    EXPECT_EQ(0.0f, mouse_event.pointer_details().tilt_x);
    EXPECT_EQ(0.0f, mouse_event.pointer_details().tilt_y);

    ui::MouseEvent mouse_event_copy(mouse_event);
    EXPECT_EQ(mouse_event.pointer_details(), mouse_event_copy.pointer_details());
}

TEST(EventTest, PointerDetailsStylus)
{
    ui::MouseEvent stylus_event(ET_MOUSE_PRESSED, gfx::Point(0, 0),
        gfx::Point(0, 0), ui::EventTimeForNow(), 0, 0);
    ui::PointerDetails pointer_details(EventPointerType::POINTER_TYPE_PEN,
        /* radius_x */ 0.0f,
        /* radius_y */ 0.0f,
        /* force */ 21.0f,
        /* tilt_x */ 45.0f,
        /* tilt_y */ -45.0f);

    stylus_event.set_pointer_details(pointer_details);
    EXPECT_EQ(EventPointerType::POINTER_TYPE_PEN,
        stylus_event.pointer_details().pointer_type);
    EXPECT_EQ(21.0f, stylus_event.pointer_details().force);
    EXPECT_EQ(45.0f, stylus_event.pointer_details().tilt_x);
    EXPECT_EQ(-45.0f, stylus_event.pointer_details().tilt_y);
    EXPECT_EQ(0.0f, stylus_event.pointer_details().radius_x);
    EXPECT_EQ(0.0f, stylus_event.pointer_details().radius_y);

    ui::MouseEvent stylus_event_copy(stylus_event);
    EXPECT_EQ(stylus_event.pointer_details(),
        stylus_event_copy.pointer_details());
}

TEST(EventTest, PointerDetailsCustomTouch)
{
    ui::TouchEvent touch_event(ET_TOUCH_PRESSED, gfx::Point(0, 0), 0,
        ui::EventTimeForNow());

    EXPECT_EQ(EventPointerType::POINTER_TYPE_TOUCH,
        touch_event.pointer_details().pointer_type);
    EXPECT_EQ(0.0f, touch_event.pointer_details().radius_x);
    EXPECT_EQ(0.0f, touch_event.pointer_details().radius_y);
    EXPECT_TRUE(std::isnan(touch_event.pointer_details().force));
    EXPECT_EQ(0.0f, touch_event.pointer_details().tilt_x);
    EXPECT_EQ(0.0f, touch_event.pointer_details().tilt_y);

    ui::PointerDetails pointer_details(EventPointerType::POINTER_TYPE_PEN,
        /* radius_x */ 5.0f,
        /* radius_y */ 6.0f,
        /* force */ 21.0f,
        /* tilt_x */ 45.0f,
        /* tilt_y */ -45.0f);
    touch_event.set_pointer_details(pointer_details);

    EXPECT_EQ(EventPointerType::POINTER_TYPE_PEN,
        touch_event.pointer_details().pointer_type);
    EXPECT_EQ(21.0f, touch_event.pointer_details().force);
    EXPECT_EQ(45.0f, touch_event.pointer_details().tilt_x);
    EXPECT_EQ(-45.0f, touch_event.pointer_details().tilt_y);
    EXPECT_EQ(5.0f, touch_event.pointer_details().radius_x);
    EXPECT_EQ(6.0f, touch_event.pointer_details().radius_y);

    ui::TouchEvent touch_event_copy(touch_event);
    EXPECT_EQ(touch_event.pointer_details(), touch_event_copy.pointer_details());
}

TEST(EventTest, PointerEventType)
{
    const ui::EventType kMouseTypeMap[][2] = {
        { ui::ET_MOUSE_PRESSED, ui::ET_POINTER_DOWN },
        { ui::ET_MOUSE_DRAGGED, ui::ET_POINTER_MOVED },
        { ui::ET_MOUSE_MOVED, ui::ET_POINTER_MOVED },
        { ui::ET_MOUSE_ENTERED, ui::ET_POINTER_ENTERED },
        { ui::ET_MOUSE_EXITED, ui::ET_POINTER_EXITED },
        { ui::ET_MOUSE_RELEASED, ui::ET_POINTER_UP },
    };
    const ui::EventType kTouchTypeMap[][2] = {
        { ui::ET_TOUCH_PRESSED, ui::ET_POINTER_DOWN },
        { ui::ET_TOUCH_MOVED, ui::ET_POINTER_MOVED },
        { ui::ET_TOUCH_RELEASED, ui::ET_POINTER_UP },
        { ui::ET_TOUCH_CANCELLED, ui::ET_POINTER_CANCELLED },
    };

    for (size_t i = 0; i < arraysize(kMouseTypeMap); i++) {
        ui::MouseEvent mouse_event(kMouseTypeMap[i][0], gfx::Point(0, 0),
            gfx::Point(0, 0), base::TimeDelta(), 0, 0);
        ui::PointerEvent pointer_event(mouse_event);
        EXPECT_EQ(kMouseTypeMap[i][1], pointer_event.type());
        EXPECT_FALSE(pointer_event.IsMouseEvent());
        EXPECT_FALSE(pointer_event.IsTouchEvent());
        EXPECT_TRUE(pointer_event.IsPointerEvent());
    }

    for (size_t i = 0; i < arraysize(kTouchTypeMap); i++) {
        ui::TouchEvent touch_event(kTouchTypeMap[i][0], gfx::Point(0, 0), 0,
            base::TimeDelta());
        ui::PointerEvent pointer_event(touch_event);
        EXPECT_EQ(kTouchTypeMap[i][1], pointer_event.type());
        EXPECT_FALSE(pointer_event.IsMouseEvent());
        EXPECT_FALSE(pointer_event.IsTouchEvent());
        EXPECT_TRUE(pointer_event.IsPointerEvent());
    }
}

TEST(EventTest, PointerEventId)
{
    {
        ui::MouseEvent mouse_event(ui::ET_MOUSE_PRESSED, gfx::Point(0, 0),
            gfx::Point(0, 0), base::TimeDelta(), 0, 0);
        ui::PointerEvent pointer_event(mouse_event);
        EXPECT_EQ(pointer_event.pointer_id(), ui::PointerEvent::kMousePointerId);
    }

    for (int touch_id = 0; touch_id < 8; touch_id++) {
        ui::TouchEvent touch_event(ui::ET_TOUCH_PRESSED, gfx::Point(0, 0), touch_id,
            base::TimeDelta());
        ui::PointerEvent pointer_event(touch_event);
        EXPECT_EQ(pointer_event.pointer_id(), touch_id);
    }
}

TEST(EventTest, PointerDetailsPointer)
{
    const float kRadiusX = 10.0f;
    const float kRadiusY = 5.0f;
    const float kForce = 15.0f;
    ui::TouchEvent touch_event(ET_TOUCH_PRESSED, gfx::Point(0, 0), 0, 0,
        ui::EventTimeForNow(), kRadiusX, kRadiusY, 0.0f,
        kForce);
    ui::PointerEvent pointer_event_from_touch(touch_event);
    EXPECT_EQ(kRadiusX, pointer_event_from_touch.pointer_details().radius_x);
    EXPECT_EQ(kRadiusY, pointer_event_from_touch.pointer_details().radius_y);
    EXPECT_EQ(kForce, pointer_event_from_touch.pointer_details().force);
    EXPECT_EQ(kRadiusX, pointer_event_from_touch.pointer_details().radius_x);
    EXPECT_EQ(0.0f, pointer_event_from_touch.pointer_details().tilt_x);
    EXPECT_EQ(0.0f, pointer_event_from_touch.pointer_details().tilt_y);
    EXPECT_EQ(EventPointerType::POINTER_TYPE_TOUCH,
        pointer_event_from_touch.pointer_details().pointer_type);

    ui::MouseEvent mouse_event(ET_MOUSE_PRESSED, gfx::Point(0, 0),
        gfx::Point(0, 0), ui::EventTimeForNow(), 0, 0);
    ui::PointerEvent pointer_event_from_mouse(mouse_event);
    EXPECT_EQ(mouse_event.pointer_details(),
        pointer_event_from_mouse.pointer_details());
}

} // namespace ui
