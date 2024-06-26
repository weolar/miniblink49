// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/cast/net/rtcp/rtcp_defines.h"

#include "media/cast/logging/logging_defines.h"

namespace media {
namespace cast {

    RtcpCastMessage::RtcpCastMessage(uint32 ssrc)
        : media_ssrc(ssrc)
        , ack_frame_id(0u)
        , target_delay_ms(0)
    {
    }
    RtcpCastMessage::RtcpCastMessage()
        : media_ssrc(0)
        , ack_frame_id(0u)
        , target_delay_ms(0)
    {
    }
    RtcpCastMessage::~RtcpCastMessage() { }

    RtcpReceiverEventLogMessage::RtcpReceiverEventLogMessage()
        : type(UNKNOWN)
        , packet_id(0u)
    {
    }
    RtcpReceiverEventLogMessage::~RtcpReceiverEventLogMessage() { }

    RtcpReceiverFrameLogMessage::RtcpReceiverFrameLogMessage(uint32 timestamp)
        : rtp_timestamp_(timestamp)
    {
    }
    RtcpReceiverFrameLogMessage::~RtcpReceiverFrameLogMessage() { }

    RtcpNackMessage::RtcpNackMessage()
        : remote_ssrc(0u)
    {
    }
    RtcpNackMessage::~RtcpNackMessage() { }

    RtcpReceiverReferenceTimeReport::RtcpReceiverReferenceTimeReport()
        : remote_ssrc(0u)
        , ntp_seconds(0u)
        , ntp_fraction(0u)
    {
    }
    RtcpReceiverReferenceTimeReport::~RtcpReceiverReferenceTimeReport() { }

    RtcpEvent::RtcpEvent()
        : type(UNKNOWN)
        , packet_id(0u)
    {
    }
    RtcpEvent::~RtcpEvent() { }

    RtpReceiverStatistics::RtpReceiverStatistics()
        : fraction_lost(0)
        , cumulative_lost(0)
        , extended_high_sequence_number(0)
        , jitter(0)
    {
    }

    SendRtcpFromRtpReceiver_Params::SendRtcpFromRtpReceiver_Params()
        : ssrc(0)
        , sender_ssrc(0)
    {
    }

    SendRtcpFromRtpReceiver_Params::~SendRtcpFromRtpReceiver_Params() { }

} // namespace cast
} // namespace media
