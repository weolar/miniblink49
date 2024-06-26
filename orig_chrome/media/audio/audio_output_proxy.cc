// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/audio/audio_output_proxy.h"

#include "base/logging.h"
#include "base/message_loop/message_loop.h"
#include "media/audio/audio_manager.h"
#include "media/audio/audio_output_dispatcher.h"

namespace media {

AudioOutputProxy::AudioOutputProxy(AudioOutputDispatcher* dispatcher)
    : dispatcher_(dispatcher)
    , state_(kCreated)
    , volume_(1.0)
{
}

AudioOutputProxy::~AudioOutputProxy()
{
    DCHECK(CalledOnValidThread());
    DCHECK(state_ == kCreated || state_ == kClosed) << "State is: " << state_;
}

bool AudioOutputProxy::Open()
{
    DCHECK(CalledOnValidThread());
    DCHECK_EQ(state_, kCreated);

    if (!dispatcher_->OpenStream()) {
        state_ = kOpenError;
        return false;
    }

    state_ = kOpened;
    return true;
}

void AudioOutputProxy::Start(AudioSourceCallback* callback)
{
    DCHECK(CalledOnValidThread());

    // We need to support both states since the callback may not handle OnError()
    // immediately (or at all).  It's also possible for subsequent StartStream()
    // calls to succeed after failing, so we allow it to be called again.
    DCHECK(state_ == kOpened || state_ == kStartError);

    if (!dispatcher_->StartStream(callback, this)) {
        state_ = kStartError;
        callback->OnError(this);
        return;
    }
    state_ = kPlaying;
}

void AudioOutputProxy::Stop()
{
    DCHECK(CalledOnValidThread());
    if (state_ != kPlaying)
        return;

    dispatcher_->StopStream(this);
    state_ = kOpened;
}

void AudioOutputProxy::SetVolume(double volume)
{
    DCHECK(CalledOnValidThread());
    volume_ = volume;
    dispatcher_->StreamVolumeSet(this, volume);
}

void AudioOutputProxy::GetVolume(double* volume)
{
    DCHECK(CalledOnValidThread());
    *volume = volume_;
}

void AudioOutputProxy::Close()
{
    DCHECK(CalledOnValidThread());
    DCHECK(state_ == kCreated || state_ == kOpenError || state_ == kOpened || state_ == kStartError);

    // kStartError means OpenStream() succeeded and the stream must be closed
    // before destruction.
    if (state_ != kCreated && state_ != kOpenError)
        dispatcher_->CloseStream(this);

    state_ = kClosed;

    // Delete the object now like is done in the Close() implementation of
    // physical stream objects.  If we delete the object via DeleteSoon, we
    // unnecessarily complicate the Shutdown procedure of the
    // dispatcher+audio manager.
    delete this;
}

} // namespace media
