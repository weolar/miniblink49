// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/cast/sender/audio_sender.h"

#include "base/bind.h"
#include "base/logging.h"
#include "base/message_loop/message_loop.h"
#include "media/cast/cast_defines.h"
#include "media/cast/net/cast_transport_config.h"
#include "media/cast/sender/audio_encoder.h"

namespace media {
namespace cast {

    AudioSender::AudioSender(scoped_refptr<CastEnvironment> cast_environment,
        const AudioSenderConfig& audio_config,
        const StatusChangeCallback& status_change_cb,
        CastTransportSender* const transport_sender)
        : FrameSender(cast_environment,
            true,
            transport_sender,
            audio_config.frequency,
            audio_config.ssrc,
            0, // |max_frame_rate_| is set after encoder initialization.
            audio_config.min_playout_delay,
            audio_config.max_playout_delay,
            NewFixedCongestionControl(audio_config.bitrate))
        , samples_in_encoder_(0)
        , weak_factory_(this)
    {
        if (!audio_config.use_external_encoder) {
            audio_encoder_.reset(
                new AudioEncoder(cast_environment,
                    audio_config.channels,
                    audio_config.frequency,
                    audio_config.bitrate,
                    audio_config.codec,
                    base::Bind(&AudioSender::OnEncodedAudioFrame,
                        weak_factory_.GetWeakPtr(),
                        audio_config.bitrate)));
        }

        // AudioEncoder provides no operational status changes during normal use.
        // Post a task now with its initialization result status to allow the client
        // to start sending frames.
        cast_environment_->PostTask(
            CastEnvironment::MAIN,
            FROM_HERE,
            base::Bind(status_change_cb,
                audio_encoder_ ? audio_encoder_->InitializationResult() : STATUS_INVALID_CONFIGURATION));

        // The number of samples per encoded audio frame depends on the codec and its
        // initialization parameters. Now that we have an encoder, we can calculate
        // the maximum frame rate.
        max_frame_rate_ = audio_config.frequency / audio_encoder_->GetSamplesPerFrame();

        media::cast::CastTransportRtpConfig transport_config;
        transport_config.ssrc = audio_config.ssrc;
        transport_config.feedback_ssrc = audio_config.receiver_ssrc;
        transport_config.rtp_payload_type = audio_config.rtp_payload_type;
        transport_config.aes_key = audio_config.aes_key;
        transport_config.aes_iv_mask = audio_config.aes_iv_mask;

        transport_sender->InitializeAudio(
            transport_config,
            base::Bind(&AudioSender::OnReceivedCastFeedback,
                weak_factory_.GetWeakPtr()),
            base::Bind(&AudioSender::OnMeasuredRoundTripTime,
                weak_factory_.GetWeakPtr()));
    }

    AudioSender::~AudioSender() { }

    void AudioSender::InsertAudio(scoped_ptr<AudioBus> audio_bus,
        const base::TimeTicks& recorded_time)
    {
        DCHECK(cast_environment_->CurrentlyOn(CastEnvironment::MAIN));

        if (!audio_encoder_) {
            NOTREACHED();
            return;
        }

        const base::TimeDelta next_frame_duration = RtpDeltaToTimeDelta(audio_bus->frames(), rtp_timebase());
        if (ShouldDropNextFrame(next_frame_duration))
            return;

        samples_in_encoder_ += audio_bus->frames();

        audio_encoder_->InsertAudio(audio_bus.Pass(), recorded_time);
    }

    int AudioSender::GetNumberOfFramesInEncoder() const
    {
        // Note: It's possible for a partial frame to be in the encoder, but returning
        // the floor() is good enough for the "design limit" check in FrameSender.
        return samples_in_encoder_ / audio_encoder_->GetSamplesPerFrame();
    }

    base::TimeDelta AudioSender::GetInFlightMediaDuration() const
    {
        const int samples_in_flight = samples_in_encoder_ + GetUnacknowledgedFrameCount() * audio_encoder_->GetSamplesPerFrame();
        return RtpDeltaToTimeDelta(samples_in_flight, rtp_timebase());
    }

    void AudioSender::OnAck(uint32 frame_id)
    {
    }

    void AudioSender::OnEncodedAudioFrame(
        int encoder_bitrate,
        scoped_ptr<SenderEncodedFrame> encoded_frame,
        int samples_skipped)
    {
        DCHECK(cast_environment_->CurrentlyOn(CastEnvironment::MAIN));

        samples_in_encoder_ -= audio_encoder_->GetSamplesPerFrame() + samples_skipped;
        DCHECK_GE(samples_in_encoder_, 0);

        SendEncodedFrame(encoder_bitrate, encoded_frame.Pass());
    }

} // namespace cast
} // namespace media
