// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/events/test/event_generator.h"

#include <stddef.h>
#include <stdint.h>

#include <utility>

#include "base/bind.h"
#include "base/location.h"
#include "base/macros.h"
#include "base/memory/scoped_ptr.h"
#include "base/single_thread_task_runner.h"
#include "base/thread_task_runner_handle.h"
#include "base/time/tick_clock.h"
#include "build/build_config.h"
#include "ui/events/event.h"
#include "ui/events/event_source.h"
#include "ui/events/event_utils.h"
#include "ui/events/test/events_test_utils.h"
#include "ui/gfx/geometry/vector2d_conversions.h"

#if defined(USE_X11)
#include "ui/events/test/events_test_utils_x11.h"
#include <X11/Xlib.h>
#endif

#if defined(OS_WIN)
#include "ui/events/keycodes/keyboard_code_conversion.h"
#endif

namespace ui {
namespace test {
    namespace {

        void DummyCallback(EventType, const gfx::Vector2dF&)
        {
        }

        class TestTickClock : public base::TickClock {
        public:
            // Starts off with a clock set to TimeTicks().
            TestTickClock() { }

            base::TimeTicks NowTicks() override
            {
                return base::TimeTicks::FromInternalValue(ticks_++ * 1000);
            }

        private:
            int64_t ticks_ = 1;

            DISALLOW_COPY_AND_ASSIGN(TestTickClock);
        };

        class TestKeyEvent : public ui::KeyEvent {
        public:
            TestKeyEvent(const base::NativeEvent& native_event, int flags)
                : KeyEvent(native_event)
            {
                set_flags(flags);
            }
        };

        class TestTouchEvent : public ui::TouchEvent {
        public:
            TestTouchEvent(ui::EventType type,
                const gfx::Point& root_location,
                int touch_id,
                int flags,
                base::TimeDelta timestamp)
                : TouchEvent(type,
                    root_location,
                    flags,
                    touch_id,
                    timestamp,
                    1.0f,
                    1.0f,
                    0.0f,
                    0.0f)
            {
            }

        private:
            DISALLOW_COPY_AND_ASSIGN(TestTouchEvent);
        };

        const int kAllButtonMask = ui::EF_LEFT_MOUSE_BUTTON | ui::EF_RIGHT_MOUSE_BUTTON;

    } // namespace

    EventGeneratorDelegate* EventGenerator::default_delegate = NULL;

    EventGenerator::EventGenerator(gfx::NativeWindow root_window)
        : current_target_(NULL)
        , flags_(0)
        , grab_(false)
        , async_(false)
        , targeting_application_(false)
        , tick_clock_(new TestTickClock())
    {
        Init(root_window, NULL);
    }

    EventGenerator::EventGenerator(gfx::NativeWindow root_window,
        const gfx::Point& point)
        : current_location_(point)
        , current_target_(NULL)
        , flags_(0)
        , grab_(false)
        , async_(false)
        , targeting_application_(false)
        , tick_clock_(new TestTickClock())
    {
        Init(root_window, NULL);
    }

    EventGenerator::EventGenerator(gfx::NativeWindow root_window,
        gfx::NativeWindow window)
        : current_target_(NULL)
        , flags_(0)
        , grab_(false)
        , async_(false)
        , targeting_application_(false)
        , tick_clock_(new TestTickClock())
    {
        Init(root_window, window);
    }

    EventGenerator::EventGenerator(EventGeneratorDelegate* delegate)
        : delegate_(delegate)
        , current_target_(NULL)
        , flags_(0)
        , grab_(false)
        , async_(false)
        , targeting_application_(false)
        , tick_clock_(new TestTickClock())
    {
        Init(NULL, NULL);
    }

    EventGenerator::~EventGenerator()
    {
        for (std::list<ui::Event*>::iterator i = pending_events_.begin();
             i != pending_events_.end(); ++i)
            delete *i;
        pending_events_.clear();
        delegate()->SetContext(NULL, NULL, NULL);
    }

    void EventGenerator::PressLeftButton()
    {
        PressButton(ui::EF_LEFT_MOUSE_BUTTON);
    }

    void EventGenerator::ReleaseLeftButton()
    {
        ReleaseButton(ui::EF_LEFT_MOUSE_BUTTON);
    }

    void EventGenerator::ClickLeftButton()
    {
        PressLeftButton();
        ReleaseLeftButton();
    }

    void EventGenerator::DoubleClickLeftButton()
    {
        flags_ &= ~ui::EF_IS_DOUBLE_CLICK;
        ClickLeftButton();
        flags_ |= ui::EF_IS_DOUBLE_CLICK;
        ClickLeftButton();
        flags_ &= ~ui::EF_IS_DOUBLE_CLICK;
    }

    void EventGenerator::PressRightButton()
    {
        PressButton(ui::EF_RIGHT_MOUSE_BUTTON);
    }

    void EventGenerator::ReleaseRightButton()
    {
        ReleaseButton(ui::EF_RIGHT_MOUSE_BUTTON);
    }

    void EventGenerator::MoveMouseWheel(int delta_x, int delta_y)
    {
        gfx::Point location = GetLocationInCurrentRoot();
        ui::MouseEvent mouseev(ui::ET_MOUSEWHEEL, location, location,
            ui::EventTimeForNow(), flags_, 0);
        ui::MouseWheelEvent wheelev(mouseev, delta_x, delta_y);
        Dispatch(&wheelev);
    }

    void EventGenerator::SendMouseExit()
    {
        gfx::Point exit_location(current_location_);
        delegate()->ConvertPointToTarget(current_target_, &exit_location);
        ui::MouseEvent mouseev(ui::ET_MOUSE_EXITED, exit_location, exit_location,
            ui::EventTimeForNow(), flags_, 0);
        Dispatch(&mouseev);
    }

    void EventGenerator::MoveMouseToWithNative(const gfx::Point& point_in_host,
        const gfx::Point& point_for_native)
    {
#if defined(USE_X11)
        ui::ScopedXI2Event xevent;
        xevent.InitMotionEvent(point_in_host, point_for_native, flags_);
        static_cast<XEvent*>(xevent)->xmotion.time = Now().InMicroseconds();
        ui::MouseEvent mouseev(xevent);
#elif defined(USE_OZONE)
        // Ozone uses the location in native event as a system location.
        // Create a fake event with the point in host, which will be passed
        // to the non native event, then update the native event with the native
        // (root) one.
        scoped_ptr<ui::MouseEvent> native_event(new ui::MouseEvent(
            ui::ET_MOUSE_MOVED, point_in_host, point_in_host, Now(), flags_, 0));
        ui::MouseEvent mouseev(native_event.get());
        native_event->set_location(point_for_native);
#else
        ui::MouseEvent mouseev(ui::ET_MOUSE_MOVED, point_in_host, point_for_native,
            Now(), flags_, 0);
        LOG(FATAL)
            << "Generating a native motion event is not supported on this platform";
#endif
        Dispatch(&mouseev);

        current_location_ = point_in_host;
        delegate()->ConvertPointFromHost(current_target_, &current_location_);
    }

    void EventGenerator::MoveMouseToInHost(const gfx::Point& point_in_host)
    {
        const ui::EventType event_type = (flags_ & ui::EF_LEFT_MOUSE_BUTTON) ? ui::ET_MOUSE_DRAGGED : ui::ET_MOUSE_MOVED;
        ui::MouseEvent mouseev(event_type, point_in_host, point_in_host,
            ui::EventTimeForNow(), flags_, 0);
        Dispatch(&mouseev);

        current_location_ = point_in_host;
        delegate()->ConvertPointFromHost(current_target_, &current_location_);
    }

    void EventGenerator::MoveMouseTo(const gfx::Point& point_in_screen,
        int count)
    {
        DCHECK_GT(count, 0);
        const ui::EventType event_type = (flags_ & ui::EF_LEFT_MOUSE_BUTTON) ? ui::ET_MOUSE_DRAGGED : ui::ET_MOUSE_MOVED;

        gfx::Vector2dF diff(point_in_screen - current_location_);
        for (float i = 1; i <= count; i++) {
            gfx::Vector2dF step(diff);
            step.Scale(i / count);
            gfx::Point move_point = current_location_ + gfx::ToRoundedVector2d(step);
            if (!grab_)
                UpdateCurrentDispatcher(move_point);
            delegate()->ConvertPointToTarget(current_target_, &move_point);
            ui::MouseEvent mouseev(event_type, move_point, move_point,
                ui::EventTimeForNow(), flags_, 0);
            Dispatch(&mouseev);
        }
        current_location_ = point_in_screen;
    }

    void EventGenerator::MoveMouseRelativeTo(const EventTarget* window,
        const gfx::Point& point_in_parent)
    {
        gfx::Point point(point_in_parent);
        delegate()->ConvertPointFromTarget(window, &point);
        MoveMouseTo(point);
    }

    void EventGenerator::DragMouseTo(const gfx::Point& point)
    {
        PressLeftButton();
        MoveMouseTo(point);
        ReleaseLeftButton();
    }

    void EventGenerator::MoveMouseToCenterOf(EventTarget* window)
    {
        MoveMouseTo(CenterOfWindow(window));
    }

    void EventGenerator::PressTouch()
    {
        PressTouchId(0);
    }

    void EventGenerator::PressTouchId(int touch_id)
    {
        TestTouchEvent touchev(
            ui::ET_TOUCH_PRESSED, GetLocationInCurrentRoot(), touch_id, flags_,
            Now());
        Dispatch(&touchev);
    }

    void EventGenerator::MoveTouch(const gfx::Point& point)
    {
        MoveTouchId(point, 0);
    }

    void EventGenerator::MoveTouchId(const gfx::Point& point, int touch_id)
    {
        current_location_ = point;
        TestTouchEvent touchev(
            ui::ET_TOUCH_MOVED, GetLocationInCurrentRoot(), touch_id, flags_,
            Now());
        Dispatch(&touchev);

        if (!grab_)
            UpdateCurrentDispatcher(point);
    }

    void EventGenerator::ReleaseTouch()
    {
        ReleaseTouchId(0);
    }

    void EventGenerator::ReleaseTouchId(int touch_id)
    {
        TestTouchEvent touchev(
            ui::ET_TOUCH_RELEASED, GetLocationInCurrentRoot(), touch_id, flags_,
            Now());
        Dispatch(&touchev);
    }

    void EventGenerator::PressMoveAndReleaseTouchTo(const gfx::Point& point)
    {
        PressTouch();
        MoveTouch(point);
        ReleaseTouch();
    }

    void EventGenerator::PressMoveAndReleaseTouchToCenterOf(EventTarget* window)
    {
        PressMoveAndReleaseTouchTo(CenterOfWindow(window));
    }

    void EventGenerator::GestureEdgeSwipe()
    {
        ui::GestureEvent gesture(
            0, 0, 0, Now(), ui::GestureEventDetails(ui::ET_GESTURE_WIN8_EDGE_SWIPE));
        Dispatch(&gesture);
    }

    void EventGenerator::GestureTapAt(const gfx::Point& location)
    {
        const int kTouchId = 2;
        ui::TouchEvent press(ui::ET_TOUCH_PRESSED,
            location,
            kTouchId,
            Now());
        Dispatch(&press);

        ui::TouchEvent release(
            ui::ET_TOUCH_RELEASED, location, kTouchId,
            press.time_stamp() + base::TimeDelta::FromMilliseconds(50));
        Dispatch(&release);
    }

    void EventGenerator::GestureTapDownAndUp(const gfx::Point& location)
    {
        const int kTouchId = 3;
        ui::TouchEvent press(ui::ET_TOUCH_PRESSED,
            location,
            kTouchId,
            Now());
        Dispatch(&press);

        ui::TouchEvent release(
            ui::ET_TOUCH_RELEASED, location, kTouchId,
            press.time_stamp() + base::TimeDelta::FromMilliseconds(1000));
        Dispatch(&release);
    }

    base::TimeDelta EventGenerator::CalculateScrollDurationForFlingVelocity(
        const gfx::Point& start,
        const gfx::Point& end,
        float velocity,
        int steps)
    {
        const float kGestureDistance = (start - end).Length();
        const float kFlingStepDelay = (kGestureDistance / velocity) / steps * 1000000;
        return base::TimeDelta::FromMicroseconds(kFlingStepDelay);
    }

    void EventGenerator::GestureScrollSequence(const gfx::Point& start,
        const gfx::Point& end,
        const base::TimeDelta& step_delay,
        int steps)
    {
        GestureScrollSequenceWithCallback(start, end, step_delay, steps,
            base::Bind(&DummyCallback));
    }

    void EventGenerator::GestureScrollSequenceWithCallback(
        const gfx::Point& start,
        const gfx::Point& end,
        const base::TimeDelta& step_delay,
        int steps,
        const ScrollStepCallback& callback)
    {
        const int kTouchId = 5;
        base::TimeDelta timestamp = Now();
        ui::TouchEvent press(ui::ET_TOUCH_PRESSED, start, 0, kTouchId,
            timestamp, 5.0f, 5.0f, 0.0f, 1.0f);
        Dispatch(&press);

        callback.Run(ui::ET_GESTURE_SCROLL_BEGIN, gfx::Vector2dF());

        float dx = static_cast<float>(end.x() - start.x()) / steps;
        float dy = static_cast<float>(end.y() - start.y()) / steps;
        gfx::PointF location(start);
        for (int i = 0; i < steps; ++i) {
            location.Offset(dx, dy);
            timestamp += step_delay;
            ui::TouchEvent move(ui::ET_TOUCH_MOVED, gfx::Point(), 0, kTouchId,
                timestamp, 5.0f, 5.0f, 0.0f, 1.0f);
            move.set_location_f(location);
            move.set_root_location_f(location);
            Dispatch(&move);
            callback.Run(ui::ET_GESTURE_SCROLL_UPDATE, gfx::Vector2dF(dx, dy));
        }

        ui::TouchEvent release(ui::ET_TOUCH_RELEASED, end, 0, kTouchId,
            timestamp, 5.0f, 5.0f, 0.0f, 1.0f);
        Dispatch(&release);

        callback.Run(ui::ET_GESTURE_SCROLL_END, gfx::Vector2dF());
    }

    void EventGenerator::GestureMultiFingerScroll(int count,
        const gfx::Point start[],
        int event_separation_time_ms,
        int steps,
        int move_x,
        int move_y)
    {
        const int kMaxTouchPoints = 10;
        int delays[kMaxTouchPoints] = { 0 };
        GestureMultiFingerScrollWithDelays(
            count, start, delays, event_separation_time_ms, steps, move_x, move_y);
    }

    void EventGenerator::GestureMultiFingerScrollWithDelays(
        int count,
        const gfx::Point start[],
        const int delay_adding_finger_ms[],
        int event_separation_time_ms,
        int steps,
        int move_x,
        int move_y)
    {
        const int kMaxTouchPoints = 10;
        gfx::Point points[kMaxTouchPoints];
        CHECK_LE(count, kMaxTouchPoints);
        CHECK_GT(steps, 0);

        int delta_x = move_x / steps;
        int delta_y = move_y / steps;

        for (int i = 0; i < count; ++i) {
            points[i] = start[i];
        }

        base::TimeDelta press_time_first = Now();
        base::TimeDelta press_time[kMaxTouchPoints];
        bool pressed[kMaxTouchPoints];
        for (int i = 0; i < count; ++i) {
            pressed[i] = false;
            press_time[i] = press_time_first + base::TimeDelta::FromMilliseconds(delay_adding_finger_ms[i]);
        }

        int last_id = 0;
        for (int step = 0; step < steps; ++step) {
            base::TimeDelta move_time = press_time_first + base::TimeDelta::FromMilliseconds(event_separation_time_ms * step);

            while (last_id < count && !pressed[last_id] && move_time >= press_time[last_id]) {
                ui::TouchEvent press(ui::ET_TOUCH_PRESSED,
                    points[last_id],
                    last_id,
                    press_time[last_id]);
                Dispatch(&press);
                pressed[last_id] = true;
                last_id++;
            }

            for (int i = 0; i < count; ++i) {
                points[i].Offset(delta_x, delta_y);
                if (i >= last_id)
                    continue;
                ui::TouchEvent move(ui::ET_TOUCH_MOVED, points[i], i, move_time);
                Dispatch(&move);
            }
        }

        base::TimeDelta release_time = press_time_first + base::TimeDelta::FromMilliseconds(event_separation_time_ms * steps);
        for (int i = 0; i < last_id; ++i) {
            ui::TouchEvent release(
                ui::ET_TOUCH_RELEASED, points[i], i, release_time);
            Dispatch(&release);
        }
    }

    void EventGenerator::ScrollSequence(const gfx::Point& start,
        const base::TimeDelta& step_delay,
        float x_offset,
        float y_offset,
        int steps,
        int num_fingers)
    {
        base::TimeDelta timestamp = Now();
        ui::ScrollEvent fling_cancel(ui::ET_SCROLL_FLING_CANCEL,
            start,
            timestamp,
            0,
            0, 0,
            0, 0,
            num_fingers);
        Dispatch(&fling_cancel);

        float dx = x_offset / steps;
        float dy = y_offset / steps;
        for (int i = 0; i < steps; ++i) {
            timestamp += step_delay;
            ui::ScrollEvent move(ui::ET_SCROLL,
                start,
                timestamp,
                0,
                dx, dy,
                dx, dy,
                num_fingers);
            Dispatch(&move);
        }

        ui::ScrollEvent fling_start(ui::ET_SCROLL_FLING_START,
            start,
            timestamp,
            0,
            x_offset, y_offset,
            x_offset, y_offset,
            num_fingers);
        Dispatch(&fling_start);
    }

    void EventGenerator::ScrollSequence(const gfx::Point& start,
        const base::TimeDelta& step_delay,
        const std::vector<gfx::PointF>& offsets,
        int num_fingers)
    {
        size_t steps = offsets.size();
        base::TimeDelta timestamp = Now();
        ui::ScrollEvent fling_cancel(ui::ET_SCROLL_FLING_CANCEL,
            start,
            timestamp,
            0,
            0, 0,
            0, 0,
            num_fingers);
        Dispatch(&fling_cancel);

        for (size_t i = 0; i < steps; ++i) {
            timestamp += step_delay;
            ui::ScrollEvent scroll(ui::ET_SCROLL,
                start,
                timestamp,
                0,
                offsets[i].x(), offsets[i].y(),
                offsets[i].x(), offsets[i].y(),
                num_fingers);
            Dispatch(&scroll);
        }

        ui::ScrollEvent fling_start(ui::ET_SCROLL_FLING_START,
            start,
            timestamp,
            0,
            offsets[steps - 1].x(), offsets[steps - 1].y(),
            offsets[steps - 1].x(), offsets[steps - 1].y(),
            num_fingers);
        Dispatch(&fling_start);
    }

    void EventGenerator::PressKey(ui::KeyboardCode key_code, int flags)
    {
        DispatchKeyEvent(true, key_code, flags);
    }

    void EventGenerator::ReleaseKey(ui::KeyboardCode key_code, int flags)
    {
        DispatchKeyEvent(false, key_code, flags);
    }

    void EventGenerator::Dispatch(ui::Event* event)
    {
        DoDispatchEvent(event, async_);
    }

    void EventGenerator::SetTickClock(scoped_ptr<base::TickClock> tick_clock)
    {
        tick_clock_ = std::move(tick_clock);
    }

    base::TimeDelta EventGenerator::Now()
    {
        // This is the same as what EventTimeForNow() does, but here we do it
        // with a tick clock that can be replaced with a simulated clock for tests.
        return base::TimeDelta::FromInternalValue(
            tick_clock_->NowTicks().ToInternalValue());
    }

    void EventGenerator::Init(gfx::NativeWindow root_window,
        gfx::NativeWindow window_context)
    {
        delegate()->SetContext(this, root_window, window_context);
        if (window_context)
            current_location_ = delegate()->CenterOfWindow(window_context);
        current_target_ = delegate()->GetTargetAt(current_location_);
    }

    void EventGenerator::DispatchKeyEvent(bool is_press,
        ui::KeyboardCode key_code,
        int flags)
    {
#if defined(OS_WIN)
        UINT key_press = WM_KEYDOWN;
        uint16_t character = ui::DomCodeToUsLayoutCharacter(
            ui::UsLayoutKeyboardCodeToDomCode(key_code), flags);
        if (is_press && character) {
            MSG native_event = { NULL, WM_KEYDOWN, key_code, 0 };
            TestKeyEvent keyev(native_event, flags);
            Dispatch(&keyev);
            // On Windows, WM_KEYDOWN event is followed by WM_CHAR with a character
            // if the key event cooresponds to a real character.
            key_press = WM_CHAR;
            key_code = static_cast<ui::KeyboardCode>(character);
        }
        MSG native_event = { NULL, (is_press ? key_press : WM_KEYUP), key_code, 0 };
        native_event.time = Now().InMicroseconds();
        TestKeyEvent keyev(native_event, flags);
#elif defined(USE_X11)
        ui::ScopedXI2Event xevent;
        xevent.InitKeyEvent(is_press ? ui::ET_KEY_PRESSED : ui::ET_KEY_RELEASED,
            key_code,
            flags);
        static_cast<XEvent*>(xevent)->xkey.time = Now().InMicroseconds();
        ui::KeyEvent keyev(xevent);
#else
        ui::EventType type = is_press ? ui::ET_KEY_PRESSED : ui::ET_KEY_RELEASED;
        ui::KeyEvent keyev(type, key_code, flags);
#endif // OS_WIN
        Dispatch(&keyev);
    }

    void EventGenerator::UpdateCurrentDispatcher(const gfx::Point& point)
    {
        current_target_ = delegate()->GetTargetAt(point);
    }

    void EventGenerator::PressButton(int flag)
    {
        if (!(flags_ & flag)) {
            flags_ |= flag;
            grab_ = (flags_ & kAllButtonMask) != 0;
            gfx::Point location = GetLocationInCurrentRoot();
            ui::MouseEvent mouseev(ui::ET_MOUSE_PRESSED, location, location,
                ui::EventTimeForNow(), flags_, flag);
            Dispatch(&mouseev);
        }
    }

    void EventGenerator::ReleaseButton(int flag)
    {
        if (flags_ & flag) {
            gfx::Point location = GetLocationInCurrentRoot();
            ui::MouseEvent mouseev(ui::ET_MOUSE_RELEASED, location, location,
                ui::EventTimeForNow(), flags_, flag);
            Dispatch(&mouseev);
            flags_ ^= flag;
        }
        grab_ = (flags_ & kAllButtonMask) != 0;
    }

    gfx::Point EventGenerator::GetLocationInCurrentRoot() const
    {
        gfx::Point p(current_location_);
        delegate()->ConvertPointToTarget(current_target_, &p);
        return p;
    }

    gfx::Point EventGenerator::CenterOfWindow(const EventTarget* window) const
    {
        return delegate()->CenterOfTarget(window);
    }

    void EventGenerator::DoDispatchEvent(ui::Event* event, bool async)
    {
        if (async) {
            ui::Event* pending_event;
            if (event->IsKeyEvent()) {
                pending_event = new ui::KeyEvent(*static_cast<ui::KeyEvent*>(event));
            } else if (event->IsMouseEvent()) {
                pending_event = new ui::MouseEvent(*static_cast<ui::MouseEvent*>(event));
            } else if (event->IsTouchEvent()) {
                pending_event = new ui::TouchEvent(*static_cast<ui::TouchEvent*>(event));
            } else if (event->IsScrollEvent()) {
                pending_event = new ui::ScrollEvent(*static_cast<ui::ScrollEvent*>(event));
            } else {
                NOTREACHED() << "Invalid event type";
                return;
            }
            if (pending_events_.empty()) {
                base::ThreadTaskRunnerHandle::Get()->PostTask(
                    FROM_HERE,
                    base::Bind(&EventGenerator::DispatchNextPendingEvent,
                        base::Unretained(this)));
            }
            pending_events_.push_back(pending_event);
        } else {
            if (event->IsKeyEvent()) {
                delegate()->DispatchKeyEventToIME(current_target_,
                    static_cast<ui::KeyEvent*>(event));
            }
            if (!event->handled()) {
                ui::EventSource* event_source = delegate()->GetEventSource(current_target_);
                ui::EventSourceTestApi event_source_test(event_source);
                ui::EventDispatchDetails details = event_source_test.SendEventToProcessor(event);
                CHECK(!details.dispatcher_destroyed);
            }
        }
    }

    void EventGenerator::DispatchNextPendingEvent()
    {
        DCHECK(!pending_events_.empty());
        ui::Event* event = pending_events_.front();
        DoDispatchEvent(event, false);
        pending_events_.pop_front();
        delete event;
        if (!pending_events_.empty()) {
            base::ThreadTaskRunnerHandle::Get()->PostTask(
                FROM_HERE,
                base::Bind(&EventGenerator::DispatchNextPendingEvent,
                    base::Unretained(this)));
        }
    }

    const EventGeneratorDelegate* EventGenerator::delegate() const
    {
        if (delegate_)
            return delegate_.get();

        DCHECK(default_delegate);
        return default_delegate;
    }

    EventGeneratorDelegate* EventGenerator::delegate()
    {
        return const_cast<EventGeneratorDelegate*>(
            const_cast<const EventGenerator*>(this)->delegate());
    }

} // namespace test
} // namespace ui
