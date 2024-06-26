// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Contains limit definition constants for the media subsystem.

#ifndef MEDIA_BASE_LIMITS_H_
#define MEDIA_BASE_LIMITS_H_

#include "base/basictypes.h"

namespace media {

namespace limits {

    enum {
        // Maximum possible dimension (width or height) for any video.
        kMaxDimension = (1 << 15) - 1, // 32767

        // Maximum possible canvas size (width multiplied by height) for any video.
        kMaxCanvas = (1 << (14 * 2)), // 16384 x 16384

        // Total number of video frames which are populating in the pipeline.
        kMaxVideoFrames = 4,

        // The following limits are used by AudioParameters::IsValid().
        //
        // A few notes on sample rates of common formats:
        //   - AAC files are limited to 96 kHz.
        //   - MP3 files are limited to 48 kHz.
        //   - Vorbis used to be limited to 96 KHz, but no longer has that
        //     restriction.
        //   - Most PC audio hardware is limited to 192 KHz.
        kMaxSampleRate = 192000,
        kMinSampleRate = 3000,
        kMaxChannels = 32,
        kMaxBytesPerSample = 4,
        kMaxBitsPerSample = kMaxBytesPerSample * 8,
        kMaxSamplesPerPacket = kMaxSampleRate,
        kMaxPacketSizeInBytes = kMaxBytesPerSample * kMaxChannels * kMaxSamplesPerPacket,

        // This limit is used by ParamTraits<VideoCaptureParams>.
        kMaxFramesPerSecond = 1000,

        // Maximum lengths for various EME API parameters. These are checks to
        // prevent unnecessarily large parameters from being passed around, and the
        // lengths are somewhat arbitrary as the EME spec doesn't specify any limits.
        kMinCertificateLength = 128,
        kMaxCertificateLength = 16 * 1024,
        kMaxSessionIdLength = 512,
        kMinKeyIdLength = 1,
        kMaxKeyIdLength = 512,
        kMaxKeyIds = 128,
        kMaxInitDataLength = 64 * 1024, // 64 KB
        kMaxSessionResponseLength = 64 * 1024, // 64 KB
        kMaxKeySystemLength = 256,
    };

} // namespace limits

} // namespace media

#endif // MEDIA_BASE_LIMITS_H_
