// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/capture/video/video_capture_device_info.h"

namespace media {

VideoCaptureDeviceInfo::VideoCaptureDeviceInfo()
{
}

VideoCaptureDeviceInfo::VideoCaptureDeviceInfo(
    const VideoCaptureDevice::Name& name,
    const VideoCaptureFormats& supported_formats)
    : name(name)
    , supported_formats(supported_formats)
{
}

VideoCaptureDeviceInfo::~VideoCaptureDeviceInfo()
{
}

} // namespace media
