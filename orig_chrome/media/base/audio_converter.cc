// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// AudioConverter implementation.  Uses MultiChannelSincResampler for resampling
// audio, ChannelMixer for channel mixing, and AudioPullFifo for buffering.
//
// Delay estimates are provided to InputCallbacks based on the frame delay
// information reported via the resampler and FIFO units.

#include "media/base/audio_converter.h"

#include <algorithm>

#include "base/bind.h"
#include "base/bind_helpers.h"
#include "media/base/audio_bus.h"
#include "media/base/audio_pull_fifo.h"
#include "media/base/channel_mixer.h"
#include "media/base/multi_channel_resampler.h"
#include "media/base/vector_math.h"

namespace media {

AudioConverter::AudioConverter(const AudioParameters& input_params,
    const AudioParameters& output_params,
    bool disable_fifo)
    : chunk_size_(input_params.frames_per_buffer())
    , downmix_early_(false)
    , resampler_frame_delay_(0)
    , input_channel_count_(input_params.channels())
{
    CHECK(input_params.IsValid());
    CHECK(output_params.IsValid());

    // Handle different input and output channel layouts.
    if (input_params.channel_layout() != output_params.channel_layout() || input_params.channels() != output_params.channels()) {
        DVLOG(1) << "Remixing channel layout from " << input_params.channel_layout()
                 << " to " << output_params.channel_layout() << "; from "
                 << input_params.channels() << " channels to "
                 << output_params.channels() << " channels.";
        channel_mixer_.reset(new ChannelMixer(input_params, output_params));

        // Pare off data as early as we can for efficiency.
        downmix_early_ = input_params.channels() > output_params.channels();
    }

    // Only resample if necessary since it's expensive.
    if (input_params.sample_rate() != output_params.sample_rate()) {
        DVLOG(1) << "Resampling from " << input_params.sample_rate() << " to "
                 << output_params.sample_rate();
        const int request_size = disable_fifo ? SincResampler::kDefaultRequestSize : input_params.frames_per_buffer();
        const double io_sample_rate_ratio = input_params.sample_rate() / static_cast<double>(output_params.sample_rate());
        resampler_.reset(new MultiChannelResampler(
            downmix_early_ ? output_params.channels() : input_params.channels(),
            io_sample_rate_ratio,
            request_size,
            base::Bind(&AudioConverter::ProvideInput, base::Unretained(this))));
    }

    input_frame_duration_ = base::TimeDelta::FromMicroseconds(
        base::Time::kMicrosecondsPerSecond / static_cast<double>(input_params.sample_rate()));
    output_frame_duration_ = base::TimeDelta::FromMicroseconds(
        base::Time::kMicrosecondsPerSecond / static_cast<double>(output_params.sample_rate()));

    // The resampler can be configured to work with a specific request size, so a
    // FIFO is not necessary when resampling.
    if (disable_fifo || resampler_)
        return;

    // Since the output device may want a different buffer size than the caller
    // asked for, we need to use a FIFO to ensure that both sides read in chunk
    // sizes they're configured for.
    if (input_params.frames_per_buffer() != output_params.frames_per_buffer()) {
        DVLOG(1) << "Rebuffering from " << input_params.frames_per_buffer()
                 << " to " << output_params.frames_per_buffer();
        chunk_size_ = input_params.frames_per_buffer();
        audio_fifo_.reset(new AudioPullFifo(
            downmix_early_ ? output_params.channels() : input_params.channels(),
            chunk_size_,
            base::Bind(&AudioConverter::SourceCallback, base::Unretained(this))));
    }
}

AudioConverter::~AudioConverter() { }

void AudioConverter::AddInput(InputCallback* input)
{
    DCHECK(std::find(transform_inputs_.begin(), transform_inputs_.end(), input) == transform_inputs_.end());
    transform_inputs_.push_back(input);
}

void AudioConverter::RemoveInput(InputCallback* input)
{
    DCHECK(std::find(transform_inputs_.begin(), transform_inputs_.end(), input) != transform_inputs_.end());
    transform_inputs_.remove(input);

    if (transform_inputs_.empty())
        Reset();
}

void AudioConverter::Reset()
{
    if (audio_fifo_)
        audio_fifo_->Clear();
    if (resampler_)
        resampler_->Flush();
}

int AudioConverter::ChunkSize() const
{
    if (!resampler_)
        return chunk_size_;
    return resampler_->ChunkSize();
}

void AudioConverter::PrimeWithSilence()
{
    if (resampler_) {
        resampler_->PrimeWithSilence();
    }
}

void AudioConverter::ConvertWithDelay(const base::TimeDelta& initial_delay,
    AudioBus* dest)
{
    initial_delay_ = initial_delay;

    if (transform_inputs_.empty()) {
        dest->Zero();
        return;
    }

    // Determine if channel mixing should be done and if it should be done before
    // or after resampling.  If it's possible to reduce the channel count prior to
    // resampling we can save a lot of processing time.  Vice versa, we don't want
    // to increase the channel count prior to resampling for the same reason.
    bool needs_mixing = channel_mixer_ && !downmix_early_;

    if (needs_mixing)
        CreateUnmixedAudioIfNecessary(dest->frames());

    AudioBus* temp_dest = needs_mixing ? unmixed_audio_.get() : dest;
    DCHECK(temp_dest);

    // Figure out which method to call based on whether we're resampling and
    // rebuffering, just resampling, or just mixing.  We want to avoid any extra
    // steps when possible since we may be converting audio data in real time.
    if (!resampler_ && !audio_fifo_) {
        SourceCallback(0, temp_dest);
    } else {
        if (resampler_)
            resampler_->Resample(temp_dest->frames(), temp_dest);
        else
            ProvideInput(0, temp_dest);
    }

    // Finally upmix the channels if we didn't do so earlier.
    if (needs_mixing) {
        DCHECK_EQ(temp_dest->frames(), dest->frames());
        channel_mixer_->Transform(temp_dest, dest);
    }
}

void AudioConverter::Convert(AudioBus* dest)
{
    ConvertWithDelay(base::TimeDelta::FromMilliseconds(0), dest);
}

void AudioConverter::SourceCallback(int fifo_frame_delay, AudioBus* dest)
{
    const bool needs_downmix = channel_mixer_ && downmix_early_;

    if (!mixer_input_audio_bus_ || mixer_input_audio_bus_->frames() != dest->frames()) {
        mixer_input_audio_bus_ = AudioBus::Create(input_channel_count_, dest->frames());
    }

    // If we're downmixing early we need a temporary AudioBus which matches
    // the the input channel count and input frame size since we're passing
    // |unmixed_audio_| directly to the |source_callback_|.
    if (needs_downmix)
        CreateUnmixedAudioIfNecessary(dest->frames());

    AudioBus* const temp_dest = needs_downmix ? unmixed_audio_.get() : dest;

    // Sanity check our inputs.
    DCHECK_EQ(temp_dest->frames(), mixer_input_audio_bus_->frames());
    DCHECK_EQ(temp_dest->channels(), mixer_input_audio_bus_->channels());

    // Calculate the buffer delay for this callback.
    base::TimeDelta buffer_delay = initial_delay_;
    if (resampler_) {
        buffer_delay += base::TimeDelta::FromMicroseconds(
            resampler_frame_delay_ * output_frame_duration_.InMicroseconds());
    }
    if (audio_fifo_) {
        buffer_delay += base::TimeDelta::FromMicroseconds(
            fifo_frame_delay * input_frame_duration_.InMicroseconds());
    }

    // If we only have a single input, avoid an extra copy.
    AudioBus* const provide_input_dest = transform_inputs_.size() == 1 ? temp_dest : mixer_input_audio_bus_.get();

    // Have each mixer render its data into an output buffer then mix the result.
    for (auto* input : transform_inputs_) {
        const float volume = input->ProvideInput(provide_input_dest, buffer_delay);

        // Optimize the most common single input, full volume case.
        if (input == transform_inputs_.front()) {
            if (volume == 1.0f) {
                if (temp_dest != provide_input_dest)
                    provide_input_dest->CopyTo(temp_dest);
            } else if (volume > 0) {
                for (int i = 0; i < provide_input_dest->channels(); ++i) {
                    vector_math::FMUL(
                        provide_input_dest->channel(i), volume,
                        provide_input_dest->frames(), temp_dest->channel(i));
                }
            } else {
                // Zero |temp_dest| otherwise, so we're mixing into a clean buffer.
                temp_dest->Zero();
            }

            continue;
        }

        // Volume adjust and mix each mixer input into |temp_dest| after rendering.
        if (volume > 0) {
            for (int i = 0; i < mixer_input_audio_bus_->channels(); ++i) {
                vector_math::FMAC(
                    mixer_input_audio_bus_->channel(i), volume,
                    mixer_input_audio_bus_->frames(), temp_dest->channel(i));
            }
        }
    }

    if (needs_downmix) {
        DCHECK_EQ(temp_dest->frames(), dest->frames());
        channel_mixer_->Transform(temp_dest, dest);
    }
}

void AudioConverter::ProvideInput(int resampler_frame_delay, AudioBus* dest)
{
    resampler_frame_delay_ = resampler_frame_delay;
    if (audio_fifo_)
        audio_fifo_->Consume(dest, dest->frames());
    else
        SourceCallback(0, dest);
}

void AudioConverter::CreateUnmixedAudioIfNecessary(int frames)
{
    if (!unmixed_audio_ || unmixed_audio_->frames() != frames)
        unmixed_audio_ = AudioBus::Create(input_channel_count_, frames);
}

} // namespace media
