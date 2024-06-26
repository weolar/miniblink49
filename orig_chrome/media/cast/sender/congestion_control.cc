// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// The purpose of this file is determine what bitrate to use for mirroring.
// Ideally this should be as much as possible, without causing any frames to
// arrive late.

// The current algorithm is to measure how much bandwidth we've been using
// recently. We also keep track of how much data has been queued up for sending
// in a virtual "buffer" (this virtual buffer represents all the buffers between
// the sender and the receiver, including retransmissions and so forth.)
// If we estimate that our virtual buffer is mostly empty, we try to use
// more bandwidth than our recent usage, otherwise we use less.

#include "media/cast/sender/congestion_control.h"

#include <deque>

#include "base/logging.h"
#include "base/trace_event/trace_event.h"
#include "media/cast/cast_config.h"
#include "media/cast/cast_defines.h"

namespace media {
namespace cast {

    class AdaptiveCongestionControl : public CongestionControl {
    public:
        AdaptiveCongestionControl(base::TickClock* clock,
            int max_bitrate_configured,
            int min_bitrate_configured,
            double max_frame_rate);

        ~AdaptiveCongestionControl() final;

        // CongestionControl implementation.
        void UpdateRtt(base::TimeDelta rtt) final;
        void UpdateTargetPlayoutDelay(base::TimeDelta delay) final;
        void SendFrameToTransport(uint32 frame_id,
            size_t frame_size_in_bits,
            base::TimeTicks when) final;
        void AckFrame(uint32 frame_id, base::TimeTicks when) final;
        int GetBitrate(base::TimeTicks playout_time,
            base::TimeDelta playout_delay,
            int soft_max_bitrate) final;

    private:
        struct FrameStats {
            FrameStats();
            // Time this frame was first enqueued for transport.
            base::TimeTicks enqueue_time;
            // Time this frame was acked.
            base::TimeTicks ack_time;
            // Size of encoded frame in bits.
            size_t frame_size_in_bits;
        };

        // Calculate how much "dead air" (idle time) there is between two frames.
        static base::TimeDelta DeadTime(const FrameStats& a, const FrameStats& b);
        // Get the FrameStats for a given |frame_id|.  Never returns nullptr.
        // Note: Older FrameStats will be removed automatically.
        FrameStats* GetFrameStats(uint32 frame_id);
        // Discard old FrameStats.
        void PruneFrameStats();
        // Calculate a safe bitrate. This is based on how much we've been
        // sending in the past.
        double CalculateSafeBitrate();

        // Estimate when the transport will start sending the data for a given frame.
        // |estimated_bitrate| is the current estimated transmit bitrate in bits per
        // second.
        base::TimeTicks EstimatedSendingTime(uint32 frame_id,
            double estimated_bitrate);

        base::TickClock* const clock_; // Not owned by this class.
        const int max_bitrate_configured_;
        const int min_bitrate_configured_;
        const double max_frame_rate_;
        std::deque<FrameStats> frame_stats_;
        uint32 last_frame_stats_;
        uint32 last_acked_frame_;
        uint32 last_enqueued_frame_;
        base::TimeDelta rtt_;
        size_t history_size_;
        size_t acked_bits_in_history_;
        base::TimeDelta dead_time_in_history_;

        DISALLOW_COPY_AND_ASSIGN(AdaptiveCongestionControl);
    };

    class FixedCongestionControl : public CongestionControl {
    public:
        explicit FixedCongestionControl(int bitrate)
            : bitrate_(bitrate)
        {
        }
        ~FixedCongestionControl() final { }

        // CongestionControl implementation.
        void UpdateRtt(base::TimeDelta rtt) final { }
        void UpdateTargetPlayoutDelay(base::TimeDelta delay) final { }
        void SendFrameToTransport(uint32 frame_id,
            size_t frame_size_in_bits,
            base::TimeTicks when) final { }
        void AckFrame(uint32 frame_id, base::TimeTicks when) final { }
        int GetBitrate(base::TimeTicks playout_time,
            base::TimeDelta playout_delay,
            int soft_max_bitrate) final
        {
            return bitrate_;
        }

    private:
        const int bitrate_;

        DISALLOW_COPY_AND_ASSIGN(FixedCongestionControl);
    };

    CongestionControl* NewAdaptiveCongestionControl(
        base::TickClock* clock,
        int max_bitrate_configured,
        int min_bitrate_configured,
        double max_frame_rate)
    {
        return new AdaptiveCongestionControl(clock,
            max_bitrate_configured,
            min_bitrate_configured,
            max_frame_rate);
    }

    CongestionControl* NewFixedCongestionControl(int bitrate)
    {
        return new FixedCongestionControl(bitrate);
    }

    // This means that we *try* to keep our buffer 90% empty.
    // If it is less full, we increase the bandwidth, if it is more
    // we decrease the bandwidth. Making this smaller makes the
    // congestion control more aggressive.
    static const double kTargetEmptyBufferFraction = 0.9;

    // This is the size of our history in frames. Larger values makes the
    // congestion control adapt slower.
    static const size_t kHistorySize = 100;

    AdaptiveCongestionControl::FrameStats::FrameStats()
        : frame_size_in_bits(0)
    {
    }

    AdaptiveCongestionControl::AdaptiveCongestionControl(
        base::TickClock* clock,
        int max_bitrate_configured,
        int min_bitrate_configured,
        double max_frame_rate)
        : clock_(clock)
        , max_bitrate_configured_(max_bitrate_configured)
        , min_bitrate_configured_(min_bitrate_configured)
        , max_frame_rate_(max_frame_rate)
        , last_frame_stats_(static_cast<uint32>(-1))
        , last_acked_frame_(static_cast<uint32>(-1))
        , last_enqueued_frame_(static_cast<uint32>(-1))
        , history_size_(kHistorySize)
        , acked_bits_in_history_(0)
    {
        DCHECK_GE(max_bitrate_configured, min_bitrate_configured) << "Invalid config";
        DCHECK_GT(min_bitrate_configured, 0);
        frame_stats_.resize(2);
        base::TimeTicks now = clock->NowTicks();
        frame_stats_[0].ack_time = now;
        frame_stats_[0].enqueue_time = now;
        frame_stats_[1].ack_time = now;
        DCHECK(!frame_stats_[0].ack_time.is_null());
    }

    CongestionControl::~CongestionControl() { }
    AdaptiveCongestionControl::~AdaptiveCongestionControl() { }

    void AdaptiveCongestionControl::UpdateRtt(base::TimeDelta rtt)
    {
        rtt_ = (7 * rtt_ + rtt) / 8;
    }

    void AdaptiveCongestionControl::UpdateTargetPlayoutDelay(
        base::TimeDelta delay)
    {
        const int max_unacked_frames = std::min(kMaxUnackedFrames,
            1 + static_cast<int>(delay * max_frame_rate_ / base::TimeDelta::FromSeconds(1)));
        DCHECK_GT(max_unacked_frames, 0);
        history_size_ = max_unacked_frames + kHistorySize;
        PruneFrameStats();
    }

    // Calculate how much "dead air" there is between two frames.
    base::TimeDelta AdaptiveCongestionControl::DeadTime(const FrameStats& a,
        const FrameStats& b)
    {
        if (b.enqueue_time > a.ack_time) {
            return b.enqueue_time - a.ack_time;
        } else {
            return base::TimeDelta();
        }
    }

    double AdaptiveCongestionControl::CalculateSafeBitrate()
    {
        double transmit_time = (GetFrameStats(last_acked_frame_)->ack_time - frame_stats_.front().enqueue_time - dead_time_in_history_).InSecondsF();

        if (acked_bits_in_history_ == 0 || transmit_time <= 0.0) {
            return min_bitrate_configured_;
        }
        return acked_bits_in_history_ / std::max(transmit_time, 1E-3);
    }

    AdaptiveCongestionControl::FrameStats*
    AdaptiveCongestionControl::GetFrameStats(uint32 frame_id)
    {
        int32 offset = static_cast<int32>(frame_id - last_frame_stats_);
        DCHECK_LT(offset, static_cast<int32>(kHistorySize));
        if (offset > 0) {
            frame_stats_.resize(frame_stats_.size() + offset);
            last_frame_stats_ += offset;
            offset = 0;
        }
        PruneFrameStats();
        offset += frame_stats_.size() - 1;
        // TODO(miu): Change the following to DCHECK once crash fix is confirmed.
        // http://crbug.com/517145
        CHECK(offset >= 0 && offset < static_cast<int32>(frame_stats_.size()));
        return &frame_stats_[offset];
    }

    void AdaptiveCongestionControl::PruneFrameStats()
    {
        while (frame_stats_.size() > history_size_) {
            DCHECK_GT(frame_stats_.size(), 1UL);
            DCHECK(!frame_stats_[0].ack_time.is_null());
            acked_bits_in_history_ -= frame_stats_[0].frame_size_in_bits;
            dead_time_in_history_ -= DeadTime(frame_stats_[0], frame_stats_[1]);
            DCHECK_GE(acked_bits_in_history_, 0UL);
            VLOG(2) << "DT: " << dead_time_in_history_.InSecondsF();
            DCHECK_GE(dead_time_in_history_.InSecondsF(), 0.0);
            frame_stats_.pop_front();
        }
    }

    void AdaptiveCongestionControl::AckFrame(uint32 frame_id,
        base::TimeTicks when)
    {
        FrameStats* frame_stats = GetFrameStats(last_acked_frame_);
        while (IsNewerFrameId(frame_id, last_acked_frame_)) {
            FrameStats* last_frame_stats = frame_stats;
            frame_stats = GetFrameStats(last_acked_frame_ + 1);
            if (frame_stats->enqueue_time.is_null()) {
                // Can't ack a frame that hasn't been sent yet.
                return;
            }
            last_acked_frame_++;
            if (when < frame_stats->enqueue_time)
                when = frame_stats->enqueue_time;

            frame_stats->ack_time = when;
            acked_bits_in_history_ += frame_stats->frame_size_in_bits;
            dead_time_in_history_ += DeadTime(*last_frame_stats, *frame_stats);
        }
    }

    void AdaptiveCongestionControl::SendFrameToTransport(uint32 frame_id,
        size_t frame_size_in_bits,
        base::TimeTicks when)
    {
        last_enqueued_frame_ = frame_id;
        FrameStats* frame_stats = GetFrameStats(frame_id);
        frame_stats->enqueue_time = when;
        frame_stats->frame_size_in_bits = frame_size_in_bits;
    }

    base::TimeTicks AdaptiveCongestionControl::EstimatedSendingTime(
        uint32 frame_id,
        double estimated_bitrate)
    {
        const base::TimeTicks now = clock_->NowTicks();

        // Starting with the time of the latest acknowledgement, extrapolate forward
        // to determine an estimated sending time for |frame_id|.
        //
        // |estimated_sending_time| will contain the estimated sending time for each
        // frame after the last ACK'ed frame.  It is possible for multiple frames to
        // be in-flight; and therefore it is common for the |estimated_sending_time|
        // for those frames to be before |now|.
        base::TimeTicks estimated_sending_time;
        for (uint32 f = last_acked_frame_; IsNewerFrameId(frame_id, f); ++f) {
            FrameStats* const stats = GetFrameStats(f);

            // |estimated_ack_time| is the local time when the sender receives the ACK,
            // and not the time when the ACK left the receiver.
            base::TimeTicks estimated_ack_time = stats->ack_time;

            // If |estimated_ack_time| is not null, then we already have the actual ACK
            // time, so we'll just use it.  Otherwise, we need to estimate when the ACK
            // will arrive.
            if (estimated_ack_time.is_null()) {
                // Model: The |estimated_sending_time| is the time at which the first byte
                // of the encoded frame is transmitted.  Then, assume the transmission of
                // the remaining bytes is paced such that the last byte has just left the
                // sender at |frame_transmit_time| later.  This last byte then takes
                // ~RTT/2 amount of time to travel to the receiver.  Finally, the ACK from
                // the receiver is sent and this takes another ~RTT/2 amount of time to
                // reach the sender.
                const base::TimeDelta frame_transmit_time = base::TimeDelta::FromSecondsD(stats->frame_size_in_bits / estimated_bitrate);
                estimated_ack_time = std::max(estimated_sending_time, stats->enqueue_time) + frame_transmit_time + rtt_;

                if (estimated_ack_time < now) {
                    // The current frame has not yet been ACK'ed and the yet the computed
                    // |estimated_ack_time| is before |now|.  This contradiction must be
                    // resolved.
                    //
                    // The solution below is a little counter-intuitive, but it seems to
                    // work.  Basically, when we estimate that the ACK should have already
                    // happened, we figure out how long ago it should have happened and
                    // guess that the ACK will happen half of that time in the future.  This
                    // will cause some over-estimation when acks are late, which is actually
                    // the desired behavior.
                    estimated_ack_time = now + (now - estimated_ack_time) / 2;
                }
            }

            // Since we [in the common case] do not wait for an ACK before we start
            // sending the next frame, estimate the next frame's sending time as the
            // time just after the last byte of the current frame left the sender (see
            // Model comment above).
            estimated_sending_time = std::max(estimated_sending_time, estimated_ack_time - rtt_);
        }

        FrameStats* const frame_stats = GetFrameStats(frame_id);
        if (frame_stats->enqueue_time.is_null()) {
            // The frame has not yet been enqueued for transport.  Since it cannot be
            // enqueued in the past, ensure the result is lower-bounded by |now|.
            estimated_sending_time = std::max(estimated_sending_time, now);
        } else {
            // |frame_stats->enqueue_time| is the time the frame was enqueued for
            // transport.  The frame may not actually start being sent until a
            // point-in-time after that, because the transport is waiting for prior
            // frames to be acknowledged.
            estimated_sending_time = std::max(estimated_sending_time, frame_stats->enqueue_time);
        }

        return estimated_sending_time;
    }

    int AdaptiveCongestionControl::GetBitrate(base::TimeTicks playout_time,
        base::TimeDelta playout_delay,
        int soft_max_bitrate)
    {
        double safe_bitrate = CalculateSafeBitrate();
        // Estimate when we might start sending the next frame.
        base::TimeDelta time_to_catch_up = playout_time - EstimatedSendingTime(last_enqueued_frame_ + 1, safe_bitrate);

        double empty_buffer_fraction = time_to_catch_up.InSecondsF() / playout_delay.InSecondsF();
        empty_buffer_fraction = std::min(empty_buffer_fraction, 1.0);
        empty_buffer_fraction = std::max(empty_buffer_fraction, 0.0);

        int bits_per_second = static_cast<int>(
            safe_bitrate * empty_buffer_fraction / kTargetEmptyBufferFraction);
        VLOG(3) << " FBR:" << (bits_per_second / 1E6)
                << " EBF:" << empty_buffer_fraction
                << " SBR:" << (safe_bitrate / 1E6);
        TRACE_COUNTER_ID1("cast.stream", "Empty Buffer Fraction", this,
            empty_buffer_fraction);
        bits_per_second = std::min(bits_per_second, soft_max_bitrate);
        bits_per_second = std::max(bits_per_second, min_bitrate_configured_);
        bits_per_second = std::min(bits_per_second, max_bitrate_configured_);

        return bits_per_second;
    }

} // namespace cast
} // namespace media
