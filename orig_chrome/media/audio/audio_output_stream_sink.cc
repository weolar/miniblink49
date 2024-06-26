// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/audio/audio_output_stream_sink.h"

#include "base/bind.h"
#include "base/bind_helpers.h"
#include "base/location.h"
#include "media/audio/audio_manager.h"

namespace media {

AudioOutputStreamSink::AudioOutputStreamSink()
    : initialized_(false)
    , started_(false)
    , render_callback_(NULL)
    , active_render_callback_(NULL)
    , audio_task_runner_(AudioManager::Get()->GetTaskRunner())
    , stream_(NULL)
{
}

AudioOutputStreamSink::~AudioOutputStreamSink()
{
}

void AudioOutputStreamSink::Initialize(const AudioParameters& params,
    RenderCallback* callback)
{
    DCHECK(callback);
    DCHECK(!started_);
    params_ = params;
    render_callback_ = callback;
    initialized_ = true;
}

void AudioOutputStreamSink::Start()
{
    DCHECK(initialized_);
    DCHECK(!started_);
    {
        base::AutoLock al(callback_lock_);
        active_render_callback_ = render_callback_;
    }
    started_ = true;
    audio_task_runner_->PostTask(
        FROM_HERE, base::Bind(&AudioOutputStreamSink::DoStart, this, params_));
}

void AudioOutputStreamSink::Stop()
{
    ClearCallback();
    started_ = false;
    audio_task_runner_->PostTask(
        FROM_HERE, base::Bind(&AudioOutputStreamSink::DoStop, this));
}

void AudioOutputStreamSink::Pause()
{
    ClearCallback();
    audio_task_runner_->PostTask(
        FROM_HERE, base::Bind(&AudioOutputStreamSink::DoPause, this));
}

void AudioOutputStreamSink::Play()
{
    {
        base::AutoLock al(callback_lock_);
        active_render_callback_ = render_callback_;
    }
    audio_task_runner_->PostTask(
        FROM_HERE, base::Bind(&AudioOutputStreamSink::DoPlay, this));
}

bool AudioOutputStreamSink::SetVolume(double volume)
{
    audio_task_runner_->PostTask(
        FROM_HERE, base::Bind(&AudioOutputStreamSink::DoSetVolume, this, volume));
    return true;
}

OutputDevice* AudioOutputStreamSink::GetOutputDevice()
{
    return nullptr;
}

int AudioOutputStreamSink::OnMoreData(AudioBus* dest,
    uint32 total_bytes_delay)
{
    // Note: Runs on the audio thread created by the OS.
    base::AutoLock al(callback_lock_);
    if (!active_render_callback_)
        return 0;

    return active_render_callback_->Render(
        dest, total_bytes_delay * 1000.0 / active_params_.GetBytesPerSecond());
}

void AudioOutputStreamSink::OnError(AudioOutputStream* stream)
{
    // Note: Runs on the audio thread created by the OS.
    base::AutoLock al(callback_lock_);
    if (active_render_callback_)
        active_render_callback_->OnRenderError();
}

void AudioOutputStreamSink::DoStart(const AudioParameters& params)
{
    DCHECK(audio_task_runner_->BelongsToCurrentThread());

    // Create an AudioOutputStreamProxy which will handle any and all resampling
    // necessary to generate a low latency output stream.
    active_params_ = params;
    stream_ = AudioManager::Get()->MakeAudioOutputStreamProxy(active_params_,
        std::string());
    if (!stream_ || !stream_->Open()) {
        {
            base::AutoLock al(callback_lock_);
            if (active_render_callback_)
                active_render_callback_->OnRenderError();
        }
        if (stream_)
            stream_->Close();
        stream_ = NULL;
    }
}

void AudioOutputStreamSink::DoStop()
{
    DCHECK(audio_task_runner_->BelongsToCurrentThread());

    if (!stream_)
        return;

    DoPause();
    stream_->Close();
    stream_ = NULL;
}

void AudioOutputStreamSink::DoPause()
{
    DCHECK(audio_task_runner_->BelongsToCurrentThread());
    stream_->Stop();
}

void AudioOutputStreamSink::DoPlay()
{
    DCHECK(audio_task_runner_->BelongsToCurrentThread());
    stream_->Start(this);
}

void AudioOutputStreamSink::DoSetVolume(double volume)
{
    DCHECK(audio_task_runner_->BelongsToCurrentThread());
    stream_->SetVolume(volume);
}

void AudioOutputStreamSink::ClearCallback()
{
    base::AutoLock al(callback_lock_);
    active_render_callback_ = NULL;
}

} // namepace media
