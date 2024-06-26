// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/base/audio_decoder_config.h"

#include "base/logging.h"
#include "media/base/limits.h"

namespace media {

AudioDecoderConfig::AudioDecoderConfig()
    : codec_(kUnknownAudioCodec)
    , sample_format_(kUnknownSampleFormat)
    , bytes_per_channel_(0)
    , channel_layout_(CHANNEL_LAYOUT_UNSUPPORTED)
    , samples_per_second_(0)
    , bytes_per_frame_(0)
    , is_encrypted_(false)
    , codec_delay_(0)
{
}

AudioDecoderConfig::AudioDecoderConfig(AudioCodec codec,
    SampleFormat sample_format,
    ChannelLayout channel_layout,
    int samples_per_second,
    const std::vector<uint8_t>& extra_data,
    bool is_encrypted)
{
    Initialize(codec, sample_format, channel_layout, samples_per_second,
        extra_data, is_encrypted, base::TimeDelta(), 0);
}

void AudioDecoderConfig::Initialize(AudioCodec codec,
    SampleFormat sample_format,
    ChannelLayout channel_layout,
    int samples_per_second,
    const std::vector<uint8_t>& extra_data,
    bool is_encrypted,
    base::TimeDelta seek_preroll,
    int codec_delay)
{
    codec_ = codec;
    channel_layout_ = channel_layout;
    samples_per_second_ = samples_per_second;
    sample_format_ = sample_format;
    bytes_per_channel_ = SampleFormatToBytesPerChannel(sample_format);
    extra_data_ = extra_data;
    is_encrypted_ = is_encrypted;
    seek_preroll_ = seek_preroll;
    codec_delay_ = codec_delay;

    int channels = ChannelLayoutToChannelCount(channel_layout_);
    bytes_per_frame_ = channels * bytes_per_channel_;
}

AudioDecoderConfig::~AudioDecoderConfig() { }

bool AudioDecoderConfig::IsValidConfig() const
{
    return codec_ != kUnknownAudioCodec && channel_layout_ != CHANNEL_LAYOUT_UNSUPPORTED && bytes_per_channel_ > 0 && bytes_per_channel_ <= limits::kMaxBytesPerSample && samples_per_second_ > 0 && samples_per_second_ <= limits::kMaxSampleRate && sample_format_ != kUnknownSampleFormat && seek_preroll_ >= base::TimeDelta() && codec_delay_ >= 0;
}

bool AudioDecoderConfig::Matches(const AudioDecoderConfig& config) const
{
    return ((codec() == config.codec()) && (bytes_per_channel() == config.bytes_per_channel()) && (channel_layout() == config.channel_layout()) && (samples_per_second() == config.samples_per_second()) && (extra_data() == config.extra_data()) && (is_encrypted() == config.is_encrypted()) && (sample_format() == config.sample_format()) && (seek_preroll() == config.seek_preroll()) && (codec_delay() == config.codec_delay()));
}

std::string AudioDecoderConfig::AsHumanReadableString() const
{
    std::ostringstream s;
    s << "codec: " << GetHumanReadableCodecName()
      << " bytes_per_channel: " << bytes_per_channel()
      << " channel_layout: " << channel_layout()
      << " samples_per_second: " << samples_per_second()
      << " sample_format: " << sample_format()
      << " bytes_per_frame: " << bytes_per_frame()
      << " seek_preroll: " << seek_preroll().InMilliseconds() << "ms"
      << " codec_delay: " << codec_delay()
      << " has extra data? " << (extra_data().empty() ? "false" : "true")
      << " encrypted? " << (is_encrypted() ? "true" : "false");
    return s.str();
}

// These names come from src/third_party/ffmpeg/libavcodec/codec_desc.c
std::string AudioDecoderConfig::GetHumanReadableCodecName() const
{
    switch (codec()) {
    case kUnknownAudioCodec:
        return "unknown";
    case kCodecAAC:
        return "aac";
    case kCodecMP3:
        return "mp3";
    case kCodecPCM:
    case kCodecPCM_S16BE:
    case kCodecPCM_S24BE:
        return "pcm";
    case kCodecVorbis:
        return "vorbis";
    case kCodecFLAC:
        return "flac";
    case kCodecAMR_NB:
        return "amr_nb";
    case kCodecAMR_WB:
        return "amr_wb";
    case kCodecGSM_MS:
        return "gsm_ms";
    case kCodecPCM_ALAW:
        return "pcm_alaw";
    case kCodecPCM_MULAW:
        return "pcm_mulaw";
    case kCodecOpus:
        return "opus";
    case kCodecALAC:
        return "alac";
    }
    NOTREACHED();
    return "";
}

} // namespace media
