// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/base/audio_renderer_mixer_input.h"

#include "base/bind.h"
#include "base/callback_helpers.h"
#include "media/base/audio_renderer_mixer.h"

namespace media {

AudioRendererMixerInput::AudioRendererMixerInput(
    const GetMixerCB& get_mixer_cb,
    const RemoveMixerCB& remove_mixer_cb,
    const std::string& device_id,
    const /*url::Origin*/ std::string& security_origin)
    : initialized_(false)
    , playing_(false)
    , volume_(1.0f)
    , get_mixer_cb_(get_mixer_cb)
    , remove_mixer_cb_(remove_mixer_cb)
    , device_id_(device_id)
    , security_origin_(security_origin)
    , mixer_(NULL)
    , callback_(NULL)
    , error_cb_(base::Bind(&AudioRendererMixerInput::OnRenderError,
          base::Unretained(this)))
{
}

AudioRendererMixerInput::~AudioRendererMixerInput()
{
    DCHECK(!mixer_);
}

void AudioRendererMixerInput::Initialize(
    const AudioParameters& params,
    AudioRendererSink::RenderCallback* callback)
{
    DCHECK(!mixer_);
    DCHECK(callback);

    params_ = params;
    callback_ = callback;
    initialized_ = true;
}

void AudioRendererMixerInput::Start()
{
    DCHECK(initialized_);
    DCHECK(!mixer_);
    mixer_ = get_mixer_cb_.Run(params_, device_id_, security_origin_, nullptr);
    if (!mixer_) {
        callback_->OnRenderError();
        return;
    }

    // Note: OnRenderError() may be called immediately after this call returns.
    mixer_->AddErrorCallback(error_cb_);

    if (!pending_switch_callback_.is_null()) {
        SwitchOutputDevice(pending_switch_device_id_,
            pending_switch_security_origin_,
            base::ResetAndReturn(&pending_switch_callback_));
    }
}

void AudioRendererMixerInput::Stop()
{
    // Stop() may be called at any time, if Pause() hasn't been called we need to
    // remove our mixer input before shutdown.
    if (playing_) {
        mixer_->RemoveMixerInput(this);
        playing_ = false;
    }

    if (mixer_) {
        // TODO(dalecurtis): This is required so that |callback_| isn't called after
        // Stop() by an error event since it may outlive this ref-counted object. We
        // should instead have sane ownership semantics: http://crbug.com/151051
        mixer_->RemoveErrorCallback(error_cb_);
        remove_mixer_cb_.Run(params_, device_id_, security_origin_);
        mixer_ = NULL;
    }

    if (!pending_switch_callback_.is_null()) {
        base::ResetAndReturn(&pending_switch_callback_)
            .Run(OUTPUT_DEVICE_STATUS_ERROR_INTERNAL);
    }
}

void AudioRendererMixerInput::Play()
{
    if (playing_ || !mixer_)
        return;

    mixer_->AddMixerInput(this);
    playing_ = true;
}

void AudioRendererMixerInput::Pause()
{
    if (!playing_ || !mixer_)
        return;

    mixer_->RemoveMixerInput(this);
    playing_ = false;
}

bool AudioRendererMixerInput::SetVolume(double volume)
{
    volume_ = volume;
    return true;
}

OutputDevice* AudioRendererMixerInput::GetOutputDevice()
{
    return this;
}

void AudioRendererMixerInput::SwitchOutputDevice(
    const std::string& device_id,
    const /*url::Origin*/ std::string& security_origin,
    const SwitchOutputDeviceCB& callback)
{
    if (!mixer_) {
        if (pending_switch_callback_.is_null()) {
            pending_switch_callback_ = callback;
            pending_switch_device_id_ = device_id;
            pending_switch_security_origin_ = security_origin;
        } else {
            callback.Run(OUTPUT_DEVICE_STATUS_ERROR_INTERNAL);
        }

        return;
    }

    DCHECK(pending_switch_callback_.is_null());
    if (device_id == device_id_) {
        callback.Run(OUTPUT_DEVICE_STATUS_OK);
        return;
    }

    OutputDeviceStatus new_mixer_status = OUTPUT_DEVICE_STATUS_ERROR_INTERNAL;
    AudioRendererMixer* new_mixer = get_mixer_cb_.Run(params_, device_id, security_origin, &new_mixer_status);
    if (new_mixer_status != OUTPUT_DEVICE_STATUS_OK) {
        callback.Run(new_mixer_status);
        return;
    }

    bool was_playing = playing_;
    Stop();
    device_id_ = device_id;
    security_origin_ = security_origin;
    mixer_ = new_mixer;
    mixer_->AddErrorCallback(error_cb_);

    if (was_playing)
        Play();

    callback.Run(OUTPUT_DEVICE_STATUS_OK);
}

AudioParameters AudioRendererMixerInput::GetOutputParameters()
{
    return mixer_->GetOutputDevice()->GetOutputParameters();
}

OutputDeviceStatus AudioRendererMixerInput::GetDeviceStatus()
{
    if (!mixer_)
        return OUTPUT_DEVICE_STATUS_ERROR_INTERNAL;

    return mixer_->GetOutputDevice()->GetDeviceStatus();
}

double AudioRendererMixerInput::ProvideInput(AudioBus* audio_bus,
    base::TimeDelta buffer_delay)
{
    int frames_filled = callback_->Render(
        audio_bus, static_cast<int>(buffer_delay.InMillisecondsF() + 0.5));

    // AudioConverter expects unfilled frames to be zeroed.
    if (frames_filled < audio_bus->frames()) {
        audio_bus->ZeroFramesPartial(
            frames_filled, audio_bus->frames() - frames_filled);
    }

    return frames_filled > 0 ? volume_ : 0;
}

void AudioRendererMixerInput::OnRenderError()
{
    callback_->OnRenderError();
}

} // namespace media
