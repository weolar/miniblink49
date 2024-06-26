// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// This class maintains a bi-directional RTCP connection with a remote
// peer.

#ifndef MEDIA_CAST_RTCP_RTCP_H_
#define MEDIA_CAST_RTCP_RTCP_H_

#include <map>
#include <queue>
#include <string>

#include "base/basictypes.h"
#include "base/memory/scoped_ptr.h"
#include "base/memory/weak_ptr.h"
#include "base/time/tick_clock.h"
#include "base/time/time.h"
#include "media/cast/cast_config.h"
#include "media/cast/cast_defines.h"
#include "media/cast/common/clock_drift_smoother.h"
#include "media/cast/net/cast_transport_defines.h"
#include "media/cast/net/cast_transport_sender.h"
#include "media/cast/net/rtcp/receiver_rtcp_event_subscriber.h"
#include "media/cast/net/rtcp/rtcp_builder.h"
#include "media/cast/net/rtcp/rtcp_defines.h"

namespace media {
namespace cast {

    class LocalRtcpReceiverFeedback;
    class PacedPacketSender;
    class RtcpReceiver;
    class RtcpBuilder;

    typedef std::pair<uint32, base::TimeTicks> RtcpSendTimePair;
    typedef std::map<uint32, base::TimeTicks> RtcpSendTimeMap;
    typedef std::queue<RtcpSendTimePair> RtcpSendTimeQueue;

    // TODO(hclam): This should be renamed to RtcpSession.
    class Rtcp {
    public:
        Rtcp(const RtcpCastMessageCallback& cast_callback,
            const RtcpRttCallback& rtt_callback,
            const RtcpLogMessageCallback& log_callback,
            base::TickClock* clock, // Not owned.
            PacedPacketSender* packet_sender, // Not owned.
            uint32 local_ssrc,
            uint32 remote_ssrc);

        virtual ~Rtcp();

        // Send a RTCP sender report.
        // |current_time| is the current time reported by a tick clock.
        // |current_time_as_rtp_timestamp| is the corresponding RTP timestamp.
        // |send_packet_count| is the number of packets sent.
        // |send_octet_count| is the number of octets sent.
        void SendRtcpFromRtpSender(
            base::TimeTicks current_time,
            uint32 current_time_as_rtp_timestamp,
            uint32 send_packet_count,
            size_t send_octet_count);

        // This function is meant to be used in conjunction with
        // SendRtcpFromRtpReceiver.
        // |now| is converted to NTP and saved internally for
        // future round-trip/lip-sync calculations.
        // This is done in a separate method so that SendRtcpFromRtpReceiver can
        // be done on a separate (temporary) RTCP object.
        RtcpTimeData ConvertToNTPAndSave(base::TimeTicks now);

        // |cast_message|, |rtcp_events| and |rtp_receiver_statistics| are optional;
        // if |cast_message| is provided the RTCP receiver report will append a Cast
        // message containing Acks and Nacks; |target_delay| is sent together with
        // |cast_message|. If |rtcp_events| is provided the RTCP receiver report will
        // append the log messages.
        void SendRtcpFromRtpReceiver(
            RtcpTimeData time_data,
            const RtcpCastMessage* cast_message,
            base::TimeDelta target_delay,
            const ReceiverRtcpEventSubscriber::RtcpEvents* rtcp_events,
            const RtpReceiverStatistics* rtp_receiver_statistics) const;

        // Submit a received packet to this object. The packet will be parsed
        // and used to maintain a RTCP session.
        // Returns false if this is not a RTCP packet or it is not directed to
        // this session, e.g. SSRC doesn't match.
        bool IncomingRtcpPacket(const uint8* data, size_t length);

        // If available, returns true and sets the output arguments to the latest
        // lip-sync timestamps gleaned from the sender reports.  While the sender
        // provides reference NTP times relative to its own wall clock, the
        // |reference_time| returned here has been translated to the local
        // CastEnvironment clock.
        bool GetLatestLipSyncTimes(uint32* rtp_timestamp,
            base::TimeTicks* reference_time) const;

        void OnReceivedReceiverLog(const RtcpReceiverLogMessage& receiver_log);

        // If greater than zero, this is the last measured network round trip time.
        base::TimeDelta current_round_trip_time() const
        {
            return current_round_trip_time_;
        }

        static bool IsRtcpPacket(const uint8* packet, size_t length);
        static uint32 GetSsrcOfSender(const uint8* rtcp_buffer, size_t length);

        uint32 GetLocalSsrc() const { return local_ssrc_; }
        uint32 GetRemoteSsrc() const { return remote_ssrc_; }

    protected:
        void OnReceivedNtp(uint32 ntp_seconds, uint32 ntp_fraction);
        void OnReceivedLipSyncInfo(uint32 rtp_timestamp,
            uint32 ntp_seconds,
            uint32 ntp_fraction);

    private:
        void OnReceivedDelaySinceLastReport(uint32 last_report,
            uint32 delay_since_last_report);

        void OnReceivedCastFeedback(const RtcpCastMessage& cast_message);

        void SaveLastSentNtpTime(const base::TimeTicks& now,
            uint32 last_ntp_seconds,
            uint32 last_ntp_fraction);

        // Remove duplicate events in |receiver_log|.
        // Returns true if any events remain.
        bool DedupeReceiverLog(RtcpReceiverLogMessage* receiver_log);

        const RtcpCastMessageCallback cast_callback_;
        const RtcpRttCallback rtt_callback_;
        const RtcpLogMessageCallback log_callback_;
        base::TickClock* const clock_; // Not owned by this class.
        RtcpBuilder rtcp_builder_;
        PacedPacketSender* packet_sender_; // Not owned.
        const uint32 local_ssrc_;
        const uint32 remote_ssrc_;

        RtcpSendTimeMap last_reports_sent_map_;
        RtcpSendTimeQueue last_reports_sent_queue_;

        // The truncated (i.e., 64-->32-bit) NTP timestamp provided in the last report
        // from the remote peer, along with the local time at which the report was
        // received.  These values are used for ping-pong'ing NTP timestamps between
        // the peers so that they can estimate the network's round-trip time.
        uint32 last_report_truncated_ntp_;
        base::TimeTicks time_last_report_received_;

        // Maintains a smoothed offset between the local clock and the remote clock.
        // Calling this member's Current() method is only valid if
        // |time_last_report_received_| is not "null."
        ClockDriftSmoother local_clock_ahead_by_;

        // Latest "lip sync" info from the sender.  The sender provides the RTP
        // timestamp of some frame of its choosing and also a corresponding reference
        // NTP timestamp sampled from a clock common to all media streams.  It is
        // expected that the sender will update this data regularly and in a timely
        // manner (e.g., about once per second).
        uint32 lip_sync_rtp_timestamp_;
        uint64 lip_sync_ntp_timestamp_;

        // The last measured network round trip time.  This is updated with each
        // sender report --> receiver report round trip.  If this is zero, then the
        // round trip time has not been measured yet.
        base::TimeDelta current_round_trip_time_;

        base::TimeTicks largest_seen_timestamp_;

        // For extending received ACK frame IDs from 8-bit to 32-bit.
        FrameIdWrapHelper ack_frame_id_wrap_helper_;

        // Maintains a history of receiver events.
        typedef std::pair<uint64, uint64> ReceiverEventKey;
        base::hash_set<ReceiverEventKey> receiver_event_key_set_;
        std::queue<ReceiverEventKey> receiver_event_key_queue_;

        DISALLOW_COPY_AND_ASSIGN(Rtcp);
    };

} // namespace cast
} // namespace media

#endif // MEDIA_CAST_RTCP_RTCP_H_
