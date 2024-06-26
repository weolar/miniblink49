// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/cast/logging/logging_defines.h"

#include "base/logging.h"

#define ENUM_TO_STRING(enum) \
    case enum:               \
        return #enum

namespace media {
namespace cast {

    const char* CastLoggingToString(CastLoggingEvent event)
    {
        switch (event) {
            ENUM_TO_STRING(UNKNOWN);
            ENUM_TO_STRING(FRAME_CAPTURE_BEGIN);
            ENUM_TO_STRING(FRAME_CAPTURE_END);
            ENUM_TO_STRING(FRAME_ENCODED);
            ENUM_TO_STRING(FRAME_ACK_RECEIVED);
            ENUM_TO_STRING(FRAME_ACK_SENT);
            ENUM_TO_STRING(FRAME_DECODED);
            ENUM_TO_STRING(FRAME_PLAYOUT);
            ENUM_TO_STRING(PACKET_SENT_TO_NETWORK);
            ENUM_TO_STRING(PACKET_RETRANSMITTED);
            ENUM_TO_STRING(PACKET_RTX_REJECTED);
            ENUM_TO_STRING(PACKET_RECEIVED);
        }
        NOTREACHED();
        return "";
    }

    FrameEvent::FrameEvent()
        : rtp_timestamp(0u)
        , frame_id(kFrameIdUnknown)
        , width(0)
        , height(0)
        , size(0u)
        , type(UNKNOWN)
        , media_type(UNKNOWN_EVENT)
        , key_frame(false)
        , target_bitrate(0)
        , encoder_cpu_utilization(-1.0)
        , idealized_bitrate_utilization(-1.0)
    {
    }
    FrameEvent::~FrameEvent() { }

    PacketEvent::PacketEvent()
        : rtp_timestamp(0)
        , frame_id(kFrameIdUnknown)
        , max_packet_id(0)
        , packet_id(0)
        , size(0)
        , type(UNKNOWN)
        , media_type(UNKNOWN_EVENT)
    {
    }
    PacketEvent::~PacketEvent() { }

} // namespace cast
} // namespace media
