// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/audio/audio_output_dispatcher.h"

#include "base/logging.h"
#include "base/single_thread_task_runner.h"

namespace media {

AudioOutputDispatcher::AudioOutputDispatcher(
    AudioManager* audio_manager,
    const AudioParameters& params,
    const std::string& device_id)
    : audio_manager_(audio_manager)
    , task_runner_(audio_manager->GetTaskRunner())
    , params_(params)
    , device_id_(device_id)
{
    // We expect to be instantiated on the audio thread.  Otherwise the
    // |task_runner_| member will point to the wrong message loop!
    DCHECK(audio_manager->GetTaskRunner()->BelongsToCurrentThread());
}

AudioOutputDispatcher::~AudioOutputDispatcher()
{
    DCHECK(task_runner_->BelongsToCurrentThread());
}

} // namespace media
