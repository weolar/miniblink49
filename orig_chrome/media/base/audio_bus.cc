// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/base/audio_bus.h"

#include "base/logging.h"
#include "base/numerics/safe_conversions.h"
#include "media/audio/audio_parameters.h"
#include "media/base/limits.h"
#include "media/base/vector_math.h"

namespace media {

static const uint8 kUint8Bias = 128;

static bool IsAligned(void* ptr)
{
    return (reinterpret_cast<uintptr_t>(ptr) & (AudioBus::kChannelAlignment - 1)) == 0U;
}

// Calculates the required size for an AudioBus with the given params, sets
// |aligned_frames| to the actual frame length of each channel array.
static int CalculateMemorySizeInternal(int channels, int frames,
    int* out_aligned_frames)
{
    // Choose a size such that each channel will be aligned by
    // kChannelAlignment when stored in a contiguous block.
    int aligned_frames = ((frames * sizeof(float) + AudioBus::kChannelAlignment - 1) & ~(AudioBus::kChannelAlignment - 1)) / sizeof(float);

    if (out_aligned_frames)
        *out_aligned_frames = aligned_frames;

    return sizeof(float) * channels * aligned_frames;
}

// |Format| is the destination type.  If a bias is present, |Fixed| must be a
// type larger than |Format| such that operations can be made without
// overflowing.  Without a bias |Fixed| must be the same as |Format|.
template <class Format, class Fixed, Format Bias>
static void FromInterleavedInternal(const void* src, int start_frame,
    int frames, AudioBus* dest,
    float min, float max)
{
    static_assert((Bias == 0 && sizeof(Fixed) == sizeof(Format)) || sizeof(Fixed) > sizeof(Format), "invalid deinterleave types");
    const Format* source = static_cast<const Format*>(src);
    const int channels = dest->channels();
    for (int ch = 0; ch < channels; ++ch) {
        float* channel_data = dest->channel(ch);
        for (int i = start_frame, offset = ch; i < start_frame + frames;
             ++i, offset += channels) {
            const Fixed v = static_cast<Fixed>(source[offset]) - Bias;
            channel_data[i] = v * (v < 0 ? -min : max);
        }
    }
}

// |Format| is the destination type.  If a bias is present, |Fixed| must be a
// type larger than |Format| such that operations can be made without
// overflowing.  Without a bias |Fixed| must be the same as |Format|.
template <class Format, class Fixed, Format Bias>
static void ToInterleavedInternal(const AudioBus* source, int start_frame,
    int frames, void* dst, Fixed min, Fixed max)
{
    static_assert((Bias == 0 && sizeof(Fixed) == sizeof(Format)) || sizeof(Fixed) > sizeof(Format), "invalid interleave types");
    Format* dest = static_cast<Format*>(dst);
    const int channels = source->channels();
    for (int ch = 0; ch < channels; ++ch) {
        const float* channel_data = source->channel(ch);
        for (int i = start_frame, offset = ch; i < start_frame + frames;
             ++i, offset += channels) {
            const float v = channel_data[i];

            Fixed sample;
            if (v < 0)
                sample = v <= -1 ? min : static_cast<Fixed>(-v * min);
            else
                sample = v >= 1 ? max : static_cast<Fixed>(v * max);

            dest[offset] = static_cast<Format>(sample) + Bias;
        }
    }
}

static void ValidateConfig(int channels, int frames)
{
    CHECK_GT(frames, 0);
    CHECK_GT(channels, 0);
    CHECK_LE(channels, static_cast<int>(limits::kMaxChannels));
}

static void CheckOverflow(int start_frame, int frames, int total_frames)
{
    CHECK_GE(start_frame, 0);
    CHECK_GE(frames, 0);
    CHECK_GT(total_frames, 0);
    int sum = start_frame + frames;
    CHECK_LE(sum, total_frames);
    CHECK_GE(sum, 0);
}

AudioBus::AudioBus(int channels, int frames)
    : frames_(frames)
    , can_set_channel_data_(false)
{
    ValidateConfig(channels, frames_);

    int aligned_frames = 0;
    int size = CalculateMemorySizeInternal(channels, frames, &aligned_frames);

    data_.reset(static_cast<float*>(base::AlignedAlloc(
        size, AudioBus::kChannelAlignment)));

    BuildChannelData(channels, aligned_frames, data_.get());
}

AudioBus::AudioBus(int channels, int frames, float* data)
    : frames_(frames)
    , can_set_channel_data_(false)
{
    // Since |data| may have come from an external source, ensure it's valid.
    CHECK(data);
    ValidateConfig(channels, frames_);

    int aligned_frames = 0;
    CalculateMemorySizeInternal(channels, frames, &aligned_frames);

    BuildChannelData(channels, aligned_frames, data);
}

AudioBus::AudioBus(int frames, const std::vector<float*>& channel_data)
    : channel_data_(channel_data)
    , frames_(frames)
    , can_set_channel_data_(false)
{
    ValidateConfig(
        base::checked_cast<int>(channel_data_.size()), frames_);

    // Sanity check wrapped vector for alignment and channel count.
    for (size_t i = 0; i < channel_data_.size(); ++i)
        DCHECK(IsAligned(channel_data_[i]));
}

AudioBus::AudioBus(int channels)
    : channel_data_(channels)
    , frames_(0)
    , can_set_channel_data_(true)
{
    CHECK_GT(channels, 0);
    for (size_t i = 0; i < channel_data_.size(); ++i)
        channel_data_[i] = NULL;
}

AudioBus::~AudioBus() { }

scoped_ptr<AudioBus> AudioBus::Create(int channels, int frames)
{
    return scoped_ptr<AudioBus>(new AudioBus(channels, frames));
}

scoped_ptr<AudioBus> AudioBus::Create(const AudioParameters& params)
{
    return scoped_ptr<AudioBus>(new AudioBus(
        params.channels(), params.frames_per_buffer()));
}

scoped_ptr<AudioBus> AudioBus::CreateWrapper(int channels)
{
    return scoped_ptr<AudioBus>(new AudioBus(channels));
}

scoped_ptr<AudioBus> AudioBus::WrapVector(
    int frames, const std::vector<float*>& channel_data)
{
    return scoped_ptr<AudioBus>(new AudioBus(frames, channel_data));
}

scoped_ptr<AudioBus> AudioBus::WrapMemory(int channels, int frames,
    void* data)
{
    // |data| must be aligned by AudioBus::kChannelAlignment.
    CHECK(IsAligned(data));
    return scoped_ptr<AudioBus>(new AudioBus(
        channels, frames, static_cast<float*>(data)));
}

scoped_ptr<AudioBus> AudioBus::WrapMemory(const AudioParameters& params,
    void* data)
{
    // |data| must be aligned by AudioBus::kChannelAlignment.
    CHECK(IsAligned(data));
    return scoped_ptr<AudioBus>(new AudioBus(
        params.channels(), params.frames_per_buffer(),
        static_cast<float*>(data)));
}

void AudioBus::SetChannelData(int channel, float* data)
{
    CHECK(can_set_channel_data_);
    CHECK(data);
    CHECK_GE(channel, 0);
    CHECK_LT(static_cast<size_t>(channel), channel_data_.size());
    DCHECK(IsAligned(data));
    channel_data_[channel] = data;
}

void AudioBus::set_frames(int frames)
{
    CHECK(can_set_channel_data_);
    ValidateConfig(static_cast<int>(channel_data_.size()), frames);
    frames_ = frames;
}

void AudioBus::ZeroFramesPartial(int start_frame, int frames)
{
    CheckOverflow(start_frame, frames, frames_);

    if (frames <= 0)
        return;

    for (size_t i = 0; i < channel_data_.size(); ++i) {
        memset(channel_data_[i] + start_frame, 0,
            frames * sizeof(*channel_data_[i]));
    }
}

void AudioBus::ZeroFrames(int frames)
{
    ZeroFramesPartial(0, frames);
}

void AudioBus::Zero()
{
    ZeroFrames(frames_);
}

bool AudioBus::AreFramesZero() const
{
    for (size_t i = 0; i < channel_data_.size(); ++i) {
        for (int j = 0; j < frames_; ++j) {
            if (channel_data_[i][j])
                return false;
        }
    }
    return true;
}

int AudioBus::CalculateMemorySize(const AudioParameters& params)
{
    return CalculateMemorySizeInternal(
        params.channels(), params.frames_per_buffer(), NULL);
}

int AudioBus::CalculateMemorySize(int channels, int frames)
{
    return CalculateMemorySizeInternal(channels, frames, NULL);
}

void AudioBus::BuildChannelData(int channels, int aligned_frames, float* data)
{
    DCHECK(IsAligned(data));
    DCHECK_EQ(channel_data_.size(), 0U);
    // Separate audio data out into channels for easy lookup later.  Figure out
    channel_data_.reserve(channels);
    for (int i = 0; i < channels; ++i)
        channel_data_.push_back(data + i * aligned_frames);
}

// TODO(dalecurtis): See if intrinsic optimizations help any here.
void AudioBus::FromInterleavedPartial(const void* source, int start_frame,
    int frames, int bytes_per_sample)
{
    CheckOverflow(start_frame, frames, frames_);
    switch (bytes_per_sample) {
    case 1:
        FromInterleavedInternal<uint8, int16, kUint8Bias>(
            source, start_frame, frames, this,
            1.0f / kint8min, 1.0f / kint8max);
        break;
    case 2:
        FromInterleavedInternal<int16, int16, 0>(
            source, start_frame, frames, this,
            1.0f / kint16min, 1.0f / kint16max);
        break;
    case 4:
        FromInterleavedInternal<int32, int32, 0>(
            source, start_frame, frames, this,
            1.0f / kint32min, 1.0f / kint32max);
        break;
    default:
        NOTREACHED() << "Unsupported bytes per sample encountered.";
        ZeroFramesPartial(start_frame, frames);
        return;
    }

    // Don't clear remaining frames if this is a partial deinterleave.
    if (!start_frame) {
        // Zero any remaining frames.
        ZeroFramesPartial(frames, frames_ - frames);
    }
}

void AudioBus::FromInterleaved(const void* source, int frames,
    int bytes_per_sample)
{
    FromInterleavedPartial(source, 0, frames, bytes_per_sample);
}

void AudioBus::ToInterleaved(int frames, int bytes_per_sample,
    void* dest) const
{
    ToInterleavedPartial(0, frames, bytes_per_sample, dest);
}

// TODO(dalecurtis): See if intrinsic optimizations help any here.
void AudioBus::ToInterleavedPartial(int start_frame, int frames,
    int bytes_per_sample, void* dest) const
{
    CheckOverflow(start_frame, frames, frames_);
    switch (bytes_per_sample) {
    case 1:
        ToInterleavedInternal<uint8, int16, kUint8Bias>(
            this, start_frame, frames, dest, kint8min, kint8max);
        break;
    case 2:
        ToInterleavedInternal<int16, int16, 0>(
            this, start_frame, frames, dest, kint16min, kint16max);
        break;
    case 4:
        ToInterleavedInternal<int32, int32, 0>(
            this, start_frame, frames, dest, kint32min, kint32max);
        break;
    default:
        NOTREACHED() << "Unsupported bytes per sample encountered.";
        memset(dest, 0, frames * bytes_per_sample);
        return;
    }
}

void AudioBus::CopyTo(AudioBus* dest) const
{
    CopyPartialFramesTo(0, frames(), 0, dest);
}

void AudioBus::CopyPartialFramesTo(int source_start_frame,
    int frame_count,
    int dest_start_frame,
    AudioBus* dest) const
{
    CHECK_EQ(channels(), dest->channels());
    CHECK_LE(source_start_frame + frame_count, frames());
    CHECK_LE(dest_start_frame + frame_count, dest->frames());

    // Since we don't know if the other AudioBus is wrapped or not (and we don't
    // want to care), just copy using the public channel() accessors.
    for (int i = 0; i < channels(); ++i) {
        memcpy(dest->channel(i) + dest_start_frame,
            channel(i) + source_start_frame,
            sizeof(*channel(i)) * frame_count);
    }
}

void AudioBus::Scale(float volume)
{
    if (volume > 0 && volume != 1) {
        for (int i = 0; i < channels(); ++i)
            vector_math::FMUL(channel(i), volume, frames(), channel(i));
    } else if (volume == 0) {
        Zero();
    }
}

void AudioBus::SwapChannels(int a, int b)
{
    DCHECK(a < channels() && a >= 0);
    DCHECK(b < channels() && b >= 0);
    DCHECK_NE(a, b);
    std::swap(channel_data_[a], channel_data_[b]);
}

scoped_refptr<AudioBusRefCounted> AudioBusRefCounted::Create(
    int channels, int frames)
{
    return scoped_refptr<AudioBusRefCounted>(
        new AudioBusRefCounted(channels, frames));
}

AudioBusRefCounted::AudioBusRefCounted(int channels, int frames)
    : AudioBus(channels, frames)
{
}

AudioBusRefCounted::~AudioBusRefCounted() { }

} // namespace media
