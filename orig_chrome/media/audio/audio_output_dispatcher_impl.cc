// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/audio/audio_output_dispatcher_impl.h"

#include <algorithm>

#include "base/bind.h"
#include "base/compiler_specific.h"
#include "base/single_thread_task_runner.h"
#include "base/time/time.h"
#include "media/audio/audio_output_proxy.h"

namespace media {

AudioOutputDispatcherImpl::AudioOutputDispatcherImpl(
    AudioManager* audio_manager,
    const AudioParameters& params,
    const std::string& output_device_id,
    const base::TimeDelta& close_delay)
    : AudioOutputDispatcher(audio_manager,
        params,
        output_device_id)
    , idle_proxies_(0)
    , close_timer_(FROM_HERE,
          close_delay,
          this,
          &AudioOutputDispatcherImpl::CloseAllIdleStreams)
    , audio_log_(
          audio_manager->CreateAudioLog(AudioLogFactory::AUDIO_OUTPUT_STREAM))
    , audio_stream_id_(0)
{
}

AudioOutputDispatcherImpl::~AudioOutputDispatcherImpl()
{
    DCHECK_EQ(idle_proxies_, 0u);
    DCHECK(proxy_to_physical_map_.empty());
    DCHECK(idle_streams_.empty());
}

bool AudioOutputDispatcherImpl::OpenStream()
{
    DCHECK(task_runner_->BelongsToCurrentThread());

    // Ensure that there is at least one open stream.
    if (idle_streams_.empty() && !CreateAndOpenStream())
        return false;

    ++idle_proxies_;
    close_timer_.Reset();
    return true;
}

bool AudioOutputDispatcherImpl::StartStream(
    AudioOutputStream::AudioSourceCallback* callback,
    AudioOutputProxy* stream_proxy)
{
    DCHECK(task_runner_->BelongsToCurrentThread());
    DCHECK(proxy_to_physical_map_.find(stream_proxy) == proxy_to_physical_map_.end());

    if (idle_streams_.empty() && !CreateAndOpenStream())
        return false;

    AudioOutputStream* physical_stream = idle_streams_.back();
    idle_streams_.pop_back();

    DCHECK_GT(idle_proxies_, 0u);
    --idle_proxies_;

    double volume = 0;
    stream_proxy->GetVolume(&volume);
    physical_stream->SetVolume(volume);
    const int stream_id = audio_stream_ids_[physical_stream];
    audio_log_->OnSetVolume(stream_id, volume);
    physical_stream->Start(callback);
    audio_log_->OnStarted(stream_id);
    proxy_to_physical_map_[stream_proxy] = physical_stream;

    close_timer_.Reset();
    return true;
}

void AudioOutputDispatcherImpl::StopStream(AudioOutputProxy* stream_proxy)
{
    DCHECK(task_runner_->BelongsToCurrentThread());

    AudioStreamMap::iterator it = proxy_to_physical_map_.find(stream_proxy);
    DCHECK(it != proxy_to_physical_map_.end());
    AudioOutputStream* physical_stream = it->second;
    proxy_to_physical_map_.erase(it);

    physical_stream->Stop();
    audio_log_->OnStopped(audio_stream_ids_[physical_stream]);
    ++idle_proxies_;
    idle_streams_.push_back(physical_stream);

    close_timer_.Reset();
}

void AudioOutputDispatcherImpl::StreamVolumeSet(AudioOutputProxy* stream_proxy,
    double volume)
{
    DCHECK(task_runner_->BelongsToCurrentThread());
    AudioStreamMap::iterator it = proxy_to_physical_map_.find(stream_proxy);
    if (it != proxy_to_physical_map_.end()) {
        AudioOutputStream* physical_stream = it->second;
        physical_stream->SetVolume(volume);
        audio_log_->OnSetVolume(audio_stream_ids_[physical_stream], volume);
    }
}

void AudioOutputDispatcherImpl::CloseStream(AudioOutputProxy* stream_proxy)
{
    DCHECK(task_runner_->BelongsToCurrentThread());

    DCHECK_GT(idle_proxies_, 0u);
    --idle_proxies_;

    // Leave at least a single stream running until the close timer fires to help
    // cycle time when streams are opened and closed repeatedly.
    CloseIdleStreams(std::max(idle_proxies_, static_cast<size_t>(1)));
    close_timer_.Reset();
}

void AudioOutputDispatcherImpl::Shutdown()
{
    DCHECK(task_runner_->BelongsToCurrentThread());

    // Close all idle streams immediately.  The |close_timer_| will handle
    // invalidating any outstanding tasks upon its destruction.
    CloseAllIdleStreams();

    // No AudioOutputProxy objects should hold a reference to us when we get
    // to this stage.
    DCHECK(HasOneRef()) << "Only the AudioManager should hold a reference";
}

bool AudioOutputDispatcherImpl::HasOutputProxies() const
{
    return idle_proxies_ || !proxy_to_physical_map_.empty();
}

bool AudioOutputDispatcherImpl::CreateAndOpenStream()
{
    DCHECK(task_runner_->BelongsToCurrentThread());
    AudioOutputStream* stream = audio_manager_->MakeAudioOutputStream(
        params_, device_id_);
    if (!stream)
        return false;

    if (!stream->Open()) {
        stream->Close();
        return false;
    }

    const int stream_id = audio_stream_id_++;
    audio_stream_ids_[stream] = stream_id;
    audio_log_->OnCreated(
        stream_id, params_, device_id_);

    idle_streams_.push_back(stream);
    return true;
}

void AudioOutputDispatcherImpl::CloseAllIdleStreams()
{
    DCHECK(task_runner_->BelongsToCurrentThread());
    CloseIdleStreams(0);
}

void AudioOutputDispatcherImpl::CloseIdleStreams(size_t keep_alive)
{
    DCHECK(task_runner_->BelongsToCurrentThread());
    if (idle_streams_.size() <= keep_alive)
        return;
    for (size_t i = keep_alive; i < idle_streams_.size(); ++i) {
        AudioOutputStream* stream = idle_streams_[i];
        stream->Close();

        AudioStreamIDMap::iterator it = audio_stream_ids_.find(stream);
        DCHECK(it != audio_stream_ids_.end());
        audio_log_->OnClosed(it->second);
        audio_stream_ids_.erase(it);
    }
    idle_streams_.erase(idle_streams_.begin() + keep_alive, idle_streams_.end());
}

} // namespace media
