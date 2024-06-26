// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// This is the main interface for the cast transport sender.  It accepts encoded
// frames (both audio and video), encrypts their encoded data, packetizes them
// and feeds them into a transport (e.g., UDP).

// Construction of the Cast Sender and the Cast Transport Sender should be done
// in the following order:
// 1. Create CastTransportSender.
// 2. Create CastSender (accepts CastTransportSender as an input).

// Destruction: The CastTransportSender is assumed to be valid as long as the
// CastSender is alive. Therefore the CastSender should be destructed before the
// CastTransportSender.

#ifndef MEDIA_CAST_NET_CAST_TRANSPORT_SENDER_H_
#define MEDIA_CAST_NET_CAST_TRANSPORT_SENDER_H_

#include "base/basictypes.h"
#include "base/callback.h"
#include "base/memory/scoped_ptr.h"
#include "base/single_thread_task_runner.h"
#include "base/threading/non_thread_safe.h"
#include "base/time/tick_clock.h"
#include "media/cast/logging/logging_defines.h"
#include "media/cast/net/cast_transport_config.h"
#include "media/cast/net/cast_transport_defines.h"
#include "media/cast/net/rtcp/receiver_rtcp_event_subscriber.h"
#include "media/cast/net/rtcp/rtcp_defines.h"
#include "net/base/ip_endpoint.h"

namespace base {
class DictionaryValue;
} // namespace base

namespace net {
class NetLog;
} // namespace net

namespace media {
namespace cast {
    struct RtpReceiverStatistics;
    struct RtcpTimeData;

    // Following the initialization of either audio or video an initialization
    // status will be sent via this callback.
    typedef base::Callback<void(CastTransportStatus status)>
        CastTransportStatusCallback;

    typedef base::Callback<void(scoped_ptr<std::vector<FrameEvent>>,
        scoped_ptr<std::vector<PacketEvent>>)>
        BulkRawEventsCallback;

    // The application should only trigger this class from the transport thread.
    class CastTransportSender : public base::NonThreadSafe {
    public:
        static scoped_ptr<CastTransportSender> Create(
            net::NetLog* net_log,
            base::TickClock* clock,
            const net::IPEndPoint& local_end_point,
            const net::IPEndPoint& remote_end_point,
            scoped_ptr<base::DictionaryValue> options,
            const CastTransportStatusCallback& status_callback,
            const BulkRawEventsCallback& raw_events_callback,
            base::TimeDelta raw_events_callback_interval,
            const PacketReceiverCallback& packet_callback,
            const scoped_refptr<base::SingleThreadTaskRunner>& transport_task_runner);

        virtual ~CastTransportSender() { }

        // Audio/Video initialization.
        // Encoded frames cannot be transmitted until the relevant initialize method
        // is called.
        virtual void InitializeAudio(const CastTransportRtpConfig& config,
            const RtcpCastMessageCallback& cast_message_cb,
            const RtcpRttCallback& rtt_cb)
            = 0;
        virtual void InitializeVideo(const CastTransportRtpConfig& config,
            const RtcpCastMessageCallback& cast_message_cb,
            const RtcpRttCallback& rtt_cb)
            = 0;

        // Encrypt, packetize and transmit |frame|. |ssrc| must refer to a
        // a channel already established with InitializeAudio / InitializeVideo.
        virtual void InsertFrame(uint32 ssrc, const EncodedFrame& frame) = 0;

        // Sends a RTCP sender report to the receiver.
        // |ssrc| is the SSRC for this report.
        // |current_time| is the current time reported by a tick clock.
        // |current_time_as_rtp_timestamp| is the corresponding RTP timestamp.
        virtual void SendSenderReport(
            uint32 ssrc,
            base::TimeTicks current_time,
            uint32 current_time_as_rtp_timestamp)
            = 0;

        // Cancels sending packets for the frames in the set.
        // |ssrc| is the SSRC for the stream.
        // |frame_ids| contains the IDs of the frames that will be cancelled.
        virtual void CancelSendingFrames(uint32 ssrc,
            const std::vector<uint32>& frame_ids)
            = 0;

        // Resends a frame or part of a frame to kickstart. This is used when the
        // stream appears to be stalled.
        virtual void ResendFrameForKickstart(uint32 ssrc, uint32 frame_id) = 0;

        // Returns a callback for receiving packets for testing purposes.
        virtual PacketReceiverCallback PacketReceiverForTesting();

        // The following functions are needed for receving.

        // Add a valid SSRC. This is used to verify that incoming packets
        // come from the right sender. Without valid SSRCs, the return address cannot
        // be automatically established.
        virtual void AddValidSsrc(uint32 ssrc) = 0;

        // Send an RTCP message from receiver to sender.
        virtual void SendRtcpFromRtpReceiver(
            uint32 ssrc,
            uint32 sender_ssrc,
            const RtcpTimeData& time_data,
            const RtcpCastMessage* cast_message,
            base::TimeDelta target_delay,
            const ReceiverRtcpEventSubscriber::RtcpEvents* rtcp_events,
            const RtpReceiverStatistics* rtp_receiver_statistics)
            = 0;
    };

} // namespace cast
} // namespace media

#endif // MEDIA_CAST_NET_CAST_TRANSPORT_SENDER_H_
