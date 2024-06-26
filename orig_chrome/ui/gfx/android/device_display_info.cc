// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/gfx/android/device_display_info.h"

#include "base/logging.h"
#include "ui/gfx/android/shared_device_display_info.h"

namespace gfx {

DeviceDisplayInfo::DeviceDisplayInfo()
{
}

DeviceDisplayInfo::~DeviceDisplayInfo()
{
}

int DeviceDisplayInfo::GetDisplayHeight() const
{
    return SharedDeviceDisplayInfo::GetInstance()->GetDisplayHeight();
}

int DeviceDisplayInfo::GetDisplayWidth() const
{
    return SharedDeviceDisplayInfo::GetInstance()->GetDisplayWidth();
}

int DeviceDisplayInfo::GetPhysicalDisplayHeight() const
{
    return SharedDeviceDisplayInfo::GetInstance()->GetPhysicalDisplayHeight();
}

int DeviceDisplayInfo::GetPhysicalDisplayWidth() const
{
    return SharedDeviceDisplayInfo::GetInstance()->GetPhysicalDisplayWidth();
}

int DeviceDisplayInfo::GetBitsPerPixel() const
{
    return SharedDeviceDisplayInfo::GetInstance()->GetBitsPerPixel();
}

int DeviceDisplayInfo::GetBitsPerComponent() const
{
    return SharedDeviceDisplayInfo::GetInstance()->GetBitsPerComponent();
}

double DeviceDisplayInfo::GetDIPScale() const
{
    return SharedDeviceDisplayInfo::GetInstance()->GetDIPScale();
}

int DeviceDisplayInfo::GetSmallestDIPWidth() const
{
    return SharedDeviceDisplayInfo::GetInstance()->GetSmallestDIPWidth();
}

int DeviceDisplayInfo::GetRotationDegrees() const
{
    return SharedDeviceDisplayInfo::GetInstance()->GetRotationDegrees();
}

} // namespace gfx
