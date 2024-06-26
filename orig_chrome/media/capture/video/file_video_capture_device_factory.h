// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_VIDEO_CAPTURE_FILE_VIDEO_CAPTURE_DEVICE_FACTORY_H_
#define MEDIA_VIDEO_CAPTURE_FILE_VIDEO_CAPTURE_DEVICE_FACTORY_H_

#include "media/capture/video/video_capture_device_factory.h"

namespace media {

// Extension of VideoCaptureDeviceFactory to create and manipulate file-backed
// fake devices. These devices play back video-only files as video capture
// input.
class MEDIA_EXPORT FileVideoCaptureDeviceFactory
    : public VideoCaptureDeviceFactory {
public:
    FileVideoCaptureDeviceFactory() { }
    ~FileVideoCaptureDeviceFactory() override { }

    scoped_ptr<VideoCaptureDevice> Create(
        const VideoCaptureDevice::Name& device_name) override;
    void GetDeviceNames(VideoCaptureDevice::Names* device_names) override;
    void GetDeviceSupportedFormats(
        const VideoCaptureDevice::Name& device,
        VideoCaptureFormats* supported_formats) override;
};

} // namespace media

#endif // MEDIA_VIDEO_CAPTURE_FILE_VIDEO_CAPTURE_DEVICE_FACTORY_H_
