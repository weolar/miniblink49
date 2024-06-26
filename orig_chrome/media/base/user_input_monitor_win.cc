// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/base/user_input_monitor.h"

#include "base/bind.h"
#include "base/location.h"
#include "base/logging.h"
#include "base/message_loop/message_loop.h"
#include "base/single_thread_task_runner.h"
#include "base/strings/stringprintf.h"
#include "base/synchronization/lock.h"
#include "base/win/message_window.h"
#include "media/base/keyboard_event_counter.h"
#include "third_party/skia/include/core/SkPoint.h"
#include "ui/events/keycodes/keyboard_code_conversion_win.h"

namespace media {
namespace {

    // From the HID Usage Tables specification.
    const USHORT kGenericDesktopPage = 1;
    const USHORT kMouseUsage = 2;
    const USHORT kKeyboardUsage = 6;

    // This is the actual implementation of event monitoring. It's separated from
    // UserInputMonitorWin since it needs to be deleted on the UI thread.
    class UserInputMonitorWinCore
        : public base::SupportsWeakPtr<UserInputMonitorWinCore>,
          public base::MessageLoop::DestructionObserver {
    public:
        enum EventBitMask {
            MOUSE_EVENT_MASK = 1,
            KEYBOARD_EVENT_MASK = 2,
        };

        explicit UserInputMonitorWinCore(
            scoped_refptr<base::SingleThreadTaskRunner> ui_task_runner,
            const scoped_refptr<UserInputMonitor::MouseListenerList>&
                mouse_listeners);
        ~UserInputMonitorWinCore() override;

        // DestructionObserver overrides.
        void WillDestroyCurrentMessageLoop() override;

        size_t GetKeyPressCount() const;
        void StartMonitor(EventBitMask type);
        void StopMonitor(EventBitMask type);

    private:
        // Handles WM_INPUT messages.
        LRESULT OnInput(HRAWINPUT input_handle);
        // Handles messages received by |window_|.
        bool HandleMessage(UINT message,
            WPARAM wparam,
            LPARAM lparam,
            LRESULT* result);
        RAWINPUTDEVICE* GetRawInputDevices(EventBitMask event, DWORD flags);

        // Task runner on which |window_| is created.
        scoped_refptr<base::SingleThreadTaskRunner> ui_task_runner_;
        scoped_refptr<base::ObserverListThreadSafe<
            UserInputMonitor::MouseEventListener>>
            mouse_listeners_;

        // These members are only accessed on the UI thread.
        scoped_ptr<base::win::MessageWindow> window_;
        uint8 events_monitored_;
        KeyboardEventCounter counter_;

        DISALLOW_COPY_AND_ASSIGN(UserInputMonitorWinCore);
    };

    class UserInputMonitorWin : public UserInputMonitor {
    public:
        explicit UserInputMonitorWin(
            const scoped_refptr<base::SingleThreadTaskRunner>& ui_task_runner);
        ~UserInputMonitorWin() override;

        // Public UserInputMonitor overrides.
        size_t GetKeyPressCount() const override;

    private:
        // Private UserInputMonitor overrides.
        void StartKeyboardMonitoring() override;
        void StopKeyboardMonitoring() override;
        void StartMouseMonitoring() override;
        void StopMouseMonitoring() override;

        scoped_refptr<base::SingleThreadTaskRunner> ui_task_runner_;
        UserInputMonitorWinCore* core_;

        DISALLOW_COPY_AND_ASSIGN(UserInputMonitorWin);
    };

    UserInputMonitorWinCore::UserInputMonitorWinCore(
        scoped_refptr<base::SingleThreadTaskRunner> ui_task_runner,
        const scoped_refptr<UserInputMonitor::MouseListenerList>& mouse_listeners)
        : ui_task_runner_(ui_task_runner)
        , mouse_listeners_(mouse_listeners)
        , events_monitored_(0)
    {
    }

    UserInputMonitorWinCore::~UserInputMonitorWinCore()
    {
        DCHECK(!window_);
        DCHECK(!events_monitored_);
    }

    void UserInputMonitorWinCore::WillDestroyCurrentMessageLoop()
    {
        DCHECK(ui_task_runner_->BelongsToCurrentThread());
        StopMonitor(MOUSE_EVENT_MASK);
        StopMonitor(KEYBOARD_EVENT_MASK);
    }

    size_t UserInputMonitorWinCore::GetKeyPressCount() const
    {
        return counter_.GetKeyPressCount();
    }

    void UserInputMonitorWinCore::StartMonitor(EventBitMask type)
    {
        DCHECK(ui_task_runner_->BelongsToCurrentThread());

        if (events_monitored_ & type)
            return;

        if (type == KEYBOARD_EVENT_MASK)
            counter_.Reset();

        if (!window_) {
            window_.reset(new base::win::MessageWindow());
            if (!window_->Create(base::Bind(&UserInputMonitorWinCore::HandleMessage,
                    base::Unretained(this)))) {
                PLOG(ERROR) << "Failed to create the raw input window";
                window_.reset();
                return;
            }
        }

        // Register to receive raw mouse and/or keyboard input.
        scoped_ptr<RAWINPUTDEVICE> device(GetRawInputDevices(type, RIDEV_INPUTSINK));
        if (!RegisterRawInputDevices(device.get(), 1, sizeof(*device))) {
            PLOG(ERROR) << "RegisterRawInputDevices() failed for RIDEV_INPUTSINK";
            window_.reset();
            return;
        }

        // Start observing message loop destruction if we start monitoring the first
        // event.
        if (!events_monitored_)
            base::MessageLoop::current()->AddDestructionObserver(this);

        events_monitored_ |= type;
    }

    void UserInputMonitorWinCore::StopMonitor(EventBitMask type)
    {
        DCHECK(ui_task_runner_->BelongsToCurrentThread());

        if (!(events_monitored_ & type))
            return;

        // Stop receiving raw input.
        DCHECK(window_);
        scoped_ptr<RAWINPUTDEVICE> device(GetRawInputDevices(type, RIDEV_REMOVE));

        if (!RegisterRawInputDevices(device.get(), 1, sizeof(*device))) {
            PLOG(INFO) << "RegisterRawInputDevices() failed for RIDEV_REMOVE";
        }

        events_monitored_ &= ~type;
        if (events_monitored_ == 0) {
            window_.reset();

            // Stop observing message loop destruction if no event is being monitored.
            base::MessageLoop::current()->RemoveDestructionObserver(this);
        }
    }

    LRESULT UserInputMonitorWinCore::OnInput(HRAWINPUT input_handle)
    {
        DCHECK(ui_task_runner_->BelongsToCurrentThread());

        // Get the size of the input record.
        UINT size = 0;
        UINT result = GetRawInputData(
            input_handle, RID_INPUT, NULL, &size, sizeof(RAWINPUTHEADER));
        if (result == -1) {
            PLOG(ERROR) << "GetRawInputData() failed";
            return 0;
        }
        DCHECK_EQ(0u, result);

        // Retrieve the input record itself.
        scoped_ptr<uint8[]> buffer(new uint8[size]);
        RAWINPUT* input = reinterpret_cast<RAWINPUT*>(buffer.get());
        result = GetRawInputData(
            input_handle, RID_INPUT, buffer.get(), &size, sizeof(RAWINPUTHEADER));
        if (result == -1) {
            PLOG(ERROR) << "GetRawInputData() failed";
            return 0;
        }
        DCHECK_EQ(size, result);

        // Notify the observer about events generated locally.
        if (input->header.dwType == RIM_TYPEMOUSE && input->header.hDevice != NULL) {
            POINT position;
            if (!GetCursorPos(&position)) {
                position.x = 0;
                position.y = 0;
            }
            mouse_listeners_->Notify(
                FROM_HERE, &UserInputMonitor::MouseEventListener::OnMouseMoved,
                SkIPoint::Make(position.x, position.y));
        } else if (input->header.dwType == RIM_TYPEKEYBOARD && input->header.hDevice != NULL) {
            ui::EventType event = (input->data.keyboard.Flags & RI_KEY_BREAK)
                ? ui::ET_KEY_RELEASED
                : ui::ET_KEY_PRESSED;
            ui::KeyboardCode key_code = ui::KeyboardCodeForWindowsKeyCode(input->data.keyboard.VKey);
            counter_.OnKeyboardEvent(event, key_code);
        }

        return DefRawInputProc(&input, 1, sizeof(RAWINPUTHEADER));
    }

    bool UserInputMonitorWinCore::HandleMessage(UINT message,
        WPARAM wparam,
        LPARAM lparam,
        LRESULT* result)
    {
        DCHECK(ui_task_runner_->BelongsToCurrentThread());

        switch (message) {
        case WM_INPUT:
            *result = OnInput(reinterpret_cast<HRAWINPUT>(lparam));
            return true;

        default:
            return false;
        }
    }

    RAWINPUTDEVICE* UserInputMonitorWinCore::GetRawInputDevices(EventBitMask event,
        DWORD flags)
    {
        DCHECK(ui_task_runner_->BelongsToCurrentThread());

        scoped_ptr<RAWINPUTDEVICE> device(new RAWINPUTDEVICE());
        if (event == MOUSE_EVENT_MASK) {
            device->dwFlags = flags;
            device->usUsagePage = kGenericDesktopPage;
            device->usUsage = kMouseUsage;
            device->hwndTarget = window_->hwnd();
        } else {
            DCHECK_EQ(KEYBOARD_EVENT_MASK, event);
            device->dwFlags = flags;
            device->usUsagePage = kGenericDesktopPage;
            device->usUsage = kKeyboardUsage;
            device->hwndTarget = window_->hwnd();
        }
        return device.release();
    }

    //
    // Implementation of UserInputMonitorWin.
    //

    UserInputMonitorWin::UserInputMonitorWin(
        const scoped_refptr<base::SingleThreadTaskRunner>& ui_task_runner)
        : ui_task_runner_(ui_task_runner)
        , core_(new UserInputMonitorWinCore(ui_task_runner, mouse_listeners()))
    {
    }

    UserInputMonitorWin::~UserInputMonitorWin()
    {
        if (!ui_task_runner_->DeleteSoon(FROM_HERE, core_))
            delete core_;
    }

    size_t UserInputMonitorWin::GetKeyPressCount() const
    {
        return core_->GetKeyPressCount();
    }

    void UserInputMonitorWin::StartKeyboardMonitoring()
    {
        ui_task_runner_->PostTask(
            FROM_HERE,
            base::Bind(&UserInputMonitorWinCore::StartMonitor,
                core_->AsWeakPtr(),
                UserInputMonitorWinCore::KEYBOARD_EVENT_MASK));
    }

    void UserInputMonitorWin::StopKeyboardMonitoring()
    {
        ui_task_runner_->PostTask(
            FROM_HERE,
            base::Bind(&UserInputMonitorWinCore::StopMonitor,
                core_->AsWeakPtr(),
                UserInputMonitorWinCore::KEYBOARD_EVENT_MASK));
    }

    void UserInputMonitorWin::StartMouseMonitoring()
    {
        ui_task_runner_->PostTask(
            FROM_HERE,
            base::Bind(&UserInputMonitorWinCore::StartMonitor,
                core_->AsWeakPtr(),
                UserInputMonitorWinCore::MOUSE_EVENT_MASK));
    }

    void UserInputMonitorWin::StopMouseMonitoring()
    {
        ui_task_runner_->PostTask(
            FROM_HERE,
            base::Bind(&UserInputMonitorWinCore::StopMonitor,
                core_->AsWeakPtr(),
                UserInputMonitorWinCore::MOUSE_EVENT_MASK));
    }

} // namespace

scoped_ptr<UserInputMonitor> UserInputMonitor::Create(
    const scoped_refptr<base::SingleThreadTaskRunner>& io_task_runner,
    const scoped_refptr<base::SingleThreadTaskRunner>& ui_task_runner)
{
    return scoped_ptr<UserInputMonitor>(new UserInputMonitorWin(ui_task_runner));
}

} // namespace media
