// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_AUDIO_AUDIO_PARAMETERS_H_
#define MEDIA_AUDIO_AUDIO_PARAMETERS_H_

#include <stdint.h>
#include <string>

#include "base/basictypes.h"
#include "base/compiler_specific.h"
#include "base/logging.h"
#include "base/time/time.h"
#include "media/audio/point.h"
#include "media/base/audio_bus.h"
#include "media/base/channel_layout.h"
#include "media/base/media_export.h"

namespace media {

// Use a struct-in-struct approach to ensure that we can calculate the required
// size as sizeof(AudioInputBufferParameters) + #(bytes in audio buffer) without
// using packing. Also align AudioInputBufferParameters instead of in
// AudioInputBuffer to be able to calculate size like so. Use a macro for the
// alignment value that's the same as AudioBus::kChannelAlignment, since MSVC
// doesn't accept the latter to be used.
#if defined(OS_WIN)
#pragma warning(push)
#pragma warning(disable : 4324) // Disable warning for added padding.
#endif
#define PARAMETERS_ALIGNMENT 16
COMPILE_ASSERT(AudioBus::kChannelAlignment == PARAMETERS_ALIGNMENT,
    AudioInputBufferParameters_alignment_not_same_as_AudioBus);
struct MEDIA_EXPORT ALIGNAS(PARAMETERS_ALIGNMENT) AudioInputBufferParameters {
    double volume;
    uint32 size;
    uint32_t hardware_delay_bytes;
    uint32_t id;
    bool key_pressed;
};
#undef PARAMETERS_ALIGNMENT
#if defined(OS_WIN)
#pragma warning(pop)
#endif

COMPILE_ASSERT(
    sizeof(AudioInputBufferParameters) % AudioBus::kChannelAlignment == 0,
    AudioInputBufferParameters_not_aligned);

struct MEDIA_EXPORT AudioInputBuffer {
    AudioInputBufferParameters params;
    int8 audio[1];
};

class MEDIA_EXPORT AudioParameters {
public:
    // TODO(miu): Rename this enum to something that correctly reflects its
    // semantics, such as "TransportScheme."
    enum Format {
        AUDIO_PCM_LINEAR = 0, // PCM is 'raw' amplitude samples.
        AUDIO_PCM_LOW_LATENCY, // Linear PCM, low latency requested.
        AUDIO_FAKE, // Creates a fake AudioOutputStream object.
        AUDIO_FORMAT_LAST = AUDIO_FAKE, // Only used for validation of format.
    };

    enum {
        // Telephone quality sample rate, mostly for speech-only audio.
        kTelephoneSampleRate = 8000,
        // CD sampling rate is 44.1 KHz or conveniently 2x2x3x3x5x5x7x7.
        kAudioCDSampleRate = 44100,
    };

    // Bitmasks to determine whether certain platform (typically hardware) audio
    // effects should be enabled.
    enum PlatformEffectsMask {
        NO_EFFECTS = 0x0,
        ECHO_CANCELLER = 0x1,
        DUCKING = 0x2, // Enables ducking if the OS supports it.
        KEYBOARD_MIC = 0x4,
        HOTWORD = 0x8,
    };

    AudioParameters();
    AudioParameters(Format format,
        ChannelLayout channel_layout,
        int sample_rate,
        int bits_per_sample,
        int frames_per_buffer);

    ~AudioParameters();

    // Re-initializes all members.
    void Reset(Format format,
        ChannelLayout channel_layout,
        int sample_rate,
        int bits_per_sample,
        int frames_per_buffer);

    // Checks that all values are in the expected range. All limits are specified
    // in media::Limits.
    bool IsValid() const;

    // Returns a human-readable string describing |*this|.  For debugging & test
    // output only.
    std::string AsHumanReadableString() const;

    // Returns size of audio buffer in bytes.
    int GetBytesPerBuffer() const;

    // Returns the number of bytes representing one second of audio.
    int GetBytesPerSecond() const;

    // Returns the number of bytes representing a frame of audio.
    int GetBytesPerFrame() const;

    // Returns the duration of this buffer as calculated from frames_per_buffer()
    // and sample_rate().
    base::TimeDelta GetBufferDuration() const;

    // Comparison with other AudioParams.
    bool Equals(const AudioParameters& other) const;

    void set_format(Format format) { format_ = format; }
    Format format() const { return format_; }

    // A setter for channel_layout_ is intentionally excluded.
    ChannelLayout channel_layout() const { return channel_layout_; }

    // The number of channels is usually computed from channel_layout_. Setting
    // this explictly is only required with CHANNEL_LAYOUT_DISCRETE.
    void set_channels_for_discrete(int channels)
    {
        DCHECK(channel_layout_ == CHANNEL_LAYOUT_DISCRETE || channels == ChannelLayoutToChannelCount(channel_layout_));
        channels_ = channels;
    }
    int channels() const { return channels_; }

    void set_sample_rate(int sample_rate) { sample_rate_ = sample_rate; }
    int sample_rate() const { return sample_rate_; }

    void set_bits_per_sample(int bits_per_sample)
    {
        bits_per_sample_ = bits_per_sample;
    }
    int bits_per_sample() const { return bits_per_sample_; }

    void set_frames_per_buffer(int frames_per_buffer)
    {
        frames_per_buffer_ = frames_per_buffer;
    }
    int frames_per_buffer() const { return frames_per_buffer_; }

    void set_effects(int effects) { effects_ = effects; }
    int effects() const { return effects_; }

    void set_mic_positions(const std::vector<Point>& mic_positions)
    {
        mic_positions_ = mic_positions;
    }
    const std::vector<Point>& mic_positions() const { return mic_positions_; }

    AudioParameters(const AudioParameters&);
    AudioParameters& operator=(const AudioParameters&);

private:
    Format format_; // Format of the stream.
    ChannelLayout channel_layout_; // Order of surround sound channels.
    int channels_; // Number of channels. Value set based on
        // |channel_layout|.
    int sample_rate_; // Sampling frequency/rate.
    int bits_per_sample_; // Number of bits per sample.
    int frames_per_buffer_; // Number of frames in a buffer.
    int effects_; // Bitmask using PlatformEffectsMask.

    // Microphone positions using Cartesian coordinates:
    // x: the horizontal dimension, with positive to the right from the camera's
    //    perspective.
    // y: the depth dimension, with positive forward from the camera's
    //    perspective.
    // z: the vertical dimension, with positive upwards.
    //
    // Usually, the center of the microphone array will be treated as the origin
    // (often the position of the camera).
    //
    // An empty vector indicates unknown positions.
    std::vector<Point> mic_positions_;
};

// Comparison is useful when AudioParameters is used with std structures.
inline bool operator<(const AudioParameters& a, const AudioParameters& b)
{
    if (a.format() != b.format())
        return a.format() < b.format();
    if (a.channels() != b.channels())
        return a.channels() < b.channels();
    if (a.sample_rate() != b.sample_rate())
        return a.sample_rate() < b.sample_rate();
    if (a.bits_per_sample() != b.bits_per_sample())
        return a.bits_per_sample() < b.bits_per_sample();
    return a.frames_per_buffer() < b.frames_per_buffer();
}

} // namespace media

#endif // MEDIA_AUDIO_AUDIO_PARAMETERS_H_
