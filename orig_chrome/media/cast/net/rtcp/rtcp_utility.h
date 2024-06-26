// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_CAST_RTCP_RTCP_UTILITY_H_
#define MEDIA_CAST_RTCP_RTCP_UTILITY_H_

#include "base/big_endian.h"
#include "media/cast/cast_config.h"
#include "media/cast/cast_defines.h"
#include "media/cast/logging/logging_defines.h"
#include "media/cast/net/rtcp/rtcp_defines.h"

namespace media {
namespace cast {

    // RFC 3550 page 44, including end null.
    static const size_t kRtcpCnameSize = 256;

    static const uint32 kCast = ('C' << 24) + ('A' << 16) + ('S' << 8) + 'T';

    static const uint8 kReceiverLogSubtype = 2;

    static const size_t kRtcpMaxReceiverLogMessages = 256;
    static const size_t kRtcpMaxCastLossFields = 100;

    struct RtcpCommonHeader {
        uint8 V; // Version.
        bool P; // Padding.
        uint8 IC; // Item count / subtype.
        uint8 PT; // Packet Type.
        size_t length_in_octets;
    };

    class RtcpParser {
    public:
        RtcpParser(uint32 local_ssrc, uint32 remote_ssrc);
        ~RtcpParser();

        bool Parse(base::BigEndianReader* reader);

        bool has_sender_report() const { return has_sender_report_; }
        const RtcpSenderInfo& sender_report() const
        {
            return sender_report_;
        }

        bool has_last_report() const { return has_last_report_; }
        uint32 last_report() const { return last_report_; }
        uint32 delay_since_last_report() const { return delay_since_last_report_; }

        bool has_receiver_log() const { return !receiver_log_.empty(); }
        const RtcpReceiverLogMessage& receiver_log() const { return receiver_log_; }
        RtcpReceiverLogMessage* mutable_receiver_log() { return &receiver_log_; }

        bool has_cast_message() const { return has_cast_message_; }
        const RtcpCastMessage& cast_message() const { return cast_message_; }
        RtcpCastMessage* mutable_cast_message() { return &cast_message_; }

        bool has_receiver_reference_time_report() const
        {
            return has_receiver_reference_time_report_;
        }
        const RtcpReceiverReferenceTimeReport&
        receiver_reference_time_report() const
        {
            return receiver_reference_time_report_;
        }

    private:
        bool ParseCommonHeader(base::BigEndianReader* reader,
            RtcpCommonHeader* parsed_header);
        bool ParseSR(base::BigEndianReader* reader,
            const RtcpCommonHeader& header);
        bool ParseRR(base::BigEndianReader* reader,
            const RtcpCommonHeader& header);
        bool ParseReportBlock(base::BigEndianReader* reader);
        bool ParseApplicationDefined(base::BigEndianReader* reader,
            const RtcpCommonHeader& header);
        bool ParseCastReceiverLogFrameItem(base::BigEndianReader* reader);
        bool ParseFeedbackCommon(base::BigEndianReader* reader,
            const RtcpCommonHeader& header);
        bool ParseExtendedReport(base::BigEndianReader* reader,
            const RtcpCommonHeader& header);
        bool ParseExtendedReportReceiverReferenceTimeReport(
            base::BigEndianReader* reader,
            uint32 remote_ssrc);
        bool ParseExtendedReportDelaySinceLastReceiverReport(
            base::BigEndianReader* reader);

        uint32 local_ssrc_;
        uint32 remote_ssrc_;

        bool has_sender_report_;
        RtcpSenderInfo sender_report_;

        uint32 last_report_;
        uint32 delay_since_last_report_;
        bool has_last_report_;

        // |receiver_log_| is a vector vector, no need for has_*.
        RtcpReceiverLogMessage receiver_log_;

        bool has_cast_message_;
        RtcpCastMessage cast_message_;

        bool has_receiver_reference_time_report_;
        RtcpReceiverReferenceTimeReport receiver_reference_time_report_;

        DISALLOW_COPY_AND_ASSIGN(RtcpParser);
    };

    // Converts a log event type to an integer value.
    // NOTE: We have only allocated 4 bits to represent the type of event over the
    // wire. Therefore, this function can only return values from 0 to 15.
    uint8 ConvertEventTypeToWireFormat(CastLoggingEvent event);

    // The inverse of |ConvertEventTypeToWireFormat()|.
    CastLoggingEvent TranslateToLogEventFromWireFormat(uint8 event);

} // namespace cast
} // namespace media

#endif // MEDIA_CAST_RTCP_RTCP_UTILITY_H_
