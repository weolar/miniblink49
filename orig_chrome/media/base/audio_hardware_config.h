// Copyright (c) 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_BASE_AUDIO_HARDWARE_CONFIG_H_
#define MEDIA_BASE_AUDIO_HARDWARE_CONFIG_H_

#include "base/compiler_specific.h"
#include "base/synchronization/lock.h"
#include "media/audio/audio_parameters.h"
#include "media/base/channel_layout.h"
#include "media/base/media_export.h"

namespace media {

// Provides thread safe access to the audio hardware configuration.
class MEDIA_EXPORT AudioHardwareConfig {
public:
    AudioHardwareConfig(const media::AudioParameters& input_params,
        const media::AudioParameters& output_params);
    virtual ~AudioHardwareConfig();

    // Accessors for the currently cached hardware configuration.  Safe to call
    // from any thread.
    int GetOutputBufferSize() const;
    int GetOutputSampleRate() const;
    ChannelLayout GetOutputChannelLayout() const;
    int GetOutputChannels() const;

    int GetInputSampleRate() const;
    ChannelLayout GetInputChannelLayout() const;
    int GetInputChannels() const;

    media::AudioParameters GetInputConfig() const;
    media::AudioParameters GetOutputConfig() const;

    // Allows callers to update the cached values for either input or output.  The
    // values are paired under the assumption that these values will only be set
    // after an input or output device change respectively.  Safe to call from
    // any thread.
    void UpdateInputConfig(const media::AudioParameters& input_params);
    void UpdateOutputConfig(const media::AudioParameters& output_params);

    // For clients which don't need low latency, a larger buffer size should be
    // used to save power and CPU resources.
    int GetHighLatencyBufferSize() const;
    static int GetHighLatencyBufferSize(
        const media::AudioParameters& output_params);

private:
    // Cached values; access is protected by |config_lock_|.
    mutable base::Lock config_lock_;
    media::AudioParameters input_params_;
    media::AudioParameters output_params_;

    DISALLOW_COPY_AND_ASSIGN(AudioHardwareConfig);
};

} // namespace media

#endif // MEDIA_BASE_AUDIO_HARDWARE_CONFIG_H_
