// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_BASE_AUDIO_DECODER_CONFIG_H_
#define MEDIA_BASE_AUDIO_DECODER_CONFIG_H_

#include <string>
#include <vector>

#include "base/basictypes.h"
#include "base/time/time.h"
#include "media/base/channel_layout.h"
#include "media/base/media_export.h"
#include "media/base/sample_format.h"

namespace media {

enum AudioCodec {
    // These values are histogrammed over time; do not change their ordinal
    // values.  When deleting a codec replace it with a dummy value; when adding a
    // codec, do so at the bottom before kAudioCodecMax, and update the value of
    // kAudioCodecMax to equal the new codec.
    kUnknownAudioCodec = 0,
    kCodecAAC = 1,
    kCodecMP3 = 2,
    kCodecPCM = 3,
    kCodecVorbis = 4,
    kCodecFLAC = 5,
    kCodecAMR_NB = 6,
    kCodecAMR_WB = 7,
    kCodecPCM_MULAW = 8,
    kCodecGSM_MS = 9,
    kCodecPCM_S16BE = 10,
    kCodecPCM_S24BE = 11,
    kCodecOpus = 12,
    // kCodecEAC3 = 13,
    kCodecPCM_ALAW = 14,
    kCodecALAC = 15,
    // DO NOT ADD RANDOM AUDIO CODECS!
    //
    // The only acceptable time to add a new codec is if there is production code
    // that uses said codec in the same CL.

    // Must always be equal to the largest entry ever logged.
    kAudioCodecMax = kCodecALAC,
};

// TODO(dalecurtis): FFmpeg API uses |bytes_per_channel| instead of
// |bits_per_channel|, we should switch over since bits are generally confusing
// to work with.
class MEDIA_EXPORT AudioDecoderConfig {
public:
    // Constructs an uninitialized object. Clients should call Initialize() with
    // appropriate values before using.
    AudioDecoderConfig();

    // Constructs an initialized object.
    AudioDecoderConfig(AudioCodec codec,
        SampleFormat sample_format,
        ChannelLayout channel_layout,
        int samples_per_second,
        const std::vector<uint8_t>& extra_data,
        bool is_encrypted);

    ~AudioDecoderConfig();

    // Resets the internal state of this object. |codec_delay| is in frames.
    void Initialize(AudioCodec codec,
        SampleFormat sample_format,
        ChannelLayout channel_layout,
        int samples_per_second,
        const std::vector<uint8>& extra_data,
        bool is_encrypted,
        base::TimeDelta seek_preroll,
        int codec_delay);

    // Returns true if this object has appropriate configuration values, false
    // otherwise.
    bool IsValidConfig() const;

    // Returns true if all fields in |config| match this config.
    // Note: The contents of |extra_data_| are compared not the raw pointers.
    bool Matches(const AudioDecoderConfig& config) const;

    // Returns a human-readable string describing |*this|.  For debugging & test
    // output only.
    std::string AsHumanReadableString() const;

    std::string GetHumanReadableCodecName() const;

    AudioCodec codec() const { return codec_; }
    int bits_per_channel() const { return bytes_per_channel_ * 8; }
    int bytes_per_channel() const { return bytes_per_channel_; }
    ChannelLayout channel_layout() const { return channel_layout_; }
    int samples_per_second() const { return samples_per_second_; }
    SampleFormat sample_format() const { return sample_format_; }
    int bytes_per_frame() const { return bytes_per_frame_; }
    base::TimeDelta seek_preroll() const { return seek_preroll_; }
    int codec_delay() const { return codec_delay_; }

    // Optional byte data required to initialize audio decoders such as Vorbis
    // codebooks.
    const std::vector<uint8_t>& extra_data() const { return extra_data_; }

    // Whether the audio stream is potentially encrypted.
    // Note that in a potentially encrypted audio stream, individual buffers
    // can be encrypted or not encrypted.
    bool is_encrypted() const { return is_encrypted_; }

private:
    AudioCodec codec_;
    SampleFormat sample_format_;
    int bytes_per_channel_;
    ChannelLayout channel_layout_;
    int samples_per_second_;
    int bytes_per_frame_;
    std::vector<uint8_t> extra_data_;
    bool is_encrypted_;

    // |seek_preroll_| is the duration of the data that the decoder must decode
    // before the decoded data is valid.
    base::TimeDelta seek_preroll_;

    // |codec_delay_| is the number of frames the decoder should discard before
    // returning decoded data.  This value can include both decoder delay as well
    // as padding added during encoding.
    int codec_delay_;

    // Not using DISALLOW_COPY_AND_ASSIGN here intentionally to allow the compiler
    // generated copy constructor and assignment operator. Since the extra data is
    // typically small, the performance impact is minimal.
};

} // namespace media

#endif // MEDIA_BASE_AUDIO_DECODER_CONFIG_H_
