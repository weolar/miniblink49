// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_CAST_RTCP_RTCP_DEFINES_H_
#define MEDIA_CAST_RTCP_RTCP_DEFINES_H_

#include <map>
#include <set>

#include "media/cast/cast_config.h"
#include "media/cast/cast_defines.h"
#include "media/cast/logging/logging_defines.h"
#include "media/cast/net/cast_transport_defines.h"

namespace media {
namespace cast {

    static const size_t kRtcpCastLogHeaderSize = 12;
    static const size_t kRtcpReceiverFrameLogSize = 8;
    static const size_t kRtcpReceiverEventLogSize = 4;

    // Handle the per frame ACK and NACK messages.
    struct RtcpCastMessage {
        explicit RtcpCastMessage(uint32 ssrc);
        RtcpCastMessage();
        ~RtcpCastMessage();

        uint32 media_ssrc;
        uint32 ack_frame_id;
        uint16 target_delay_ms;
        MissingFramesAndPacketsMap missing_frames_and_packets;
    };

    // Log messages from receiver to sender.
    struct RtcpReceiverEventLogMessage {
        RtcpReceiverEventLogMessage();
        ~RtcpReceiverEventLogMessage();

        CastLoggingEvent type;
        base::TimeTicks event_timestamp;
        base::TimeDelta delay_delta;
        uint16 packet_id;
    };

    typedef std::list<RtcpReceiverEventLogMessage> RtcpReceiverEventLogMessages;

    struct RtcpReceiverFrameLogMessage {
        explicit RtcpReceiverFrameLogMessage(uint32 rtp_timestamp);
        ~RtcpReceiverFrameLogMessage();

        uint32 rtp_timestamp_;
        RtcpReceiverEventLogMessages event_log_messages_;

        // TODO(mikhal): Investigate what's the best way to allow adding
        // DISALLOW_COPY_AND_ASSIGN, as currently it contradicts the implementation
        // and possible changes have a big impact on design.
    };

    typedef std::list<RtcpReceiverFrameLogMessage> RtcpReceiverLogMessage;

    struct RtcpNackMessage {
        RtcpNackMessage();
        ~RtcpNackMessage();

        uint32 remote_ssrc;
        std::list<uint16> nack_list;

        DISALLOW_COPY_AND_ASSIGN(RtcpNackMessage);
    };

    struct RtcpReceiverReferenceTimeReport {
        RtcpReceiverReferenceTimeReport();
        ~RtcpReceiverReferenceTimeReport();

        uint32 remote_ssrc;
        uint32 ntp_seconds;
        uint32 ntp_fraction;
    };

    inline bool operator==(RtcpReceiverReferenceTimeReport lhs,
        RtcpReceiverReferenceTimeReport rhs)
    {
        return lhs.remote_ssrc == rhs.remote_ssrc && lhs.ntp_seconds == rhs.ntp_seconds && lhs.ntp_fraction == rhs.ntp_fraction;
    }

    // Struct used by raw event subscribers as an intermediate format before
    // sending off to the other side via RTCP.
    // (i.e., {Sender,Receiver}RtcpEventSubscriber)
    struct RtcpEvent {
        RtcpEvent();
        ~RtcpEvent();

        CastLoggingEvent type;

        // Time of event logged.
        base::TimeTicks timestamp;

        // Render/playout delay. Only set for FRAME_PLAYOUT events.
        base::TimeDelta delay_delta;

        // Only set for packet events.
        uint16 packet_id;
    };

    typedef base::Callback<void(const RtcpCastMessage&)> RtcpCastMessageCallback;
    typedef base::Callback<void(base::TimeDelta)> RtcpRttCallback;
    typedef base::Callback<void(const RtcpReceiverLogMessage&)> RtcpLogMessageCallback;

    // TODO(hubbe): Document members of this struct.
    struct RtpReceiverStatistics {
        RtpReceiverStatistics();
        uint8 fraction_lost;
        uint32 cumulative_lost; // 24 bits valid.
        uint32 extended_high_sequence_number;
        uint32 jitter;
    };

    // These are intended to only be created using Rtcp::ConvertToNTPAndSave.
    struct RtcpTimeData {
        uint32 ntp_seconds;
        uint32 ntp_fraction;
        base::TimeTicks timestamp;
    };

    // This struct is used to encapsulate all the parameters of the
    // SendRtcpFromRtpReceiver for IPC transportation.
    struct SendRtcpFromRtpReceiver_Params {
        SendRtcpFromRtpReceiver_Params();
        ~SendRtcpFromRtpReceiver_Params();
        uint32 ssrc;
        uint32 sender_ssrc;
        RtcpTimeData time_data;
        scoped_ptr<RtcpCastMessage> cast_message;
        base::TimeDelta target_delay;
        scoped_ptr<std::vector<std::pair<RtpTimestamp, RtcpEvent>>> rtcp_events;
        scoped_ptr<RtpReceiverStatistics> rtp_receiver_statistics;
    };

} // namespace cast
} // namespace media

#endif // MEDIA_CAST_RTCP_RTCP_DEFINES_H_
