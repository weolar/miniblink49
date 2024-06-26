// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/events/ozone/device/device_event.h"

namespace ui {

DeviceEvent::DeviceEvent(DeviceType type,
    ActionType action,
    const base::FilePath& path)
    : device_type_(type)
    , action_type_(action)
    , path_(path)
{
}

} // namespace ui
