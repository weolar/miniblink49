// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// This is the base class for an object that send frames to a receiver.
// TODO(hclam): Refactor such that there is no separate AudioSender vs.
// VideoSender, and the functionality of both is rolled into this class.

#ifndef MEDIA_CAST_SENDER_FRAME_SENDER_H_
#define MEDIA_CAST_SENDER_FRAME_SENDER_H_

#include "base/basictypes.h"
#include "base/memory/ref_counted.h"
#include "base/memory/weak_ptr.h"
#include "base/time/time.h"
#include "media/cast/cast_environment.h"
#include "media/cast/net/rtcp/rtcp.h"
#include "media/cast/sender/congestion_control.h"

namespace media {
namespace cast {

    struct SenderEncodedFrame;

    class FrameSender {
    public:
        FrameSender(scoped_refptr<CastEnvironment> cast_environment,
            bool is_audio,
            CastTransportSender* const transport_sender,
            int rtp_timebase,
            uint32 ssrc,
            double max_frame_rate,
            base::TimeDelta min_playout_delay,
            base::TimeDelta max_playout_delay,
            CongestionControl* congestion_control);
        virtual ~FrameSender();

        int rtp_timebase() const { return rtp_timebase_; }

        // Calling this function is only valid if the receiver supports the
        // "extra_playout_delay", rtp extension.
        void SetTargetPlayoutDelay(base::TimeDelta new_target_playout_delay);

        base::TimeDelta GetTargetPlayoutDelay() const
        {
            return target_playout_delay_;
        }

        // Called by the encoder with the next EncodeFrame to send.
        void SendEncodedFrame(int requested_bitrate_before_encode,
            scoped_ptr<SenderEncodedFrame> encoded_frame);

    protected:
        // Returns the number of frames in the encoder's backlog.
        virtual int GetNumberOfFramesInEncoder() const = 0;

        // Returns the duration of the data in the encoder's backlog plus the duration
        // of sent, unacknowledged frames.
        virtual base::TimeDelta GetInFlightMediaDuration() const = 0;

        // Called when we get an ACK for a frame.
        virtual void OnAck(uint32 frame_id) = 0;

    protected:
        // Schedule and execute periodic sending of RTCP report.
        void ScheduleNextRtcpReport();
        void SendRtcpReport(bool schedule_future_reports);

        void OnMeasuredRoundTripTime(base::TimeDelta rtt);

        const scoped_refptr<CastEnvironment> cast_environment_;

        // Sends encoded frames over the configured transport (e.g., UDP).  In
        // Chromium, this could be a proxy that first sends the frames from a renderer
        // process to the browser process over IPC, with the browser process being
        // responsible for "packetizing" the frames and pushing packets into the
        // network layer.
        CastTransportSender* const transport_sender_;

        const uint32 ssrc_;

    protected:
        // Schedule and execute periodic checks for re-sending packets.  If no
        // acknowledgements have been received for "too long," AudioSender will
        // speculatively re-send certain packets of an unacked frame to kick-start
        // re-transmission.  This is a last resort tactic to prevent the session from
        // getting stuck after a long outage.
        void ScheduleNextResendCheck();
        void ResendCheck();
        void ResendForKickstart();

        // Protected for testability.
        void OnReceivedCastFeedback(const RtcpCastMessage& cast_feedback);

        // Returns true if too many frames would be in-flight by encoding and sending
        // the next frame having the given |frame_duration|.
        bool ShouldDropNextFrame(base::TimeDelta frame_duration) const;

        // Record or retrieve a recent history of each frame's timestamps.
        // Warning: If a frame ID too far in the past is requested, the getters will
        // silently succeed but return incorrect values.  Be sure to respect
        // media::cast::kMaxUnackedFrames.
        void RecordLatestFrameTimestamps(uint32 frame_id,
            base::TimeTicks reference_time,
            RtpTimestamp rtp_timestamp);
        base::TimeTicks GetRecordedReferenceTime(uint32 frame_id) const;
        RtpTimestamp GetRecordedRtpTimestamp(uint32 frame_id) const;

        // Returns the number of frames that were sent but not yet acknowledged.
        int GetUnacknowledgedFrameCount() const;

        // The total amount of time between a frame's capture/recording on the sender
        // and its playback on the receiver (i.e., shown to a user).  This is fixed as
        // a value large enough to give the system sufficient time to encode,
        // transmit/retransmit, receive, decode, and render; given its run-time
        // environment (sender/receiver hardware performance, network conditions,
        // etc.).
        base::TimeDelta target_playout_delay_;
        base::TimeDelta min_playout_delay_;
        base::TimeDelta max_playout_delay_;

        // If true, we transmit the target playout delay to the receiver.
        bool send_target_playout_delay_;

        // Max encoded frames generated per second.
        double max_frame_rate_;

        // Counts how many RTCP reports are being "aggressively" sent (i.e., one per
        // frame) at the start of the session.  Once a threshold is reached, RTCP
        // reports are instead sent at the configured interval + random drift.
        int num_aggressive_rtcp_reports_sent_;

        // This is "null" until the first frame is sent.  Thereafter, this tracks the
        // last time any frame was sent or re-sent.
        base::TimeTicks last_send_time_;

        // The ID of the last frame sent.  Logic throughout FrameSender assumes this
        // can safely wrap-around.  This member is invalid until
        // |!last_send_time_.is_null()|.
        uint32 last_sent_frame_id_;

        // The ID of the latest (not necessarily the last) frame that has been
        // acknowledged.  Logic throughout AudioSender assumes this can safely
        // wrap-around.  This member is invalid until |!last_send_time_.is_null()|.
        uint32 latest_acked_frame_id_;

        // Counts the number of duplicate ACK that are being received.  When this
        // number reaches a threshold, the sender will take this as a sign that the
        // receiver hasn't yet received the first packet of the next frame.  In this
        // case, VideoSender will trigger a re-send of the next frame.
        int duplicate_ack_counter_;

        // This object controls how we change the bitrate to make sure the
        // buffer doesn't overflow.
        scoped_ptr<CongestionControl> congestion_control_;

        // The most recently measured round trip time.
        base::TimeDelta current_round_trip_time_;

    private:
        // Returns the maximum media duration currently allowed in-flight.  This
        // fluctuates in response to the currently-measured network latency.
        base::TimeDelta GetAllowedInFlightMediaDuration() const;

        // RTP timestamp increment representing one second.
        const int rtp_timebase_;

        const bool is_audio_;

        // Ring buffers to keep track of recent frame timestamps (both in terms of
        // local reference time and RTP media time).  These should only be accessed
        // through the Record/GetXXX() methods.
        base::TimeTicks frame_reference_times_[256];
        RtpTimestamp frame_rtp_timestamps_[256];

        // NOTE: Weak pointers must be invalidated before all other member variables.
        base::WeakPtrFactory<FrameSender> weak_factory_;

        DISALLOW_COPY_AND_ASSIGN(FrameSender);
    };

} // namespace cast
} // namespace media

#endif // MEDIA_CAST_SENDER_FRAME_SENDER_H_
