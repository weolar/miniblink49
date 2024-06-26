// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_BASE_AUDIO_RENDERER_MIXER_INPUT_H_
#define MEDIA_BASE_AUDIO_RENDERER_MIXER_INPUT_H_

#include <string>

#include "base/callback.h"
#include "media/base/audio_converter.h"
#include "media/base/audio_renderer_sink.h"
//#include "url/origin.h"

namespace media {

class AudioRendererMixer;

class MEDIA_EXPORT AudioRendererMixerInput
    : NON_EXPORTED_BASE(public RestartableAudioRendererSink),
      NON_EXPORTED_BASE(public OutputDevice),
      public AudioConverter::InputCallback {
public:
    typedef base::Callback<AudioRendererMixer*(const AudioParameters& params,
        const std::string& device_id,
        const /*url::Origin*/ std::string& security_origin,
        OutputDeviceStatus* device_status)>
        GetMixerCB;
    typedef base::Callback<void(const AudioParameters& params,
        const std::string& device_id,
        const /*url::Origin*/ std::string& security_origin)>
        RemoveMixerCB;

    AudioRendererMixerInput(const GetMixerCB& get_mixer_cb,
        const RemoveMixerCB& remove_mixer_cb,
        const std::string& device_id,
        const /*url::Origin*/ std::string& security_origin);

    // RestartableAudioRendererSink implementation.
    void Start() override;
    void Stop() override;
    void Play() override;
    void Pause() override;
    bool SetVolume(double volume) override;
    OutputDevice* GetOutputDevice() override;
    void Initialize(const AudioParameters& params,
        AudioRendererSink::RenderCallback* renderer) override;

    // OutputDevice implementation.
    void SwitchOutputDevice(const std::string& device_id,
        const /*url::Origin*/ std::string& security_origin,
        const SwitchOutputDeviceCB& callback) override;
    AudioParameters GetOutputParameters() override;
    OutputDeviceStatus GetDeviceStatus() override;

    // Called by AudioRendererMixer when an error occurs.
    void OnRenderError();

protected:
    ~AudioRendererMixerInput() override;

private:
    friend class AudioRendererMixerInputTest;

    bool initialized_;
    bool playing_;
    double volume_;

    // AudioConverter::InputCallback implementation.
    double ProvideInput(AudioBus* audio_bus,
        base::TimeDelta buffer_delay) override;

    // Callbacks provided during construction which allow AudioRendererMixerInput
    // to retrieve a mixer during Initialize() and notify when it's done with it.
    const GetMixerCB get_mixer_cb_;
    const RemoveMixerCB remove_mixer_cb_;

    // AudioParameters received during Initialize().
    AudioParameters params_;

    // ID of hardware device to use
    std::string device_id_;
    /*url::Origin*/ std::string security_origin_;

    // AudioRendererMixer provided through |get_mixer_cb_| during Initialize(),
    // guaranteed to live (at least) until |remove_mixer_cb_| is called.
    AudioRendererMixer* mixer_;

    // Source of audio data which is provided to the mixer.
    AudioRendererSink::RenderCallback* callback_;

    // Error callback for handing to AudioRendererMixer.
    const base::Closure error_cb_;

    // Pending switch-device callback, in case SwitchOutputDevice() is invoked
    // before Start()
    SwitchOutputDeviceCB pending_switch_callback_;
    std::string pending_switch_device_id_;
    /*url::Origin*/ std::string pending_switch_security_origin_;

    DISALLOW_COPY_AND_ASSIGN(AudioRendererMixerInput);
};

} // namespace media

#endif // MEDIA_BASE_AUDIO_RENDERER_MIXER_INPUT_H_
