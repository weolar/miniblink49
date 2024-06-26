// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/cast/net/rtcp/rtcp_utility.h"

#include "base/logging.h"
#include "media/cast/net/cast_transport_defines.h"

namespace media {
namespace cast {

    RtcpParser::RtcpParser(uint32 local_ssrc, uint32 remote_ssrc)
        : local_ssrc_(local_ssrc)
        , remote_ssrc_(remote_ssrc)
        , has_sender_report_(false)
        , has_last_report_(false)
        , has_cast_message_(false)
        , has_receiver_reference_time_report_(false)
    {
    }

    RtcpParser::~RtcpParser() { }

    bool RtcpParser::Parse(base::BigEndianReader* reader)
    {
        while (reader->remaining()) {
            RtcpCommonHeader header;
            if (!ParseCommonHeader(reader, &header))
                return false;

            base::StringPiece tmp;
            if (!reader->ReadPiece(&tmp, header.length_in_octets - 4))
                return false;
            base::BigEndianReader chunk(tmp.data(), tmp.size());

            switch (header.PT) {
            case kPacketTypeSenderReport:
                if (!ParseSR(&chunk, header))
                    return false;
                break;

            case kPacketTypeReceiverReport:
                if (!ParseRR(&chunk, header))
                    return false;
                break;

            case kPacketTypeApplicationDefined:
                if (!ParseApplicationDefined(&chunk, header))
                    return false;
                break;

            case kPacketTypePayloadSpecific:
                if (!ParseFeedbackCommon(&chunk, header))
                    return false;
                break;

            case kPacketTypeXr:
                if (!ParseExtendedReport(&chunk, header))
                    return false;
                break;
            }
        }
        return true;
    }

    bool RtcpParser::ParseCommonHeader(base::BigEndianReader* reader,
        RtcpCommonHeader* parsed_header)
    {
        //  0                   1                   2                   3
        //  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
        // +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
        // |V=2|P|    IC   |      PT       |             length            |
        // +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
        //
        // Common header for all Rtcp packets, 4 octets.

        uint8 byte;
        if (!reader->ReadU8(&byte))
            return false;
        parsed_header->V = byte >> 6;
        parsed_header->P = ((byte & 0x20) == 0) ? false : true;

        // Check if RTP version field == 2.
        if (parsed_header->V != 2)
            return false;

        parsed_header->IC = byte & 0x1f;
        if (!reader->ReadU8(&parsed_header->PT))
            return false;

        uint16 bytes;
        if (!reader->ReadU16(&bytes))
            return false;

        parsed_header->length_in_octets = (static_cast<size_t>(bytes) + 1) * 4;

        if (parsed_header->length_in_octets == 0)
            return false;

        return true;
    }

    bool RtcpParser::ParseSR(base::BigEndianReader* reader,
        const RtcpCommonHeader& header)
    {
        uint32 sender_ssrc;
        if (!reader->ReadU32(&sender_ssrc))
            return false;

        if (sender_ssrc != remote_ssrc_)
            return true;

        uint32 tmp;
        if (!reader->ReadU32(&sender_report_.ntp_seconds) || !reader->ReadU32(&sender_report_.ntp_fraction) || !reader->ReadU32(&sender_report_.rtp_timestamp) || !reader->ReadU32(&sender_report_.send_packet_count) || !reader->ReadU32(&tmp))
            return false;
        sender_report_.send_octet_count = tmp;
        has_sender_report_ = true;

        for (size_t block = 0; block < header.IC; block++)
            if (!ParseReportBlock(reader))
                return false;

        return true;
    }

    bool RtcpParser::ParseRR(base::BigEndianReader* reader,
        const RtcpCommonHeader& header)
    {
        uint32 receiver_ssrc;
        if (!reader->ReadU32(&receiver_ssrc))
            return false;

        if (receiver_ssrc != remote_ssrc_)
            return true;

        for (size_t block = 0; block < header.IC; block++)
            if (!ParseReportBlock(reader))
                return false;

        return true;
    }

    bool RtcpParser::ParseReportBlock(base::BigEndianReader* reader)
    {
        uint32 ssrc, last_report, delay;
        if (!reader->ReadU32(&ssrc) || !reader->Skip(12) || !reader->ReadU32(&last_report) || !reader->ReadU32(&delay))
            return false;

        if (ssrc == local_ssrc_) {
            last_report_ = last_report;
            delay_since_last_report_ = delay;
            has_last_report_ = true;
        }

        return true;
    }

    bool RtcpParser::ParseApplicationDefined(base::BigEndianReader* reader,
        const RtcpCommonHeader& header)
    {
        uint32 sender_ssrc;
        uint32 name;
        if (!reader->ReadU32(&sender_ssrc) || !reader->ReadU32(&name))
            return false;

        if (sender_ssrc != remote_ssrc_)
            return true;

        if (name != kCast)
            return false;

        switch (header.IC /* subtype */) {
        case kReceiverLogSubtype:
            if (!ParseCastReceiverLogFrameItem(reader))
                return false;
            break;
        }
        return true;
    }

    bool RtcpParser::ParseCastReceiverLogFrameItem(
        base::BigEndianReader* reader)
    {

        while (reader->remaining()) {
            uint32 rtp_timestamp;
            uint32 data;
            if (!reader->ReadU32(&rtp_timestamp) || !reader->ReadU32(&data))
                return false;

            // We have 24 LSB of the event timestamp base on the wire.
            base::TimeTicks event_timestamp_base = base::TimeTicks() + base::TimeDelta::FromMilliseconds(data & 0xffffff);

            size_t num_events = 1 + static_cast<uint8>(data >> 24);

            RtcpReceiverFrameLogMessage frame_log(rtp_timestamp);
            for (size_t event = 0; event < num_events; event++) {
                uint16 delay_delta_or_packet_id;
                uint16 event_type_and_timestamp_delta;
                if (!reader->ReadU16(&delay_delta_or_packet_id) || !reader->ReadU16(&event_type_and_timestamp_delta))
                    return false;

                RtcpReceiverEventLogMessage event_log;
                event_log.type = TranslateToLogEventFromWireFormat(
                    static_cast<uint8>(event_type_and_timestamp_delta >> 12));
                event_log.event_timestamp = event_timestamp_base + base::TimeDelta::FromMilliseconds(event_type_and_timestamp_delta & 0xfff);
                if (event_log.type == PACKET_RECEIVED) {
                    event_log.packet_id = delay_delta_or_packet_id;
                } else {
                    event_log.delay_delta = base::TimeDelta::FromMilliseconds(
                        static_cast<int16>(delay_delta_or_packet_id));
                }
                frame_log.event_log_messages_.push_back(event_log);
            }

            receiver_log_.push_back(frame_log);
        }

        return true;
    }

    // RFC 4585.
    bool RtcpParser::ParseFeedbackCommon(base::BigEndianReader* reader,
        const RtcpCommonHeader& header)
    {
        // See RTC 4585 Section 6.4 for application specific feedback messages.
        if (header.IC != 15) {
            return true;
        }
        uint32 remote_ssrc;
        uint32 media_ssrc;
        if (!reader->ReadU32(&remote_ssrc) || !reader->ReadU32(&media_ssrc))
            return false;

        if (remote_ssrc != remote_ssrc_)
            return true;

        uint32 name;
        if (!reader->ReadU32(&name))
            return false;

        if (name != kCast) {
            return true;
        }

        cast_message_.media_ssrc = remote_ssrc;

        uint8 last_frame_id;
        uint8 number_of_lost_fields;
        if (!reader->ReadU8(&last_frame_id) || !reader->ReadU8(&number_of_lost_fields) || !reader->ReadU16(&cast_message_.target_delay_ms))
            return false;

        // Please note, this frame_id is still only 8-bit!
        cast_message_.ack_frame_id = last_frame_id;

        for (size_t i = 0; i < number_of_lost_fields; i++) {
            uint8 frame_id;
            uint16 packet_id;
            uint8 bitmask;
            if (!reader->ReadU8(&frame_id) || !reader->ReadU16(&packet_id) || !reader->ReadU8(&bitmask))
                return false;
            cast_message_.missing_frames_and_packets[frame_id].insert(packet_id);
            if (packet_id != kRtcpCastAllPacketsLost) {
                while (bitmask) {
                    packet_id++;
                    if (bitmask & 1)
                        cast_message_.missing_frames_and_packets[frame_id].insert(packet_id);
                    bitmask >>= 1;
                }
            }
        }

        has_cast_message_ = true;
        return true;
    }

    bool RtcpParser::ParseExtendedReport(base::BigEndianReader* reader,
        const RtcpCommonHeader& header)
    {
        uint32 remote_ssrc;
        if (!reader->ReadU32(&remote_ssrc))
            return false;

        // Is it for us?
        if (remote_ssrc != remote_ssrc_)
            return true;

        while (reader->remaining()) {
            uint8 block_type;
            uint16 block_length;
            if (!reader->ReadU8(&block_type) || !reader->Skip(1) || !reader->ReadU16(&block_length))
                return false;

            switch (block_type) {
            case 4: // RRTR. RFC3611 Section 4.4.
                if (block_length != 2)
                    return false;
                if (!ParseExtendedReportReceiverReferenceTimeReport(reader,
                        remote_ssrc))
                    return false;
                break;

            default:
                // Skip unknown item.
                if (!reader->Skip(block_length * 4))
                    return false;
            }
        }

        return true;
    }

    bool RtcpParser::ParseExtendedReportReceiverReferenceTimeReport(
        base::BigEndianReader* reader,
        uint32 remote_ssrc)
    {
        receiver_reference_time_report_.remote_ssrc = remote_ssrc;
        if (!reader->ReadU32(&receiver_reference_time_report_.ntp_seconds) || !reader->ReadU32(&receiver_reference_time_report_.ntp_fraction))
            return false;

        has_receiver_reference_time_report_ = true;
        return true;
    }

    // Converts a log event type to an integer value.
    // NOTE: We have only allocated 4 bits to represent the type of event over the
    // wire. Therefore, this function can only return values from 0 to 15.
    uint8 ConvertEventTypeToWireFormat(CastLoggingEvent event)
    {
        switch (event) {
        case FRAME_ACK_SENT:
            return 11;
        case FRAME_PLAYOUT:
            return 12;
        case FRAME_DECODED:
            return 13;
        case PACKET_RECEIVED:
            return 14;
        default:
            return 0; // Not an interesting event.
        }
    }

    CastLoggingEvent TranslateToLogEventFromWireFormat(uint8 event)
    {
        // TODO(imcheng): Remove the old mappings once they are no longer used.
        switch (event) {
        case 1: // AudioAckSent
        case 5: // VideoAckSent
        case 11: // Unified
            return FRAME_ACK_SENT;
        case 2: // AudioPlayoutDelay
        case 7: // VideoRenderDelay
        case 12: // Unified
            return FRAME_PLAYOUT;
        case 3: // AudioFrameDecoded
        case 6: // VideoFrameDecoded
        case 13: // Unified
            return FRAME_DECODED;
        case 4: // AudioPacketReceived
        case 8: // VideoPacketReceived
        case 14: // Unified
            return PACKET_RECEIVED;
        case 9: // DuplicateAudioPacketReceived
        case 10: // DuplicateVideoPacketReceived
        default:
            // If the sender adds new log messages we will end up here until we add
            // the new messages in the receiver.
            VLOG(1) << "Unexpected log message received: " << static_cast<int>(event);
            return UNKNOWN;
        }
    }

} // namespace cast
} // namespace media
