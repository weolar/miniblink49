// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <linux/input.h>
#include <utility>

#include "base/bind.h"
#include "base/macros.h"
#include "base/memory/scoped_ptr.h"
#include "base/memory/scoped_vector.h"
#include "base/message_loop/message_loop.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "ui/events/event.h"
#include "ui/events/keycodes/dom/dom_code.h"
#include "ui/events/keycodes/keyboard_codes.h"
#include "ui/events/ozone/device/device_manager.h"
#include "ui/events/ozone/evdev/cursor_delegate_evdev.h"
#include "ui/events/ozone/evdev/event_converter_evdev_impl.h"
#include "ui/events/ozone/evdev/event_converter_test_util.h"
#include "ui/events/ozone/evdev/event_factory_evdev.h"
#include "ui/events/ozone/evdev/keyboard_evdev.h"
#include "ui/events/ozone/layout/keyboard_layout_engine_manager.h"

namespace ui {

const char kTestDevicePath[] = "/dev/input/test-device";

class MockEventConverterEvdevImpl : public EventConverterEvdevImpl {
public:
    MockEventConverterEvdevImpl(int fd,
        CursorDelegateEvdev* cursor,
        DeviceEventDispatcherEvdev* dispatcher)
        : EventConverterEvdevImpl(fd,
            base::FilePath(kTestDevicePath),
            1,
            EventDeviceInfo(),
            cursor,
            dispatcher)
    {
        SetEnabled(true);
    }
    ~MockEventConverterEvdevImpl() override { SetEnabled(false); }

    // EventConverterEvdevImpl:
    bool HasKeyboard() const override { return true; }
    bool HasTouchpad() const override { return true; }

private:
    DISALLOW_COPY_AND_ASSIGN(MockEventConverterEvdevImpl);
};

class MockCursorEvdev : public CursorDelegateEvdev {
public:
    MockCursorEvdev() { }
    ~MockCursorEvdev() override { }

    // CursorDelegateEvdev:
    void MoveCursorTo(gfx::AcceleratedWidget widget,
        const gfx::PointF& location) override
    {
        cursor_location_ = location;
    }
    void MoveCursorTo(const gfx::PointF& location) override
    {
        cursor_location_ = location;
    }
    void MoveCursor(const gfx::Vector2dF& delta) override
    {
        cursor_location_ = gfx::PointF(delta.x(), delta.y());
    }
    bool IsCursorVisible() override { return 1; }
    gfx::Rect GetCursorConfinedBounds() override
    {
        NOTIMPLEMENTED();
        return gfx::Rect();
    }
    gfx::PointF GetLocation() override { return cursor_location_; }

private:
    // The location of the mock cursor.
    gfx::PointF cursor_location_;

    DISALLOW_COPY_AND_ASSIGN(MockCursorEvdev);
};

} // namespace ui

// Test fixture.
class EventConverterEvdevImplTest : public testing::Test {
public:
    EventConverterEvdevImplTest() { }

    // Overridden from testing::Test:
    void SetUp() override
    {
        // Set up pipe to satisfy message pump (unused).
        int evdev_io[2];
        if (pipe(evdev_io))
            PLOG(FATAL) << "failed pipe";
        events_in_ = evdev_io[0];
        events_out_ = evdev_io[1];

        cursor_.reset(new ui::MockCursorEvdev());

        device_manager_ = ui::CreateDeviceManagerForTest();
        event_factory_ = ui::CreateEventFactoryEvdevForTest(
            cursor_.get(), device_manager_.get(),
            ui::KeyboardLayoutEngineManager::GetKeyboardLayoutEngine(),
            base::Bind(&EventConverterEvdevImplTest::DispatchEventForTest,
                base::Unretained(this)));
        dispatcher_ = ui::CreateDeviceEventDispatcherEvdevForTest(event_factory_.get());
        device_.reset(new ui::MockEventConverterEvdevImpl(events_in_, cursor_.get(),
            dispatcher_.get()));
    }

    void TearDown() override
    {
        device_.reset();
        cursor_.reset();
        close(events_in_);
        close(events_out_);
    }

    ui::MockCursorEvdev* cursor() { return cursor_.get(); }
    ui::MockEventConverterEvdevImpl* device() { return device_.get(); }

    unsigned size() { return dispatched_events_.size(); }
    ui::KeyEvent* dispatched_event(unsigned index)
    {
        DCHECK_GT(dispatched_events_.size(), index);
        ui::Event* ev = dispatched_events_[index].get();
        DCHECK(ev->IsKeyEvent());
        return static_cast<ui::KeyEvent*>(ev);
    }
    ui::MouseEvent* dispatched_mouse_event(unsigned index)
    {
        DCHECK_GT(dispatched_events_.size(), index);
        ui::Event* ev = dispatched_events_[index].get();
        DCHECK(ev->IsMouseEvent());
        return static_cast<ui::MouseEvent*>(ev);
    }

    void ClearDispatchedEvents()
    {
        dispatched_events_.clear();
    }

    void DestroyDevice() { device_.reset(); }

private:
    void DispatchEventForTest(ui::Event* event)
    {
        scoped_ptr<ui::Event> cloned_event = ui::Event::Clone(*event);
        dispatched_events_.push_back(std::move(cloned_event));
    }

    base::MessageLoopForUI ui_loop_;

    scoped_ptr<ui::MockCursorEvdev> cursor_;
    scoped_ptr<ui::DeviceManager> device_manager_;
    scoped_ptr<ui::EventFactoryEvdev> event_factory_;
    scoped_ptr<ui::DeviceEventDispatcherEvdev> dispatcher_;
    scoped_ptr<ui::MockEventConverterEvdevImpl> device_;

    std::vector<scoped_ptr<ui::Event>> dispatched_events_;

    int events_out_;
    int events_in_;

    DISALLOW_COPY_AND_ASSIGN(EventConverterEvdevImplTest);
};

TEST_F(EventConverterEvdevImplTest, KeyPress)
{
    ui::MockEventConverterEvdevImpl* dev = device();

    struct input_event mock_kernel_queue[] = {
        { { 0, 0 }, EV_MSC, MSC_SCAN, 0x7002a },
        { { 0, 0 }, EV_KEY, KEY_BACKSPACE, 1 },
        { { 0, 0 }, EV_SYN, SYN_REPORT, 0 },

        { { 0, 0 }, EV_MSC, MSC_SCAN, 0x7002a },
        { { 0, 0 }, EV_KEY, KEY_BACKSPACE, 0 },
        { { 0, 0 }, EV_SYN, SYN_REPORT, 0 },
    };

    dev->ProcessEvents(mock_kernel_queue, arraysize(mock_kernel_queue));
    EXPECT_EQ(2u, size());

    ui::KeyEvent* event;

    event = dispatched_event(0);
    EXPECT_EQ(ui::ET_KEY_PRESSED, event->type());
    EXPECT_EQ(ui::VKEY_BACK, event->key_code());
    EXPECT_EQ(0, event->flags());

    event = dispatched_event(1);
    EXPECT_EQ(ui::ET_KEY_RELEASED, event->type());
    EXPECT_EQ(ui::VKEY_BACK, event->key_code());
    EXPECT_EQ(0, event->flags());
}

TEST_F(EventConverterEvdevImplTest, KeyRepeat)
{
    ui::MockEventConverterEvdevImpl* dev = device();

    struct input_event mock_kernel_queue[] = {
        { { 0, 0 }, EV_MSC, MSC_SCAN, 0x7002a },
        { { 0, 0 }, EV_KEY, KEY_BACKSPACE, 1 },
        { { 0, 0 }, EV_SYN, SYN_REPORT, 0 },

        { { 0, 0 }, EV_MSC, MSC_SCAN, 0x7002a },
        { { 0, 0 }, EV_KEY, KEY_BACKSPACE, 2 },
        { { 0, 0 }, EV_SYN, SYN_REPORT, 0 },

        { { 0, 0 }, EV_MSC, MSC_SCAN, 0x7002a },
        { { 0, 0 }, EV_KEY, KEY_BACKSPACE, 2 },
        { { 0, 0 }, EV_SYN, SYN_REPORT, 0 },

        { { 0, 0 }, EV_MSC, MSC_SCAN, 0x7002a },
        { { 0, 0 }, EV_KEY, KEY_BACKSPACE, 0 },
        { { 0, 0 }, EV_SYN, SYN_REPORT, 0 },
    };

    dev->ProcessEvents(mock_kernel_queue, arraysize(mock_kernel_queue));
    EXPECT_EQ(2u, size());

    ui::KeyEvent* event;

    event = dispatched_event(0);
    EXPECT_EQ(ui::ET_KEY_PRESSED, event->type());
    EXPECT_EQ(ui::VKEY_BACK, event->key_code());
    EXPECT_EQ(0, event->flags());

    event = dispatched_event(1);
    EXPECT_EQ(ui::ET_KEY_RELEASED, event->type());
    EXPECT_EQ(ui::VKEY_BACK, event->key_code());
    EXPECT_EQ(0, event->flags());
}

TEST_F(EventConverterEvdevImplTest, NoEvents)
{
    ui::MockEventConverterEvdevImpl* dev = device();
    dev->ProcessEvents(NULL, 0);
    EXPECT_EQ(0u, size());
}

TEST_F(EventConverterEvdevImplTest, KeyWithModifier)
{
    ui::MockEventConverterEvdevImpl* dev = device();

    struct input_event mock_kernel_queue[] = {
        { { 0, 0 }, EV_MSC, MSC_SCAN, 0x700e1 },
        { { 0, 0 }, EV_KEY, KEY_LEFTSHIFT, 1 },
        { { 0, 0 }, EV_SYN, SYN_REPORT, 0 },

        { { 0, 0 }, EV_MSC, MSC_SCAN, 0x70004 },
        { { 0, 0 }, EV_KEY, KEY_A, 1 },
        { { 0, 0 }, EV_SYN, SYN_REPORT, 0 },

        { { 0, 0 }, EV_MSC, MSC_SCAN, 0x70004 },
        { { 0, 0 }, EV_KEY, KEY_A, 0 },
        { { 0, 0 }, EV_SYN, SYN_REPORT, 0 },

        { { 0, 0 }, EV_MSC, MSC_SCAN, 0x700e1 },
        { { 0, 0 }, EV_KEY, KEY_LEFTSHIFT, 0 },
        { { 0, 0 }, EV_SYN, SYN_REPORT, 0 },
    };

    dev->ProcessEvents(mock_kernel_queue, arraysize(mock_kernel_queue));
    EXPECT_EQ(4u, size());

    ui::KeyEvent* event;

    event = dispatched_event(0);
    EXPECT_EQ(ui::ET_KEY_PRESSED, event->type());
    EXPECT_EQ(ui::VKEY_SHIFT, event->key_code());
    EXPECT_EQ(ui::EF_SHIFT_DOWN, event->flags());

    event = dispatched_event(1);
    EXPECT_EQ(ui::ET_KEY_PRESSED, event->type());
    EXPECT_EQ(ui::VKEY_A, event->key_code());
    EXPECT_EQ(ui::EF_SHIFT_DOWN, event->flags());

    event = dispatched_event(2);
    EXPECT_EQ(ui::ET_KEY_RELEASED, event->type());
    EXPECT_EQ(ui::VKEY_A, event->key_code());
    EXPECT_EQ(ui::EF_SHIFT_DOWN, event->flags());

    event = dispatched_event(3);
    EXPECT_EQ(ui::ET_KEY_RELEASED, event->type());
    EXPECT_EQ(ui::VKEY_SHIFT, event->key_code());
    EXPECT_EQ(0, event->flags());
}

TEST_F(EventConverterEvdevImplTest, KeyWithDuplicateModifier)
{
    ui::MockEventConverterEvdevImpl* dev = device();

    struct input_event mock_kernel_queue[] = {
        { { 0, 0 }, EV_MSC, MSC_SCAN, 0x700e1 },
        { { 0, 0 }, EV_KEY, KEY_LEFTCTRL, 1 },
        { { 0, 0 }, EV_SYN, SYN_REPORT, 0 },

        { { 0, 0 }, EV_MSC, MSC_SCAN, 0x700e5 },
        { { 0, 0 }, EV_KEY, KEY_RIGHTCTRL, 1 },
        { { 0, 0 }, EV_SYN, SYN_REPORT, 0 },

        { { 0, 0 }, EV_MSC, MSC_SCAN, 0x7001d },
        { { 0, 0 }, EV_KEY, KEY_Z, 1 },
        { { 0, 0 }, EV_SYN, SYN_REPORT, 0 },

        { { 0, 0 }, EV_MSC, MSC_SCAN, 0x7001d },
        { { 0, 0 }, EV_KEY, KEY_Z, 0 },
        { { 0, 0 }, EV_SYN, SYN_REPORT, 0 },

        { { 0, 0 }, EV_MSC, MSC_SCAN, 0x700e1 },
        { { 0, 0 }, EV_KEY, KEY_LEFTCTRL, 0 },
        { { 0, 0 }, EV_SYN, SYN_REPORT, 0 },

        { { 0, 0 }, EV_MSC, MSC_SCAN, 0x700e5 },
        { { 0, 0 }, EV_KEY, KEY_RIGHTCTRL, 0 },
        { { 0, 0 }, EV_SYN, SYN_REPORT, 0 },
    };

    dev->ProcessEvents(mock_kernel_queue, arraysize(mock_kernel_queue));
    EXPECT_EQ(6u, size());

    ui::KeyEvent* event;

    event = dispatched_event(0);
    EXPECT_EQ(ui::ET_KEY_PRESSED, event->type());
    EXPECT_EQ(ui::VKEY_CONTROL, event->key_code());
    EXPECT_EQ(ui::EF_CONTROL_DOWN, event->flags());

    event = dispatched_event(1);
    EXPECT_EQ(ui::ET_KEY_PRESSED, event->type());
    EXPECT_EQ(ui::VKEY_CONTROL, event->key_code());
    EXPECT_EQ(ui::EF_CONTROL_DOWN, event->flags());

    event = dispatched_event(2);
    EXPECT_EQ(ui::ET_KEY_PRESSED, event->type());
    EXPECT_EQ(ui::VKEY_Z, event->key_code());
    EXPECT_EQ(ui::EF_CONTROL_DOWN, event->flags());

    event = dispatched_event(3);
    EXPECT_EQ(ui::ET_KEY_RELEASED, event->type());
    EXPECT_EQ(ui::VKEY_Z, event->key_code());
    EXPECT_EQ(ui::EF_CONTROL_DOWN, event->flags());

    event = dispatched_event(4);
    EXPECT_EQ(ui::ET_KEY_RELEASED, event->type());
    EXPECT_EQ(ui::VKEY_CONTROL, event->key_code());
    EXPECT_EQ(ui::EF_CONTROL_DOWN, event->flags());

    event = dispatched_event(5);
    EXPECT_EQ(ui::ET_KEY_RELEASED, event->type());
    EXPECT_EQ(ui::VKEY_CONTROL, event->key_code());
    EXPECT_EQ(0, event->flags());
}

TEST_F(EventConverterEvdevImplTest, KeyWithLock)
{
    ui::MockEventConverterEvdevImpl* dev = device();

    struct input_event mock_kernel_queue[] = {
        { { 0, 0 }, EV_MSC, MSC_SCAN, 0x70039 },
        { { 0, 0 }, EV_KEY, KEY_CAPSLOCK, 1 },
        { { 0, 0 }, EV_SYN, SYN_REPORT, 0 },

        { { 0, 0 }, EV_MSC, MSC_SCAN, 0x70039 },
        { { 0, 0 }, EV_KEY, KEY_CAPSLOCK, 0 },
        { { 0, 0 }, EV_SYN, SYN_REPORT, 0 },
    };

    dev->ProcessEvents(mock_kernel_queue, arraysize(mock_kernel_queue));
    EXPECT_EQ(2u, size());

    ui::KeyEvent* event;

    event = dispatched_event(0);
    EXPECT_EQ(ui::ET_KEY_PRESSED, event->type());
    EXPECT_EQ(ui::VKEY_CAPITAL, event->key_code());
    EXPECT_EQ(ui::EF_MOD3_DOWN, event->flags());

    event = dispatched_event(1);
    EXPECT_EQ(ui::ET_KEY_RELEASED, event->type());
    EXPECT_EQ(ui::VKEY_CAPITAL, event->key_code());
    EXPECT_EQ(ui::EF_NONE, event->flags());
}

TEST_F(EventConverterEvdevImplTest, MouseButton)
{
    ui::MockEventConverterEvdevImpl* dev = device();

    struct input_event mock_kernel_queue[] = {
        { { 0, 0 }, EV_KEY, BTN_LEFT, 1 },
        { { 0, 0 }, EV_SYN, SYN_REPORT, 0 },

        { { 0, 0 }, EV_KEY, BTN_LEFT, 0 },
        { { 0, 0 }, EV_SYN, SYN_REPORT, 0 },
    };

    dev->ProcessEvents(mock_kernel_queue, arraysize(mock_kernel_queue));
    EXPECT_EQ(2u, size());

    ui::MouseEvent* event;

    event = dispatched_mouse_event(0);
    EXPECT_EQ(ui::ET_MOUSE_PRESSED, event->type());
    EXPECT_EQ(true, event->IsLeftMouseButton());

    event = dispatched_mouse_event(1);
    EXPECT_EQ(ui::ET_MOUSE_RELEASED, event->type());
    EXPECT_EQ(true, event->IsLeftMouseButton());
}

// Test that BTN_BACK and BTN_SIDE are treated as the same button.
TEST_F(EventConverterEvdevImplTest, MouseBackButton)
{
    ui::MockEventConverterEvdevImpl* dev = device();

    struct input_event mock_kernel_queue[] = {
        { { 0, 0 }, EV_KEY, BTN_SIDE, 1 },
        { { 0, 0 }, EV_SYN, SYN_REPORT, 0 },

        { { 0, 0 }, EV_KEY, BTN_BACK, 1 },
        { { 0, 0 }, EV_SYN, SYN_REPORT, 0 },

        { { 0, 0 }, EV_KEY, BTN_SIDE, 0 },
        { { 0, 0 }, EV_SYN, SYN_REPORT, 0 },

        { { 0, 0 }, EV_KEY, BTN_BACK, 0 },
        { { 0, 0 }, EV_SYN, SYN_REPORT, 0 }
    };

    dev->ProcessEvents(mock_kernel_queue, arraysize(mock_kernel_queue));
    EXPECT_EQ(2u, size());

    ui::MouseEvent* event = nullptr;

    event = dispatched_mouse_event(0);
    EXPECT_EQ(ui::ET_MOUSE_PRESSED, event->type());
    EXPECT_EQ(ui::EF_BACK_MOUSE_BUTTON, event->changed_button_flags());

    event = dispatched_mouse_event(1);
    EXPECT_EQ(ui::ET_MOUSE_RELEASED, event->type());
    EXPECT_EQ(ui::EF_BACK_MOUSE_BUTTON, event->changed_button_flags());
}

// Test that BTN_FORWARD and BTN_EXTRA are treated as the same button.
TEST_F(EventConverterEvdevImplTest, MouseForwardButton)
{
    ui::MockEventConverterEvdevImpl* dev = device();

    struct input_event mock_kernel_queue[] = {
        { { 0, 0 }, EV_KEY, BTN_FORWARD, 1 },
        { { 0, 0 }, EV_SYN, SYN_REPORT, 0 },

        { { 0, 0 }, EV_KEY, BTN_EXTRA, 1 },
        { { 0, 0 }, EV_SYN, SYN_REPORT, 0 },

        { { 0, 0 }, EV_KEY, BTN_EXTRA, 0 },
        { { 0, 0 }, EV_SYN, SYN_REPORT, 0 },

        { { 0, 0 }, EV_KEY, BTN_FORWARD, 0 },
        { { 0, 0 }, EV_SYN, SYN_REPORT, 0 }
    };

    dev->ProcessEvents(mock_kernel_queue, arraysize(mock_kernel_queue));
    EXPECT_EQ(2u, size());

    ui::MouseEvent* event = nullptr;

    event = dispatched_mouse_event(0);
    EXPECT_EQ(ui::ET_MOUSE_PRESSED, event->type());
    EXPECT_EQ(ui::EF_FORWARD_MOUSE_BUTTON, event->changed_button_flags());

    event = dispatched_mouse_event(1);
    EXPECT_EQ(ui::ET_MOUSE_RELEASED, event->type());
    EXPECT_EQ(ui::EF_FORWARD_MOUSE_BUTTON, event->changed_button_flags());
}

TEST_F(EventConverterEvdevImplTest, MouseMove)
{
    ui::MockEventConverterEvdevImpl* dev = device();

    struct input_event mock_kernel_queue[] = {
        { { 0, 0 }, EV_REL, REL_X, 4 },
        { { 0, 0 }, EV_REL, REL_Y, 2 },
        { { 0, 0 }, EV_SYN, SYN_REPORT, 0 },
    };

    dev->ProcessEvents(mock_kernel_queue, arraysize(mock_kernel_queue));
    EXPECT_EQ(1u, size());

    ui::MouseEvent* event;

    event = dispatched_mouse_event(0);
    EXPECT_EQ(ui::ET_MOUSE_MOVED, event->type());
    EXPECT_EQ(cursor()->GetLocation(), gfx::PointF(4, 2));
}

TEST_F(EventConverterEvdevImplTest, UnmappedKeyPress)
{
    ui::MockEventConverterEvdevImpl* dev = device();

    struct input_event mock_kernel_queue[] = {
        { { 0, 0 }, EV_KEY, BTN_TOUCH, 1 },
        { { 0, 0 }, EV_SYN, SYN_REPORT, 0 },

        { { 0, 0 }, EV_KEY, BTN_TOUCH, 0 },
        { { 0, 0 }, EV_SYN, SYN_REPORT, 0 },
    };

    dev->ProcessEvents(mock_kernel_queue, arraysize(mock_kernel_queue));
    EXPECT_EQ(0u, size());
}

TEST_F(EventConverterEvdevImplTest, ShouldReleaseKeysOnUnplug)
{
    ui::MockEventConverterEvdevImpl* dev = device();

    struct input_event mock_kernel_queue[] = {
        { { 0, 0 }, EV_KEY, KEY_A, 1 },
        { { 0, 0 }, EV_SYN, SYN_REPORT, 0 },
    };

    dev->ProcessEvents(mock_kernel_queue, arraysize(mock_kernel_queue));
    EXPECT_EQ(1u, size());

    DestroyDevice();
    EXPECT_EQ(2u, size());

    ui::KeyEvent* event = dispatched_event(0);
    EXPECT_EQ(ui::ET_KEY_PRESSED, event->type());
    EXPECT_EQ(ui::VKEY_A, event->key_code());

    event = dispatched_event(1);
    EXPECT_EQ(ui::ET_KEY_RELEASED, event->type());
    EXPECT_EQ(ui::VKEY_A, event->key_code());
}

TEST_F(EventConverterEvdevImplTest, ShouldReleaseKeysOnSynDropped)
{
    ui::MockEventConverterEvdevImpl* dev = device();

    struct input_event mock_kernel_queue[] = {
        { { 0, 0 }, EV_KEY, KEY_A, 1 },
        { { 0, 0 }, EV_SYN, SYN_REPORT, 0 },

        { { 0, 0 }, EV_SYN, SYN_DROPPED, 0 },
    };

    dev->ProcessEvents(mock_kernel_queue, arraysize(mock_kernel_queue));
    EXPECT_EQ(2u, size());

    ui::KeyEvent* event = dispatched_event(0);
    EXPECT_EQ(ui::ET_KEY_PRESSED, event->type());
    EXPECT_EQ(ui::VKEY_A, event->key_code());

    event = dispatched_event(1);
    EXPECT_EQ(ui::ET_KEY_RELEASED, event->type());
    EXPECT_EQ(ui::VKEY_A, event->key_code());
}

TEST_F(EventConverterEvdevImplTest, ShouldReleaseKeysOnDisable)
{
    ui::MockEventConverterEvdevImpl* dev = device();

    struct input_event mock_kernel_queue[] = {
        { { 0, 0 }, EV_KEY, KEY_A, 1 },
        { { 0, 0 }, EV_SYN, SYN_REPORT, 0 },
    };

    dev->ProcessEvents(mock_kernel_queue, arraysize(mock_kernel_queue));
    EXPECT_EQ(1u, size());

    dev->SetEnabled(false);
    EXPECT_EQ(2u, size());

    ui::KeyEvent* event = dispatched_event(0);
    EXPECT_EQ(ui::ET_KEY_PRESSED, event->type());
    EXPECT_EQ(ui::VKEY_A, event->key_code());

    event = dispatched_event(1);
    EXPECT_EQ(ui::ET_KEY_RELEASED, event->type());
    EXPECT_EQ(ui::VKEY_A, event->key_code());
}

// Test that SetAllowedKeys() causes events for non-allowed keys to be dropped.
TEST_F(EventConverterEvdevImplTest, SetAllowedKeys)
{
    ui::MockEventConverterEvdevImpl* dev = device();
    struct input_event mock_kernel_queue[] = {
        { { 0, 0 }, EV_KEY, KEY_A, 1 },
        { { 0, 0 }, EV_SYN, SYN_REPORT, 0 },

        { { 0, 0 }, EV_KEY, KEY_A, 0 },
        { { 0, 0 }, EV_SYN, SYN_REPORT, 0 },

        { { 0, 0 }, EV_KEY, KEY_POWER, 1 },
        { { 0, 0 }, EV_SYN, SYN_REPORT, 0 },

        { { 0, 0 }, EV_KEY, KEY_POWER, 0 },
        { { 0, 0 }, EV_KEY, SYN_REPORT, 0 },
    };

    dev->ProcessEvents(mock_kernel_queue, arraysize(mock_kernel_queue));

    ASSERT_EQ(4u, size());
    ui::KeyEvent* event = dispatched_event(0);
    EXPECT_EQ(ui::ET_KEY_PRESSED, event->type());
    EXPECT_EQ(ui::VKEY_A, event->key_code());
    event = dispatched_event(1);
    EXPECT_EQ(ui::ET_KEY_RELEASED, event->type());
    EXPECT_EQ(ui::VKEY_A, event->key_code());
    event = dispatched_event(2);
    EXPECT_EQ(ui::ET_KEY_PRESSED, event->type());
    EXPECT_EQ(ui::VKEY_POWER, event->key_code());
    event = dispatched_event(3);
    EXPECT_EQ(ui::ET_KEY_RELEASED, event->type());
    EXPECT_EQ(ui::VKEY_POWER, event->key_code());

    ClearDispatchedEvents();
    std::vector<ui::DomCode> allowed_keys;
    allowed_keys.push_back(ui::DomCode::POWER);
    dev->SetKeyFilter(true /* enable_filter */, allowed_keys);
    dev->ProcessEvents(mock_kernel_queue, arraysize(mock_kernel_queue));

    ASSERT_EQ(2u, size());
    event = dispatched_event(0);
    EXPECT_EQ(ui::ET_KEY_PRESSED, event->type());
    EXPECT_EQ(ui::VKEY_POWER, event->key_code());
    event = dispatched_event(1);
    EXPECT_EQ(ui::ET_KEY_RELEASED, event->type());
    EXPECT_EQ(ui::VKEY_POWER, event->key_code());

    ClearDispatchedEvents();
    dev->SetKeyFilter(false /* enable_filter */, std::vector<ui::DomCode>());
    dev->ProcessEvents(mock_kernel_queue, arraysize(mock_kernel_queue));

    event = dispatched_event(0);
    EXPECT_EQ(ui::ET_KEY_PRESSED, event->type());
    EXPECT_EQ(ui::VKEY_A, event->key_code());
    event = dispatched_event(1);
    EXPECT_EQ(ui::ET_KEY_RELEASED, event->type());
    EXPECT_EQ(ui::VKEY_A, event->key_code());
    event = dispatched_event(2);
    EXPECT_EQ(ui::ET_KEY_PRESSED, event->type());
    EXPECT_EQ(ui::VKEY_POWER, event->key_code());
    event = dispatched_event(3);
    EXPECT_EQ(ui::ET_KEY_RELEASED, event->type());
    EXPECT_EQ(ui::VKEY_POWER, event->key_code());
}

// Test that if a non-allowed key is pressed when SetAllowedKeys() is called
// that the non-allowed key is released.
TEST_F(EventConverterEvdevImplTest, SetAllowedKeysBlockedKeyPressed)
{
    ui::MockEventConverterEvdevImpl* dev = device();

    struct input_event key_press[] = {
        { { 0, 0 }, EV_KEY, KEY_A, 1 },
        { { 0, 0 }, EV_SYN, SYN_REPORT, 0 },
    };
    struct input_event key_release[] = {
        { { 0, 0 }, EV_KEY, KEY_A, 0 },
        { { 0, 0 }, EV_SYN, SYN_REPORT, 0 },
    };

    dev->ProcessEvents(key_press, arraysize(key_press));
    ASSERT_EQ(1u, size());
    ui::KeyEvent* event = dispatched_event(0);
    EXPECT_EQ(ui::ET_KEY_PRESSED, event->type());

    // Block all key events. Calling SetAllowKeys() should dispatch a synthetic
    // key release for VKEY_A.
    ClearDispatchedEvents();
    std::vector<ui::DomCode> allowed_keys;
    dev->SetKeyFilter(true /* enable_filter */, allowed_keys);
    ASSERT_EQ(1u, size());
    event = dispatched_event(0);
    EXPECT_EQ(ui::ET_KEY_RELEASED, event->type());

    // The real key release should be dropped, whenever it comes.
    ClearDispatchedEvents();
    dev->ProcessEvents(key_release, arraysize(key_release));
    ASSERT_EQ(0u, size());
}
