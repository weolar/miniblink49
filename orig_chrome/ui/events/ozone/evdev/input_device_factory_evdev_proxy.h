// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UI_EVENTS_OZONE_EVDEV_INPUT_DEVICE_FACTORY_PROXY_EVDEV_H_
#define UI_EVENTS_OZONE_EVDEV_INPUT_DEVICE_FACTORY_PROXY_EVDEV_H_

#include <set>
#include <vector>

#include "base/compiler_specific.h"
#include "base/files/file_path.h"
#include "base/macros.h"
#include "base/memory/ref_counted.h"
#include "base/memory/weak_ptr.h"
#include "base/single_thread_task_runner.h"
#include "ui/events/ozone/evdev/events_ozone_evdev_export.h"

namespace ui {

enum class DomCode;
class InputDeviceFactoryEvdev;
struct InputDeviceSettingsEvdev;

typedef base::Callback<void(scoped_ptr<std::string>)> GetTouchDeviceStatusReply;
typedef base::Callback<void(scoped_ptr<std::vector<base::FilePath>>)>
    GetTouchEventLogReply;

// Thread safe proxy for InputDeviceFactoryEvdev.
//
// This is used on the UI thread to proxy calls to the real object on
// the device I/O thread.
class EVENTS_OZONE_EVDEV_EXPORT InputDeviceFactoryEvdevProxy {
public:
    InputDeviceFactoryEvdevProxy(
        scoped_refptr<base::SingleThreadTaskRunner> task_runner,
        base::WeakPtr<InputDeviceFactoryEvdev> input_device_factory);
    ~InputDeviceFactoryEvdevProxy();

    // See InputDeviceFactoryEvdev for docs. These calls simply forward to
    // that object on another thread.
    void AddInputDevice(int id, const base::FilePath& path);
    void RemoveInputDevice(const base::FilePath& path);
    void OnStartupScanComplete();
    void SetCapsLockLed(bool enabled);
    void SetTouchEventLoggingEnabled(bool enabled);
    void UpdateInputDeviceSettings(const InputDeviceSettingsEvdev& settings);
    void GetTouchDeviceStatus(const GetTouchDeviceStatusReply& reply);
    void GetTouchEventLog(const base::FilePath& out_dir,
        const GetTouchEventLogReply& reply);

private:
    scoped_refptr<base::SingleThreadTaskRunner> task_runner_;
    base::WeakPtr<InputDeviceFactoryEvdev> input_device_factory_;

    DISALLOW_COPY_AND_ASSIGN(InputDeviceFactoryEvdevProxy);
};

} // namespace ui

#endif // UI_EVENTS_OZONE_EVDEV_INPUT_DEVICE_FACTORY_PROXY_EVDEV_H_
