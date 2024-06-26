// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/audio/audio_parameters.h"

#include "base/logging.h"
#include "media/base/limits.h"

namespace media {

AudioParameters::AudioParameters()
    : AudioParameters(AUDIO_PCM_LINEAR, CHANNEL_LAYOUT_NONE, 0, 0, 0)
{
}

AudioParameters::AudioParameters(Format format,
    ChannelLayout channel_layout,
    int sample_rate,
    int bits_per_sample,
    int frames_per_buffer)
{
    Reset(format, channel_layout, sample_rate, bits_per_sample,
        frames_per_buffer);
}

AudioParameters::~AudioParameters() { }

AudioParameters::AudioParameters(const AudioParameters&) = default;
AudioParameters& AudioParameters::operator=(const AudioParameters&) = default;

void AudioParameters::Reset(Format format,
    ChannelLayout channel_layout,
    int sample_rate,
    int bits_per_sample,
    int frames_per_buffer)
{
    format_ = format;
    channel_layout_ = channel_layout;
    channels_ = ChannelLayoutToChannelCount(channel_layout);
    sample_rate_ = sample_rate;
    bits_per_sample_ = bits_per_sample;
    frames_per_buffer_ = frames_per_buffer;
    effects_ = NO_EFFECTS;
    mic_positions_.clear();
}

bool AudioParameters::IsValid() const
{
    return (channels_ > 0) && (channels_ <= media::limits::kMaxChannels) && (channel_layout_ > CHANNEL_LAYOUT_UNSUPPORTED) && (sample_rate_ >= media::limits::kMinSampleRate) && (sample_rate_ <= media::limits::kMaxSampleRate) && (bits_per_sample_ > 0) && (bits_per_sample_ <= media::limits::kMaxBitsPerSample) && (frames_per_buffer_ > 0) && (frames_per_buffer_ <= media::limits::kMaxSamplesPerPacket) && (channel_layout_ == CHANNEL_LAYOUT_DISCRETE || channels_ == ChannelLayoutToChannelCount(channel_layout_));
}

std::string AudioParameters::AsHumanReadableString() const
{
    std::ostringstream s;
    s << "format: " << format() << " channel_layout: " << channel_layout()
      << " channels: " << channels() << " sample_rate: " << sample_rate()
      << " bits_per_sample: " << bits_per_sample()
      << " frames_per_buffer: " << frames_per_buffer()
      << " effects: " << effects()
      << " mic_positions: " << PointsToString(mic_positions_);
    return s.str();
}

int AudioParameters::GetBytesPerBuffer() const
{
    return frames_per_buffer_ * GetBytesPerFrame();
}

int AudioParameters::GetBytesPerSecond() const
{
    return sample_rate_ * GetBytesPerFrame();
}

int AudioParameters::GetBytesPerFrame() const
{
    return channels_ * bits_per_sample_ / 8;
}

base::TimeDelta AudioParameters::GetBufferDuration() const
{
    return base::TimeDelta::FromMicroseconds(static_cast<int64>(
        frames_per_buffer_ * base::Time::kMicrosecondsPerSecond / static_cast<float>(sample_rate_)));
}

bool AudioParameters::Equals(const AudioParameters& other) const
{
    return format_ == other.format() && sample_rate_ == other.sample_rate() && channel_layout_ == other.channel_layout() && channels_ == other.channels() && bits_per_sample_ == other.bits_per_sample() && frames_per_buffer_ == other.frames_per_buffer() && effects_ == other.effects() && mic_positions_ == other.mic_positions_;
}

} // namespace media
