// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/base/audio_renderer_mixer.h"

#include "base/bind.h"
#include "base/bind_helpers.h"
#include "base/logging.h"

namespace media {

enum { kPauseDelaySeconds = 10 };

AudioRendererMixer::AudioRendererMixer(
    const AudioParameters& input_params, const AudioParameters& output_params,
    const scoped_refptr<AudioRendererSink>& sink)
    : audio_sink_(sink)
    , audio_converter_(input_params, output_params, true)
    , pause_delay_(base::TimeDelta::FromSeconds(kPauseDelaySeconds))
    , last_play_time_(base::TimeTicks::Now())
    ,
    // Initialize |playing_| to true since Start() results in an auto-play.
    playing_(true)
{
    audio_sink_->Initialize(output_params, this);
    audio_sink_->Start();
}

AudioRendererMixer::~AudioRendererMixer()
{
    // AudioRendererSinks must be stopped before being destructed.
    audio_sink_->Stop();

    // Ensure that all mixer inputs have removed themselves prior to destruction.
    DCHECK(audio_converter_.empty());
    DCHECK_EQ(error_callbacks_.size(), 0U);
}

void AudioRendererMixer::AddMixerInput(AudioConverter::InputCallback* input)
{
    base::AutoLock auto_lock(lock_);
    if (!playing_) {
        playing_ = true;
        last_play_time_ = base::TimeTicks::Now();
        audio_sink_->Play();
    }

    audio_converter_.AddInput(input);
}

void AudioRendererMixer::RemoveMixerInput(
    AudioConverter::InputCallback* input)
{
    base::AutoLock auto_lock(lock_);
    audio_converter_.RemoveInput(input);
}

void AudioRendererMixer::AddErrorCallback(const base::Closure& error_cb)
{
    base::AutoLock auto_lock(lock_);
    error_callbacks_.push_back(error_cb);
}

void AudioRendererMixer::RemoveErrorCallback(const base::Closure& error_cb)
{
    base::AutoLock auto_lock(lock_);
    for (ErrorCallbackList::iterator it = error_callbacks_.begin();
         it != error_callbacks_.end();
         ++it) {
        if (it->Equals(error_cb)) {
            error_callbacks_.erase(it);
            return;
        }
    }

    // An error callback should always exist when called.
    NOTREACHED();
}

OutputDevice* AudioRendererMixer::GetOutputDevice()
{
    DVLOG(1) << __FUNCTION__;
    base::AutoLock auto_lock(lock_);
    return audio_sink_->GetOutputDevice();
}

int AudioRendererMixer::Render(AudioBus* audio_bus,
    int audio_delay_milliseconds)
{
    base::AutoLock auto_lock(lock_);

    // If there are no mixer inputs and we haven't seen one for a while, pause the
    // sink to avoid wasting resources when media elements are present but remain
    // in the pause state.
    const base::TimeTicks now = base::TimeTicks::Now();
    if (!audio_converter_.empty()) {
        last_play_time_ = now;
    } else if (now - last_play_time_ >= pause_delay_ && playing_) {
        audio_sink_->Pause();
        playing_ = false;
    }

    audio_converter_.ConvertWithDelay(
        base::TimeDelta::FromMilliseconds(audio_delay_milliseconds), audio_bus);
    return audio_bus->frames();
}

void AudioRendererMixer::OnRenderError()
{
    // Call each mixer input and signal an error.
    base::AutoLock auto_lock(lock_);
    for (const auto& cb : error_callbacks_)
        cb.Run();
}

} // namespace media
