// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/base/video_capture_types.h"

#include "base/logging.h"
#include "base/strings/stringprintf.h"
#include "media/base/limits.h"
#include "media/base/video_frame.h"

namespace media {

// This list is ordered by precedence of use.
static VideoPixelFormat const kSupportedCapturePixelFormats[] = {
    PIXEL_FORMAT_I420,
    PIXEL_FORMAT_YV12,
    PIXEL_FORMAT_NV12,
    PIXEL_FORMAT_NV21,
    PIXEL_FORMAT_UYVY,
    PIXEL_FORMAT_YUY2,
    PIXEL_FORMAT_RGB24,
    PIXEL_FORMAT_RGB32,
    PIXEL_FORMAT_ARGB,
    PIXEL_FORMAT_MJPEG,
};

VideoCaptureFormat::VideoCaptureFormat()
    : frame_rate(0.0f)
    , pixel_format(PIXEL_FORMAT_UNKNOWN)
    , pixel_storage(PIXEL_STORAGE_CPU)
{
}

VideoCaptureFormat::VideoCaptureFormat(const gfx::Size& frame_size,
    float frame_rate,
    VideoPixelFormat pixel_format)
    : frame_size(frame_size)
    , frame_rate(frame_rate)
    , pixel_format(pixel_format)
    , pixel_storage(PIXEL_STORAGE_CPU)
{
}

VideoCaptureFormat::VideoCaptureFormat(const gfx::Size& frame_size,
    float frame_rate,
    VideoPixelFormat pixel_format,
    VideoPixelStorage pixel_storage)
    : frame_size(frame_size)
    , frame_rate(frame_rate)
    , pixel_format(pixel_format)
    , pixel_storage(pixel_storage)
{
}

bool VideoCaptureFormat::IsValid() const
{
    return (frame_size.width() < media::limits::kMaxDimension) && (frame_size.height() < media::limits::kMaxDimension) && (frame_size.GetArea() >= 0) && (frame_size.GetArea() < media::limits::kMaxCanvas) && (frame_rate >= 0.0f) && (frame_rate < media::limits::kMaxFramesPerSecond) && (pixel_storage != PIXEL_STORAGE_TEXTURE || pixel_format == PIXEL_FORMAT_ARGB);
}

size_t VideoCaptureFormat::ImageAllocationSize() const
{
    return VideoFrame::AllocationSize(pixel_format, frame_size);
}

//static
std::string VideoCaptureFormat::ToString(const VideoCaptureFormat& format)
{
    return base::StringPrintf(
        "(%s)@%.3ffps, pixel format: %s storage: %s.",
        format.frame_size.ToString().c_str(), format.frame_rate,
        VideoPixelFormatToString(format.pixel_format).c_str(),
        PixelStorageToString(format.pixel_storage).c_str());
}

// static
std::string VideoCaptureFormat::PixelStorageToString(
    VideoPixelStorage storage)
{
    switch (storage) {
    case PIXEL_STORAGE_CPU:
        return "CPU";
    case PIXEL_STORAGE_TEXTURE:
        return "TEXTURE";
    case PIXEL_STORAGE_GPUMEMORYBUFFER:
        return "GPUMEMORYBUFFER";
    }
    NOTREACHED() << "Invalid VideoPixelStorage provided: "
                 << static_cast<int>(storage);
    return std::string();
}

// static
bool VideoCaptureFormat::ComparePixelFormatPreference(
    const VideoPixelFormat& lhs,
    const VideoPixelFormat& rhs)
{
    const auto& format_lhs = std::find(
        kSupportedCapturePixelFormats,
        kSupportedCapturePixelFormats + arraysize(kSupportedCapturePixelFormats),
        lhs);
    const auto& format_rhs = std::find(
        kSupportedCapturePixelFormats,
        kSupportedCapturePixelFormats + arraysize(kSupportedCapturePixelFormats),
        rhs);
    return format_lhs < format_rhs;
}

VideoCaptureParams::VideoCaptureParams()
    : resolution_change_policy(RESOLUTION_POLICY_FIXED_RESOLUTION)
    , power_line_frequency(PowerLineFrequency::FREQUENCY_DEFAULT)
{
}

} // namespace media
