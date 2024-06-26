// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_CAST_NET_PACING_PACED_SENDER_H_
#define MEDIA_CAST_NET_PACING_PACED_SENDER_H_

#include <map>
#include <vector>

#include "base/basictypes.h"
#include "base/memory/scoped_ptr.h"
#include "base/memory/weak_ptr.h"
#include "base/single_thread_task_runner.h"
#include "base/threading/non_thread_safe.h"
#include "base/time/default_tick_clock.h"
#include "base/time/tick_clock.h"
#include "base/time/time.h"
#include "media/cast/logging/logging_defines.h"
#include "media/cast/net/cast_transport_config.h"

namespace media {
namespace cast {

    // Meant to use as defaults for pacer construction.
    static const size_t kTargetBurstSize = 10;
    static const size_t kMaxBurstSize = 20;

    // Use std::pair for free comparison operators.
    // { capture_time, ssrc, packet_id }
    // The PacketKey is designed to meet two criteria:
    // 1. When we re-send the same packet again, we can use the packet key
    //    to identify it so that we can de-duplicate packets in the queue.
    // 2. The sort order of the PacketKey determines the order that packets
    //    are sent out. Using the capture_time as the first member basically
    //    means that older packets are sent first.
    typedef std::pair<base::TimeTicks, std::pair<uint32, uint16>> PacketKey;
    typedef std::vector<std::pair<PacketKey, PacketRef>> SendPacketVector;

    // Information used to deduplicate retransmission packets.
    // There are two criteria for deduplication.
    //
    // 1. Using another muxed stream.
    //    Suppose there are multiple streams muxed and sent via the same
    //    socket. When there is a retransmission request for packet X, we
    //    will reject the retransmission if there is a packet sent from
    //    another stream just before X but not acked. Typically audio stream
    //    is used for this purpose. |last_byte_acked_for_audio| provides this
    //    information.
    //
    // 2. Using a time interval.
    //    Time between sending the same packet must be greater than
    //    |resend_interval|.
    struct DedupInfo {
        DedupInfo();
        base::TimeDelta resend_interval;
        int64 last_byte_acked_for_audio;
    };

    // We have this pure virtual class to enable mocking.
    class PacedPacketSender {
    public:
        virtual bool SendPackets(const SendPacketVector& packets) = 0;
        virtual bool ResendPackets(const SendPacketVector& packets,
            const DedupInfo& dedup_info)
            = 0;
        virtual bool SendRtcpPacket(uint32 ssrc, PacketRef packet) = 0;
        virtual void CancelSendingPacket(const PacketKey& packet_key) = 0;

        virtual ~PacedPacketSender() { }

        static PacketKey MakePacketKey(const base::TimeTicks& ticks,
            uint32 ssrc,
            uint16 packet_id);
    };

    class PacedSender : public PacedPacketSender,
                        public base::NonThreadSafe,
                        public base::SupportsWeakPtr<PacedSender> {
    public:
        // |recent_packet_events| is an externally-owned vector where PacedSender will
        // add PacketEvents related to sending, retransmission, and rejection.  The
        // |external_transport| should only be used by the Cast receiver and for
        // testing.
        PacedSender(
            size_t target_burst_size, // Should normally be kTargetBurstSize.
            size_t max_burst_size, // Should normally be kMaxBurstSize.
            base::TickClock* clock,
            std::vector<PacketEvent>* recent_packet_events,
            PacketSender* external_transport,
            const scoped_refptr<base::SingleThreadTaskRunner>& transport_task_runner);

        ~PacedSender() final;

        // These must be called before non-RTCP packets are sent.
        void RegisterAudioSsrc(uint32 audio_ssrc);
        void RegisterVideoSsrc(uint32 video_ssrc);

        // Register SSRC that has a higher priority for sending. Multiple SSRCs can
        // be registered.
        // Note that it is not expected to register many SSRCs with this method.
        // Because IsHigherPriority() is determined in linear time.
        void RegisterPrioritySsrc(uint32 ssrc);

        // Returns the total number of bytes sent to the socket when the specified
        // packet was just sent.
        // Returns 0 if the packet cannot be found or not yet sent.
        int64 GetLastByteSentForPacket(const PacketKey& packet_key);

        // Returns the total number of bytes sent to the socket when the last payload
        // identified by SSRC is just sent.
        int64 GetLastByteSentForSsrc(uint32 ssrc);

        // PacedPacketSender implementation.
        bool SendPackets(const SendPacketVector& packets) final;
        bool ResendPackets(const SendPacketVector& packets,
            const DedupInfo& dedup_info) final;
        bool SendRtcpPacket(uint32 ssrc, PacketRef packet) final;
        void CancelSendingPacket(const PacketKey& packet_key) final;

    private:
        // Actually sends the packets to the transport.
        void SendStoredPackets();

        // Convenience method for building a PacketEvent and storing it in the
        // externally-owned container of |recent_packet_events_|.
        void LogPacketEvent(const Packet& packet, CastLoggingEvent event);

        // Returns true if retransmission for packet indexed by |packet_key| is
        // accepted. |dedup_info| contains information to help deduplicate
        // retransmission. |now| is the current time to save on fetching it from the
        // clock multiple times.
        bool ShouldResend(const PacketKey& packet_key,
            const DedupInfo& dedup_info,
            const base::TimeTicks& now);

        enum PacketType {
            PacketType_RTCP,
            PacketType_Resend,
            PacketType_Normal
        };
        enum State {
            // In an unblocked state, we can send more packets.
            // We have to check the current time against |burst_end_| to see if we are
            // appending to the current burst or if we can start a new one.
            State_Unblocked,
            // In this state, we are waiting for a callback from the udp transport.
            // This happens when the OS-level buffer is full. Once we receive the
            // callback, we go to State_Unblocked and see if we can write more packets
            // to the current burst. (Or the next burst if enough time has passed.)
            State_TransportBlocked,
            // Once we've written enough packets for a time slice, we go into this
            // state and PostDelayTask a call to ourselves to wake up when we can
            // send more data.
            State_BurstFull
        };

        bool empty() const;
        size_t size() const;

        // Returns the next packet to send. RTCP packets have highest priority,
        // resend packets have second highest priority and then comes everything
        // else.
        PacketRef PopNextPacket(PacketType* packet_type,
            PacketKey* packet_key);

        // Returns true if the packet should have a higher priority.
        bool IsHighPriority(const PacketKey& packet_key) const;

        // These are externally-owned objects injected via the constructor.
        base::TickClock* const clock_;
        std::vector<PacketEvent>* const recent_packet_events_;
        PacketSender* const transport_;

        scoped_refptr<base::SingleThreadTaskRunner> transport_task_runner_;
        uint32 audio_ssrc_;
        uint32 video_ssrc_;

        // Set of SSRCs that have higher priority. This is a vector instead of a
        // set because there's only very few in it (most likely 1).
        std::vector<uint32> priority_ssrcs_;
        typedef std::map<PacketKey, std::pair<PacketType, PacketRef>> PacketList;
        PacketList packet_list_;
        PacketList priority_packet_list_;

        struct PacketSendRecord {
            PacketSendRecord();
            base::TimeTicks time; // Time when the packet was sent.
            int64 last_byte_sent; // Number of bytes sent to network just after this
                // packet was sent.
            int64 last_byte_sent_for_audio; // Number of bytes sent to network from
                // audio stream just before this packet.
        };
        typedef std::map<PacketKey, PacketSendRecord> PacketSendHistory;
        PacketSendHistory send_history_;
        PacketSendHistory send_history_buffer_;
        // Records the last byte sent for payload with a specific SSRC.
        std::map<uint32, int64> last_byte_sent_;

        size_t target_burst_size_;
        size_t max_burst_size_;

        // Maximum burst size for the next three bursts.
        size_t current_max_burst_size_;
        size_t next_max_burst_size_;
        size_t next_next_max_burst_size_;
        // Number of packets already sent in the current burst.
        size_t current_burst_size_;
        // This is when the current burst ends.
        base::TimeTicks burst_end_;

        State state_;

        bool has_reached_upper_bound_once_;

        // NOTE: Weak pointers must be invalidated before all other member variables.
        base::WeakPtrFactory<PacedSender> weak_factory_;

        DISALLOW_COPY_AND_ASSIGN(PacedSender);
    };

} // namespace cast
} // namespace media

#endif // MEDIA_CAST_NET_PACING_PACED_SENDER_H_
