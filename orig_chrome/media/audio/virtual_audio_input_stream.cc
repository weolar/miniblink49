// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/audio/virtual_audio_input_stream.h"

#include <algorithm>
#include <utility>

#include "base/bind.h"
#include "base/single_thread_task_runner.h"
#include "media/audio/virtual_audio_output_stream.h"

namespace media {

// LoopbackAudioConverter works similar to AudioConverter and converts input
// streams to different audio parameters. Then, the LoopbackAudioConverter can
// be used as an input to another AudioConverter. This allows us to
// use converted audio from AudioOutputStreams as input to an AudioConverter.
// For example, this allows converting multiple streams into a common format and
// using the converted audio as input to another AudioConverter (i.e. a mixer).
class LoopbackAudioConverter : public AudioConverter::InputCallback {
public:
    LoopbackAudioConverter(const AudioParameters& input_params,
        const AudioParameters& output_params)
        : audio_converter_(input_params, output_params, false)
    {
    }

    ~LoopbackAudioConverter() override { }

    void AddInput(AudioConverter::InputCallback* input)
    {
        audio_converter_.AddInput(input);
    }

    void RemoveInput(AudioConverter::InputCallback* input)
    {
        audio_converter_.RemoveInput(input);
    }

private:
    double ProvideInput(AudioBus* audio_bus,
        base::TimeDelta buffer_delay) override
    {
        audio_converter_.ConvertWithDelay(buffer_delay, audio_bus);
        return 1.0;
    }

    AudioConverter audio_converter_;

    DISALLOW_COPY_AND_ASSIGN(LoopbackAudioConverter);
};

VirtualAudioInputStream::VirtualAudioInputStream(
    const AudioParameters& params,
    const scoped_refptr<base::SingleThreadTaskRunner>& worker_task_runner,
    const AfterCloseCallback& after_close_cb)
    : worker_task_runner_(worker_task_runner)
    , after_close_cb_(after_close_cb)
    , callback_(NULL)
    , buffer_(new uint8[params.GetBytesPerBuffer()])
    , params_(params)
    , mixer_(params_, params_, false)
    , num_attached_output_streams_(0)
    , fake_worker_(worker_task_runner_, params_)
    , audio_bus_(AudioBus::Create(params))
{
    DCHECK(params_.IsValid());
    DCHECK(worker_task_runner_.get());

    // VAIS can be constructed on any thread, but will DCHECK that all
    // AudioInputStream methods are called from the same thread.
    thread_checker_.DetachFromThread();
}

VirtualAudioInputStream::~VirtualAudioInputStream()
{
    DCHECK(!callback_);

    // Sanity-check: Contract for Add/RemoveOutputStream() requires that all
    // output streams be removed before VirtualAudioInputStream is destroyed.
    DCHECK_EQ(0, num_attached_output_streams_);

    for (AudioConvertersMap::iterator it = converters_.begin();
         it != converters_.end(); ++it) {
        delete it->second;
    }
}

bool VirtualAudioInputStream::Open()
{
    DCHECK(thread_checker_.CalledOnValidThread());
    memset(buffer_.get(), 0, params_.GetBytesPerBuffer());
    return true;
}

void VirtualAudioInputStream::Start(AudioInputCallback* callback)
{
    DCHECK(thread_checker_.CalledOnValidThread());
    callback_ = callback;
    fake_worker_.Start(base::Bind(
        &VirtualAudioInputStream::PumpAudio, base::Unretained(this)));
}

void VirtualAudioInputStream::Stop()
{
    DCHECK(thread_checker_.CalledOnValidThread());
    fake_worker_.Stop();
    callback_ = NULL;
}

void VirtualAudioInputStream::AddOutputStream(
    VirtualAudioOutputStream* stream, const AudioParameters& output_params)
{
    DCHECK(thread_checker_.CalledOnValidThread());

    base::AutoLock scoped_lock(converter_network_lock_);

    AudioConvertersMap::iterator converter = converters_.find(output_params);
    if (converter == converters_.end()) {
        std::pair<AudioConvertersMap::iterator, bool> result = converters_.insert(
            std::make_pair(output_params,
                new LoopbackAudioConverter(output_params, params_)));
        converter = result.first;

        // Add to main mixer if we just added a new AudioTransform.
        mixer_.AddInput(converter->second);
    }
    converter->second->AddInput(stream);
    ++num_attached_output_streams_;
}

void VirtualAudioInputStream::RemoveOutputStream(
    VirtualAudioOutputStream* stream, const AudioParameters& output_params)
{
    DCHECK(thread_checker_.CalledOnValidThread());

    base::AutoLock scoped_lock(converter_network_lock_);

    DCHECK(converters_.find(output_params) != converters_.end());
    converters_[output_params]->RemoveInput(stream);

    --num_attached_output_streams_;
    DCHECK_LE(0, num_attached_output_streams_);
}

void VirtualAudioInputStream::PumpAudio()
{
    DCHECK(worker_task_runner_->BelongsToCurrentThread());

    {
        base::AutoLock scoped_lock(converter_network_lock_);
        // Because the audio is being looped-back, the delay until it will be played
        // out is zero.
        mixer_.ConvertWithDelay(base::TimeDelta(), audio_bus_.get());
    }
    // Because the audio is being looped-back, the delay since since it was
    // recorded is zero.
    callback_->OnData(this, audio_bus_.get(), 0, 1.0);
}

void VirtualAudioInputStream::Close()
{
    DCHECK(thread_checker_.CalledOnValidThread());

    Stop(); // Make sure callback_ is no longer being used.

    // If a non-null AfterCloseCallback was provided to the constructor, invoke it
    // here.  The callback is moved to a stack-local first since |this| could be
    // destroyed during Run().
    if (!after_close_cb_.is_null()) {
        const AfterCloseCallback cb = after_close_cb_;
        after_close_cb_.Reset();
        cb.Run(this);
    }
}

double VirtualAudioInputStream::GetMaxVolume()
{
    return 1.0;
}

void VirtualAudioInputStream::SetVolume(double volume) { }

double VirtualAudioInputStream::GetVolume()
{
    return 1.0;
}

bool VirtualAudioInputStream::SetAutomaticGainControl(bool enabled)
{
    return false;
}

bool VirtualAudioInputStream::GetAutomaticGainControl()
{
    return false;
}

bool VirtualAudioInputStream::IsMuted()
{
    return false;
}

} // namespace media
