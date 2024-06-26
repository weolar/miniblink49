// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/media/audio_renderer_mixer_manager.h"

#include <string>

#include "base/bind.h"
#include "base/bind_helpers.h"
#include "content/media/audio_device_factory.h"
#include "media/audio/audio_output_device.h"
#include "media/base/audio_hardware_config.h"
#include "media/base/audio_renderer_mixer.h"
#include "media/base/audio_renderer_mixer_input.h"

namespace content {

AudioRendererMixerManager::AudioRendererMixerManager()
    : sink_for_testing_(nullptr)
{
}

AudioRendererMixerManager::~AudioRendererMixerManager()
{
    // References to AudioRendererMixers may be owned by garbage collected
    // objects.  During process shutdown they may be leaked, so, transitively,
    // |mixers_| may leak (i.e., may be non-empty at this time) as well.
}

media::AudioRendererMixerInput* AudioRendererMixerManager::CreateInput(
    int source_render_frame_id,
    const std::string& device_id,
    const /*url::Origin*/ std::string& security_origin)
{
    return new media::AudioRendererMixerInput(base::Bind(&AudioRendererMixerManager::GetMixer, base::Unretained(this), source_render_frame_id),
        base::Bind(&AudioRendererMixerManager::RemoveMixer, base::Unretained(this), source_render_frame_id), device_id, security_origin);
}

void AudioRendererMixerManager::SetAudioRendererSinkForTesting(
    media::AudioRendererSink* sink)
{
    sink_for_testing_ = sink;
}

media::AudioRendererMixer* AudioRendererMixerManager::GetMixer(
    int source_render_frame_id,
    const media::AudioParameters& params,
    const std::string& device_id,
    const std::string& security_origin,
    media::OutputDeviceStatus* device_status)
{
    // Effects are not passed through to output creation, so ensure none are set.
    DCHECK_EQ(params.effects(), media::AudioParameters::NO_EFFECTS);

    const MixerKey key(source_render_frame_id, params, device_id, security_origin);
    base::AutoLock auto_lock(mixers_lock_);

    AudioRendererMixerMap::iterator it = mixers_.find(key);
    if (it != mixers_.end()) {
        if (device_status)
            *device_status = media::OUTPUT_DEVICE_STATUS_OK;

        it->second.ref_count++;
        return it->second.mixer;
    }

    scoped_refptr<media::AudioRendererSink> sink = sink_for_testing_
        ? sink_for_testing_
        : AudioDeviceFactory::NewOutputDevice(source_render_frame_id, 0,
            device_id, security_origin)
              .get();

    media::OutputDeviceStatus new_device_status = sink->GetOutputDevice()->GetDeviceStatus();
    if (device_status)
        *device_status = new_device_status;
    if (new_device_status != media::OUTPUT_DEVICE_STATUS_OK) {
        sink->Stop();
        return nullptr;
    }

    media::AudioParameters hardware_params = sink->GetOutputDevice()->GetOutputParameters();

    // On ChromeOS we can rely on the playback device to handle resampling, so
    // don't waste cycles on it here.
#if defined(OS_CHROMEOS)
    int sample_rate = params.sample_rate();
#else
    int sample_rate = hardware_params.format() != media::AudioParameters::AUDIO_FAKE
        ? hardware_params.sample_rate()
        : params.sample_rate();
#endif

    int buffer_size = hardware_params.format() != media::AudioParameters::AUDIO_FAKE
        ? media::AudioHardwareConfig::GetHighLatencyBufferSize(hardware_params)
        : params.frames_per_buffer();

    // Create output parameters based on the audio hardware configuration for
    // passing on to the output sink.  Force to 16-bit output for now since we
    // know that works everywhere; ChromeOS does not support other bit depths.
    media::AudioParameters output_params(media::AudioParameters::AUDIO_PCM_LOW_LATENCY, params.channel_layout(), sample_rate, 16, buffer_size);

    // If we've created invalid output parameters, simply pass on the input
    // params and let the browser side handle automatic fallback.
    if (!output_params.IsValid())
        output_params = params;

    media::AudioRendererMixer* mixer = new media::AudioRendererMixer(params, output_params, sink);
    AudioRendererMixerReference mixer_reference = { mixer, 1 };
    mixers_[key] = mixer_reference;
    return mixer;
}

void AudioRendererMixerManager::RemoveMixer(
    int source_render_frame_id,
    const media::AudioParameters& params,
    const std::string& device_id,
    const std::string& security_origin)
{
    const MixerKey key(source_render_frame_id, params, device_id, security_origin);
    base::AutoLock auto_lock(mixers_lock_);

    AudioRendererMixerMap::iterator it = mixers_.find(key);
    DCHECK(it != mixers_.end());

    // Only remove the mixer if AudioRendererMixerManager is the last owner.
    it->second.ref_count--;
    if (it->second.ref_count == 0) {
        delete it->second.mixer;
        mixers_.erase(it);
    }
}

AudioRendererMixerManager::MixerKey::MixerKey(
    int source_render_frame_id,
    const media::AudioParameters& params,
    const std::string& device_id,
    const std::string& security_origin)
    : source_render_frame_id(source_render_frame_id)
    , params(params)
    , device_id(device_id)
    , security_origin(security_origin)
{
}

} // namespace content
