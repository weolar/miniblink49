// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_VIDEO_CAPTURE_VIDEO_CAPTURE_DEVICE_INFO_H_
#define MEDIA_VIDEO_CAPTURE_VIDEO_CAPTURE_DEVICE_INFO_H_

#include "media/base/video_capture_types.h"
#include "media/capture/video/video_capture_device.h"

namespace media {

// A convenience wrap of a device's name and associated supported formats.
struct MEDIA_EXPORT VideoCaptureDeviceInfo {
    VideoCaptureDeviceInfo();
    VideoCaptureDeviceInfo(const VideoCaptureDevice::Name& name,
        const VideoCaptureFormats& supported_formats);
    ~VideoCaptureDeviceInfo();

    VideoCaptureDevice::Name name;
    VideoCaptureFormats supported_formats;
};

typedef std::vector<VideoCaptureDeviceInfo> VideoCaptureDeviceInfos;

} // namespace media

#endif // MEDIA_VIDEO_CAPTURE_VIDEO_CAPTURE_DEVICE_INFO_H_
