// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Windows specific implementation of VideoCaptureDevice.
// DirectShow is used for capturing. DirectShow provide its own threads
// for capturing.

#ifndef MEDIA_VIDEO_CAPTURE_WIN_CAPABILITY_LIST_WIN_H_
#define MEDIA_VIDEO_CAPTURE_WIN_CAPABILITY_LIST_WIN_H_

#include <list>
#include <windows.h>

#include "media/base/video_capture_types.h"

namespace media {

struct CapabilityWin {
    CapabilityWin(int index, const VideoCaptureFormat& format)
        : stream_index(index)
        , supported_format(format)
        , info_header()
    {
    }

    // Used by VideoCaptureDeviceWin.
    CapabilityWin(int index,
        const VideoCaptureFormat& format,
        const BITMAPINFOHEADER& info_header)
        : stream_index(index)
        , supported_format(format)
        , info_header(info_header)
    {
    }

    const int stream_index;
    const VideoCaptureFormat supported_format;

    // |info_header| is only valid if DirectShow is used.
    const BITMAPINFOHEADER info_header;
};

typedef std::list<CapabilityWin> CapabilityList;

const CapabilityWin& GetBestMatchedCapability(
    const VideoCaptureFormat& requested,
    const CapabilityList& capabilities);

} // namespace media

#endif // MEDIA_VIDEO_CAPTURE_WIN_CAPABILITY_LIST_WIN_H_
