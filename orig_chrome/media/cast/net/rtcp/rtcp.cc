// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/cast/net/rtcp/rtcp.h"

#include "media/cast/cast_config.h"
#include "media/cast/cast_defines.h"
#include "media/cast/cast_environment.h"
#include "media/cast/net/cast_transport_defines.h"
#include "media/cast/net/pacing/paced_sender.h"
#include "media/cast/net/rtcp/rtcp_builder.h"
#include "media/cast/net/rtcp/rtcp_defines.h"
#include "media/cast/net/rtcp/rtcp_utility.h"

using base::TimeDelta;

namespace media {
namespace cast {

    static const int32 kStatsHistoryWindowMs = 10000; // 10 seconds.
    // Reject packets that are older than 0.5 seconds older than
    // the newest packet we've seen so far. This protect internal
    // states from crazy routers. (Based on RRTR)
    static const int32 kOutOfOrderMaxAgeMs = 500;

    namespace {

        // A receiver frame event is identified by frame RTP timestamp, event timestamp
        // and event type.
        // A receiver packet event is identified by all of the above plus packet id.
        // The key format is as follows:
        // First uint64:
        //   bits 0-11: zeroes (unused).
        //   bits 12-15: event type ID.
        //   bits 16-31: packet ID if packet event, 0 otherwise.
        //   bits 32-63: RTP timestamp.
        // Second uint64:
        //   bits 0-63: event TimeTicks internal value.
        std::pair<uint64, uint64> GetReceiverEventKey(
            uint32 frame_rtp_timestamp,
            const base::TimeTicks& event_timestamp,
            uint8 event_type,
            uint16 packet_id_or_zero)
        {
            uint64 value1 = event_type;
            value1 <<= 16;
            value1 |= packet_id_or_zero;
            value1 <<= 32;
            value1 |= frame_rtp_timestamp;
            return std::make_pair(
                value1, static_cast<uint64>(event_timestamp.ToInternalValue()));
        }

    } // namespace

    Rtcp::Rtcp(const RtcpCastMessageCallback& cast_callback,
        const RtcpRttCallback& rtt_callback,
        const RtcpLogMessageCallback& log_callback,
        base::TickClock* clock,
        PacedPacketSender* packet_sender,
        uint32 local_ssrc,
        uint32 remote_ssrc)
        : cast_callback_(cast_callback)
        , rtt_callback_(rtt_callback)
        , log_callback_(log_callback)
        , clock_(clock)
        , rtcp_builder_(local_ssrc)
        , packet_sender_(packet_sender)
        , local_ssrc_(local_ssrc)
        , remote_ssrc_(remote_ssrc)
        , last_report_truncated_ntp_(0)
        , local_clock_ahead_by_(ClockDriftSmoother::GetDefaultTimeConstant())
        , lip_sync_rtp_timestamp_(0)
        , lip_sync_ntp_timestamp_(0)
        , largest_seen_timestamp_(
              base::TimeTicks::FromInternalValue(kint64min))
    {
    }

    Rtcp::~Rtcp() { }

    bool Rtcp::IsRtcpPacket(const uint8* packet, size_t length)
    {
        if (length < kMinLengthOfRtcp) {
            LOG(ERROR) << "Invalid RTCP packet received.";
            return false;
        }

        uint8 packet_type = packet[1];
        return packet_type >= kPacketTypeLow && packet_type <= kPacketTypeHigh;
    }

    uint32 Rtcp::GetSsrcOfSender(const uint8* rtcp_buffer, size_t length)
    {
        if (length < kMinLengthOfRtcp)
            return 0;
        uint32 ssrc_of_sender;
        base::BigEndianReader big_endian_reader(
            reinterpret_cast<const char*>(rtcp_buffer), length);
        big_endian_reader.Skip(4); // Skip header.
        big_endian_reader.ReadU32(&ssrc_of_sender);
        return ssrc_of_sender;
    }

    bool Rtcp::IncomingRtcpPacket(const uint8* data, size_t length)
    {
        // Check if this is a valid RTCP packet.
        if (!IsRtcpPacket(data, length)) {
            VLOG(1) << "Rtcp@" << this << "::IncomingRtcpPacket() -- "
                    << "Received an invalid (non-RTCP?) packet.";
            return false;
        }

        // Check if this packet is to us.
        uint32 ssrc_of_sender = GetSsrcOfSender(data, length);
        if (ssrc_of_sender != remote_ssrc_) {
            return false;
        }

        // Parse this packet.
        RtcpParser parser(local_ssrc_, remote_ssrc_);
        base::BigEndianReader reader(reinterpret_cast<const char*>(data), length);
        if (parser.Parse(&reader)) {
            if (parser.has_receiver_reference_time_report()) {
                base::TimeTicks t = ConvertNtpToTimeTicks(
                    parser.receiver_reference_time_report().ntp_seconds,
                    parser.receiver_reference_time_report().ntp_fraction);
                if (t > largest_seen_timestamp_) {
                    largest_seen_timestamp_ = t;
                } else if ((largest_seen_timestamp_ - t).InMilliseconds() > kOutOfOrderMaxAgeMs) {
                    // Reject packet, it is too old.
                    VLOG(1) << "Rejecting RTCP packet as it is too old ("
                            << (largest_seen_timestamp_ - t).InMilliseconds()
                            << " ms)";
                    return true;
                }

                OnReceivedNtp(parser.receiver_reference_time_report().ntp_seconds,
                    parser.receiver_reference_time_report().ntp_fraction);
            }
            if (parser.has_sender_report()) {
                OnReceivedNtp(parser.sender_report().ntp_seconds,
                    parser.sender_report().ntp_fraction);
                OnReceivedLipSyncInfo(parser.sender_report().rtp_timestamp,
                    parser.sender_report().ntp_seconds,
                    parser.sender_report().ntp_fraction);
            }
            if (parser.has_receiver_log()) {
                if (DedupeReceiverLog(parser.mutable_receiver_log())) {
                    OnReceivedReceiverLog(parser.receiver_log());
                }
            }
            if (parser.has_last_report()) {
                OnReceivedDelaySinceLastReport(parser.last_report(),
                    parser.delay_since_last_report());
            }
            if (parser.has_cast_message()) {
                parser.mutable_cast_message()->ack_frame_id = ack_frame_id_wrap_helper_.MapTo32bitsFrameId(
                    parser.mutable_cast_message()->ack_frame_id);
                OnReceivedCastFeedback(parser.cast_message());
            }
        }
        return true;
    }

    bool Rtcp::DedupeReceiverLog(RtcpReceiverLogMessage* receiver_log)
    {
        RtcpReceiverLogMessage::iterator i = receiver_log->begin();
        while (i != receiver_log->end()) {
            RtcpReceiverEventLogMessages* messages = &i->event_log_messages_;
            RtcpReceiverEventLogMessages::iterator j = messages->begin();
            while (j != messages->end()) {
                ReceiverEventKey key = GetReceiverEventKey(i->rtp_timestamp_,
                    j->event_timestamp,
                    j->type,
                    j->packet_id);
                RtcpReceiverEventLogMessages::iterator tmp = j;
                ++j;
                if (receiver_event_key_set_.insert(key).second) {
                    receiver_event_key_queue_.push(key);
                    if (receiver_event_key_queue_.size() > kReceiverRtcpEventHistorySize) {
                        receiver_event_key_set_.erase(receiver_event_key_queue_.front());
                        receiver_event_key_queue_.pop();
                    }
                } else {
                    messages->erase(tmp);
                }
            }

            RtcpReceiverLogMessage::iterator tmp = i;
            ++i;
            if (messages->empty()) {
                receiver_log->erase(tmp);
            }
        }
        return !receiver_log->empty();
    }

    RtcpTimeData Rtcp::ConvertToNTPAndSave(base::TimeTicks now)
    {
        RtcpTimeData ret;
        ret.timestamp = now;

        // Attach our NTP to all RTCP packets; with this information a "smart" sender
        // can make decisions based on how old the RTCP message is.
        ConvertTimeTicksToNtp(now, &ret.ntp_seconds, &ret.ntp_fraction);
        SaveLastSentNtpTime(now, ret.ntp_seconds, ret.ntp_fraction);
        return ret;
    }

    void Rtcp::SendRtcpFromRtpReceiver(
        RtcpTimeData time_data,
        const RtcpCastMessage* cast_message,
        base::TimeDelta target_delay,
        const ReceiverRtcpEventSubscriber::RtcpEvents* rtcp_events,
        const RtpReceiverStatistics* rtp_receiver_statistics) const
    {
        RtcpReportBlock report_block;
        RtcpReceiverReferenceTimeReport rrtr;
        rrtr.ntp_seconds = time_data.ntp_seconds;
        rrtr.ntp_fraction = time_data.ntp_fraction;

        if (rtp_receiver_statistics) {
            report_block.remote_ssrc = 0; // Not needed to set send side.
            report_block.media_ssrc = remote_ssrc_; // SSRC of the RTP packet sender.
            report_block.fraction_lost = rtp_receiver_statistics->fraction_lost;
            report_block.cumulative_lost = rtp_receiver_statistics->cumulative_lost;
            report_block.extended_high_sequence_number = rtp_receiver_statistics->extended_high_sequence_number;
            report_block.jitter = rtp_receiver_statistics->jitter;
            report_block.last_sr = last_report_truncated_ntp_;
            if (!time_last_report_received_.is_null()) {
                uint32 delay_seconds = 0;
                uint32 delay_fraction = 0;
                base::TimeDelta delta = time_data.timestamp - time_last_report_received_;
                ConvertTimeToFractions(delta.InMicroseconds(), &delay_seconds,
                    &delay_fraction);
                report_block.delay_since_last_sr = ConvertToNtpDiff(delay_seconds, delay_fraction);
            } else {
                report_block.delay_since_last_sr = 0;
            }
        }
        RtcpBuilder rtcp_builder(local_ssrc_);
        packet_sender_->SendRtcpPacket(
            local_ssrc_,
            rtcp_builder.BuildRtcpFromReceiver(
                rtp_receiver_statistics ? &report_block : NULL,
                &rrtr,
                cast_message,
                rtcp_events,
                target_delay));
    }

    void Rtcp::SendRtcpFromRtpSender(base::TimeTicks current_time,
        uint32 current_time_as_rtp_timestamp,
        uint32 send_packet_count,
        size_t send_octet_count)
    {
        uint32 current_ntp_seconds = 0;
        uint32 current_ntp_fractions = 0;
        ConvertTimeTicksToNtp(current_time, &current_ntp_seconds,
            &current_ntp_fractions);
        SaveLastSentNtpTime(current_time, current_ntp_seconds,
            current_ntp_fractions);

        RtcpSenderInfo sender_info;
        sender_info.ntp_seconds = current_ntp_seconds;
        sender_info.ntp_fraction = current_ntp_fractions;
        sender_info.rtp_timestamp = current_time_as_rtp_timestamp;
        sender_info.send_packet_count = send_packet_count;
        sender_info.send_octet_count = send_octet_count;

        packet_sender_->SendRtcpPacket(
            local_ssrc_,
            rtcp_builder_.BuildRtcpFromSender(sender_info));
    }

    void Rtcp::OnReceivedNtp(uint32 ntp_seconds, uint32 ntp_fraction)
    {
        last_report_truncated_ntp_ = ConvertToNtpDiff(ntp_seconds, ntp_fraction);

        const base::TimeTicks now = clock_->NowTicks();
        time_last_report_received_ = now;

        // TODO(miu): This clock offset calculation does not account for packet
        // transit time over the network.  End2EndTest.EvilNetwork confirms that this
        // contributes a very significant source of error here.  Determine whether
        // RTT should be factored-in, and how that changes the rest of the
        // calculation.
        const base::TimeDelta measured_offset = now - ConvertNtpToTimeTicks(ntp_seconds, ntp_fraction);
        local_clock_ahead_by_.Update(now, measured_offset);
        if (measured_offset < local_clock_ahead_by_.Current()) {
            // Logically, the minimum offset between the clocks has to be the correct
            // one.  For example, the time it took to transmit the current report may
            // have been lower than usual, and so some of the error introduced by the
            // transmission time can be eliminated.
            local_clock_ahead_by_.Reset(now, measured_offset);
        }
        VLOG(1) << "Local clock is ahead of the remote clock by: "
                << "measured=" << measured_offset.InMicroseconds() << " usec, "
                << "filtered=" << local_clock_ahead_by_.Current().InMicroseconds()
                << " usec.";
    }

    void Rtcp::OnReceivedLipSyncInfo(uint32 rtp_timestamp, uint32 ntp_seconds,
        uint32 ntp_fraction)
    {
        if (ntp_seconds == 0) {
            NOTREACHED();
            return;
        }
        lip_sync_rtp_timestamp_ = rtp_timestamp;
        lip_sync_ntp_timestamp_ = (static_cast<uint64>(ntp_seconds) << 32) | ntp_fraction;
    }

    bool Rtcp::GetLatestLipSyncTimes(uint32* rtp_timestamp,
        base::TimeTicks* reference_time) const
    {
        if (!lip_sync_ntp_timestamp_)
            return false;

        const base::TimeTicks local_reference_time = ConvertNtpToTimeTicks(static_cast<uint32>(lip_sync_ntp_timestamp_ >> 32),
                                                         static_cast<uint32>(lip_sync_ntp_timestamp_))
            + local_clock_ahead_by_.Current();

        // Sanity-check: Getting regular lip sync updates?
        DCHECK((clock_->NowTicks() - local_reference_time) < base::TimeDelta::FromMinutes(1));

        *rtp_timestamp = lip_sync_rtp_timestamp_;
        *reference_time = local_reference_time;
        return true;
    }

    void Rtcp::OnReceivedDelaySinceLastReport(uint32 last_report,
        uint32 delay_since_last_report)
    {
        RtcpSendTimeMap::iterator it = last_reports_sent_map_.find(last_report);
        if (it == last_reports_sent_map_.end()) {
            return; // Feedback on another report.
        }

        const base::TimeDelta sender_delay = clock_->NowTicks() - it->second;
        const base::TimeDelta receiver_delay = ConvertFromNtpDiff(delay_since_last_report);
        current_round_trip_time_ = sender_delay - receiver_delay;
        // If the round trip time was computed as less than 1 ms, assume clock
        // imprecision by one or both peers caused a bad value to be calculated.
        // While plenty of networks do easily achieve less than 1 ms round trip time,
        // such a level of precision cannot be measured with our approach; and 1 ms is
        // good enough to represent "under 1 ms" for our use cases.
        current_round_trip_time_ = std::max(current_round_trip_time_, base::TimeDelta::FromMilliseconds(1));

        if (!rtt_callback_.is_null())
            rtt_callback_.Run(current_round_trip_time_);
    }

    void Rtcp::OnReceivedCastFeedback(const RtcpCastMessage& cast_message)
    {
        if (cast_callback_.is_null())
            return;
        cast_callback_.Run(cast_message);
    }

    void Rtcp::SaveLastSentNtpTime(const base::TimeTicks& now,
        uint32 last_ntp_seconds,
        uint32 last_ntp_fraction)
    {
        // Make sure |now| is always greater than the last element in
        // |last_reports_sent_queue_|.
        if (!last_reports_sent_queue_.empty()) {
            DCHECK(now >= last_reports_sent_queue_.back().second);
        }

        uint32 last_report = ConvertToNtpDiff(last_ntp_seconds, last_ntp_fraction);
        last_reports_sent_map_[last_report] = now;
        last_reports_sent_queue_.push(std::make_pair(last_report, now));

        const base::TimeTicks timeout = now - TimeDelta::FromMilliseconds(kStatsHistoryWindowMs);

        // Cleanup old statistics older than |timeout|.
        while (!last_reports_sent_queue_.empty()) {
            RtcpSendTimePair oldest_report = last_reports_sent_queue_.front();
            if (oldest_report.second < timeout) {
                last_reports_sent_map_.erase(oldest_report.first);
                last_reports_sent_queue_.pop();
            } else {
                break;
            }
        }
    }

    void Rtcp::OnReceivedReceiverLog(const RtcpReceiverLogMessage& receiver_log)
    {
        if (log_callback_.is_null())
            return;
        log_callback_.Run(receiver_log);
    }

} // namespace cast
} // namespace media
