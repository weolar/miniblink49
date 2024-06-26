// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/base/audio_buffer.h"

#include <cmath>

#include "base/logging.h"
#include "media/base/audio_bus.h"
#include "media/base/limits.h"
#include "media/base/timestamp_constants.h"

namespace media {

static base::TimeDelta CalculateDuration(int frames, double sample_rate)
{
    DCHECK_GT(sample_rate, 0);
    return base::TimeDelta::FromMicroseconds(
        frames * base::Time::kMicrosecondsPerSecond / sample_rate);
}

AudioBuffer::AudioBuffer(SampleFormat sample_format,
    ChannelLayout channel_layout,
    int channel_count,
    int sample_rate,
    int frame_count,
    bool create_buffer,
    const uint8* const* data,
    const base::TimeDelta timestamp)
    : sample_format_(sample_format)
    , channel_layout_(channel_layout)
    , channel_count_(channel_count)
    , sample_rate_(sample_rate)
    , adjusted_frame_count_(frame_count)
    , trim_start_(0)
    , end_of_stream_(!create_buffer && data == NULL && frame_count == 0)
    , timestamp_(timestamp)
    , duration_(end_of_stream_
              ? base::TimeDelta()
              : CalculateDuration(adjusted_frame_count_, sample_rate_))
{
    CHECK_GE(channel_count_, 0);
    CHECK_LE(channel_count_, limits::kMaxChannels);
    CHECK_GE(frame_count, 0);
    DCHECK(channel_layout == CHANNEL_LAYOUT_DISCRETE || ChannelLayoutToChannelCount(channel_layout) == channel_count);

    int bytes_per_channel = SampleFormatToBytesPerChannel(sample_format);
    DCHECK_LE(bytes_per_channel, kChannelAlignment);
    int data_size = frame_count * bytes_per_channel;

    // Empty buffer?
    if (!create_buffer)
        return;

    if (sample_format == kSampleFormatPlanarF32 || sample_format == kSampleFormatPlanarS16 || sample_format == kSampleFormatPlanarS32) {
        // Planar data, so need to allocate buffer for each channel.
        // Determine per channel data size, taking into account alignment.
        int block_size_per_channel = (data_size + kChannelAlignment - 1) & ~(kChannelAlignment - 1);
        DCHECK_GE(block_size_per_channel, data_size);

        // Allocate a contiguous buffer for all the channel data.
        data_.reset(static_cast<uint8*>(base::AlignedAlloc(
            channel_count_ * block_size_per_channel, kChannelAlignment)));
        channel_data_.reserve(channel_count_);

        // Copy each channel's data into the appropriate spot.
        for (int i = 0; i < channel_count_; ++i) {
            channel_data_.push_back(data_.get() + i * block_size_per_channel);
            if (data)
                memcpy(channel_data_[i], data[i], data_size);
        }
        return;
    }

    // Remaining formats are interleaved data.
    DCHECK(sample_format_ == kSampleFormatU8 || sample_format_ == kSampleFormatS16 || sample_format_ == kSampleFormatS32 || sample_format_ == kSampleFormatF32) << sample_format_;
    // Allocate our own buffer and copy the supplied data into it. Buffer must
    // contain the data for all channels.
    data_size *= channel_count_;
    data_.reset(
        static_cast<uint8*>(base::AlignedAlloc(data_size, kChannelAlignment)));
    channel_data_.reserve(1);
    channel_data_.push_back(data_.get());
    if (data)
        memcpy(data_.get(), data[0], data_size);
}

AudioBuffer::~AudioBuffer() { }

// static
scoped_refptr<AudioBuffer> AudioBuffer::CopyFrom(
    SampleFormat sample_format,
    ChannelLayout channel_layout,
    int channel_count,
    int sample_rate,
    int frame_count,
    const uint8* const* data,
    const base::TimeDelta timestamp)
{
    // If you hit this CHECK you likely have a bug in a demuxer. Go fix it.
    CHECK_GT(frame_count, 0); // Otherwise looks like an EOF buffer.
    CHECK(data[0]);
    return make_scoped_refptr(new AudioBuffer(sample_format,
        channel_layout,
        channel_count,
        sample_rate,
        frame_count,
        true,
        data,
        timestamp));
}

// static
scoped_refptr<AudioBuffer> AudioBuffer::CreateBuffer(
    SampleFormat sample_format,
    ChannelLayout channel_layout,
    int channel_count,
    int sample_rate,
    int frame_count)
{
    CHECK_GT(frame_count, 0); // Otherwise looks like an EOF buffer.
    return make_scoped_refptr(new AudioBuffer(sample_format,
        channel_layout,
        channel_count,
        sample_rate,
        frame_count,
        true,
        NULL,
        kNoTimestamp()));
}

// static
scoped_refptr<AudioBuffer> AudioBuffer::CreateEmptyBuffer(
    ChannelLayout channel_layout,
    int channel_count,
    int sample_rate,
    int frame_count,
    const base::TimeDelta timestamp)
{
    CHECK_GT(frame_count, 0); // Otherwise looks like an EOF buffer.
    // Since data == NULL, format doesn't matter.
    return make_scoped_refptr(new AudioBuffer(kSampleFormatF32,
        channel_layout,
        channel_count,
        sample_rate,
        frame_count,
        false,
        NULL,
        timestamp));
}

// static
scoped_refptr<AudioBuffer> AudioBuffer::CreateEOSBuffer()
{
    return make_scoped_refptr(new AudioBuffer(kUnknownSampleFormat,
        CHANNEL_LAYOUT_NONE,
        0,
        0,
        0,
        false,
        NULL,
        kNoTimestamp()));
}

template <typename Target, typename Dest>
static inline Dest ConvertSample(Target value);

// Convert int16 values in the range [INT16_MIN, INT16_MAX] to [-1.0, 1.0].
template <>
inline float ConvertSample<int16, float>(int16 value)
{
    return value * (value < 0 ? -1.0f / std::numeric_limits<int16>::min() : 1.0f / std::numeric_limits<int16>::max());
}

// Specializations for int32
template <>
inline int32 ConvertSample<int16, int32>(int16 value)
{
    return static_cast<int32>(value) << 16;
}

template <>
inline int32 ConvertSample<int32, int32>(int32 value)
{
    return value;
}

template <>
inline int32 ConvertSample<float, int32>(float value)
{
    return static_cast<int32>(value < 0
            ? (-value) * std::numeric_limits<int32>::min()
            : value * std::numeric_limits<int32>::max());
}

// Specializations for int16
template <>
inline int16 ConvertSample<int16, int16>(int16 sample)
{
    return sample;
}

template <>
inline int16 ConvertSample<int32, int16>(int32 sample)
{
    return sample >> 16;
}

template <>
inline int16 ConvertSample<float, int16>(float sample)
{
    return static_cast<int16>(
        nearbyint(sample < 0 ? (-sample) * std::numeric_limits<int16>::min()
                             : sample * std::numeric_limits<int16>::max()));
}

void AudioBuffer::ReadFrames(int frames_to_copy,
    int source_frame_offset,
    int dest_frame_offset,
    AudioBus* dest)
{
    // Deinterleave each channel (if necessary) and convert to 32bit
    // floating-point with nominal range -1.0 -> +1.0 (if necessary).

    // |dest| must have the same number of channels, and the number of frames
    // specified must be in range.
    DCHECK(!end_of_stream());
    DCHECK_EQ(dest->channels(), channel_count_);
    DCHECK_LE(source_frame_offset + frames_to_copy, adjusted_frame_count_);
    DCHECK_LE(dest_frame_offset + frames_to_copy, dest->frames());

    // Move the start past any frames that have been trimmed.
    source_frame_offset += trim_start_;

    if (!data_) {
        // Special case for an empty buffer.
        dest->ZeroFramesPartial(dest_frame_offset, frames_to_copy);
        return;
    }

    if (sample_format_ == kSampleFormatPlanarF32) {
        // Format is planar float32. Copy the data from each channel as a block.
        for (int ch = 0; ch < channel_count_; ++ch) {
            const float* source_data = reinterpret_cast<const float*>(channel_data_[ch]) + source_frame_offset;
            memcpy(dest->channel(ch) + dest_frame_offset,
                source_data,
                sizeof(float) * frames_to_copy);
        }
        return;
    }

    if (sample_format_ == kSampleFormatPlanarS16) {
        // Format is planar signed16. Convert each value into float and insert into
        // output channel data.
        for (int ch = 0; ch < channel_count_; ++ch) {
            const int16* source_data = reinterpret_cast<const int16*>(channel_data_[ch]) + source_frame_offset;
            float* dest_data = dest->channel(ch) + dest_frame_offset;
            for (int i = 0; i < frames_to_copy; ++i) {
                dest_data[i] = ConvertSample<int16, float>(source_data[i]);
            }
        }
        return;
    }

    if (sample_format_ == kSampleFormatF32) {
        // Format is interleaved float32. Copy the data into each channel.
        const float* source_data = reinterpret_cast<const float*>(data_.get()) + source_frame_offset * channel_count_;
        for (int ch = 0; ch < channel_count_; ++ch) {
            float* dest_data = dest->channel(ch) + dest_frame_offset;
            for (int i = 0, offset = ch; i < frames_to_copy;
                 ++i, offset += channel_count_) {
                dest_data[i] = source_data[offset];
            }
        }
        return;
    }

    // Remaining formats are integer interleaved data. Use the deinterleaving code
    // in AudioBus to copy the data.
    DCHECK(sample_format_ == kSampleFormatU8 || sample_format_ == kSampleFormatS16 || sample_format_ == kSampleFormatS32);
    int bytes_per_channel = SampleFormatToBytesPerChannel(sample_format_);
    int frame_size = channel_count_ * bytes_per_channel;
    const uint8* source_data = data_.get() + source_frame_offset * frame_size;
    dest->FromInterleavedPartial(
        source_data, dest_frame_offset, frames_to_copy, bytes_per_channel);
}

template <class Target, typename Dest>
void InterleaveAndConvert(const std::vector<uint8*>& channel_data,
    size_t frames_to_copy,
    int trim_start,
    Dest* dest_data)
{
    for (size_t ch = 0; ch < channel_data.size(); ++ch) {
        const Target* source_data = reinterpret_cast<const Target*>(channel_data[ch]) + trim_start;
        for (size_t i = 0, offset = ch; i < frames_to_copy;
             ++i, offset += channel_data.size()) {
            dest_data[offset] = ConvertSample<Target, Dest>(source_data[i]);
        }
    }
}

template <typename Dest>
void ReadFramesInterleaved(const std::vector<uint8*>& channel_data,
    int channel_count,
    SampleFormat sample_format,
    int frames_to_copy,
    int trim_start,
    Dest* dest_data)
{
    switch (sample_format) {
    case kSampleFormatU8:
        NOTREACHED();
        break;
    case kSampleFormatS16:
        InterleaveAndConvert<int16, Dest>(
            channel_data, frames_to_copy * channel_count, trim_start, dest_data);
        break;
    case kSampleFormatS32:
        InterleaveAndConvert<int32, Dest>(
            channel_data, frames_to_copy * channel_count, trim_start, dest_data);
        break;
    case kSampleFormatF32:
        InterleaveAndConvert<float, Dest>(
            channel_data, frames_to_copy * channel_count, trim_start, dest_data);
        break;
    case kSampleFormatPlanarS16:
        InterleaveAndConvert<int16, Dest>(channel_data, frames_to_copy,
            trim_start, dest_data);
        break;
    case kSampleFormatPlanarF32:
        InterleaveAndConvert<float, Dest>(channel_data, frames_to_copy,
            trim_start, dest_data);
        break;
    case kSampleFormatPlanarS32:
        InterleaveAndConvert<int32, Dest>(channel_data, frames_to_copy,
            trim_start, dest_data);
        break;
    case kUnknownSampleFormat:
        NOTREACHED();
        break;
    }
}

void AudioBuffer::ReadFramesInterleavedS32(int frames_to_copy,
    int32* dest_data)
{
    DCHECK_LE(frames_to_copy, adjusted_frame_count_);
    ReadFramesInterleaved<int32>(channel_data_, channel_count_, sample_format_,
        frames_to_copy, trim_start_, dest_data);
}

void AudioBuffer::ReadFramesInterleavedS16(int frames_to_copy,
    int16* dest_data)
{
    DCHECK_LE(frames_to_copy, adjusted_frame_count_);
    ReadFramesInterleaved<int16>(channel_data_, channel_count_, sample_format_,
        frames_to_copy, trim_start_, dest_data);
}

void AudioBuffer::TrimStart(int frames_to_trim)
{
    CHECK_GE(frames_to_trim, 0);
    CHECK_LE(frames_to_trim, adjusted_frame_count_);

    // Adjust the number of frames in this buffer and where the start really is.
    adjusted_frame_count_ -= frames_to_trim;
    trim_start_ += frames_to_trim;

    // Adjust timestamp_ and duration_ to reflect the smaller number of frames.
    const base::TimeDelta old_duration = duration_;
    duration_ = CalculateDuration(adjusted_frame_count_, sample_rate_);
    timestamp_ += old_duration - duration_;
}

void AudioBuffer::TrimEnd(int frames_to_trim)
{
    CHECK_GE(frames_to_trim, 0);
    CHECK_LE(frames_to_trim, adjusted_frame_count_);

    // Adjust the number of frames and duration for this buffer.
    adjusted_frame_count_ -= frames_to_trim;
    duration_ = CalculateDuration(adjusted_frame_count_, sample_rate_);
}

void AudioBuffer::TrimRange(int start, int end)
{
    CHECK_GE(start, 0);
    CHECK_LE(end, adjusted_frame_count_);

    const int frames_to_trim = end - start;
    CHECK_GE(frames_to_trim, 0);
    CHECK_LE(frames_to_trim, adjusted_frame_count_);

    const int bytes_per_channel = SampleFormatToBytesPerChannel(sample_format_);
    const int frames_to_copy = adjusted_frame_count_ - end;
    if (frames_to_copy > 0) {
        switch (sample_format_) {
        case kSampleFormatPlanarS16:
        case kSampleFormatPlanarF32:
        case kSampleFormatPlanarS32:
            // Planar data must be shifted per channel.
            for (int ch = 0; ch < channel_count_; ++ch) {
                memmove(channel_data_[ch] + (trim_start_ + start) * bytes_per_channel,
                    channel_data_[ch] + (trim_start_ + end) * bytes_per_channel,
                    bytes_per_channel * frames_to_copy);
            }
            break;
        case kSampleFormatU8:
        case kSampleFormatS16:
        case kSampleFormatS32:
        case kSampleFormatF32: {
            // Interleaved data can be shifted all at once.
            const int frame_size = channel_count_ * bytes_per_channel;
            memmove(channel_data_[0] + (trim_start_ + start) * frame_size,
                channel_data_[0] + (trim_start_ + end) * frame_size,
                frame_size * frames_to_copy);
            break;
        }
        case kUnknownSampleFormat:
            NOTREACHED() << "Invalid sample format!";
        }
    } else {
        CHECK_EQ(frames_to_copy, 0);
    }

    // Trim the leftover data off the end of the buffer and update duration.
    TrimEnd(frames_to_trim);
}

} // namespace media
