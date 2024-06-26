// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/cast/sender/video_sender.h"

#include <algorithm>
#include <cmath>
#include <cstring>

#include "base/bind.h"
#include "base/logging.h"
#include "base/trace_event/trace_event.h"
#include "media/cast/cast_defines.h"
#include "media/cast/net/cast_transport_config.h"
#include "media/cast/sender/performance_metrics_overlay.h"
#include "media/cast/sender/video_encoder.h"

namespace media {
namespace cast {

    namespace {

        // The following two constants are used to adjust the target
        // playout delay (when allowed). They were calculated using
        // a combination of cast_benchmark runs and manual testing.
        //
        // This is how many round trips we think we need on the network.
        const int kRoundTripsNeeded = 4;

        // This is an estimate of all the the constant time needed independent of
        // network quality (e.g., additional time that accounts for encode and decode
        // time).
        const int kConstantTimeMs = 75;

        // The target maximum utilization of the encoder and network resources.  This is
        // used to attenuate the actual measured utilization values in order to provide
        // "breathing room" (i.e., to ensure there will be sufficient CPU and bandwidth
        // available to handle the occasional more-complex frames).
        const int kTargetUtilizationPercentage = 75;

        // Extract capture begin/end timestamps from |video_frame|'s metadata and log
        // it.
        void LogVideoCaptureTimestamps(CastEnvironment* cast_environment,
            const media::VideoFrame& video_frame,
            RtpTimestamp rtp_timestamp)
        {
            scoped_ptr<FrameEvent> capture_begin_event(new FrameEvent());
            capture_begin_event->type = FRAME_CAPTURE_BEGIN;
            capture_begin_event->media_type = VIDEO_EVENT;
            capture_begin_event->rtp_timestamp = rtp_timestamp;

            scoped_ptr<FrameEvent> capture_end_event(new FrameEvent());
            capture_end_event->type = FRAME_CAPTURE_END;
            capture_end_event->media_type = VIDEO_EVENT;
            capture_end_event->rtp_timestamp = rtp_timestamp;
            capture_end_event->width = video_frame.visible_rect().width();
            capture_end_event->height = video_frame.visible_rect().height();

            if (!video_frame.metadata()->GetTimeTicks(
                    media::VideoFrameMetadata::CAPTURE_BEGIN_TIME,
                    &capture_begin_event->timestamp)
                || !video_frame.metadata()->GetTimeTicks(
                    media::VideoFrameMetadata::CAPTURE_END_TIME,
                    &capture_end_event->timestamp)) {
                // The frame capture timestamps were not provided by the video capture
                // source.  Simply log the events as happening right now.
                capture_begin_event->timestamp = capture_end_event->timestamp = cast_environment->Clock()->NowTicks();
            }

            cast_environment->logger()->DispatchFrameEvent(capture_begin_event.Pass());
            cast_environment->logger()->DispatchFrameEvent(capture_end_event.Pass());
        }

    } // namespace

    // Note, we use a fixed bitrate value when external video encoder is used.
    // Some hardware encoder shows bad behavior if we set the bitrate too
    // frequently, e.g. quality drop, not abiding by target bitrate, etc.
    // See details: crbug.com/392086.
    VideoSender::VideoSender(
        scoped_refptr<CastEnvironment> cast_environment,
        const VideoSenderConfig& video_config,
        const StatusChangeCallback& status_change_cb,
        const CreateVideoEncodeAcceleratorCallback& create_vea_cb,
        const CreateVideoEncodeMemoryCallback& create_video_encode_mem_cb,
        CastTransportSender* const transport_sender,
        const PlayoutDelayChangeCB& playout_delay_change_cb)
        : FrameSender(
            cast_environment,
            false,
            transport_sender,
            kVideoFrequency,
            video_config.ssrc,
            video_config.max_frame_rate,
            video_config.min_playout_delay,
            video_config.max_playout_delay,
            video_config.use_external_encoder
                ? NewFixedCongestionControl(
                    (video_config.min_bitrate + video_config.max_bitrate) / 2)
                : NewAdaptiveCongestionControl(cast_environment->Clock(),
                    video_config.max_bitrate,
                    video_config.min_bitrate,
                    video_config.max_frame_rate))
        , frames_in_encoder_(0)
        , last_bitrate_(0)
        , playout_delay_change_cb_(playout_delay_change_cb)
        , last_reported_deadline_utilization_(-1.0)
        , last_reported_lossy_utilization_(-1.0)
        , weak_factory_(this)
    {
        video_encoder_ = VideoEncoder::Create(
            cast_environment_,
            video_config,
            status_change_cb,
            create_vea_cb,
            create_video_encode_mem_cb);
        if (!video_encoder_) {
            cast_environment_->PostTask(
                CastEnvironment::MAIN,
                FROM_HERE,
                base::Bind(status_change_cb, STATUS_UNSUPPORTED_CODEC));
        }

        media::cast::CastTransportRtpConfig transport_config;
        transport_config.ssrc = video_config.ssrc;
        transport_config.feedback_ssrc = video_config.receiver_ssrc;
        transport_config.rtp_payload_type = video_config.rtp_payload_type;
        transport_config.aes_key = video_config.aes_key;
        transport_config.aes_iv_mask = video_config.aes_iv_mask;

        transport_sender->InitializeVideo(
            transport_config,
            base::Bind(&VideoSender::OnReceivedCastFeedback,
                weak_factory_.GetWeakPtr()),
            base::Bind(&VideoSender::OnMeasuredRoundTripTime,
                weak_factory_.GetWeakPtr()));
    }

    VideoSender::~VideoSender()
    {
    }

    void VideoSender::InsertRawVideoFrame(
        const scoped_refptr<media::VideoFrame>& video_frame,
        const base::TimeTicks& reference_time)
    {
        DCHECK(cast_environment_->CurrentlyOn(CastEnvironment::MAIN));

        if (!video_encoder_) {
            NOTREACHED();
            return;
        }

        const RtpTimestamp rtp_timestamp = TimeDeltaToRtpDelta(video_frame->timestamp(), kVideoFrequency);
        LogVideoCaptureTimestamps(cast_environment_.get(), *video_frame,
            rtp_timestamp);

        // Used by chrome/browser/extension/api/cast_streaming/performance_test.cc
        TRACE_EVENT_INSTANT2(
            "cast_perf_test", "InsertRawVideoFrame",
            TRACE_EVENT_SCOPE_THREAD,
            "timestamp", reference_time.ToInternalValue(),
            "rtp_timestamp", rtp_timestamp);

        // Drop the frame if either its RTP or reference timestamp is not an increase
        // over the last frame's.  This protects: 1) the duration calculations that
        // assume timestamps are monotonically non-decreasing, and 2) assumptions made
        // deeper in the implementation where each frame's RTP timestamp needs to be
        // unique.
        if (!last_enqueued_frame_reference_time_.is_null() && (!IsNewerRtpTimestamp(rtp_timestamp, last_enqueued_frame_rtp_timestamp_) || reference_time <= last_enqueued_frame_reference_time_)) {
            VLOG(1) << "Dropping video frame: RTP or reference time did not increase.";
            TRACE_EVENT_INSTANT2("cast.stream", "Video Frame Drop",
                TRACE_EVENT_SCOPE_THREAD,
                "rtp_timestamp", rtp_timestamp,
                "reason", "time did not increase");
            return;
        }

        // Two video frames are needed to compute the exact media duration added by
        // the next frame.  If there are no frames in the encoder, compute a guess
        // based on the configured |max_frame_rate_|.  Any error introduced by this
        // guess will be eliminated when |duration_in_encoder_| is updated in
        // OnEncodedVideoFrame().
        const base::TimeDelta duration_added_by_next_frame = frames_in_encoder_ > 0 ? reference_time - last_enqueued_frame_reference_time_ : base::TimeDelta::FromSecondsD(1.0 / max_frame_rate_);

        if (ShouldDropNextFrame(duration_added_by_next_frame)) {
            base::TimeDelta new_target_delay = std::min(
                current_round_trip_time_ * kRoundTripsNeeded + base::TimeDelta::FromMilliseconds(kConstantTimeMs),
                max_playout_delay_);
            if (new_target_delay > target_playout_delay_) {
                VLOG(1) << "New target delay: " << new_target_delay.InMilliseconds();
                playout_delay_change_cb_.Run(new_target_delay);
            }

            // Some encoder implementations have a frame window for analysis. Since we
            // are dropping this frame, unless we instruct the encoder to flush all the
            // frames that have been enqueued for encoding, frames_in_encoder_ and
            // last_enqueued_frame_reference_time_ will never be updated and we will
            // drop every subsequent frame for the rest of the session.
            video_encoder_->EmitFrames();

            TRACE_EVENT_INSTANT2("cast.stream", "Video Frame Drop",
                TRACE_EVENT_SCOPE_THREAD,
                "rtp_timestamp", rtp_timestamp,
                "reason", "too much in flight");
            return;
        }

        if (video_frame->visible_rect().IsEmpty()) {
            VLOG(1) << "Rejecting empty video frame.";
            return;
        }

        const int bitrate = congestion_control_->GetBitrate(
            reference_time + target_playout_delay_, target_playout_delay_,
            GetMaximumTargetBitrateForFrame(*video_frame));
        if (bitrate != last_bitrate_) {
            video_encoder_->SetBitRate(bitrate);
            last_bitrate_ = bitrate;
        }

        TRACE_COUNTER_ID1("cast.stream", "Video Target Bitrate", this, bitrate);

        MaybeRenderPerformanceMetricsOverlay(bitrate,
            frames_in_encoder_ + 1,
            last_reported_deadline_utilization_,
            last_reported_lossy_utilization_,
            video_frame.get());

        if (video_encoder_->EncodeVideoFrame(
                video_frame,
                reference_time,
                base::Bind(&VideoSender::OnEncodedVideoFrame,
                    weak_factory_.GetWeakPtr(),
                    video_frame,
                    bitrate))) {
            TRACE_EVENT_ASYNC_BEGIN1("cast.stream", "Video Encode", video_frame.get(),
                "rtp_timestamp", rtp_timestamp);
            frames_in_encoder_++;
            duration_in_encoder_ += duration_added_by_next_frame;
            last_enqueued_frame_rtp_timestamp_ = rtp_timestamp;
            last_enqueued_frame_reference_time_ = reference_time;
        } else {
            VLOG(1) << "Encoder rejected a frame.  Skipping...";
            TRACE_EVENT_INSTANT1("cast.stream", "Video Encode Reject",
                TRACE_EVENT_SCOPE_THREAD,
                "rtp_timestamp", rtp_timestamp);
        }
    }

    scoped_ptr<VideoFrameFactory> VideoSender::CreateVideoFrameFactory()
    {
        return video_encoder_ ? video_encoder_->CreateVideoFrameFactory() : nullptr;
    }

    int VideoSender::GetNumberOfFramesInEncoder() const
    {
        return frames_in_encoder_;
    }

    base::TimeDelta VideoSender::GetInFlightMediaDuration() const
    {
        if (GetUnacknowledgedFrameCount() > 0) {
            const uint32 oldest_unacked_frame_id = latest_acked_frame_id_ + 1;
            return last_enqueued_frame_reference_time_ - GetRecordedReferenceTime(oldest_unacked_frame_id);
        } else {
            return duration_in_encoder_;
        }
    }

    void VideoSender::OnAck(uint32 frame_id)
    {
        video_encoder_->LatestFrameIdToReference(frame_id);
    }

    // static
    int VideoSender::GetMaximumTargetBitrateForFrame(
        const media::VideoFrame& frame)
    {
        enum {
            // Constants used to linearly translate between lines of resolution and a
            // maximum target bitrate.  These values are based on observed quality
            // trade-offs over a wide range of content.  The math will use these values
            // to compute a bitrate of 2 Mbps for 360 lines of resolution and 4 Mbps for
            // 720 lines.
            BITRATE_FOR_HIGH_RESOLUTION = 4000000,
            BITRATE_FOR_STANDARD_RESOLUTION = 2000000,
            HIGH_RESOLUTION_LINES = 720,
            STANDARD_RESOLUTION_LINES = 360,

            // The smallest maximum target bitrate, regardless of what the math says.
            MAX_BITRATE_LOWER_BOUND = 1000000,

            // Constants used to boost the result for high frame rate content.
            HIGH_FRAME_RATE_THRESHOLD_USEC = 25000, // 40 FPS
            HIGH_FRAME_RATE_BOOST_NUMERATOR = 3,
            HIGH_FRAME_RATE_BOOST_DENOMINATOR = 2,
        };

        // Determine the approximate height of a 16:9 frame having the same area
        // (number of pixels) as |frame|.
        const gfx::Size& resolution = frame.visible_rect().size();
        const int lines_of_resolution = ((resolution.width() * 9) == (resolution.height() * 16)) ? resolution.height() : static_cast<int>(sqrt(resolution.GetArea() * 9.0 / 16.0));

        // Linearly translate from |lines_of_resolution| to a maximum target bitrate.
        int64 result = lines_of_resolution - STANDARD_RESOLUTION_LINES;
        result *= BITRATE_FOR_HIGH_RESOLUTION - BITRATE_FOR_STANDARD_RESOLUTION;
        result /= HIGH_RESOLUTION_LINES - STANDARD_RESOLUTION_LINES;
        result += BITRATE_FOR_STANDARD_RESOLUTION;

        // Boost the result for high frame rate content.
        base::TimeDelta frame_duration;
        if (frame.metadata()->GetTimeDelta(media::VideoFrameMetadata::FRAME_DURATION,
                &frame_duration)
            && frame_duration > base::TimeDelta() && frame_duration.InMicroseconds() <= HIGH_FRAME_RATE_THRESHOLD_USEC) {
            result *= HIGH_FRAME_RATE_BOOST_NUMERATOR;
            result /= HIGH_FRAME_RATE_BOOST_DENOMINATOR;
        }

        // Return a lower-bounded result.
        return std::max<int>(result, MAX_BITRATE_LOWER_BOUND);
    }

    void VideoSender::OnEncodedVideoFrame(
        const scoped_refptr<media::VideoFrame>& video_frame,
        int encoder_bitrate,
        scoped_ptr<SenderEncodedFrame> encoded_frame)
    {
        DCHECK(cast_environment_->CurrentlyOn(CastEnvironment::MAIN));

        frames_in_encoder_--;
        DCHECK_GE(frames_in_encoder_, 0);

        duration_in_encoder_ = last_enqueued_frame_reference_time_ - encoded_frame->reference_time;

        last_reported_deadline_utilization_ = encoded_frame->deadline_utilization;
        last_reported_lossy_utilization_ = encoded_frame->lossy_utilization;

        TRACE_EVENT_ASYNC_END2("cast.stream", "Video Encode", video_frame.get(),
            "deadline_utilization", last_reported_deadline_utilization_,
            "lossy_utilization", last_reported_lossy_utilization_);

        // Report the resource utilization for processing this frame.  Take the
        // greater of the two utilization values and attenuate them such that the
        // target utilization is reported as the maximum sustainable amount.
        const double attenuated_utilization = std::max(last_reported_deadline_utilization_,
                                                  last_reported_lossy_utilization_)
            / (kTargetUtilizationPercentage / 100.0);
        if (attenuated_utilization >= 0.0) {
            // Key frames are artificially capped to 1.0 because their actual
            // utilization is atypical compared to the other frames in the stream, and
            // this can misguide the producer of the input video frames.
            video_frame->metadata()->SetDouble(
                media::VideoFrameMetadata::RESOURCE_UTILIZATION,
                encoded_frame->dependency == EncodedFrame::KEY ? std::min(1.0, attenuated_utilization) : attenuated_utilization);
        }

        SendEncodedFrame(encoder_bitrate, encoded_frame.Pass());
    }

} // namespace cast
} // namespace media
