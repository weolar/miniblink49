// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UI_EVENTS_TEST_EVENT_GENERATOR_H_
#define UI_EVENTS_TEST_EVENT_GENERATOR_H_

#include <list>
#include <vector>

#include "base/callback.h"
#include "base/macros.h"
#include "base/memory/scoped_ptr.h"
#include "base/time/time.h"
#include "ui/events/event_constants.h"
#include "ui/events/keycodes/keyboard_codes.h"
#include "ui/gfx/geometry/point.h"
#include "ui/gfx/native_widget_types.h"

namespace base {
class TickClock;
}

namespace gfx {
class PointF;
}

namespace ui {
class Event;
class EventProcessor;
class EventSource;
class EventTarget;
class KeyEvent;
class MouseEvent;
class ScrollEvent;
class TouchEvent;

namespace test {

    typedef base::Callback<void(EventType, const gfx::Vector2dF&)>
        ScrollStepCallback;

    class EventGenerator;

    // A delegate interface for EventGenerator to abstract platform-specific event
    // targeting and coordinate conversion.
    class EventGeneratorDelegate {
    public:
        virtual ~EventGeneratorDelegate() { }

        // Set the context of the delegate, whilst it is being used by an active
        // EventGenerator.
        virtual void SetContext(EventGenerator* owner,
            gfx::NativeWindow root_window,
            gfx::NativeWindow window) { }

        // The ui::EventTarget at the given |location|.
        virtual EventTarget* GetTargetAt(const gfx::Point& location) = 0;

        // The ui::EventSource for the given |target|.
        virtual EventSource* GetEventSource(EventTarget* target) = 0;

        // Helper functions to determine the center point of |target| or |window|.
        virtual gfx::Point CenterOfTarget(const EventTarget* target) const = 0;
        virtual gfx::Point CenterOfWindow(gfx::NativeWindow window) const = 0;

        // Convert a point between API's coordinates and |target|'s coordinates.
        virtual void ConvertPointFromTarget(const EventTarget* target,
            gfx::Point* point) const = 0;
        virtual void ConvertPointToTarget(const EventTarget* target,
            gfx::Point* point) const = 0;

        // Convert a point from the coordinate system in the host that contains
        // |hosted_target| into the root window's coordinate system.
        virtual void ConvertPointFromHost(const EventTarget* hosted_target,
            gfx::Point* point) const = 0;

        // Detemines whether the input method should be the first to handle key events
        // before dispathcing to Views. If it does, the given |event| will be
        // dispatched and processed by the input method from the host of |target|.
        virtual void DispatchKeyEventToIME(EventTarget* target,
            ui::KeyEvent* event)
            = 0;
    };

    // ui::test::EventGenerator is a tool that generates and dispatches events.
    // Unlike |ui_controls| package in ui/base/test, this does not use platform
    // native message loops. Instead, it sends events to the event dispatcher
    // synchronously.
    //
    // This class is not suited for the following cases:
    //
    // 1) If your test depends on native events (ui::Event::native_event()).
    //   This return is empty/NULL event with EventGenerator.
    // 2) If your test involves nested message loop, such as
    //    menu or drag & drop. Because this class directly
    //    post an event to WindowEventDispatcher, this event will not be
    //    handled in the nested message loop.
    // 3) Similarly, |base::MessagePumpObserver| will not be invoked.
    // 4) Any other code that requires native message loops, such as
    //    tests for WindowTreeHostWin/WindowTreeHostX11.
    //
    // If one of these applies to your test, please use |ui_controls|
    // package instead.
    //
    // Note: The coordinates of the points in API is determined by the
    // EventGeneratorDelegate.
    class EventGenerator {
    public:
        // Creates an EventGenerator with the mouse/touch location (0,0),
        // which uses the |root_window|'s coordinates and the default delegate for
        // this platform.
        explicit EventGenerator(gfx::NativeWindow root_window);

        // Create an EventGenerator with EventGeneratorDelegate,
        // which uses the coordinates conversions and targeting provided by
        // |delegate|.
        explicit EventGenerator(EventGeneratorDelegate* delegate);

        // Creates an EventGenerator with the mouse/touch location
        // at |initial_location|, which uses the |root_window|'s coordinates.
        EventGenerator(gfx::NativeWindow root_window,
            const gfx::Point& initial_location);

        // Creates an EventGenerator with the mouse/touch location centered over
        // |window|. This is currently the only constructor that works on Mac, since
        // a specific window is required (and there is no root window).
        EventGenerator(gfx::NativeWindow root_window, gfx::NativeWindow window);

        virtual ~EventGenerator();

        // Explicitly sets the location used by mouse/touch events. This is set by the
        // various methods that take a location but can be manipulated directly,
        // typically for touch.
        void set_current_location(const gfx::Point& location)
        {
            current_location_ = location;
        }
        const gfx::Point& current_location() const { return current_location_; }

        void set_async(bool async) { async_ = async; }
        bool async() const { return async_; }

        // Dispatch events through the application instead of directly to the
        // target window. Currently only supported on Mac.
        void set_targeting_application(bool targeting_application)
        {
            targeting_application_ = targeting_application;
        }
        bool targeting_application() const { return targeting_application_; }

        // Resets the event flags bitmask.
        void set_flags(int flags) { flags_ = flags; }
        int flags() const { return flags_; }

        // Generates a left button press event.
        void PressLeftButton();

        // Generates a left button release event.
        void ReleaseLeftButton();

        // Generates events to click (press, release) left button.
        void ClickLeftButton();

        // Generates a double click event using the left button.
        void DoubleClickLeftButton();

        // Generates a right button press event.
        void PressRightButton();

        // Generates a right button release event.
        void ReleaseRightButton();

        // Moves the mouse wheel by |delta_x|, |delta_y|.
        void MoveMouseWheel(int delta_x, int delta_y);

        // Generates a mouse exit.
        void SendMouseExit();

        // Generates events to move mouse to be the given |point| in the
        // |current_root_window_|'s host window coordinates.
        void MoveMouseToInHost(const gfx::Point& point_in_host);
        void MoveMouseToInHost(int x, int y)
        {
            MoveMouseToInHost(gfx::Point(x, y));
        }

        // Generates a mouse move event at the point given in the host
        // coordinates, with a native event with |point_for_natve|.
        void MoveMouseToWithNative(const gfx::Point& point_in_host,
            const gfx::Point& point_for_native);

        // Generates events to move mouse to be the given |point| in screen
        // coordinates.
        void MoveMouseTo(const gfx::Point& point_in_screen, int count);
        void MoveMouseTo(const gfx::Point& point_in_screen)
        {
            MoveMouseTo(point_in_screen, 1);
        }
        void MoveMouseTo(int x, int y)
        {
            MoveMouseTo(gfx::Point(x, y));
        }

        // Generates events to move mouse to be the given |point| in |window|'s
        // coordinates.
        void MoveMouseRelativeTo(const EventTarget* window, const gfx::Point& point);
        void MoveMouseRelativeTo(const EventTarget* window, int x, int y)
        {
            MoveMouseRelativeTo(window, gfx::Point(x, y));
        }

        void MoveMouseBy(int x, int y)
        {
            MoveMouseTo(current_location_ + gfx::Vector2d(x, y));
        }

        // Generates events to drag mouse to given |point|.
        void DragMouseTo(const gfx::Point& point);

        void DragMouseTo(int x, int y)
        {
            DragMouseTo(gfx::Point(x, y));
        }

        void DragMouseBy(int dx, int dy)
        {
            DragMouseTo(current_location_ + gfx::Vector2d(dx, dy));
        }

        // Generates events to move the mouse to the center of the window.
        void MoveMouseToCenterOf(EventTarget* window);

        // Generates a touch press event.
        void PressTouch();

        // Generates a touch press event with |touch_id|.
        void PressTouchId(int touch_id);

        // Generates a ET_TOUCH_MOVED event to |point|.
        void MoveTouch(const gfx::Point& point);

        // Generates a ET_TOUCH_MOVED event to |point| with |touch_id|.
        void MoveTouchId(const gfx::Point& point, int touch_id);

        // Generates a touch release event.
        void ReleaseTouch();

        // Generates a touch release event with |touch_id|.
        void ReleaseTouchId(int touch_id);

        // Generates press, move and release event to move touch
        // to be the given |point|.
        void PressMoveAndReleaseTouchTo(const gfx::Point& point);

        void PressMoveAndReleaseTouchTo(int x, int y)
        {
            PressMoveAndReleaseTouchTo(gfx::Point(x, y));
        }

        void PressMoveAndReleaseTouchBy(int x, int y)
        {
            PressMoveAndReleaseTouchTo(current_location_ + gfx::Vector2d(x, y));
        }

        // Generates press, move and release events to move touch
        // to the center of the window.
        void PressMoveAndReleaseTouchToCenterOf(EventTarget* window);

        // Generates and dispatches a Win8 edge-swipe event (swipe up from bottom or
        // swipe down from top).  Note that it is not possible to distinguish between
        // the two edges with this event.
        void GestureEdgeSwipe();

        // Generates and dispatches touch-events required to generate a TAP gesture.
        // Note that this can generate a number of other gesture events at the same
        // time (e.g. GESTURE_BEGIN, TAP_DOWN, END).
        void GestureTapAt(const gfx::Point& point);

        // Generates press and release touch-events to generate a TAP_DOWN event, but
        // without generating any scroll or tap events. This can also generate a few
        // other gesture events (e.g. GESTURE_BEGIN, END).
        void GestureTapDownAndUp(const gfx::Point& point);

        // Calculates a time duration that can be used with the given |start|, |end|,
        // and |steps| values when calling GestureScrollSequence (or
        // GestureScrollSequenceWithCallback) to achieve the given |velocity|.
        base::TimeDelta CalculateScrollDurationForFlingVelocity(
            const gfx::Point& start,
            const gfx::Point& end,
            float velocity,
            int steps);

        // Generates press, move, release touch-events to generate a sequence of
        // scroll events. |duration| and |steps| affect the velocity of the scroll,
        // and depending on these values, this may also generate FLING scroll
        // gestures. If velocity/fling is irrelevant for the test, then any non-zero
        // values for these should be sufficient.
        void GestureScrollSequence(const gfx::Point& start,
            const gfx::Point& end,
            const base::TimeDelta& duration,
            int steps);

        // The same as GestureScrollSequence(), with the exception that |callback| is
        // called at each step of the scroll sequence. |callback| is called at the
        // start of the sequence with ET_GESTURE_SCROLL_BEGIN, followed by one or more
        // ET_GESTURE_SCROLL_UPDATE and ends with an ET_GESTURE_SCROLL_END.
        void GestureScrollSequenceWithCallback(const gfx::Point& start,
            const gfx::Point& end,
            const base::TimeDelta& duration,
            int steps,
            const ScrollStepCallback& callback);

        // Generates press, move, release touch-events to generate a sequence of
        // multi-finger scroll events. |count| specifies the number of touch-points
        // that should generate the scroll events. |start| are the starting positions
        // of all the touch points. |steps| and |event_separation_time_ms| are
        // relevant when testing velocity/fling/swipe, otherwise these can be any
        // non-zero value. |delta_x| and |delta_y| are the amount that each finger
        // should be moved. Internally calls GestureMultiFingerScrollWithDelays
        // with zeros as |delay_adding_finger_ms| forcing all touch down events to be
        // immediate.
        void GestureMultiFingerScroll(int count,
            const gfx::Point start[],
            int event_separation_time_ms,
            int steps,
            int move_x,
            int move_y);

        // Generates press, move, release touch-events to generate a sequence of
        // multi-finger scroll events. |count| specifies the number of touch-points
        // that should generate the scroll events. |start| are the starting positions
        // of all the touch points. |delay_adding_finger_ms| are delays in ms from the
        // starting time till touching down of each finger. |delay_adding_finger_ms|
        // is useful when testing complex gestures that start with 1 or 2 fingers and
        // add fingers with a delay. |steps| and |event_separation_time_ms| are
        // relevant when testing velocity/fling/swipe, otherwise these can be any
        // non-zero value. |delta_x| and |delta_y| are the amount that each finger
        // should be moved.
        void GestureMultiFingerScrollWithDelays(int count,
            const gfx::Point start[],
            const int delay_adding_finger_ms[],
            int event_separation_time_ms,
            int steps,
            int move_x,
            int move_y);

        // Generates scroll sequences of a FlingCancel, Scrolls, FlingStart, with
        // constant deltas to |x_offset| and |y_offset| in |steps|.
        void ScrollSequence(const gfx::Point& start,
            const base::TimeDelta& step_delay,
            float x_offset,
            float y_offset,
            int steps,
            int num_fingers);

        // Generates scroll sequences of a FlingCancel, Scrolls, FlingStart, sending
        // scrolls of each of the values in |offsets|.
        void ScrollSequence(const gfx::Point& start,
            const base::TimeDelta& step_delay,
            const std::vector<gfx::PointF>& offsets,
            int num_fingers);

        // Generates a key press event. On platforms except Windows and X11, a key
        // event without native_event() is generated. Note that ui::EF_ flags should
        // be passed as |flags|, not the native ones like 'ShiftMask' in <X11/X.h>.
        // TODO(yusukes): Support native_event() on all platforms.
        void PressKey(KeyboardCode key_code, int flags);

        // Generates a key release event. On platforms except Windows and X11, a key
        // event without native_event() is generated. Note that ui::EF_ flags should
        // be passed as |flags|, not the native ones like 'ShiftMask' in <X11/X.h>.
        // TODO(yusukes): Support native_event() on all platforms.
        void ReleaseKey(KeyboardCode key_code, int flags);

        // Dispatch the event to the WindowEventDispatcher.
        void Dispatch(Event* event);

        void set_current_target(EventTarget* target)
        {
            current_target_ = target;
        }

        // Specify an alternative tick clock to be used for simulating time in tests.
        void SetTickClock(scoped_ptr<base::TickClock> tick_clock);

        // Get the current time from the tick clock.
        base::TimeDelta Now();

        // Default delegate set by a platform-specific GeneratorDelegate singleton.
        static EventGeneratorDelegate* default_delegate;

    private:
        // Set up the test context using the delegate.
        void Init(gfx::NativeWindow root_window, gfx::NativeWindow window_context);

        // Dispatch a key event to the WindowEventDispatcher.
        void DispatchKeyEvent(bool is_press, KeyboardCode key_code, int flags);

        void UpdateCurrentDispatcher(const gfx::Point& point);
        void PressButton(int flag);
        void ReleaseButton(int flag);

        gfx::Point GetLocationInCurrentRoot() const;
        gfx::Point CenterOfWindow(const EventTarget* window) const;

        void DispatchNextPendingEvent();
        void DoDispatchEvent(Event* event, bool async);

        const EventGeneratorDelegate* delegate() const;
        EventGeneratorDelegate* delegate();

        scoped_ptr<EventGeneratorDelegate> delegate_;
        gfx::Point current_location_;
        EventTarget* current_target_;
        int flags_;
        bool grab_;
        std::list<Event*> pending_events_;
        // Set to true to cause events to be posted asynchronously.
        bool async_;
        bool targeting_application_;
        scoped_ptr<base::TickClock> tick_clock_;

        DISALLOW_COPY_AND_ASSIGN(EventGenerator);
    };

} // namespace test
} // namespace ui

#endif // UI_EVENTS_TEST_EVENT_GENERATOR_H_
