// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/audio/fake_audio_input_stream.h"

#include "base/bind.h"
#include "base/bind_helpers.h"
#include "base/command_line.h"
#include "base/files/file_path.h"
#include "base/single_thread_task_runner.h"
#include "base/time/time.h"
#include "media/audio/audio_manager_base.h"
#include "media/audio/simple_sources.h"
#include "media/base/audio_bus.h"
#include "media/base/media_switches.h"

namespace media {

AudioInputStream* FakeAudioInputStream::MakeFakeStream(
    AudioManagerBase* manager,
    const AudioParameters& params)
{
    return new FakeAudioInputStream(manager, params);
}

FakeAudioInputStream::FakeAudioInputStream(AudioManagerBase* manager,
    const AudioParameters& params)
    : audio_manager_(manager)
    , callback_(NULL)
    , fake_audio_worker_(manager->GetWorkerTaskRunner(), params)
    , params_(params)
    , audio_bus_(AudioBus::Create(params))
{
    DCHECK(audio_manager_->GetTaskRunner()->BelongsToCurrentThread());
}

FakeAudioInputStream::~FakeAudioInputStream()
{
    DCHECK(!callback_);
}

bool FakeAudioInputStream::Open()
{
    DCHECK(audio_manager_->GetTaskRunner()->BelongsToCurrentThread());
    audio_bus_->Zero();

    return true;
}

void FakeAudioInputStream::Start(AudioInputCallback* callback)
{
    DCHECK(audio_manager_->GetTaskRunner()->BelongsToCurrentThread());
    callback_ = callback;
    fake_audio_worker_.Start(base::Bind(
        &FakeAudioInputStream::ReadAudioFromSource, base::Unretained(this)));
}

void FakeAudioInputStream::Stop()
{
    DCHECK(audio_manager_->GetTaskRunner()->BelongsToCurrentThread());
    fake_audio_worker_.Stop();
    callback_ = NULL;
}

void FakeAudioInputStream::Close()
{
    DCHECK(audio_manager_->GetTaskRunner()->BelongsToCurrentThread());
    DCHECK(!callback_);
    audio_manager_->ReleaseInputStream(this);
}

double FakeAudioInputStream::GetMaxVolume()
{
    DCHECK(audio_manager_->GetTaskRunner()->BelongsToCurrentThread());
    return 1.0;
}

void FakeAudioInputStream::SetVolume(double volume)
{
    DCHECK(audio_manager_->GetTaskRunner()->BelongsToCurrentThread());
}

double FakeAudioInputStream::GetVolume()
{
    DCHECK(audio_manager_->GetTaskRunner()->BelongsToCurrentThread());
    return 1.0;
}

bool FakeAudioInputStream::IsMuted()
{
    DCHECK(audio_manager_->GetTaskRunner()->BelongsToCurrentThread());
    return false;
}

bool FakeAudioInputStream::SetAutomaticGainControl(bool enabled)
{
    return false;
}

bool FakeAudioInputStream::GetAutomaticGainControl()
{
    return false;
}

void FakeAudioInputStream::ReadAudioFromSource()
{
    DCHECK(audio_manager_->GetWorkerTaskRunner()->BelongsToCurrentThread());
    DCHECK(callback_);

    if (!audio_source_)
        audio_source_ = ChooseSource();

    const int kNoDelay = 0;
    audio_source_->OnMoreData(audio_bus_.get(), kNoDelay);
    callback_->OnData(this, audio_bus_.get(), 0, 1.0);
}

using AudioSourceCallback = AudioOutputStream::AudioSourceCallback;
scoped_ptr<AudioSourceCallback> FakeAudioInputStream::ChooseSource()
{
    DCHECK(audio_manager_->GetWorkerTaskRunner()->BelongsToCurrentThread());

    //     if (base::CommandLine::ForCurrentProcess()->HasSwitch(
    //             switches::kUseFileForFakeAudioCapture)) {
    //         base::FilePath path_to_wav_file = base::CommandLine::ForCurrentProcess()->GetSwitchValuePath(
    //             switches::kUseFileForFakeAudioCapture);
    //         CHECK(!path_to_wav_file.empty())
    //             << "You must pass the file to use as argument to --"
    //             << switches::kUseFileForFakeAudioCapture << ".";
    //
    //         return make_scoped_ptr(new FileSource(params_, path_to_wav_file));
    //     }
    //     return make_scoped_ptr(new BeepingSource(params_));
    DebugBreak();
    return nullptr;
}

void FakeAudioInputStream::BeepOnce()
{
    BeepingSource::BeepOnce();
}

} // namespace media
