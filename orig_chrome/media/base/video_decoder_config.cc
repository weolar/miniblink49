// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/base/video_decoder_config.h"

#include <vector>

#include "base/logging.h"
#include "media/base/video_frame.h"

namespace media {

VideoCodec VideoCodecProfileToVideoCodec(VideoCodecProfile profile)
{
    switch (profile) {
    case VIDEO_CODEC_PROFILE_UNKNOWN:
        return kUnknownVideoCodec;
    case H264PROFILE_BASELINE:
    case H264PROFILE_MAIN:
    case H264PROFILE_EXTENDED:
    case H264PROFILE_HIGH:
    case H264PROFILE_HIGH10PROFILE:
    case H264PROFILE_HIGH422PROFILE:
    case H264PROFILE_HIGH444PREDICTIVEPROFILE:
    case H264PROFILE_SCALABLEBASELINE:
    case H264PROFILE_SCALABLEHIGH:
    case H264PROFILE_STEREOHIGH:
    case H264PROFILE_MULTIVIEWHIGH:
        return kCodecH264;
    case VP8PROFILE_ANY:
        return kCodecVP8;
    case VP9PROFILE_ANY:
        return kCodecVP9;
    }
    NOTREACHED();
    return kUnknownVideoCodec;
}

VideoDecoderConfig::VideoDecoderConfig()
    : codec_(kUnknownVideoCodec)
    , profile_(VIDEO_CODEC_PROFILE_UNKNOWN)
    , format_(PIXEL_FORMAT_UNKNOWN)
    , is_encrypted_(false)
{
}

VideoDecoderConfig::VideoDecoderConfig(VideoCodec codec,
    VideoCodecProfile profile,
    VideoPixelFormat format,
    ColorSpace color_space,
    const gfx::Size& coded_size,
    const gfx::Rect& visible_rect,
    const gfx::Size& natural_size,
    const std::vector<uint8_t>& extra_data,
    bool is_encrypted)
{
    Initialize(codec, profile, format, color_space, coded_size, visible_rect,
        natural_size, extra_data, is_encrypted);
}

VideoDecoderConfig::~VideoDecoderConfig() { }

void VideoDecoderConfig::Initialize(VideoCodec codec,
    VideoCodecProfile profile,
    VideoPixelFormat format,
    ColorSpace color_space,
    const gfx::Size& coded_size,
    const gfx::Rect& visible_rect,
    const gfx::Size& natural_size,
    const std::vector<uint8_t>& extra_data,
    bool is_encrypted)
{
    codec_ = codec;
    profile_ = profile;
    format_ = format;
    color_space_ = color_space;
    coded_size_ = coded_size;
    visible_rect_ = visible_rect;
    natural_size_ = natural_size;
    extra_data_ = extra_data;
    is_encrypted_ = is_encrypted;
}

bool VideoDecoderConfig::IsValidConfig() const
{
    return codec_ != kUnknownVideoCodec && natural_size_.width() > 0 && natural_size_.height() > 0 && VideoFrame::IsValidConfig(format_, VideoFrame::STORAGE_UNOWNED_MEMORY, coded_size_, visible_rect_, natural_size_);
}

bool VideoDecoderConfig::Matches(const VideoDecoderConfig& config) const
{
    return ((codec() == config.codec()) && (format() == config.format()) && (profile() == config.profile()) && (coded_size() == config.coded_size()) && (visible_rect() == config.visible_rect()) && (natural_size() == config.natural_size()) && (extra_data() == config.extra_data()) && (is_encrypted() == config.is_encrypted()));
}

std::string VideoDecoderConfig::AsHumanReadableString() const
{
    std::ostringstream s;
    s << "codec: " << GetHumanReadableCodecName()
      << " format: " << format()
      << " profile: " << profile()
      << " coded size: [" << coded_size().width()
      << "," << coded_size().height() << "]"
      << " visible rect: [" << visible_rect().x()
      << "," << visible_rect().y()
      << "," << visible_rect().width()
      << "," << visible_rect().height() << "]"
      << " natural size: [" << natural_size().width()
      << "," << natural_size().height() << "]"
      << " has extra data? " << (extra_data().empty() ? "false" : "true")
      << " encrypted? " << (is_encrypted() ? "true" : "false");
    return s.str();
}

// The names come from src/third_party/ffmpeg/libavcodec/codec_desc.c
std::string VideoDecoderConfig::GetHumanReadableCodecName() const
{
    switch (codec()) {
    case kUnknownVideoCodec:
        return "unknown";
    case kCodecH264:
        return "h264";
    case kCodecHEVC:
        return "hevc";
    case kCodecVC1:
        return "vc1";
    case kCodecMPEG2:
        return "mpeg2video";
    case kCodecMPEG4:
        return "mpeg4";
    case kCodecTheora:
        return "theora";
    case kCodecVP8:
        return "vp8";
    case kCodecVP9:
        return "vp9";
    }
    NOTREACHED();
    return "";
}

} // namespace media
