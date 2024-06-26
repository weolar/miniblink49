// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_BASE_AUDIO_BUS_H_
#define MEDIA_BASE_AUDIO_BUS_H_

#include <vector>

#include "base/memory/aligned_memory.h"
#include "base/memory/ref_counted.h"
#include "base/memory/scoped_ptr.h"
#include "media/base/media_export.h"

namespace media {
class AudioParameters;

// Scoped container for "busing" audio channel data around.  Each channel is
// stored in planar format and guaranteed to be aligned by kChannelAlignment.
// AudioBus objects can be created normally or via wrapping.  Normally, AudioBus
// will dice up a contiguous memory block for channel data.  When wrapped,
// AudioBus instead routes requests for channel data to the wrapped object.
class MEDIA_EXPORT AudioBus {
public:
    // Guaranteed alignment of each channel's data; use 16-byte alignment for easy
    // SSE optimizations.
    enum { kChannelAlignment = 16 };

    // Creates a new AudioBus and allocates |channels| of length |frames|.  Uses
    // channels() and frames_per_buffer() from AudioParameters if given.
    static scoped_ptr<AudioBus> Create(int channels, int frames);
    static scoped_ptr<AudioBus> Create(const AudioParameters& params);

    // Creates a new AudioBus with the given number of channels, but zero length.
    // It's expected to be used with SetChannelData() and set_frames() to
    // wrap externally allocated memory.
    static scoped_ptr<AudioBus> CreateWrapper(int channels);

    // Creates a new AudioBus from an existing channel vector.  Does not transfer
    // ownership of |channel_data| to AudioBus; i.e., |channel_data| must outlive
    // the returned AudioBus.  Each channel must be aligned by kChannelAlignment.
    static scoped_ptr<AudioBus> WrapVector(
        int frames, const std::vector<float*>& channel_data);

    // Creates a new AudioBus by wrapping an existing block of memory.  Block must
    // be at least CalculateMemorySize() bytes in size.  |data| must outlive the
    // returned AudioBus.  |data| must be aligned by kChannelAlignment.
    static scoped_ptr<AudioBus> WrapMemory(int channels, int frames, void* data);
    static scoped_ptr<AudioBus> WrapMemory(const AudioParameters& params,
        void* data);
    static int CalculateMemorySize(const AudioParameters& params);

    // Calculates the required size for an AudioBus given the number of channels
    // and frames.
    static int CalculateMemorySize(int channels, int frames);

    // Helper methods for converting an AudioBus from and to interleaved integer
    // data.  Expects interleaving to be [ch0, ch1, ..., chN, ch0, ch1, ...] with
    // |bytes_per_sample| per value.  Values are scaled and bias corrected during
    // conversion.  ToInterleaved() will also clip values to format range.
    // Handles uint8, int16, and int32 currently.  FromInterleaved() will zero out
    // any unfilled frames when |frames| is less than frames().
    void FromInterleaved(const void* source, int frames, int bytes_per_sample);
    void ToInterleaved(int frames, int bytes_per_sample, void* dest) const;
    void ToInterleavedPartial(int start_frame, int frames, int bytes_per_sample,
        void* dest) const;

    // Similar to FromInterleaved() above, but meant for streaming sources.  Does
    // not zero out remaining frames, the caller is responsible for doing so using
    // ZeroFramesPartial().  Frames are deinterleaved from the start of |source|
    // to channel(x)[start_frame].
    void FromInterleavedPartial(const void* source, int start_frame, int frames,
        int bytes_per_sample);

    // Helper method for copying channel data from one AudioBus to another.  Both
    // AudioBus object must have the same frames() and channels().
    void CopyTo(AudioBus* dest) const;

    // Helper method to copy frames from one AudioBus to another. Both AudioBus
    // objects must have the same number of channels(). |source_start_frame| is
    // the starting offset. |dest_start_frame| is the starting offset in |dest|.
    // |frame_count| is the number of frames to copy.
    void CopyPartialFramesTo(int source_start_frame,
        int frame_count,
        int dest_start_frame,
        AudioBus* dest) const;

    // Returns a raw pointer to the requested channel.  Pointer is guaranteed to
    // have a 16-byte alignment.  Warning: Do not rely on having sane (i.e. not
    // inf, nan, or between [-1.0, 1.0]) values in the channel data.
    float* channel(int channel) { return channel_data_[channel]; }
    const float* channel(int channel) const { return channel_data_[channel]; }
    void SetChannelData(int channel, float* data);

    int channels() const { return static_cast<int>(channel_data_.size()); }
    int frames() const { return frames_; }
    void set_frames(int frames);

    // Helper method for zeroing out all channels of audio data.
    void Zero();
    void ZeroFrames(int frames);
    void ZeroFramesPartial(int start_frame, int frames);

    // Checks if all frames are zero.
    bool AreFramesZero() const;

    // Scale internal channel values by |volume| >= 0.  If an invalid value
    // is provided, no adjustment is done.
    void Scale(float volume);

    // Swaps channels identified by |a| and |b|.  The caller needs to make sure
    // the channels are valid.
    void SwapChannels(int a, int b);

    virtual ~AudioBus();

protected:
    AudioBus(int channels, int frames);
    AudioBus(int channels, int frames, float* data);
    AudioBus(int frames, const std::vector<float*>& channel_data);
    explicit AudioBus(int channels);

private:
    // Helper method for building |channel_data_| from a block of memory.  |data|
    // must be at least BlockSize() bytes in size.
    void BuildChannelData(int channels, int aligned_frame, float* data);

    // Contiguous block of channel memory.
    scoped_ptr<float, base::AlignedFreeDeleter> data_;

    std::vector<float*> channel_data_;
    int frames_;

    // Protect SetChannelData() and set_frames() for use by CreateWrapper().
    bool can_set_channel_data_;

    DISALLOW_COPY_AND_ASSIGN(AudioBus);
};

// RefCounted version of AudioBus. This is not meant for general use. Only use
// this when your lifetime requirements make it impossible to use an
// AudioBus scoped_ptr.
class MEDIA_EXPORT AudioBusRefCounted
    : public media::AudioBus,
      public base::RefCountedThreadSafe<AudioBusRefCounted> {
public:
    static scoped_refptr<AudioBusRefCounted> Create(int channels, int frames);

private:
    friend class base::RefCountedThreadSafe<AudioBusRefCounted>;

    AudioBusRefCounted(int channels, int frames);
    ~AudioBusRefCounted() override;

    DISALLOW_COPY_AND_ASSIGN(AudioBusRefCounted);
};

} // namespace media

#endif // MEDIA_BASE_AUDIO_BUS_H_
