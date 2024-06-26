// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_CAST_NET_RTP_RTP_DEFINES_H_
#define MEDIA_CAST_NET_RTP_RTP_DEFINES_H_

#include "base/basictypes.h"
#include "media/cast/net/rtcp/rtcp_defines.h"

namespace media {
namespace cast {

    static const uint16 kRtpHeaderLength = 12;
    static const uint16 kCastHeaderLength = 7;

    // RTP Header
    static const uint8 kRtpExtensionBitMask = 0x10;
    static const uint8 kRtpMarkerBitMask = 0x80;
    static const uint8 kRtpNumCsrcsMask = 0x0f;

    // Cast Header
    static const uint8 kCastKeyFrameBitMask = 0x80;
    static const uint8 kCastReferenceFrameIdBitMask = 0x40;
    static const uint8 kCastExtensionCountmask = 0x3f;

    // Cast RTP extensions.
    static const uint8 kCastRtpExtensionAdaptiveLatency = 1;

    struct RtpCastHeader {
        RtpCastHeader();
        // Elements from RTP packet header.
        bool marker;
        uint8 payload_type;
        uint16 sequence_number;
        uint32 rtp_timestamp;
        uint32 sender_ssrc;
        uint8 num_csrcs;

        // Elements from Cast header (at beginning of RTP payload).
        bool is_key_frame;
        bool is_reference;
        uint32 frame_id;
        uint16 packet_id;
        uint16 max_packet_id;
        uint32 reference_frame_id;
        uint16 new_playout_delay_ms;
        uint8 num_extensions;
    };

    class RtpPayloadFeedback {
    public:
        virtual void CastFeedback(const RtcpCastMessage& cast_feedback) = 0;

    protected:
        virtual ~RtpPayloadFeedback();
    };

} // namespace cast
} // namespace media

#endif // MEDIA_CAST_NET_RTP_RTP_DEFINES_H_
