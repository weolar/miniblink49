// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONTENT_RENDERER_MEDIA_AUDIO_RENDERER_MIXER_MANAGER_H_
#define CONTENT_RENDERER_MEDIA_AUDIO_RENDERER_MIXER_MANAGER_H_

#include <map>
#include <string>
#include <utility>

#include "base/synchronization/lock.h"
//#include "content/common/content_export.h"
#include "media/audio/audio_parameters.h"
#include "media/base/output_device.h"
//#include "url/origin.h"

namespace media {
class AudioHardwareConfig;
class AudioRendererMixer;
class AudioRendererMixerInput;
class AudioRendererSink;
}

namespace content {

// Manages sharing of an AudioRendererMixer among AudioRendererMixerInputs based
// on their AudioParameters configuration.  Inputs with the same AudioParameters
// configuration will share a mixer while a new AudioRendererMixer will be
// lazily created if one with the exact AudioParameters does not exist.
//
// There should only be one instance of AudioRendererMixerManager per render
// thread.
//
// TODO(dalecurtis): Right now we require AudioParameters to be an exact match
// when we should be able to ignore bits per channel since we're only dealing
// with floats.  However, bits per channel is currently used to interleave the
// audio data by AudioOutputDevice::AudioThreadCallback::Process for consumption
// via the shared memory.  See http://crbug.com/114700.
class AudioRendererMixerManager {
public:
    AudioRendererMixerManager();
    ~AudioRendererMixerManager();

    // Creates an AudioRendererMixerInput with the proper callbacks necessary to
    // retrieve an AudioRendererMixer instance from AudioRendererMixerManager.
    // |source_render_frame_id| refers to the RenderFrame containing the entity
    // rendering the audio.  Caller must ensure AudioRendererMixerManager outlives
    // the returned input. |device_id| and |security_origin| identify the output
    // device to use
    media::AudioRendererMixerInput* CreateInput(
        int source_render_frame_id,
        const std::string& device_id,
        const std::string& security_origin);

    // Returns a mixer instance based on AudioParameters; an existing one if one
    // with the provided AudioParameters exists or a new one if not.
    media::AudioRendererMixer* GetMixer(int source_render_frame_id,
        const media::AudioParameters& params,
        const std::string& device_id,
        const std::string& security_origin,
        media::OutputDeviceStatus* device_status);

    // Remove a mixer instance given a mixer if the only other reference is held
    // by AudioRendererMixerManager.  Every AudioRendererMixer owner must call
    // this method when it's done with a mixer.
    void RemoveMixer(int source_render_frame_id,
        const media::AudioParameters& params,
        const std::string& device_id,
        const std::string& security_origin);

private:
    friend class AudioRendererMixerManagerTest;

    // Define a key so that only those AudioRendererMixerInputs from the same
    // RenderView, AudioParameters and output device can be mixed together.
    struct MixerKey {
        MixerKey(int source_render_frame_id,
            const media::AudioParameters& params,
            const std::string& device_id,
            const std::string& security_origin);
        int source_render_frame_id;
        media::AudioParameters params;
        std::string device_id;
        std::string security_origin;
    };

    // Custom compare operator for the AudioRendererMixerMap.  Allows reuse of
    // mixers where only irrelevant keys mismatch; e.g., effects, bits per sample.
    struct MixerKeyCompare {
        bool operator()(const MixerKey& a, const MixerKey& b) const
        {
            if (a.source_render_frame_id != b.source_render_frame_id)
                return a.source_render_frame_id < b.source_render_frame_id;
            if (a.params.sample_rate() != b.params.sample_rate())
                return a.params.sample_rate() < b.params.sample_rate();
            if (a.params.channels() != b.params.channels())
                return a.params.channels() < b.params.channels();

            // Ignore effects(), bits_per_sample(), format(), and frames_per_buffer(),
            // these parameters do not affect mixer reuse.  All AudioRendererMixer
            // units disable FIFO, so frames_per_buffer() can be safely ignored.
            if (a.params.channel_layout() != b.params.channel_layout())
                return a.params.channel_layout() < b.params.channel_layout();

            if (a.device_id != b.device_id)
                return a.device_id < b.device_id;

            return a.security_origin < b.security_origin;
        }
    };

    // Map of MixerKey to <AudioRendererMixer, Count>.  Count allows
    // AudioRendererMixerManager to keep track explicitly (v.s. RefCounted which
    // is implicit) of the number of outstanding AudioRendererMixers.
    struct AudioRendererMixerReference {
        media::AudioRendererMixer* mixer;
        int ref_count;
    };
    typedef std::map<MixerKey, AudioRendererMixerReference, MixerKeyCompare>
        AudioRendererMixerMap;

    // Overrides the AudioRendererSink implementation for unit testing.
    void SetAudioRendererSinkForTesting(media::AudioRendererSink* sink);

    // Active mixers.
    AudioRendererMixerMap mixers_;
    base::Lock mixers_lock_;

    media::AudioRendererSink* sink_for_testing_;

    DISALLOW_COPY_AND_ASSIGN(AudioRendererMixerManager);
};

} // namespace content

#endif // CONTENT_RENDERER_MEDIA_AUDIO_RENDERER_MIXER_MANAGER_H_
