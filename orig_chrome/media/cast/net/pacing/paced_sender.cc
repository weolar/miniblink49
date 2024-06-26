// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/cast/net/pacing/paced_sender.h"

#include "base/big_endian.h"
#include "base/bind.h"
#include "base/debug/dump_without_crashing.h"
#include "base/message_loop/message_loop.h"

namespace media {
namespace cast {

    namespace {

        static const int64 kPacingIntervalMs = 10;
        // Each frame will be split into no more than kPacingMaxBurstsPerFrame
        // bursts of packets.
        static const size_t kPacingMaxBurstsPerFrame = 3;
        static const size_t kMaxDedupeWindowMs = 500;

        // "Impossible" upper-bound on the maximum number of packets that should ever be
        // enqueued in the pacer.  This is used to detect bugs, reported as crash dumps.
        static const size_t kHugeQueueLengthSeconds = 10;
        static const size_t kRidiculousNumberOfPackets = kHugeQueueLengthSeconds * (kMaxBurstSize * 1000 / kPacingIntervalMs);

    } // namespace

    DedupInfo::DedupInfo()
        : last_byte_acked_for_audio(0)
    {
    }

    // static
    PacketKey PacedPacketSender::MakePacketKey(const base::TimeTicks& ticks,
        uint32 ssrc,
        uint16 packet_id)
    {
        return std::make_pair(ticks, std::make_pair(ssrc, packet_id));
    }

    PacedSender::PacketSendRecord::PacketSendRecord()
        : last_byte_sent(0)
        , last_byte_sent_for_audio(0)
    {
    }

    PacedSender::PacedSender(
        size_t target_burst_size,
        size_t max_burst_size,
        base::TickClock* clock,
        std::vector<PacketEvent>* recent_packet_events,
        PacketSender* transport,
        const scoped_refptr<base::SingleThreadTaskRunner>& transport_task_runner)
        : clock_(clock)
        , recent_packet_events_(recent_packet_events)
        , transport_(transport)
        , transport_task_runner_(transport_task_runner)
        , audio_ssrc_(0)
        , video_ssrc_(0)
        , target_burst_size_(target_burst_size)
        , max_burst_size_(max_burst_size)
        , current_max_burst_size_(target_burst_size_)
        , next_max_burst_size_(target_burst_size_)
        , next_next_max_burst_size_(target_burst_size_)
        , current_burst_size_(0)
        , state_(State_Unblocked)
        , has_reached_upper_bound_once_(false)
        , weak_factory_(this)
    {
    }

    PacedSender::~PacedSender() { }

    void PacedSender::RegisterAudioSsrc(uint32 audio_ssrc)
    {
        audio_ssrc_ = audio_ssrc;
    }

    void PacedSender::RegisterVideoSsrc(uint32 video_ssrc)
    {
        video_ssrc_ = video_ssrc;
    }

    void PacedSender::RegisterPrioritySsrc(uint32 ssrc)
    {
        priority_ssrcs_.push_back(ssrc);
    }

    int64 PacedSender::GetLastByteSentForPacket(const PacketKey& packet_key)
    {
        PacketSendHistory::const_iterator it = send_history_.find(packet_key);
        if (it == send_history_.end())
            return 0;
        return it->second.last_byte_sent;
    }

    int64 PacedSender::GetLastByteSentForSsrc(uint32 ssrc)
    {
        std::map<uint32, int64>::const_iterator it = last_byte_sent_.find(ssrc);
        if (it == last_byte_sent_.end())
            return 0;
        return it->second;
    }

    bool PacedSender::SendPackets(const SendPacketVector& packets)
    {
        if (packets.empty()) {
            return true;
        }
        const bool high_priority = IsHighPriority(packets.begin()->first);
        for (size_t i = 0; i < packets.size(); i++) {
            DCHECK(IsHighPriority(packets[i].first) == high_priority);
            if (high_priority) {
                priority_packet_list_[packets[i].first] = make_pair(PacketType_Normal, packets[i].second);
            } else {
                packet_list_[packets[i].first] = make_pair(PacketType_Normal, packets[i].second);
            }
        }
        if (state_ == State_Unblocked) {
            SendStoredPackets();
        }
        return true;
    }

    bool PacedSender::ShouldResend(const PacketKey& packet_key,
        const DedupInfo& dedup_info,
        const base::TimeTicks& now)
    {
        PacketSendHistory::const_iterator it = send_history_.find(packet_key);

        // No history of previous transmission. It might be sent too long ago.
        if (it == send_history_.end())
            return true;

        // Suppose there is request to retransmit X and there is an audio
        // packet Y sent just before X. Reject retransmission of X if ACK for
        // Y has not been received.
        // Only do this for video packets.
        if (packet_key.second.first == video_ssrc_) {
            if (dedup_info.last_byte_acked_for_audio && it->second.last_byte_sent_for_audio && dedup_info.last_byte_acked_for_audio < it->second.last_byte_sent_for_audio) {
                return false;
            }
        }
        // Retransmission interval has to be greater than |resend_interval|.
        if (now - it->second.time < dedup_info.resend_interval)
            return false;
        return true;
    }

    bool PacedSender::ResendPackets(const SendPacketVector& packets,
        const DedupInfo& dedup_info)
    {
        if (packets.empty()) {
            return true;
        }
        const bool high_priority = IsHighPriority(packets.begin()->first);
        const base::TimeTicks now = clock_->NowTicks();
        for (size_t i = 0; i < packets.size(); i++) {
            if (!ShouldResend(packets[i].first, dedup_info, now)) {
                LogPacketEvent(packets[i].second->data, PACKET_RTX_REJECTED);
                continue;
            }

            DCHECK(IsHighPriority(packets[i].first) == high_priority);
            if (high_priority) {
                priority_packet_list_[packets[i].first] = make_pair(PacketType_Resend, packets[i].second);
            } else {
                packet_list_[packets[i].first] = make_pair(PacketType_Resend, packets[i].second);
            }
        }
        if (state_ == State_Unblocked) {
            SendStoredPackets();
        }
        return true;
    }

    bool PacedSender::SendRtcpPacket(uint32 ssrc, PacketRef packet)
    {
        if (state_ == State_TransportBlocked) {
            priority_packet_list_[PacedPacketSender::MakePacketKey(base::TimeTicks(), ssrc, 0)] = make_pair(PacketType_RTCP, packet);
        } else {
            // We pass the RTCP packets straight through.
            if (!transport_->SendPacket(
                    packet,
                    base::Bind(&PacedSender::SendStoredPackets,
                        weak_factory_.GetWeakPtr()))) {
                state_ = State_TransportBlocked;
            }
        }
        return true;
    }

    void PacedSender::CancelSendingPacket(const PacketKey& packet_key)
    {
        packet_list_.erase(packet_key);
        priority_packet_list_.erase(packet_key);
    }

    PacketRef PacedSender::PopNextPacket(PacketType* packet_type,
        PacketKey* packet_key)
    {
        PacketList* list = !priority_packet_list_.empty() ? &priority_packet_list_ : &packet_list_;
        DCHECK(!list->empty());
        PacketList::iterator i = list->begin();
        *packet_type = i->second.first;
        *packet_key = i->first;
        PacketRef ret = i->second.second;
        list->erase(i);
        return ret;
    }

    bool PacedSender::IsHighPriority(const PacketKey& packet_key) const
    {
        return std::find(priority_ssrcs_.begin(), priority_ssrcs_.end(),
                   packet_key.second.first)
            != priority_ssrcs_.end();
    }

    bool PacedSender::empty() const
    {
        return packet_list_.empty() && priority_packet_list_.empty();
    }

    size_t PacedSender::size() const
    {
        return packet_list_.size() + priority_packet_list_.size();
    }

    // This function can be called from three places:
    // 1. User called one of the Send* functions and we were in an unblocked state.
    // 2. state_ == State_TransportBlocked and the transport is calling us to
    //    let us know that it's ok to send again.
    // 3. state_ == State_BurstFull and there are still packets to send. In this
    //    case we called PostDelayedTask on this function to start a new burst.
    void PacedSender::SendStoredPackets()
    {
        State previous_state = state_;
        state_ = State_Unblocked;
        if (empty()) {
            return;
        }

        // If the queue ever becomes impossibly long, send a crash dump without
        // actually crashing the process.
        if (size() > kRidiculousNumberOfPackets && !has_reached_upper_bound_once_) {
            NOTREACHED();
            // Please use Cr=Internals-Cast label in bug reports:
            base::debug::DumpWithoutCrashing();
            has_reached_upper_bound_once_ = true;
        }

        base::TimeTicks now = clock_->NowTicks();
        // I don't actually trust that PostDelayTask(x - now) will mean that
        // now >= x when the call happens, so check if the previous state was
        // State_BurstFull too.
        if (now >= burst_end_ || previous_state == State_BurstFull) {
            // Start a new burst.
            current_burst_size_ = 0;
            burst_end_ = now + base::TimeDelta::FromMilliseconds(kPacingIntervalMs);

            // The goal here is to try to send out the queued packets over the next
            // three bursts, while trying to keep the burst size below 10 if possible.
            // We have some evidence that sending more than 12 packets in a row doesn't
            // work very well, but we don't actually know why yet. Sending out packets
            // sooner is better than sending out packets later as that gives us more
            // time to re-send them if needed. So if we have less than 30 packets, just
            // send 10 at a time. If we have less than 60 packets, send n / 3 at a time.
            // if we have more than 60, we send 20 at a time. 20 packets is ~24Mbit/s
            // which is more bandwidth than the cast library should need, and sending
            // out more data per second is unlikely to be helpful.
            size_t max_burst_size = std::min(
                max_burst_size_,
                std::max(target_burst_size_, size() / kPacingMaxBurstsPerFrame));
            current_max_burst_size_ = std::max(next_max_burst_size_, max_burst_size);
            next_max_burst_size_ = std::max(next_next_max_burst_size_, max_burst_size);
            next_next_max_burst_size_ = max_burst_size;
        }

        base::Closure cb = base::Bind(&PacedSender::SendStoredPackets,
            weak_factory_.GetWeakPtr());
        while (!empty()) {
            if (current_burst_size_ >= current_max_burst_size_) {
                transport_task_runner_->PostDelayedTask(FROM_HERE,
                    cb,
                    burst_end_ - now);
                state_ = State_BurstFull;
                return;
            }
            PacketType packet_type;
            PacketKey packet_key;
            PacketRef packet = PopNextPacket(&packet_type, &packet_key);
            PacketSendRecord send_record;
            send_record.time = now;

            switch (packet_type) {
            case PacketType_Resend:
                LogPacketEvent(packet->data, PACKET_RETRANSMITTED);
                break;
            case PacketType_Normal:
                LogPacketEvent(packet->data, PACKET_SENT_TO_NETWORK);
                break;
            case PacketType_RTCP:
                break;
            }

            const bool socket_blocked = !transport_->SendPacket(packet, cb);

            // Save the send record.
            send_record.last_byte_sent = transport_->GetBytesSent();
            send_record.last_byte_sent_for_audio = GetLastByteSentForSsrc(audio_ssrc_);
            send_history_[packet_key] = send_record;
            send_history_buffer_[packet_key] = send_record;
            last_byte_sent_[packet_key.second.first] = send_record.last_byte_sent;

            if (socket_blocked) {
                state_ = State_TransportBlocked;
                return;
            }
            current_burst_size_++;
        }

        // Keep ~0.5 seconds of data (1000 packets).
        if (send_history_buffer_.size() >= max_burst_size_ * kMaxDedupeWindowMs / kPacingIntervalMs) {
            send_history_.swap(send_history_buffer_);
            send_history_buffer_.clear();
        }
        DCHECK_LE(send_history_buffer_.size(),
            max_burst_size_ * kMaxDedupeWindowMs / kPacingIntervalMs);
        state_ = State_Unblocked;
    }

    void PacedSender::LogPacketEvent(const Packet& packet, CastLoggingEvent type)
    {
        if (!recent_packet_events_)
            return;

        recent_packet_events_->push_back(PacketEvent());
        PacketEvent& event = recent_packet_events_->back();

        // Populate the new PacketEvent by parsing the wire-format |packet|.
        //
        // TODO(miu): This parsing logic belongs in RtpParser.
        event.timestamp = clock_->NowTicks();
        event.type = type;
        base::BigEndianReader reader(reinterpret_cast<const char*>(&packet[0]),
            packet.size());
        bool success = reader.Skip(4);
        success &= reader.ReadU32(&event.rtp_timestamp);
        uint32 ssrc;
        success &= reader.ReadU32(&ssrc);
        if (ssrc == audio_ssrc_) {
            event.media_type = AUDIO_EVENT;
        } else if (ssrc == video_ssrc_) {
            event.media_type = VIDEO_EVENT;
        } else {
            DVLOG(3) << "Got unknown ssrc " << ssrc << " when logging packet event";
            return;
        }
        success &= reader.Skip(2);
        success &= reader.ReadU16(&event.packet_id);
        success &= reader.ReadU16(&event.max_packet_id);
        event.size = packet.size();
        DCHECK(success);
    }

} // namespace cast
} // namespace media
