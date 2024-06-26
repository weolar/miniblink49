// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/events/ozone/evdev/event_thread_evdev.h"

#include <utility>

#include "base/bind.h"
#include "base/callback.h"
#include "base/logging.h"
#include "base/thread_task_runner_handle.h"
#include "base/threading/thread.h"
#include "base/trace_event/trace_event.h"
#include "ui/events/ozone/evdev/device_event_dispatcher_evdev.h"
#include "ui/events/ozone/evdev/input_device_factory_evdev.h"
#include "ui/events/ozone/evdev/input_device_factory_evdev_proxy.h"

namespace ui {

namespace {

    // Internal base::Thread subclass for events thread.
    class EvdevThread : public base::Thread {
    public:
        EvdevThread(scoped_ptr<DeviceEventDispatcherEvdev> dispatcher,
            CursorDelegateEvdev* cursor,
            const EventThreadStartCallback& callback)
            : base::Thread("evdev")
            , dispatcher_(std::move(dispatcher))
            , cursor_(cursor)
            , init_callback_(callback)
            , init_runner_(base::ThreadTaskRunnerHandle::Get())
        {
        }
        ~EvdevThread() override { Stop(); }

        void Init() override
        {
            TRACE_EVENT0("evdev", "EvdevThread::Init");
            input_device_factory_ = new InputDeviceFactoryEvdev(std::move(dispatcher_), cursor_);

            scoped_ptr<InputDeviceFactoryEvdevProxy> proxy(
                new InputDeviceFactoryEvdevProxy(base::ThreadTaskRunnerHandle::Get(),
                    input_device_factory_->GetWeakPtr()));

            init_runner_->PostTask(FROM_HERE,
                base::Bind(init_callback_, base::Passed(&proxy)));
        }

        void CleanUp() override
        {
            TRACE_EVENT0("evdev", "EvdevThread::CleanUp");
            delete input_device_factory_;
        }

    private:
        // Initialization bits passed from main thread.
        scoped_ptr<DeviceEventDispatcherEvdev> dispatcher_;
        CursorDelegateEvdev* cursor_;
        EventThreadStartCallback init_callback_;
        scoped_refptr<base::SingleThreadTaskRunner> init_runner_;

        // Thread-internal state.
        InputDeviceFactoryEvdev* input_device_factory_ = nullptr;
    };

} // namespace

EventThreadEvdev::EventThreadEvdev()
{
}

EventThreadEvdev::~EventThreadEvdev()
{
}

void EventThreadEvdev::Start(scoped_ptr<DeviceEventDispatcherEvdev> dispatcher,
    CursorDelegateEvdev* cursor,
    const EventThreadStartCallback& callback)
{
    TRACE_EVENT0("evdev", "EventThreadEvdev::Start");
    thread_.reset(new EvdevThread(std::move(dispatcher), cursor, callback));
    if (!thread_->StartWithOptions(
            base::Thread::Options(base::MessageLoop::TYPE_UI, 0)))
        LOG(FATAL) << "Failed to create input thread";
}

} // namespace ui
