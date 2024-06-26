// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_CAST_NET_RTP_RTP_PARSER_H_
#define MEDIA_CAST_NET_RTP_RTP_PARSER_H_

#include "media/cast/net/cast_transport_defines.h"
#include "media/cast/net/rtp/rtp_defines.h"

namespace media {
namespace cast {

    // TODO(miu): RtpParser and RtpPacketizer should be consolidated into a single
    // module that handles all RTP/Cast packet serialization and deserialization
    // throughout the media/cast library.
    class RtpParser {
    public:
        RtpParser(uint32 expected_sender_ssrc, uint8 expected_payload_type);

        virtual ~RtpParser();

        // Parses the |packet|, expecting an RTP header along with a Cast header at
        // the beginning of the the RTP payload.  This method populates the structure
        // pointed to by |rtp_header| and sets the |payload_data| pointer and
        // |payload_size| to the memory region within |packet| containing the Cast
        // payload data.  Returns false if the data appears to be invalid, is not from
        // the expected sender (as identified by the SSRC field), or is not the
        // expected payload type.
        bool ParsePacket(const uint8* packet,
            size_t length,
            RtpCastHeader* rtp_header,
            const uint8** payload_data,
            size_t* payload_size);

        static bool ParseSsrc(const uint8* packet, size_t length, uint32* ssrc);

    private:
        const uint32 expected_sender_ssrc_;
        const uint8 expected_payload_type_;
        FrameIdWrapHelper frame_id_wrap_helper_;

        DISALLOW_COPY_AND_ASSIGN(RtpParser);
    };

} // namespace cast
} // namespace media

#endif // MEDIA_CAST_NET_RTP_RTP_PARSER_H_
