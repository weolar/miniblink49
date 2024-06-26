// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/base/video_types.h"

#include "base/logging.h"

namespace media {

std::string VideoPixelFormatToString(VideoPixelFormat format)
{
    switch (format) {
    case PIXEL_FORMAT_UNKNOWN:
        return "PIXEL_FORMAT_UNKNOWN";
    case PIXEL_FORMAT_I420:
        return "PIXEL_FORMAT_I420";
    case PIXEL_FORMAT_YV12:
        return "PIXEL_FORMAT_YV12";
    case PIXEL_FORMAT_YV16:
        return "PIXEL_FORMAT_YV16";
    case PIXEL_FORMAT_YV12A:
        return "PIXEL_FORMAT_YV12A";
    case PIXEL_FORMAT_YV24:
        return "PIXEL_FORMAT_YV24";
    case PIXEL_FORMAT_NV12:
        return "PIXEL_FORMAT_NV12";
    case PIXEL_FORMAT_NV21:
        return "PIXEL_FORMAT_NV21";
    case PIXEL_FORMAT_UYVY:
        return "PIXEL_FORMAT_UYVY";
    case PIXEL_FORMAT_YUY2:
        return "PIXEL_FORMAT_YUY2";
    case PIXEL_FORMAT_ARGB:
        return "PIXEL_FORMAT_ARGB";
    case PIXEL_FORMAT_XRGB:
        return "PIXEL_FORMAT_XRGB";
    case PIXEL_FORMAT_RGB24:
        return "PIXEL_FORMAT_RGB24";
    case PIXEL_FORMAT_RGB32:
        return "PIXEL_FORMAT_RGB32";
    case PIXEL_FORMAT_MJPEG:
        return "PIXEL_FORMAT_MJPEG";
    case PIXEL_FORMAT_MT21:
        return "PIXEL_FORMAT_MT21";
    }
    NOTREACHED() << "Invalid VideoPixelFormat provided: " << format;
    return "";
}

bool IsYuvPlanar(VideoPixelFormat format)
{
    switch (format) {
    case PIXEL_FORMAT_YV12:
    case PIXEL_FORMAT_I420:
    case PIXEL_FORMAT_YV16:
    case PIXEL_FORMAT_YV12A:
    case PIXEL_FORMAT_YV24:
    case PIXEL_FORMAT_NV12:
    case PIXEL_FORMAT_NV21:
    case PIXEL_FORMAT_MT21:
        return true;

    case PIXEL_FORMAT_UNKNOWN:
    case PIXEL_FORMAT_UYVY:
    case PIXEL_FORMAT_YUY2:
    case PIXEL_FORMAT_ARGB:
    case PIXEL_FORMAT_XRGB:
    case PIXEL_FORMAT_RGB24:
    case PIXEL_FORMAT_RGB32:
    case PIXEL_FORMAT_MJPEG:
        return false;
    }
    return false;
}

} // namespace media
