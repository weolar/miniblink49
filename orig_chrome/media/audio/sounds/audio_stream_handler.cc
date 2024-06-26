// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/audio/sounds/audio_stream_handler.h"

#include <string>

#include "base/cancelable_callback.h"
#include "base/logging.h"
#include "base/single_thread_task_runner.h"
#include "base/synchronization/lock.h"
#include "base/time/time.h"
#include "media/audio/audio_manager.h"
#include "media/audio/audio_manager_base.h"
#include "media/base/channel_layout.h"

namespace media {

namespace {

    // Volume percent.
    const double kOutputVolumePercent = 0.8;

    // The number of frames each OnMoreData() call will request.
    const int kDefaultFrameCount = 1024;

    // Keep alive timeout for audio stream.
    const int kKeepAliveMs = 1500;

    AudioStreamHandler::TestObserver* g_observer_for_testing = NULL;
    AudioOutputStream::AudioSourceCallback* g_audio_source_for_testing = NULL;

} // namespace

class AudioStreamHandler::AudioStreamContainer
    : public AudioOutputStream::AudioSourceCallback {
public:
    explicit AudioStreamContainer(const WavAudioHandler& wav_audio)
        : started_(false)
        , stream_(NULL)
        , cursor_(0)
        , delayed_stop_posted_(false)
        , wav_audio_(wav_audio)
    {
    }

    ~AudioStreamContainer() override
    {
        DCHECK(AudioManager::Get()->GetTaskRunner()->BelongsToCurrentThread());
    }

    void Play()
    {
        DCHECK(AudioManager::Get()->GetTaskRunner()->BelongsToCurrentThread());

        if (!stream_) {
            const AudioParameters params(
                AudioParameters::AUDIO_PCM_LOW_LATENCY,
                GuessChannelLayout(wav_audio_.num_channels()),
                wav_audio_.sample_rate(), wav_audio_.bits_per_sample(),
                kDefaultFrameCount);
            stream_ = AudioManager::Get()->MakeAudioOutputStreamProxy(params,
                std::string());
            if (!stream_ || !stream_->Open()) {
                LOG(ERROR) << "Failed to open an output stream.";
                return;
            }
            stream_->SetVolume(kOutputVolumePercent);
        }

        {
            base::AutoLock al(state_lock_);

            delayed_stop_posted_ = false;
            stop_closure_.Reset(base::Bind(&AudioStreamContainer::StopStream,
                base::Unretained(this)));

            if (started_) {
                if (wav_audio_.AtEnd(cursor_))
                    cursor_ = 0;
                return;
            }

            cursor_ = 0;
        }

        started_ = true;
        if (g_audio_source_for_testing)
            stream_->Start(g_audio_source_for_testing);
        else
            stream_->Start(this);

        if (g_observer_for_testing)
            g_observer_for_testing->OnPlay();
    }

    void Stop()
    {
        DCHECK(AudioManager::Get()->GetTaskRunner()->BelongsToCurrentThread());
        StopStream();
        if (stream_)
            stream_->Close();
        stream_ = NULL;
        stop_closure_.Cancel();
    }

private:
    // AudioOutputStream::AudioSourceCallback overrides:
    // Following methods could be called from *ANY* thread.
    int OnMoreData(AudioBus* dest, uint32 /* total_bytes_delay */) override
    {
        base::AutoLock al(state_lock_);
        size_t bytes_written = 0;

        if (wav_audio_.AtEnd(cursor_) || !wav_audio_.CopyTo(dest, cursor_, &bytes_written)) {
            if (delayed_stop_posted_)
                return 0;
            delayed_stop_posted_ = true;
            AudioManager::Get()->GetTaskRunner()->PostDelayedTask(
                FROM_HERE,
                stop_closure_.callback(),
                base::TimeDelta::FromMilliseconds(kKeepAliveMs));
            return 0;
        }
        cursor_ += bytes_written;
        return dest->frames();
    }

    void OnError(AudioOutputStream* /* stream */) override
    {
        LOG(ERROR) << "Error during system sound reproduction.";
        AudioManager::Get()->GetTaskRunner()->PostTask(
            FROM_HERE,
            base::Bind(&AudioStreamContainer::Stop, base::Unretained(this)));
    }

    void StopStream()
    {
        DCHECK(AudioManager::Get()->GetTaskRunner()->BelongsToCurrentThread());

        if (stream_ && started_) {
            // Do not hold the |state_lock_| while stopping the output stream.
            stream_->Stop();
            if (g_observer_for_testing)
                g_observer_for_testing->OnStop(cursor_);
        }

        started_ = false;
    }

    // Must only be accessed on the AudioManager::GetTaskRunner() thread.
    bool started_;
    AudioOutputStream* stream_;

    // All variables below must be accessed under |state_lock_| when |started_|.
    base::Lock state_lock_;
    size_t cursor_;
    bool delayed_stop_posted_;
    const WavAudioHandler wav_audio_;
    base::CancelableClosure stop_closure_;

    DISALLOW_COPY_AND_ASSIGN(AudioStreamContainer);
};

AudioStreamHandler::AudioStreamHandler(const base::StringPiece& wav_data)
    : wav_audio_(wav_data)
    , initialized_(false)
{
    AudioManager* manager = AudioManager::Get();
    if (!manager) {
        LOG(ERROR) << "Can't get access to audio manager.";
        return;
    }
    const AudioParameters params(
        AudioParameters::AUDIO_PCM_LOW_LATENCY,
        GuessChannelLayout(wav_audio_.num_channels()), wav_audio_.sample_rate(),
        wav_audio_.bits_per_sample(), kDefaultFrameCount);
    if (!params.IsValid()) {
        LOG(ERROR) << "Audio params are invalid.";
        return;
    }
    stream_.reset(new AudioStreamContainer(wav_audio_));
    initialized_ = true;
}

AudioStreamHandler::~AudioStreamHandler()
{
    DCHECK(CalledOnValidThread());
    AudioManager::Get()->GetTaskRunner()->PostTask(
        FROM_HERE,
        base::Bind(&AudioStreamContainer::Stop, base::Unretained(stream_.get())));
    AudioManager::Get()->GetTaskRunner()->DeleteSoon(FROM_HERE,
        stream_.release());
}

bool AudioStreamHandler::IsInitialized() const
{
    DCHECK(CalledOnValidThread());
    return initialized_;
}

bool AudioStreamHandler::Play()
{
    DCHECK(CalledOnValidThread());

    if (!IsInitialized())
        return false;

    AudioManager::Get()->GetTaskRunner()->PostTask(
        FROM_HERE,
        base::Bind(base::IgnoreResult(&AudioStreamContainer::Play),
            base::Unretained(stream_.get())));
    return true;
}

void AudioStreamHandler::Stop()
{
    DCHECK(CalledOnValidThread());
    AudioManager::Get()->GetTaskRunner()->PostTask(
        FROM_HERE,
        base::Bind(&AudioStreamContainer::Stop, base::Unretained(stream_.get())));
}

// static
void AudioStreamHandler::SetObserverForTesting(TestObserver* observer)
{
    g_observer_for_testing = observer;
}

// static
void AudioStreamHandler::SetAudioSourceForTesting(
    AudioOutputStream::AudioSourceCallback* source)
{
    g_audio_source_for_testing = source;
}

} // namespace media
