// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_CAST_NET_CAST_TRANSPORT_CONFIG_H_
#define MEDIA_CAST_NET_CAST_TRANSPORT_CONFIG_H_

#include <string>
#include <vector>

#include "base/basictypes.h"
#include "base/callback.h"
#include "base/memory/linked_ptr.h"
#include "base/memory/ref_counted.h"
#include "base/stl_util.h"
#include "media/cast/net/cast_transport_defines.h"

namespace media {
namespace cast {

    enum Codec {
        CODEC_UNKNOWN,
        CODEC_AUDIO_OPUS,
        CODEC_AUDIO_PCM16,
        CODEC_AUDIO_AAC,
        CODEC_VIDEO_FAKE,
        CODEC_VIDEO_VP8,
        CODEC_VIDEO_H264,
        CODEC_LAST = CODEC_VIDEO_H264
    };

    struct CastTransportRtpConfig {
        CastTransportRtpConfig();
        ~CastTransportRtpConfig();

        // Identifier refering to this sender.
        uint32 ssrc;

        // Identifier for incoming RTCP traffic.
        uint32 feedback_ssrc;

        // RTP payload type enum: Specifies the type/encoding of frame data.
        int rtp_payload_type;

        // The AES crypto key and initialization vector.  Each of these strings
        // contains the data in binary form, of size kAesKeySize.  If they are empty
        // strings, crypto is not being used.
        std::string aes_key;
        std::string aes_iv_mask;
    };

    // A combination of metadata and data for one encoded frame.  This can contain
    // audio data or video data or other.
    struct EncodedFrame {
        enum Dependency {
            // "null" value, used to indicate whether |dependency| has been set.
            UNKNOWN_DEPENDENCY,

            // Not decodable without the reference frame indicated by
            // |referenced_frame_id|.
            DEPENDENT,

            // Independently decodable.
            INDEPENDENT,

            // Independently decodable, and no future frames will depend on any frames
            // before this one.
            KEY,

            DEPENDENCY_LAST = KEY
        };

        EncodedFrame();
        virtual ~EncodedFrame();

        // Convenience accessors to data as an array of uint8 elements.
        const uint8* bytes() const
        {
            return reinterpret_cast<uint8*>(string_as_array(
                const_cast<std::string*>(&data)));
        }
        uint8* mutable_bytes()
        {
            return reinterpret_cast<uint8*>(string_as_array(&data));
        }

        // Copies all data members except |data| to |dest|.
        // Does not modify |dest->data|.
        void CopyMetadataTo(EncodedFrame* dest) const;

        // This frame's dependency relationship with respect to other frames.
        Dependency dependency;

        // The label associated with this frame.  Implies an ordering relative to
        // other frames in the same stream.
        uint32 frame_id;

        // The label associated with the frame upon which this frame depends.  If
        // this frame does not require any other frame in order to become decodable
        // (e.g., key frames), |referenced_frame_id| must equal |frame_id|.
        uint32 referenced_frame_id;

        // The stream timestamp, on the timeline of the signal data.  For example, RTP
        // timestamps for audio are usually defined as the total number of audio
        // samples encoded in all prior frames.  A playback system uses this value to
        // detect gaps in the stream, and otherwise stretch the signal to match
        // playout targets.
        uint32 rtp_timestamp;

        // The common reference clock timestamp for this frame.  This value originates
        // from a sender and is used to provide lip synchronization between streams in
        // a receiver.  Thus, in the sender context, this is set to the time at which
        // the frame was captured/recorded.  In the receiver context, this is set to
        // the target playout time.  Over a sequence of frames, this time value is
        // expected to drift with respect to the elapsed time implied by the RTP
        // timestamps; and it may not necessarily increment with precise regularity.
        base::TimeTicks reference_time;

        // Playout delay for this and all future frames. Used by the Adaptive
        // Playout delay extension. Zero means no change.
        uint16 new_playout_delay_ms;

        // The encoded signal data.
        std::string data;
    };

    typedef std::vector<uint8> Packet;
    typedef scoped_refptr<base::RefCountedData<Packet>> PacketRef;
    typedef std::vector<PacketRef> PacketList;

    typedef base::Callback<void(scoped_ptr<Packet> packet)> PacketReceiverCallback;
    typedef base::Callback<bool(scoped_ptr<Packet> packet)>
        PacketReceiverCallbackWithStatus;

    class PacketSender {
    public:
        // Send a packet to the network. Returns false if the network is blocked
        // and we should wait for |cb| to be called. It is not allowed to called
        // SendPacket again until |cb| has been called. Any other errors that
        // occur will be reported through side channels, in such cases, this function
        // will return true indicating that the channel is not blocked.
        virtual bool SendPacket(PacketRef packet, const base::Closure& cb) = 0;

        // Returns the number of bytes ever sent.
        virtual int64 GetBytesSent() = 0;

        virtual ~PacketSender() { }
    };

    struct RtcpSenderInfo {
        RtcpSenderInfo();
        ~RtcpSenderInfo();
        // First three members are used for lipsync.
        // First two members are used for rtt.
        uint32 ntp_seconds;
        uint32 ntp_fraction;
        uint32 rtp_timestamp;
        uint32 send_packet_count;
        size_t send_octet_count;
    };

    struct RtcpReportBlock {
        RtcpReportBlock();
        ~RtcpReportBlock();
        uint32 remote_ssrc; // SSRC of sender of this report.
        uint32 media_ssrc; // SSRC of the RTP packet sender.
        uint8 fraction_lost;
        uint32 cumulative_lost; // 24 bits valid.
        uint32 extended_high_sequence_number;
        uint32 jitter;
        uint32 last_sr;
        uint32 delay_since_last_sr;
    };

    struct RtcpDlrrReportBlock {
        RtcpDlrrReportBlock();
        ~RtcpDlrrReportBlock();
        uint32 last_rr;
        uint32 delay_since_last_rr;
    };

    inline bool operator==(RtcpSenderInfo lhs, RtcpSenderInfo rhs)
    {
        return lhs.ntp_seconds == rhs.ntp_seconds && lhs.ntp_fraction == rhs.ntp_fraction && lhs.rtp_timestamp == rhs.rtp_timestamp && lhs.send_packet_count == rhs.send_packet_count && lhs.send_octet_count == rhs.send_octet_count;
    }

} // namespace cast
} // namespace media

#endif // MEDIA_CAST_NET_CAST_TRANSPORT_CONFIG_H_
