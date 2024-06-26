// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Implementation of a VideoCaptureDeviceFactory class for Windows platforms.

#ifndef MEDIA_VIDEO_CAPTURE_VIDEO_CAPTURE_DEVICE_FACTORY_WIN_H_
#define MEDIA_VIDEO_CAPTURE_VIDEO_CAPTURE_DEVICE_FACTORY_WIN_H_

#include "media/capture/video/video_capture_device_factory.h"

namespace media {

// Extension of VideoCaptureDeviceFactory to create and manipulate Windows
// devices, via either DirectShow or MediaFoundation APIs.
class MEDIA_EXPORT VideoCaptureDeviceFactoryWin
    : public VideoCaptureDeviceFactory {
public:
    static bool PlatformSupportsMediaFoundation();

    VideoCaptureDeviceFactoryWin();
    ~VideoCaptureDeviceFactoryWin() override { }

    scoped_ptr<VideoCaptureDevice> Create(
        const VideoCaptureDevice::Name& device_name) override;
    void GetDeviceNames(VideoCaptureDevice::Names* device_names) override;
    void GetDeviceSupportedFormats(
        const VideoCaptureDevice::Name& device,
        VideoCaptureFormats* supported_formats) override;

private:
    bool use_media_foundation_;

    DISALLOW_COPY_AND_ASSIGN(VideoCaptureDeviceFactoryWin);
};

} // namespace media

#endif // MEDIA_VIDEO_CAPTURE_VIDEO_CAPTURE_DEVICE_FACTORY_WIN_H_
