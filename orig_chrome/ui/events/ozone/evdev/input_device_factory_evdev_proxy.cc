// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/events/ozone/evdev/input_device_factory_evdev_proxy.h"

#include "base/bind.h"
#include "base/thread_task_runner_handle.h"
#include "ui/events/ozone/evdev/input_device_factory_evdev.h"

namespace ui {

namespace {

    void ForwardGetTouchDeviceStatusReply(
        scoped_refptr<base::SingleThreadTaskRunner> reply_runner,
        const GetTouchDeviceStatusReply& reply,
        scoped_ptr<std::string> status)
    {
        // Thread hop back to UI for reply.
        reply_runner->PostTask(FROM_HERE, base::Bind(reply, base::Passed(&status)));
    }

    void ForwardGetTouchEventLogReply(
        scoped_refptr<base::SingleThreadTaskRunner> reply_runner,
        const GetTouchEventLogReply& reply,
        scoped_ptr<std::vector<base::FilePath>> log_paths)
    {
        // Thread hop back to UI for reply.
        reply_runner->PostTask(FROM_HERE,
            base::Bind(reply, base::Passed(&log_paths)));
    }

} // namespace

InputDeviceFactoryEvdevProxy::InputDeviceFactoryEvdevProxy(
    scoped_refptr<base::SingleThreadTaskRunner> task_runner,
    base::WeakPtr<InputDeviceFactoryEvdev> input_device_factory)
    : task_runner_(task_runner)
    , input_device_factory_(input_device_factory)
{
}

InputDeviceFactoryEvdevProxy::~InputDeviceFactoryEvdevProxy()
{
}

void InputDeviceFactoryEvdevProxy::AddInputDevice(int id,
    const base::FilePath& path)
{
    task_runner_->PostTask(FROM_HERE,
        base::Bind(&InputDeviceFactoryEvdev::AddInputDevice,
            input_device_factory_, id, path));
}

void InputDeviceFactoryEvdevProxy::RemoveInputDevice(
    const base::FilePath& path)
{
    task_runner_->PostTask(FROM_HERE,
        base::Bind(&InputDeviceFactoryEvdev::RemoveInputDevice,
            input_device_factory_, path));
}

void InputDeviceFactoryEvdevProxy::OnStartupScanComplete()
{
    task_runner_->PostTask(
        FROM_HERE, base::Bind(&InputDeviceFactoryEvdev::OnStartupScanComplete, input_device_factory_));
}

void InputDeviceFactoryEvdevProxy::SetCapsLockLed(bool enabled)
{
    task_runner_->PostTask(FROM_HERE,
        base::Bind(&InputDeviceFactoryEvdev::SetCapsLockLed,
            input_device_factory_, enabled));
}

void InputDeviceFactoryEvdevProxy::UpdateInputDeviceSettings(
    const InputDeviceSettingsEvdev& settings)
{
    task_runner_->PostTask(
        FROM_HERE, base::Bind(&InputDeviceFactoryEvdev::UpdateInputDeviceSettings, input_device_factory_, settings));
}

void InputDeviceFactoryEvdevProxy::GetTouchDeviceStatus(
    const GetTouchDeviceStatusReply& reply)
{
    task_runner_->PostTask(
        FROM_HERE,
        base::Bind(&InputDeviceFactoryEvdev::GetTouchDeviceStatus,
            input_device_factory_,
            base::Bind(&ForwardGetTouchDeviceStatusReply,
                base::ThreadTaskRunnerHandle::Get(), reply)));
}

void InputDeviceFactoryEvdevProxy::GetTouchEventLog(
    const base::FilePath& out_dir,
    const GetTouchEventLogReply& reply)
{
    task_runner_->PostTask(
        FROM_HERE,
        base::Bind(&InputDeviceFactoryEvdev::GetTouchEventLog,
            input_device_factory_, out_dir,
            base::Bind(&ForwardGetTouchEventLogReply,
                base::ThreadTaskRunnerHandle::Get(), reply)));
}

} // namespace ui
