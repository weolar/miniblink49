// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// This class maintains a send transport for audio and video in a Cast
// Streaming session.
// Audio, video frames and RTCP messages are submitted to this object
// and then packetized and paced to the underlying UDP socket.
//
// The hierarchy of send transport in a Cast Streaming session:
//
// CastTransportSender              RTP                      RTCP
// ------------------------------------------------------------------
//                      TransportEncryptionHandler (A/V)
//                      RtpSender (A/V)                   Rtcp (A/V)
//                                      PacedSender (Shared)
//                                      UdpTransport (Shared)
//
// There are objects of TransportEncryptionHandler, RtpSender and Rtcp
// for each audio and video stream.
// PacedSender and UdpTransport are shared between all RTP and RTCP
// streams.

#ifndef MEDIA_CAST_NET_CAST_TRANSPORT_SENDER_IMPL_H_
#define MEDIA_CAST_NET_CAST_TRANSPORT_SENDER_IMPL_H_

#include <set>
#include <vector>

#include "base/callback.h"
#include "base/gtest_prod_util.h"
#include "base/memory/ref_counted.h"
#include "base/memory/scoped_ptr.h"
#include "base/memory/weak_ptr.h"
#include "base/time/tick_clock.h"
#include "base/time/time.h"
#include "media/cast/common/transport_encryption_handler.h"
#include "media/cast/logging/logging_defines.h"
#include "media/cast/net/cast_transport_config.h"
#include "media/cast/net/cast_transport_sender.h"
#include "media/cast/net/pacing/paced_sender.h"
#include "media/cast/net/rtcp/rtcp.h"
#include "media/cast/net/rtp/rtp_parser.h"
#include "media/cast/net/rtp/rtp_sender.h"
#include "net/base/network_interfaces.h"

namespace media {
namespace cast {

    class UdpTransport;

    class CastTransportSenderImpl : public CastTransportSender {
    public:
        // |external_transport| is only used for testing.
        // |raw_events_callback|: Raw events will be returned on this callback
        // which will be invoked every |raw_events_callback_interval|.
        // This can be a null callback, i.e. if user is not interested in raw events.
        // |raw_events_callback_interval|: This can be |base::TimeDelta()| if
        // |raw_events_callback| is a null callback.
        // |options| contains optional settings for the transport, possible
        // keys are:
        //   "DSCP" (value ignored) - turns DSCP on
        //   "non_blocking_io" (value ignored) - Windows only.
        //                                       Turns on non-blocking IO for socket.
        //   "pacer_target_burst_size": int - specifies how many packets to send
        //                                    per 10 ms ideally.
        //   "pacer_max_burst_size": int - specifies how many pakcets to send
        //                                 per 10 ms, max
        //   "send_buffer_min_size": int - specifies the minimum socket send buffer
        //                                 size
        //   "disable_wifi_scan" (value ignored) - disable wifi scans while streaming
        //   "media_streaming_mode" (value ignored) - turn media streaming mode on
        // Note, these options may be ignored on some platforms.
        // TODO(hubbe): Too many callbacks, replace with an interface.
        CastTransportSenderImpl(
            net::NetLog* net_log,
            base::TickClock* clock,
            const net::IPEndPoint& local_end_point,
            const net::IPEndPoint& remote_end_point,
            scoped_ptr<base::DictionaryValue> options,
            const CastTransportStatusCallback& status_callback,
            const BulkRawEventsCallback& raw_events_callback,
            base::TimeDelta raw_events_callback_interval,
            const scoped_refptr<base::SingleThreadTaskRunner>& transport_task_runner,
            const PacketReceiverCallback& packet_callback,
            PacketSender* external_transport);

        ~CastTransportSenderImpl() final;

        // CastTransportSender implementation.
        void InitializeAudio(const CastTransportRtpConfig& config,
            const RtcpCastMessageCallback& cast_message_cb,
            const RtcpRttCallback& rtt_cb) final;
        void InitializeVideo(const CastTransportRtpConfig& config,
            const RtcpCastMessageCallback& cast_message_cb,
            const RtcpRttCallback& rtt_cb) final;
        void InsertFrame(uint32 ssrc, const EncodedFrame& frame) final;

        void SendSenderReport(uint32 ssrc,
            base::TimeTicks current_time,
            uint32 current_time_as_rtp_timestamp) final;

        void CancelSendingFrames(uint32 ssrc,
            const std::vector<uint32>& frame_ids) final;

        void ResendFrameForKickstart(uint32 ssrc, uint32 frame_id) final;

        PacketReceiverCallback PacketReceiverForTesting() final;

        // CastTransportReceiver implementation.
        void AddValidSsrc(uint32 ssrc) final;

        void SendRtcpFromRtpReceiver(
            uint32 ssrc,
            uint32 sender_ssrc,
            const RtcpTimeData& time_data,
            const RtcpCastMessage* cast_message,
            base::TimeDelta target_delay,
            const ReceiverRtcpEventSubscriber::RtcpEvents* rtcp_events,
            const RtpReceiverStatistics* rtp_receiver_statistics) final;

    private:
        FRIEND_TEST_ALL_PREFIXES(CastTransportSenderImplTest, NacksCancelRetransmits);
        FRIEND_TEST_ALL_PREFIXES(CastTransportSenderImplTest, CancelRetransmits);
        FRIEND_TEST_ALL_PREFIXES(CastTransportSenderImplTest, Kickstart);
        FRIEND_TEST_ALL_PREFIXES(CastTransportSenderImplTest,
            DedupRetransmissionWithAudio);

        // Resend packets for the stream identified by |ssrc|.
        // If |cancel_rtx_if_not_in_list| is true then transmission of packets for the
        // frames but not in the list will be dropped.
        // See PacedSender::ResendPackets() to see how |dedup_info| works.
        void ResendPackets(uint32 ssrc,
            const MissingFramesAndPacketsMap& missing_packets,
            bool cancel_rtx_if_not_in_list,
            const DedupInfo& dedup_info);

        // If |raw_events_callback_| is non-null, calls it with events collected
        // in |recent_frame_events_| and |recent_packet_events_| since last call.
        void SendRawEvents();

        // Called when a packet is received.
        bool OnReceivedPacket(scoped_ptr<Packet> packet);

        // Called when a log message is received.
        void OnReceivedLogMessage(EventMediaType media_type,
            const RtcpReceiverLogMessage& log);

        // Called when a RTCP Cast message is received.
        void OnReceivedCastMessage(uint32 ssrc,
            const RtcpCastMessageCallback& cast_message_cb,
            const RtcpCastMessage& cast_message);

        base::TickClock* clock_; // Not owned by this class.
        CastTransportStatusCallback status_callback_;
        scoped_refptr<base::SingleThreadTaskRunner> transport_task_runner_;

        // FrameEvents and PacketEvents pending delivery via |raw_events_callback_|.
        // Do not add elements to these when |raw_events_callback_.is_null()|.
        std::vector<FrameEvent> recent_frame_events_;
        std::vector<PacketEvent> recent_packet_events_;

        // Interface to a UDP socket.
        scoped_ptr<UdpTransport> transport_;

        // Packet sender that performs pacing.
        PacedSender pacer_;

        // Packetizer for audio and video frames.
        scoped_ptr<RtpSender> audio_sender_;
        scoped_ptr<RtpSender> video_sender_;

        // Maintains RTCP session for audio and video.
        scoped_ptr<Rtcp> audio_rtcp_session_;
        scoped_ptr<Rtcp> video_rtcp_session_;

        // Encrypts data in EncodedFrames before they are sent.  Note that it's
        // important for the encryption to happen here, in code that would execute in
        // the main browser process, for security reasons.  This helps to mitigate
        // the damage that could be caused by a compromised renderer process.
        TransportEncryptionHandler audio_encryptor_;
        TransportEncryptionHandler video_encryptor_;

        BulkRawEventsCallback raw_events_callback_;
        base::TimeDelta raw_events_callback_interval_;

        // Right after a frame is sent we record the number of bytes sent to the
        // socket. We record the corresponding bytes sent for the most recent ACKed
        // audio packet.
        int64 last_byte_acked_for_audio_;

        // Packets that don't match these ssrcs are ignored.
        std::set<uint32> valid_ssrcs_;

        // Called with incoming packets. (Unless they match the
        // channels created by Initialize{Audio,Video}.
        PacketReceiverCallback packet_callback_;

        scoped_ptr<net::ScopedWifiOptions> wifi_options_autoreset_;

        base::WeakPtrFactory<CastTransportSenderImpl> weak_factory_;

        DISALLOW_COPY_AND_ASSIGN(CastTransportSenderImpl);
    };

} // namespace cast
} // namespace media

#endif // MEDIA_CAST_NET_CAST_TRANSPORT_SENDER_IMPL_H_
