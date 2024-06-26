// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_CAST_NET_CAST_TRANSPORT_DEFINES_H_
#define MEDIA_CAST_NET_CAST_TRANSPORT_DEFINES_H_

#include <stdint.h>

#include <map>
#include <set>
#include <string>

#include "base/basictypes.h"
#include "base/time/time.h"

namespace media {
namespace cast {

    // TODO(mikhal): Implement and add more types.
    enum CastTransportStatus {
        TRANSPORT_AUDIO_UNINITIALIZED = 0,
        TRANSPORT_VIDEO_UNINITIALIZED,
        TRANSPORT_AUDIO_INITIALIZED,
        TRANSPORT_VIDEO_INITIALIZED,
        TRANSPORT_INVALID_CRYPTO_CONFIG,
        TRANSPORT_SOCKET_ERROR,
        CAST_TRANSPORT_STATUS_LAST = TRANSPORT_SOCKET_ERROR
    };

    const size_t kMaxIpPacketSize = 1500;
    // Each uint16 represents one packet id within a cast frame.
    typedef std::set<uint16> PacketIdSet;
    // Each uint8 represents one cast frame.
    typedef std::map<uint8, PacketIdSet> MissingFramesAndPacketsMap;

    // Rtcp defines.

    enum RtcpPacketFields {
        kPacketTypeLow = 194, // SMPTE time-code mapping.
        kPacketTypeSenderReport = 200,
        kPacketTypeReceiverReport = 201,
        kPacketTypeApplicationDefined = 204,
        kPacketTypeGenericRtpFeedback = 205,
        kPacketTypePayloadSpecific = 206,
        kPacketTypeXr = 207,
        kPacketTypeHigh = 210, // Port Mapping.
    };

    // Each uint16 represents one packet id within a cast frame.
    typedef std::set<uint16> PacketIdSet;
    // Each uint8 represents one cast frame.
    typedef std::map<uint8, PacketIdSet> MissingFramesAndPacketsMap;

    class FrameIdWrapHelperTest;

    // TODO(miu): UGLY IN-LINE DEFINITION IN HEADER FILE!  Move to appropriate
    // location, separated into .h and .cc files.
    class FrameIdWrapHelper {
    public:
        FrameIdWrapHelper()
            : largest_frame_id_seen_(kStartFrameId)
        {
        }

        uint32 MapTo32bitsFrameId(const uint8 over_the_wire_frame_id)
        {
            uint32 ret = (largest_frame_id_seen_ & ~0xff) | over_the_wire_frame_id;
            // Add 1000 to both sides to avoid underflows.
            if (1000 + ret - largest_frame_id_seen_ > 1000 + 127) {
                ret -= 0x100;
            } else if (1000 + ret - largest_frame_id_seen_ < 1000 - 128) {
                ret += 0x100;
            }
            if (1000 + ret - largest_frame_id_seen_ > 1000) {
                largest_frame_id_seen_ = ret;
            }
            return ret;
        }

    private:
        friend class FrameIdWrapHelperTest;
        static const uint32 kStartFrameId = UINT32_C(0xffffffff);

        uint32 largest_frame_id_seen_;

        DISALLOW_COPY_AND_ASSIGN(FrameIdWrapHelper);
    };

} // namespace cast
} // namespace media

#endif // MEDIA_CAST_NET_CAST_TRANSPORT_DEFINES_H_
