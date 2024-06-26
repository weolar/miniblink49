// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/cast/net/rtp/rtp_defines.h"

namespace media {
namespace cast {

    RtpCastHeader::RtpCastHeader()
        : marker(false)
        , payload_type(0)
        , sequence_number(0)
        , rtp_timestamp(0)
        , sender_ssrc(0)
        , is_key_frame(false)
        , frame_id(0)
        , packet_id(0)
        , max_packet_id(0)
        , reference_frame_id(0)
        , new_playout_delay_ms(0)
    {
    }

    RtpPayloadFeedback::~RtpPayloadFeedback() { }

} // namespace cast
} // namespace media
